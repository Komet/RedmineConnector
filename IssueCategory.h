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

    int id();
    QString name();

    void setId(int id);
    void setName(QString name);
    
signals:
    
public slots:
    
private:
    Project *_project;
    int _id;
    QString _name;
};

}

#endif // ISSUECATEGORY_H
