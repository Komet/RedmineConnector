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

    int id();
    QString firstName();
    QString lastName();
    QString mail();

    void setId(int id);
    void setFirstName(QString firstName);
    void setLastName(QString lastName);
    void setMail(QString mail);
    
signals:
    
public slots:

private:
    Repository *_repository;
    int _id;
    QString _firstName;
    QString _lastName;
    QString _mail;
};

}

#endif // USER_H
