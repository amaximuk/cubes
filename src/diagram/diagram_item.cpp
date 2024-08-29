#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QGuiApplication>
#include <QCursor>
#include "diagram_scene.h"
#include "diagram_item.h"
#include "../properties/properties_item.h"

using namespace CubesDiagram;

DiagramItem::DiagramItem(CubesUnit::PropertiesId propertiesId, CubesDiagram::PropertiesForDrawing pfd, QGraphicsItem* parent):
    QGraphicsItem(parent)
{
    setAcceptHoverEvents(true);
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable |
        QGraphicsItem::ItemSendsGeometryChanges | QGraphicsItem::ItemIsFocusable);


    propertiesId_ = propertiesId;
    pfd_ = pfd;

    borderOnly_ = false;
    font_ = QFont("Arial", 10);
    groupFont_ = QFont("Times", 10);
    if (pfd_.itemType == ItemType::Text)
    {
        iconRect_ = QRect(0, 0, pfd_.size.width(), pfd.size.height());
    }
    else
        iconRect_ = QRect(0, 0, GridSize * 2, GridSize * 2);
    
    UpdateGeometry();
}

DiagramItem::DiagramItem(const DiagramItem& other)
{
    setAcceptHoverEvents(true);
    setFlags(other.flags());

    propertiesId_ = other.propertiesId_;
    pfd_ = other.pfd_;

    borderOnly_ = other.borderOnly_;
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
        qDebug() << "paint border " << pfd_.name;
        painter->fillRect(iconRect_, Qt::white);
        painter->setPen(Qt::black);
        painter->drawRect(iconRect_);
    }
    else
    {
        qDebug() << "paint full " << pfd_.name;
        if (scene() != nullptr)
        {
            DiagramScene* ds = reinterpret_cast<DiagramScene*>(scene());

            if (pfd_.itemType == ItemType::Text)
            {
                painter->setRenderHint(QPainter::Antialiasing);
                painter->setFont(font_);
                painter->setPen(Qt::black);
                painter->drawText(iconRect_, pfd_.text, Qt::AlignCenter | Qt::AlignHCenter);
            }
            else
            {
                painter->setRenderHint(QPainter::Antialiasing);
                painter->drawImage(iconRect_, pfd_.pixmap);
                painter->setFont(font_);
                painter->setPen(Qt::blue);
                painter->drawText(textRect_, pfd_.name, Qt::AlignCenter | Qt::AlignHCenter);
                painter->setPen(QPen(QBrush(pfd_.color, Qt::SolidPattern), 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                painter->drawRect(iconRect_);

                QString includeName = pfd_.includeName;
                if (includeName != "<not selected>")
                {
                    QColor colorGroup(pfd_.color);
                    colorGroup.setAlpha(0xFF);
                    painter->setFont(groupFont_);
                    painter->setPen(colorGroup);
                    painter->drawText(includeTextRect_, includeName, Qt::AlignCenter | Qt::AlignHCenter);
                }
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
            qDebug() << "selected: " << this->isSelected();
            qDebug() << "moving: " << sc->IsItemMoving();
            if (this->isSelected() && sc->IsItemMoving())
            {
                QPointF newPos = value.toPointF();

                // Grid
                int gridSize = GridSize;
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

bool isResizing = false;
bool isOnBorder = false;
QPointF startResizePos{};
QPointF startPos{};
QRect iconRect{};
Qt::CursorShape resizeCursor = Qt::ArrowCursor;
enum class SizingType
{
    letfTop,
    top,
    rightTop,
    right,
    rightBottom,
    bottom,
    leftBottom,
    left
};
SizingType sizingType = SizingType::letfTop;

void DiagramItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    qDebug() << "mousePressEvent";
    QGraphicsItem::mousePressEvent(event);

    if (isOnBorder)
    {
        isResizing = true;
        startResizePos = event->scenePos();
        startPos = pos();
        iconRect = iconRect_;
    }
}

void DiagramItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    qDebug() << "mouseMoveEvent " << event->pos();

    if (isResizing)
    {
        prepareGeometryChange();

        QPointF delta = event->scenePos() - startResizePos;
        if (resizeCursor == Qt::SizeHorCursor)
            delta.setY(0);
        else if (resizeCursor == Qt::SizeVerCursor)
            delta.setX(0);

        // Grid
        int gridSize = GridSize;
        qreal xV = round(delta.x() / gridSize) * gridSize;
        qreal yV = round(delta.y() / gridSize) * gridSize;
        delta = { xV, yV };

        if (sizingType == SizingType::rightBottom || sizingType == SizingType::right || sizingType == SizingType::bottom)
        {
            iconRect_ = iconRect.adjusted(0, 0, delta.x(), delta.y());

            if (sizingType != SizingType::bottom)
                iconRect_.setWidth(iconRect_.width() / gridSize * gridSize);
            if (sizingType != SizingType::right)
                iconRect_.setHeight(iconRect_.height() / gridSize * gridSize);

            if (iconRect_.width() < gridSize)
                iconRect_.setWidth(gridSize);
            if (iconRect_.height() < gridSize)
                iconRect_.setHeight(gridSize);
        }
        else if (sizingType == SizingType::letfTop || sizingType == SizingType::top || sizingType == SizingType::left)
        {
            iconRect_ = iconRect.adjusted(0, 0, -delta.x(), -delta.y());
            if (iconRect_.width() < gridSize)
                iconRect_.setWidth(iconRect.width() % gridSize + gridSize);
            if (iconRect_.height() < gridSize)
                iconRect_.setHeight(iconRect.height() % gridSize + gridSize);

            QPoint deltaPos{ iconRect.width() - iconRect_.width(), iconRect.height() - iconRect_.height() };
            QPointF pos{ startPos + deltaPos };

            setPos(pos);
        }
        else if (sizingType == SizingType::rightTop)
        {
            iconRect_ = iconRect.adjusted(0, 0, delta.x(), -delta.y());

            iconRect_.setWidth(iconRect_.width() / gridSize * gridSize);

            if (iconRect_.width() < gridSize)
                iconRect_.setWidth(gridSize);
            if (iconRect_.height() < gridSize)
                iconRect_.setHeight(iconRect.height() % gridSize + gridSize);

            QPoint deltaPos{ 0, iconRect.height() - iconRect_.height() };
            QPointF pos{ startPos + deltaPos };

            setPos(pos);
        }
        else if (sizingType == SizingType::leftBottom)
        {
            iconRect_ = iconRect.adjusted(0, 0, -delta.x(), delta.y());
        
            iconRect_.setHeight(iconRect_.height() / gridSize * gridSize);

            if (iconRect_.width() < gridSize)
                iconRect_.setWidth(iconRect.width() % gridSize + gridSize);
            if (iconRect_.height() < gridSize)
                iconRect_.setHeight(gridSize);

            QPoint deltaPos{ iconRect.width() - iconRect_.width(), 0 };
            QPointF pos{ startPos + deltaPos };

            setPos(pos);
        }

        UpdateGeometry();

        qDebug() << "DiagramItem::mouseMoveEvent : " << iconRect_;

        DiagramScene* sc = qobject_cast<DiagramScene*>(scene());
        sc->InformItemSizeChanged(this);
        scene()->invalidate();
    }
    else
    {
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void DiagramItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    qDebug() << "mouseReleaseEvent";
    QGraphicsItem::mouseReleaseEvent(event);

    isResizing = false;
}

void DiagramItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    qDebug() << "hoverEnterEvent";
    QGraphicsItem::hoverEnterEvent(event);

    //if (scene() != nullptr)
    //{
    //    if (isSelected() && scene()->selectedItems().size() == 1)
    //    {
    //        qDebug() << event->pos();
    //        qDebug() << iconRect_;
    //        qDebug() << iconRect_.adjusted(2, 2, -2, -2);
    //        if (iconRect_.contains(event->pos().toPoint()) &&
    //            !iconRect_.adjusted(2, 2, -2, -2).contains(event->pos().toPoint()))
    //        QGuiApplication::setOverrideCursor(Qt::SizeAllCursor);
    //    }
    //}
    //SizeVerCursor,
    //    SizeHorCursor,
    //    SizeBDiagCursor,
    //    SizeFDiagCursor,
    //    SizeAllCursor,
}

void DiagramItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    //qDebug() << "hoverMoveEvent";
    QGraphicsItem::hoverMoveEvent(event);

    if (scene() != nullptr)
    {
        if (isSelected() && scene()->selectedItems().size() == 1)
        {
            if (iconRect_.adjusted(-2, -2, 2, 2).contains(event->pos().toPoint()) &&
                !iconRect_.adjusted(2, 2, -2, -2).contains(event->pos().toPoint()))
            {
                isOnBorder = true;

                QRect leftTopRect(iconRect_.topLeft() - QPoint{ 2, 2 }, QSize{ 4, 4 });
                QRect rightTopRect(iconRect_.topRight() - QPoint{ 2, 2 }, QSize{ 4, 4 });
                QRect leftBottomRect(iconRect_.bottomLeft() - QPoint{ 2, 2 }, QSize{ 4, 4 });
                QRect rightBottomRect(iconRect_.bottomRight() - QPoint{ 2, 2 }, QSize{ 4, 4 });

                QRect leftRect(iconRect_.topLeft() - QPoint{ 2, -2 }, QSize{ 4, iconRect_.height() - 4 });
                QRect rightRect(iconRect_.topRight() - QPoint{ 2, -2 }, QSize{ 4, iconRect_.height() - 4 });
                QRect topRect(iconRect_.topLeft() - QPoint{ -2, 2 }, QSize{ iconRect_.width() - 4, 4 });
                QRect bottomRect(iconRect_.bottomLeft() - QPoint{ -2, 2 }, QSize{ iconRect_.width() - 4, 4 });



            // HOVER!!!!

            //if (iconRect_.adjusted(-4, -4, 4, 4).contains(event->pos().toPoint()) &&
            //    !iconRect_.contains(event->pos().toPoint()))
            //{
            //    isOnBorder = true;

            //    QRect leftTopRect(iconRect_.topLeft() + QPoint{ -4, -4 }, QSize{ 4, 4 });
            //    QRect rightTopRect(iconRect_.topRight() + QPoint{ 0, 4 }, QSize{ 4, 4 });
            //    QRect leftBottomRect(iconRect_.bottomLeft() + QPoint{ -4, 0 }, QSize{ 4, 4 });
            //    QRect rightBottomRect(iconRect_.bottomRight() + QPoint{ 0, 0 }, QSize{ 4, 4 });

            //    QRect leftRect(iconRect_.topLeft() + QPoint{ -4, 0 }, QSize{ 4, iconRect_.height() });
            //    QRect rightRect(iconRect_.topRight() + QPoint{ 0, 0 }, QSize{ 4, iconRect_.height() });
            //    QRect topRect(iconRect_.topLeft() + QPoint{ 0, -4 }, QSize{ iconRect_.width(), 4 });
            //    QRect bottomRect(iconRect_.bottomLeft() + QPoint{ 0, 0 }, QSize{ iconRect_.width(), 4 });

                if (leftTopRect.contains(event->pos().toPoint()))
                {
                    resizeCursor = Qt::SizeFDiagCursor;
                    sizingType = SizingType::letfTop;
                }
                else if (rightBottomRect.contains(event->pos().toPoint()))
                {
                    resizeCursor = Qt::SizeFDiagCursor;
                    sizingType = SizingType::rightBottom;
                }
                else if (rightTopRect.contains(event->pos().toPoint()))
                { 
                    resizeCursor = Qt::SizeBDiagCursor;
                    sizingType = SizingType::rightTop;
                }
                else if (leftBottomRect.contains(event->pos().toPoint()))
                {
                    resizeCursor = Qt::SizeBDiagCursor;
                    sizingType = SizingType::leftBottom;
                }
                else if (leftRect.contains(event->pos().toPoint()))
                {
                    resizeCursor = Qt::SizeHorCursor;
                    sizingType = SizingType::left;
                }
                else if (rightRect.contains(event->pos().toPoint()))
                {
                    resizeCursor = Qt::SizeHorCursor;
                    sizingType = SizingType::right;
                }
                else if (topRect.contains(event->pos().toPoint()))
                {
                    resizeCursor = Qt::SizeVerCursor;
                    sizingType = SizingType::top;
                }
                else if (bottomRect.contains(event->pos().toPoint()))
                {
                    resizeCursor = Qt::SizeVerCursor;
                    sizingType = SizingType::bottom;
                }

                QGuiApplication::setOverrideCursor(resizeCursor);
            }
            else
            {
                isOnBorder = false;
                QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
            }
        }
    }

}

void DiagramItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    qDebug() << "hoverLeaveEvent";
    QGraphicsItem::hoverLeaveEvent(event);

    QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
    isOnBorder = false;
}

void DiagramItem::keyPressEvent(QKeyEvent* keyEvent)
{
    // NOT WORK AS EXPECTED!!!!!!!!!!!!!!!!!!!
    // ITEM IS MOVING
    qDebug() << "keyPressEvent";
    if (keyEvent->key() == Qt::Key_Escape && isResizing)
    {
        isResizing = false;
        isOnBorder = false;

        prepareGeometryChange();

        iconRect_ = iconRect;
        setPos(startPos);

        UpdateGeometry();

        scene()->invalidate();
        QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
    }

    QGraphicsItem::keyPressEvent(keyEvent);
}

void DiagramItem::SetIncludeName(QString includeName)
{
    pfd_.includeName = includeName;

    QFontMetricsF groupFontMetrics(groupFont_);
    includeTextRect_ = groupFontMetrics.boundingRect(QRect(0, 0, 0, 0), Qt::AlignCenter | Qt::AlignHCenter, pfd_.includeName);
    includeTextRect_.adjust(-1, 0, 1, 0);
    includeTextRect_.translate(iconRect_.width() / 2, -textRect_.height() + 6);
    boundingRect_ = iconRect_.united(textRect_.toAlignedRect()).united(includeTextRect_.toAlignedRect());
    if (scene() != nullptr)
    {
        scene()->invalidate();
    }
}

void DiagramItem::SetName(QString name)
{
    pfd_.name = name;
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

void DiagramItem::SetColor(QColor color)
{
    pfd_.color = color;
    if (scene() != nullptr)
    {
        scene()->invalidate();
    }
}

void DiagramItem::SetBorderOnly(bool borderOnly)
{
    qDebug() << "set border " << borderOnly << " " << pfd_.name;

    borderOnly_ = borderOnly;
    if (scene() != nullptr)
    {
        scene()->invalidate();
    }
}

void DiagramItem::SetSize(QSizeF size)
{
    qDebug() << "DiagramItem::SetSize : " << size;

    prepareGeometryChange();

    iconRect_.setWidth(size.width());// / GridSize * GridSize
    iconRect_.setHeight(size.height());

    UpdateGeometry();
}

bool DiagramItem::IsResizing()
{
    return isResizing;
}

void DiagramItem::UpdateGeometry()
{
    QFontMetricsF fontMetrics(font_);
    textRect_ = fontMetrics.boundingRect(QRect(0, 0, 0, 0), Qt::AlignCenter | Qt::AlignHCenter, pfd_.name);
    textRect_.adjust(-1, 0, 1, 0);
    textRect_.translate(iconRect_.width() / 2, iconRect_.height() + textRect_.height() - 6);

    QFontMetricsF groupFontMetrics(groupFont_);
    includeTextRect_ = groupFontMetrics.boundingRect(QRect(0, 0, 0, 0), Qt::AlignCenter | Qt::AlignHCenter, pfd_.includeName);
    includeTextRect_.adjust(-1, 0, 1, 0);
    includeTextRect_.translate(iconRect_.width() / 2, -textRect_.height() + 6);

    // Adjust iconRect_ for colored frame
    boundingRect_ = iconRect_.adjusted(-2, -2, 2, 2).united(textRect_.toAlignedRect()).united(includeTextRect_.toAlignedRect());
}
