#include "IssueStatus.h"

namespace RedmineConnector {

IssueStatus::IssueStatus(Repository *repository) :
    QObject(repository)
{
    this->_repository = repository;
}

int IssueStatus::id()
{
    return this->_id;
}

QString IssueStatus::name()
{
    return this->_name;
}

void IssueStatus::setId(int id)
{
    this->_id = id;
}

void IssueStatus::setName(QString name)
{
    this->_name = name;
}

}
