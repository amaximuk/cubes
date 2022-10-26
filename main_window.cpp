#include "main_window.h"
#include "drag_widget.h"
#include "diagram_view.h"

#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTableView>
#include <QTreeView>
#include <QToolBox>
#include <QToolButton>
#include <QSplitter>
#include <QDialog>
#include <QDebug>
#include <QPushButton>

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
    //tool_box_->setMinimumWidth(500);
    sp_scene_ = new QGraphicsScene;
    //sp_scene_->setSceneRect(0,0,500,500);
    view_ = new diagram_view(sp_scene_);
    tree_view_ = new QTreeView;

    CreateToolBox();
    CreateTreeView();

    table_view_log_ = new QTableView;
    table_view_info_ = new QTableView;
    table_view_properties_ = new QTableView;
    //main_lay->addWidget(table_view_);






    doubleManager = new QtDoublePropertyManager(this);
    stringManager = new QtStringPropertyManager(this);
    colorManager = new QtColorPropertyManager(this);
    fontManager = new QtFontPropertyManager(this);
    pointManager = new QtPointPropertyManager(this);
    sizeManager = new QtSizePropertyManager(this);

    qDebug() << connect(doubleManager, SIGNAL(valueChanged(QtProperty*,double)), this, SLOT(valueChanged(QtProperty*,double)));
    qDebug() << connect(stringManager, SIGNAL(valueChanged(QtProperty*,const QString&)), this, SLOT(valueChanged(QtProperty*,const QString&)));
    qDebug() << connect(colorManager, SIGNAL(valueChanged(QtProperty*,const QColor&)), this, SLOT(valueChanged(QtProperty*,const QColor&)));
    qDebug() << connect(fontManager, SIGNAL(valueChanged(QtProperty*,const QFont&)), this, SLOT(valueChanged(QtProperty*,const QFont&)));
    qDebug() << connect(pointManager, SIGNAL(valueChanged(QtProperty*,const QPoint&)), this, SLOT(valueChanged(QtProperty*,const QPoint&)));
    qDebug() << connect(sizeManager, SIGNAL(valueChanged(QtProperty*,const QSize&)), this, SLOT(valueChanged(QtProperty*,const QSize&)));

    QtDoubleSpinBoxFactory *doubleSpinBoxFactory = new QtDoubleSpinBoxFactory(this);
    QtCheckBoxFactory *checkBoxFactory = new QtCheckBoxFactory(this);
    QtSpinBoxFactory *spinBoxFactory = new QtSpinBoxFactory(this);
    QtLineEditFactory *lineEditFactory = new QtLineEditFactory(this);
    QtEnumEditorFactory *comboBoxFactory = new QtEnumEditorFactory(this);

    propertyEditor = new QtTreePropertyBrowser();
    propertyEditor->setFactoryForManager(doubleManager, doubleSpinBoxFactory);
    propertyEditor->setFactoryForManager(stringManager, lineEditFactory);
    propertyEditor->setFactoryForManager(colorManager->subIntPropertyManager(), spinBoxFactory);
    propertyEditor->setFactoryForManager(fontManager->subIntPropertyManager(), spinBoxFactory);
    propertyEditor->setFactoryForManager(fontManager->subBoolPropertyManager(), checkBoxFactory);
    propertyEditor->setFactoryForManager(fontManager->subEnumPropertyManager(), comboBoxFactory);
    propertyEditor->setFactoryForManager(pointManager->subIntPropertyManager(), spinBoxFactory);
    propertyEditor->setFactoryForManager(sizeManager->subIntPropertyManager(), spinBoxFactory);





//    QtProperty *property;

//    property = doubleManager->addProperty(tr("Position X"));
//    doubleManager->setRange(property, 0, 100);
//    doubleManager->setValue(property, 50);
//    addProperty(property, QLatin1String("xpos"));

//    property = doubleManager->addProperty(tr("Position Y"));
//    doubleManager->setRange(property, 0, 100);
//    doubleManager->setValue(property, 70);
//    addProperty(property, QLatin1String("ypos"));

//    property = doubleManager->addProperty(tr("Position Z"));
//    doubleManager->setRange(property, 0, 256);
//    doubleManager->setValue(property, 33);
//    addProperty(property, QLatin1String("zpos"));

//    property = colorManager->addProperty(tr("Color"));
//    colorManager->setValue(property, Qt::GlobalColor::darkRed);
//    addProperty(property, QLatin1String("color"));

//    updateExpandState();





    QHBoxLayout* hosts_buttons = new QHBoxLayout;
    QToolButton* buttonAdd = new QToolButton;
    buttonAdd->setFixedSize(32, 32);
    buttonAdd->setIcon(QIcon("c:/QtProjects/cubes/resource/plus.png"));
    qDebug() << connect(buttonAdd, &QPushButton::clicked, this, &MainWindow::MyFirstBtnClicked);
    hosts_buttons->addWidget(buttonAdd);
    hosts_buttons->addStretch();
    QVBoxLayout* hosts = new QVBoxLayout;
    hosts->addLayout(hosts_buttons);
    hosts->addWidget(propertyEditor);
    QWidget *hostsWidget = new QWidget;
    hostsWidget->setContentsMargins(0,0,0,0);
    hostsWidget->setLayout(hosts);

//    QWidget *topWidget = new QWidget;
//    topWidget->setLayout(layout1);
//    ...
//    splitter->addWidget(topWidget);
//    splitter->addWidget(bottomWidget);

//    QWidget* splitter_widget = new QWidget();
    splitter_tool_box_ = new QSplitter(Qt::Horizontal);
    splitter_tool_box_->addWidget(tool_box_);
    splitter_tool_box_->addWidget(view_);
    splitter_tool_box_->setStretchFactor(0, 0);
    splitter_tool_box_->setStretchFactor(1, 1);

    splitter_log_ = new QSplitter(Qt::Vertical);
    splitter_log_->addWidget(splitter_tool_box_);
    splitter_log_->addWidget(table_view_log_);
    splitter_log_->setStretchFactor(0, 1);
    splitter_log_->setStretchFactor(1, 0);

    splitter_info_ = new QSplitter(Qt::Vertical);
    splitter_info_->addWidget(hostsWidget);
    splitter_info_->addWidget(table_view_properties_);
    splitter_info_->setStretchFactor(0, 0);
    splitter_info_->setStretchFactor(1, 1);

    splitter_info_properties_ = new QSplitter(Qt::Horizontal);
    splitter_info_properties_->addWidget(splitter_log_);
    splitter_info_properties_->addWidget(splitter_info_);
    splitter_info_properties_->setStretchFactor(0, 1);
    splitter_info_properties_->setStretchFactor(1, 0);

//    lay->addWidget(tool_box_);
//    lay->addWidget(view_);
//    lay->addWidget(tree_view_);
//    lay->addWidget(splitter_h_);

    main_lay->addWidget(splitter_info_properties_);


    setLayout(main_lay);
}

void MainWindow::MyFirstBtnClicked()
{
    qDebug() <<"xxxxxxxxx";
    QtProperty *property;

    property = doubleManager->addProperty(tr("Position X"));
    doubleManager->setRange(property, 0, 100);
    doubleManager->setValue(property, 50);
    addProperty(property, QLatin1String("xpos"));

    property = doubleManager->addProperty(tr("Position Y"));
    doubleManager->setRange(property, 0, 100);
    doubleManager->setValue(property, 70);
    addProperty(property, QLatin1String("ypos"));

    property = doubleManager->addProperty(tr("Position Z"));
    doubleManager->setRange(property, 0, 256);
    doubleManager->setValue(property, 33);
    addProperty(property, QLatin1String("zpos"));

    property = colorManager->addProperty(tr("Color"));
    colorManager->setValue(property, Qt::GlobalColor::darkRed);
    addProperty(property, QLatin1String("color"));

    updateExpandState();

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

    layout->setRowStretch(3, 1);
    layout->setColumnStretch(2, 1);

    QWidget *itemWidget = new QWidget;
    itemWidget->setLayout(layout);

    tool_box_->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
    tool_box_->setMinimumWidth(itemWidget->sizeHint().width());
    tool_box_->addItem(itemWidget, tr("Basic Flowchart Shapes"));
}

void MainWindow::addProperty(QtProperty *property, const QString &id)
{
    propertyToId[property] = id;
    idToProperty[id] = property;
    QtBrowserItem *item = propertyEditor->addProperty(property);
    if (idToExpanded.contains(id))
        propertyEditor->setExpanded(item, idToExpanded[id]);
}


void MainWindow::updateExpandState()
{
    QList<QtBrowserItem *> list = propertyEditor->topLevelItems();
    QListIterator<QtBrowserItem *> it(list);
    while (it.hasNext()) {
        QtBrowserItem *item = it.next();
        QtProperty *prop = item->property();
        idToExpanded[propertyToId[prop]] = propertyEditor->isExpanded(item);
    }
}

void MainWindow::CreateTreeView()
{

}

void MainWindow::valueChanged(QtProperty *property, double value)
{
    qDebug() << "valueChanged value = " << value;

//    if (!propertyToId.contains(property))
//        return;

//    if (!currentItem)
//        return;

//    QString id = propertyToId[property];
//    if (id == QLatin1String("xpos")) {
//        currentItem->setX(value);
//    } else if (id == QLatin1String("ypos")) {
//        currentItem->setY(value);
//    } else if (id == QLatin1String("zpos")) {
//        currentItem->setZ(value);
//    }
//    canvas->update();
}

void MainWindow::valueChanged(QtProperty *property, const QString &value)
{
//    if (!propertyToId.contains(property))
//        return;

//    if (!currentItem)
//        return;

//    QString id = propertyToId[property];
//    if (id == QLatin1String("text")) {
//        if (currentItem->rtti() == QtCanvasItem::Rtti_Text) {
//            QtCanvasText *i = (QtCanvasText *)currentItem;
//            i->setText(value);
//        }
//    }
//    canvas->update();
}

void MainWindow::valueChanged(QtProperty *property, const QColor &value)
{
//    if (!propertyToId.contains(property))
//        return;

//    if (!currentItem)
//        return;

//    QString id = propertyToId[property];
//    if (id == QLatin1String("color")) {
//        if (currentItem->rtti() == QtCanvasItem::Rtti_Text) {
//            QtCanvasText *i = (QtCanvasText *)currentItem;
//            i->setColor(value);
//        }
//    } else if (id == QLatin1String("brush")) {
//        if (currentItem->rtti() == QtCanvasItem::Rtti_Rectangle ||
//                currentItem->rtti() == QtCanvasItem::Rtti_Ellipse) {
//            QtCanvasPolygonalItem *i = (QtCanvasPolygonalItem *)currentItem;
//            QBrush b = i->brush();
//            b.setColor(value);
//            i->setBrush(b);
//        }
//    } else if (id == QLatin1String("pen")) {
//        if (currentItem->rtti() == QtCanvasItem::Rtti_Rectangle ||
//                currentItem->rtti() == QtCanvasItem::Rtti_Line) {
//            QtCanvasPolygonalItem *i = (QtCanvasPolygonalItem *)currentItem;
//            QPen p = i->pen();
//            p.setColor(value);
//            i->setPen(p);
//        }
//    }
//    canvas->update();
}

void MainWindow::valueChanged(QtProperty *property, const QFont &value)
{
//    if (!propertyToId.contains(property))
//        return;

//    if (!currentItem)
//        return;

//    QString id = propertyToId[property];
//    if (id == QLatin1String("font")) {
//        if (currentItem->rtti() == QtCanvasItem::Rtti_Text) {
//            QtCanvasText *i = (QtCanvasText *)currentItem;
//            i->setFont(value);
//        }
//    }
//    canvas->update();
}

void MainWindow::valueChanged(QtProperty *property, const QPoint &value)
{
//    if (!propertyToId.contains(property))
//        return;

//    if (!currentItem)
//        return;

//    QString id = propertyToId[property];
//    if (currentItem->rtti() == QtCanvasItem::Rtti_Line) {
//        QtCanvasLine *i = (QtCanvasLine *)currentItem;
//        if (id == QLatin1String("endpoint")) {
//            i->setPoints(i->startPoint().x(), i->startPoint().y(), value.x(), value.y());
//        }
//    }
//    canvas->update();
}

void MainWindow::valueChanged(QtProperty *property, const QSize &value)
{
//    if (!propertyToId.contains(property))
//        return;

//    if (!currentItem)
//        return;

//    QString id = propertyToId[property];
//    if (id == QLatin1String("size")) {
//        if (currentItem->rtti() == QtCanvasItem::Rtti_Rectangle) {
//            QtCanvasRectangle *i = (QtCanvasRectangle *)currentItem;
//            i->setSize(value.width(), value.height());
//        } else if (currentItem->rtti() == QtCanvasItem::Rtti_Ellipse) {
//            QtCanvasEllipse *i = (QtCanvasEllipse *)currentItem;
//            i->setSize(value.width(), value.height());
//        }
//    }
//    canvas->update();
}
