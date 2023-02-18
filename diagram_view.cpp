#include "diagram_view.h"
#include "drag_widget.h"
#include "diagram_item.h"
#include "main_window.h"

#include <QtWidgets>
#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>

diagram_view::diagram_view(QGraphicsScene *scene, QWidget *parent):
    QGraphicsView(scene, parent)
{
    setAcceptDrops(true);
}

void diagram_view::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
    //if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void diagram_view::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
    //if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

int counter = 0;
void diagram_view::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
    //if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
        QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

//        QPixmap pixmap;
//        QPoint offset;
//        QString name;
//        dataStream >> pixmap >> offset >> name;
//        qDebug() << "offset: " << offset;

//        diagram_item *newIcon = new diagram_item(pixmap);
//        QPoint position = mapToScene(event->pos() - offset).toPoint();
//        this->scene()->addItem(newIcon);
//        newIcon->setPos(position);

        QPixmap pixmap;
        QPoint offset;
        QString name;
        dataStream >> name;
        qDebug() << "name: " << name;
        QString vvv = QString::number(counter++);

        diagram_item *newIcon = new diagram_item(QIcon("c:/QtProjects/cubes/resource/plus.png").pixmap(48,48), vvv);
        qDebug() << connect(newIcon, &diagram_item::positionChanged, qobject_cast<MainWindow*>(QApplication::activeWindow()), &MainWindow::itemPositionChanged);



        QPoint position = mapToScene(event->pos()-QPoint(24,24)).toPoint();
        this->scene()->addItem(newIcon);
        this->scene()->clearSelection();
        newIcon->setPos(position);
        newIcon->setSelected(true);


        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}
