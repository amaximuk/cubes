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
#include "graph.h"
#include "arrow_item.h"

#include <vector>

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
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    modified_ = false;
    defaultColorIndex_ = 0;
    for (auto& c : defaultColors_)
        c.setAlpha(0x20);

    setWindowIcon(QIcon(":/images/cubes.png"));

    CreateUi();

    auto fi = new files_item();
    fi->SetName(QString::fromLocal8Bit("АРМ"));
    if (defaultColorIndex_ < defaultColors_.size())
        fi->SetColor(defaultColors_[defaultColorIndex_++]);
    else
        fi->SetColor(QColor("White"));
    fi->ApplyToBrowser(filesPropertyEditor_);
    files_items_.push_back(fi);
    comboBoxFiles_->addItem(QString::fromLocal8Bit("АРМ"));
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
    QWidget* widgetMainTab= CreateTabWidget(0);
    tabWidget_->addTab(widgetMainTab, "Main");

    qDebug() << connect(tabWidget_, &QTabWidget::currentChanged, this, &MainWindow::on_Tab_currentChanged);

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

QWidget* MainWindow::CreateTabWidget(int index)
{
    if (panes_.size() != index)
        return nullptr;

    panes_.resize(index + 1);
    CreateScene(index);
    CreateView(index);
    return panes_[index].second;
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

void MainWindow::CreateScene(int index)
{
    panes_[index].first = new diagram_scene(this);
    panes_[index].first->setSceneRect(-10000, -10000, 20032, 20032);

    qDebug() << connect(panes_[index].first, &diagram_scene::itemPositionChanged, this, &MainWindow::itemPositionChanged);
    qDebug() << connect(panes_[index].first, &diagram_scene::afterItemCreated, this, &MainWindow::afterItemCreated);
    qDebug() << connect(panes_[index].first, &diagram_scene::beforeItemDeleted, this, &MainWindow::beforeItemDeleted);
    qDebug() << connect(panes_[index].first, &diagram_scene::selectionChanged, this, &MainWindow::selectionChanged);
    qDebug() << connect(panes_[index].first, &diagram_scene::itemNameChanged, this, &MainWindow::itemNameChanged);
    qDebug() << connect(panes_[index].first, &diagram_scene::itemFileChanged, this, &MainWindow::itemFileChanged);
}

void MainWindow::CreateView(int index)
{
    panes_[index].second = new diagram_view(panes_[index].first, this);
    panes_[index].second->setDragMode(QGraphicsView::RubberBandDrag);
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

    QWidget* hostsButtonsWidget = CreateFilesButtonsWidget();

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

    QWidget* hostsButtonsWidget = CreateUnitsButtonsWidget();

    QVBoxLayout* propertiesPaneLayout = new QVBoxLayout;
    propertiesPaneLayout->addWidget(hostsButtonsWidget);
    propertiesPaneLayout->addWidget(propertyEditor_);
    propertiesPaneLayout->setContentsMargins(0, 0, 0, 0);

    propertiesPanelWidget->setLayout(propertiesPaneLayout);

    return propertiesPanelWidget;
}

QWidget* MainWindow::CreateFilesButtonsWidget()
{
    comboBoxFiles_ = new QComboBox;
    connect(comboBoxFiles_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_Files_currentIndexChanged);

    QHBoxLayout* hBoxLayoutPropertyListButtons = new QHBoxLayout;
    hBoxLayoutPropertyListButtons->setMargin(0);
    hBoxLayoutPropertyListButtons->setContentsMargins(0, 0, 0, 0);

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
    connect(toolButtonPropertyListAdd, &QToolButton::clicked, this, &MainWindow::on_AddFile_clicked);

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
    connect(toolButtonPropertyListRemove, &QToolButton::clicked, this, &MainWindow::on_RemoveFile_clicked);

    QWidget* buttonsWidget = new QWidget;
    buttonsWidget->setLayout(hBoxLayoutPropertyListButtons);

    QSplitter* splitterComboButtons = new QSplitter(Qt::Horizontal);
    splitterComboButtons->addWidget(comboBoxFiles_);
    splitterComboButtons->addWidget(buttonsWidget);
    splitterComboButtons->setStretchFactor(0, 1);
    splitterComboButtons->setStretchFactor(1, 0);

    //QLabel* label = new QLabel;
    //label->setStyleSheet("font-weight: bold; font-size: 14px");
    //label->setText(QString::fromLocal8Bit("Файлы"));

    QWidget* mainWidget = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    //mainLayout->addWidget(label, 1, Qt::AlignCenter);
    mainLayout->addWidget(splitterComboButtons);
    mainWidget->setLayout(mainLayout);
    return mainWidget;

    //QFrame* widgetPropertyListButtons = new QFrame;
    //widgetPropertyListButtons->setLayout(hBoxLayoutPropertyListButtons);
    //widgetPropertyListButtons->setFrameShape(QFrame::NoFrame);
    //return widgetPropertyListButtons;
}


QWidget* MainWindow::CreateUnitsButtonsWidget()
{
    comboBoxUnits_ = new QComboBox;
    comboBoxUnits_->addItem("<not selected>");
    connect(comboBoxUnits_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_Units_currentIndexChanged);

    QWidget* mainWidget = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(comboBoxUnits_);
    mainWidget->setLayout(mainLayout);
    return mainWidget;
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
    //{
    //    QStandardItem* item = new QStandardItem(QString::fromLocal8Bit("Служебные"));
    //    item->setEditable(false);
    //    item->setDragEnabled(false);
    //    model->setItem(row, 0, item);
    //    QStandardItem* child = new QStandardItem(QString::fromLocal8Bit("Группа"));
    //    child->setEditable(false);

    //    QPixmap px;
    //    px.load(":/images/module.png");
    //    child->setIcon(QIcon(px));
    //    item->appendRow(child);
    //}

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
    QString directoryPath(QCoreApplication::applicationDirPath() + "/doc/all_units_solid");
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

QColor MainWindow::GetFileColor(const QString& fileId)
{
    for (auto& fi : files_items_)
    {
        if (fi->GetName() == fileId)
            return fi->GetColor();
    }
    return QColor("Black");
}

QString MainWindow::GetNewUnitName(const QString& baseName)
{
    QString n = baseName;
    int sharp_index = baseName.lastIndexOf("#");
    if (sharp_index != -1)
        n = baseName.left(sharp_index);

    QString name = n;
    int counter = 0;
    while (true)
    {
        bool found = false;
        for (int i = 0; i < panes_.count(); ++i)
        {
            QList<QPair<QString, QString>> variables;

            QString tabName = tabWidget_->tabText(i);
            for (const auto& pi : panes_[0].first->items())
            {
                diagram_item* di = reinterpret_cast<diagram_item*>(pi);
                if (di->getProperties()->GetName() == tabName)
                {
                    variables = di->getProperties()->GetVariables();
                    break;
                }
            }

            for (const auto& item : panes_[i].first->items())
            {
                diagram_item* di = reinterpret_cast<diagram_item*>(item);
                QString realName = di->getName();
                for (const auto& v : variables)
                {
                    QString replace = QString("@%1@").arg(v.first);
                    realName.replace(replace, v.second);
                }
                if (realName == name)
                {
                    found = true;
                    break;
                }
            }
            if (found)
                break;
        }
        if (found)
            name = QString("%1#%2").arg(n).arg(++counter);
        else
            break;
    }
    return name;
}

QString MainWindow::GetDisplayName(const QString& baseName, const QString& groupName)
{
    QList<QPair<QString, QString>> variables;

    for (int i = 0; i < panes_.count(); ++i)
    {
        QString tabName = tabWidget_->tabText(i);
        for (const auto& pi : panes_[0].first->items())
        {
            diagram_item* di = reinterpret_cast<diagram_item*>(pi);
            if (di->getProperties()->GetName() == tabName)
            {
                variables = di->getProperties()->GetVariables();
                break;
            }
        }
    }

    QString realName = baseName;
    for (const auto& v : variables)
    {
        QString replace = QString("@%1@").arg(v.first);
        realName.replace(replace, v.second);
    }

    return realName;
}

QString MainWindow::GetCurrentGroup()
{
    int i = tabWidget_->indexOf(tabWidget_->currentWidget());
    if (i == -1)
        return "";

    return tabWidget_->tabText(i);
}

unit_types::UnitParameters* MainWindow::GetUnitParameters(const QString& id)
{
    for (auto& up : unitParameters_)
    {
        if (QString::fromStdString(up.fileInfo.info.id) == id)
            return &up;
    }
    return nullptr;
}

QStringList MainWindow::GetFileNames()
{
    QStringList result;
    for (auto& fi : files_items_)
        result.push_back(fi->GetName());
    return result;
}

QString MainWindow::GetCurrentFileName()
{
    return comboBoxFiles_->currentText();
}

QGraphicsItemGroup *group;
void MainWindow::MyFirstBtnClicked()
{

    // Group all selected items together
    group = panes_[0].first->createItemGroup(panes_[0].first->selectedItems());

    // Destroy the group, and delete the group item
    panes_[0].first->destroyItemGroup(group);




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
    int i = tabWidget_->indexOf(tabWidget_->currentWidget());
    if (i == -1)
        return;

    if (panes_[i].first->selectedItems().count() > 0)
    {
        diagram_item* di = (diagram_item*)(panes_[i].first->selectedItems()[0]);
        di->getProperties()->ApplyToBrowser(propertyEditor_);
        di->getProperties()->PositionChanged(di->pos());
        di->getProperties()->ZOrderChanged(di->zValue());
        //di->getProperties()->applyExpandState(propertyEditor_);
        comboBoxUnits_->setCurrentText(di->getName());
    }
    else
    {
        propertyEditor_->clear();
        plainTextEditHint_->setPlainText("");
        //comboBoxUnits_->blockSignals(true);
        comboBoxUnits_->setCurrentIndex(0);
        //comboBoxUnits_->blockSignals(false);
    }


    //propertyEditor->clear();
    //if (panes_[0].first->selectedItems().count() > 0)
    //{
    //    //diagram_item* gi = qobject_cast<diagram_item*>(sp_scene_->selectedItems()[0]);
    //    diagram_item* di = (diagram_item*)(panes_[0].first->selectedItems()[0]);
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

void MainWindow::on_Tab_currentChanged(int index)
{
    int i = tabWidget_->indexOf(tabWidget_->currentWidget());
    if (i == -1)
        return;

    if (index == 0)
    {
        comboBoxFiles_->setEnabled(true);
        filesPropertyEditor_->setEnabled(true);
    }
    else
    {
        comboBoxFiles_->setEnabled(false);
        filesPropertyEditor_->setEnabled(false);
    }

    comboBoxUnits_->clear();
    comboBoxUnits_->addItem("<empty>");
    for (const auto& item : panes_[i].first->items())
    {
        diagram_item* di = reinterpret_cast<diagram_item*>(item);
        comboBoxUnits_->addItem(di->getName());
    }
}

void MainWindow::itemPositionChanged(diagram_item* item)
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

void MainWindow::afterItemCreated(diagram_item* item)
{
    if (item->getProperties()->GetId() == "group")
    {
        QWidget* widgetTab = CreateTabWidget(tabWidget_->count());
        tabWidget_->addTab(widgetTab, item->getName());
    }
    comboBoxUnits_->addItem(item->getName());

    int i = tabWidget_->indexOf(tabWidget_->currentWidget());
    if (i > 0)
    {
        QString name = tabWidget_->tabText(i);
        for (const auto& pi : panes_[0].first->items())
        {
            diagram_item* di = reinterpret_cast<diagram_item*>(pi);
            if (di->getProperties()->GetName() == name)
            {
                QString fileName = di->getProperties()->GetFileName();
                item->getProperties()->SetFileName(fileName);
                item->getProperties()->SetFileNameReadOnly();
                break;
            }
        }

    }
}

void MainWindow::beforeItemDeleted(diagram_item* item)
{
    for (int i = 1; i < comboBoxUnits_->count(); i++)
    {
        if (comboBoxUnits_->itemText(i) == item->getName())
        {
            comboBoxUnits_->removeItem(i);
            break;
        }
    }

    if (item->getProperties()->GetId() == "group")
    {
        QString name = item->getProperties()->GetName();
        for (int i = 0; i < tabWidget_->count(); ++i)
        {
            if (tabWidget_->tabText(i) == name)
            {
                tabWidget_->removeTab(i);
                for (auto item : panes_[i].first->items())
                    delete item;
                delete panes_[i].first; // Use Smart pointers!!!
                delete panes_[i].second; // Use Smart pointers!!!
                panes_.removeAt(i);
                break;
            }
        }
    }
}

void MainWindow::itemNameChanged(diagram_item* item, QString oldName)
{
    if (item->getProperties()->GetId() == "group")
    {
        QString name = item->getProperties()->GetName();
        for (int i = 0; i < tabWidget_->count(); ++i)
        {
            if (tabWidget_->tabText(i) == oldName)
            {
                tabWidget_->setTabText(i, name);
                break;
            }
        }
    }
    int i = comboBoxUnits_->findText(oldName);
    if (i != -1)
        comboBoxUnits_->setItemText(i, item->getProperties()->GetName());
}

void MainWindow::itemFileChanged(diagram_item* item)
{
    if (item->getProperties()->GetId() == "group")
    {
        QString name = item->getProperties()->GetName();
        QString fileName = item->getProperties()->GetFileName();
        for (int i = 0; i < tabWidget_->count(); ++i)
        {
            if (tabWidget_->tabText(i) == name)
            {
                for (auto& item : panes_[i].first->items())
                {
                    diagram_item* di = reinterpret_cast<diagram_item*>(item);
                    di->getProperties()->SetFileName(fileName);
                }
                break;
            }
        }
    }
}

void MainWindow::collapsed(QtBrowserItem* item)
{
    if (panes_[0].first->selectedItems().count() > 0)
    {
        diagram_item* di = reinterpret_cast<diagram_item*>(panes_[0].first->selectedItems()[0]);
        di->getProperties()->ExpandedChanged(item->property(), false);
    }
}

void MainWindow::expanded(QtBrowserItem* item)
{
    if (panes_[0].first->selectedItems().count() > 0)
    {
        diagram_item* di = reinterpret_cast<diagram_item*>(panes_[0].first->selectedItems()[0]);
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

    QAction* sortAct = new QAction(QString::fromLocal8Bit("Сортировать"), this);
    sortAct->setStatusTip(QString::fromLocal8Bit("Автоматическая сортировка"));
    connect(sortAct, &QAction::triggered, this, &MainWindow::on_Sort_action);

    QMenu* editMenu = menuBar()->addMenu(QString::fromLocal8Bit("Правка"));
    editMenu->addAction(sortAct);

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

    xml::File f{};
    {
        QFileDialog dialog(this);
        dialog.setNameFilter("Settings XML Files (*.xml)");
        dialog.setAcceptMode(QFileDialog::AcceptOpen);

        QStringList fileNames;
        if (dialog.exec())
            fileNames = dialog.selectedFiles();

        if (fileNames.size() == 0)
            return;

        xml::parser::parse(fileNames[0], f);
    }

    if (f.config.networking_is_set)
    {
        // main


        if (panes_.size() == 1 && panes_[0].first->items().size() == 0)
        {
            filesPropertyEditor_->clear();
            files_items_.clear();
            comboBoxFiles_->clear();
            defaultColorIndex_ = 0;
        }

        auto fi = new files_item();
        fi->SetName(f.fileName);
        if (defaultColorIndex_ < defaultColors_.size())
            fi->SetColor(defaultColors_[defaultColorIndex_++]);
        else
            fi->SetColor(QColor("White"));
        fi->ApplyToBrowser(filesPropertyEditor_);
        files_items_.push_back(fi);
        comboBoxFiles_->addItem(f.fileName);
        comboBoxFiles_->setCurrentIndex(comboBoxFiles_->count() - 1);

        QStringList fileNames;
        for (auto& file : files_items_)
            fileNames.push_back(file->GetName());

        for (int i = 0; i < panes_.count(); ++i)
        {
            for (auto& item : panes_[i].first->items())
                reinterpret_cast<diagram_item*>(item)->getProperties()->SetFileNames(fileNames);
        }

        //int i = tabWidget_->indexOf(tabWidget_->currentWidget());
        //if (i == -1)
        //    return;

        //if (panes_[i].first->selectedItems().size() > 0)
        //    reinterpret_cast<diagram_item*>(panes_[0].first->selectedItems()[0])->getProperties()->ApplyToBrowser(propertyEditor_);















        QDir dir = QFileInfo(fileNames[0]).absoluteDir();

        // Convert includes into unit
        xml::Group g{};
        g.path = "service";
        int unitIndex = 0;
        for (const auto& include : f.includes)
        {
            xml::Unit u{};
            u.id = "group";
            u.name = QString::fromLocal8Bit("Группа %1").arg(unitIndex++);
            xml::Param p{};
            p.name = "FILE_PATH";
            p.type = "str";
            p.val = include.fileName;
            u.params.push_back(std::move(p));
            xml::Array a{};
            a.name = "VARIABLES";
            for (const auto& kvp : include.variables.toStdMap())
            {
                xml::Item i{};
                xml::Param p1{};
                p1.name = "NAME";
                p1.type = "str";
                p1.val = kvp.first;
                i.params.push_back(std::move(p1));
                xml::Param p2{};
                p2.name = "VALUE";
                p2.type = "str";
                p2.val = kvp.second;
                i.params.push_back(std::move(p2));
                a.items.push_back(std::move(i));
            }
            u.arrays.push_back(std::move(a));
            g.units.push_back(std::move(u));
        }

        if (f.includes.size() > 0)
            f.config.groups.push_back(std::move(g));

        //for (int i = 0; i < f.includes.size(); i++)
        //{
        //    QString includedFileName = dir.filePath(f.includes[i].fileName);
        //    xml::File includedFile{};
        //    xml::parser::parse(includedFileName, includedFile);


        //}
    }
    else
    {
        // included config
    }








    QVector<xml::Unit> all_units;
    for (const auto& g : f.config.groups)
    {
        for (const auto& u : g.units)
        {
            QString name = u.id;
            auto up = GetUnitParameters(name);
            if (up != nullptr)
            {
                all_units.push_back(u);
            }
            else
            {
                // !!! error
            }
        }
    }

    // Transform
    for (int i = 0; i < all_units.size(); i++)
    {
        QString name = all_units[i].id;
        auto up = GetUnitParameters(name);

        if (up != nullptr)
        {
            diagram_item* di = new diagram_item(*up);

            di->getProperties()->ApplyXmlProperties(all_units[i]);
            
            QStringList fileNames;
            for (auto& file : files_items_)
                fileNames.push_back(file->GetName());

            di->getProperties()->SetFileNames(fileNames);
            di->getProperties()->SetFileName(f.fileName);

            panes_[0].first->addItem(di);
            panes_[0].first->clearSelection();
        }
        else
        {
            // error
        }
    }

    // Prepare sort
    int nextIndex = 0;
    QMap<QString, int> nameToIndex;
    QMap<int, QString> indexToName;
    QMap<QString, QSet<QString>> connectedNames;
    for (auto& item : panes_[0].first->items())
    {
        diagram_item* di = reinterpret_cast<diagram_item*>(item);

        if (!nameToIndex.contains(di->getProperties()->GetInstanceName()))
        {
            nameToIndex[di->getProperties()->GetInstanceName()] = nextIndex;
            indexToName[nextIndex] = di->getProperties()->GetInstanceName();
            nextIndex++;
        }

        auto connected = di->getConnectedNames();
        connectedNames[di->getProperties()->GetInstanceName()].unite(QSet<QString>(connected.begin(), connected.end()));
    }

    // Sort
    std::vector<std::pair<int, int>> edges;

    for (const auto& kvp : connectedNames.toStdMap())
    {
        for (const auto& se : kvp.second)
        {
            if (nameToIndex.contains(kvp.first) && nameToIndex.contains(se))
                edges.push_back({ nameToIndex[kvp.first], nameToIndex[se] });
        }
    }

    std::vector<std::pair<int, int>> coordinates;
    if (!rearrangeGraph(nameToIndex.size(), edges, coordinates))
    {
        return;
    }

    auto vr = panes_[0].second->mapToScene(panes_[0].second->viewport()->geometry()).boundingRect();
    for (auto& item : panes_[0].first->items())
    {
        diagram_item* di = reinterpret_cast<diagram_item*>(item);
        
        int i = nameToIndex[di->getProperties()->GetInstanceName()];

        QPoint position(vr.left() + 60 + coordinates[i].first * 60,
            vr.top() + 60 + coordinates[i].second * 60);

        int gridSize = 20;
        qreal xV = round(position.x() / gridSize) * gridSize;
        qreal yV = round(position.y() / gridSize) * gridSize;
        position = QPoint(xV, yV);

        di->setPos(position);
        //di->setSelected(true);
    }

    panes_[0].first->invalidate();


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

void MainWindow::on_Sort_action()
{

    int nextIndex = 0;
    QMap<QString, int> nameToIndex;
    QMap<int, QString> indexToName;
    QMap<QString, QSet<QString>> connectedNames;
    for (auto& item : panes_[0].first->items())
    {
        diagram_item* di = reinterpret_cast<diagram_item*>(item);

        if (!nameToIndex.contains(di->getProperties()->GetInstanceName()))
        {
            nameToIndex[di->getProperties()->GetInstanceName()] = nextIndex;
            indexToName[nextIndex] = di->getProperties()->GetInstanceName();
            nextIndex++;
        }

        auto connected = di->getConnectedNames();
        connectedNames[di->getProperties()->GetInstanceName()].unite(QSet<QString>(connected.begin(), connected.end()));
    }

    // Sort
    std::vector<std::pair<int, int>> edges;

    for (const auto& kvp : connectedNames.toStdMap())
    {
        for (const auto& se : kvp.second)
        {
            if (nameToIndex.contains(kvp.first) && nameToIndex.contains(se))
                edges.push_back({ nameToIndex[kvp.first], nameToIndex[se] });
        }
    }

    std::vector<std::pair<int, int>> coordinates;
    if (!rearrangeGraph(nameToIndex.size(), edges, coordinates))
    {
        return;
    }

    auto vr = panes_[0].second->mapToScene(panes_[0].second->viewport()->geometry()).boundingRect();
    for (auto& item : panes_[0].first->items())
    {
        diagram_item* di = reinterpret_cast<diagram_item*>(item);

        int i = nameToIndex[di->getProperties()->GetInstanceName()];

        QPoint position(vr.left() + 60 + coordinates[i].first * 60,
            vr.top() + 60 + coordinates[i].second * 60);

        int gridSize = 20;
        qreal xV = round(position.x() / gridSize) * gridSize;
        qreal yV = round(position.y() / gridSize) * gridSize;
        position = QPoint(xV, yV);

        di->setPos(position);
    }

    panes_[0].first->invalidate();
}

void MainWindow::on_AddFile_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, "Add host", QString::fromLocal8Bit("Имя хоста:"), QLineEdit::Normal, "", &ok);
    if (!ok || text.isEmpty())
        return;

    auto fi = new files_item();
    fi->SetName(text);
    if (defaultColorIndex_ < defaultColors_.size())
        fi->SetColor(defaultColors_[defaultColorIndex_++]);
    else
        fi->SetColor(QColor("White"));
    fi->ApplyToBrowser(filesPropertyEditor_);
    files_items_.push_back(fi);
    comboBoxFiles_->addItem(text);
    comboBoxFiles_->setCurrentIndex(comboBoxFiles_->count() - 1);

    QStringList fileNames;
    for (auto& file : files_items_)
        fileNames.push_back(file->GetName());

    for (int i = 0; i < panes_.count(); ++i)
    {
        for (auto& item : panes_[i].first->items())
            reinterpret_cast<diagram_item*>(item)->getProperties()->SetFileNames(fileNames);
    }

    int i = tabWidget_->indexOf(tabWidget_->currentWidget());
    if (i == -1)
        return;

    if (panes_[i].first->selectedItems().size() > 0)
        reinterpret_cast<diagram_item*>(panes_[0].first->selectedItems()[0])->getProperties()->ApplyToBrowser(propertyEditor_);
}

void MainWindow::on_RemoveFile_clicked()
{
    //if (panes_[0].first->items().size() > 1)
    //{
    //    Arrow* a = new Arrow(reinterpret_cast<diagram_item*>(panes_[0].first->items()[0]), reinterpret_cast<diagram_item*>(panes_[0].first->items()[1]));
    //    panes_[0].first->addItem(a);

    //}
    

    QMessageBox::StandardButton resBtn = QMessageBox::question(this, "parameters_composer",
        QString::fromLocal8Bit("Вы действительно хотите выйти?\nВсе несохраненные изменения будут потеряны!"), QMessageBox::No | QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes)
        QApplication::quit();
}

void MainWindow::on_Files_currentIndexChanged(int index)
{
    QString name = comboBoxFiles_->currentText();
    for (const auto& fi : files_items_)
    {
        if (fi->GetName() == name)
            fi->ApplyToBrowser(filesPropertyEditor_);
    }
}

void MainWindow::on_Units_currentIndexChanged(int index)
{
    int i = tabWidget_->indexOf(tabWidget_->currentWidget());
    if (i == -1)
        return;

    panes_[i].first->blockSignals(true);
    if (panes_[i].first->selectedItems().size() > 0)
        panes_[i].first->clearSelection();
    propertyEditor_->clear();
    panes_[i].first->blockSignals(false);
    if (index == 0)
        return;

    QString name = comboBoxUnits_->currentText();
    for (const auto& item : panes_[i].first->items())
    {
        diagram_item* di = reinterpret_cast<diagram_item*>(item);
        if (di->getName() == name)
            item->setSelected(true);
    }
}

void MainWindow::currentItemChanged(QtBrowserItem* item)
{
    if (item != nullptr)
        qDebug() << item->property()->propertyName();

    int i = tabWidget_->indexOf(tabWidget_->currentWidget());
    if (i == -1)
        return;

    if (panes_[i].first->selectedItems().size() > 0)
    {
        auto di = reinterpret_cast<diagram_item*>(panes_[i].first->selectedItems()[0]);
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
//    if (panes_[0].first->selectedItems().count() > 0 && !panes_[0].first->isItemMoving())
//    {
//        //diagram_item* gi = qobject_cast<diagram_item*>(sp_scene_->selectedItems()[0]);
//        diagram_item* gi = (diagram_item*)(panes_[0].first->selectedItems()[0]);
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
