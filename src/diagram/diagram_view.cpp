#include <QtWidgets>
#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <QErrorMessage>
#include "diagram_item.h"
#include "diagram_scene.h"
#include "../main_window.h"
#include "diagram_view.h"

using namespace CubeDiagram;

DiagramView::DiagramView(ITopManager* topManager, QGraphicsScene *scene, QWidget *parent):
    QGraphicsView(scene, parent)
{
    topManager_ = topManager;
    setAcceptDrops(true);
    // setMouseTracking - для корректной работы масштабирования при потере фокуса
    // без этого в начале масштабирования используется неправильная позиция курсора
    setMouseTracking(true);

    //setDragMode(QGraphicsView::ScrollHandDrag);
}

void DiagramView::dragEnterEvent(QDragEnterEvent *event)
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

void DiagramView::dragMoveEvent(QDragMoveEvent *event)
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

void DiagramView::dropEvent(QDropEvent *event)
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
        QString unitId;
        dataStream >> unitId >> offset;
        qDebug() << "unitId: " << unitId << offset;


        //if (name == "group_mock" && main_->GetCurrentGroup() != "Main")
        //{
        //    event->ignore();
        //    QMessageBox::critical(this, "Error", QString::fromLocal8Bit("Группы допускается создавать только в главном окне"));
        //    return;
        //}

        if (scene() != nullptr)
        {
            DiagramScene* ds = qobject_cast<DiagramScene*>(this->scene());
            //auto up = *main_->GetUnitParameters(name);

            uint32_t propertiesId{ 0 };
            if (!topManager_->CreatePropetiesItem(unitId, propertiesId))
            {
                qDebug() << "ERROR CreatePropetiesItem: " << unitId;
            }

            PropertiesForDrawing pfd{};
            if (!topManager_->GetPropetiesForDrawing(propertiesId, pfd))
            {
                qDebug() << "ERROR GetPropeties: " << propertiesId;
            }

            // !!!!!!!!!!!!!! pfd.groupName всегда пуст
            DiagramItem *di = new DiagramItem(propertiesId, pfd.pixmap, pfd.name, pfd.fileName, pfd.groupName, pfd.color);

            //di->GetProperties()->SetFileNames(main_->GetFileNames());
            //di->GetProperties()->SetFileName(main_->GetCurrentFileName());
            //di->GetProperties()->SetGroupNames(main_->GetCurrentFileIncludeNames());
            //di->GetProperties()->SetGroupName("<not selected>");
            //di->GetProperties()->SetName(main_->GetNewUnitName(di->GetProperties()->GetName()));
            
            //di->SetGroupName(main_->GetCurrentGroup());
            ds->InformItemCreated(di);


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

void DiagramView::wheelEvent(QWheelEvent* event)
{
    //https://bugreports.qt.io/browse/QTBUG-73033
    //lastMouseMoveScenePoint = mouseEvent.scenePos();
    // Решение - setMouseTracking(true);


    bool ctrl = (event->modifiers() == Qt::ControlModifier);
    if (ctrl)
    {
        qreal scaleFactor = pow(2.0, event->angleDelta().y() / 240.0);
        qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
        if (factor < 0.25 || factor > 10)
            return;

        const ViewportAnchor anchor = transformationAnchor();
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

        scale(scaleFactor, scaleFactor);
        setTransformationAnchor(anchor);
        resetCachedContent();
        event->accept();
    }
    else
        QGraphicsView::wheelEvent(event);
}

void DiagramView::mouseDoubleClickEvent(QMouseEvent* event)
{
    bool ctrl = (event->modifiers() == Qt::ControlModifier);
    if (ctrl)
    {
        const ViewportAnchor anchor = transformationAnchor();
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        resetTransform();
        setTransformationAnchor(anchor);
        resetCachedContent();
    }
    else
        QGraphicsView::mouseDoubleClickEvent(event);
}

void DiagramView::mousePressEvent(QMouseEvent* event)
{
    //bool ctrl = (event->modifiers() == Qt::ControlModifier);
    //if (ctrl)
    //{
    //    setDragMode(QGraphicsView::ScrollHandDrag);
    //}
    //else
    //    setDragMode(QGraphicsView::RubberBandDrag);


    QGraphicsView::mousePressEvent(event);
}
void DiagramView::mouseReleaseEvent(QMouseEvent* event)
{
    //setDragMode(QGraphicsView::RubberBandDrag);

    QGraphicsView::mouseReleaseEvent(event);
}

void DiagramView::keyPressEvent(QKeyEvent* event)
{
    //bool ctrl = (event->modifiers() == Qt::ControlModifier);
    bool ctrl = (event->modifiers() == Qt::ShiftModifier);
    if (ctrl)
    {
        setDragMode(QGraphicsView::ScrollHandDrag);
    }
    QGraphicsView::keyPressEvent(event);
}

void DiagramView::keyReleaseEvent(QKeyEvent* event)
{
    //bool ctrl = (event->modifiers() == Qt::ControlModifier);
    bool ctrl = (event->modifiers() == Qt::ShiftModifier);
    if (!ctrl)
    {
        setDragMode(QGraphicsView::RubberBandDrag);
    }
    QGraphicsView::keyReleaseEvent(event);
}
