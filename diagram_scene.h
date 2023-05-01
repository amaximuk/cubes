#ifndef SCENE_H
#define SCENE_H

class QGraphicsItem;
class diagram_item;

#include <QGraphicsScene>
#include <QpointF>

#include "main_window.h"

class diagram_scene : public QGraphicsScene
{
    Q_OBJECT

private:
    MainWindow* main_;
    QPointF ppp_;
    bool is_item_moving_;
    QGraphicsItem* moving_item_;
    QList<diagram_item*> drag_items_;
    const int id_;

public:
    explicit diagram_scene(MainWindow* main, int id, QObject *parent = nullptr);

public:
    void informItemPositionChanged(diagram_item* item);
    void informItemCreated(diagram_item* item);
    void informItemNameChanged(diagram_item* item, QString oldName);
    void informItemFileChanged(diagram_item* item);
    bool isItemMoving() { return is_item_moving_; };
    MainWindow* getMain() { return main_; };

signals:
     void xxx(QPointF ppp);
     void itemPositionChanged(diagram_item* item);
     void afterItemCreated(diagram_item* item);
     void beforeItemDeleted(diagram_item* item);
     void itemNameChanged(diagram_item* item, QString oldName);
     void itemFileChanged(diagram_item* item);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* keyEvent) override;
    void keyReleaseEvent(QKeyEvent* keyEvent) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
    diagram_item* getDiagramItem(QString name);
    void drawConnections(QPainter* painter, const QRectF& rect);
    void drawDependencies(QPainter* painter, const QRectF& rect);

public:
    diagram_item* GetDiagramItem(QString name) { return getDiagramItem(name); }
};

#endif // SCENE_H
