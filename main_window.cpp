#include "main_window.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTableView>
#include <QTreeView>
#include <QToolBox>

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
    tool_box_->setMinimumWidth(300);
    sp_scene_ = QSharedPointer<QGraphicsScene>::create();
    view_ = new QGraphicsView(sp_scene_.get());
    tree_view_ = new QTreeView;

    lay->addWidget(tool_box_);
    lay->addWidget(view_);
    lay->addWidget(tree_view_);

    main_lay->addLayout(lay);

    table_view_ = new QTableView;
    main_lay->addWidget(table_view_);

    setLayout(main_lay);
}
