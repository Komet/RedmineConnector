#ifndef SETTINGS_H
#define SETTINGS_H

#include <coreplugin/dialogs/ioptionspage.h>
#include <QIcon>
#include "SettingsWidget.h"

namespace RedmineConnector {

class Settings : public Core::IOptionsPage
{
    Q_OBJECT
public:
    Settings(QObject *parent);
    ~Settings();

    QString id() const;
    QString displayName() const;
    QString category() const;
    QString displayCategory() const;
    QWidget *createPage(QWidget *parent);
    QIcon categoryIcon() const;
    void apply();
    void finish();
    QList<Repository> repositories();
    static Settings* instance();

private:
    SettingsWidget *settingsWidget;
    QList<Repository> _repositories;
    static Settings *_instance;
};

} // namespace RedmineConnector

#endif // SETTINGS_H
