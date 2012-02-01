#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QDateTime>
#include <QAuthenticator>

#include "User.h"
#include "IssueStatus.h"
#include "Project.h"

namespace RedmineConnector {

class Project;

class Repository : public QObject
{
    Q_OBJECT
public:
    explicit Repository(QObject *parent = 0);
    ~Repository();

    bool ready();
    void initialize();

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

    User* user(int id);
    IssueStatus *issueStatus(int id);
    QList<Project*> projects();
    
signals:
    void ready(int, bool);
    
public slots:
    void authRequired(QNetworkReply *reply, QAuthenticator *auth);

private slots:
    void usersReadyRead();
    void issueStatusesReadyRead();
    void projectsReadyRead();
    void projectReady(int projectId, bool error);
    void checkForTimeouts();
    
private:
    bool _ready;
    QNetworkAccessManager _qnam;
    QNetworkReply *_projectsReply;
    QNetworkReply *_usersReply;
    QNetworkReply *_issueStatusesReply;
    QTimer _timeoutChecker;
    bool _queryRunning;
    QDateTime _lastQueryStarted;

    int _id;
    QString _name;
    QString _server;
    QString _username;
    QString _password;

    QList<Project*> _projects;
    QList<User*> _users;
    QList<IssueStatus*> _issueStatuses;

    void cleanUp();
    void parseUsers(QString xml);
    void parseIssueStatuses(QString xml);
    void parseProjects(QString xml);
};

}

#endif // REPOSITORY_H
