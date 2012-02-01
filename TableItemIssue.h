#ifndef TABLEITEMISSUE_H
#define TABLEITEMISSUE_H

#include <QWidget>

namespace Ui {
class TableItemIssue;
}

class TableItemIssue : public QWidget
{
    Q_OBJECT
    
public:
    explicit TableItemIssue(QWidget *parent = 0);
    ~TableItemIssue();

    void setSubject(QString subject);
    void setStatus(QString status);
    void setAssignedTo(QString assignedTo);
    void setUpdated(QString updated);
    void setIssueId(int id);
    
private:
    Ui::TableItemIssue *ui;
};

#endif // TABLEITEMISSUE_H
