#include "Project.h"

#include <QDebug>
#include <QDomDocument>

#include "RedmineConnectorConstants.h"

namespace RedmineConnector {

Project::Project(Repository *repository) :
    QObject(repository)
{
    this->_repository = repository;
    this->_ready = false;
    this->_queryRunning = false;
    this->_categoriesReply = 0;
    this->_issuesReply = 0;
    connect(&this->_timeoutChecker, SIGNAL(timeout()), this, SLOT(checkForTimeouts()));
    this->_timeoutChecker.start(2000);
}

Project::~Project()
{
}

void Project::cleanUp()
{
    this->_ready = false;
    this->_queryRunning = false;
    if( this->_categoriesReply != 0 ) {
        this->_categoriesReply->abort();
    }
    if( this->_issuesReply != 0 ) {
        this->_issuesReply->abort();
    }
    for( int i=0 ; i<this->_issues.size() ; i++ ) {
        delete this->_issues[i];
    }
    this->_issues.clear();
    for( int i=0 ; i<this->_issueCategories.size() ; i++ ) {
        delete this->_issueCategories[i];
    }
    this->_issueCategories.clear();
}

bool Project::ready()
{
    return this->_ready;
}

void Project::checkForTimeouts()
{
    if( this->_queryRunning && this->_lastQueryStarted.secsTo(QDateTime::currentDateTime()) > Constants::NETWORK_TIMEOUT_VALUE ) {
        if( this->_categoriesReply != 0 ) {
            this->_categoriesReply->abort();
        }
        if( this->_issuesReply != 0 ) {
            this->_issuesReply->abort();
        }
    }
}

void Project::initialize()
{
    qDebug("Project::initialize %d: %s", this->_id, qPrintable(this->_identifier));

    this->cleanUp();

    QUrl categoriesUrl(this->_repository->server() + QString("/projects/%1/issue_categories.xml").arg(this->id()));
    categoriesUrl.setUserName(this->_repository->username());
    categoriesUrl.setPassword(this->_repository->password());

    this->_lastQueryStarted = QDateTime::currentDateTime();
    this->_queryRunning = true;
    this->_categoriesReply = this->_qnam.get(QNetworkRequest(categoriesUrl));
    connect(this->_categoriesReply, SIGNAL(finished()), this, SLOT(categoriesReadyRead()));
}

void Project::categoriesReadyRead()
{
    qDebug("Project::categoriesReadyRead %s", qPrintable(this->_identifier));

    this->_queryRunning = false;

    if( this->_categoriesReply->error() != QNetworkReply::NoError ) {
        this->cleanUp();
        emit ready(this->id(), true);
        return;
    }

    QString msg = this->_categoriesReply->readAll();
    this->parseCategories(msg);
    qDebug("  we now have %d categories", this->_issueCategories.size());

    QUrl issuesUrl(this->_repository->server() + QString("/projects/%1/issues.xml?limit=1000").arg(this->id()));
    issuesUrl.setUserName(this->_repository->username());
    issuesUrl.setPassword(this->_repository->password());

    this->_lastQueryStarted = QDateTime::currentDateTime();
    this->_queryRunning = true;
    this->_issuesReply = this->_qnam.get(QNetworkRequest(issuesUrl));
    connect(this->_issuesReply, SIGNAL(finished()), this, SLOT(issuesReadyRead()));
}

void Project::issuesReadyRead()
{
    qDebug("Project::issuesReadyRead %s", qPrintable(this->identifier()));

    this->_queryRunning = false;

    if( this->_issuesReply->error() != QNetworkReply::NoError ) {
        this->cleanUp();
        emit ready(this->id(), true);
        return;
    }

    QString msg = this->_issuesReply->readAll();
    this->parseIssues(msg);
    qDebug("  we now have %d issues", this->_issues.size());
    this->_ready = true;
    emit ready(this->id(), false);
    this->_categoriesReply = 0;
    this->_issuesReply = 0;
}

void Project::parseCategories(QString xml)
{
    QDomDocument domDoc;
    domDoc.setContent(xml);
    QDomNodeList nl = domDoc.elementsByTagName("issue_category");
    for( int i=0 ; i<nl.count() ; i++ ) {
        IssueCategory *ic = new IssueCategory(this);
        ic->setId(nl.at(i).toElement().elementsByTagName("id").at(0).toElement().text().toInt());
        ic->setName(nl.at(i).toElement().elementsByTagName("name").at(0).toElement().text());
        this->_issueCategories.append(ic);
    }
}

void Project::parseIssues(QString xml)
{
    QDomDocument domDoc;
    domDoc.setContent(xml);
    QDomNodeList nl = domDoc.elementsByTagName("issue");
    for( int i=0 ; i<nl.count() ; i++ ) {
        Issue *issue = new Issue(this);
        issue->setId(nl.at(i).toElement().elementsByTagName("id").at(0).toElement().text().toInt());
        issue->setPriorityId(nl.at(i).toElement().elementsByTagName("priority").at(0).toElement().attribute("id").toInt());
        issue->setPriority(nl.at(i).toElement().elementsByTagName("priority").at(0).toElement().attribute("name"));
        issue->setSubject(nl.at(i).toElement().elementsByTagName("subject").at(0).toElement().text());
        issue->setDescription(nl.at(i).toElement().elementsByTagName("description").at(0).toElement().text());
        issue->setStartDate(nl.at(i).toElement().elementsByTagName("start_date").at(0).toElement().text());
        issue->setDueDate(nl.at(i).toElement().elementsByTagName("due_date").at(0).toElement().text());
        issue->setDoneRatio(nl.at(i).toElement().elementsByTagName("done_ratio").at(0).toElement().text().toFloat());
        issue->setCreatedOn(nl.at(i).toElement().elementsByTagName("created_on").at(0).toElement().text());
        issue->setUpdatedOn(nl.at(i).toElement().elementsByTagName("updated_on").at(0).toElement().text());
        issue->setAuthor(this->_repository->user(nl.at(i).toElement().elementsByTagName("author").at(0).toElement().attribute("id").toInt()));
        issue->setAssignedTo(this->_repository->user(nl.at(i).toElement().elementsByTagName("assigned_to").at(0).toElement().attribute("id").toInt()));
        issue->setIssueStatus(this->_repository->issueStatus(nl.at(i).toElement().elementsByTagName("status").at(0).toElement().attribute("id").toInt()));
        this->_issues.append(issue);
    }
}


QList<Issue*> Project::issues()
{
    return this->_issues;
}

/*** GETTER ***/

int Project::id()
{
    return this->_id;
}

QString Project::identifier()
{
    return this->_identifier;
}

QString Project::name()
{
    return this->_name;
}

QString Project::description()
{
    return this->_description;
}


/*** SETTER ***/

void Project::setId(int id)
{
    this->_id = id;
}

void Project::setIdentifier(QString identifier)
{
    this->_identifier = identifier;
}

void Project::setName(QString name)
{
    this->_name = name;
}

void Project::setDescription(QString description)
{
    this->_description = description;
}


}
