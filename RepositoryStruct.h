#ifndef REPOSITORYSTRUCT_H
#define REPOSITORYSTRUCT_H

#include <QString>

struct Repository {
    QString name;
    QString server;
    QString user;
    QString password;
    int savePassword;
};

#endif // REPOSITORYSTRUCT_H
