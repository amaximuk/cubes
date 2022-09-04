#include "diagram_view.h"
#include "drag_widget.h"
#include "diagram_item.h"

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

void diagram_view::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QPixmap pixmap;
        QPoint offset;
        QString name;
        dataStream >> pixmap >> offset >> name;
        qDebug() << "offset: " << offset;

        diagram_item *newIcon = new diagram_item(pixmap);
        QPoint position = mapToScene(event->pos() - offset).toPoint();
        this->scene()->addItem(newIcon);
        newIcon->setPos(position);

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
