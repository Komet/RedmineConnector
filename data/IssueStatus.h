#ifndef ISSUESTATUS_H
#define ISSUESTATUS_H

#include <QObject>

#include "data/Repository.h"

namespace RedmineConnector {

class Repository;

class IssueStatus : public QObject
{
    Q_OBJECT
public:
    explicit IssueStatus(Repository *repository);
    ~IssueStatus();
    
    int id();
    QString name();

    void setId(int id);
    void setName(QString name);

signals:
    
public slots:
    
private:
    Repository *m_repository;

    int m_id;
    QString m_name;
};

}

#endif // ISSUESTATUS_H
