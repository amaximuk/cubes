#ifndef DIAGRAM_ITEM_H
#define DIAGRAM_ITEM_H

#include <QGraphicsItem>
#include <QObject>
#include <QPointer>

#include "unit_types.h"

class properties_item;

QT_BEGIN_NAMESPACE
class QPixmap;
class QGraphicsSceneContextMenuEvent;
class QMenu;
class QPolygonF;
QT_END_NAMESPACE

class diagram_item : public QGraphicsItem
{
private:
    QPixmap pixmap_;
    QString name_;
    QSharedPointer<properties_item> properties_;
    QFont font_;
    QRect iconRect_;
    QRectF textRect_;
    QRectF boundingRect_;

public:
    enum { Type = UserType + 15 };

    diagram_item(unit_types::UnitParameters unitParameters, QGraphicsItem *parent = nullptr);
    int type() const override { return Type; }
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    QString getName() { return name_; };
    QSharedPointer<properties_item> getProperties() { return properties_; };

public:
    void InformPositionXChanged(double x);
    void InformPositionYChanged(double y);
};

#endif // DIAGRAM_ITEM_H
