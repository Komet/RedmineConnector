#include "Project.h"

#include <QDebug>
#include <QDomDocument>

#include "RedmineConnectorConstants.h"

namespace RedmineConnector {

Project::Project(Repository *repository) :
    QObject(repository)
{
    this->m_repository = repository;
    this->m_ready = false;
    this->m_queryRunning = false;
    this->m_projectReply = 0;
    this->m_issuesReply = 0;
    connect(&this->m_timeoutChecker, SIGNAL(timeout()), this, SLOT(checkForTimeouts()));
    this->m_timeoutChecker.start(2000);
}

Project::~Project()
{
}

void Project::cleanUp()
{
    this->m_ready = false;
    this->m_queryRunning = false;
    if( this->m_projectReply != 0 ) {
        this->m_projectReply->abort();
    }
    if( this->m_issuesReply != 0 ) {
        this->m_issuesReply->abort();
    }
    for( int i=0, n=this->m_issues.size() ; i<n ; i++ ) {
        delete this->m_issues[i];
    }
    this->m_issues.clear();
    for( int i=0, n=this->m_issueCategories.size() ; i<n ; i++ ) {
        delete this->m_issueCategories[i];
    }
    this->m_issueCategories.clear();
    this->m_trackers.clear();
}

bool Project::ready()
{
    return this->m_ready;
}

void Project::checkForTimeouts()
{
    if( this->m_queryRunning && this->m_lastQueryStarted.secsTo(QDateTime::currentDateTime()) > Constants::NETWORK_TIMEOUT_VALUE ) {
        if( this->m_projectReply != 0 ) {
            this->m_projectReply->abort();
        }
        if( this->m_issuesReply != 0 ) {
            this->m_issuesReply->abort();
        }
    }
}

void Project::initialize()
{
    this->cleanUp();

    QUrl projectUrl(this->m_repository->server() + QString::fromLatin1("/projects/%1.xml?include=trackers,issue_categories").arg(this->id()));
    projectUrl.setUserName(this->m_repository->username());
    projectUrl.setPassword(this->m_repository->password());

    this->m_lastQueryStarted = QDateTime::currentDateTime();
    this->m_queryRunning = true;
    this->m_projectReply = this->qnam()->get(QNetworkRequest(projectUrl));
    connect(this->m_projectReply, SIGNAL(finished()), this, SLOT(projectReadyRead()));
}

void Project::projectReadyRead()
{
    this->m_queryRunning = false;

    if( this->m_projectReply->error() != QNetworkReply::NoError ) {
        this->cleanUp();
        emit ready(this->id(), true);
        return;
    }

    QString msg = QString::fromLatin1(this->m_projectReply->readAll().constData());
    this->parseProject(msg);

    QUrl issuesUrl(this->m_repository->server() + QString::fromLatin1("/projects/%1/issues.xml?limit=100").arg(this->id()));
    issuesUrl.setUserName(this->m_repository->username());
    issuesUrl.setPassword(this->m_repository->password());

    this->m_lastQueryStarted = QDateTime::currentDateTime();
    this->m_queryRunning = true;
    this->m_issuesReply = this->qnam()->get(QNetworkRequest(issuesUrl));
    connect(this->m_issuesReply, SIGNAL(finished()), this, SLOT(issuesReadyRead()));
}

void Project::issuesReadyRead()
{
    this->m_queryRunning = false;

    if( this->m_issuesReply->error() != QNetworkReply::NoError ) {
        this->cleanUp();
        emit ready(this->id(), true);
        return;
    }

    QString msg = QString::fromLatin1(this->m_issuesReply->readAll().constData());
    this->parseIssues(msg);
    this->m_ready = true;
    emit ready(this->id(), false);
    this->m_projectReply = 0;
    this->m_issuesReply = 0;
}

void Project::parseProject(QString xml)
{
    // create an empty category
    IssueCategory *eC = new IssueCategory(this);
    eC->setName(QString());
    eC->setId(0);
    this->m_issueCategories.append(eC);

    QDomDocument domDoc;
    domDoc.setContent(xml);
    QDomNodeList nl = domDoc.elementsByTagName(QLatin1String("issue_category"));
    for( int i=0, n=nl.count() ; i<n ; i++ ) {
        IssueCategory *ic = new IssueCategory(this);
        ic->setId(nl.at(i).toElement().attribute(QLatin1String("id")).toInt());
        ic->setName(nl.at(i).toElement().attribute(QLatin1String("name")));
        this->m_issueCategories.append(ic);
    }

    nl = domDoc.elementsByTagName(QLatin1String("tracker"));
    for( int i=0, n=nl.count() ; i<n ; i++ ) {
        Tracker t;
        t.id = nl.at(i).toElement().attribute(QLatin1String("id")).toInt();
        t.name = nl.at(i).toElement().attribute(QLatin1String("name"));
        this->m_trackers.append(t);
    }
}

void Project::parseIssues(QString xml)
{
    QDomDocument domDoc;
    domDoc.setContent(xml);
    QDomNodeList nl = domDoc.elementsByTagName(QLatin1String("issue"));
    for( int i=0, n=nl.count() ; i<n ; i++ ) {
        Issue *issue = new Issue(this);
        issue->setId(nl.at(i).toElement().elementsByTagName(QLatin1String("id")).at(0).toElement().text().toInt());
        issue->setPriorityId(nl.at(i).toElement().elementsByTagName(QLatin1String("priority")).at(0).toElement().attribute(QLatin1String("id")).toInt());
        issue->setPriorityName(nl.at(i).toElement().elementsByTagName(QLatin1String("priority")).at(0).toElement().attribute(QLatin1String("name")));
        issue->setSubject(nl.at(i).toElement().elementsByTagName(QLatin1String("subject")).at(0).toElement().text());
        issue->setDescription(nl.at(i).toElement().elementsByTagName(QLatin1String("description")).at(0).toElement().text());
        issue->setStartDate(nl.at(i).toElement().elementsByTagName(QLatin1String("start_date")).at(0).toElement().text());
        issue->setDueDate(nl.at(i).toElement().elementsByTagName(QLatin1String("due_date")).at(0).toElement().text());
        issue->setDoneRatio(nl.at(i).toElement().elementsByTagName(QLatin1String("done_ratio")).at(0).toElement().text().toFloat());
        issue->setCreatedOn(nl.at(i).toElement().elementsByTagName(QLatin1String("created_on")).at(0).toElement().text());
        issue->setUpdatedOn(nl.at(i).toElement().elementsByTagName(QLatin1String("updated_on")).at(0).toElement().text());
        issue->setAuthor(this->m_repository->user(nl.at(i).toElement().elementsByTagName(QLatin1String("author")).at(0).toElement().attribute(QLatin1String("id")).toInt()));
        issue->setAssignedTo(this->m_repository->user(nl.at(i).toElement().elementsByTagName(QLatin1String("assigned_to")).at(0).toElement().attribute(QLatin1String("id")).toInt()));
        issue->setIssueStatus(this->m_repository->issueStatus(nl.at(i).toElement().elementsByTagName(QLatin1String("status")).at(0).toElement().attribute(QLatin1String("id")).toInt()));
        issue->setIssueCategory(this->issueCategoryFromId(nl.at(i).toElement().elementsByTagName(QLatin1String("category")).at(0).toElement().attribute(QLatin1String("id")).toInt()));
        issue->setTracker(this->tracker(nl.at(i).toElement().elementsByTagName(QLatin1String("tracker")).at(0).toElement().attribute(QLatin1String("id")).toInt()));
        this->m_issues.append(issue);
    }
}


/*** GETTER ***/

QList<Issue*> Project::issues()
{
    return this->m_issues;
}

QList<IssueCategory*> Project::issueCategories()
{
    return this->m_issueCategories;
}

Issue* Project::issue(int index)
{
    if( index >= 0 && index < this->m_issues.size() ) {
        return this->m_issues.at(index);
    }

    return new Issue(this);
}

QList<Tracker> Project::trackers()
{
    return this->m_trackers;
}

Tracker Project::tracker(int id)
{
    for( int i=0, n=this->m_trackers.size() ; i<n ; i++ ) {
        if( this->m_trackers[i].id == id ) {
            return this->m_trackers[i];
        }
    }
    Tracker t;
    t.id = 0;
    t.name = tr("unknown", "unknown tracker name");
    return t;
}

IssueCategory* Project::issueCategory(int index)
{
    if( index >= 0 && index < this->m_issueCategories.size() ) {
        return this->m_issueCategories.at(index);
    }

    return new IssueCategory(this);
}

IssueCategory* Project::issueCategoryFromId(int id)
{
    for( int i=0, n=this->m_issueCategories.size() ; i<n ; i++ ) {
        if( this->m_issueCategories[i]->id() == id ) {
            return this->m_issueCategories[i];
        }
    }
    return new IssueCategory(this);
}

QNetworkAccessManager* Project::qnam()
{
    return this->repository()->qnam();
}

Repository *Project::repository()
{
    return this->m_repository;
}

int Project::id()
{
    return this->m_id;
}

QString Project::identifier()
{
    return this->m_identifier;
}

QString Project::name()
{
    return this->m_name;
}

QString Project::description()
{
    return this->m_description;
}


/*** SETTER ***/

void Project::setId(int id)
{
    this->m_id = id;
}

void Project::setIdentifier(QString identifier)
{
    this->m_identifier = identifier;
}

void Project::setName(QString name)
{
    this->m_name = name;
}

void Project::setDescription(QString description)
{
    this->m_description = description;
}


}
