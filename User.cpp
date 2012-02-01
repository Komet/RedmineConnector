#include "User.h"

namespace RedmineConnector {

User::User(Repository *repository) :
    QObject(repository)
{
    this->_repository = repository;
}

int User::id()
{
    return this->_id;
}

QString User::firstName()
{
    return this->_firstName;
}

QString User::lastName()
{
    return this->_lastName;
}

QString User::mail()
{
    return this->_mail;
}

void User::setId(int id)
{
    this->_id = id;
}

void User::setFirstName(QString firstName)
{
    this->_firstName = firstName;
}

void User::setLastName(QString lastName)
{
    this->_lastName = lastName;
}

void User::setMail(QString mail)
{
    this->_mail = mail;
}

}
