#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <coreplugin/icore.h>
#include <QSettings>
#include "RedmineConnectorConstants.h"
#include "TableItemIssue.h"
#include "TableWidgetItemDelegate.h"

using namespace RedmineConnector;

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->buttonReloadProjects, SIGNAL(clicked(bool)), ui->buttonReloadProjects, SLOT(setEnabled(bool)));
    connect(ui->buttonReloadProjects, SIGNAL(clicked()), this, SLOT(reload()));
    connect(Manager::instance(), SIGNAL(ready()), this, SLOT(repositoriesReady()));
    connect(ui->comboProjects, SIGNAL(currentIndexChanged(int)), this, SLOT(repositoryChanged(int)));
    connect(ui->splitter, SIGNAL(splitterMoved(int,int)), this, SLOT(saveSplitterSizes()));

    ui->tableIssues->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    ui->tableIssues->setItemDelegate(new TableWidgetItemDelegate(ui->tableIssues));

    ui->splitter->restoreState(Settings::instance()->mainWindowSplitterSizes());

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
    ui->comboProjects->clear();
    ui->tableIssues->clearContents();
    ui->tableIssues->setRowCount(0);
    Manager::instance()->initialize();
}


void MainWindow::repositoriesReady()
{
    ui->buttonReloadProjects->setEnabled(true);
    QList<Repository*> repositories = Manager::instance()->repositories();
    for( int i=0 ; i<repositories.size() ; i++ ) {
        QString prefix = (repositories.size() > 1) ? repositories[i]->name() + "/" : "";
        QList<Project*> projects = repositories[i]->projects();
        for( int n=0 ; n<projects.size() ; n++ ) {
            ui->comboProjects->addItem(prefix + projects[n]->name());
            ui->comboProjects->setItemData(ui->comboProjects->count()-1, i, Qt::UserRole);
            ui->comboProjects->setItemData(ui->comboProjects->count()-1, n, Qt::UserRole+1);
        }
    }
}


void MainWindow::repositoryChanged(int index)
{
    if( index == -1 ) {
        return;
    }
    int repository = ui->comboProjects->itemData(index, Qt::UserRole).toInt();
    int project = ui->comboProjects->itemData(index, Qt::UserRole+1).toInt();

    ui->tableIssues->clearContents();
    ui->tableIssues->setRowCount(0);

    QList<Issue*> issues = Manager::instance()->repositories().at(repository)->projects().at(project)->issues();
    for( int i=0 ; i<issues.size() ; i++ ) {
        TableItemIssue *issue = new TableItemIssue(ui->tableIssues);
        issue->setSubject(issues[i]->subject());
        issue->setStatus(issues[i]->issueStatus()->name());
        issue->setAssignedTo(issues[i]->assignedTo()->firstName() + " " + issues[i]->assignedTo()->lastName());
        issue->setUpdated(issues[i]->updatedOn().toString("dd.MM.yyyy"));
        issue->setIssueId(issues[i]->id());

        int row = ui->tableIssues->rowCount();
        ui->tableIssues->insertRow(row);
        ui->tableIssues->setCellWidget(row, 0, issue);
    }
}
