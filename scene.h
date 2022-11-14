#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>

class scene : public QGraphicsScene
{
public:
    explicit scene(QObject *parent = nullptr);

protected slots:
    void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );

};

#endif // SCENE_H
