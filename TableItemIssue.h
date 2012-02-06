#ifndef TABLEITEMISSUE_H
#define TABLEITEMISSUE_H

#include <QDate>
#include <QWidget>

#include "data/Issue.h"

namespace Ui {
class TableItemIssue;
}

class TableItemIssue : public QWidget
{
    Q_OBJECT
    
public:
    explicit TableItemIssue(QWidget *parent = 0);
    ~TableItemIssue();

    void setIssue(RedmineConnector::Issue *issue);
    RedmineConnector::Issue* issue();
    void setIsCurrentItem(bool isCurrent);
    
private:
    Ui::TableItemIssue *ui;

    RedmineConnector::Issue* m_issue;
};

#endif // TABLEITEMISSUE_H
