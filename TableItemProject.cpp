#include "TableItemProject.h"
#include "ui_TableItemProject.h"

TableItemProject::TableItemProject(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TableItemProject)
{
    ui->setupUi(this);
}

TableItemProject::~TableItemProject()
{
    delete ui;
}

void TableItemProject::setProject(RedmineConnector::Project *project)
{
    this->m_project = project;
    ui->labelProject->setText(this->m_project->name());
    ui->labelIssues->setText(tr("%n issues", "", this->m_project->issues().size()));
}

RedmineConnector::Project* TableItemProject::project()
{
    return this->m_project;
}

void TableItemProject::setIsCurrentItem(bool isCurrent)
{
    QFont font = ui->labelProject->font();
    font.setBold(isCurrent);
    ui->labelProject->setFont(font);
    if( isCurrent ) {
        ui->tableWidgetProject->setStyleSheet("background-color: rgba(200, 220, 255);");
    } else {
        ui->tableWidgetProject->setStyleSheet("");
    }

}
