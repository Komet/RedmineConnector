#ifndef ISSUESTATUS_H
#define ISSUESTATUS_H

#include <QObject>

#include "Repository.h"

namespace RedmineConnector {

class Repository;

class IssueStatus : public QObject
{
    Q_OBJECT
public:
    explicit IssueStatus(Repository *repository);
    
    int id();
    QString name();

    void setId(int id);
    void setName(QString name);

signals:
    
public slots:
    
private:
    Repository *_repository;

    int _id;
    QString _name;
};

}

#endif // ISSUESTATUS_H
