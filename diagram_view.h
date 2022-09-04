#ifndef DIAGRAM_VIEW_H
#define DIAGRAM_VIEW_H

#include <QGraphicsView>

QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDropEvent;
QT_END_NAMESPACE

class diagram_view : public QGraphicsView
{
public:
    diagram_view(QGraphicsScene *scene, QWidget *parent = nullptr);
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

#endif // DIAGRAM_VIEW_H
