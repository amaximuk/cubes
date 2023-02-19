#include "diagram_scene.h"
#include <QPainterPath>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QGuiApplication>
#include <QCursor>
#include <QKeyEvent>
#include <QDebug>

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
