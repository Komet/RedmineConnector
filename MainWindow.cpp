#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <coreplugin/icore.h>
#include <QDebug>
#include <QSettings>
#include <QMovie>

#include "RedmineConnectorConstants.h"
#include "TableItemIssue.h"
#include "TableItemProject.h"

using namespace RedmineConnector;

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->buttonReloadProjects, SIGNAL(clicked(bool)), ui->buttonReloadProjects, SLOT(setEnabled(bool)));
    connect(ui->buttonReloadProjects, SIGNAL(clicked()), this, SLOT(reload()));
    connect(ui->splitter, SIGNAL(splitterMoved(int,int)), this, SLOT(saveSplitterSizes()));
    connect(ui->dateDueTo, SIGNAL(dateChanged(QDate)), this, SLOT(setCurrentIssueHasDueDate()));
    connect(ui->dateStarted, SIGNAL(dateChanged(QDate)), this, SLOT(setCurrentIssueHasStartDate()));
    connect(ui->buttonSave, SIGNAL(clicked()), this, SLOT(saveIssueChanges()));
    connect(ui->tableIssues, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(issueListCellChanged(int,int,int,int)));
    connect(ui->tableProjects, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(projectListCellChanged(int,int,int,int)));
    connect(Manager::instance(), SIGNAL(ready()), this, SLOT(repositoriesReady()));

    ui->tableIssues->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    ui->tableProjects->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    ui->splitter->restoreState(Settings::instance()->mainWindowSplitterSizes());

    QFont font = ui->subject->font();
    font.setPointSize(font.pointSize()+6);
    ui->subject->setFont(font);
    ui->labelIssues->setFont(font);
    ui->labelProjects->setFont(font);

    QMovie *movie = new QMovie(":/img/ajax-loader-1.gif");
    ui->labelLoading->setMovie(movie);
    movie->start();

    this->m_currentIssue = 0;

    ui->buttonReloadProjects->click();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::saveSplitterSizes()
{
    Settings::instance()->setMainWindowSplitterSizes(ui->splitter->saveState());
}

void MainWindow::reload()
{
    this->m_currentIssue = 0;
    ui->stackedWidget->setCurrentIndex(0);
    ui->tableIssues->clearContents();
    ui->tableIssues->setRowCount(0);
    ui->tableProjects->clearContents();
    ui->tableProjects->setRowCount(0);
    Manager::instance()->initialize();
    ui->buttonReloadProjects->setText(tr("Refreshing..."));
}


void MainWindow::repositoriesReady()
{
    ui->tableProjects->clearContents();
    ui->tableProjects->setRowCount(0);
    foreach( Repository *repository, Manager::instance()->repositories() ) {
        foreach( Project *project, repository->projects() ) {
            TableItemProject *item = new TableItemProject(ui->tableProjects);
            item->setProject(project);

            int row = ui->tableProjects->rowCount();
            ui->tableProjects->insertRow(row);
            ui->tableProjects->setCellWidget(row, 0, item);
        }
    }

    int height = 0;
    for( int i=0, n=ui->tableProjects->rowCount() ; i<n ; i++ ) {
        height += ui->tableProjects->rowHeight(i);
    }
    ui->tableProjects->setFixedHeight(height+22);

    ui->buttonReloadProjects->setText(tr("Refresh"));
    ui->buttonReloadProjects->setEnabled(true);
}

void MainWindow::projectListCellChanged(int row, int column, int previousRow, int previousColumn)
{
    Q_UNUSED(column);
    Q_UNUSED(previousColumn);

    ui->stackedWidget->setCurrentIndex(0);

    if( previousRow > -1 && previousRow < ui->tableProjects->rowCount() ) {
        static_cast<TableItemProject*>(ui->tableProjects->cellWidget(previousRow, 0))->setIsCurrentItem(false);
    }
    if( row > -1 && row < ui->tableProjects->rowCount() ) {
        static_cast<TableItemProject*>(ui->tableProjects->cellWidget(row, 0))->setIsCurrentItem(true);
        ui->tableIssues->clearContents();
        ui->tableIssues->setRowCount(0);
        foreach( Issue *issue, static_cast<TableItemProject*>(ui->tableProjects->cellWidget(row, 0))->project()->issues() ) {
            TableItemIssue *issueItem = new TableItemIssue(ui->tableIssues);
            issueItem->setIssue(issue);
            int row = ui->tableIssues->rowCount();
            ui->tableIssues->insertRow(row);
            ui->tableIssues->setCellWidget(row, 0, issueItem);
        }
    }
}

void MainWindow::issueListCellChanged(int row, int column, int previousRow, int previousColumn)
{
    Q_UNUSED(column);
    Q_UNUSED(previousColumn);
    if( previousRow > -1 && previousRow < ui->tableIssues->rowCount() ) {
        static_cast<TableItemIssue*>(ui->tableIssues->cellWidget(previousRow, 0))->setIsCurrentItem(false);
    }
    if( row > -1 && row < ui->tableIssues->rowCount() ) {
        static_cast<TableItemIssue*>(ui->tableIssues->cellWidget(row, 0))->setIsCurrentItem(true);
        this->loadIssue(static_cast<TableItemIssue*>(ui->tableIssues->cellWidget(row, 0))->issue());
    }
}

void MainWindow::loadIssue(RedmineConnector::Issue *issue)
{
    ui->stackedWidget->setCurrentIndex(1);
    if( this->m_currentIssue != 0) {
        disconnect(this->m_currentIssue, 0, this, 0);
    }
    connect(issue, SIGNAL(ready(RedmineConnector::Issue*)), this, SLOT(showIssue(RedmineConnector::Issue*)));
    issue->load();
}

void MainWindow::showIssue(Issue *issue)
{
    this->m_currentIssue = issue;

    ui->subject->setText(QString("#%1 %2").arg(issue->id()).arg(issue->subject()));
    ui->dateDueTo->setDate((issue->dueDate().isValid()) ? issue->dueDate() : QDate::currentDate());
    this->m_currentIssueHasDueDate = issue->dueDate().isValid();
    ui->dateStarted->setDate((issue->startDate().isValid()) ? issue->startDate() : QDate::currentDate());
    this->m_currentIssueHasStartDate = issue->startDate().isValid();
    ui->comboDoneRatio->setCurrentIndex(issue->doneRatio()/10);

    for( int i=0, n=this->m_notes.size() ; i<n ; i++ ) {
        ui->verticalLayoutDetails->removeWidget(this->m_notes.at(i));
        delete this->m_notes.at(i);
    }
    this->m_notes.clear();

    QList<QString> emptyList;
    ui->issueDescription->setIsTopElement(true);
    ui->issueDescription->setNotes(issue->description(), emptyList);

    QList<IssueDetail> issueDetails = issue->issueDetails();
    foreach( IssueDetail detail, issueDetails ) {
        IssueDetailWidget *issueDetailWidget = new IssueDetailWidget(this);
        issueDetailWidget->setAuthor(detail.user->firstName() + " " + detail.user->lastName());
        issueDetailWidget->setDateTime(detail.createdOn);
        issueDetailWidget->setNotes(detail.notes, detail.details);
        issueDetailWidget->setIsTopElement(false);
        ui->verticalLayoutDetails->addWidget(issueDetailWidget);
        this->m_notes.append(issueDetailWidget);
    }

    // Description
    ui->comment->setText("");

    // User
    ui->comboAssignedTo->clear();
    QList<User*> users = issue->project()->repository()->users();
    for( int i=0, n=users.size() ; i<n ; i++ ) {
        ui->comboAssignedTo->addItem(QString("%1 %2").arg(users.at(i)->firstName()).arg(users.at(i)->lastName()), users.at(i)->id());
        if( users.at(i)->id() == issue->assignedTo()->id() ) {
            ui->comboAssignedTo->setCurrentIndex(i);
        }
    }

    // Category
    ui->comboCategory->clear();
    QList<IssueCategory*> categories = issue->project()->issueCategories();
    for( int i=0, n=categories.size() ; i<n ; i++ ) {
        ui->comboCategory->addItem(categories.at(i)->name(), categories.at(i)->id());
        if( categories.at(i)->id() == issue->issueCategory()->id() ) {
            ui->comboCategory->setCurrentIndex(i);
        }
    }

    // Status
    ui->comboStatus->clear();
    QList<IssueStatus*> status = issue->project()->repository()->issueStatuses();
    for( int i=0, n=status.size() ; i<n ; i++ ) {
        ui->comboStatus->addItem(status.at(i)->name(), status.at(i)->id());
        if( status.at(i)->id() == issue->issueStatus()->id() ) {
            ui->comboStatus->setCurrentIndex(i);
        }
    }

    // Priority
    ui->comboPriority->clear();
    QList<Priority*> priorities = issue->project()->repository()->priorities();
    for( int i=0, n=priorities.size() ; i<n ; i++ ) {
        ui->comboPriority->addItem(priorities.at(i)->name(), priorities.at(i)->id());
        if( priorities.at(i)->id() == issue->priority()->id() ) {
            ui->comboPriority->setCurrentIndex(i);
        }
    }

    // Tracker
    ui->comboTracker->clear();
    QList<Tracker> trackers = issue->project()->trackers();
    for( int i=0, n=trackers.size() ; i<n ; i++ ) {
        ui->comboTracker->addItem(trackers[i].name, trackers[i].id);
        if( trackers[i].id == issue->tracker().id ) {
            ui->comboTracker->setCurrentIndex(i);
        }
    }

    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::setCurrentIssueHasDueDate()
{
    this->m_currentIssueHasDueDate = true;
}

void MainWindow::setCurrentIssueHasStartDate()
{
    this->m_currentIssueHasStartDate = true;
}

void MainWindow::saveIssueChanges()
{
    QString description = ui->comment->toPlainText();
    QDate startDate = ui->dateStarted->date();
    if( !this->m_currentIssueHasDueDate ) {
        // set an invalid date
        startDate.setDate(2000, 2, 30);
    }
    QDate dueDate = ui->dateDueTo->date();
    if( !this->m_currentIssueHasDueDate ) {
        // set an invalid date
        dueDate.setDate(2000, 2, 30);
    }
    int doneRatio = ui->comboDoneRatio->currentIndex()*10;
    IssueStatus *status = this->m_currentIssue->project()->repository()->issueStatus(ui->comboStatus->itemData(ui->comboStatus->currentIndex()).toInt());
    Priority *priority = this->m_currentIssue->project()->repository()->getAndAddPriority(ui->comboStatus->itemData(ui->comboPriority->currentIndex()).toInt(), "");
    User *assignedTo = this->m_currentIssue->project()->repository()->user(ui->comboAssignedTo->itemData(ui->comboAssignedTo->currentIndex()).toInt());
    Tracker tracker = this->m_currentIssue->project()->tracker(ui->comboTracker->itemData(ui->comboTracker->currentIndex()).toInt());
    IssueCategory *category = this->m_currentIssue->project()->issueCategory(ui->comboCategory->currentIndex());
    ui->stackedWidget->setCurrentIndex(1);
    disconnect(this->m_currentIssue, 0, this, 0);
    connect(this->m_currentIssue, SIGNAL(ready(RedmineConnector::Issue*)), this, SLOT(loadIssue(RedmineConnector::Issue*)));
    this->m_currentIssue->setChangeset(status, priority, assignedTo, tracker, category, startDate, dueDate, doneRatio, description);
}
