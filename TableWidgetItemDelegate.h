#ifndef TABLEWIDGETITEMDELEGATE_H
#define TABLEWIDGETITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>

class TableWidgetItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TableWidgetItemDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;

signals:
    
public slots:
    
};

#endif // TABLEWIDGETITEMDELEGATE_H
