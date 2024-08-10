#pragma once

#include <QGraphicsView>
#include "../top/top_manager_interface.h"

namespace CubesTop { class ITopManager; }

namespace CubesDiagram
{
    // Класс DiagramView нужен для создания юнитов путем перетаскивания из дерева
    class DiagramView : public QGraphicsView
    {
    private:
        CubesTop::ITopManager* topManager_;

    public:
        DiagramView(CubesTop::ITopManager* topManager, QGraphicsScene* scene, QWidget* parent = nullptr);

    public:
        void dragEnterEvent(QDragEnterEvent* event) override;
        void dragMoveEvent(QDragMoveEvent* event) override;
        void dropEvent(QDropEvent* event) override;
        void wheelEvent(QWheelEvent* event) override;
        void mouseDoubleClickEvent(QMouseEvent* event) override;
        void mousePressEvent(QMouseEvent* event) override;
        void mouseReleaseEvent(QMouseEvent* event) override;
        void keyPressEvent(QKeyEvent* event) override;
        void keyReleaseEvent(QKeyEvent* event) override;

    public:
        QRectF GetVisibleSceneRect();
    };
}
