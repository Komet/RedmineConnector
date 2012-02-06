#include "IssueStatus.h"

namespace RedmineConnector {

IssueStatus::IssueStatus(Repository *repository) :
    QObject(repository)
{
    this->m_repository = repository;
    this->m_id = 0;
}

IssueStatus::~IssueStatus()
{
}

int IssueStatus::id()
{
    return this->m_id;
}

QString IssueStatus::name()
{
    return this->m_name;
}

void IssueStatus::setId(int id)
{
    this->m_id = id;
}

void IssueStatus::setName(QString name)
{
    this->m_name = name;
}

}
