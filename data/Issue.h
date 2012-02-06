#ifndef ISSUE_H
#define ISSUE_H

#include <QDate>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>

#include "data/IssueCategory.h"
#include "data/IssueDetail.h"
#include "data/IssueStatus.h"
#include "data/Priority.h"
#include "data/Project.h"
#include "data/Tracker.h"
#include "data/User.h"

namespace RedmineConnector {

class Project;
class User;
class IssueCategory;
class IssueStatus;
class Priority;

class Issue : public QObject
{
    Q_OBJECT
public:
    explicit Issue(Project *project);
    ~Issue();

    int id();
    int priorityId();
    QString priorityName();
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
    IssueCategory *issueCategory();
    Priority *priority();
    Tracker tracker();
    QList<IssueDetail> issueDetails();
    Project *project();

    void setId(int id);
    void setPriorityId(int id);
    void setPriorityName(QString name);
    void setPriority(Priority* priority);
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
    void setIssueCategory(IssueCategory *issueCategory);
    void setTracker(Tracker tracker);

    void load();
    void setChangeset(IssueStatus *status, Priority *priority, User *assignedTo, Tracker tracker, IssueCategory *category, QDate startDate, QDate dueDate, int doneRatio, QString note);
    
signals:
    void ready(RedmineConnector::Issue*);
    
public slots:

private slots:
    void issueReadyRead();
    void issuePutReadyRead();

private:
    Project *m_project;
    QNetworkReply *m_issueReply;
    QNetworkReply *m_issuePutReply;

    int m_id;
    int m_priorityId;
    QString m_priorityName;
    QString m_subject;
    QString m_description;
    QDate m_startDate;
    QDate m_dueDate;
    float m_doneRatio;
    QDateTime m_createdOn;
    QDateTime m_updatedOn;
    Priority *m_priority;
    IssueStatus *m_issueStatus;
    User *m_author;
    User *m_assignedTo;
    Tracker m_tracker;
    IssueCategory *m_issueCategory;
    QList<IssueDetail> m_issueDetails;

    void parseIssue(QString xml);
};

}

#endif // ISSUE_H
