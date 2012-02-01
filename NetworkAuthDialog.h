#ifndef NETWORKAUTHDIALOG_H
#define NETWORKAUTHDIALOG_H

#include <QDialog>

namespace Ui {
class NetworkAuthDialog;
}

class NetworkAuthDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NetworkAuthDialog(QWidget *parent = 0);
    ~NetworkAuthDialog();

    QString username();
    QString password();
    void setUsername(QString username);
    void setPassword(QString password);
    void setSitename(QString sitename);
    
private:
    Ui::NetworkAuthDialog *ui;
};

#endif // NETWORKAUTHDIALOG_H
