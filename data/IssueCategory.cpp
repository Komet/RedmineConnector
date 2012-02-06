#include "IssueCategory.h"

namespace RedmineConnector {

IssueCategory::IssueCategory(Project *project) :
    QObject(project)
{
    this->m_project = project;
    this->m_id = 0;
}

IssueCategory::~IssueCategory()
{
}

int IssueCategory::id()
{
    return this->m_id;
}

QString IssueCategory::name()
{
    return this->m_name;
}

void IssueCategory::setId(int id)
{
    this->m_id = id;
}

void IssueCategory::setName(QString name)
{
    this->m_name = name;
}

}
