#ifndef SETTINGSREPOSITORY_H
#define SETTINGSREPOSITORY_H

#include <QString>

namespace RedmineConnector {

struct SettingsRepository
{
    QString name;
    QString server;
    QString user;
    QString password;
    int savePassword;
};

}

#endif // SETTINGSREPOSITORY_H
