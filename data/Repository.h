#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QDateTime>
#include <QAuthenticator>

#include "IssueStatus.h"
#include "Project.h"
#include "Priority.h"
#include "Tracker.h"
#include "User.h"

namespace RedmineConnector {

class Project;
class User;

class Repository : public QObject
{
    Q_OBJECT
public:
    explicit Repository(QObject *parent = 0);
    ~Repository();

    bool ready();
    void initialize();
    QNetworkAccessManager* qnam();

    int id();
    QString name();
    QString server();
    QString username();
    QString password();

    void setId(int id);
    void setName(QString name);
    void setServer(QString server);
    void setUsername(QString username);
    void setPassword(QString password);
    Priority* getAndAddPriority(int id, QString name);

    User* user(int id);
    IssueStatus *issueStatus(int id);
    Tracker tracker(int id);
    QList<Project*> projects();
    QList<User*> users();
    QList<IssueStatus*> issueStatuses();
    QList<Priority*> priorities();
    QList<Tracker> trackers();
    Project* project(int index);
    
signals:
    void ready(int, bool);
    
public slots:
    void authRequired(QNetworkReply *reply, QAuthenticator *auth);

private slots:
    void usersReadyRead();
    void issueStatusesReadyRead();
    void trackersReadyRead();
    void projectsReadyRead();
    void projectReady(int projectId, bool error);
    void checkForTimeouts();
    
private:
    bool m_ready;
    QNetworkAccessManager m_qnam;
    QNetworkReply *m_projectsReply;
    QNetworkReply *m_usersReply;
    QNetworkReply *m_issueStatusesReply;
    QNetworkReply *m_trackersReply;
    QTimer m_timeoutChecker;
    bool m_queryRunning;
    QDateTime m_lastQueryStarted;

    int m_id;
    QString m_name;
    QString m_server;
    QString m_username;
    QString m_password;

    QList<Project*> m_projects;
    QList<User*> m_users;
    QList<IssueStatus*> m_issueStatuses;
    QList<Priority*> m_priorities;
    QList<Tracker> m_trackers;

    void cleanUp();
    void parseUsers(QString xml);
    void parseIssueStatuses(QString xml);
    void parseProjects(QString xml);
    void parseTrackers(QString xml);
    void gatherPriorities();
};

}

#endif // REPOSITORY_H
