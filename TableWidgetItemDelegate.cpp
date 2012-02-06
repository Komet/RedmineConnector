#include "TableWidgetItemDelegate.h"

#include <QBrush>
#include <QDebug>

TableWidgetItemDelegate::TableWidgetItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void TableWidgetItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    QStyledItemDelegate::paint(painter, option, index);

    painter->save();

    // painter->setPen(QColor(0, 0, 0, 80));
    // painter->drawLine(option.rect.left()+3, option.rect.bottom(), option.rect.right()-3, option.rect.bottom());
/*
    if( option.state & QStyle::State_Selected ) {
        painter->setPen(QColor(0, 0, 0, 0));
        painter->setBrush(QBrush(QColor(0, 0, 0, 20)));
        painter->drawRect(option.rect.left()+2, option.rect.top()+2, option.rect.width()-5, option.rect.height()-5);
    }
*/

    painter->restore();

    QStyledItemDelegate::paint(painter, option, index);
}
