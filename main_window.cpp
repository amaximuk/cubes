#include "main_window.h"
#include "drag_widget.h"
#include "diagram_view.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTableView>
#include <QTreeView>
#include <QToolBox>
#include <QToolButton>
#include <QSplitter>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    CreateUi();
}

MainWindow::~MainWindow()
{

}

void MainWindow::CreateUi()
{
    QVBoxLayout* main_lay = new QVBoxLayout;
    QHBoxLayout* lay = new QHBoxLayout;

    tool_box_ = new QToolBox;
    tool_box_->setMinimumWidth(400);
    sp_scene_ = new QGraphicsScene();
    //sp_scene_->setSceneRect(0,0,500,500);
    view_ = new diagram_view(sp_scene_);
    tree_view_ = new QTreeView;

    CreateToolBox();
    CreateTreeView();

    table_view_ = new QTableView;
    //main_lay->addWidget(table_view_);

//    QWidget *topWidget = new QWidget;
//    topWidget->setLayout(layout1);
//    ...
//    splitter->addWidget(topWidget);
//    splitter->addWidget(bottomWidget);

//    QWidget* splitter_widget = new QWidget();
    splitter_v_ = new QSplitter(Qt::Horizontal);
    splitter_v_->addWidget(tool_box_);
    splitter_v_->addWidget(tree_view_);
    splitter_v_->setStretchFactor(0, 0);
    splitter_v_->setStretchFactor(1, 1);

    splitter_h_ = new QSplitter(Qt::Vertical);
    splitter_h_->addWidget(splitter_v_);
    splitter_h_->addWidget(table_view_);
    splitter_h_->setStretchFactor(0, 1);
    splitter_h_->setStretchFactor(1, 0);
//    lay->addWidget(tool_box_);
//    lay->addWidget(view_);
//    lay->addWidget(tree_view_);
//    lay->addWidget(splitter_h_);

    main_lay->addWidget(splitter_h_);


    setLayout(main_lay);
}

void MainWindow::CreateToolBox()
{
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new drag_widget(QPixmap("c:/QtProjects/cubes/resource/boat.png"), "SigmaDriver"), 0, 0);
    layout->addWidget(new drag_widget(QPixmap("c:/QtProjects/cubes/resource/boat.png"), "SigmaDriver"), 0, 1);
    layout->addWidget(new drag_widget(QPixmap("c:/QtProjects/cubes/resource/boat.png"), "SigmaDriver"), 1, 0);
    layout->addWidget(new drag_widget(QPixmap("c:/QtProjects/cubes/resource/boat.png"), "SigmaDriver"), 1, 1);
    layout->addWidget(new drag_widget(QPixmap("c:/QtProjects/cubes/resource/boat.png"), "SigmaDriver"), 2, 0);
    layout->addWidget(new drag_widget(QPixmap("c:/QtProjects/cubes/resource/boat.png"), "SigmaDriver"), 2, 1);
    layout->addWidget(new drag_widget(QPixmap("c:/QtProjects/cubes/resource/boat.png"), "SigmaDriver"), 3, 0);
    layout->addWidget(new drag_widget(QPixmap("c:/QtProjects/cubes/resource/boat.png"), "SigmaDriver"), 3, 1);

    layout->setRowStretch(3, 10);
    layout->setColumnStretch(2, 10);

    QWidget *itemWidget = new QWidget;
    itemWidget->setLayout(layout);

    tool_box_->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
    tool_box_->setMinimumWidth(itemWidget->sizeHint().width());
    tool_box_->addItem(itemWidget, tr("Basic Flowchart Shapes"));
}

void MainWindow::CreateTreeView()
{

}
