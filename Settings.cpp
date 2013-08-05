#include <QSettings>
#include <coreplugin/icore.h>

#include "Settings.h"
#include "RedmineConnectorPlugin.h"
#include "RedmineConnectorConstants.h"

namespace RedmineConnector {

Settings* Settings::_instance = 0;

Settings::Settings(QObject *parent) :
    IOptionsPage(parent)
{
  setDisplayCategory(tr("Redmine Connector"));
  setCategoryIcon(categoryIcon());
  setDisplayName(tr("General"));

    _instance = this;
    if( QSettings *settings = Core::ICore::instance()->settings() ) {
        settings->beginGroup(QString::fromLatin1(Constants::SETTINGS_CATEGORY));
        int size = settings->beginReadArray(QLatin1String("Repositories"));
        for( int i=0 ; i<size ; i++ ) {
            SettingsRepository r;
            settings->setArrayIndex(i);
            r.name         = settings->value(QLatin1String("Name")).toString();
            r.server       = settings->value(QLatin1String("Server")).toString();
            r.user         = settings->value(QLatin1String("User")).toString();
            r.savePassword = settings->value(QLatin1String("SavePassword")).toInt();
            r.password     = settings->value(QLatin1String("Password")).toString();
            this->_repositories.append(r);
        }
        settings->endArray();
        settings->endGroup();
    }
}

Settings::~Settings()
{
}

Settings* Settings::instance()
{
    if( _instance == 0 ) {
        _instance = new Settings(0);
    }
    return _instance;
}

QString Settings::id() const
{
    return QLatin1String("RedmineConnectorSettings");
}

QString Settings::displayName() const
{
    return tr("Redmine");
}

QString Settings::category() const
{
    return QLatin1String(Constants::SETTINGS_CATEGORY);
}


QString Settings::displayCategory() const
{
    return tr("Redmine");
}

QString Settings::categoryIcon() const
{
    return QLatin1String(":/img/redmine_fluid_icon.png");
}

QWidget* Settings::createPage(QWidget *parent)
{
    this->settingsWidget = new SettingsWidget(parent);
    this->settingsWidget->setRepositories(this->repositories());
    return this->settingsWidget;
}

void Settings::apply()
{
    this->_repositories = this->settingsWidget->repositories();
    if( QSettings *settings = Core::ICore::instance()->settings() ) {
        settings->beginGroup(QLatin1String(Constants::SETTINGS_CATEGORY));
        settings->beginWriteArray(QLatin1String("Repositories"));
        for( int i=0 ; i<this->_repositories.size() ; i++ ) {
            settings->setArrayIndex(i);
            settings->setValue(QLatin1String("Name"), this->_repositories.at(i).name);
            settings->setValue(QLatin1String("Server"), this->_repositories.at(i).server);
            settings->setValue(QLatin1String("User"), this->_repositories.at(i).user);
            settings->setValue(QLatin1String("SavePassword"), this->_repositories.at(i).savePassword);
            settings->setValue(QLatin1String("Password"), this->_repositories.at(i).password);
        }
        settings->endArray();
        settings->endGroup();
    }
}

void Settings::finish()
{
}

QList<SettingsRepository> Settings::repositories()
{
    return this->_repositories;
}

void Settings::setMainWindowSplitterSizes(QByteArray sizes)
{
    if( QSettings *settings = Core::ICore::instance()->settings() ) {
        settings->beginGroup(QLatin1String(Constants::SETTINGS_CATEGORY));
        settings->setValue(QLatin1String("MainWindowSplitterSizes"), sizes);
        settings->endGroup();
    }
}

QByteArray Settings::mainWindowSplitterSizes()
{
    QByteArray sizes;
    if( QSettings *settings = Core::ICore::instance()->settings() ) {
        settings->beginGroup(QLatin1String(Constants::SETTINGS_CATEGORY));
        sizes = settings->value(QLatin1String("MainWindowSplitterSizes")).toByteArray();
        settings->endGroup();
    }
    return sizes;
}

}
