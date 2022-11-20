#ifndef DIAGRAM_ITEM_H
#define DIAGRAM_ITEM_H

#include <QGraphicsItem>

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

public:
    enum { Type = UserType + 15 };

    diagram_item(QPixmap pixmap, QString name_, QGraphicsItem *parent = nullptr);
    int type() const override { return Type; }
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};

#endif // DIAGRAM_ITEM_H
