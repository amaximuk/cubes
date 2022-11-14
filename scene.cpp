#include "scene.h"
#include <QPainterPath>
#include <QGraphicsSceneMouseEvent>

scene::scene(QObject *parent) : QGraphicsScene(parent)
{

}

void scene::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
  event->ignore();

  // Reset selectionArea
  setSelectionArea(QPainterPath());

  // Always remember to call parents mousePressEvent
  QGraphicsScene::mousePressEvent(event);
}

void scene::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
  event->ignore();

  bool ctrl = (event->modifiers() == Qt::ControlModifier);

  QPainterPath tmpPath = selectionArea();
  if(tmpPath.isEmpty())
  {
    // if ctrl pressed, then toggle selection
    //emit select(event->scenePos(), ctrl);
  }
  else
  {
    // if ctrl pressed, then add selection
    //emit select(tmpPath, ctrl);
  }

  // Always remember to call parents mousePressEvent
  QGraphicsScene::mouseReleaseEvent(event);

}
