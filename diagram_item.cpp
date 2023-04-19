#include <QPainter>
#include <QDebug>

#include "diagram_scene.h"
#include "diagram_item.h"
#include "properties_item.h"

diagram_item::diagram_item(unit_types::UnitParameters unitParameters, QGraphicsItem *parent):
    QGraphicsItem(parent)
{
    borderOnly_ = false;
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);
    properties_.reset(new properties_item(unitParameters, this));
    pixmap_ = properties_->GetPixmap();

    font_ = QFont("Arial", 10);
    iconRect_ = QRect(0, 0, 32, 32);
    QFontMetricsF fontMetrics(font_);
    //textRect_ = fontMetrics.boundingRect(properties_->GetName());
    QString x = properties_->GetName();
    textRect_ = fontMetrics.boundingRect(QRect(0, 0, 0, 0), Qt::AlignCenter | Qt::AlignHCenter, properties_->GetName());
    textRect_.adjust(-1, 0, 1, 0);
    textRect_.translate(iconRect_.width() / 2, iconRect_.height() + textRect_.height());
    //textRect_.translate(iconRect_.width() / 2 - textRect_.width() / 2, iconRect_.height() + textRect_.height());

    // Adjust iconRect_ for colored frame
    boundingRect_ = iconRect_.adjusted(-2, -2, 2, 2).united(textRect_.toAlignedRect());
}

diagram_item::diagram_item(const diagram_item& other)
{
    borderOnly_ = other.borderOnly_;
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);
    pixmap_ = QPixmap(other.pixmap_);
    properties_.reset(new properties_item(*other.properties_, this));
    font_ = QFont(other.font_);
    iconRect_ = other.iconRect_;
    textRect_ = other.textRect_;
    boundingRect_ = other.boundingRect_;
    setPos(other.pos() + QPointF{0, 0});
    setZValue(other.zValue() - 1);
}

QRectF diagram_item::boundingRect() const
{
    return boundingRect_;
}

void diagram_item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (borderOnly_)
    {
        painter->fillRect(iconRect_, Qt::white);
        painter->setPen(Qt::black);
        painter->drawRect(iconRect_);
    }
    else
    {
        painter->drawPixmap(iconRect_, pixmap_);
        painter->setFont(font_);
        painter->setPen(Qt::blue);
        painter->drawText(textRect_, properties_->GetName());

        if (scene() != nullptr)
        {
            QColor c = QColor("Black");
            diagram_scene* ds = reinterpret_cast<diagram_scene*>(scene());
            if (ds != nullptr)
                c = reinterpret_cast<MainWindow*>(ds->getMain())->GetFileColor(properties_->GetFileName());
            //c.setAlpha(0x20);
            painter->setPen(QPen(QBrush(c, Qt::SolidPattern), 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter->setRenderHint(QPainter::Antialiasing);
            painter->drawRect(iconRect_);
        }

        if (this->isSelected())
        {
            //painter->setPen(QPen(QBrush(Qt::white, Qt::SolidPattern), 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            //painter->drawRect(iconRect_);
            //painter->setPen(QPen(QBrush(Qt::black, Qt::SolidPattern), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter->setPen(Qt::black);
            painter->drawRect(iconRect_);
        }

        //QPointF centerPoint(iconRect_.bottomRight());
        //qreal centerRadius = 8;

        //QRadialGradient radialGrad(centerPoint, centerRadius);
        //radialGrad.setColorAt(0.000, QColor(0, 0, 0, 0.9 * 255));
        //radialGrad.setColorAt(1.000, QColor(0, 0, 0, 0.000));

        //QPen pen;
        //pen.setWidth(16);
        //pen.setColor("blue");
        //pen.setBrush(radialGrad);

        //painter->setPen(pen);
        //painter->drawPoint(centerPoint);
    }
}

QVariant diagram_item::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange)
    {
        if (scene() != nullptr)
        {
            diagram_scene* sc = qobject_cast<diagram_scene*>(scene());
            if (this->isSelected() && sc->isItemMoving())
            {
                QPointF newPos = value.toPointF();

                //QRectF rect = scene()->sceneRect();
                //rect.setBottomRight({rect.bottom() - iconRect_.width(), rect.right() - iconRect_.height()});
                //if (!rect.contains(newPos)) {
                //    // Keep the item inside the scene rect.
                //    newPos.setX(qMin(rect.right(), qMax(newPos.x(), rect.left())));
                //    newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top())));
                //    properties_->PositionChanged(newPos);
                //    return newPos;
                //}
                //properties_->PositionChanged(newPos);

                // Grid
                int gridSize = 20;
                qreal xV = round(newPos.x() / gridSize) * gridSize;
                qreal yV = round(newPos.y() / gridSize) * gridSize;
                properties_->PositionChanged(QPointF(xV, yV));
                return QPointF(xV, yV);

            }
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
    if (scene() != nullptr)
    {
        diagram_scene* sc = qobject_cast<diagram_scene*>(scene());
        if (this->isSelected() && sc != nullptr && !sc->isItemMoving())
            setX(x);
    }
}

void diagram_item::InformPositionYChanged(double y)
{
    if (scene() != nullptr)
    {
        diagram_scene* sc = qobject_cast<diagram_scene*>(scene());
        if (this->isSelected() && sc != nullptr && !sc->isItemMoving())
            setY(y);
    }
}

void diagram_item::InformPositionZChanged(double z)
{
    if (scene() != nullptr)
    {
        diagram_scene* sc = qobject_cast<diagram_scene*>(scene());
        if (this->isSelected() && sc != nullptr && !sc->isItemMoving())
            setZValue(z);
    }
}

void diagram_item::InformFileChanged()
{
    if (scene() != nullptr)
    {
        reinterpret_cast<diagram_scene*>(scene())->informItemFileChanged(this);
        scene()->invalidate(mapRectToScene(iconRect_.adjusted(-5, -5, 5, 5)));
    }
}

void diagram_item::InformNameChanged(QString name, QString oldName)
{
    QFontMetricsF fontMetrics(font_);
    //textRect_ = fontMetrics.boundingRect(name);
    textRect_ = fontMetrics.boundingRect(QRect(0, 0, 0, 0), Qt::AlignCenter | Qt::AlignHCenter, name);
    textRect_.adjust(-1, 0, 1, 0);
    textRect_.translate(iconRect_.width() / 2, iconRect_.height() + textRect_.height());
    //textRect_.translate(iconRect_.width() / 2 - textRect_.width() / 2, iconRect_.height() + textRect_.height());
    boundingRect_ = iconRect_.united(textRect_.toAlignedRect());
    if (scene() != nullptr)
    {
        reinterpret_cast<diagram_scene*>(scene())->informItemNameChanged(this, oldName);
        scene()->invalidate(mapRectToScene(textRect_));
    }
}

void diagram_item::SetBorderOnly(bool borderOnly)
{
    borderOnly_ = borderOnly;
    if (scene() != nullptr)
        scene()->invalidate(mapRectToScene(boundingRect_));
}
