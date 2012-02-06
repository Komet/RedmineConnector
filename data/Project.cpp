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
    this->m_categoriesReply = 0;
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
    if( this->m_categoriesReply != 0 ) {
        this->m_categoriesReply->abort();
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
}

bool Project::ready()
{
    return this->m_ready;
}

void Project::checkForTimeouts()
{
    if( this->m_queryRunning && this->m_lastQueryStarted.secsTo(QDateTime::currentDateTime()) > Constants::NETWORK_TIMEOUT_VALUE ) {
        if( this->m_categoriesReply != 0 ) {
            this->m_categoriesReply->abort();
        }
        if( this->m_issuesReply != 0 ) {
            this->m_issuesReply->abort();
        }
    }
}

void Project::initialize()
{
    this->cleanUp();

    QUrl categoriesUrl(this->m_repository->server() + QString("/projects/%1/issue_categories.xml").arg(this->id()));
    categoriesUrl.setUserName(this->m_repository->username());
    categoriesUrl.setPassword(this->m_repository->password());

    this->m_lastQueryStarted = QDateTime::currentDateTime();
    this->m_queryRunning = true;
    this->m_categoriesReply = this->qnam()->get(QNetworkRequest(categoriesUrl));
    connect(this->m_categoriesReply, SIGNAL(finished()), this, SLOT(categoriesReadyRead()));
}

void Project::categoriesReadyRead()
{
    this->m_queryRunning = false;

    if( this->m_categoriesReply->error() != QNetworkReply::NoError ) {
        this->cleanUp();
        emit ready(this->id(), true);
        return;
    }

    QString msg = this->m_categoriesReply->readAll();
    this->parseCategories(msg);

    QUrl issuesUrl(this->m_repository->server() + QString("/projects/%1/issues.xml?limit=1000").arg(this->id()));
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

    QString msg = this->m_issuesReply->readAll();
    this->parseIssues(msg);
    this->m_ready = true;
    emit ready(this->id(), false);
    this->m_categoriesReply = 0;
    this->m_issuesReply = 0;
}

void Project::parseCategories(QString xml)
{
    // create an empty category
    IssueCategory *eC = new IssueCategory(this);
    eC->setName("");
    eC->setId(0);
    this->m_issueCategories.append(eC);

    QDomDocument domDoc;
    domDoc.setContent(xml);
    QDomNodeList nl = domDoc.elementsByTagName("issue_category");
    for( int i=0, n=nl.count() ; i<n ; i++ ) {
        IssueCategory *ic = new IssueCategory(this);
        ic->setId(nl.at(i).toElement().elementsByTagName("id").at(0).toElement().text().toInt());
        ic->setName(nl.at(i).toElement().elementsByTagName("name").at(0).toElement().text());
        this->m_issueCategories.append(ic);
    }
}

void Project::parseIssues(QString xml)
{
    QDomDocument domDoc;
    domDoc.setContent(xml);
    QDomNodeList nl = domDoc.elementsByTagName("issue");
    for( int i=0, n=nl.count() ; i<n ; i++ ) {
        Issue *issue = new Issue(this);
        issue->setId(nl.at(i).toElement().elementsByTagName("id").at(0).toElement().text().toInt());
        issue->setPriorityId(nl.at(i).toElement().elementsByTagName("priority").at(0).toElement().attribute("id").toInt());
        issue->setPriorityName(nl.at(i).toElement().elementsByTagName("priority").at(0).toElement().attribute("name"));
        issue->setSubject(nl.at(i).toElement().elementsByTagName("subject").at(0).toElement().text());
        issue->setDescription(nl.at(i).toElement().elementsByTagName("description").at(0).toElement().text());
        issue->setStartDate(nl.at(i).toElement().elementsByTagName("start_date").at(0).toElement().text());
        issue->setDueDate(nl.at(i).toElement().elementsByTagName("due_date").at(0).toElement().text());
        issue->setDoneRatio(nl.at(i).toElement().elementsByTagName("done_ratio").at(0).toElement().text().toFloat());
        issue->setCreatedOn(nl.at(i).toElement().elementsByTagName("created_on").at(0).toElement().text());
        issue->setUpdatedOn(nl.at(i).toElement().elementsByTagName("updated_on").at(0).toElement().text());
        issue->setAuthor(this->m_repository->user(nl.at(i).toElement().elementsByTagName("author").at(0).toElement().attribute("id").toInt()));
        issue->setAssignedTo(this->m_repository->user(nl.at(i).toElement().elementsByTagName("assigned_to").at(0).toElement().attribute("id").toInt()));
        issue->setIssueStatus(this->m_repository->issueStatus(nl.at(i).toElement().elementsByTagName("status").at(0).toElement().attribute("id").toInt()));
        issue->setIssueCategory(this->issueCategoryFromId(nl.at(i).toElement().elementsByTagName("category").at(0).toElement().attribute("id").toInt()));
        issue->setTracker(this->m_repository->tracker(nl.at(i).toElement().elementsByTagName("tracker").at(0).toElement().attribute("id").toInt()));
        this->m_issues.append(issue);
    }
}


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

/*** GETTER ***/

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
