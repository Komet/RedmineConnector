#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDateTime>
#include <QTimer>

#include "Repository.h"
#include "IssueCategory.h"
#include "Issue.h"

namespace RedmineConnector {

class Issue;

class Project : public QObject
{
    Q_OBJECT
public:
    explicit Project(Repository *repository);
    ~Project();

    bool ready();
    void initialize();

    int id();
    QString name();
    QString identifier();
    QString description();

    void setId(int id);
    void setName(QString name);
    void setIdentifier(QString identifier);
    void setDescription(QString description);

    QList<Issue*> issues();

signals:
    void ready(int, bool);
    
public slots:

private slots:
    void categoriesReadyRead();
    void issuesReadyRead();
    void checkForTimeouts();

private:
    Repository *_repository;
    bool _ready;
    QNetworkAccessManager _qnam;
    QNetworkReply *_issuesReply;
    QNetworkReply *_categoriesReply;
    bool _queryRunning;
    QTimer _timeoutChecker;
    QDateTime _lastQueryStarted;

    int _id;
    QString _name;
    QString _identifier;
    QString _description;

    QList<IssueCategory*> _issueCategories;
    QList<Issue*> _issues;

    void cleanUp();
    void parseCategories(QString xml);
    void parseIssues(QString xml);
};

}
#endif // PROJECT_H
