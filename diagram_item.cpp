#include <QPainter>
#include <QDebug>

#include "diagram_scene.h"
#include "diagram_item.h"
#include "properties_item.h"

diagram_item::diagram_item(unit_types::UnitParameters unitParameters, QGraphicsItem *parent):
    QGraphicsItem(parent)
{
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);
    properties_.reset(new properties_item(unitParameters, this));
    pixmap_ = properties_->GetPixmap();

    font_ = QFont("Arial", 10);
    iconRect_ = QRect(0, 0, 32, 32);
    QFontMetricsF fontMetrics(font_);
    textRect_ = fontMetrics.boundingRect(properties_->getName());
    textRect_.translate(iconRect_.width() / 2 - textRect_.width() / 2, iconRect_.height() + textRect_.height());

    boundingRect_ = iconRect_.united(textRect_.toAlignedRect());
}

QRectF diagram_item::boundingRect() const
{
    return boundingRect_;
}

void diagram_item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->drawPixmap(iconRect_, pixmap_);
    painter->setFont(font_);
    painter->drawText(textRect_, properties_->getName());

    if(this->isSelected())
    {
        painter->drawRect(iconRect_);
    }
}

QVariant diagram_item::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange)
    {
        diagram_scene* sc = qobject_cast<diagram_scene*>(scene());
        if (this->isSelected() && sc->isItemMoving())
        {
            QPointF newPos = value.toPointF();
            QRectF rect = scene()->sceneRect();
            rect.setBottomRight({rect.bottom() - iconRect_.width(), rect.right() - iconRect_.height()});
            if (!rect.contains(newPos)) {
                // Keep the item inside the scene rect.
                newPos.setX(qMin(rect.right(), qMax(newPos.x(), rect.left())));
                newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top())));
                properties_->PositionChanged(newPos);
                return newPos;
            }
            properties_->PositionChanged(newPos);
        }


 
        //diagram_scene* ds = qobject_cast<diagram_scene*>(this->scene());
        //ds->informItemPositionChanged(name_, value.toPointF());
    }
    return QGraphicsItem::itemChange(change, value);
}
//
//QVariant diagram_item::itemChange(GraphicsItemChange change, const QVariant& value)
//{
//    if (change == ItemPositionChange && scene()) {
//        // value is the new position.
//        QPointF newPos = value.toPointF();
//        QRectF rect = scene()->sceneRect();
//        if (!rect.contains(newPos)) {
//            // Keep the item inside the scene rect.
//            newPos.setX(qMin(rect.right(), qMax(newPos.x(), rect.left())));
//            newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top())));
//            return newPos;
//        }
//    }
//    return QGraphicsItem::itemChange(change, value);
//}

void diagram_item::InformPositionXChanged(double x)
{
    diagram_scene* sc = qobject_cast<diagram_scene*>(scene());
    if (this->isSelected() && !sc->isItemMoving())
        setX(x);
}

void diagram_item::InformPositionYChanged(double y)
{
    diagram_scene* sc = qobject_cast<diagram_scene*>(scene());
    if (this->isSelected() && !sc->isItemMoving())
        setY(y);
}
