#pragma once

class QGraphicsItem;

#include <QGraphicsScene>
#include <QpointF>

#include "../main_window.h"

namespace CubeDiagram
{
    class DiagramItem;

    class DiagramScene : public QGraphicsScene
    {
        Q_OBJECT

    private:
        MainWindow* main_;
        QPointF ppp_;
        bool isItemMoving_;
        QGraphicsItem* movingItem_;
        QList<DiagramItem*> dragItems_;

    public:
        explicit DiagramScene(MainWindow* main, QObject* parent = nullptr);

    public:
        void InformItemPositionChanged(DiagramItem* item);
        void InformItemCreated(DiagramItem* item);
        void InformItemNameChanged(DiagramItem* item, QString oldName);
        void InformItemFileChanged(DiagramItem* item);
        void InformItemGroupChanged(DiagramItem* item);
        bool IsItemMoving() { return isItemMoving_; };
        MainWindow* GetMain() { return main_; };

    signals:
        void ItemPositionChanged(DiagramItem* item);
        void AfterItemCreated(DiagramItem* item);
        void BeforeItemDeleted(DiagramItem* item);
        void ItemNameChanged(DiagramItem* item, QString oldName);
        void ItemFileChanged(DiagramItem* item);
        void ItemGroupChanged(DiagramItem* item);

    protected:
        void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
        void keyPressEvent(QKeyEvent* keyEvent) override;
        void keyReleaseEvent(QKeyEvent* keyEvent) override;
        void drawBackground(QPainter* painter, const QRectF& rect) override;
        void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

    private:
        DiagramItem* GetDiagramItem(QString name);
        void DrawConnections(QPainter* painter, const QRectF& rect);
    };
}
