#include "TableItemIssue.h"
#include "ui_TableItemIssue.h"

#include <QDebug>

TableItemIssue::TableItemIssue(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TableItemIssue)
{
    ui->setupUi(this);

    ui->subject->setText(QString());
    ui->status->setText(QString());
    ui->assignedTo->setText(QString());
    ui->updated->setText(QString());
    ui->due->setText(QString());

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
        ui->tableWidgetIssue->setStyleSheet(QLatin1String("background-color: rgba(200, 220, 255);"));
    } else {
        ui->tableWidgetIssue->setStyleSheet(QString());
    }
}

void TableItemIssue::setIssue(RedmineConnector::Issue *issue)
{
    this->m_issue = issue;

    ui->issueId->setText(QString::fromLatin1("#%1").arg(issue->id()));
    ui->subject->setText(issue->subject());
    ui->status->setText(issue->issueStatus()->name());
    ui->assignedTo->setText(QString::fromLatin1("%1 %2").arg(issue->assignedTo()->firstName()).arg(issue->assignedTo()->lastName()).trimmed());
    ui->updated->setText(issue->updatedOn().date().toString(Qt::SystemLocaleShortDate));

    if( !issue->dueDate().isValid() ) {
        ui->due->setText(QString());
        return;
    }
    int daysTo = QDate::currentDate().daysTo(issue->dueDate());
    if( daysTo == 0 ) {
        ui->due->setText(tr("Today"));
    } else {
        ui->due->setText(tr("In %n Days", "", daysTo));
    }

    if( daysTo > 7 ) {
        ui->due->setStyleSheet(QLatin1String("color: rgb(170, 170, 170);"));
    } else if( daysTo > 3 ) {
        ui->due->setStyleSheet(QLatin1String("color: rgb(40, 158, 65);"));
    } else if( daysTo > 1 ) {
        ui->due->setStyleSheet(QLatin1String("color: rgb(255, 160, 35);"));
    } else {
        ui->due->setStyleSheet(QLatin1String("color: rgb(255, 0, 0);"));
    }
}

RedmineConnector::Issue* TableItemIssue::issue()
{
    return this->m_issue;
}
