#include "main_window.h"
#include "diagram_view.h"

#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "tree_item_model.h"
#include "diagram_scene.h"
#include "diagram_item.h"
#include "yaml_parser.h"
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


#include <QStandardItemModel>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    modified_ = false;

    CreateUi();
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
    CreatePropertyBrowser();
    CreateTreeView();
    FillParametersInfo();
    FillTreeView();

    plainTextEditHint_ = qobject_cast<QPlainTextEdit*>(CreateHintWidget());
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
    scene_ = new diagram_scene();
    scene_->setSceneRect(-10000, -10000, 10000, 10000);

    qDebug() << connect(scene_, &diagram_scene::itemPositionChanged, this, &MainWindow::itemPositionChanged);
    qDebug() << connect(scene_, &diagram_scene::itemCreated, this, &MainWindow::itemCreated);
    qDebug() << connect(scene_, &diagram_scene::selectionChanged, this, &MainWindow::selectionChanged);
}

void MainWindow::CreateView()
{
    view_ = new diagram_view(scene_);
    view_->setDragMode(QGraphicsView::RubberBandDrag);
}

void MainWindow::CreatePropertyBrowser()
{
    groupManager = new QtGroupPropertyManager(this);
    doubleManager = new QtDoublePropertyManager(this);
    stringManager = new QtStringPropertyManager(this);
    colorManager = new QtColorPropertyManager(this);
    fontManager = new QtFontPropertyManager(this);
    pointManager = new QtPointPropertyManager(this);
    sizeManager = new QtSizePropertyManager(this);

    qDebug() << connect(doubleManager, SIGNAL(valueChanged(QtProperty*, double)), this, SLOT(valueChanged(QtProperty*, double)));
    qDebug() << connect(stringManager, SIGNAL(valueChanged(QtProperty*, const QString&)), this, SLOT(valueChanged(QtProperty*, const QString&)));
    qDebug() << connect(colorManager, SIGNAL(valueChanged(QtProperty*, const QColor&)), this, SLOT(valueChanged(QtProperty*, const QColor&)));
    qDebug() << connect(fontManager, SIGNAL(valueChanged(QtProperty*, const QFont&)), this, SLOT(valueChanged(QtProperty*, const QFont&)));
    qDebug() << connect(pointManager, SIGNAL(valueChanged(QtProperty*, const QPoint&)), this, SLOT(valueChanged(QtProperty*, const QPoint&)));
    qDebug() << connect(sizeManager, SIGNAL(valueChanged(QtProperty*, const QSize&)), this, SLOT(valueChanged(QtProperty*, const QSize&)));

    QtDoubleSpinBoxFactory* doubleSpinBoxFactory = new QtDoubleSpinBoxFactory(this);
    QtCheckBoxFactory* checkBoxFactory = new QtCheckBoxFactory(this);
    QtSpinBoxFactory* spinBoxFactory = new QtSpinBoxFactory(this);
    QtLineEditFactory* lineEditFactory = new QtLineEditFactory(this);
    QtEnumEditorFactory* comboBoxFactory = new QtEnumEditorFactory(this);

    propertyEditor = new QtTreePropertyBrowser();
    propertyEditor->setFactoryForManager(doubleManager, doubleSpinBoxFactory);
    propertyEditor->setFactoryForManager(stringManager, lineEditFactory);
    propertyEditor->setFactoryForManager(colorManager->subIntPropertyManager(), spinBoxFactory);
    propertyEditor->setFactoryForManager(fontManager->subIntPropertyManager(), spinBoxFactory);
    propertyEditor->setFactoryForManager(fontManager->subBoolPropertyManager(), checkBoxFactory);
    propertyEditor->setFactoryForManager(fontManager->subEnumPropertyManager(), comboBoxFactory);
    propertyEditor->setFactoryForManager(pointManager->subIntPropertyManager(), spinBoxFactory);
    propertyEditor->setFactoryForManager(sizeManager->subIntPropertyManager(), spinBoxFactory);
}

void MainWindow::CreateTreeView()
{
    tree_ = new QTreeView();
    tree_->setDragEnabled(true);
    tree_->setHeaderHidden(true);
}

QWidget* MainWindow::CreatePropertiesPanelWidget()
{
    QWidget* propertiesPanelWidget = new QWidget;

    QWidget* propertieslWidget = CreatePropertieslWidget();
    QWidget* hintWidget = CreateHintWidget();

    QSplitter* tabVSplitter = new QSplitter(Qt::Vertical);
    tabVSplitter->addWidget(propertieslWidget);
    tabVSplitter->addWidget(hintWidget);
    tabVSplitter->setStretchFactor(0, 1);
    tabVSplitter->setStretchFactor(1, 0);

    QVBoxLayout* propertiesPaneLayout = new QVBoxLayout;
    propertiesPaneLayout->addWidget(tabVSplitter);
    propertiesPaneLayout->setContentsMargins(0, 0, 0, 0);

    propertiesPanelWidget->setLayout(propertiesPaneLayout);

    return propertiesPanelWidget;
}

QWidget* MainWindow::CreatePropertieslWidget()
{
    QWidget* propertiesPanelWidget = new QWidget;

    QWidget* hostsButtonsWidget = CreateHostsButtonsWidget();
    QWidget* hintWidget = CreateHintWidget();

    QVBoxLayout* propertiesPaneLayout = new QVBoxLayout;
    propertiesPaneLayout->addWidget(hostsButtonsWidget);
    propertiesPaneLayout->addWidget(propertyEditor);
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
    toolButtonPropertyListAdd->setToolTip(QString::fromLocal8Bit("�������� %1").arg("toolTipBase"));
    hBoxLayoutPropertyListButtons->addWidget(toolButtonPropertyListAdd);
    //connect(toolButtonPropertyListAdd, &QToolButton::clicked, this, &MainWindow::on_ListControlClicked);


    QToolButton* toolButtonPropertyListRemove = new QToolButton;
    toolButtonPropertyListRemove->setFixedSize(24, 24);
    toolButtonPropertyListRemove->setIconSize(QSize(24, 24));
    toolButtonPropertyListRemove->setIcon(QIcon(":/images/minus.png"));
    //toolButtonPropertyListAdd->setProperty("type", type);
    //toolButtonPropertyListAdd->setProperty("group", static_cast<int>(group));
    //toolButtonPropertyListAdd->setProperty("name", name);
    //toolButtonPropertyListAdd->setProperty("action", "add");
    toolButtonPropertyListRemove->setToolTip(QString::fromLocal8Bit("������� %1").arg("toolTipBase"));
    hBoxLayoutPropertyListButtons->addWidget(toolButtonPropertyListRemove);
    //connect(toolButtonPropertyListAdd, &QToolButton::clicked, this, &MainWindow::on_ListControlClicked);


    QFrame* widgetPropertyListButtons = new QFrame;
    widgetPropertyListButtons->setLayout(hBoxLayoutPropertyListButtons);
    widgetPropertyListButtons->setFrameShape(QFrame::NoFrame);

    return widgetPropertyListButtons;
}

QWidget* MainWindow::CreateHintWidget()
{
    QWidget* hintWidget = new QWidget;
    QPlainTextEdit* plainTextEditHint = new QPlainTextEdit;
    plainTextEditHint->setFixedHeight(100);
    plainTextEditHint->setReadOnly(true);
    QVBoxLayout* vBoxLayoutHint = new QVBoxLayout;
    vBoxLayoutHint->setMargin(0);
    vBoxLayoutHint->addWidget(plainTextEditHint);
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
        if (up.fiileInfo.info.category != "")
            category = QString::fromStdString(up.fiileInfo.info.category).toLower();
        categoriesMap[category].insert(QString::fromStdString(up.fiileInfo.info.id));
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

            if (unitParameters_[id].fiileInfo.info.pictogram != "")
            {
                std::string s = base64_decode(unitParameters_[id].fiileInfo.info.pictogram);
                QByteArray ba(s.c_str(), s.size());
                QPixmap px;
                if (!px.loadFromData(ba, "PNG", Qt::AutoColor))
                    child->setIcon(QIcon(":/images/minus.png"));
                else
                    child->setIcon(QIcon(px));
            }
            else
                child->setIcon(QIcon(":/images/minus.png"));
            
            //child->setData(QPoint(row, col), Qt::UserRole + 1);
            item->appendRow(child);
            col++;
        }
        model->setItem(row, 0, item);
        row++;
    }
    tree_->setModel(model);


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
                up.fiileInfo = fi;
                up.platforms.insert(QFileInfo(platformDir).baseName());
            }
        }
    }
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
    propertyEditor->clear();
    if (scene_->selectedItems().count() > 0)
    {
        //diagram_item* gi = qobject_cast<diagram_item*>(sp_scene_->selectedItems()[0]);
        diagram_item* gi = (diagram_item*)(scene_->selectedItems()[0]);
        qDebug() << gi->getName();

        QtProperty *property;
        QtProperty *mainProperty = groupManager->addProperty(gi->getName());

        property = doubleManager->addProperty(tr("Position X"));
        doubleManager->setRange(property, -10000, 10000);
        doubleManager->setValue(property, gi->scenePos().x());
        addProperty(property, QLatin1String("Position X"));
        mainProperty->addSubProperty(property);

        property = doubleManager->addProperty(tr("Position Y"));
        doubleManager->setRange(property, -10000, 10000);
        doubleManager->setValue(property, gi->scenePos().y());
        addProperty(property, QLatin1String("Position Y"));
        mainProperty->addSubProperty(property);

        addProperty(mainProperty, gi->getName());
        propertyEditor->addProperty(mainProperty);
    }

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
    QtProperty* p = idToProperty[id];
    if (p != nullptr)
    {
        for (auto sp : p->subProperties())
        {
            if (sp->propertyName() == "Position X")
            {
                qDebug() << sp->valueText();
                doubleManager->setValue(sp, newPos.x());
            }
            if (sp->propertyName() == "Position Y")
            {
                qDebug() << sp->valueText();
                doubleManager->setValue(sp, newPos.y());
            }
        }
    }
    qDebug() << "!!!!!!!!!!!!!!" << newPos;

}

void MainWindow::itemCreated(QString id, diagram_item* item)
{

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

void MainWindow::addProperty(QtProperty *property, const QString &id)
{
    propertyToId[property] = id;
    idToProperty[id] = property;
    //QtBrowserItem *item = propertyEditor->addProperty(property);
    //if (idToExpanded.contains(id))
    //    propertyEditor->setExpanded(item, idToExpanded[id]);
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

void MainWindow::CreateMenu()
{
    QAction* newAct = new QAction(QString::fromLocal8Bit("�������"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(QString::fromLocal8Bit("������� ����� ����"));
    connect(newAct, &QAction::triggered, this, &MainWindow::on_NewFile_action);

    QAction* openAct = new QAction(QString::fromLocal8Bit("�������"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(QString::fromLocal8Bit("������� ����"));
    connect(openAct, &QAction::triggered, this, &MainWindow::on_OpenFile_action);

    QAction* saveAct = new QAction(QString::fromLocal8Bit("���������"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(QString::fromLocal8Bit("��������� ����"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::on_SaveFile_action);

    QAction* saveAsAct = new QAction(QString::fromLocal8Bit("��������� ���..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(QString::fromLocal8Bit("��������� ���� ���..."));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::on_SaveAsFile_action);

    QAction* quitAct = new QAction(QString::fromLocal8Bit("�����"), this);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(QString::fromLocal8Bit("����� �� ����������"));
    connect(quitAct, &QAction::triggered, this, &MainWindow::on_Quit_action);

    QMenu* fileMenu = menuBar()->addMenu(QString::fromLocal8Bit("����"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    //recentMenu_ = fileMenu->addMenu(QString::fromLocal8Bit("�������� �����"));
    //fileMenu->addSeparator();
    fileMenu->addAction(quitAct);

}

void MainWindow::on_NewFile_action()
{
    if (modified_)
    {
        QMessageBox::StandardButton resBtn = QMessageBox::question(this, "parameters_composer",
            QString::fromLocal8Bit("�� ������������� ������ ������� ����� ����?\n��� ������������� ��������� ����� ��������!"), QMessageBox::No | QMessageBox::Yes);
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
            QString::fromLocal8Bit("�� ������������� ������ ������� ����?\n��� ������������� ��������� ����� ��������!"), QMessageBox::No | QMessageBox::Yes);
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
    //        QString::fromLocal8Bit("�� ������������� ������ ��������� ����?\n���� ����� �����������!"), QMessageBox::No | QMessageBox::Yes);
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
    //        QMessageBox::critical(this, "Rearrange error", QString::fromLocal8Bit("������ ������������������ ���������������� ����� ����� �����������"));
    //        return;
    //    }
    //    else if (have_type_loop)
    //    {
    //        QMessageBox::warning(this, "Rearrange", QString::fromLocal8Bit("���������� ����������� ����������� � �����.\n���� ����� ��������, �� ��� ���������� ������ ������� �����������!"));
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
            QString::fromLocal8Bit("�� ������������� ������ �����?\n��� ������������� ��������� ����� ��������!"), QMessageBox::No | QMessageBox::Yes);
        if (resBtn == QMessageBox::Yes)
            QApplication::quit();
    }
    else
    {
        QApplication::quit();
    }
}

void MainWindow::valueChanged(QtProperty *property, double value)
{
    qDebug() << "valueChanged value = " << value;

    if (!propertyToId.contains(property))
        return;

    //if (!currentItem)
    //    return;

    if (scene_->selectedItems().count() > 0 && !scene_->isItemMoving())
    {
        //diagram_item* gi = qobject_cast<diagram_item*>(sp_scene_->selectedItems()[0]);
        diagram_item* gi = (diagram_item*)(scene_->selectedItems()[0]);
        qDebug() << gi->getName();

        QString id = propertyToId[property];
        if (id == "Position X")
            gi->setX(value);
        else if (id == "Position Y")
            gi->setY(value);
    }


    //QString id = propertyToId[property];
    //if (id == QLatin1String("xpos")) {
    //    currentItem->setX(value);
    //} else if (id == QLatin1String("ypos")) {
    //    currentItem->setY(value);
    //} else if (id == QLatin1String("zpos")) {
    //    currentItem->setZ(value);
    //}
    //canvas->update();
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
