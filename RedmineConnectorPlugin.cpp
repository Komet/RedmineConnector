#include "RedmineConnectorPlugin.h"
#include "RedmineConnectorConstants.h"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/coreconstants.h>
#include <QtGui/QMainWindow>
#include <QtCore/QtPlugin>
#include "MainMode.h"
#include "MainWindow.h"
#include "Settings.h"

using namespace RedmineConnector::Internal;

RedmineConnectorPlugin* RedmineConnectorPlugin::_instance = 0;

RedmineConnectorPlugin::RedmineConnectorPlugin()
{
}

RedmineConnectorPlugin::~RedmineConnectorPlugin()
{
}

bool RedmineConnectorPlugin::initialize(const QStringList &arguments, QString *errorString)
{    
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    _instance = this;

    MainWindow *mainWindow = new MainWindow();

    Core::IMode *mainMode = new MainMode(mainWindow);
    addAutoReleasedObject(mainMode);

    //Core::IOptionsPage *settings = new Settings(this);
    Settings::instance()->setParent(this);
    addAutoReleasedObject(Settings::instance());

    return true;
}

void RedmineConnectorPlugin::extensionsInitialized()
{
}

ExtensionSystem::IPlugin::ShutdownFlag RedmineConnectorPlugin::aboutToShutdown()
{
    return SynchronousShutdown;
}

RedmineConnectorPlugin* RedmineConnectorPlugin::instance()
{
    return _instance;
}

Q_EXPORT_PLUGIN2(RedmineConnector, RedmineConnectorPlugin)

