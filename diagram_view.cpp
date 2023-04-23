#include "diagram_view.h"
#include "diagram_item.h"
#include "diagram_scene.h"
#include "main_window.h"

#include <QtWidgets>
#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>

diagram_view::diagram_view(QGraphicsScene *scene, MainWindow *main, QWidget *parent):
    QGraphicsView(scene, parent)
{
    main_ = main;
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
    if (event->mimeData()->hasFormat("application/x-dnditemdata"))
    {
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
        dataStream >> name >> offset;
        qDebug() << "name: " << name << offset;
        QString vvv = QString::number(counter++);


        if (scene() != nullptr)
        {
            auto up = *main_->GetUnitParameters(name);

            diagram_item *di = new diagram_item(up);
            diagram_scene* ds = qobject_cast<diagram_scene*>(this->scene());

            di->getProperties()->SetFileNames(main_->GetFileNames());
            di->getProperties()->SetFileName(main_->GetCurrentFileName());
            di->getProperties()->SetName(main_->GetNewUnitName(di->getProperties()->GetName(), di->GetGroupName()));
            di->SetGroupName(main_->GetCurrentGroup());
            ds->informItemCreated(di);


            QPoint position = mapToScene(event->pos()-QPoint(24,24)).toPoint();

            int gridSize = 20;
            qreal xV = round(position.x() / gridSize) * gridSize;
            qreal yV = round(position.y() / gridSize) * gridSize;
            position = QPoint(xV, yV);

            this->scene()->addItem(di);
            this->scene()->clearSelection();
            di->setPos(position);
            di->setSelected(true);
        }


        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }

    if (scene() != nullptr)
        scene()->invalidate(sceneRect(), QGraphicsScene::BackgroundLayer);

}
