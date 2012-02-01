#ifndef ISSUE_H
#define ISSUE_H

#include <QObject>
#include <QDate>

#include "Project.h"
#include "IssueStatus.h"
#include "IssueCategory.h"
#include "User.h"

namespace RedmineConnector {

class Project;
class User;
class IssueCategory;
class IssueStatus;

class Issue : public QObject
{
    Q_OBJECT
public:
    explicit Issue(Project *project);
    ~Issue();

    int id();
    int priorityId();
    QString priority();
    QString subject();
    QString description();
    QDate startDate();
    QDate dueDate();
    float doneRatio();
    QDateTime createdOn();
    QDateTime updatedOn();
    User *author();
    User *assignedTo();
    IssueStatus *issueStatus();

    void setId(int id);
    void setPriorityId(int priorityId);
    void setPriority(QString priority);
    void setSubject(QString subject);
    void setDescription(QString description);
    void setStartDate(QString startDate);
    void setDueDate(QString dueDate);
    void setDoneRatio(float doneRatio);
    void setCreatedOn(QString createdOn);
    void setUpdatedOn(QString updatedOn);
    void setAuthor(User *author);
    void setAssignedTo(User *assignedTo);
    void setIssueStatus(IssueStatus *issueStatus);
    
signals:
    
public slots:

private:
    Project *_project;

    int _id;
    int _priorityId;
    QString _priority;
    QString _subject;
    QString _description;
    QDate _startDate;
    QDate _dueDate;
    float _doneRatio;
    QDateTime _createdOn;
    QDateTime _updatedOn;
    IssueStatus *_issueStatus;
    User *_author;
    User *_assignedTo;
    IssueCategory *_issueCategory;
};

}

#endif // ISSUE_H
