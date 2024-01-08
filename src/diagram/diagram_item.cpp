#include <QPainter>
#include "diagram_scene.h"
#include "diagram_item.h"
#include "../properties_item/properties_item.h"

using namespace CubeDiagram;

DiagramItem::DiagramItem(uint32_t propertiesId, QPixmap pixmap, QString name, QString fileName, QString includeName, QColor color, QGraphicsItem* parent):
    QGraphicsItem(parent)
{
    propertiesId_ = propertiesId;
    pixmap_ = pixmap;
    name_ = name;
    fileName_ = fileName;
    includeName_ = includeName;
    color_ = color;

    borderOnly_ = false;
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);

    font_ = QFont("Arial", 10);
    groupFont_ = QFont("Times", 10);
    iconRect_ = QRect(0, 0, 32, 32);

    QFontMetricsF fontMetrics(font_);
    textRect_ = fontMetrics.boundingRect(QRect(0, 0, 0, 0), Qt::AlignCenter | Qt::AlignHCenter, name_);
    textRect_.adjust(-1, 0, 1, 0);
    textRect_.translate(iconRect_.width() / 2, iconRect_.height() + textRect_.height() - 6);

    QFontMetricsF groupFontMetrics(groupFont_);
    includeTextRect_ = groupFontMetrics.boundingRect(QRect(0, 0, 0, 0), Qt::AlignCenter | Qt::AlignHCenter, includeName_);
    includeTextRect_.adjust(-1, 0, 1, 0);
    includeTextRect_.translate(iconRect_.width() / 2, -textRect_.height() + 6);

    // Adjust iconRect_ for colored frame
    boundingRect_ = iconRect_.adjusted(-2, -2, 2, 2).united(textRect_.toAlignedRect()).united(includeTextRect_.toAlignedRect());
}

DiagramItem::DiagramItem(const DiagramItem& other)
{
    propertiesId_ = other.propertiesId_;
    pixmap_ = QPixmap(other.pixmap_);
    name_ = other.name_;
    includeName_ = other.includeName_;
    color_ = QColor(other.color_);

    borderOnly_ = other.borderOnly_;
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);
    font_ = QFont(other.font_);
    groupFont_ = QFont(other.groupFont_);
    iconRect_ = other.iconRect_;
    textRect_ = other.textRect_;
    includeTextRect_ = other.includeTextRect_;
    boundingRect_ = other.boundingRect_;
    setPos(other.pos() + QPointF{0, 0});
    setZValue(other.zValue() - 1);
}

DiagramItem::~DiagramItem()
{
    qDebug() << "~diagram_item";
}

QRectF DiagramItem::boundingRect() const
{
    return boundingRect_;
}

QPainterPath DiagramItem::shape() const
{
    QPainterPath path;
    path.addRect(iconRect_.adjusted(-2, -2, 2, 2));
    return path;
}

void DiagramItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (borderOnly_)
    {
        qDebug() << "paint border " << name_;
        painter->fillRect(iconRect_, Qt::white);
        painter->setPen(Qt::black);
        painter->drawRect(iconRect_);
    }
    else
    {
        qDebug() << "paint full " << name_;
        if (scene() != nullptr)
        {
            DiagramScene* ds = reinterpret_cast<DiagramScene*>(scene());

            painter->setRenderHint(QPainter::Antialiasing);
            painter->drawPixmap(iconRect_, pixmap_);
            painter->setFont(font_);
            painter->setPen(Qt::blue);
            painter->drawText(textRect_, name_, Qt::AlignCenter | Qt::AlignHCenter);
            painter->setPen(QPen(QBrush(color_, Qt::SolidPattern), 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter->drawRect(iconRect_);

            QString groupName = includeName_;
            if (groupName != "<not selected>")
            {
                QColor colorGroup(color_);
                colorGroup.setAlpha(0xFF);
                painter->setFont(groupFont_);
                painter->setPen(colorGroup);
                painter->drawText(includeTextRect_, groupName, Qt::AlignCenter | Qt::AlignHCenter);
            }
        }

        if (this->isSelected())
        {
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

QVariant DiagramItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange)
    {
        if (scene() != nullptr)
        {
            DiagramScene* sc = qobject_cast<DiagramScene*>(scene());
            if (this->isSelected() && sc->IsItemMoving())
            {
                QPointF newPos = value.toPointF();

                // Grid
                int gridSize = 20;
                qreal xV = round(newPos.x() / gridSize) * gridSize;
                qreal yV = round(newPos.y() / gridSize) * gridSize;
                sc->InformItemPositionChanged(this);
                return QPointF(xV, yV);

            }
        }
    }
    else if (change == ItemSelectedChange || change == ItemSelectedHasChanged)
    {
        if (value.toInt() == 0)
        {
            qDebug() << value;
        }


    }
    return QGraphicsItem::itemChange(change, value);
}

void DiagramItem::InformIncludeChanged()
{
    QFontMetricsF groupFontMetrics(groupFont_);
    includeTextRect_ = groupFontMetrics.boundingRect(QRect(0, 0, 0, 0), Qt::AlignCenter | Qt::AlignHCenter, includeName_);
    includeTextRect_.adjust(-1, 0, 1, 0);
    includeTextRect_.translate(iconRect_.width() / 2, -textRect_.height() + 6);
    boundingRect_ = iconRect_.united(textRect_.toAlignedRect()).united(includeTextRect_.toAlignedRect());
    if (scene() != nullptr)
    {
        scene()->invalidate();
    }
}

void DiagramItem::InformNameChanged(QString name, QString oldName)
{
    QFontMetricsF fontMetrics(font_);
    textRect_ = fontMetrics.boundingRect(QRect(0, 0, 0, 0), Qt::AlignCenter | Qt::AlignHCenter, name);
    textRect_.adjust(-1, 0, 1, 0);
    textRect_.translate(iconRect_.width() / 2, iconRect_.height() + textRect_.height() - 6);
    boundingRect_ = iconRect_.united(textRect_.toAlignedRect()).united(includeTextRect_.toAlignedRect());
    if (scene() != nullptr)
    {
        scene()->invalidate();
    }
}

void DiagramItem::SetBorderOnly(bool borderOnly)
{
    qDebug() << "set border " << borderOnly << " " << name_;

    borderOnly_ = borderOnly;
    if (scene() != nullptr)
    {
        scene()->invalidate();
    }
}
