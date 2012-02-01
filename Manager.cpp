#include "Manager.h"

namespace RedmineConnector {

Manager *Manager::_instance = 0;

Manager::Manager(QObject *parent) :
    QObject(parent)
{
}

Manager* Manager::instance()
{
    if( _instance == 0 ) {
        _instance = new Manager();
    }
    return _instance;
}

void Manager::initialize()
{
    qDebug("Manager::initialize");

    for( int i=0 ; i<this->_repositories.size() ; i++ ) {
        delete this->_repositories[i];
    }
    this->_repositories.clear();

    QList<SettingsRepository> repositories = Settings::instance()->repositories();
    for( int i=0 ; i<repositories.size() ; i++ ) {
        Repository *r = new Repository(this);
        r->setId(i);
        r->setName(repositories[i].name);
        r->setServer(repositories[i].server);
        r->setUsername(repositories[i].user);
        r->setPassword(repositories[i].password);
        this->_repositories.append(r);
    }

    for( int i=0 ; i<this->_repositories.size() ; i++ ) {
        qDebug("Manager initialize Repository %d", i);
        this->_repositories[i]->initialize();
        connect(this->_repositories[i], SIGNAL(ready(int, bool)), this, SLOT(repositoryReady(int, bool)));
    }
}

void Manager::repositoryReady(int id, bool error)
{
    Q_UNUSED(id);
    qDebug("Manager::repositoryReady %d", id);

    if( error ) {
        for( int i=0 ; i<this->_repositories.size() ; i++ ) {
            if( this->_repositories[i]->id() == id ) {
                this->_repositories[i]->deleteLater();
                this->_repositories.removeAt(i);
                break;
            }
        }
    }

    bool allRepositoriesReady = true;
    for( int i=0 ; i<this->_repositories.size() ; i++ ) {
        if( !this->_repositories[i]->ready() ) {
            allRepositoriesReady = false;
        }
    }

    if( allRepositoriesReady ) {
        qDebug("  allRepositoriesReady");
        qDebug("    %d Repositories", this->_repositories.size());
        for( int i=0 ; i<this->_repositories.size() ; i++ ) {
            qDebug("      %d Projects", this->_repositories[i]->projects().size());
        }
        emit ready();
    }
}


/*** GETTER ***/

QList<Repository*> Manager::repositories()
{
    return this->_repositories;
}

}
