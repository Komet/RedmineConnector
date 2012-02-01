#include "TableWidgetItemDelegate.h"

TableWidgetItemDelegate::TableWidgetItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void TableWidgetItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    painter->save();
    painter->setPen(QColor(0, 0, 0, 80));
    painter->drawLine(option.rect.left()+3, option.rect.bottom(), option.rect.right()-3, option.rect.bottom());
    painter->restore();
    this->QStyledItemDelegate::paint(painter, option, index);
}
