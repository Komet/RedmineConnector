#include "Priority.h"

namespace RedmineConnector {

Priority::Priority(Repository *repository) :
    QObject(repository)
{
    this->m_repository = repository;
}

Priority::~Priority()
{
}

/*** GETTER ***/

int Priority::id()
{
    return this->m_id;
}

QString Priority::name()
{
    return this->m_name;
}

/*** SETTER ***/

void Priority::setId(int id)
{
    this->m_id = id;
}

void Priority::setName(QString name)
{
    this->m_name = name;
}

}
