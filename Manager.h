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
    
signals:
    void ready();

public slots:

private slots:
    void repositoryReady(int id, bool error);

private:
    static Manager *_instance;

    QList<Repository*> _repositories;
};

}

#endif // MANAGER_H
