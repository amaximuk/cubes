#include <QtWidgets>
#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <QErrorMessage>
//#include "../windows/main_window.h"
#include "diagram_item.h"
#include "diagram_scene.h"
#include "diagram_view.h"

using namespace CubesDiagram;

//#define COPY_ON_DRAG_ENABLED

DiagramView::DiagramView(CubesTop::ITopManager* topManager, QGraphicsScene *scene, QWidget *parent):
    QGraphicsView(scene, parent)
{
    topManager_ = topManager;
    setAcceptDrops(true);
    // setMouseTracking - для корректной работы масштабирования при потере фокуса
    // без этого в начале масштабирования используется неправильная позиция курсора
    setMouseTracking(true);

    //setContextMenuPolicy(Qt::CustomContextMenu);
    //setDragMode(QGraphicsView::ScrollHandDrag);
}

void DiagramView::dragEnterEvent(QDragEnterEvent *event)
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

void DiagramView::dragMoveEvent(QDragMoveEvent *event)
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

void DiagramView::dropEvent(QDropEvent *event)
{
    // Может приходить при перетаскивании из другой копии приложения
    if (event->mimeData()->hasFormat("application/x-dnditemdata"))
    {
        QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QImage pixmap;
        QPoint offset;
        QString unitId;
        dataStream >> unitId >> offset;
        qDebug() << "unitId: " << unitId << offset;

        if (scene() != nullptr)
        {
            DiagramScene* ds = qobject_cast<DiagramScene*>(this->scene());

            CubesUnit::PropertiesId propertiesId{ CubesUnit::InvalidPropertiesId };
            if (!topManager_->CreatePropetiesItem(unitId, propertiesId))
            {
                qDebug() << "ERROR CreatePropetiesItem: " << unitId;
            }

            CubesDiagram::PropertiesForDrawing pfd{};
            if (!topManager_->GetPropetiesForDrawing(propertiesId, pfd))
            {
                qDebug() << "ERROR GetPropeties: " << propertiesId;
            }

            DiagramItem *di = new DiagramItem(propertiesId, pfd);

            ds->InformItemCreated(di);

            QPoint position = mapToScene(event->pos()-QPoint(24,24)).toPoint();

            int gridSize = GridSize;
            qreal xV = round(position.x() / gridSize) * gridSize;
            qreal yV = round(position.y() / gridSize) * gridSize;
            position = QPoint(xV, yV);

            this->scene()->addItem(di);
            this->scene()->clearSelection();
            di->setPos(position);
            ds->InformItemPositionChanged(di);

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
    // https://bugreports.qt.io/browse/QTBUG-73033
    // lastMouseMoveScenePoint = mouseEvent.scenePos();
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
    QGraphicsView::mousePressEvent(event);
}

void DiagramView::mouseMoveEvent(QMouseEvent* event)
{
    QGraphicsView::mouseMoveEvent(event);
}

void DiagramView::mouseReleaseEvent(QMouseEvent* event)
{
    QGraphicsView::mouseReleaseEvent(event);
}

void DiagramView::keyPressEvent(QKeyEvent* event)
{
#ifdef COPY_ON_DRAG_ENABLED
    //bool ctrl = (event->modifiers() == Qt::ControlModifier);
    bool shift = (event->modifiers() == Qt::ShiftModifier);
    if (shift)
    {
        setDragMode(QGraphicsView::ScrollHandDrag);
    }
#endif

    QGraphicsView::keyPressEvent(event);
}

void DiagramView::keyReleaseEvent(QKeyEvent* event)
{
#ifdef COPY_ON_DRAG_ENABLED
    //bool ctrl = (event->modifiers() == Qt::ControlModifier);
    bool shift = (event->modifiers() == Qt::ShiftModifier);
    if (!shift)
    {
        setDragMode(QGraphicsView::RubberBandDrag);
    }
#endif

    QGraphicsView::keyReleaseEvent(event);
}

QRectF DiagramView::GetVisibleSceneRect()
{
    return mapToScene(rect()).boundingRect();
}
