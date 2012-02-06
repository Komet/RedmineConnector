#include "Manager.h"

namespace RedmineConnector {

Manager *Manager::m_instance = 0;

Manager::Manager(QObject *parent) :
    QObject(parent)
{
}

Manager* Manager::instance()
{
    if( m_instance == 0 ) {
        m_instance = new Manager();
    }
    return m_instance;
}

void Manager::initialize()
{
    for( int i=0, n=this->m_repositories.size() ; i<n ; i++ ) {
        delete this->m_repositories[i];
    }
    this->m_repositories.clear();

    QList<SettingsRepository> repositories = Settings::instance()->repositories();
    for( int i=0, n=repositories.size() ; i<n ; i++ ) {
        Repository *r = new Repository(this);
        r->setId(i);
        r->setName(repositories[i].name);
        r->setServer(repositories[i].server);
        r->setUsername(repositories[i].user);
        r->setPassword(repositories[i].password);
        this->m_repositories.append(r);
    }

    for( int i=0 ; i<this->m_repositories.size() ; i++ ) {
        this->m_repositories[i]->initialize();
        connect(this->m_repositories[i], SIGNAL(ready(int, bool)), this, SLOT(repositoryReady(int, bool)));
    }
}

void Manager::repositoryReady(int id, bool error)
{
    Q_UNUSED(id);

    if( error ) {
        for( int i=0, n=this->m_repositories.size() ; i<n ; i++ ) {
            if( this->m_repositories[i]->id() == id ) {
                this->m_repositories[i]->deleteLater();
                this->m_repositories.removeAt(i);
                break;
            }
        }
    }

    bool allRepositoriesReady = true;
    for( int i=0, n=this->m_repositories.size() ; i<n ; i++ ) {
        if( !this->m_repositories[i]->ready() ) {
            allRepositoriesReady = false;
        }
    }

    if( allRepositoriesReady ) {
        emit ready();
    }
}


/*** GETTER ***/

QList<Repository*> Manager::repositories()
{
    return this->m_repositories;
}

Repository* Manager::repository(int index)
{
    if( index >= 0 && index < this->m_repositories.size() ) {
        return this->m_repositories.at(index);
    }

    return new Repository(this);
}

}
