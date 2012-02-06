#ifndef TABLEITEMPROJECT_H
#define TABLEITEMPROJECT_H

#include <QWidget>

#include "data/Project.h"

namespace Ui {
class TableItemProject;
}

class TableItemProject : public QWidget
{
    Q_OBJECT
    
public:
    explicit TableItemProject(QWidget *parent = 0);
    ~TableItemProject();

    void setProject(RedmineConnector::Project *project);
    RedmineConnector::Project *project();
    void setIsCurrentItem(bool isCurrent);
    
private:
    Ui::TableItemProject *ui;

    RedmineConnector::Project *m_project;
};

#endif // TABLEITEMPROJECT_H
