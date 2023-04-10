#ifndef SCENE_H
#define SCENE_H

class QGraphicsItem;
class diagram_item;

#include <QGraphicsScene>
#include <QpointF>

#include "main_window.h"

class diagram_scene : public QGraphicsScene
{
    Q_OBJECT

private:
    MainWindow* main_;
    QPointF ppp_;
    bool is_item_moving_;
    QGraphicsItem* moving_item_;
    QList<diagram_item*> drag_items_;

public:
    explicit diagram_scene(MainWindow* main, QObject *parent = nullptr);

public:
    void informItemPositionChanged(QString id, QPointF newPos);
    void informItemCreated(QString id, diagram_item* item);
    bool isItemMoving() { return is_item_moving_; };
    MainWindow* getMain() { return main_; };

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
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);

private:
    diagram_item* getDiagramItem(QString name);
};

#endif // SCENE_H
