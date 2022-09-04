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
    tool_box_->setMinimumWidth(350);
    sp_scene_ = QSharedPointer<QGraphicsScene>::create();
    //sp_scene_->setSceneRect(0,0,500,500);
    view_ = new diagram_view(sp_scene_.get());
    tree_view_ = new QTreeView;

    CreateToolBox();
    CreateTreeView();

    lay->addWidget(tool_box_);
    lay->addWidget(view_);
    lay->addWidget(tree_view_);

    main_lay->addLayout(lay);

    table_view_ = new QTableView;
    main_lay->addWidget(table_view_);

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
