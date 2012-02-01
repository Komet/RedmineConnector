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

    QFont font = ui->assignedTo->font();
    font.setPointSize(font.pointSize()-2);
    ui->assignedTo->setFont(font);
    ui->updated->setFont(font);
    font.setBold(true);
    ui->issueId->setFont(font);
}

TableItemIssue::~TableItemIssue()
{
    delete ui;
}

void TableItemIssue::setSubject(QString subject)
{
    ui->subject->setText(subject);
}

void TableItemIssue::setStatus(QString status)
{
    ui->status->setText(status);
}

void TableItemIssue::setAssignedTo(QString assignedTo)
{
    ui->assignedTo->setText(assignedTo);
}

void TableItemIssue::setUpdated(QString updated)
{
    ui->updated->setText(updated);
}

void TableItemIssue::setIssueId(int id)
{
    ui->issueId->setText(QString("#%1").arg(id));
}
