#pragma once

class QGraphicsItem;
class DiagramItem;

#include <QGraphicsScene>
#include <QpointF>

#include "../main_window.h"

class DiagramScene : public QGraphicsScene
{
    Q_OBJECT

private:
    MainWindow* main_;
    QPointF ppp_;
    bool is_item_moving_;
    QGraphicsItem* moving_item_;
    QList<DiagramItem*> drag_items_;
    //const int id_;

public:
    explicit DiagramScene(MainWindow* main, QObject *parent = nullptr);

public:
    void informItemPositionChanged(DiagramItem* item);
    void informItemCreated(DiagramItem* item);
    void informItemNameChanged(DiagramItem* item, QString oldName);
    void informItemFileChanged(DiagramItem* item);
    void informItemGroupChanged(DiagramItem* item);
    bool isItemMoving() { return is_item_moving_; };
    MainWindow* getMain() { return main_; };

signals:
     void xxx(QPointF ppp);
     void itemPositionChanged(DiagramItem* item);
     void afterItemCreated(DiagramItem* item);
     void beforeItemDeleted(DiagramItem* item);
     void itemNameChanged(DiagramItem* item, QString oldName);
     void itemFileChanged(DiagramItem* item);
     void itemGroupChanged(DiagramItem* item);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* keyEvent) override;
    void keyReleaseEvent(QKeyEvent* keyEvent) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
    DiagramItem* getDiagramItem(QString name);
    void drawConnections(QPainter* painter, const QRectF& rect);

public:
    DiagramItem* GetDiagramItem(QString name) { return getDiagramItem(name); }
};
