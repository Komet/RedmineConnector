#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>

#include "Repository.h"
#include "Settings.h"
#include "SettingsRepository.h"

namespace RedmineConnector {

class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = 0);
    static Manager* instance();

    void initialize();
    QList<Repository*> repositories();
    Repository* repository(int index);
    
signals:
    void ready();

public slots:

private slots:
    void repositoryReady(int id, bool error);

private:
    static Manager *m_instance;

    QList<Repository*> m_repositories;
};

}

#endif // MANAGER_H
