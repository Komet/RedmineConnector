#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QList>
#include "SettingsRepository.h"

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit SettingsWidget(QWidget *parent = 0);
    ~SettingsWidget();

    QList<RedmineConnector::SettingsRepository> repositories();
    void setRepositories(QList<RedmineConnector::SettingsRepository> repositories);
    
private slots:
    void addNewRepository();
    void deleteCurrentRepository();
    void showRepository(int num);
    void setCurrentRepoName(QString name);
    void setCurrentRepoServer(QString server);
    void setCurrentRepoUser(QString user);
    void setCurrentRepoPassword(QString password);
    void setCurrentRepoSavePassword(int state);

private:
    Ui::SettingsWidget *ui;

    QList<RedmineConnector::SettingsRepository> _repositories;
    int _currentRepository;

    void setInputsEnabled(bool enabled);
};

#endif // SETTINGSWIDGET_H
