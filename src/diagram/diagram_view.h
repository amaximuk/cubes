#pragma once

#include <QGraphicsView>
#include "../top_manager_interface.h"


class QDragEnterEvent;
class QDropEvent;

namespace CubeDiagram
{
    // ����� DiagramView ����� ��� �������� ������ ����� �������������� �� ������
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
    };
}
