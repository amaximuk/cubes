#pragma once

#include <QGraphicsView>
#include "../top_manager_interface.h"

namespace CubeDiagram
{
    // Класс DiagramView нужен для создания юнитов путем перетаскивания из дерева
    class DiagramView : public QGraphicsView
    {
    private:
        ITopManager* topManager_;

    public:
        DiagramView(ITopManager* topManager, QGraphicsScene* scene, QWidget* parent = nullptr);

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
    };
}
