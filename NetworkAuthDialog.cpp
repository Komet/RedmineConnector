#include "NetworkAuthDialog.h"
#include "ui_NetworkAuthDialog.h"


NetworkAuthDialog::NetworkAuthDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetworkAuthDialog)
{
    ui->setupUi(this);
}

NetworkAuthDialog::~NetworkAuthDialog()
{
    delete ui;
}


void NetworkAuthDialog::setUsername(QString username)
{
    ui->username->setText(username);
}

void NetworkAuthDialog::setPassword(QString password)
{
    ui->password->setText(password);
}

void NetworkAuthDialog::setSitename(QString sitename)
{
    ui->labelSite->setText(sitename);
}

QString NetworkAuthDialog::username()
{
    return ui->username->text();
}

QString NetworkAuthDialog::password()
{
    return ui->password->text();
}
