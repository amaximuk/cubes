//#include <vector>
//
#include <QHBoxLayout>
#include <QVBoxLayout>
//#include <QGraphicsScene>
//#include <QGraphicsView>
#include <QTableView>
#include <QTreeView>
//#include <QToolBox>
#include <QToolButton>
#include <QSplitter>
//#include <QDialog>
//#include <QDebug>
//#include <QPushButton>
//#include <QFile>
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QMenuBar>
#include <QApplication>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QFileDialog>
#include <QHeaderView>
#include "parameters/yaml_parser.h"
#include "diagram/diagram_view.h"
#include "diagram/diagram_scene.h"
#include "diagram/diagram_item.h"
#include "file_item/file_item.h"
#include "file_item/file_items_manager.h"
#include "log_table/log_table_model.h"
#include "log_table/sort_filter_model.h"
#include "parameters_compiler/base64.h"
#include "properties_item/properties_item.h"
#include "properties_item/properties_items_manager.h"
#include "qttreepropertybrowser.h" // потом убрать
#include "tree_item_model.h"
#include "xml/xml_parser.h"
#include "xml/xml_writer.h"
#include "graph.h"
#include "zip.h"
#include "main_window.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    modified_ = false;
    uniqueNumber_ = 0;

    setWindowIcon(QIcon(":/images/cubes.png"));

    fileItemsManager_ = new CubesFile::FileItemsManager(this);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::FileNameChanged, this, &MainWindow::FileNameChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::FilesListChanged, this, &MainWindow::FileListChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::IncludeNameChanged, this, &MainWindow::FileIncludeNameChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::IncludesListChanged, this, &MainWindow::FileIncludesListChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::VariableNameChanged, this, &MainWindow::FileVariableNameChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::VariablesListChanged, this, &MainWindow::FileVariablesListChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::ColorChanged, this, &MainWindow::FileColorChanged);
    
    propertiesItemsManager_ = new CubesProperties::PropertiesItemsManager(this, false);
    connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::BasePropertiesChanged, this, &MainWindow::PropertiesBasePropertiesChanged);
    connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::SelectedItemChanged, this, &MainWindow::PropertiesSelectedItemChanged);
    connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::PositionChanged, this, &MainWindow::PropertiesPositionChanged);
    connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::OnError, this, &MainWindow::PropertiesOnError);
    connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::OnConnectionChanged, this, &MainWindow::PropertiesConnectionChanged);
    //connect(properties_items_manager_, &Properties::properties_items_manager::FileNameChanged, this, &MainWindow::propertiesFileNameChanged);
    //connect(properties_items_manager_, &Properties::properties_items_manager::FilesListChanged, this, &MainWindow::fileListChanged);
    //connect(properties_items_manager_, &Properties::properties_items_manager::IncludeNameChanged, this, &MainWindow::fileIncludeNameChanged);
    //connect(properties_items_manager_, &Properties::properties_items_manager::IncludesListChanged, this, &MainWindow::fileIncludesListChanged);

    CreateUi();

    uint32_t fileId{ 0 };
    fileItemsManager_->Create(QString::fromLocal8Bit("config.xml"), QString::fromLocal8Bit("АРМ"), QString::fromStdString(CubesUnitTypes::platform_names_[0]), fileId);
}

MainWindow::~MainWindow()
{
}

// ITopManager
void MainWindow::GetUnitsInFileList(const CubesUnitTypes::FileId& fileId, QStringList& unitNames)
{
    // Соберем имена юнитов в файле
    for (const auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
        if (pi->GetFileId() == fileId)
        {
            QString name = pi->GetInstanceName();
            unitNames.push_back(name);
        }
    }
}

void MainWindow::GetUnitsInFileIncludeList(const CubesUnitTypes::FileId& fileId,
    const CubesUnitTypes::IncludeId includeId, QStringList& unitNames)
{
    // Соберем имена юнитов в файле
    for (const auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
        if (pi->GetFileId() == fileId &&
            pi->GetIncludeId() == includeId)
        {
            QString name = pi->GetInstanceName();
            unitNames.push_back(name);
        }
    }
}

void MainWindow::GetUnitParameters(const QString& unitId, CubesUnitTypes::UnitParameters& unitParameters)
{
    unitParameters = unitParameters_[unitId];
}

void MainWindow::GetFileIncludeList(const CubesUnitTypes::FileId& fileId, CubesUnitTypes::IncludeIdNames& includeNames)
{
    includeNames = fileItemsManager_->GetFileIncludeNames(fileId, true);
}

void MainWindow::GetFileIncludeVariableList(const CubesUnitTypes::FileId& fileId,
    const CubesUnitTypes::IncludeId includeId, CubesUnitTypes::VariableIdVariables& variables)
{
    variables = fileItemsManager_->GetFileIncludeVariables(fileId, includeId);
}

bool MainWindow::CreatePropetiesItem(const QString& unitId, uint32_t& propertiesId)
{
    //instanceName = name + QString("_#%1").arg(unique_number_++);
    //uint32_t propertiesId{ 0 };
    propertiesItemsManager_->Create(unitId, propertiesId);
    auto pi = propertiesItemsManager_->GetItem(propertiesId);
    pi->SetFileNames(GetFileNames());
    pi->SetFileId(fileItemsManager_->GetCurrentFileId(), fileItemsManager_->GetCurrentFileName());
    pi->SetIncludeNames(GetCurrentFileIncludeNames());
    pi->SetIncludeId(CubesUnitTypes::InvalidIncludeId, "<not selected>");
    //pi->SetName(GetNewUnitName(pi->GetName()));
    //properties_items_manager_->Select(propertiesId);
    return true;
}

bool MainWindow::GetPropetiesForDrawing(const uint32_t propertiesId, PropertiesForDrawing& pfd)
{
    //auto pi = propertiesItemsManager_->GetItem(propertiesId);
    //pfd.pixmap = pi->GetPixmap();
    //pfd.name = pi->GetName();
    //pfd.fileName = pi->GetFileName();
    //pfd.groupName = pi->GetGroupName();
    //pfd.color = GetFileColor(pi->GetFileName());
    if (!propertiesItemsManager_->GetPropetiesForDrawing(propertiesId, pfd))
        return false;
    auto fileId = fileItemsManager_->GetFileId(pfd.fileName);
    pfd.color = fileItemsManager_->GetFileColor(fileId);
    return true;
}

bool MainWindow::GetPropetiesUnitParameters(const uint32_t propertiesId, CubesUnitTypes::UnitParameters& unitParameters)
{
    return propertiesItemsManager_->GetUnitParameters(propertiesId, unitParameters);
}

bool MainWindow::GetPropetiesUnitId(const uint32_t propertiesId, QString& unitId)
{
    return propertiesItemsManager_->GetUnitId(propertiesId, unitId);
}

QString MainWindow::GetNewUnitName(const QString& baseName)
{
    return {};

    //int tabIndex = -1;
    //for (int i = 0; i < panes_.count(); ++i)
    //{
    //    QString tabName = tabWidget_->tabText(i);
    //    if (groupName == tabName)
    //    {
    //        tabIndex = i;
    //        break;
    //    }
    //}

    //if (tabIndex == -1)
    //    return false;

    //QString unitName = baseName;
    //for (const auto& pi : scene_->items())
    //{
    //    diagram_item* di = reinterpret_cast<diagram_item*>(pi);
    //    if (di->getProperties()->GetName() == baseName)
    //    {
    //        unitName = di->getProperties()->GetUnitName();
    //        break;
    //    }
    //}

    //QString name = unitName;

    //QString name = baseName;
    //int sharp_index = baseName.lastIndexOf("#");
    //if (sharp_index != -1)
    //    name = baseName.left(sharp_index);
    //QString varName = name;

    //{
    //    QList<QPair<QString, QString>> variables;
    //    for (const auto& item : scene_->items())
    //    {
    //        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
    //        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
    //        if (pi->GetIncludeName() != "<not selected>")
    //        {
    //            auto fileId = fileItemsManager_->GetFileId(pi->GetName());
    //            variables = fileItemsManager_->GetFileIncludeVariables(fileId,
    //                pi->GetIncludeFileId());
    //        }
    //    }

    //    for (const auto& v : variables)
    //    {
    //        QString replace = QString("@%1@").arg(v.first);
    //        varName.replace(replace, v.second);
    //    }
    //}
    //QString newName = varName;

    //int counter = 0;
    //while (true)
    //{
    //    QList<QPair<QString, QString>> variables;
    //    for (const auto& pi : scene_->items())
    //    {
    //        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(pi);
    //        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
    //        if (pi->GetIncludeName() != "<not selected>")
    //        {
    //            auto fileId = fileItemsManager_->GetFileId(pi->GetName());
    //            variables = fileItemsManager_->GetFileIncludeVariables(fileId,
    //                pi->GetIncludeName());
    //        }
    //    }

    //    bool found = false;
    //    for (const auto& item : scene_->items())
    //    {
    //        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
    //        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
    //        QString realName = pi->GetName();
    //        for (const auto& v : variables)
    //        {
    //            QString replace = QString("@%1@").arg(v.first);
    //            realName.replace(replace, v.second);
    //        }
    //        if (realName == newName)
    //        {
    //            found = true;
    //            break;
    //        }
    //    }

    //    if (found)
    //        newName = QString("%1#%2").arg(varName).arg(++counter);
    //    else
    //        break;
    //}
    //if (counter == 0)
    //    return name;
    //else
    //    return QString("%1#%2").arg(name).arg(counter);
}

QMap<QString, QStringList> MainWindow::GetUnitsConnections()
{
    return GetConnectionsInternal(false);
}

QMap<QString, QStringList> MainWindow::GetDependsConnections()
{
    return GetConnectionsInternal(true);
}

void MainWindow::EnshureVisible(uint32_t propertiesId)
{
    for (const auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
        if (di->GetPropertiesId() == propertiesId)
        {
            QPointF center = di->GetLineAncorPosition();
            //QPointF center = scene_->selectionArea().boundingRect().center();
            view_->centerOn(center);

            scene_->invalidate();

        }
    }
}

// UI
void MainWindow::CreateUi()
{
    resize(1000, 600);

    CreateMenu();

    QWidget* mainWidget = CreateMainWidget();
    setCentralWidget(mainWidget);
}

void MainWindow::CreateMenu()
{
    QAction* newAct = new QAction(QString::fromLocal8Bit("Создать"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(QString::fromLocal8Bit("Создать новый файл"));
    connect(newAct, &QAction::triggered, this, &MainWindow::OnNewFileAction);

    QAction* openAct = new QAction(QString::fromLocal8Bit("Открыть"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(QString::fromLocal8Bit("Открыть файл"));
    connect(openAct, &QAction::triggered, this, &MainWindow::OnOpenFileAction);

    QAction* importXmlAct = new QAction(QString::fromLocal8Bit("Импорт xml"), this);
    importXmlAct->setShortcut(QKeySequence("Ctrl+I"));
    importXmlAct->setStatusTip(QString::fromLocal8Bit("Импортировать xml файл"));
    connect(importXmlAct, &QAction::triggered, this, &MainWindow::OnImportXmlFileAction);

    QAction* saveAct = new QAction(QString::fromLocal8Bit("Сохранить"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(QString::fromLocal8Bit("Сохранить файл"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::OnSaveFileAction);

    QAction* saveAsAct = new QAction(QString::fromLocal8Bit("Сохранить как..."), this);
    //saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setShortcut(QKeySequence("Ctrl+Shift+S"));
    saveAsAct->setStatusTip(QString::fromLocal8Bit("Сохранить файл как..."));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::OnSaveAsFileAction);

    QAction* quitAct = new QAction(QString::fromLocal8Bit("Выйти"), this);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(QString::fromLocal8Bit("Выйти из приложения"));
    connect(quitAct, &QAction::triggered, this, &MainWindow::OnQuitAction);

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

    QAction* sortBoostAct = new QAction(QString::fromLocal8Bit("Сортировать (boost)"), this);
    sortBoostAct->setStatusTip(QString::fromLocal8Bit("Автоматическая сортировка"));
    connect(sortBoostAct, &QAction::triggered, this, &MainWindow::OnSortBoostAction);

    QAction* sortRectAct = new QAction(QString::fromLocal8Bit("Расставить по сетке"), this);
    sortRectAct->setStatusTip(QString::fromLocal8Bit("Автоматическая сортировка"));
    connect(sortRectAct, &QAction::triggered, this, &MainWindow::OnSortRectAction);

    QMenu* editMenu = menuBar()->addMenu(QString::fromLocal8Bit("Правка"));
    editMenu->addAction(sortBoostAct);
    editMenu->addAction(sortRectAct);
}

QWidget* MainWindow::CreateMainWidget()
{
    //CreateFilesPropertyBrowser();
    //CreateGroupsPropertyBrowser();
    //CreatePropertyBrowser();
    CreateTreeView();
    FillParametersInfo();
    FillTreeView();

    CreateScene();
    CreateView();

    //tabWidget_ = new QTabWidget;
    //QWidget* widgetMainTab= CreateTabWidget(0);
    //tabWidget_->addTab(widgetMainTab, "Main");

    //qDebug() << connect(tabWidget_, &QTabWidget::currentChanged, this, &MainWindow::on_Tab_currentChanged);

    QSplitter* splitterTreeTab = new QSplitter(Qt::Horizontal);
    splitterTreeTab->addWidget(tree_);
    splitterTreeTab->addWidget(view_);
    splitterTreeTab->setStretchFactor(0, 0);
    splitterTreeTab->setStretchFactor(1, 1);

    QWidget* logWidget = CreateLogWidget();
    QSplitter* splitterTreeTabLog = new QSplitter(Qt::Vertical);
    splitterTreeTabLog->addWidget(splitterTreeTab);
    splitterTreeTabLog->addWidget(logWidget);
    splitterTreeTabLog->setStretchFactor(0, 1);
    splitterTreeTabLog->setStretchFactor(1, 0);

    QWidget* propertiesPanelWidget = CreatePropertiesPanelWidget();
    propertiesPanelWidget->setMinimumWidth(400);
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

QWidget* MainWindow::CreateLogWidget()
{
    table_view_log_ = new QTableView;

    log_table_model_ = new CubeLog::LogTableModel;
    sort_filter_model_ = new CubeLog::SortFilterModel;
    sort_filter_model_->setSourceModel(log_table_model_);
    sort_filter_model_->SetFilter({ CubeLog::MessageType::error, CubeLog::MessageType::warning, CubeLog::MessageType::information });

    table_view_log_->setModel(sort_filter_model_);
    table_view_log_->setSortingEnabled(true);
    table_view_log_->verticalHeader()->hide();
    table_view_log_->horizontalHeader()->setHighlightSections(false);
    //table_view_log_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_view_log_->horizontalHeader()->setStretchLastSection(true);
    table_view_log_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_view_log_->setSelectionMode(QAbstractItemView::SingleSelection);
    //table_view_log_->sortByColumn(0, Qt::AscendingOrder);
    table_view_log_->resizeColumnsToContents();

    QToolButton* buttonError = new QToolButton;
    buttonError->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    buttonError->setCheckable(true);
    buttonError->setIcon(QIcon(":/images/error.png"));
    buttonError->setText(QString::fromLocal8Bit("Ошибка"));
    buttonError->setChecked(true);
    qDebug() << connect(buttonError, &QToolButton::clicked, this, &MainWindow::OnErrorButtonClicked);
    QToolButton* buttonWarning = new QToolButton;
    buttonWarning->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    buttonWarning->setCheckable(true);
    buttonWarning->setIcon(QIcon(":/images/warning.png"));
    buttonWarning->setText(QString::fromLocal8Bit("Предупреждение"));
    buttonWarning->setChecked(true);
    qDebug() << connect(buttonWarning, &QToolButton::clicked, this, &MainWindow::OnWarningButtonClicked);
    QToolButton* buttonInformation = new QToolButton;
    buttonInformation->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    buttonInformation->setCheckable(true);
    buttonInformation->setIcon(QIcon(":/images/information.png"));
    buttonInformation->setText(QString::fromLocal8Bit("Информация"));
    buttonInformation->setChecked(true);
    qDebug() << connect(buttonInformation, &QToolButton::clicked, this, &MainWindow::OnInformationButtonClicked);

    QHBoxLayout* buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(buttonError);
    buttonsLayout->addWidget(buttonWarning);
    buttonsLayout->addWidget(buttonInformation);
    buttonsLayout->addStretch();
    buttonsLayout->setContentsMargins(0, 0, 0, 0);

    QWidget* buttonsWidget = new QWidget;
    buttonsWidget->setLayout(buttonsLayout);

    QWidget* mainWidget = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(buttonsWidget);
    mainLayout->addWidget(table_view_log_);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainWidget->setLayout(mainLayout);
    return mainWidget;
}

void MainWindow::CreateScene()
{
    scene_ = new CubeDiagram::DiagramScene(this);
    scene_->setSceneRect(-10000, -10000, 20032, 20032);

    qDebug() << connect(scene_, &CubeDiagram::DiagramScene::ItemPositionChanged, this, &MainWindow::DiagramItemPositionChanged);
    qDebug() << connect(scene_, &CubeDiagram::DiagramScene::AfterItemCreated, this, &MainWindow::DiagramAfterItemCreated);
    qDebug() << connect(scene_, &CubeDiagram::DiagramScene::BeforeItemDeleted, this, &MainWindow::DiagramBeforeItemDeleted);
    //qDebug() << connect(scene_, &CubeDiagram::DiagramScene::ItemNameChanged, this, &MainWindow::DiagramItemNameChanged);
    //qDebug() << connect(scene_, &CubeDiagram::DiagramScene::ItemFileChanged, this, &MainWindow::DiagramItemFileChanged);
    //qDebug() << connect(scene_, &CubeDiagram::DiagramScene::ItemGroupChanged, this, &MainWindow::DiagramItemGroupChanged);
    qDebug() << connect(scene_, &CubeDiagram::DiagramScene::selectionChanged, this, &MainWindow::selectionChanged);
}

void MainWindow::CreateView()
{
    view_ = new CubeDiagram::DiagramView(this, scene_);
    view_->setDragMode(QGraphicsView::RubberBandDrag);
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

    QWidget* filesWidget = fileItemsManager_->GetWidget();
    QWidget* propertiesWidget = propertiesItemsManager_->GetWidget();
    //QWidget* hintWidget = CreateHintWidget();

    QSplitter* tabVSplitter = new QSplitter(Qt::Vertical);
    tabVSplitter->addWidget(filesWidget);
    tabVSplitter->addWidget(propertiesWidget);
    //tabVSplitter->addWidget(hintWidget);
    tabVSplitter->setStretchFactor(0, 0);
    tabVSplitter->setStretchFactor(1, 1);
    //tabVSplitter->setStretchFactor(2, 0);

    QVBoxLayout* propertiesPaneLayout = new QVBoxLayout;
    propertiesPaneLayout->addWidget(tabVSplitter);
    propertiesPaneLayout->setContentsMargins(0, 0, 0, 0);

    propertiesPanelWidget->setLayout(propertiesPaneLayout);

    return propertiesPanelWidget;
}

//QWidget* MainWindow::CreateHintWidget()
//{
//    QWidget* hintWidget = new QWidget;
//    plainTextEditHint_ = new QPlainTextEdit;
//    plainTextEditHint_->setFixedHeight(100);
//    plainTextEditHint_->setReadOnly(true);
//    QVBoxLayout* vBoxLayoutHint = new QVBoxLayout;
//    vBoxLayoutHint->setMargin(0);
//    vBoxLayoutHint->addWidget(plainTextEditHint_);
//    vBoxLayoutHint->setContentsMargins(0, 0, 0, 0);
//    hintWidget->setLayout(vBoxLayoutHint);
//    return hintWidget;
//}

void MainWindow::FillTreeView()
{
    TreeItemModel* model = new TreeItemModel();

    QMap<QString, QSet<QString>> categoriesMap;
    for (const auto& up : unitParameters_)
    {
        QString category = "default";
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
        auto list = cat.second.values();
        std::sort(list.begin(), list.end());
        for (const auto& id : list)
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
                parameters::file_info fi{};
                if (!parameters::yaml::parser::parse(fullPath.toStdString(), false, fi))
                {
                    CubeLog::LogMessage m{};
                    m.type = CubeLog::MessageType::error;
                    m.source = filename;
                    m.description = QString::fromLocal8Bit("Файл параметров %1 не разобран. Параметры не добавлены.").arg(fullPath);
                    log_table_model_->AddMessage(m);
                }

                // Добавляем параметр - зависимости, его нет в параметрах юнитов, но он может присутствовать в xml файле
                // Принцип обработки такой же как и у остальных параметров
                //if (fi.info.id != "group" && fi.info.id != "group_mock")
                {
                    parameters::parameter_info pi{};
                    pi.type = QString::fromLocal8Bit("array<string>").toStdString();
                    pi.name = ids_.dependencies.toString().toStdString();
                    pi.display_name = QString::fromLocal8Bit("Зависимости").toStdString();
                    pi.description = QString::fromLocal8Bit("Зависимости юнита от других юнитов").toStdString();
                    pi.required = QString::fromLocal8Bit("false").toStdString();
                    pi.default_ = QString::fromLocal8Bit("не задано").toStdString();
                    fi.parameters.push_back(std::move(pi));
                }

                auto& up = unitParameters_[QString::fromStdString(fi.info.id)];
                up.fileInfo = fi;
                up.platforms.insert(QFileInfo(platformDir).baseName());
            }
        }
    }
}

// Units
bool MainWindow::AddMainFile(const CubesXml::File& file, const QString& zipFileName)
{
    // Не очищаем, вдруг там уже что-то поменяно
    //if (scene_->items().size() == 0)
    //    fileItemsManager_->Clear();

    //QString fileName = QFileInfo(file.fileName).fileName();

    //QString name = file.name;
    CubesUnitTypes::FileId fileId{ CubesUnitTypes::InvalidFileId };
    fileItemsManager_->Create(file, fileId);
    fileItemsManager_->Select(fileId);
    //QString name;
    //auto res = fileItemsManager_->GetName(fileId, name);

    //for (const auto& include : file.includes)
    //    fileItemsManager_->AddFileInclude(fileId, include.fileName, include.variables);

    //for (int i = 0; i < file.includes.size(); i++)
    //{
    //    QList<QPair<QString, QString>> variables;
    //    for (const auto& kvp : file.includes[i].variables.toStdMap())
    //        variables.push_back({ kvp.first, kvp.second });
    //}

    //QStringList fileNames = fileItemsManager_->GetFileNames();
    //for (auto& item : scene_->items())
    //{
    //    CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
    //    auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
    //    pi->SetFileNames(fileNames);
    //}

    // Convert includes into unit
    //CubesXml::Group g{};
    //g.path = "service";
    //for (int i = 0; i < file.includes.size(); i++)
    //{
    //    CubesXml::Unit u{};
    //    u.id = "group";
    //    u.name = QString::fromLocal8Bit("Группа %1").arg(i);
    //    CubesXml::Param p{};
    //    p.name = "FILE_PATH";
    //    p.type = "str";
    //    p.val = file.includes[i].fileName;
    //    u.params.push_back(std::move(p));
    //    CubesXml::Array a{};
    //    a.name = "VARIABLES";
    //    for (const auto& kvp : file.includes[i].variables.toStdMap())
    //    {
    //        CubesXml::Item i1{};
    //        CubesXml::Param p1{};
    //        p1.name = "NAME";
    //        p1.type = "str";
    //        p1.val = kvp.first;
    //        i1.params.push_back(std::move(p1));
    //        CubesXml::Param p2{};
    //        p2.name = "VALUE";
    //        p2.type = "str";
    //        p2.val = kvp.second;
    //        i1.params.push_back(std::move(p2));
    //        a.items.push_back(std::move(i1));
    //    }
    //    u.arrays.push_back(std::move(a));
    //    g.units.push_back(std::move(u));
    //}

    //if (file.includes.size() > 0)
    //    file.config.groups.push_back(std::move(g));

    if (!AddUnits(fileId, CubesUnitTypes::InvalidIncludeId, file))
        return false;

    if (zipFileName.isEmpty())
    {
        // Запрашиваем список из fileItemsManager_, чтобы получить includeId
        const auto includes = fileItemsManager_->GetFileIncludeNames(fileId, false);
        QDir dir = QFileInfo(file.fileName).absoluteDir();

        for (const auto& includeId : includes.keys())
        {
            const auto includePath = fileItemsManager_->GetFileIncludePath(fileId, includeId);

            QString includeName = dir.filePath(includePath);
            CubesXml::File includedFile{};
            if (!CubesXml::Parser::Parse(includeName, includedFile))
                return false;

            if (!AddUnits(fileId, includeId, includedFile))
                return false;
        }

        //for (int i = 0; i < file.includes.size(); i++)
        //{
        //    QString includeFileName = dir.filePath(file.includes[i].fileName);
        //    CubesXml::File includedFile{};
        //    if (!CubesXml::Parser::Parse(includeFileName, includedFile))
        //        return false;
        //    if (!AddUnits(fileId, file.includes[i].fileName, includedFile))
        //        return false;
        //}
    }
    else
    {
        // Запрашиваем список из fileItemsManager_, чтобы получить includeId
        const auto includes = fileItemsManager_->GetFileIncludeNames(fileId, false);

        for (const auto& includeId : includes.keys())
        {
            const auto includePath = fileItemsManager_->GetFileIncludePath(fileId, includeId);

            QByteArray byteArray;
            if (!CubesZip::UnZipFile(zipFileName, includePath, byteArray))
                return false;

            CubesXml::File includedFile{};
            if (!CubesXml::Parser::Parse(byteArray, includePath, includedFile))
                return false;

            if (!AddUnits(fileId, includeId, includedFile))
                return false;
        }

        //for (int i = 0; i < file.includes.size(); i++)
        //{
        //    QByteArray byteArray;
        //    if (!CubesZip::UnZipFile(zipFileName, file.includes[i].fileName, byteArray))
        //        return false;

        //    CubesXml::File includedFile{};
        //    CubesXml::Parser::Parse(byteArray, file.includes[i].fileName, includedFile);

        //    if (!AddUnits(name, file.includes[i].fileName, includedFile))
        //        return false;
        //}

    }



    if (!SortUnitsRectangular(true))
        return false;

    return true;
}

bool MainWindow::AddUnits(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId includeId, const CubesXml::File& file)
{
    QVector<CubesXml::Unit> all_units;
    for (const auto& g : file.config.groups)
    {
        for (const auto& u : g.units)
        {
            auto up = GetUnitParameters(u.id);
            if (up != nullptr)
            {
                all_units.push_back(u);
            }
            else
            {
                CubeLog::LogMessage m{};
                m.type = CubeLog::MessageType::error;
                m.source = QFileInfo(file.fileName).fileName();
                m.description = QString::fromLocal8Bit("Нет файла параметров для юнита %1 (%2). Юнит не добавлен.").arg(u.name, u.id);
                log_table_model_->AddMessage(m);
            }
        }
    }
    log_table_model_->submit();
    table_view_log_->resizeColumnsToContents();
    table_view_log_->update();

    // Get fileNames list
    CubesUnitTypes::FileIdNames fileNames = fileItemsManager_->GetFileNames();
    QString fileName = fileItemsManager_->GetFileName(fileId);

    // Transform
    CubeDiagram::DiagramItem* di = nullptr;
    for (int i = 0; i < all_units.size(); i++)
    {
        QString name = all_units[i].id;
        auto up = GetUnitParameters(name);

        if (up != nullptr)
        {
            uint32_t propertiesId{ 0 };
            propertiesItemsManager_->Create(all_units[i], propertiesId);
            auto pi = propertiesItemsManager_->GetItem(propertiesId);

            //pi->ApplyXmlProperties(all_units[i]);
            pi->SetFileNames(fileNames);
            pi->SetFileId(fileId, fileName);
            //if (includedFileName != "")
            {
                CubesUnitTypes::IncludeIdNames includeNames = fileItemsManager_->GetFileIncludeNames(fileId, true);
                // TODO: Добавить в xml название файла и убрать функцию GetFileIncludeName отовсюду за ненадобностью
                QString includeName = fileItemsManager_->GetFileIncludeName(fileId, includeId);
                if (includeName.isEmpty())
                    includeName = "<not selected>";
                pi->SetIncludeNames(includeNames);
                pi->SetIncludeId(includeId, includeName);
            }

            PropertiesForDrawing pfd{};
            if (!GetPropetiesForDrawing(propertiesId, pfd))
            {
                qDebug() << "ERROR GetPropeties: " << propertiesId;
            }

            di = new CubeDiagram::DiagramItem(propertiesId, pfd.pixmap, pfd.name, pfd.fileName, pfd.includeName, pfd.color);
            di->setX(all_units[i].x);
            di->setY(all_units[i].y);
            di->setZValue(all_units[i].z);
            scene_->addItem(di);
        }
        else
        {
            // error
        }
    }

    if (all_units.size() > 0 && di != nullptr)
    {
        scene_->clearSelection();
        propertiesItemsManager_->Select(0);
        DiagramAfterItemCreated(di);
    }
    //if (!SortUnitsRectangular())
    //    return false;

    return true;
}

bool MainWindow::SortUnits()
{
    // Prepare sort
    int nextIndex = 0;
    QMap<QString, int> nameToIndex;
    QMap<int, QString> indexToName;
    QMap<QString, QSet<QString>> connectedNames;
    for (auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);

        if (!nameToIndex.contains(di->name_))
        {
            nameToIndex[di->name_] = nextIndex;
            indexToName[nextIndex] = di->name_;
            nextIndex++;
        }

        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
        auto connected = pi->GetConnectedNames();
        connectedNames[di->name_].unite(QSet<QString>(connected.begin(), connected.end()));
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
    if (!CubesGraph::RearrangeGraph(nameToIndex.size(), edges, coordinates))
    {
        return false;
    }

    auto vr = view_->mapToScene(view_->viewport()->geometry()).boundingRect();
    for (auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);

        int i = nameToIndex[di->name_];

        //QPoint position(vr.left() + 60 + coordinates[i].first * 60,
        //    vr.top() + 60 + coordinates[i].second * 60);

        int gridSize = 20;
        //qreal xV = round(position.x() / gridSize) * gridSize;
        //qreal yV = round(position.y() / gridSize) * gridSize;

        //QPoint position(round(vr.left() / gridSize) * gridSize, round(vr.top() / gridSize) * gridSize);
        //position += QPoint(60 + coordinates[i].first * 60, 60 + coordinates[i].second * 60);




        QPoint position(80 + coordinates[i].first * 80, 80 + coordinates[i].second * 80);
        //QPoint position(60 + coordinates[i].first * 60, 60 + coordinates[i].second * 60);
        di->setPos(position);

        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
        pi->PositionChanged(di->pos());

        //di->setSelected(true);
    }
    QPointF center = scene_->itemsBoundingRect().center();
    //QPointF centerMapped = view_->mapFromScene(center);
    view_->centerOn(center);

    scene_->invalidate();
    return true;
}

bool MainWindow::SortUnitsRectangular(bool check)
{
    if (scene_->items().size() == 0)
        return true;

    bool sort = true;
    if (check)
    {
        int count = 0;
        for (auto& item : scene_->items())
        {
            CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
            QPointF p = di->pos();
            if (qFuzzyIsNull(p.x()) && qFuzzyIsNull(p.y()))
                ++count;
        }

        // Все нулевые, распределяем по сетке
        if (count != scene_->items().size())
            sort = false;
    }


    if (sort)
    {
        int size = scene_->items().size();
        int rows = std::sqrt(scene_->items().size());
        int columns = (scene_->items().size() + rows - 1) / rows;

        int c = 0;
        int r = 0;
        for (auto& item : scene_->items())
        {
            CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
            QPoint position(c * 200, r * 80);
            di->setPos(position);

            auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
            pi->PositionChanged(di->pos());

            if (++c == columns) { ++r; c = 0; };
        }
        QPointF center = scene_->itemsBoundingRect().center();
        view_->centerOn(center);

        scene_->invalidate();
    }

    return true;
}

QMap<QString, QStringList> MainWindow::GetConnectionsInternal(bool depends)
{
    // Сюда будем собирать реальные соединения на этой сцене
    QMap<QString, QStringList> result;

    // Соберем имена юнитов на главной панели
    QStringList mainUnits;
    for (const auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
        //auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
        QString name;
        if (propertiesItemsManager_->GetName(di->propertiesId_, name))
            mainUnits.push_back(name);
    }

    // Для юнитов сцены собираем список зависимостей, а для групп еще список юнитов
    QMap<QString, QStringList> connections;
    for (const auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
        QString name;
        if (propertiesItemsManager_->GetName(di->propertiesId_, name))
        {
            QStringList conn = depends ? pi->GetDependentNames() : pi->GetConnectedNames();
            if (conn.size() > 0)
                connections[name].append(conn);
        }
    }

    // Перебираем юниты сцены и для них рисуем соединения
    for (const auto& kvp : connections.toStdMap())
    {
        // Проверяем этот юнит
        QString unitName = kvp.first;

        // Перебираем все соединения
        for (const auto& name : kvp.second)
        {
            // Проверяем, что соединение с юнитом, который на этой сцене
            if (mainUnits.contains(name))
            {
                // Отсеиваем дубликаты
                if ((result.contains(unitName) && result[unitName].contains(name)) ||
                    (result.contains(name) && result[name].contains(unitName)))
                {
                    // Уже есть
                }
                else
                {
                    // Добавляем
                    result[unitName].push_back(name);
                }
            }
        }
    }

    return result;
}

CubesUnitTypes::UnitParameters* MainWindow::GetUnitParameters(const QString& id)
{
    for (auto& up : unitParameters_)
    {
        if (QString::fromStdString(up.fileInfo.info.id) == id)
            return &up;
    }
    return nullptr;
}

// Files
CubesUnitTypes::FileIdNames MainWindow::GetFileNames()
{
    return fileItemsManager_->GetFileNames();
}

//QString MainWindow::GetCurrentFileName()
//{
//    return fileItemsManager_->GetCurrentFileName();
//}

CubesUnitTypes::IncludeIdNames MainWindow::GetCurrentFileIncludeNames()
{
    auto fileId = fileItemsManager_->GetFileId(fileItemsManager_->GetCurrentFileName());
    return fileItemsManager_->GetFileIncludeNames(fileId, true);
}

//QColor MainWindow::GetFileColor(const QString& fileName)
//{
//    return fileItemsManager_->GetFileColor(fileName);
//}

QString MainWindow::GetDisplayName(const QString& baseName)
{
    CubesUnitTypes::VariableIdVariables variables;
    for (const auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
        if (pi->GetIncludeName() != "<not selected>")
        {
            auto fileId = fileItemsManager_->GetFileId(pi->GetName());
            variables = fileItemsManager_->GetFileIncludeVariables(fileId,
                pi->GetIncludeId());
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

// DiagramScene (as manager)
void MainWindow::DiagramItemPositionChanged(CubeDiagram::DiagramItem* di)
{
    auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
    pi->PositionChanged(di->pos());
}

void MainWindow::DiagramAfterItemCreated(CubeDiagram::DiagramItem* di)
{
    propertiesItemsManager_->Select(di->propertiesId_);
}

void MainWindow::DiagramBeforeItemDeleted(CubeDiagram::DiagramItem* di)
{
    propertiesItemsManager_->Remove(di->propertiesId_);
    //for (int i = 1; i < propertiesItemsManager_->GetSelector()->count(); i++)
    //{
    //    auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
    //    if (propertiesItemsManager_->GetSelector()->itemText(i) == pi->GetName())
    //    {
    //        propertiesItemsManager_->GetSelector()->removeItem(i);
    //        break;
    //    }
    //}
}

//void MainWindow::DiagramItemNameChanged(CubeDiagram::DiagramItem* di, QString oldName)
//{
//    int i = propertiesItemsManager_->GetSelector()->findText(oldName);
//    auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
//    if (i != -1)
//        propertiesItemsManager_->GetSelector()->setItemText(i, pi->GetName());
//}
//
//void MainWindow::DiagramItemFileChanged(CubeDiagram::DiagramItem* di)
//{
//    auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
//    QString fileName = pi->GetFileName();
//    QStringList includeNames = fileItemsManager_->GetFileIncludeNames(fileName);
//    pi->SetGroupNames(includeNames);
//    pi->SetGroupName("<not selected>");
//}
//
//void MainWindow::DiagramItemGroupChanged(CubeDiagram::DiagramItem* item)
//{
//}

void MainWindow::selectionChanged()
{
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if (scene_->selectedItems().count() > 0)
    {
        CubeDiagram::DiagramItem* di = (CubeDiagram::DiagramItem*)(scene_->selectedItems()[0]);
        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
        //pi->PositionChanged(di->pos());
        //pi->ZOrderChanged(di->zValue());
        //if (scene_->selectedItems().count() == 1)
        if (propertiesItemsManager_->GetCurrentPropertiesId() == 0)
            propertiesItemsManager_->Select(di->propertiesId_);
    }
    else
    {
        //plainTextEditHint_->setPlainText("");
        propertiesItemsManager_->Select(0);
    }
}

// FileItemsManager
void MainWindow::FileNameChanged(const CubesUnitTypes::FileId& fileId)
{
    CubesUnitTypes::FileIdNames fileNames = fileItemsManager_->GetFileNames();
    CubesUnitTypes::IncludeIdNames fileIncludeNames = fileItemsManager_->GetFileIncludeNames(fileId, true);
    auto fileName = fileItemsManager_->GetFileName(fileId);
    for (auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);


        if (pi->GetFileId() == fileId)
            pi->SetFileId(fileId, fileName);

        //QString currentName = pi->GetFileName();
        //QString currentIncludeName = pi->GetIncludeName();
        //pi->SetFileNames(fileNames);
        //if (currentName == oldFileName)
        //{
        //    pi->SetFileName(fileName);
        //    pi->SetIncludeName(currentIncludeName);
        //}
    }

    scene_->invalidate();
}

void MainWindow::FileListChanged(const CubesUnitTypes::FileIdNames& fileNames)
{
    for (auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
        pi->SetFileNames(fileNames);
    }

    scene_->invalidate();
}

void MainWindow::FileIncludeNameChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId& includeId)
{
    CubesUnitTypes::IncludeIdNames fileIncludeNames = fileItemsManager_->GetFileIncludeNames(fileId, true);
    auto includeName = fileItemsManager_->GetFileIncludeName(fileId, includeId);
    for (auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
        if (pi->GetFileId() == fileId && pi->GetIncludeId() == includeId)
            pi->SetIncludeId(includeId, includeName);

        //const auto includeFileId = pi->GetIncludeFileId();
        //pi->SetIncludeNames(fileIncludeNames);
        //if (includeFileId == oldIncludeId)
        //{
        //    const auto includeName = fileItemsManager_->GetFileIncludeName();
        //    pi->SetIncludeName(includeName);
        //}
    }

    scene_->invalidate();
}

void MainWindow::FileIncludesListChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeIdNames& includeNames)
{
    for (auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
        if (pi->GetFileId() == fileId)
            pi->SetIncludeNames(includeNames);
    }

    scene_->invalidate();
}

void MainWindow::FileVariableNameChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId& includeId,
    const QString& variableName, const QString& oldVariableName)
{
    propertiesItemsManager_->InformVariableChanged();
}

void MainWindow::FileVariablesListChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId& includeId,
    const CubesUnitTypes::VariableIdVariables& variables)
{
    propertiesItemsManager_->InformVariableChanged();
}

void MainWindow::FileColorChanged(const CubesUnitTypes::FileId& fileId, const QColor& color)
{
    for (auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);

        auto pi = propertiesItemsManager_->GetItem(di->GetPropertiesId());
        if (pi->GetFileId() == fileId)
            di->color_ = color;
    }

    scene_->invalidate();
}

//void MainWindow::FileVariableChanged(const QString& fileName, const QString& includeName, const QList<QPair<QString, QString>>& variables)
//{
//    propertiesItemsManager_->InformVariableChanged();
//}

// PropertiesItemsManager
void MainWindow::PropertiesBasePropertiesChanged(const uint32_t propertiesId, const QString& name, const QString& fileName, const QString& groupName)
{
    for (auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
        if (di->propertiesId_ == propertiesId)
        {
            di->name_ = name;
            di->fileName_ = fileName;
            di->includeName_ = groupName;
            auto fileId = fileItemsManager_->GetFileId(fileName);
            di->color_ = fileItemsManager_->GetFileColor(fileId);
            di->InformNameChanged(name, "");
            di->InformIncludeChanged();
        }
    }

    scene_->invalidate();
}

void MainWindow::PropertiesSelectedItemChanged(const uint32_t propertiesId)
{
    QGraphicsItem* item_to_select = nullptr;
    for (auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
        if (di->propertiesId_ == propertiesId)
        {
            item_to_select = item;
            break;
        }
    }
//!!!!!!!!!!!!!!!!!!!
    for (auto& item : scene_->selectedItems())
    {
        if (item != item_to_select)
            item->setSelected(false);
    }

    if (item_to_select != nullptr)
        item_to_select->setSelected(true);
}

void MainWindow::PropertiesPositionChanged(const uint32_t propertiesId, double posX, double posY, double posZ)
{
    for (auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
        if (di->propertiesId_ == propertiesId)
        {
            di->setPos(QPointF(posX, posY));
            di->setZValue(posZ);
            break;
        }
    }
}

void MainWindow::PropertiesOnError(const uint32_t propertiesId, const QString& message)
{
    CubeLog::LogMessage m{};
    m.type = CubeLog::MessageType::error;
    m.source = QString("%1").arg(propertiesId);
    m.description = message;
    log_table_model_->AddMessage(m);

    log_table_model_->submit();
    table_view_log_->resizeColumnsToContents();
    table_view_log_->update();
}

void MainWindow::PropertiesConnectionChanged(const uint32_t propertiesId)
{
    scene_->invalidate();
}

// Кнопки
void MainWindow::OnNewFileAction()
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

void MainWindow::OnOpenFileAction()
{
    if (modified_)
    {
        QMessageBox::StandardButton resBtn = QMessageBox::question(this, "parameters_composer",
            QString::fromLocal8Bit("Вы действительно хотите открыть файл?\nВсе несохраненные изменения будут потеряны!"), QMessageBox::No | QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes)
            return;
    }



    scene_->clear();
    propertiesItemsManager_->Clear();
    fileItemsManager_->Clear();




    QFileDialog dialog(this);
    dialog.setNameFilter("Parameters Archive Files (*.xlmx)");
    dialog.setAcceptMode(QFileDialog::AcceptOpen);

    QStringList selectedFileNames;
    if (dialog.exec())
        selectedFileNames = dialog.selectedFiles();

    if (selectedFileNames.size() == 0)
        return;

    QList<QString> fileNames;
    if (!CubesZip::GetZippedFileNames(selectedFileNames[0], fileNames))
        return;

    for (const auto& fileName : fileNames)
    {
        QByteArray byteArray;
        if (!CubesZip::UnZipFile(selectedFileNames[0], fileName, byteArray))
            return;

        CubesXml::File f{};
        CubesXml::Parser::Parse(byteArray, fileName, f);

        if (f.config.networkingIsSet)
        {
            log_table_model_->Clear();

            if (!AddMainFile(f, selectedFileNames[0]))
                return;
        }
        else
        {
            continue;
        }
    }

    for (auto& item : scene_->items())
    {
        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);

        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
        const auto position = pi->GetPosition();
        di->setPos(position);
    }

    //auto _OutputFolder = QFileDialog::getExistingDirectory(0, ("Select Output Folder"), QDir::currentPath());

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

void MainWindow::OnImportXmlFileAction()
{
    //if (modified_)
    //{
    //    QMessageBox::StandardButton resBtn = QMessageBox::question(this, "parameters_composer",
    //        QString::fromLocal8Bit("Вы действительно хотите открыть файл?\nВсе несохраненные изменения будут потеряны!"), QMessageBox::No | QMessageBox::Yes);
    //    if (resBtn != QMessageBox::Yes)
    //        return;
    //}

    CubesXml::File f{};
    {
        QFileDialog dialog(this);
        dialog.setNameFilter("Settings XML Files (*.xml)");
        dialog.setAcceptMode(QFileDialog::AcceptOpen);

        QStringList fileNames;
        if (dialog.exec())
            fileNames = dialog.selectedFiles();

        if (fileNames.size() == 0)
            return;

        CubesXml::Parser::Parse(fileNames[0], f);
    }

    if (f.config.networkingIsSet)
    {
        log_table_model_->Clear();

        if (!AddMainFile(f, ""))
            return;
    }
    else
    {
        QMessageBox::critical(this, "Error", QString::fromLocal8Bit("Это подключаемый файл, нельзя его импортировать."));
    }
}

void MainWindow::OnSaveFileAction()
{
    //if (!modified_)
    //    return;


    QFileDialog dialog(this);
    dialog.setNameFilters({ "Parameters Archive Files (*.xlmx)" });
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("xmlx");

    QStringList selectedFileNames;
    if (dialog.exec())
        selectedFileNames = dialog.selectedFiles();
    if (selectedFileNames.size() <= 0)
        return;

    //bool is_json = (dialog.selectedNameFilter() == "Parameters Compiler JSON Files (*.json)");
    QString selectedFileName = selectedFileNames[0];
    //SaveAsInternal(fileName, is_json, false);



    //QDir dir;
    //dir.mkdir("tmp");

    bool is_first = true;

    // Получаем список главных файлов
    CubesUnitTypes::FileIdNames fileNames = fileItemsManager_->GetFileNames();
    for (const auto& kvpFile : fileNames.toStdMap())
    {
        //// Соберем всю информацию о файле
        //auto file = fileItemsManager_->GetFile(fileName);
        //auto groups = propertiesItemsManager_->GetXmlGroups(fileName);

        //// Соберем всю информацию о файле
        //CubesXml::File xmlFile{};
        //xmlFile.fileName = fileName;
        //xmlFile.config.networkingIsSet = true;
        //xmlFile.config.logIsSet = true;

        //xmlFile.config.networking = file.network;

        auto xmlFile = fileItemsManager_->GetXmlFile(kvpFile.first);
        QFileInfo xmlFileInfo(xmlFile.fileName);
        const auto xmlFileName = xmlFileInfo.fileName();
        const auto xmlZipFilePath = selectedFileName; // QString("tmp/%1.xmlx").arg(xmlFileInfo.completeBaseName());

        {
            auto xmlGroups = propertiesItemsManager_->GetXmlGroups(kvpFile.first);
            xmlFile.config.groups = std::move(xmlGroups);

            QByteArray byteArray;
            if (!CubesXml::Writer::Write(byteArray, xmlFile))
                return;

            if (is_first)
            {
                if (!CubesZip::ZipFile(byteArray, xmlFileName, xmlZipFilePath, CubesZip::ZipMethod::Create))
                    return;
                is_first = false;
            }
            else
            {
                if (!CubesZip::ZipFile(byteArray, xmlFileName, xmlZipFilePath, CubesZip::ZipMethod::Append))
                    return;
            }
            //CubesXml::Writer::Write(xmlFileName, xmlFile);
            //CubesZip::ZipFile(xmlFileName, xmlZipFileName, CubesZip::ZipMethod::Create);
        }

        auto includes = fileItemsManager_->GetFileIncludeNames(kvpFile.first, false);
        for (const auto& kvpInclude : includes.toStdMap())
        {
            auto includeGroups = propertiesItemsManager_->GetXmlGroups(kvpFile.first, kvpInclude.first);
            CubesXml::File includeXmlFile{};
            
            const auto item = fileItemsManager_->GetItem(kvpFile.first);
            const auto includeName = item->GetIncludeName(kvpInclude.first);
            const auto includePath = item->GetIncludePath(kvpInclude.first);

            includeXmlFile.name = includeName;
            //includeXmlFile.platform = xmlFile.platform;
            includeXmlFile.fileName = includePath;

            includeXmlFile.config.logIsSet = false;
            includeXmlFile.config.networkingIsSet = false;
            includeXmlFile.config.groups = includeGroups;


            QFileInfo includeXmlFileInfo(includeXmlFile.fileName);
            const auto includeXmlFileName = includeXmlFileInfo.fileName();


            QByteArray byteArray;
            if (!CubesXml::Writer::Write(byteArray, includeXmlFile))
                return;

            if (!CubesZip::ZipFile(byteArray, includeXmlFileName, xmlZipFilePath, CubesZip::ZipMethod::Append))
                return;

            //CubesXml::Writer::Write(includeXmlFileName, includeXmlFile);
            //CubesZip::ZipFile(includeXmlFileName, xmlZipFilePath, CubesZip::ZipMethod::Append);
        }
    }

        /*
        // Получаем список включаемых файлов
        QStringList fileIncludeNames = fileItemsManager_->GetFileIncludeNames(fileName, false);
        if (fileIncludeNames.size() > 0)
        {
            // Соберем всю информацию о включаемом файле
            CubesXml::Include xmlInclude{};

            for (const auto& fileIncludeName : fileIncludeNames)
            {
                QList<uint32_t> propertiesIds = propertiesItemsManager_->GetPropertyIdsByFileName(fileName, fileIncludeName);
                for (const auto& propertiesId : propertiesIds)
                {
                    qDebug() << propertiesId << " ";


                    auto item = propertiesItemsManager_->GetItem(propertiesId);

                    CubesXml::Unit xmlUnit{};
                    item->GetXml(xmlUnit);
                }
            }

            // Добавляем файл в массив
            xmlFile.includes.push_back(xmlInclude);
        }
        else
        {
            // Соберем всю информацию о конфигурации
            xmlFile.config.networking;
            xmlFile.config.log;
            xmlFile.config.groups;

            QMap<QString, CubesXml::Group> xmlGroups;

            QList<uint32_t> propertiesIds = propertiesItemsManager_->GetPropertyIdsByFileName(fileName);
            for (const auto& propertiesId : propertiesIds)
            {
                qDebug() << propertiesId << " ";

                auto item = propertiesItemsManager_->GetItem(propertiesId);

                CubesXml::Unit xmlUnit{};
                item->GetXml(xmlUnit);
                const auto group = item->GetUnitCategory().toLower();
                
                if (!xmlGroups.contains(group))
                {
                    CubesXml::Group xmlGroup{};
                    xmlGroup.path = group;
                    xmlGroups[group] = xmlGroup;
                }
                xmlGroups[group].units.push_back(xmlUnit);
            }

            for (const auto& xmlGroup : xmlGroups)
                xmlFile.config.groups.push_back(xmlGroup);
            //xmlFile.config.groups = xmlGroups.values();
            //std::sort(xmlFile.config.groups.begin(), xmlFile.config.groups.end());
        }
    */
    // Получаем спи


    //struct Group
    //{
    //    QString path;
    //    QList<Unit> units;
    //};

    //struct Config
    //{
    //    Networking networking;
    //    bool networking_is_set;
    //    Log log;
    //    bool log_is_set;
    //    QList<Group> groups;
    //};

    //struct File
    //{
    //    QString fileName;
    //    QList<Include> includes;
    //    Config config;
    //};



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

void MainWindow::OnSaveAsFileAction()
{
    //SaveAs();
}

void MainWindow::OnQuitAction()
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

void MainWindow::OnSortBoostAction()
{
    SortUnits();
}

void MainWindow::OnSortRectAction()
{
    SortUnitsRectangular(false);
}

// TODO: Перенести подсказку в менеджер
//void MainWindow::currentItemChanged(QtBrowserItem* item)
//{
//    if (item != nullptr)
//        qDebug() << item->property()->propertyName();
//
//    if (scene_->selectedItems().size() > 0)
//    {
//        auto di = reinterpret_cast<CubeDiagram::DiagramItem*>(scene_->selectedItems()[0]);
//        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
//        if (item != nullptr)
//            plainTextEditHint_->setPlainText(pi->GetPropertyDescription(item->property()));
//        else
//            plainTextEditHint_->setPlainText("");
//    }
//}

// Лог
void MainWindow::OnErrorButtonClicked(bool checked)
{
    if (checked)
        sort_filter_model_->AddToFilter(CubeLog::MessageType::error);
    else
        sort_filter_model_->RemoveFromFilter(CubeLog::MessageType::error);
}

void MainWindow::OnWarningButtonClicked(bool checked)
{
    if (checked)
        sort_filter_model_->AddToFilter(CubeLog::MessageType::warning);
    else
        sort_filter_model_->RemoveFromFilter(CubeLog::MessageType::warning);
}

void MainWindow::OnInformationButtonClicked(bool checked)
{
    if (checked)
        sort_filter_model_->AddToFilter(CubeLog::MessageType::information);
    else
        sort_filter_model_->RemoveFromFilter(CubeLog::MessageType::information);
}
