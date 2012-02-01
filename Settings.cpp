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
    _instance = this;
    if( QSettings *settings = Core::ICore::instance()->settings() ) {
        settings->beginGroup(Constants::SETTINGS_CATEGORY);
        int size = settings->beginReadArray("Repositories");
        for( int i=0 ; i<size ; i++ ) {
            SettingsRepository r;
            settings->setArrayIndex(i);
            r.name         = settings->value("Name").toString();
            r.server       = settings->value("Server").toString();
            r.user         = settings->value("User").toString();
            r.savePassword = settings->value("SavePassword").toInt();
            r.password     = settings->value("Password").toString();
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
    return "RedmineConnectorSettings";
}

QString Settings::displayName() const
{
    return tr("Redmine");
}

QString Settings::category() const
{
    return Constants::SETTINGS_CATEGORY;
}


QString Settings::displayCategory() const
{
    return tr("Redmine");
}

QIcon Settings::categoryIcon() const
{
    return QIcon(":/img/redmine_fluid_icon.png");
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
        settings->beginGroup(Constants::SETTINGS_CATEGORY);
        settings->beginWriteArray("Repositories");
        for( int i=0 ; i<this->_repositories.size() ; i++ ) {
            settings->setArrayIndex(i);
            settings->setValue("Name", this->_repositories.at(i).name);
            settings->setValue("Server", this->_repositories.at(i).server);
            settings->setValue("User", this->_repositories.at(i).user);
            settings->setValue("SavePassword", this->_repositories.at(i).savePassword);
            settings->setValue("Password", this->_repositories.at(i).password);
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
        settings->beginGroup(Constants::SETTINGS_CATEGORY);
        settings->setValue("MainWindowSplitterSizes", sizes);
        settings->endGroup();
    }
}

QByteArray Settings::mainWindowSplitterSizes()
{
    QByteArray sizes;
    if( QSettings *settings = Core::ICore::instance()->settings() ) {
        settings->beginGroup(Constants::SETTINGS_CATEGORY);
        sizes = settings->value("MainWindowSplitterSizes").toByteArray();
        settings->endGroup();
    }
    return sizes;
}

}
