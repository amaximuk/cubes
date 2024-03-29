#pragma once

#include <QGraphicsScene>
#include "../top_manager_interface.h"

namespace CubeDiagram
{
    class DiagramItem;

    class DiagramScene : public QGraphicsScene
    {
        Q_OBJECT

    private:
        ITopManager* topManager_;
        QPointF startPosition_;
        bool isItemMoving_;
        QGraphicsItem* movingItem_;
        QList<QPair<DiagramItem*, DiagramItem*>> dragItems_;
        bool selectedWithCtrl_;

    public:
        explicit DiagramScene(ITopManager* topManager, QObject* parent = nullptr);

    public:
        // TODO: ��� ��������� ��� ������� ����������, ����� �������� �� ��������� diagram_item
        void InformItemPositionChanged(const uint32_t propertiesId, double posX, double posY, double posZ);

        void InformItemPositionChanged(DiagramItem* item);
        void InformItemCreated(DiagramItem* item);
        bool IsItemMoving() { return isItemMoving_; };

    signals:
        void ItemPositionChanged(DiagramItem* item);
        void AfterItemCreated(DiagramItem* item);
        void BeforeItemDeleted(DiagramItem* item);

    protected:
        void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
        void keyPressEvent(QKeyEvent* keyEvent) override;
        void keyReleaseEvent(QKeyEvent* keyEvent) override;
        void drawBackground(QPainter* painter, const QRectF& rect) override;
        void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
        //void wheelEvent(QGraphicsSceneWheelEvent* wheelEvent) override;

    private:
        DiagramItem* GetDiagramItem(QString name);
        void DrawConnections(QPainter* painter, const QRectF& rect);
    };
}
