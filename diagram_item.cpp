#include "diagram_scene.h"
#include "diagram_item.h"

#include <QPainter>
#include <QDebug>

diagram_item::diagram_item(MainWindow::UnitParameters unitParameters, QGraphicsItem *parent):
    QGraphicsItem(parent)
{
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable |  QGraphicsItem::ItemSendsGeometryChanges);
    properties_ = new properties_item(unitParameters);
    pixmap_ = properties_->GetPixmap();
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
    if (change == ItemPositionChange)
    {
        diagram_scene* ds = qobject_cast<diagram_scene*>(this->scene());
        ds->informItemPositionChanged(name_, value.toPointF());
    }
    return QGraphicsItem::itemChange(change, value);
}
