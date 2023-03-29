#ifndef SCENE_H
#define SCENE_H

class QGraphicsItem;
class diagram_item;

#include <QGraphicsScene>
#include <QpointF>

class diagram_scene : public QGraphicsScene
{
    Q_OBJECT

private:
    QPointF ppp_;
    bool is_item_moving_;
    QGraphicsItem* moving_item_;

public:
    explicit diagram_scene(QObject *parent = nullptr);

public:
    void informItemPositionChanged(QString id, QPointF newPos);
    void informItemCreated(QString id, diagram_item* item);
    bool isItemMoving() { return is_item_moving_; };

signals:
     void xxx(QPointF ppp);
     void itemPositionChanged(QString id, QPointF newPos);
     void itemCreated(QString id, diagram_item* item);

protected slots:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void keyPressEvent(QKeyEvent* keyEvent);
    void keyReleaseEvent(QKeyEvent* keyEvent);
    void drawBackground(QPainter* painter, const QRectF& rect);

private:
    diagram_item* getDiagramItem(QString name);
};

#endif // SCENE_H
