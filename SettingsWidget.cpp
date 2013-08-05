#include "SettingsWidget.h"
#include "ui_SettingsWidget.h"

SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);

    connect(ui->buttonAddRepo, SIGNAL(clicked()), this, SLOT(addNewRepository()));
    connect(ui->buttonDelete, SIGNAL(clicked()), this, SLOT(deleteCurrentRepository()));
    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(showRepository(int)));
    connect(ui->repoName, SIGNAL(textEdited(QString)), this, SLOT(setCurrentRepoName(QString)));
    connect(ui->repoServer, SIGNAL(textEdited(QString)), this, SLOT(setCurrentRepoServer(QString)));
    connect(ui->repoUser, SIGNAL(textEdited(QString)), this, SLOT(setCurrentRepoUser(QString)));
    connect(ui->repoPassword, SIGNAL(textChanged(QString)), this, SLOT(setCurrentRepoPassword(QString)));
    connect(ui->repoSavePassword, SIGNAL(stateChanged(int)), this, SLOT(setCurrentRepoSavePassword(int)));

    this->m_currentRepository = -1;
    this->setInputsEnabled(this->m_repositories.size() > 0);
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
}


void SettingsWidget::addNewRepository()
{
    RedmineConnector::SettingsRepository repo;
    repo.name = tr("New Repository");
    repo.savePassword = Qt::Unchecked;
    this->m_repositories.append(repo);
    this->setInputsEnabled(true);
    this->m_currentRepository = this->m_repositories.size()-1;
    QListWidgetItem *item = new QListWidgetItem(repo.name, ui->listWidget);
    ui->listWidget->setCurrentItem(item);
}

void SettingsWidget::deleteCurrentRepository()
{
    int num = ui->listWidget->currentRow();
    if( num < 0 || num > this->m_repositories.size() -1 ) {
        return;
    }
    QListWidgetItem *item = ui->listWidget->takeItem(num);
    delete item;
    this->m_repositories.removeAt(num);
    this->showRepository(ui->listWidget->currentRow());
}

void SettingsWidget::setInputsEnabled(bool enabled)
{
    ui->buttonDelete->setEnabled(enabled);
    ui->repoName->setEnabled(enabled);
    ui->repoServer->setEnabled(enabled);
    ui->repoUser->setEnabled(enabled);
    ui->repoSavePassword->setEnabled(enabled);
    ui->repoPassword->setEnabled(ui->repoSavePassword->isChecked());
}

void SettingsWidget::showRepository(int num)
{
    if( num < 0 || num >= this->m_repositories.size() ) {
        ui->repoName->setText(QString());
        ui->repoServer->setText(QString());
        ui->repoUser->setText(QString());
        ui->repoPassword->setText(QString());
        ui->repoSavePassword->setChecked(false);
        this->setInputsEnabled(false);
        return;
    }

    this->m_currentRepository = num;
    ui->repoName->setText(this->m_repositories.at(num).name);
    ui->repoServer->setText(this->m_repositories.at(num).server);
    ui->repoUser->setText(this->m_repositories.at(num).user);
    ui->repoPassword->setText(this->m_repositories.at(num).password);
    ui->repoSavePassword->setChecked(this->m_repositories.at(num).savePassword == Qt::Checked);
}

void SettingsWidget::setCurrentRepoName(QString name)
{
    if( this->m_currentRepository < 0 || this->m_currentRepository >= this->m_repositories.size() ) {
        return;
    }

    this->m_repositories[this->m_currentRepository].name = name;
    ui->listWidget->item(this->m_currentRepository)->setText(name);
}

void SettingsWidget::setCurrentRepoServer(QString server)
{
    if( this->m_currentRepository < 0 || this->m_currentRepository >= this->m_repositories.size() ) {
        return;
    }
    this->m_repositories[this->m_currentRepository].server = server;
}

void SettingsWidget::setCurrentRepoUser(QString user)
{
    if( this->m_currentRepository < 0 || this->m_currentRepository >= this->m_repositories.size() ) {
        return;
    }
    this->m_repositories[this->m_currentRepository].user = user;
}

void SettingsWidget::setCurrentRepoPassword(QString password)
{
    if( this->m_currentRepository < 0 || this->m_currentRepository >= this->m_repositories.size() ) {
        return;
    }
    this->m_repositories[this->m_currentRepository].password = password;
}

void SettingsWidget::setCurrentRepoSavePassword(int state)
{
    if( this->m_currentRepository < 0 || this->m_currentRepository >= this->m_repositories.size() ) {
        return;
    }
    this->m_repositories[this->m_currentRepository].savePassword = state;
    if( state == Qt::Unchecked ) {
        ui->repoPassword->setText(QString());
    }
    ui->repoPassword->setEnabled(state != Qt::Unchecked);
}

QList<RedmineConnector::SettingsRepository> SettingsWidget::repositories()
{
    return this->m_repositories;
}

void SettingsWidget::setRepositories(QList<RedmineConnector::SettingsRepository> repositories)
{
    this->m_repositories = repositories;
    for( int i=0 ; i<this->m_repositories.size() ; i++ ) {
        new QListWidgetItem(this->m_repositories.at(i).name, ui->listWidget);
    }
    if( this->m_repositories.size() > 0 ) {
        this->setInputsEnabled(true);
        ui->listWidget->setCurrentItem(ui->listWidget->item(0));
        this->m_currentRepository = 0;
    }
}
