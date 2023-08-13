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

void DiagramScene::InformItemPositionChanged(DiagramItem* item)
{
    emit ItemPositionChanged(item);
}

void DiagramScene::InformItemCreated(DiagramItem* item)
{
    emit AfterItemCreated(item);
}

void DiagramScene::InformItemNameChanged(DiagramItem* item, QString oldName)
{
    emit ItemNameChanged(item, oldName);
}

void DiagramScene::InformItemFileChanged(DiagramItem* item)
{
    emit ItemFileChanged(item);
}

void DiagramScene::InformItemGroupChanged(DiagramItem* item)
{
    emit ItemGroupChanged(item);
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

        QString oldName = di->name_;
        QString newName = topManager_->GetNewUnitName(oldName);
        di->name_ = newName;
        qDebug() << "X1: " << oldName << " - " << newName;

        //di->getProperties()->SetName("<new item>");
        dragItems_.push_back(di);
        di->SetBorderOnly(true);
        addItem(di);
    }
    
    bool ctrl = (event->modifiers() == Qt::ControlModifier);
    if (ctrl)
    {
        QGuiApplication::setOverrideCursor(Qt::DragCopyCursor);
        for (auto& item : dragItems_)
        {
            DiagramItem* di = reinterpret_cast<DiagramItem*>(item);
            qDebug() << "X2: " << di->name_;

            di->SetBorderOnly(false);
        }
    }
    else
        QGuiApplication::setOverrideCursor(Qt::ArrowCursor);

    QPointF p = movingItem_->mapFromParent(event->scenePos());
    startPosition_ = event->scenePos() - p;

  }

//  if (selectedItems().count() > 0)
//  {
//    qDebug() << "yes";
//  }
  // Reset selectionArea
  //setSelectionArea(QPainterPath());


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
      if (ctrl)
      {
          QPointF delta = startPosition_ - movingItem_->pos();
          //      if (selectedItems().count() > 0)
          //      {
          //          selectedItems()[0]->setPos(ppp_);
          //      }
          for (auto& item : selectedItems())
          {
              item->setPos(item->pos() + delta);
          }
          clearSelection();


          for (auto& item : dragItems_)
          {
              //diagram_item* di = new diagram_item(*reinterpret_cast<diagram_item*>(item));

    //          diagram_item* difrom = reinterpret_cast<diagram_item*>(item);
    //          QString name = difrom->getName();
    //
    //          auto up = *main_->GetUnitParameters(name);
    //
    //
    //          diagram_item* di = new diagram_item(up);
    //          //diagram_scene* ds = qobject_cast<diagram_scene*>(this->scene());
    //          //ds->informItemCreated(name, di);
    //          informItemCreated(name, di);

              //item->getProperties()->SetName(main_->GetNewUnitName(item->getProperties()->GetName()));
              item->SetBorderOnly(false);
              InformItemCreated(item);

              QPointF position = item->pos() - delta;
              //QPoint position = mapToScene(event->pos() - QPoint(24, 24)).toPoint();


              int gridSize = 20;
              qreal xV = round(position.x() / gridSize) * gridSize;
              qreal yV = round(position.y() / gridSize) * gridSize;
              position = QPoint(xV, yV);

              addItem(item);
              item->setPos(position);
              item->setSelected(true);
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
          for (auto& item : dragItems_)
              delete item;
          dragItems_.clear();
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
            for (auto& item : dragItems_)
            {
                DiagramItem* di = reinterpret_cast<DiagramItem*>(item);
                di->SetBorderOnly(false);
            }
        }
        else
        {
            qDebug() << "release " << dragItems_.size();
            QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
            for (auto& item : dragItems_)
            {
                DiagramItem* di = reinterpret_cast<DiagramItem*>(item);
                di->SetBorderOnly(true);
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
            for (auto& item : dragItems_)
            {
                DiagramItem* di = reinterpret_cast<DiagramItem*>(item);
                di->SetBorderOnly(false);
            }
        }
        else
        {
            QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
            for (auto& item : dragItems_)
            {
                DiagramItem* di = reinterpret_cast<DiagramItem*>(item);
                di->SetBorderOnly(true);
            }
        }
    }

    QGraphicsScene::keyPressEvent(keyEvent);
}

void DiagramScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    DrawConnections(painter, rect);
    QGraphicsScene::drawBackground(painter, rect);
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
