#ifndef PROJECT_H
#define PROJECT_H

#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QTimer>

#include "data/Issue.h"
#include "data/IssueCategory.h"
#include "data/Repository.h"

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

signals:
    void ready(int, bool);
    
public slots:

private slots:
    void categoriesReadyRead();
    void issuesReadyRead();
    void checkForTimeouts();

private:
    Repository *m_repository;
    bool m_ready;
    QNetworkReply *m_issuesReply;
    QNetworkReply *m_categoriesReply;
    bool m_queryRunning;
    QTimer m_timeoutChecker;
    QDateTime m_lastQueryStarted;

    int m_id;
    QString m_name;
    QString m_identifier;
    QString m_description;

    QList<IssueCategory*> m_issueCategories;
    QList<Issue*> m_issues;

    void cleanUp();
    void parseCategories(QString xml);
    void parseIssues(QString xml);
};

}
#endif // PROJECT_H
