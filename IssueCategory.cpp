#include "IssueCategory.h"

namespace RedmineConnector {

IssueCategory::IssueCategory(Project *project) :
    QObject(project)
{
    this->_project = project;
}

int IssueCategory::id()
{
    return this->_id;
}

QString IssueCategory::name()
{
    return this->_name;
}

void IssueCategory::setId(int id)
{
    this->_id = id;
}

void IssueCategory::setName(QString name)
{
    this->_name = name;
}

}
