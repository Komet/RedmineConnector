#ifndef SETTINGS_H
#define SETTINGS_H

#include <coreplugin/dialogs/ioptionspage.h>
#include <QIcon>

#include "SettingsWidget.h"
#include "SettingsRepository.h"

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
    QString categoryIcon() const;
    void apply();
    void finish();
    QList<SettingsRepository> repositories();
    static Settings* instance();

    void setMainWindowSplitterSizes(QByteArray sizes);
    QByteArray mainWindowSplitterSizes();

private:
    SettingsWidget *settingsWidget;
    QList<SettingsRepository> _repositories;
    static Settings *_instance;
};

} // namespace RedmineConnector

#endif // SETTINGS_H
