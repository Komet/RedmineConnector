#ifndef ISSUEDETAILWIDGET_H
#define ISSUEDETAILWIDGET_H

#include <QDateTime>
#include <QWidget>

namespace Ui {
class IssueDetailWidget;
}

class IssueDetailWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit IssueDetailWidget(QWidget *parent = 0);
    ~IssueDetailWidget();

    void setAuthor(QString author);
    void setNotes(QString notes, QList<QString> details);
    void setDateTime(QDateTime dateTime);
    void setIsTopElement(bool top);
    
private:
    Ui::IssueDetailWidget *ui;
};

#endif // ISSUEDETAILWIDGET_H
