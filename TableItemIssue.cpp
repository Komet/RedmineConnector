#include "TableItemIssue.h"
#include "ui_TableItemIssue.h"

TableItemIssue::TableItemIssue(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TableItemIssue)
{
    ui->setupUi(this);

    ui->subject->setText("");
    ui->status->setText("");
    ui->assignedTo->setText("");
    ui->updated->setText("");
    ui->due->setText("");

    QFont font = ui->assignedTo->font();
    font.setPointSize(font.pointSize()-2);
    ui->assignedTo->setFont(font);
    ui->updated->setFont(font);
    ui->due->setFont(font);
    font.setBold(true);
    ui->issueId->setFont(font);
}

TableItemIssue::~TableItemIssue()
{
    delete ui;
}

void TableItemIssue::setIsCurrentItem(bool isCurrent)
{
    QFont font = ui->subject->font();
    font.setBold(isCurrent);
    ui->subject->setFont(font);
    if( isCurrent ) {
        ui->tableWidgetIssue->setStyleSheet("background-color: rgba(200, 220, 255);");
    } else {
        ui->tableWidgetIssue->setStyleSheet("");
    }
}

void TableItemIssue::setIssue(RedmineConnector::Issue *issue)
{
    this->m_issue = issue;

    ui->issueId->setText(QString("#%1").arg(issue->id()));
    ui->subject->setText(issue->subject());
    ui->status->setText(issue->issueStatus()->name());
    ui->assignedTo->setText(QString("%1 %2").arg(issue->assignedTo()->firstName()).arg(issue->assignedTo()->lastName()).trimmed());
    ui->updated->setText(issue->updatedOn().toString("dd.MM.yyyy"));

    if( !issue->dueDate().isValid() ) {
        ui->due->setText("");
        return;
    }
    int daysTo = QDate::currentDate().daysTo(issue->dueDate());
    if( daysTo == 0 ) {
        ui->due->setText(tr("heute"));
    } else {
        ui->due->setText(tr("in %n Tagen", "", daysTo));
    }

    if( daysTo > 7 ) {
        ui->due->setStyleSheet("color: rgb(170, 170, 170);");
    } else if( daysTo > 3 ) {
        ui->due->setStyleSheet("color: rgb(40, 158, 65);");
    } else if( daysTo > 1 ) {
        ui->due->setStyleSheet("color: rgb(255, 160, 35);");
    } else {
        ui->due->setStyleSheet("color: rgb(255, 0, 0);");
    }
}

RedmineConnector::Issue* TableItemIssue::issue()
{
    return this->m_issue;
}
