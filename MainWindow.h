#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "Manager.h"

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
    void repositoryChanged(int index);
    void saveSplitterSizes();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
