#include "diagram_item.h"

#include <QPainter>
#include <QDebug>

diagram_item::diagram_item(QPixmap pixmap, QGraphicsItem *parent)
    : QGraphicsItem(parent),
      pixmap_(pixmap)
{
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
}

QRectF diagram_item::boundingRect() const
{
    return pixmap_.rect();
}

void diagram_item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->drawPixmap(0, 0, pixmap_);
}

