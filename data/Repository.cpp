#include "Repository.h"

#include <QDebug>
#include <QDomDocument>

#include "RedmineConnectorConstants.h"
#include "NetworkAuthDialog.h"

namespace RedmineConnector {

Repository::Repository(QObject *parent) :
    QObject(parent)
{
    this->m_ready = false;
    this->m_queryRunning = false;
    this->m_issueStatusesReply = 0;
    this->m_projectsReply = 0;
    this->m_usersReply = 0;
    connect(&this->m_timeoutChecker, SIGNAL(timeout()), this, SLOT(checkForTimeouts()));
    this->m_timeoutChecker.start(2000);

    connect(&this->m_qnam, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), this, SLOT(authRequired(QNetworkReply*, QAuthenticator*)));
}

Repository::~Repository()
{
}

QNetworkAccessManager* Repository::qnam()
{
    return &this->m_qnam;
}

void Repository::cleanUp()
{
    this->m_ready = false;
    this->m_queryRunning = false;
    if( this->m_issueStatusesReply != 0 ) {
        this->m_issueStatusesReply->abort();
    }
    if( this->m_projectsReply != 0 ) {
        this->m_projectsReply->abort();
    }
    if( this->m_usersReply != 0 ) {
        this->m_usersReply->abort();
    }
    for( int i=0, n=this->m_projects.size() ; i<n ; i++ ) {
        delete this->m_projects[i];
    }
    this->m_projects.clear();
    for( int i=0, n=this->m_users.size() ; i<n ; i++ ) {
        delete this->m_users[i];
    }
    this->m_users.clear();
    for( int i=0, n=this->m_issueStatuses.size() ; i<n ; i++ ) {
        delete this->m_issueStatuses[i];
    }
    this->m_issueStatuses.clear();
    this->m_trackers.clear();
}

void Repository::checkForTimeouts()
{
    if( this->m_queryRunning && this->m_lastQueryStarted.secsTo(QDateTime::currentDateTime()) > Constants::NETWORK_TIMEOUT_VALUE ) {
        if( this->m_issueStatusesReply != 0 ) {
            this->m_issueStatusesReply->abort();
        }
        if( this->m_projectsReply != 0 ) {
            this->m_projectsReply->abort();
        }
        if( this->m_usersReply != 0 ) {
            this->m_usersReply->abort();
        }
    }
}

void Repository::initialize()
{
    this->cleanUp();

    QUrl usersUrl(this->server() + "/users.xml?limit=100");
    usersUrl.setUserName(this->username());
    usersUrl.setPassword(this->password());

    this->m_lastQueryStarted = QDateTime::currentDateTime();
    this->m_queryRunning = true;
    this->m_usersReply = this->m_qnam.get(QNetworkRequest(usersUrl));
    connect(this->m_usersReply, SIGNAL(finished()), this, SLOT(usersReadyRead()));
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
    this->m_queryRunning = false;

    if( this->m_usersReply->error() != QNetworkReply::NoError ) {
        this->cleanUp();
        emit ready(this->id(), true);
        return;
    }

    QString msg = this->m_usersReply->readAll();
    this->parseUsers(msg);

    QUrl issueStatusesUrl(this->server() + "/issue_statuses.xml");
    issueStatusesUrl.setUserName(this->username());
    issueStatusesUrl.setPassword(this->password());

    this->m_lastQueryStarted = QDateTime::currentDateTime();
    this->m_queryRunning = true;
    this->m_issueStatusesReply = this->m_qnam.get(QNetworkRequest(issueStatusesUrl));
    connect(this->m_issueStatusesReply, SIGNAL(finished()), this, SLOT(issueStatusesReadyRead()));
}

void Repository::issueStatusesReadyRead()
{
    this->m_queryRunning = false;

    if( this->m_issueStatusesReply->error() != QNetworkReply::NoError ) {
        this->cleanUp();
        emit ready(this->id(), true);
        return;
    }

    QString msg = this->m_issueStatusesReply->readAll();
    this->parseIssueStatuses(msg);

    QUrl trackersUrl(this->server() + "/trackers.xml");
    trackersUrl.setUserName(this->username());
    trackersUrl.setPassword(this->password());

    this->m_lastQueryStarted = QDateTime::currentDateTime();
    this->m_queryRunning = true;
    this->m_trackersReply = this->m_qnam.get(QNetworkRequest(trackersUrl));
    connect(this->m_trackersReply, SIGNAL(finished()), this, SLOT(trackersReadyRead()));
}

void Repository::trackersReadyRead()
{
    this->m_queryRunning = false;

    if( this->m_trackersReply->error() != QNetworkReply::NoError ) {
        this->cleanUp();
        emit ready(this->id(), true);
        return;
    }

    QString msg = this->m_trackersReply->readAll();
    this->parseTrackers(msg);

    QUrl projectsUrl(this->server() + "/projects.xml?limit=100");
    projectsUrl.setUserName(this->username());
    projectsUrl.setPassword(this->password());

    this->m_lastQueryStarted = QDateTime::currentDateTime();
    this->m_queryRunning = true;
    this->m_projectsReply = this->m_qnam.get(QNetworkRequest(projectsUrl));
    connect(this->m_projectsReply, SIGNAL(finished()), this, SLOT(projectsReadyRead()));
}

void Repository::projectsReadyRead()
{
    this->m_queryRunning = false;

    if( this->m_projectsReply->error() != QNetworkReply::NoError ) {
        this->cleanUp();
        emit ready(this->id(), true);
        return;
    }

    QString msg = this->m_projectsReply->readAll();
    this->parseProjects(msg);
    for( int i=0, n=this->m_projects.size() ; i<n ; i++ ) {
        this->m_projects[i]->initialize();
        connect(this->m_projects[i], SIGNAL(ready(int, bool)), this, SLOT(projectReady(int, bool)));
    }
    this->m_projectsReply = 0;
    this->m_issueStatusesReply = 0;
    this->m_usersReply = 0;
}

void Repository::projectReady(int projectId, bool error)
{
    if( error ) {
        for( int i=0, n=this->m_projects.size() ; i<n ; i++ ) {
            if( this->m_projects[i]->id() == projectId ) {
                this->m_projects[i]->deleteLater();
                this->m_projects.removeAt(i);
                break;
            }
        }
    }

    bool allProjectsReady = true;
    for( int i=0, n=this->m_projects.size() ; i<n ; i++ ) {
        if( !this->m_projects[i]->ready() ) {
            allProjectsReady = false;
        }
    }

    if( allProjectsReady ) {
        this->gatherPriorities();
        this->m_ready = true;
        emit ready(this->m_id, false);
    }
}

void Repository::parseUsers(QString xml)
{
    // create an empty user
    User *eU = new User(this);
    eU->setFirstName("");
    eU->setLastName("");
    eU->setMail("");
    eU->setId(0);
    this->m_users.append(eU);

    QDomDocument domDoc;
    domDoc.setContent(xml);
    QDomNodeList nl = domDoc.elementsByTagName("user");
    for( int i=0, n=nl.count() ; i<n ; i++ ) {
        User *u = new User(this);
        u->setId(nl.at(i).toElement().elementsByTagName("id").at(0).toElement().text().toInt());
        u->setFirstName(nl.at(i).toElement().elementsByTagName("firstname").at(0).toElement().text());
        u->setLastName(nl.at(i).toElement().elementsByTagName("lastname").at(0).toElement().text());
        u->setMail(nl.at(i).toElement().elementsByTagName("mail").at(0).toElement().text());
        this->m_users.append(u);
    }
}

void Repository::parseIssueStatuses(QString xml)
{
    QDomDocument domDoc;
    domDoc.setContent(xml);
    QDomNodeList nl = domDoc.elementsByTagName("issue_status");
    for( int i=0, n=nl.count() ; i<n ; i++ ) {
        IssueStatus *is = new IssueStatus(this);
        is->setId(nl.at(i).toElement().elementsByTagName("id").at(0).toElement().text().toInt());
        is->setName(nl.at(i).toElement().elementsByTagName("name").at(0).toElement().text());
        this->m_issueStatuses.append(is);
    }
}

void Repository::parseTrackers(QString xml)
{
    QDomDocument domDoc;
    domDoc.setContent(xml);
    QDomNodeList nl = domDoc.elementsByTagName("tracker");
    for( int i=0, n=nl.count() ; i<n ; i++ ) {
        Tracker t;
        t.id = nl.at(i).toElement().elementsByTagName("id").at(0).toElement().text().toInt();
        t.name = nl.at(i).toElement().elementsByTagName("name").at(0).toElement().text();
        this->m_trackers.append(t);
    }
}

void Repository::parseProjects(QString xml)
{
    QDomDocument domDoc;
    domDoc.setContent(xml);
    QDomNodeList nl = domDoc.elementsByTagName("project");
    for( int i=0, n=nl.count() ; i<n ; i++ ) {
        Project *p = new Project(this);
        p->setId(nl.at(i).toElement().elementsByTagName("id").at(0).toElement().text().toInt());
        p->setName(nl.at(i).toElement().elementsByTagName("name").at(0).toElement().text());
        p->setIdentifier(nl.at(i).toElement().elementsByTagName("identifier").at(0).toElement().text());
        p->setDescription(nl.at(i).toElement().elementsByTagName("description").at(0).toElement().text());
        this->m_projects.append(p);
    }
}

bool Repository::ready()
{
    return this->m_ready;
}

User* Repository::user(int id)
{
    for( int i=0, n=this->m_users.size() ; i<n ; i++ ) {
        if( this->m_users[i]->id() == id ) {
            return this->m_users[i];
        }
    }

    // user not found, create a new one
    User *u = new User(this);
    u->setId(id);
    u->setFirstName(tr(""));
    this->m_users.append(u);
    return u;
}

IssueStatus* Repository::issueStatus(int id)
{
    for( int i=0, n=this->m_issueStatuses.size() ; i<n ; i++ ) {
        if( this->m_issueStatuses[i]->id() == id ) {
            return this->m_issueStatuses[i];
        }
    }

    // issue status not found, create a new one
    IssueStatus *is = new IssueStatus(this);
    is->setId(id);
    is->setName(tr("unknown", "unknown issue status"));
    this->m_issueStatuses.append(is);
    return is;
}


Project* Repository::project(int index)
{
    if( index >= 0 && index < this->m_projects.size() ) {
        return this->m_projects.at(index);
    }

    return new Project(this);
}


Priority* Repository::getAndAddPriority(int id, QString name)
{
    for( int i=0, n=this->m_priorities.size() ; i<n ; i++ ) {
        if( this->m_priorities[i]->id() == id ) {
            return this->m_priorities[i];
        }
    }

    Priority *priority = new Priority(this);
    priority->setId(id);
    priority->setName(name);
    this->m_priorities.append(priority);

    return priority;
}

void Repository::gatherPriorities()
{
    for( int i=0, cP=this->projects().size() ; i<cP ; i++ ) {
        QList<Issue*> issues = this->projects().at(i)->issues();
        for( int n=0, cI=issues.size() ; n<cI ; n++ ) {
            Priority *p = this->getAndAddPriority(issues[n]->priorityId(), issues[n]->priorityName());
            issues[n]->setPriority(p);
        }
    }
}

/*** GETTER ***/

QList<Priority*> Repository::priorities()
{
    return this->m_priorities;
}

QList<User*> Repository::users()
{
    return this->m_users;
}

QList<IssueStatus*> Repository::issueStatuses()
{
    return this->m_issueStatuses;
}

QList<Tracker> Repository::trackers()
{
    return this->m_trackers;
}

Tracker Repository::tracker(int id)
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

QList<Project*> Repository::projects()
{
    return this->m_projects;
}

int Repository::id()
{
    return this->m_id;
}

QString Repository::name()
{
    return this->m_name;
}

QString Repository::server()
{
    return this->m_server;
}

QString Repository::username()
{
    return this->m_username;
}

QString Repository::password()
{
    return this->m_password;
}


/*** SETTER ***/

void Repository::setId(int id)
{
    this->m_id = id;
}

void Repository::setName(QString name)
{
    this->m_name = name;
}

void Repository::setServer(QString server)
{
    this->m_server = server;
}

void Repository::setUsername(QString username)
{
    this->m_username = username;
}

void Repository::setPassword(QString password)
{
    this->m_password = password;
}

}
