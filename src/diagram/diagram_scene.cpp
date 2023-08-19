#include <QGraphicsSceneMouseEvent>
#include <QGuiApplication>
#include <QCursor>
#include <QKeyEvent>
#include <QPainter>
#include "diagram_item.h"
#include "diagram_scene.h"

using namespace CubeDiagram;

DiagramScene::DiagramScene(ITopManager* topManager, QObject *parent) :
    QGraphicsScene(parent)
{
    topManager_ = topManager;
    isItemMoving_ = false;
    movingItem_ = nullptr;
}

void DiagramScene::InformItemPositionChanged(const uint32_t propertiesId, double posX, double posY, double posZ)
{

}

void DiagramScene::InformItemPositionChanged(DiagramItem* item)
{
    emit ItemPositionChanged(item);
}

void DiagramScene::InformItemCreated(DiagramItem* item)
{
    emit AfterItemCreated(item);
}

void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  //event->ignore();

  movingItem_ = itemAt(event->scenePos(), QTransform());
  if (movingItem_ != nullptr)
  {
    isItemMoving_ = true;
    
    // Если сразу тащим, без предварительного выделения, selectedItems() пустой
    // Если при этом были выделены другие item, то selectedItems() надо очистить
    if (!selectedItems().contains(movingItem_))
    {
        clearSelection();
        movingItem_->setSelected(true);
   }

    for (auto& item : selectedItems())
    {
        DiagramItem* di = new DiagramItem(*reinterpret_cast<DiagramItem*>(item));
        DiagramItem* olddi = reinterpret_cast<DiagramItem*>(item);

        di->name_ = olddi->name_;
        dragItems_.push_back({ di, olddi });
        di->SetBorderOnly(true);
        addItem(di);
    }
    
    bool ctrl = (event->modifiers() == Qt::ControlModifier);
    if (ctrl)
    {
        QGuiApplication::setOverrideCursor(Qt::DragCopyCursor);
        for (auto& pair : dragItems_)
        {
            pair.second->name_ = "<new item>";
            pair.second->InformNameChanged("<new item>", "");
            pair.first->SetBorderOnly(false);
        }
    }
    else
        QGuiApplication::setOverrideCursor(Qt::ArrowCursor);

    QPointF p = movingItem_->mapFromParent(event->scenePos());
    startPosition_ = event->scenePos() - p;

  }

  // Always remember to call parents mousePressEvent
  QGraphicsScene::mousePressEvent(event);
}

void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  //event->ignore();
//  bool ctrl = (event->modifiers() == Qt::ControlModifier);
//  if (ctrl)
//      QGuiApplication::setOverrideCursor(Qt::DragCopyCursor);
//  else
//      QGuiApplication::setOverrideCursor(Qt::ArrowCursor);

  // Always remember to call parents mousePressEvent
  QGraphicsScene::mouseMoveEvent(event);

  QGraphicsScene::invalidate(sceneRect(), QGraphicsScene::BackgroundLayer);

}

void DiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    //event->ignore();
    isItemMoving_ = false;

    bool ctrl = (event->modifiers() == Qt::ControlModifier);
    //  QGuiApplication::setOverrideCursor(Qt::ArrowCursor);


    //  QPainterPath tmpPath = selectionArea();
    //  if(tmpPath.isEmpty())
    //  {
    //    // if ctrl pressed, then toggle selection
    //    //emit select(event->scenePos(), ctrl);
    //  }
    //  else
    //  {
    //    // if ctrl pressed, then add selection
    //    //emit select(tmpPath, ctrl);
    //  }

    if (movingItem_ != nullptr)
    {
        const int gridSize = 20;
        QPointF delta = startPosition_ - movingItem_->pos();
        if (ctrl && (std::abs(delta.x()) >= gridSize || std::abs(delta.y()) >= gridSize))
        {
            for (auto& item : selectedItems())
            {
                item->setPos(item->pos() + delta);
            }
            clearSelection();


            for (auto& pair : dragItems_)
            {
                QString unitId;
                if (!topManager_->GetPropetiesUnitId(pair.first->propertiesId_, unitId))
                {
                    qDebug() << "ERROR GetPropetiesUnitParameters: " << pair.first->propertiesId_;
                }

                uint32_t propertiesId{ 0 };
                if (!topManager_->CreatePropetiesItem(unitId, propertiesId))
                {
                    qDebug() << "ERROR CreatePropetiesItem: " << unitId;
                }

                PropertiesForDrawing pfd{};
                if (!topManager_->GetPropetiesForDrawing(propertiesId, pfd))
                {
                    qDebug() << "ERROR GetPropetiesForDrawing: " << propertiesId;
                }

                pair.second->name_ = pair.first->name_;
                pair.second->InformNameChanged(pair.first->name_, "");

                pair.first->name_ = pfd.name;
                pair.first->InformNameChanged(pfd.name, "");
                pair.first->propertiesId_ = propertiesId;


                pair.first->SetBorderOnly(false);
                InformItemCreated(pair.first);

                QPointF position = pair.first->pos() - delta;

                qreal xV = round(position.x() / gridSize) * gridSize;
                qreal yV = round(position.y() / gridSize) * gridSize;
                position = QPoint(xV, yV);

                addItem(pair.first);
                pair.first->setPos(position);
                pair.first->setSelected(true);
                //addItem(di);
                //di->setPos(position);
      //          di->setSelected(true);
            }
            //clearSelection();
            //for (auto& item : selectedItems())
            //{
            //    item->setPos(item->pos() + delta);
            //}
            //clearSelection();
            //for (auto& item : drag_items_)
            //    item->setSelected(true);
            dragItems_.clear();

            //emit xxx(ppp_);
            QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
        }
        else
        {
            for (auto& pair : dragItems_)
            {
                pair.second->name_ = pair.first->name_;
                pair.second->InformNameChanged(pair.first->name_, "");
                delete pair.first;
            }
            dragItems_.clear();
            QGuiApplication::setOverrideCursor(Qt::ArrowCursor);

            //for (auto& pair : dragItems_)
            //    delete pair.first;
            //dragItems_.clear();
        }

        movingItem_ = nullptr;
    }

    // Always remember to call parents mousePressEvent
    QGraphicsScene::mouseReleaseEvent(event);
    QGraphicsScene::invalidate(sceneRect(), QGraphicsScene::BackgroundLayer);
}

void DiagramScene::keyPressEvent(QKeyEvent *keyEvent)
{
    if (isItemMoving_)
    {
        bool ctrl = (keyEvent->modifiers() == Qt::ControlModifier);
        if (ctrl)
        {
            qDebug() << "press" << dragItems_.size();
            QGuiApplication::setOverrideCursor(Qt::DragCopyCursor);
            for (auto& pair : dragItems_)
            {
                pair.second->name_ = "<new item>";
                pair.second->InformNameChanged("<new item>", "");
                pair.first->SetBorderOnly(false);
            }
        }
        else
        {
            qDebug() << "release " << dragItems_.size();
            QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
            for (auto& pair : dragItems_)
            {
                pair.second->name_ = pair.first->name_;
                pair.second->InformNameChanged(pair.first->name_, "");
                pair.first->SetBorderOnly(true);
            }
        }
    }
    else
    {
        if (keyEvent->key() == Qt::Key_Delete)
        {
            for (auto item : selectedItems())
            {
                DiagramItem* di = reinterpret_cast<DiagramItem*>(item);
                emit BeforeItemDeleted(di);
                delete item;
            }
            invalidate();
        }
    }

    QGraphicsScene::keyPressEvent(keyEvent);
}

void DiagramScene::keyReleaseEvent(QKeyEvent *keyEvent)
{
    if (isItemMoving_)
    {
        bool ctrl = (keyEvent->modifiers() == Qt::ControlModifier);
        if (ctrl)
        {
            QGuiApplication::setOverrideCursor(Qt::DragCopyCursor);
            for (auto& pair : dragItems_)
            {
                pair.second->name_ = "<new item>";
                pair.second->InformNameChanged("<new item>", "");
                pair.first->SetBorderOnly(false);
            }
        }
        else
        {
            QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
            for (auto& pair : dragItems_)
            {
                pair.second->name_ = pair.first->name_;
                pair.second->InformNameChanged(pair.first->name_, "");
                pair.first->SetBorderOnly(true);
            }
        }
    }

    QGraphicsScene::keyPressEvent(keyEvent);
}

void DiagramScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    //DrawConnections(painter, rect);
    //QGraphicsScene::drawBackground(painter, rect);
}

DiagramItem* DiagramScene::GetDiagramItem(QString name)
{
    for (const auto& item : items())
    {
        DiagramItem* di = reinterpret_cast<DiagramItem*>(item);
        if (name == di->name_)
        {
            return di;
        }
        //qDebug() << name << " - " << di->getInstanceName();
    }
    return nullptr;
}

void DiagramScene::DrawConnections(QPainter* painter, const QRectF& rect)
{
    {
        QMap<QString, QStringList> connections = topManager_->GetUnitsConnections();
        painter->setPen(QPen(QBrush(QColor(0xFF, 0, 0, 0x20), Qt::SolidPattern), 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->setRenderHint(QPainter::Antialiasing);
        for (const auto& kvp : connections.toStdMap())
        {
            auto di1 = GetDiagramItem(kvp.first);
            for (const auto& item : kvp.second)
            {
                auto di2 = GetDiagramItem(item);
                painter->drawLine(di1->GetLineAncorPosition(), di2->GetLineAncorPosition());
            }
        }
    }

    {
        QMap<QString, QStringList> connections = topManager_->GetDependsConnections();
        painter->setPen(QPen(QBrush(QColor(0, 0, 0, 0x80), Qt::SolidPattern), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->setRenderHint(QPainter::Antialiasing);
        for (const auto& kvp : connections.toStdMap())
        {
            auto di1 = GetDiagramItem(kvp.first);
            for (const auto& item : kvp.second)
            {
                auto di2 = GetDiagramItem(item);
                painter->drawLine(di1->GetLineAncorPosition(), di2->GetLineAncorPosition());
            }
        }
    }
}

void DiagramScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    auto item = itemAt(event->scenePos(), QTransform());
    if (item != nullptr)
    {
        DiagramItem* di = reinterpret_cast<DiagramItem*>(item);
        //if (di->getProperties()->GetId() == "group")
        //    main_->ActivateGroup(di->getProperties()->GetName());
    }
}
