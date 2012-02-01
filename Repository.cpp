#include "Repository.h"

#include <QDebug>
#include <QDomDocument>

#include "RedmineConnectorConstants.h"
#include "NetworkAuthDialog.h"

namespace RedmineConnector {

Repository::Repository(QObject *parent) :
    QObject(parent)
{
    this->_ready = false;
    this->_queryRunning = false;
    this->_issueStatusesReply = 0;
    this->_projectsReply = 0;
    this->_usersReply = 0;
    connect(&this->_timeoutChecker, SIGNAL(timeout()), this, SLOT(checkForTimeouts()));
    this->_timeoutChecker.start(2000);

    connect(&this->_qnam, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), this, SLOT(authRequired(QNetworkReply*, QAuthenticator*)));
}

Repository::~Repository()
{
    qDebug("destruct repository %d", this->id());
    this->cleanUp();
    qDebug("cleanup done");
}

void Repository::cleanUp()
{
    this->_ready = false;
    this->_queryRunning = false;
    if( this->_issueStatusesReply != 0 ) {
        this->_issueStatusesReply->abort();
    }
    if( this->_projectsReply != 0 ) {
        this->_projectsReply->abort();
    }
    if( this->_usersReply != 0 ) {
        this->_usersReply->abort();
    }
    for( int i=0 ; i<this->_projects.size() ; i++ ) {
        delete this->_projects[i];
    }
    this->_projects.clear();
    for( int i=0 ; i<this->_users.size() ; i++ ) {
        delete this->_users[i];
    }
    this->_users.clear();
    for( int i=0 ; i<this->_issueStatuses.size() ; i++ ) {
        delete this->_issueStatuses[i];
    }
    this->_issueStatuses.clear();
}

void Repository::checkForTimeouts()
{
    if( this->_queryRunning && this->_lastQueryStarted.secsTo(QDateTime::currentDateTime()) > Constants::NETWORK_TIMEOUT_VALUE ) {
        if( this->_issueStatusesReply != 0 ) {
            this->_issueStatusesReply->abort();
        }
        if( this->_projectsReply != 0 ) {
            this->_projectsReply->abort();
        }
        if( this->_usersReply != 0 ) {
            this->_usersReply->abort();
        }
    }
}

void Repository::initialize()
{
    qDebug("Repository::initialize %d", this->_id);

    this->cleanUp();

    QUrl usersUrl(this->server() + "/users.xml?limit=1000");
    usersUrl.setUserName(this->username());
    usersUrl.setPassword(this->password());

    this->_lastQueryStarted = QDateTime::currentDateTime();
    this->_queryRunning = true;
    this->_usersReply = this->_qnam.get(QNetworkRequest(usersUrl));
    connect(this->_usersReply, SIGNAL(finished()), this, SLOT(usersReadyRead()));
}

void Repository::authRequired(QNetworkReply *reply, QAuthenticator *auth)
{
    NetworkAuthDialog dlg;
    dlg.setSitename(QString("%1 @ %2").arg(auth->realm()).arg(reply->url().host()));
    dlg.setUsername(reply->url().userName());
    dlg.setPassword(reply->url().password());
    if( dlg.exec() == QDialog::Accepted ) {
        this->setPassword(dlg.password());
        this->setUsername(dlg.username());
        auth->setUser(dlg.username());
        auth->setPassword(dlg.password());
    }
}

void Repository::usersReadyRead()
{
    qDebug("Repository::usersReadyRead %d", this->_id);

    this->_queryRunning = false;

    if( this->_usersReply->error() != QNetworkReply::NoError ) {
        this->cleanUp();
        emit ready(this->id(), true);
        return;
    }

    QString msg = this->_usersReply->readAll();
    this->parseUsers(msg);
    qDebug("  we now have %d users", this->_users.size());

    QUrl issueStatusesUrl(this->server() + "/issue_statuses.xml");
    issueStatusesUrl.setUserName(this->username());
    issueStatusesUrl.setPassword(this->password());

    this->_lastQueryStarted = QDateTime::currentDateTime();
    this->_queryRunning = true;
    this->_issueStatusesReply = this->_qnam.get(QNetworkRequest(issueStatusesUrl));
    connect(this->_issueStatusesReply, SIGNAL(finished()), this, SLOT(issueStatusesReadyRead()));
}

void Repository::issueStatusesReadyRead()
{
    qDebug("Repository::issueStatusesReadyRead %d", this->_id);

    this->_queryRunning = false;

    if( this->_issueStatusesReply->error() != QNetworkReply::NoError ) {
        this->cleanUp();
        emit ready(this->id(), true);
        return;
    }

    QString msg = this->_issueStatusesReply->readAll();
    this->parseIssueStatuses(msg);
    qDebug("  we now have %d issue statuses", this->_issueStatuses.size());

    QUrl projectsUrl(this->server() + "/projects.xml?limit=1000");
    projectsUrl.setUserName(this->username());
    projectsUrl.setPassword(this->password());

    this->_lastQueryStarted = QDateTime::currentDateTime();
    this->_queryRunning = true;
    this->_projectsReply = this->_qnam.get(QNetworkRequest(projectsUrl));
    connect(this->_projectsReply, SIGNAL(finished()), this, SLOT(projectsReadyRead()));
}

void Repository::projectsReadyRead()
{
    qDebug("Repository::projectsReadyRead %d", this->_id);

    this->_queryRunning = false;

    if( this->_projectsReply->error() != QNetworkReply::NoError ) {
        this->cleanUp();
        emit ready(this->id(), true);
        return;
    }

    QString msg = this->_projectsReply->readAll();
    this->parseProjects(msg);
    qDebug("  we now have %d projects", this->_projects.size());
    for( int i=0 ; i<this->_projects.size() ; i++ ) {
        this->_projects[i]->initialize();
        connect(this->_projects[i], SIGNAL(ready(int, bool)), this, SLOT(projectReady(int, bool)));
    }
    this->_projectsReply = 0;
    this->_issueStatusesReply = 0;
    this->_usersReply = 0;
}

void Repository::projectReady(int projectId, bool error)
{
    qDebug("Repository::projectReady %d", projectId);

    if( error ) {
        for( int i=0 ; i<this->_projects.size() ; i++ ) {
            if( this->_projects[i]->id() == projectId ) {
                this->_projects[i]->deleteLater();
                this->_projects.removeAt(i);
                break;
            }
        }
    }

    bool allProjectsReady = true;
    for( int i=0 ; i<this->_projects.size() ; i++ ) {
        if( !this->_projects[i]->ready() ) {
            allProjectsReady = false;
        }
    }

    if( allProjectsReady ) {
        this->_ready = true;
        emit ready(this->_id, false);
    }
}

void Repository::parseUsers(QString xml)
{
    QDomDocument domDoc;
    domDoc.setContent(xml);
    QDomNodeList nl = domDoc.elementsByTagName("user");
    for( int i=0 ; i<nl.count() ; i++ ) {
        User *u = new User(this);
        u->setId(nl.at(i).toElement().elementsByTagName("id").at(0).toElement().text().toInt());
        u->setFirstName(nl.at(i).toElement().elementsByTagName("firstname").at(0).toElement().text());
        u->setLastName(nl.at(i).toElement().elementsByTagName("lastname").at(0).toElement().text());
        u->setMail(nl.at(i).toElement().elementsByTagName("mail").at(0).toElement().text());
        this->_users.append(u);
    }
}

void Repository::parseIssueStatuses(QString xml)
{
    QDomDocument domDoc;
    domDoc.setContent(xml);
    QDomNodeList nl = domDoc.elementsByTagName("issue_status");
    for( int i=0 ; i<nl.count() ; i++ ) {
        IssueStatus *is = new IssueStatus(this);
        is->setId(nl.at(i).toElement().elementsByTagName("id").at(0).toElement().text().toInt());
        is->setName(nl.at(i).toElement().elementsByTagName("name").at(0).toElement().text());
        this->_issueStatuses.append(is);
    }
}

void Repository::parseProjects(QString xml)
{
    QDomDocument domDoc;
    domDoc.setContent(xml);
    QDomNodeList nl = domDoc.elementsByTagName("project");
    for( int i=0 ; i<nl.count() ; i++ ) {
        Project *p = new Project(this);
        p->setId(nl.at(i).toElement().elementsByTagName("id").at(0).toElement().text().toInt());
        p->setName(nl.at(i).toElement().elementsByTagName("name").at(0).toElement().text());
        p->setIdentifier(nl.at(i).toElement().elementsByTagName("identifier").at(0).toElement().text());
        p->setDescription(nl.at(i).toElement().elementsByTagName("description").at(0).toElement().text());
        this->_projects.append(p);
    }
}

bool Repository::ready()
{
    return this->_ready;
}

User* Repository::user(int id)
{
    for( int i=0 ; i<this->_users.size() ; i++ ) {
        if( this->_users[i]->id() == id ) {
            return this->_users[i];
        }
    }

    // user not found, create a new one
    User *u = new User(this);
    u->setId(id);
    u->setFirstName(tr("nobody"));
    this->_users.append(u);
    return u;
}

IssueStatus* Repository::issueStatus(int id)
{
    for( int i=0 ; i<this->_issueStatuses.size() ; i++ ) {
        if( this->_issueStatuses[i]->id() == id ) {
            return this->_issueStatuses[i];
        }
    }

    // issue status not found, create a new one
    IssueStatus *is = new IssueStatus(this);
    is->setId(id);
    is->setName(tr("unknown"));
    this->_issueStatuses.append(is);
    return is;
}

QList<Project*> Repository::projects()
{
    return this->_projects;
}

/*** GETTER ***/

int Repository::id()
{
    return this->_id;
}

QString Repository::name()
{
    return this->_name;
}

QString Repository::server()
{
    return this->_server;
}

QString Repository::username()
{
    return this->_username;
}

QString Repository::password()
{
    return this->_password;
}


/*** SETTER ***/

void Repository::setId(int id)
{
    this->_id = id;
}

void Repository::setName(QString name)
{
    this->_name = name;
}

void Repository::setServer(QString server)
{
    this->_server = server;
}

void Repository::setUsername(QString username)
{
    this->_username = username;
}

void Repository::setPassword(QString password)
{
    this->_password = password;
}

}
