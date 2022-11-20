#include "diagram_item.h"

#include <QPainter>
#include <QDebug>

diagram_item::diagram_item(QPixmap pixmap, QString name, QGraphicsItem *parent):
    QGraphicsItem(parent),
    pixmap_(pixmap),
    name_(name)
{
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable |  QGraphicsItem::ItemSendsGeometryChanges);
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
    if(this->isSelected())
    {
        painter->drawRect(pixmap_.rect());
    }
}

QVariant diagram_item::itemChange(GraphicsItemChange change, const QVariant &value)
{
    qDebug() << name_ << " !!!!!!!!!!!shdbfgjkshgkdfgskjdghkjdsfhgkjsdfhgkjsdfhksjhfk";
    return QGraphicsItem::itemChange(change, value);
}
