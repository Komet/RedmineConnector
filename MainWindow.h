#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSpacerItem>
#include <QWidget>

#include "data/Issue.h"
#include "data/Manager.h"
#include "IssueDetailWidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void reload();
    void repositoriesReady();
    void saveSplitterSizes();
    void issueListCellChanged(int row, int column, int previousRow, int previousColumn);
    void projectListCellChanged(int row, int column, int previousRow, int previousColumn);
    void showIssue(RedmineConnector::Issue *issue);
    void setCurrentIssueHasDueDate();
    void setCurrentIssueHasStartDate();
    void saveIssueChanges();
    void loadIssue(RedmineConnector::Issue *issue);

private:
    Ui::MainWindow *ui;

    QList<QWidget*> m_notes;
    QSpacerItem *m_notesSpacer;
    bool m_currentIssueHasDueDate;
    bool m_currentIssueHasStartDate;
    RedmineConnector::Issue *m_currentIssue;
};

#endif // MAINWINDOW_H
