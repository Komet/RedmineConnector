#include "User.h"

namespace RedmineConnector {

User::User(Repository *repository) :
    QObject(repository)
{
    this->m_repository = repository;
}

User::~User()
{
}

int User::id()
{
    return this->m_id;
}

QString User::firstName()
{
    return this->m_firstName;
}

QString User::lastName()
{
    return this->m_lastName;
}

QString User::fullName()
{
    return QString::fromLatin1("%1 %2").arg(this->m_firstName).arg(this->m_lastName).trimmed();
}

QString User::mail()
{
    return this->m_mail;
}

void User::setId(int id)
{
    this->m_id = id;
}

void User::setFirstName(QString firstName)
{
    this->m_firstName = firstName;
}

void User::setLastName(QString lastName)
{
    this->m_lastName = lastName;
}

void User::setMail(QString mail)
{
    this->m_mail = mail;
}

}
