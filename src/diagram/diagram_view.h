#pragma once

#include <QGraphicsView>
#include "../main_window.h"


class QDragEnterEvent;
class QDropEvent;

namespace CubeDiagram
{
    class DiagramView : public QGraphicsView
    {
    private:
        MainWindow* main_;

    public:
        DiagramView(QGraphicsScene* scene, MainWindow* main, QWidget* parent = nullptr);

    public:
        void dragEnterEvent(QDragEnterEvent* event) override;
        void dragMoveEvent(QDragMoveEvent* event) override;
        void dropEvent(QDropEvent* event) override;
    };
}
