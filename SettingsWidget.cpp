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

    this->_currentRepository = -1;
    this->setInputsEnabled(this->_repositories.size() > 0);
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
}


void SettingsWidget::addNewRepository()
{
    Repository repo;
    repo.name = tr("Neues Repository");
    repo.savePassword = Qt::Unchecked;
    this->_repositories.append(repo);
    this->setInputsEnabled(true);
    this->_currentRepository = this->_repositories.size()-1;
    QListWidgetItem *item = new QListWidgetItem(repo.name, ui->listWidget);
    ui->listWidget->setCurrentItem(item);
}

void SettingsWidget::deleteCurrentRepository()
{
    int num = ui->listWidget->currentRow();
    if( num < 0 || num > this->_repositories.size() -1 ) {
        return;
    }
    QListWidgetItem *item = ui->listWidget->takeItem(num);
    delete item;
    this->_repositories.removeAt(num);
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
    if( num < 0 || num >= this->_repositories.size() ) {
        ui->repoName->setText("");
        ui->repoServer->setText("");
        ui->repoUser->setText("");
        ui->repoPassword->setText("");
        ui->repoSavePassword->setChecked(false);
        this->setInputsEnabled(false);
        return;
    }

    this->_currentRepository = num;
    ui->repoName->setText(this->_repositories.at(num).name);
    ui->repoServer->setText(this->_repositories.at(num).server);
    ui->repoUser->setText(this->_repositories.at(num).user);
    ui->repoPassword->setText(this->_repositories.at(num).password);
    ui->repoSavePassword->setChecked(this->_repositories.at(num).savePassword == Qt::Checked);
}

void SettingsWidget::setCurrentRepoName(QString name)
{
    if( this->_currentRepository < 0 && this->_currentRepository >= this->_repositories.size() ) {
        return;
    }

    this->_repositories[this->_currentRepository].name = name;
    ui->listWidget->item(this->_currentRepository)->setText(name);
}

void SettingsWidget::setCurrentRepoServer(QString server)
{
    if( this->_currentRepository < 0 && this->_currentRepository >= this->_repositories.size() ) {
        return;
    }
    this->_repositories[this->_currentRepository].server = server;
}

void SettingsWidget::setCurrentRepoUser(QString user)
{
    if( this->_currentRepository < 0 && this->_currentRepository >= this->_repositories.size() ) {
        return;
    }
    this->_repositories[this->_currentRepository].user = user;
}

void SettingsWidget::setCurrentRepoPassword(QString password)
{
    if( this->_currentRepository < 0 && this->_currentRepository >= this->_repositories.size() ) {
        return;
    }
    this->_repositories[this->_currentRepository].password = password;
}

void SettingsWidget::setCurrentRepoSavePassword(int state)
{
    if( this->_currentRepository < 0 && this->_currentRepository >= this->_repositories.size() ) {
        return;
    }
    this->_repositories[this->_currentRepository].savePassword = state;
    if( state == Qt::Unchecked ) {
        ui->repoPassword->setText("");
    }
    ui->repoPassword->setEnabled(state != Qt::Unchecked);
}

QList<Repository> SettingsWidget::repositories()
{
    return this->_repositories;
}

void SettingsWidget::setRepositories(QList<Repository> repositories)
{
    this->_repositories = repositories;
    for( int i=0 ; i<this->_repositories.size() ; i++ ) {
        new QListWidgetItem(this->_repositories.at(i).name, ui->listWidget);
    }
    if( this->_repositories.size() > 0 ) {
        this->setInputsEnabled(true);
        ui->listWidget->setCurrentItem(ui->listWidget->item(0));
        this->_currentRepository = 0;
    }
}
