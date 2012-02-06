#ifndef USER_H
#define USER_H

#include <QObject>

#include "Repository.h"

namespace RedmineConnector {

class Repository;

class User : public QObject
{
    Q_OBJECT
public:
    explicit User(Repository *repository);
    ~User();

    int id();
    QString firstName();
    QString lastName();
    QString fullName();
    QString mail();

    void setId(int id);
    void setFirstName(QString firstName);
    void setLastName(QString lastName);
    void setMail(QString mail);
    
signals:
    
public slots:

private:
    Repository *m_repository;
    int m_id;
    QString m_firstName;
    QString m_lastName;
    QString m_mail;
};

}

#endif // USER_H
