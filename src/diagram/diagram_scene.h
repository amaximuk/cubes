#pragma once

#include <QGraphicsScene>
#include <QColor>
#include "../top/top_manager_interface.h"

namespace CubesTop { class ITopManager; }
namespace CubesDiagram { class DiagramItem; }
namespace CubesLog { class ILogManager; }
namespace CubesLog { class LogHelper; }

namespace CubesDiagram
{
    class DiagramScene : public QGraphicsScene
    {
        Q_OBJECT

    private:
        CubesTop::ITopManager* topManager_;
        CubesLog::ILogManager* logManager_;
        bool isArray_;
        QPointF startPosition_;
        bool isItemMoving_;
        QGraphicsItem* movingItem_;
        QList<QPair<DiagramItem*, DiagramItem*>> dragItems_;
        bool selectedWithCtrl_;

    public:
        explicit DiagramScene(CubesTop::ITopManager* topManager, CubesLog::ILogManager* logManager,
            bool isArray, QObject* parent = nullptr);

    public:
        // TODO: Это заготовка для функций интерфейса, чтобы напрямую не управлять diagram_item
        //void InformItemPositionChanged(CubesUnit::PropertiesId propertiesId, double posX, double posY, double posZ);
        void InformBasePropertiesChanged(CubesUnit::PropertiesId propertiesId, const QString& name,
            const QString& fileName, const QString& includeName, const QColor& color);

        void InformItemPositionChanged(DiagramItem* item);
        void InformItemSizeChanged(DiagramItem* item);
        void InformItemCreated(DiagramItem* item);
        bool IsItemMoving() { return isItemMoving_; };

    signals:
        void ItemPositionChanged(DiagramItem* item);
        void ItemSizeChanged(DiagramItem* item);
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
        void contextMenuEvent(QGraphicsSceneContextMenuEvent* contextMenuEvent) override;

    private:
        DiagramItem* GetDiagramItem(QString name);
        void DrawConnections(QPainter* painter, const QRectF& rect);
    };
}
