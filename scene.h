#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
#include <QpointF>

class scene : public QGraphicsScene
{
    Q_OBJECT

private:
    QPointF ppp_;

public:
    explicit scene(QObject *parent = nullptr);

signals:
     void xxx(QPointF ppp);

protected slots:
    void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );

};

#endif // SCENE_H
