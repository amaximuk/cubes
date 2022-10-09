#pragma once

#include <QWidget>
#include <QPointer>

class QGraphicsScene;
class QGraphicsView;
class QTableView;
class QTreeView;
class QToolBox;
class diagram_view;
class QSplitter;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void CreateUi();
    void CreateToolBox();
    void CreateTreeView();

private:
    QPointer<QToolBox> tool_box_;
    QPointer<QGraphicsScene> sp_scene_;
    QPointer<diagram_view> view_;
    QPointer<QTreeView> tree_view_;
    QPointer<QTableView> table_view_;
    QPointer<QSplitter> splitter_v_;
    QPointer<QSplitter> splitter_h_;
};
