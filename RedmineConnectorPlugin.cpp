#include "RedmineConnectorPlugin.h"
#include "RedmineConnectorConstants.h"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/coreconstants.h>

#include <QCoreApplication>
#include <QDebug>
#include <QLocale>
#include <QTranslator>
#include <QtCore/QtPlugin>
#include <QtCore/QLibraryInfo>
#include <QtGui/QMainWindow>

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

    const QString &locale = Core::ICore::instance()->userInterfaceLanguage();
    if (!locale.isEmpty()) {
        QTranslator *translator = new QTranslator(this);
        const QString &creatorTrPath = Core::ICore::instance()->resourcePath() + QLatin1String("/translations");
        const QString &qtTrPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
        const QString &trFile = QLatin1String("RedmineConnector_") + locale;
        if( translator->load(trFile, qtTrPath) || translator->load(trFile, creatorTrPath) ) {
            qApp->installTranslator(translator);
        }
    }

    _instance = this;

    MainWindow *mainWindow = new MainWindow();

    Core::IMode *mainMode = new MainMode(mainWindow);
    addAutoReleasedObject(mainMode);

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

