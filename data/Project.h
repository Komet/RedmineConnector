#ifndef PROJECT_H
#define PROJECT_H

#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QTimer>

#include "Issue.h"
#include "IssueCategory.h"
#include "Repository.h"
#include "Tracker.h"

namespace RedmineConnector {

class Issue;
class IssueCategory;

class Project : public QObject
{
    Q_OBJECT
public:
    explicit Project(Repository *repository);
    ~Project();

    bool ready();
    void initialize();
    Repository *repository();
    QNetworkAccessManager *qnam();

    int id();
    QString name();
    QString identifier();
    QString description();

    void setId(int id);
    void setName(QString name);
    void setIdentifier(QString identifier);
    void setDescription(QString description);

    QList<Issue*> issues();
    QList<IssueCategory*> issueCategories();
    Issue* issue(int index);
    IssueCategory* issueCategory(int index);
    IssueCategory* issueCategoryFromId(int id);
    Tracker tracker(int id);
    QList<Tracker> trackers();

signals:
    void ready(int, bool);
    
public slots:

private slots:
    void projectReadyRead();
    void issuesReadyRead();
    void checkForTimeouts();

private:
    Repository *m_repository;
    bool m_ready;
    QNetworkReply *m_issuesReply;
    QNetworkReply *m_projectReply;
    bool m_queryRunning;
    QTimer m_timeoutChecker;
    QDateTime m_lastQueryStarted;

    int m_id;
    QString m_name;
    QString m_identifier;
    QString m_description;

    QList<IssueCategory*> m_issueCategories;
    QList<Issue*> m_issues;
    QList<Tracker> m_trackers;

    void cleanUp();
    void parseProject(QString xml);
    void parseIssues(QString xml);
};

}
#endif // PROJECT_H
