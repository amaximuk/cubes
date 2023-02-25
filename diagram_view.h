#ifndef DIAGRAM_VIEW_H
#define DIAGRAM_VIEW_H

#include <QGraphicsView>
#include "main_window.h"


QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDropEvent;
QT_END_NAMESPACE

class diagram_view : public QGraphicsView
{
private:
    MainWindow* main_;

public:
    diagram_view(QGraphicsScene *scene, MainWindow* main, QWidget *parent = nullptr);
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

};

#endif // DIAGRAM_VIEW_H
