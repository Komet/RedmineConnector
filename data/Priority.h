#ifndef PRIORITY_H
#define PRIORITY_H

#include <QObject>

#include "data/Repository.h"

namespace RedmineConnector {

class Repository;

class Priority : public QObject
{
    Q_OBJECT
public:
    explicit Priority(Repository *repository);
    ~Priority();

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

#endif // PRIORITY_H
