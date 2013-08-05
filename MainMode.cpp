#include "MainMode.h"

namespace RedmineConnector {

MainMode::MainMode(QWidget *widget)
{
    setWidget(widget);
    setDisplayName(tr("Redmine"));
    setIcon(QIcon(QLatin1String(":/img/redmine_fluid_icon.png")));
    setPriority(0);
}

}
