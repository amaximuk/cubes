#include <QPainterPath>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QGuiApplication>
#include <QCursor>
#include <QKeyEvent>
#include <QDebug>
#include <QPainter>

#include "diagram_item.h"
#include "diagram_scene.h"

diagram_scene::diagram_scene(MainWindow* main, QObject *parent) : QGraphicsScene(parent)
{
    main_ = main;
    is_item_moving_ = false;
    moving_item_ = 0;
}

void diagram_scene::informItemPositionChanged(QString id, QPointF newPos)
{
    emit itemPositionChanged(id, newPos);
}

void diagram_scene::informItemCreated(QString id, diagram_item* item)
{
    emit itemCreated(id, item);
}

void diagram_scene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  //event->ignore();

  moving_item_ = itemAt(event->scenePos(), QTransform());
  if (moving_item_ != 0)
  {
    is_item_moving_ = true;
    
    // Если сразу тащим, без предварительного выделения, selectedItems() пустой
    // Если при этом были выделены другие item, то selectedItems() надо очистить
    if (!selectedItems().contains(moving_item_))
    {
        clearSelection();
        moving_item_->setSelected(true);
   }

    for (auto& item : selectedItems())
    {
        diagram_item* di = new diagram_item(*reinterpret_cast<diagram_item*>(item));
        drag_items_.push_back(di);
        di->setVisible(false);
        addItem(di);
    }
    
    bool ctrl = (event->modifiers() == Qt::ControlModifier);
    if (ctrl)
    {
        QGuiApplication::setOverrideCursor(Qt::DragCopyCursor);
        for (auto& item : drag_items_)
            item->setVisible(true);
    }
    else
        QGuiApplication::setOverrideCursor(Qt::ArrowCursor);

    QPointF p = moving_item_->mapFromParent(event->scenePos());
    ppp_ = event->scenePos() - p;

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

void diagram_scene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
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

void diagram_scene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  //event->ignore();
  is_item_moving_ = false;

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

  if (moving_item_ != nullptr)
  {
      if (ctrl)
      {
          QPointF delta = ppp_ - moving_item_->pos();
          //      if (selectedItems().count() > 0)
          //      {
          //          selectedItems()[0]->setPos(ppp_);
          //      }
          for (auto& item : selectedItems())
          {
              item->setPos(item->pos() + delta);
          }
          clearSelection();


          for (auto& item : drag_items_)
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
    //
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
          drag_items_.clear();

          //emit xxx(ppp_);
          QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
      }
      else
      {
          for (auto& item : drag_items_)
              delete item;
          drag_items_.clear();
      }

      moving_item_ = nullptr;
  }

  // Always remember to call parents mousePressEvent
  QGraphicsScene::mouseReleaseEvent(event);


  QGraphicsScene::invalidate(sceneRect(), QGraphicsScene::BackgroundLayer);
}

void diagram_scene::keyPressEvent(QKeyEvent *keyEvent)
{
    if (is_item_moving_)
    {
        bool ctrl = (keyEvent->modifiers() == Qt::ControlModifier);
        if (ctrl)
        {
            qDebug() << "press" << drag_items_.size();
            QGuiApplication::setOverrideCursor(Qt::DragCopyCursor);
            for (auto& item : drag_items_)
                item->setVisible(true);
        }
        else
        {
            qDebug() << "release " << drag_items_.size();
            QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
            for (auto& item : drag_items_)
                item->setVisible(false);
        }
    }
    else
    {
        if (keyEvent->key() == Qt::Key_Delete)
        {
            for (auto item : selectedItems())
            {
                delete item;
            }
        }
    }

    QGraphicsScene::keyPressEvent(keyEvent);
}

void diagram_scene::keyReleaseEvent(QKeyEvent *keyEvent)
{
    if (is_item_moving_)
    {
        bool ctrl = (keyEvent->modifiers() == Qt::ControlModifier);
        if (ctrl)
        {
            QGuiApplication::setOverrideCursor(Qt::DragCopyCursor);
            for (auto& item : drag_items_)
                item->setVisible(true);
        }
        else
        {
            QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
            for (auto& item : drag_items_)
                item->setVisible(false);
        }
    }

    QGraphicsScene::keyPressEvent(keyEvent);
}

void diagram_scene::drawBackground(QPainter* painter, const QRectF& rect)
{
    QMap<QString, QList<QString>> connections;
    for (const auto& item : items())
    {
        diagram_item* di = reinterpret_cast<diagram_item*>(item);
        QString name = di->getInstanceName();
        if (name != "")
        {
            QList<QString> conn = di->getConnectedNames();
            connections[name].append(conn);
        }
    }
    
    //painter->fillRect(sceneRect(), Qt::blue);RadialGradientPattern
    painter->setPen(QPen(QBrush(QColor(0xFF, 0, 0, 0x20), Qt::SolidPattern), 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->setRenderHint(QPainter::Antialiasing);

    for (const auto& kvp : connections.toStdMap())
    {
        auto di1 = getDiagramItem(kvp.first);
        if (di1 != nullptr)
        {
            for (const auto& item : kvp.second)
            {
                auto di2 = getDiagramItem(item);
                if (di2 != nullptr)
                {
                    //painter->drawLine(di1->scenePos(), di2->scenePos());
                    painter->drawLine(di1->getLineAncorPosition(), di2->getLineAncorPosition());
                }
            }
        }
    }
    QGraphicsScene::drawBackground(painter, rect);
}

diagram_item* diagram_scene::getDiagramItem(QString name)
{
    for (const auto& item : items())
    {
        diagram_item* di = reinterpret_cast<diagram_item*>(item);
        if (name == di->getInstanceName())
        {
            return di;
        }
        qDebug() << name << " - " << di->getInstanceName();
    }
    return nullptr;
}

void diagram_scene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    auto item = itemAt(event->scenePos(), QTransform());
    if (item != nullptr)
    {
        diagram_item* di = reinterpret_cast<diagram_item*>(item);

        qDebug() << di->getName();
    }
}
