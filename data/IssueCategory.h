#ifndef ISSUECATEGORY_H
#define ISSUECATEGORY_H

#include <QObject>

#include "Project.h"

namespace RedmineConnector {

class Project;

class IssueCategory : public QObject
{
    Q_OBJECT
public:
    explicit IssueCategory(Project *project);
    ~IssueCategory();

    int id();
    QString name();

    void setId(int id);
    void setName(QString name);
    
signals:
    
public slots:
    
private:
    Project *m_project;
    int m_id;
    QString m_name;
};

}

#endif // ISSUECATEGORY_H
