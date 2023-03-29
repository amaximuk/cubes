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

diagram_scene::diagram_scene(QObject *parent) : QGraphicsScene(parent)
{
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

void diagram_scene::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
  event->ignore();

  moving_item_ = itemAt(event->scenePos(), QTransform());
  if (moving_item_ != 0)
  {
    qDebug() << "yes!!!!!!!!!!";
    is_item_moving_ = true;
    bool ctrl = (event->modifiers() == Qt::ControlModifier);
    if (ctrl)
        QGuiApplication::setOverrideCursor(Qt::DragCopyCursor);
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

void diagram_scene::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
  event->ignore();
//  bool ctrl = (event->modifiers() == Qt::ControlModifier);
//  if (ctrl)
//      QGuiApplication::setOverrideCursor(Qt::DragCopyCursor);
//  else
//      QGuiApplication::setOverrideCursor(Qt::ArrowCursor);

  // Always remember to call parents mousePressEvent
  QGraphicsScene::mouseMoveEvent(event);

  QGraphicsScene::invalidate(sceneRect(), QGraphicsScene::BackgroundLayer);

}

void diagram_scene::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
  event->ignore();
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


  if (ctrl)
  {
      QPointF delta = ppp_ - moving_item_->pos();
//      if (selectedItems().count() > 0)
//      {
//          selectedItems()[0]->setPos(ppp_);
//      }
      for (auto& item: selectedItems())
      {
          item->setPos(item->pos() + delta);
      }
      //emit xxx(ppp_);
      QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
  }

  moving_item_ = 0;
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
            QGuiApplication::setOverrideCursor(Qt::DragCopyCursor);
        else
            QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
    }

    QGraphicsScene::keyPressEvent(keyEvent);
}

void diagram_scene::keyReleaseEvent(QKeyEvent *keyEvent)
{
    if (is_item_moving_)
    {
        bool ctrl = (keyEvent->modifiers() == Qt::ControlModifier);
        if (ctrl)
            QGuiApplication::setOverrideCursor(Qt::DragCopyCursor);
        else
            QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
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
    
    //painter->fillRect(sceneRect(), Qt::blue);

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
                    painter->drawLine(di1->scenePos(), di2->scenePos());
                    painter->drawLine(di1->pos(), di2->pos());
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
    }
    return nullptr;
}