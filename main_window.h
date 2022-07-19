#pragma once

#include <QWidget>
#include <QPointer>

class QGraphicsScene;
class QGraphicsView;
class QTableView;
class QTreeView;
class QToolBox;

class MainWindow
        : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:

    void CreateUi();

private:

    QPointer<QToolBox> tool_box_;
    QSharedPointer<QGraphicsScene> sp_scene_;
    QPointer<QGraphicsView> view_;

    QPointer<QTreeView> tree_view_;

    QPointer<QTableView> table_view_;
};
