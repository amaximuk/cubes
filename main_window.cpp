#include "main_window.h"
#include "diagram_view.h"

#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "tree_item_model.h"
#include "diagram_scene.h"
#include "diagram_item.h"
#include "properties_item.h"
#include "yaml_parser.h"
#include "xml_parser.h"
#include "base64.h"

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
#include <QFile>
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QMenuBar>
#include <QApplication>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QDirIterator>
#include <QInputDialog>
#include <QFileDialog>
#include <QStandardItemModel>





#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>
#include <vector>

#include <boost/graph/planar_canonical_ordering.hpp>
#include <boost/graph/is_straight_line_drawing.hpp>
#include <boost/graph/chrobak_payne_drawing.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>

using namespace boost;

// a class to hold the coordinates of the straight line embedding
struct coord_t
{
    std::size_t x;
    std::size_t y;
};

int xxx()
{
    typedef adjacency_list< vecS, vecS, undirectedS,
        property< vertex_index_t, int > >
        graph;

    // Define the storage type for the planar embedding
    typedef std::vector< std::vector< graph_traits< graph >::edge_descriptor > >
        embedding_storage_t;
    typedef boost::iterator_property_map< embedding_storage_t::iterator,
        property_map< graph, vertex_index_t >::type >
        embedding_t;

    // Create the graph - a maximal planar graph on 7 vertices. The functions
    // planar_canonical_ordering and chrobak_payne_straight_line_drawing both
    // require a maximal planar graph. If you start with a graph that isn't
    // maximal planar (or you're not sure), you can use the functions
    // make_connected, make_biconnected_planar, and make_maximal planar in
    // sequence to add a set of edges to any undirected planar graph to make
    // it maximal planar.

    graph g(10);
    add_edge(1, 7, g);
    add_edge(1, 8, g);
    add_edge(1, 9, g);
    add_edge(1, 7, g);
    add_edge(0, 1, g);
    add_edge(1, 2, g);
    add_edge(2, 3, g);
    add_edge(3, 0, g);
    add_edge(3, 4, g);
    add_edge(4, 5, g);
    add_edge(5, 6, g);
    add_edge(6, 3, g);
    add_edge(0, 4, g);
    add_edge(1, 3, g);
    add_edge(3, 5, g);
    add_edge(2, 6, g);
    add_edge(1, 4, g);
    add_edge(1, 5, g);
    add_edge(1, 6, g);


    // Create the planar embedding
    embedding_storage_t embedding_storage(num_vertices(g));
    embedding_t embedding(embedding_storage.begin(), get(vertex_index, g));

    boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g,
        boyer_myrvold_params::embedding = embedding);

    // Find a canonical ordering
    std::vector< graph_traits< graph >::vertex_descriptor > ordering;
    planar_canonical_ordering(g, embedding, std::back_inserter(ordering));

    // Set up a property map to hold the mapping from vertices to coord_t's
    typedef std::vector< coord_t > straight_line_drawing_storage_t;
    typedef boost::iterator_property_map<
        straight_line_drawing_storage_t::iterator,
        property_map< graph, vertex_index_t >::type >
        straight_line_drawing_t;

    straight_line_drawing_storage_t straight_line_drawing_storage(
        num_vertices(g));
    straight_line_drawing_t straight_line_drawing(
        straight_line_drawing_storage.begin(), get(vertex_index, g));

    // Compute the straight line drawing
    chrobak_payne_straight_line_drawing(
        g, embedding, ordering.begin(), ordering.end(), straight_line_drawing);

    std::cout << "The straight line drawing is: " << std::endl;
    graph_traits< graph >::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi)
    {
        coord_t coord(get(straight_line_drawing, *vi));
        std::cout << *vi << " -> (" << coord.x << ", " << coord.y << ")"
            << std::endl;
    }

    // Verify that the drawing is actually a plane drawing
    if (is_straight_line_drawing(g, straight_line_drawing))
        std::cout << "Is a plane drawing." << std::endl;
    else
        std::cout << "Is not a plane drawing." << std::endl;

    return 0;
}





MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    modified_ = false;

    setWindowIcon(QIcon(":/images/cubes.png"));
    xxx();

    CreateUi();

    auto fi = new files_item();
    fi->ApplyToBrowser(filesPropertyEditor_);
    files_items_.push_back(fi);
}

MainWindow::~MainWindow()
{

}

void MainWindow::CreateUi()
{
    resize(1000, 600);

    CreateMenu();

    QWidget* mainWidget = CreateMainWidget();
    setCentralWidget(mainWidget);
}

QWidget* MainWindow::CreateMainWidget()
{
    CreateFilesPropertyBrowser();
    CreatePropertyBrowser();
    CreateTreeView();
    FillParametersInfo();
    FillTreeView();

    table_view_log_ = new QTableView;

    tabWidget_ = new QTabWidget;
    QWidget* widgetMainTab= CreateMainTabWidget();
    tabWidget_->addTab(widgetMainTab, "Main");

    QSplitter* splitterTreeTab = new QSplitter(Qt::Horizontal);
    splitterTreeTab->addWidget(tree_);
    splitterTreeTab->addWidget(tabWidget_);
    splitterTreeTab->setStretchFactor(0, 0);
    splitterTreeTab->setStretchFactor(1, 1);

    QSplitter* splitterTreeTabLog = new QSplitter(Qt::Vertical);
    splitterTreeTabLog->addWidget(splitterTreeTab);
    splitterTreeTabLog->addWidget(table_view_log_);
    splitterTreeTabLog->setStretchFactor(0, 1);
    splitterTreeTabLog->setStretchFactor(1, 0);

    QWidget* propertiesPanelWidget = CreatePropertiesPanelWidget();
    propertiesPanelWidget->setMinimumWidth(360);
    QSplitter* splitterMain = new QSplitter(Qt::Horizontal);
    splitterMain->addWidget(splitterTreeTabLog);
    splitterMain->addWidget(propertiesPanelWidget);
    splitterMain->setStretchFactor(0, 1);
    splitterMain->setStretchFactor(1, 0);

    QWidget* mainWidget = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(splitterMain);
    mainWidget->setLayout(mainLayout);
    return mainWidget;
}

QWidget* MainWindow::CreateMainTabWidget()
{
    CreateScene();
    CreateView();
    return view_;
}

//QWidget* MainWindow::CreateMainTabWidget()
//{
//    QWidget* widgetTabProperties = new QWidget;
//
//    QWidget* widgetSplitterInfo = CreateMainTabInfoWidget();
//    QWidget* widgetSplitterPropertyList = CreatePropertyListWidget("Main");
//    QWidget* widgetSplitterProperties = CreatePropertiesWidget("Main");
//
//    AddGroupWidget(widgetSplitterInfo, "INFO_GROUP", "Main", ControlsGroup::Info);
//    AddGroupWidget(widgetSplitterPropertyList, "PARAMETERS_GROUP", "Main", ControlsGroup::Parameters);
//    AddGroupWidget(widgetSplitterProperties, "PROPERTIES_GROUP", "Main", ControlsGroup::Properties);
//
//    QSplitter* tabHSplitter = new QSplitter(Qt::Horizontal);
//    tabHSplitter->addWidget(widgetSplitterInfo);
//    tabHSplitter->addWidget(widgetSplitterPropertyList);
//    tabHSplitter->addWidget(widgetSplitterProperties);
//    tabHSplitter->setStretchFactor(0, 1);
//    tabHSplitter->setStretchFactor(1, 0);
//    tabHSplitter->setStretchFactor(2, 1);
//    widgetSplitterProperties->setEnabled(false);
//
//    QVBoxLayout* vBoxLayoutSplitter = new QVBoxLayout;
//    vBoxLayoutSplitter->addWidget(tabHSplitter);
//    widgetTabProperties->setLayout(vBoxLayoutSplitter);
//
//    return widgetTabProperties;
//}

void MainWindow::CreateScene()
{
    scene_ = new diagram_scene(this);
    scene_->setSceneRect(-10000, -10000, 20032, 20032);

    qDebug() << connect(scene_, &diagram_scene::itemPositionChanged, this, &MainWindow::itemPositionChanged);
    qDebug() << connect(scene_, &diagram_scene::itemCreated, this, &MainWindow::itemCreated);
    qDebug() << connect(scene_, &diagram_scene::selectionChanged, this, &MainWindow::selectionChanged);
}

void MainWindow::CreateView()
{
    view_ = new diagram_view(scene_, this);
    view_->setDragMode(QGraphicsView::RubberBandDrag);
}

//void MainWindow::CreatePropertyBrowser()
//{
//    groupManager = new QtGroupPropertyManager(this);
//    intManager = new QtIntPropertyManager(this);
//    doubleManager = new QtDoublePropertyManager(this);
//    stringManager = new QtStringPropertyManager(this);
//    colorManager = new QtColorPropertyManager(this);
//    fontManager = new QtFontPropertyManager(this);
//    pointManager = new QtPointPropertyManager(this);
//    sizeManager = new QtSizePropertyManager(this);
//
//    qDebug() << connect(intManager, SIGNAL(valueChanged(QtProperty*, int)), this, SLOT(valueChanged(QtProperty*, int)));
//    qDebug() << connect(doubleManager, SIGNAL(valueChanged(QtProperty*, double)), this, SLOT(valueChanged(QtProperty*, double)));
//    qDebug() << connect(stringManager, SIGNAL(valueChanged(QtProperty*, const QString&)), this, SLOT(valueChanged(QtProperty*, const QString&)));
//    qDebug() << connect(colorManager, SIGNAL(valueChanged(QtProperty*, const QColor&)), this, SLOT(valueChanged(QtProperty*, const QColor&)));
//    qDebug() << connect(fontManager, SIGNAL(valueChanged(QtProperty*, const QFont&)), this, SLOT(valueChanged(QtProperty*, const QFont&)));
//    qDebug() << connect(pointManager, SIGNAL(valueChanged(QtProperty*, const QPoint&)), this, SLOT(valueChanged(QtProperty*, const QPoint&)));
//    qDebug() << connect(sizeManager, SIGNAL(valueChanged(QtProperty*, const QSize&)), this, SLOT(valueChanged(QtProperty*, const QSize&)));
//
//    QtSpinBoxFactory* intSpinBoxFactory = new QtSpinBoxFactory(this);
//    QtDoubleSpinBoxFactory* doubleSpinBoxFactory = new QtDoubleSpinBoxFactory(this);
//    QtCheckBoxFactory* checkBoxFactory = new QtCheckBoxFactory(this);
//    QtSpinBoxFactory* spinBoxFactory = new QtSpinBoxFactory(this);
//    QtLineEditFactory* lineEditFactory = new QtLineEditFactory(this);
//    QtEnumEditorFactory* comboBoxFactory = new QtEnumEditorFactory(this);
//
//    propertyEditor = new QtTreePropertyBrowser();
//    propertyEditor->setFactoryForManager(intManager, intSpinBoxFactory);
//    propertyEditor->setFactoryForManager(doubleManager, doubleSpinBoxFactory);
//    propertyEditor->setFactoryForManager(stringManager, lineEditFactory);
//    propertyEditor->setFactoryForManager(colorManager->subIntPropertyManager(), spinBoxFactory);
//    propertyEditor->setFactoryForManager(fontManager->subIntPropertyManager(), spinBoxFactory);
//    propertyEditor->setFactoryForManager(fontManager->subBoolPropertyManager(), checkBoxFactory);
//    propertyEditor->setFactoryForManager(fontManager->subEnumPropertyManager(), comboBoxFactory);
//    propertyEditor->setFactoryForManager(pointManager->subIntPropertyManager(), spinBoxFactory);
//    propertyEditor->setFactoryForManager(sizeManager->subIntPropertyManager(), spinBoxFactory);
//}

void MainWindow::CreateFilesPropertyBrowser()
{
    filesPropertyEditor_ = new QtTreePropertyBrowser();
    //qDebug() << connect(propertyEditor_, SIGNAL(currentItemChanged(QtBrowserItem*)), this, SLOT(currentItemChanged(QtBrowserItem*)));
    //qDebug() << connect(propertyEditor_, SIGNAL(collapsed(QtBrowserItem*)), this, SLOT(collapsed(QtBrowserItem*)));
    //qDebug() << connect(propertyEditor_, SIGNAL(expanded(QtBrowserItem*)), this, SLOT(expanded(QtBrowserItem*)));
}

void MainWindow::CreatePropertyBrowser()
{
    propertyEditor_ = new QtTreePropertyBrowser();
    qDebug() << connect(propertyEditor_, SIGNAL(currentItemChanged(QtBrowserItem*)), this, SLOT(currentItemChanged(QtBrowserItem*)));
    qDebug() << connect(propertyEditor_, SIGNAL(collapsed(QtBrowserItem*)), this, SLOT(collapsed(QtBrowserItem*)));
    qDebug() << connect(propertyEditor_, SIGNAL(expanded(QtBrowserItem*)), this, SLOT(expanded(QtBrowserItem*)));
}

void MainWindow::CreateTreeView()
{
    tree_ = new QTreeView();
    tree_->setDragEnabled(true);
    tree_->setHeaderHidden(true);
    tree_->setMinimumWidth(300);
}

QWidget* MainWindow::CreatePropertiesPanelWidget()
{
    QWidget* propertiesPanelWidget = new QWidget;

    QWidget* filesPropertiesWidget = CreateFilesPropertiesWidget();
    QWidget* propertiesWidget = CreatePropertiesWidget();
    QWidget* hintWidget = CreateHintWidget();

    QSplitter* tabVSplitter = new QSplitter(Qt::Vertical);
    tabVSplitter->addWidget(filesPropertiesWidget);
    tabVSplitter->addWidget(propertiesWidget);
    tabVSplitter->addWidget(hintWidget);
    tabVSplitter->setStretchFactor(0, 0);
    tabVSplitter->setStretchFactor(1, 1);
    tabVSplitter->setStretchFactor(2, 0);

    QVBoxLayout* propertiesPaneLayout = new QVBoxLayout;
    propertiesPaneLayout->addWidget(tabVSplitter);
    propertiesPaneLayout->setContentsMargins(0, 0, 0, 0);

    propertiesPanelWidget->setLayout(propertiesPaneLayout);

    return propertiesPanelWidget;
}

QWidget* MainWindow::CreateFilesPropertiesWidget()
{
    QWidget* propertiesPanelWidget = new QWidget;

    QWidget* hostsButtonsWidget = CreateHostsButtonsWidget();

    QVBoxLayout* propertiesPaneLayout = new QVBoxLayout;
    propertiesPaneLayout->addWidget(hostsButtonsWidget);
    propertiesPaneLayout->addWidget(filesPropertyEditor_);
    propertiesPaneLayout->setContentsMargins(0, 0, 0, 0);

    propertiesPanelWidget->setLayout(propertiesPaneLayout);

    return propertiesPanelWidget;
}

QWidget* MainWindow::CreatePropertiesWidget()
{
    QWidget* propertiesPanelWidget = new QWidget;

    QWidget* hostsButtonsWidget = CreateHostsButtonsWidget();

    QVBoxLayout* propertiesPaneLayout = new QVBoxLayout;
    propertiesPaneLayout->addWidget(hostsButtonsWidget);
    propertiesPaneLayout->addWidget(propertyEditor_);
    propertiesPaneLayout->setContentsMargins(0, 0, 0, 0);

    propertiesPanelWidget->setLayout(propertiesPaneLayout);

    return propertiesPanelWidget;
}

QWidget* MainWindow::CreateHostsButtonsWidget()
{
    QHBoxLayout* hBoxLayoutPropertyListButtons = new QHBoxLayout;
    hBoxLayoutPropertyListButtons->setMargin(0);
    hBoxLayoutPropertyListButtons->setContentsMargins(0, 0, 0, 0);

    QComboBox* comboBoxPlatforms = new QComboBox;
    comboBoxPlatforms->addItem("Windows x64");
    hBoxLayoutPropertyListButtons->addWidget(comboBoxPlatforms);
    hBoxLayoutPropertyListButtons->addStretch();

    QToolButton* toolButtonPropertyListAdd = new QToolButton;
    toolButtonPropertyListAdd->setFixedSize(24, 24);
    toolButtonPropertyListAdd->setIconSize(QSize(24, 24));
    toolButtonPropertyListAdd->setIcon(QIcon(":/images/plus.png"));
    //toolButtonPropertyListAdd->setProperty("type", type);
    //toolButtonPropertyListAdd->setProperty("group", static_cast<int>(group));
    //toolButtonPropertyListAdd->setProperty("name", name);
    //toolButtonPropertyListAdd->setProperty("action", "add");
    toolButtonPropertyListAdd->setToolTip(QString::fromLocal8Bit("Добавить хост"));
    hBoxLayoutPropertyListButtons->addWidget(toolButtonPropertyListAdd);
    connect(toolButtonPropertyListAdd, &QToolButton::clicked, this, &MainWindow::on_AddHost_clicked);


    QToolButton* toolButtonPropertyListRemove = new QToolButton;
    toolButtonPropertyListRemove->setFixedSize(24, 24);
    toolButtonPropertyListRemove->setIconSize(QSize(24, 24));
    toolButtonPropertyListRemove->setIcon(QIcon(":/images/minus.png"));
    //toolButtonPropertyListAdd->setProperty("type", type);
    //toolButtonPropertyListAdd->setProperty("group", static_cast<int>(group));
    //toolButtonPropertyListAdd->setProperty("name", name);
    //toolButtonPropertyListAdd->setProperty("action", "add");
    toolButtonPropertyListRemove->setToolTip(QString::fromLocal8Bit("Удалить хост"));
    hBoxLayoutPropertyListButtons->addWidget(toolButtonPropertyListRemove);
    connect(toolButtonPropertyListAdd, &QToolButton::clicked, this, &MainWindow::on_RemoveHost_clicked);


    QFrame* widgetPropertyListButtons = new QFrame;
    widgetPropertyListButtons->setLayout(hBoxLayoutPropertyListButtons);
    widgetPropertyListButtons->setFrameShape(QFrame::NoFrame);

    return widgetPropertyListButtons;
}

QWidget* MainWindow::CreateHintWidget()
{
    QWidget* hintWidget = new QWidget;
    plainTextEditHint_ = new QPlainTextEdit;
    plainTextEditHint_->setFixedHeight(100);
    plainTextEditHint_->setReadOnly(true);
    QVBoxLayout* vBoxLayoutHint = new QVBoxLayout;
    vBoxLayoutHint->setMargin(0);
    vBoxLayoutHint->addWidget(plainTextEditHint_);
    vBoxLayoutHint->setContentsMargins(0, 0, 0, 0);
    hintWidget->setLayout(vBoxLayoutHint);
    return hintWidget;
}

void MainWindow::FillTreeView()
{
    tree_item_model* model = new tree_item_model();

    QMap<QString, QSet<QString>> categoriesMap;
    for (const auto& up : unitParameters_)
    {
        QString category = "Default";
        if (up.fileInfo.info.category != "")
            category = QString::fromStdString(up.fileInfo.info.category).toLower();
        categoriesMap[category].insert(QString::fromStdString(up.fileInfo.info.id));
    }

    int row = 0;
    for (const auto& cat : categoriesMap.toStdMap())
    {
        QStandardItem* item = new QStandardItem(cat.first);
        item->setEditable(false);
        item->setDragEnabled(false);
        model->setItem(row, 0, item);
        int col = 0;
        for (const auto& id : cat.second)
        {
            QStandardItem* child = new QStandardItem(id);
            child->setEditable(false);

            QPixmap px;
            bool loaded = false;
            if (unitParameters_[id].fileInfo.info.pictogram != "")
            {
                std::string s = base64_decode(unitParameters_[id].fileInfo.info.pictogram);
                QByteArray ba(s.c_str(), static_cast<int>(s.size()));
                try
                {
                    loaded = px.loadFromData(ba);
                }
                catch (...)
                {
                    loaded = false;
                }
            }
            if (!loaded)
                px.load(":/images/ice.png");
            child->setIcon(QIcon(px));
            
            //child->setData(QPoint(row, col), Qt::UserRole + 1);
            item->appendRow(child);
            col++;
        }
        model->setItem(row, 0, item);
        row++;
    }
    tree_->setModel(model);
    tree_->expandAll();

    //tree_item_model* model = new tree_item_model();
    ////QStandardItemModel* model = new QStandardItemModel( 5, 1 );
    //for (int r = 0; r < 5; r++)
    //    for (int c = 0; c < 1; c++)
    //    {
    //        QStandardItem* item = new QStandardItem(QString("Category %0").arg(r));
    //        item->setEditable(false);
    //        item->setDragEnabled(false);
    //        if (c == 0)
    //            for (int i = 0; i < 3; i++)
    //            {
    //                QStandardItem* child = new QStandardItem(QString("Item %0").arg(i));
    //                child->setEditable(false);

    //                QFile f("c:/QtProjects/cubes/resource/plus.png");
    //                if (!f.open(QIODevice::ReadOnly))
    //                    return; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //                QByteArray ba = f.readAll();
    //                QPixmap px;
    //                bool isLoaded = px.loadFromData(ba, "PNG", Qt::AutoColor);
    //                QIcon ico(px);
    //                child->setIcon(ico);
    //                //child->setIcon(QIcon("c:/QtProjects/cubes/resource/plus.png"));

    //                child->setData(QPoint(r, i), Qt::UserRole + 1);

    //                item->appendRow(child);
    //            }

    //        model->setItem(r, c, item);
    //    }

    //tree_->setModel(model);
}

void MainWindow::FillParametersInfo()
{
    QString directoryPath("c:/QtProjects/cubes/build/bin/doc/all_units_solid");
    QStringList platformDirs;
    QDirIterator directories(directoryPath, QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);
    while (directories.hasNext()) {
        directories.next();
        platformDirs << directories.filePath();
    }

    for (const auto& platformDir : platformDirs)
    {
        QString ymlPath = QDir(platformDir).filePath("yml");
        if (QFileInfo(ymlPath).exists() && QFileInfo(ymlPath).isDir())
        {
            QStringList propFiles = QDir(ymlPath).entryList(QStringList() << "*.yml" << "*.json", QDir::Files);
            foreach(QString filename, propFiles)
            {
                QString fullPath = QDir(ymlPath).filePath(filename);
                parameters_compiler::file_info fi{};
                if (!yaml::parser::parse(fullPath.toStdString(), fi))
                {
                    //!!!!!!!!!!!!!!!
                }
                auto& up = unitParameters_[QString::fromStdString(fi.info.id)];
                up.fileInfo = fi;
                up.platforms.insert(QFileInfo(platformDir).baseName());
            }
        }
    }
}

unit_types::UnitParameters MainWindow::GetUnitParameters(QString id) const
{
    for (const auto& up : unitParameters_)
    {
        if (QString::fromStdString(up.fileInfo.info.id) == id)
            return up;
    }
    return {};
}

QGraphicsItemGroup *group;
void MainWindow::MyFirstBtnClicked()
{

    // Group all selected items together
    group = scene_->createItemGroup(scene_->selectedItems());

    // Destroy the group, and delete the group item
    scene_->destroyItemGroup(group);




//    qDebug() <<"xxxxxxxxx";
//    QtProperty *property;

//    QtProperty *mainProperty = groupManager->addProperty("Item1");

//    property = doubleManager->addProperty(tr("Position X"));
//    doubleManager->setRange(property, 0, 100);
//    doubleManager->setValue(property, 50);
//    addProperty(property, QLatin1String("xpos"));
//    mainProperty->addSubProperty(property);

//    property = doubleManager->addProperty(tr("Position Y"));
//    doubleManager->setRange(property, 0, 100);
//    doubleManager->setValue(property, 70);
//    addProperty(property, QLatin1String("ypos"));
//    mainProperty->addSubProperty(property);

//    property = doubleManager->addProperty(tr("Position Z"));
//    doubleManager->setRange(property, 0, 256);
//    doubleManager->setValue(property, 33);
//    addProperty(property, QLatin1String("zpos"));
//    mainProperty->addSubProperty(property);

//    property = colorManager->addProperty(tr("Color"));
//    colorManager->setValue(property, Qt::GlobalColor::darkRed);
//    addProperty(property, QLatin1String("color"));
//    mainProperty->addSubProperty(property);

//    addProperty(mainProperty, QLatin1String("Item1-XXX"));

    //updateExpandState();

}

void MainWindow::selectionChanged()
{
    if (scene_->selectedItems().count() > 0)
    {
        diagram_item* di = (diagram_item*)(scene_->selectedItems()[0]);
        di->getProperties()->ApplyToBrowser(propertyEditor_);
        di->getProperties()->PositionChanged(di->pos());
        di->getProperties()->ZOrderChanged(di->zValue());
        //di->getProperties()->applyExpandState(propertyEditor_);
    }
    else
    {
        propertyEditor_->clear();
        plainTextEditHint_->setPlainText("");
    }


    //propertyEditor->clear();
    //if (scene_->selectedItems().count() > 0)
    //{
    //    //diagram_item* gi = qobject_cast<diagram_item*>(sp_scene_->selectedItems()[0]);
    //    diagram_item* di = (diagram_item*)(scene_->selectedItems()[0]);
    //    qDebug() << di->getName();

    //    QtProperty* mainGroup = groupManager->addProperty(di->getName());

    //    QtProperty* propertiesGroup = groupManager->addProperty(QString::fromLocal8Bit("Свойства"));
    //    mainGroup->addSubProperty(propertiesGroup);

    //    bool found = false;
    //    UnitParameters unitParameters{};
    //    for (const auto& up : unitParameters_)
    //    {
    //        if (QString::fromStdString(up.fiileInfo.info.id) == di->getName())
    //        {
    //            found = true;
    //            unitParameters = up;
    //            break;
    //        }
    //    }

    //    if (found)
    //    {
    //        for (const auto& pi : unitParameters.fiileInfo.parameters)
    //        {
    //            QtProperty* channelsGroup = stringManager->addProperty(QString::fromStdString(pi.display_name));
    //            propertiesGroup->addSubProperty(channelsGroup);
    //            //addProperty(channelsGroup, QLatin1String("Channels"));
    //        }
    //    }


    //    //QtProperty* channelsGroup = groupManager->addProperty(QString::fromLocal8Bit("Каналы"));
    //    //propertiesGroup->addSubProperty(channelsGroup);
    //    //addProperty(channelsGroup, QLatin1String("Channels"));

    //    //QtProperty* channelsCountProperty = intManager->addProperty(QString::fromLocal8Bit("Количество"));
    //    //intManager->setRange(channelsCountProperty, 0, 10000);
    //    //intManager->setValue(channelsCountProperty, 0);
    //    //addProperty(channelsCountProperty, QLatin1String("Count"));
    //    //channelsGroup->addSubProperty(channelsCountProperty);


    //    QtProperty* editorGroup = groupManager->addProperty(QString::fromLocal8Bit("Редактор"));
    //    mainGroup->addSubProperty(editorGroup);


    //    QtProperty* positionXProperty = doubleManager->addProperty("Position X");
    //    doubleManager->setRange(positionXProperty, -10000, 10000);
    //    doubleManager->setValue(positionXProperty, di->scenePos().x());
    //    addProperty(positionXProperty, QLatin1String("Position X"));
    //    editorGroup->addSubProperty(positionXProperty);

    //    QtProperty* positionYProperty = doubleManager->addProperty("Position Y");
    //    doubleManager->setRange(positionYProperty, -10000, 10000);
    //    doubleManager->setValue(positionYProperty, di->scenePos().y());
    //    addProperty(positionYProperty, QLatin1String("Position Y"));
    //    editorGroup->addSubProperty(positionYProperty);


    //    addProperty(mainGroup, di->getName());
    //    propertyEditor->addProperty(mainGroup);
    //}

}

void MainWindow::test2(QPointF ppp)
{
    qDebug() << "!!!!!111111111111111111111!!!!!!!!!!!!!!!";
//    diagram_item *newIcon = new diagram_item(QIcon("c:/QtProjects/cubes/resource/plus.png").pixmap(48,48));
//    QPoint position = (sp_scene_->selectedItems()[0]->pos()-QPoint(24,24)).toPoint();
//    sp_scene_->addItem(newIcon);
//    sp_scene_->clearSelection();
//    newIcon->setPos(position);
//    newIcon->setSelected(true);
}

void MainWindow::itemPositionChanged(QString id, QPointF newPos)
{
    //QtProperty* p = idToProperty[id];
    //if (p != nullptr)
    //{
    //    for (auto sp : p->subProperties())
    //    {
    //        if (sp->propertyName() == "Position X")
    //        {
    //            qDebug() << sp->valueText();
    //            doubleManager->setValue(sp, newPos.x());
    //        }
    //        if (sp->propertyName() == "Position Y")
    //        {
    //            qDebug() << sp->valueText();
    //            doubleManager->setValue(sp, newPos.y());
    //        }
    //    }
    //}
}

void MainWindow::itemCreated(QString id, diagram_item* item)
{

}

void MainWindow::collapsed(QtBrowserItem* item)
{
    if (scene_->selectedItems().count() > 0)
    {
        diagram_item* di = (diagram_item*)(scene_->selectedItems()[0]);
        di->getProperties()->ExpandedChanged(item->property(), false);
    }
}

void MainWindow::expanded(QtBrowserItem* item)
{
    if (scene_->selectedItems().count() > 0)
    {
        diagram_item* di = (diagram_item*)(scene_->selectedItems()[0]);
        di->getProperties()->ExpandedChanged(item->property(), true);
    }
}

//void MainWindow::CreateToolBox()
//{
//    QGridLayout *layout = new QGridLayout;
//    //layout->addWidget(new drag_widget(QPixmap("c:/QtProjects/cubes/resource/boat.png"), "SigmaDriver"), 0, 0);
//    //layout->addWidget(new drag_widget(QPixmap("c:/QtProjects/cubes/resource/boat.png"), "SigmaDriver"), 0, 1);
//    //layout->addWidget(new drag_widget(QPixmap("c:/QtProjects/cubes/resource/boat.png"), "SigmaDriver"), 1, 0);
//    //layout->addWidget(new drag_widget(QPixmap("c:/QtProjects/cubes/resource/boat.png"), "SigmaDriver"), 1, 1);
//    //layout->addWidget(new drag_widget(QPixmap("c:/QtProjects/cubes/resource/boat.png"), "SigmaDriver"), 2, 0);
//    //layout->addWidget(new drag_widget(QPixmap("c:/QtProjects/cubes/resource/boat.png"), "SigmaDriver"), 2, 1);
//    //layout->addWidget(new drag_widget(QPixmap("c:/QtProjects/cubes/resource/boat.png"), "SigmaDriver"), 3, 0);
//    //layout->addWidget(new drag_widget(QPixmap("c:/QtProjects/cubes/resource/boat.png"), "SigmaDriver"), 3, 1);
//
//    layout->setRowStretch(3, 1);
//    layout->setColumnStretch(2, 1);
//
//    QWidget *itemWidget = new QWidget;
//    itemWidget->setLayout(layout);
//
//    //tool_box_->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
//    //tool_box_->setMinimumWidth(itemWidget->sizeHint().width());
//    //tool_box_->addItem(itemWidget, tr("Basic Flowchart Shapes"));
//}



void MainWindow::CreateMenu()
{
    QAction* newAct = new QAction(QString::fromLocal8Bit("Создать"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(QString::fromLocal8Bit("Создать новый файл"));
    connect(newAct, &QAction::triggered, this, &MainWindow::on_NewFile_action);

    QAction* openAct = new QAction(QString::fromLocal8Bit("Открыть"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(QString::fromLocal8Bit("Открыть файл"));
    connect(openAct, &QAction::triggered, this, &MainWindow::on_OpenFile_action);

    QAction* importXmlAct = new QAction(QString::fromLocal8Bit("Импорт xml"), this);
    importXmlAct->setStatusTip(QString::fromLocal8Bit("Импортировать xml файл"));
    connect(importXmlAct, &QAction::triggered, this, &MainWindow::on_ImportXmlFile_action);

    QAction* saveAct = new QAction(QString::fromLocal8Bit("Сохранить"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(QString::fromLocal8Bit("Сохранить файл"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::on_SaveFile_action);

    QAction* saveAsAct = new QAction(QString::fromLocal8Bit("Сохранить как..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(QString::fromLocal8Bit("Сохранить файл как..."));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::on_SaveAsFile_action);

    QAction* quitAct = new QAction(QString::fromLocal8Bit("Выйти"), this);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(QString::fromLocal8Bit("Выйти из приложения"));
    connect(quitAct, &QAction::triggered, this, &MainWindow::on_Quit_action);

    QMenu* fileMenu = menuBar()->addMenu(QString::fromLocal8Bit("Файл"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(importXmlAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    //recentMenu_ = fileMenu->addMenu(QString::fromLocal8Bit("Недавние файлы"));
    //fileMenu->addSeparator();
    fileMenu->addAction(quitAct);

}

void MainWindow::on_NewFile_action()
{
    if (modified_)
    {
        QMessageBox::StandardButton resBtn = QMessageBox::question(this, "parameters_composer",
            QString::fromLocal8Bit("Вы действительно хотите создать новый файл?\nВсе несохраненные изменения будут потеряны!"), QMessageBox::No | QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes)
            return;
    }

    //while (tabWidget_->count() > 0)
    //    tabWidget_->removeTab(0);

    //QWidget* widgetTabMain = CreateMainTabWidget();
    //tabWidget_->addTab(widgetTabMain, "Main");

    //fileInfo_ = {};

    //currentFileName_ = "";
    //modified_ = false;
    //UpdateWindowTitle();
}

void MainWindow::on_OpenFile_action()
{
    if (modified_)
    {
        QMessageBox::StandardButton resBtn = QMessageBox::question(this, "parameters_composer",
            QString::fromLocal8Bit("Вы действительно хотите открыть файл?\nВсе несохраненные изменения будут потеряны!"), QMessageBox::No | QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes)
            return;
    }

    //QFileDialog dialog(this);
    //dialog.setNameFilters({ "Parameters Compiler YAML Files (*.yml *.yaml)", "Parameters Compiler JSON Files (*.json)" });
    //dialog.setAcceptMode(QFileDialog::AcceptOpen);

    //QStringList fileNames;
    //if (dialog.exec())
    //    fileNames = dialog.selectedFiles();

    //if (fileNames.size() == 0)
    //    return;

    //bool is_json = (dialog.selectedNameFilter() == "Parameters Compiler JSON Files (*.json)");

    //OpenFileInternal(fileNames[0], is_json);
}

void MainWindow::on_ImportXmlFile_action()
{
    //if (modified_)
    //{
    //    QMessageBox::StandardButton resBtn = QMessageBox::question(this, "parameters_composer",
    //        QString::fromLocal8Bit("Вы действительно хотите открыть файл?\nВсе несохраненные изменения будут потеряны!"), QMessageBox::No | QMessageBox::Yes);
    //    if (resBtn != QMessageBox::Yes)
    //        return;
    //}

    QFileDialog dialog(this);
    dialog.setNameFilter("Settings XML Files (*.xml)");
    dialog.setAcceptMode(QFileDialog::AcceptOpen);

    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();

    if (fileNames.size() == 0)
        return;

    xml::File f{};
    xml::parser::parse(fileNames[0], f);
    //bool is_json = (dialog.selectedNameFilter() == "Parameters Compiler JSON Files (*.json)");

    //OpenFileInternal(fileNames[0], is_json);
}

void MainWindow::on_SaveFile_action()
{
    if (!modified_)
        return;

    //if (currentFileName_ == "")
    //{
    //    SaveAs();
    //}
    //else
    //{
    //    qDebug() << currentFileName_;

    //    QMessageBox::StandardButton resBtn = QMessageBox::question(this, "parameters_composer",
    //        QString::fromLocal8Bit("Вы действительно хотите сохранить файл?\nФайл будет перезаписан!"), QMessageBox::No | QMessageBox::Yes);
    //    if (resBtn != QMessageBox::Yes)
    //        return;

    //    if (!ReadCurrentFileInfo())
    //        return;

    //    std::string message;
    //    if (!parameters_compiler::helper::validate(fileInfo_, message))
    //    {
    //        QMessageBox::critical(this, "Validate error", QString::fromLocal8Bit(message.c_str()));
    //        return;
    //    }

    //    bool have_type_loop = false;
    //    if (!parameters_compiler::helper::rearrange_types(fileInfo_, have_type_loop))
    //    {
    //        QMessageBox::critical(this, "Rearrange error", QString::fromLocal8Bit("Ошибка переупорядочивания пользовательских типов перед сохранением"));
    //        return;
    //    }
    //    else if (have_type_loop)
    //    {
    //        QMessageBox::warning(this, "Rearrange", QString::fromLocal8Bit("Обнаружена циклицеская зависимость в типах.\nФайл будет сохранен, но эта логическая ошибка требует исправления!"));
    //    }

    //    if (is_json_)
    //    {
    //        if (!json::writer::write(currentFileName_.toStdString(), fileInfo_))
    //            return;
    //    }
    //    else
    //    {
    //        if (!yaml::writer::write(currentFileName_.toStdString(), fileInfo_))
    //            return;
    //    }

    //    modified_ = false;
    //    UpdateWindowTitle();
    //}
}

void MainWindow::on_SaveAsFile_action()
{
    //SaveAs();
}

void MainWindow::on_Quit_action()
{
    if (modified_)
    {
        QMessageBox::StandardButton resBtn = QMessageBox::question(this, "parameters_composer",
            QString::fromLocal8Bit("Вы действительно хотите выйти?\nВсе несохраненные изменения будут потеряны!"), QMessageBox::No | QMessageBox::Yes);
        if (resBtn == QMessageBox::Yes)
            QApplication::quit();
    }
    else
    {
        QApplication::quit();
    }
}

void MainWindow::on_AddHost_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, "Add host", QString::fromLocal8Bit("Имя хоста:"), QLineEdit::Normal, "", &ok);
    if (!ok || text.isEmpty())
        return;
}

void MainWindow::on_RemoveHost_clicked()
{
    QMessageBox::StandardButton resBtn = QMessageBox::question(this, "parameters_composer",
        QString::fromLocal8Bit("Вы действительно хотите выйти?\nВсе несохраненные изменения будут потеряны!"), QMessageBox::No | QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes)
        QApplication::quit();
}

void MainWindow::currentItemChanged(QtBrowserItem* item)
{
    if (item != nullptr)
        qDebug() << item->property()->propertyName();

    if (scene_->selectedItems().size() > 0)
    {
        auto di = reinterpret_cast<diagram_item*>(scene_->selectedItems()[0]);
        if (item != nullptr)
            plainTextEditHint_->setPlainText(di->getProperties()->GetPropertyDescription(item->property()));
        else
            plainTextEditHint_->setPlainText("");
    }
}


//
//void MainWindow::valueChanged(QtProperty* property, int value)
//{
//    qDebug() << "valueChanged value = " << value;
//
//    if (!idToProperty.contains("Channels"))
//        return;
//
//    QtProperty* channelsGroup = idToProperty["Channels"];
//
//    while (value > channelsGroup->subProperties().size() - 1)
//    {
//        QtProperty* channelsCountProperty = intManager->addProperty(QString::fromLocal8Bit("Item %1").arg(channelsGroup->subProperties().size()));
//        intManager->setRange(channelsCountProperty, 0, 10000);
//        intManager->setValue(channelsCountProperty, 0);
//        addProperty(channelsCountProperty, QString::fromLocal8Bit("Item %1").arg(channelsGroup->subProperties().size()));
//        channelsGroup->addSubProperty(channelsCountProperty);
//    }
//
//    while (value < channelsGroup->subProperties().size() - 1)
//    {
//        QtProperty* channelsCountProperty = channelsGroup->subProperties()[channelsGroup->subProperties().size() - 1];
//        //removeProperty(channelsCountProperty, QString::fromLocal8Bit("Item %1").arg(channelsGroup->subProperties().size()));
//
//        channelsGroup->removeSubProperty(channelsCountProperty);
//    }
//
//
//}
//
//void MainWindow::valueChanged(QtProperty *property, double value)
//{
//    qDebug() << "valueChanged value = " << value;
//
//    if (!propertyToId.contains(property))
//        return;
//
//    //if (!currentItem)
//    //    return;
//
//    if (scene_->selectedItems().count() > 0 && !scene_->isItemMoving())
//    {
//        //diagram_item* gi = qobject_cast<diagram_item*>(sp_scene_->selectedItems()[0]);
//        diagram_item* gi = (diagram_item*)(scene_->selectedItems()[0]);
//        qDebug() << gi->getName();
//
//        QString id = propertyToId[property];
//        if (id == "Position X")
//            gi->setX(value);
//        else if (id == "Position Y")
//            gi->setY(value);
//    }
//
//
//    //QString id = propertyToId[property];
//    //if (id == QLatin1String("xpos")) {
//    //    currentItem->setX(value);
//    //} else if (id == QLatin1String("ypos")) {
//    //    currentItem->setY(value);
//    //} else if (id == QLatin1String("zpos")) {
//    //    currentItem->setZ(value);
//    //}
//    //canvas->update();
//}
//
//void MainWindow::valueChanged(QtProperty *property, const QString &value)
//{
////    if (!propertyToId.contains(property))
////        return;
//
////    if (!currentItem)
////        return;
//
////    QString id = propertyToId[property];
////    if (id == QLatin1String("text")) {
////        if (currentItem->rtti() == QtCanvasItem::Rtti_Text) {
////            QtCanvasText *i = (QtCanvasText *)currentItem;
////            i->setText(value);
////        }
////    }
////    canvas->update();
//}
//
//void MainWindow::valueChanged(QtProperty *property, const QColor &value)
//{
////    if (!propertyToId.contains(property))
////        return;
//
////    if (!currentItem)
////        return;
//
////    QString id = propertyToId[property];
////    if (id == QLatin1String("color")) {
////        if (currentItem->rtti() == QtCanvasItem::Rtti_Text) {
////            QtCanvasText *i = (QtCanvasText *)currentItem;
////            i->setColor(value);
////        }
////    } else if (id == QLatin1String("brush")) {
////        if (currentItem->rtti() == QtCanvasItem::Rtti_Rectangle ||
////                currentItem->rtti() == QtCanvasItem::Rtti_Ellipse) {
////            QtCanvasPolygonalItem *i = (QtCanvasPolygonalItem *)currentItem;
////            QBrush b = i->brush();
////            b.setColor(value);
////            i->setBrush(b);
////        }
////    } else if (id == QLatin1String("pen")) {
////        if (currentItem->rtti() == QtCanvasItem::Rtti_Rectangle ||
////                currentItem->rtti() == QtCanvasItem::Rtti_Line) {
////            QtCanvasPolygonalItem *i = (QtCanvasPolygonalItem *)currentItem;
////            QPen p = i->pen();
////            p.setColor(value);
////            i->setPen(p);
////        }
////    }
////    canvas->update();
//}
//
//void MainWindow::valueChanged(QtProperty *property, const QFont &value)
//{
////    if (!propertyToId.contains(property))
////        return;
//
////    if (!currentItem)
////        return;
//
////    QString id = propertyToId[property];
////    if (id == QLatin1String("font")) {
////        if (currentItem->rtti() == QtCanvasItem::Rtti_Text) {
////            QtCanvasText *i = (QtCanvasText *)currentItem;
////            i->setFont(value);
////        }
////    }
////    canvas->update();
//}
//
//void MainWindow::valueChanged(QtProperty *property, const QPoint &value)
//{
////    if (!propertyToId.contains(property))
////        return;
//
////    if (!currentItem)
////        return;
//
////    QString id = propertyToId[property];
////    if (currentItem->rtti() == QtCanvasItem::Rtti_Line) {
////        QtCanvasLine *i = (QtCanvasLine *)currentItem;
////        if (id == QLatin1String("endpoint")) {
////            i->setPoints(i->startPoint().x(), i->startPoint().y(), value.x(), value.y());
////        }
////    }
////    canvas->update();
//}
//
//void MainWindow::valueChanged(QtProperty *property, const QSize &value)
//{
////    if (!propertyToId.contains(property))
////        return;
//
////    if (!currentItem)
////        return;
//
////    QString id = propertyToId[property];
////    if (id == QLatin1String("size")) {
////        if (currentItem->rtti() == QtCanvasItem::Rtti_Rectangle) {
////            QtCanvasRectangle *i = (QtCanvasRectangle *)currentItem;
////            i->setSize(value.width(), value.height());
////        } else if (currentItem->rtti() == QtCanvasItem::Rtti_Ellipse) {
////            QtCanvasEllipse *i = (QtCanvasEllipse *)currentItem;
////            i->setSize(value.width(), value.height());
////        }
////    }
////    canvas->update();
//}
