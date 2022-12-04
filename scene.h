#ifndef SCENE_H
#define SCENE_H

class QGraphicsItem;

#include <QGraphicsScene>
#include <QpointF>

class scene : public QGraphicsScene
{
    Q_OBJECT

private:
    QPointF ppp_;
    bool is_item_moving_;
    QGraphicsItem* moving_item_;

public:
    explicit scene(QObject *parent = nullptr);

signals:
     void xxx(QPointF ppp);

protected slots:
    void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
    void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );
    void keyPressEvent(QKeyEvent *keyEvent);
    void keyReleaseEvent(QKeyEvent *keyEvent);
};

#endif // SCENE_H
