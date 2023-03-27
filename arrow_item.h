#pragma once

#include <QGraphicsItem>
#include <QObject>
#include <QPointer>

#include "unit_types.h"

#include <QGraphicsLineItem>

class diagram_item;

class properties_item;

QT_BEGIN_NAMESPACE
class QPixmap;
class QGraphicsSceneContextMenuEvent;
class QMenu;
class QPolygonF;
QT_END_NAMESPACE

class Arrow : public QGraphicsLineItem
{
public:
    enum { Type = UserType + 4 };

    Arrow(diagram_item*startItem, diagram_item*endItem,
          QGraphicsItem *parent = nullptr);

    int type() const override { return Type; }
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void setColor(const QColor &color) { myColor = color; }
    diagram_item* startItem() const { return myStartItem; }
    diagram_item* endItem() const { return myEndItem; }

    void updatePosition();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

private:
    diagram_item* myStartItem;
    diagram_item* myEndItem;
    QPolygonF arrowHead;
    QColor myColor = Qt::black;
};
