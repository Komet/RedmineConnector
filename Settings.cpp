#include "Settings.h"
#include "RepositoryStruct.h"
#include "RedmineConnectorPlugin.h"
#include "RedmineConnectorConstants.h"

namespace RedmineConnector {

Settings* Settings::_instance = 0;

Settings::Settings(QObject *parent) :
    IOptionsPage(parent)
{
    _instance = this;
}

Settings::~Settings()
{
}

Settings* Settings::instance()
{
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
}

void Settings::finish()
{
}

QList<Repository> Settings::repositories()
{
    return this->_repositories;
}

}
