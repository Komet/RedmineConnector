#ifndef MAINMODE_H
#define MAINMODE_H

#include <coreplugin/imode.h>

namespace RedmineConnector {

class MainMode : public Core::IMode
{
public:
    MainMode(QWidget *widget);
};

}

#endif // MAINMODE_H
