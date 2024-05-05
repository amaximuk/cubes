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
#include "analysis/analysis_manager.h"
#include "qttreepropertybrowser.h" // потом убрать
#include "tree_item_model.h"
#include "xml/xml_parser.h"
#include "xml/xml_writer.h"
#include "xml/xml_helper.h"
#include "graph.h"
#include "zip.h"
#include "main_window.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    modified_ = false;
    uniqueNumber_ = 0;

    setWindowIcon(QIcon(":/images/cubes.png"));
    UpdateFileState("", false);

    fileItemsManager_ = new CubesFile::FileItemsManager(this, this);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::FileNameChanged, this, &MainWindow::FileNameChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::FilesListChanged, this, &MainWindow::FileListChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::IncludeNameChanged, this, &MainWindow::FileIncludeNameChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::IncludesListChanged, this, &MainWindow::FileIncludesListChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::VariableNameChanged, this, &MainWindow::FileVariableNameChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::VariablesListChanged, this, &MainWindow::FileVariablesListChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::ColorChanged, this, &MainWindow::FileColorChanged);
    connect(fileItemsManager_, &CubesFile::FileItemsManager::PropertiesChanged, this, &MainWindow::FilePropertiesChanged);
    
    propertiesItemsManager_ = new CubesProperties::PropertiesItemsManager(this, this, false);
    connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::BasePropertiesChanged, this, &MainWindow::PropertiesBasePropertiesChanged);
    connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::SelectedItemChanged, this, &MainWindow::PropertiesSelectedItemChanged);
    connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::PositionChanged, this, &MainWindow::PropertiesPositionChanged);
    connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::Error, this, &MainWindow::PropertiesError);
    connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::ConnectionChanged, this, &MainWindow::PropertiesConnectionChanged);
    connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::PropertiesChanged, this, &MainWindow::PropertiesPropertiesChanged);
    //connect(properties_items_manager_, &Properties::properties_items_manager::FileNameChanged, this, &MainWindow::propertiesFileNameChanged);
    //connect(properties_items_manager_, &Properties::properties_items_manager::FilesListChanged, this, &MainWindow::fileListChanged);
    //connect(properties_items_manager_, &Properties::properties_items_manager::IncludeNameChanged, this, &MainWindow::fileIncludeNameChanged);
    //connect(properties_items_manager_, &Properties::properties_items_manager::IncludesListChanged, this, &MainWindow::fileIncludesListChanged);

    analysisManager_ = new CubesAnalysis::AnalysisManager(this, this);

    CreateUi();

    //uint32_t fileId{ 0 };
    //fileItemsManager_->Create(QString::fromLocal8Bit("config.xml"), QString::fromLocal8Bit("АРМ"), QString::fromStdString(CubesUnitTypes::platform_names_[0]), fileId);
}

MainWindow::~MainWindow()
{
}

// ITopManager
bool MainWindow::GetUnitsInFileList(const CubesUnitTypes::FileId& fileId, QStringList& unitNames)
{
    return propertiesItemsManager_->GetUnitsInFileList(fileId, unitNames);
}

bool MainWindow::GetUnitsInFileIncludeList(const CubesUnitTypes::FileId& fileId,
    const CubesUnitTypes::IncludeId includeId, QStringList& unitNames)
{
    return propertiesItemsManager_->GetUnitsInFileIncludeList(fileId, includeId, unitNames);
}

bool MainWindow::GetUnitParameters(const QString& unitId, CubesUnitTypes::UnitParameters& unitParameters)
{
    unitParameters = unitParameters_[unitId];
    return true;
}

bool MainWindow::GetFileIncludeList(const CubesUnitTypes::FileId& fileId, CubesUnitTypes::IncludeIdNames& includeNames)
{
    return fileItemsManager_->GetFileIncludeNames(fileId, true, includeNames);
}

bool MainWindow::GetFileIncludeVariableList(const CubesUnitTypes::FileId& fileId,
    const CubesUnitTypes::IncludeId includeId, CubesUnitTypes::VariableIdVariables& variables)
{
    return fileItemsManager_->GetFileIncludeVariables(fileId, includeId, variables);
}

bool MainWindow::CreatePropetiesItem(const QString& unitId, uint32_t& propertiesId)
{
    //instanceName = name + QString("_#%1").arg(unique_number_++);
    //uint32_t propertiesId{ 0 };
    propertiesItemsManager_->Create(unitId, propertiesId);
    auto pi = propertiesItemsManager_->GetItem(propertiesId);
    pi->SetFileIdNames(GetFileNames());
    pi->SetFileIdName(fileItemsManager_->GetCurrentFileId(), fileItemsManager_->GetCurrentFileName());
    pi->SetIncludeIdNames(GetCurrentFileIncludeNames());
    pi->SetIncludeIdName(CubesUnitTypes::InvalidIncludeId, "<not selected>");
    //pi->SetName(GetNewUnitName(pi->GetName()));
    //properties_items_manager_->Select(propertiesId);
    return true;
}

bool MainWindow::GetPropetiesForDrawing(const uint32_t propertiesId, PropertiesForDrawing& pfd)
{
    auto pi = propertiesItemsManager_->GetItem(propertiesId);
    if (pi == nullptr)
        return false;

    pfd.pixmap = pi->GetPixmap();
    QString name;
    if (!propertiesItemsManager_->GetName(propertiesId, name))
        return false;
    pfd.name = name;
    pfd.fileName = pi->GetFileName();
    pfd.includeName = pi->GetIncludeName();
    pfd.color = fileItemsManager_->GetFileColor(pi->GetFileId());

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

bool MainWindow::GetUnitsConnections(QMap<QString, QStringList>& connections)
{
    return propertiesItemsManager_->GetUnitsConnections(connections);
}

bool MainWindow::GetDependsConnections(QMap<QString, QStringList>& connections)
{
    return propertiesItemsManager_->GetDependsConnections(connections);
}

bool MainWindow::EnshureVisible(uint32_t propertiesId)
{
    for (const auto& item : scene_->items())
    {
        CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);
        if (di->GetPropertiesId() == propertiesId)
        {
            QPointF center = di->GetLineAncorPosition();
            //QPointF center = scene_->selectionArea().boundingRect().center();
            view_->centerOn(center);

            scene_->invalidate();

        }
    }

    return true;
}

bool MainWindow::GetAnalysisFiles(QVector<CubesAnalysis::File>& files)
{
    return fileItemsManager_->GetAnalysisFiles(files);
}

bool MainWindow::GetAnalysisProperties(QVector<CubesAnalysis::Properties>& properties)
{
    return propertiesItemsManager_->GetAnalysisProperties(properties);
}

// ILogManager
void MainWindow::AddMessage(const CubesLog::LogMessage& m)
{
    log_table_model_->AddMessage(m);

    table_view_log_->resizeColumnsToContents();
    table_view_log_->horizontalHeader()->setStretchLastSection(false);
    table_view_log_->horizontalHeader()->setStretchLastSection(true);
    table_view_log_->resizeRowsToContents();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (modified_)
    {
        QMessageBox::StandardButton resBtn = QMessageBox::question(this, "cubes",
            QString::fromLocal8Bit("Вы действительно хотите выйти?\nВсе несохраненные изменения будут потеряны!"), QMessageBox::No | QMessageBox::Yes);
        if (resBtn == QMessageBox::Yes)
            event->accept();
        else
            event->ignore();
    }
    else
    {
        event->accept();
    }
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);

    table_view_log_->resizeColumnsToContents();
    table_view_log_->horizontalHeader()->setStretchLastSection(false);
    table_view_log_->horizontalHeader()->setStretchLastSection(true);
    table_view_log_->resizeRowsToContents();
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

    QAction* openFolderAct = new QAction(QString::fromLocal8Bit("Открыть каталог"), this);
    openFolderAct->setShortcut(QKeySequence("Ctrl+K"));
    openFolderAct->setStatusTip(QString::fromLocal8Bit("Открыть все файлы из каталога"));
    connect(openFolderAct, &QAction::triggered, this, &MainWindow::OnOpenFolderAction);

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

    QAction* saveFolderAct = new QAction(QString::fromLocal8Bit("Сохранить каталог"), this);
    saveFolderAct->setShortcut(QKeySequence("Ctrl+L"));
    saveFolderAct->setStatusTip(QString::fromLocal8Bit("Сохранить все файлы в каталог"));
    connect(saveFolderAct, &QAction::triggered, this, &MainWindow::OnSaveFolderAction);

    QAction* quitAct = new QAction(QString::fromLocal8Bit("Выйти"), this);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(QString::fromLocal8Bit("Выйти из приложения"));
    connect(quitAct, &QAction::triggered, this, &MainWindow::OnQuitAction);

    QMenu* fileMenu = menuBar()->addMenu(QString::fromLocal8Bit("Файл"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(openFolderAct);
    fileMenu->addAction(importXmlAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(saveFolderAct);
    fileMenu->addSeparator();
    recentMenu_ = fileMenu->addMenu(QString::fromLocal8Bit("Недавние файлы"));
    fileMenu->addSeparator();
    fileMenu->addAction(quitAct);

    UpdateRecent();

    QAction* sortBoostAct = new QAction(QString::fromLocal8Bit("Сортировать (boost)"), this);
    sortBoostAct->setStatusTip(QString::fromLocal8Bit("Автоматическая сортировка"));
    connect(sortBoostAct, &QAction::triggered, this, &MainWindow::OnSortBoostAction);

    QAction* sortRectAct = new QAction(QString::fromLocal8Bit("Расставить по сетке"), this);
    sortRectAct->setStatusTip(QString::fromLocal8Bit("Автоматическая сортировка"));
    connect(sortRectAct, &QAction::triggered, this, &MainWindow::OnSortRectAction);

    QMenu* editMenu = menuBar()->addMenu(QString::fromLocal8Bit("Правка"));
    editMenu->addAction(sortBoostAct);
    editMenu->addAction(sortRectAct);

    QAction* testAct = new QAction(QString::fromLocal8Bit("Тест"), this);
    testAct->setShortcut(QKeySequence("Ctrl+T"));
    testAct->setStatusTip(QString::fromLocal8Bit("Тест конфигурации"));
    connect(testAct, &QAction::triggered, this, &MainWindow::OnTestAction);

    QMenu* analysisMenu = menuBar()->addMenu(QString::fromLocal8Bit("Анализ"));
    analysisMenu->addAction(testAct);
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

    log_table_model_ = new CubesLog::LogTableModel;
    sort_filter_model_ = new CubesLog::SortFilterModel;
    sort_filter_model_->setSourceModel(log_table_model_);
    sort_filter_model_->SetFilter({ CubesLog::MessageType::error, CubesLog::MessageType::warning, CubesLog::MessageType::information });

    table_view_log_->setModel(sort_filter_model_);
    table_view_log_->setSortingEnabled(true);
    table_view_log_->verticalHeader()->hide();
    table_view_log_->horizontalHeader()->setHighlightSections(false);
    //table_view_log_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //table_view_log_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table_view_log_->horizontalHeader()->setStretchLastSection(true);
    table_view_log_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_view_log_->setSelectionMode(QAbstractItemView::SingleSelection);
    //table_view_log_->sortByColumn(0, Qt::AscendingOrder);
    //table_view_log_->resizeColumnsToContents();
    qDebug() << connect(table_view_log_, &QTableView::doubleClicked, this, &MainWindow::OnDoubleClicked);

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
    scene_ = new CubesDiagram::DiagramScene(this);
    scene_->setSceneRect(-10000, -10000, 20032, 20032);

    qDebug() << connect(scene_, &CubesDiagram::DiagramScene::ItemPositionChanged, this, &MainWindow::DiagramItemPositionChanged);
    qDebug() << connect(scene_, &CubesDiagram::DiagramScene::AfterItemCreated, this, &MainWindow::DiagramAfterItemCreated);
    qDebug() << connect(scene_, &CubesDiagram::DiagramScene::BeforeItemDeleted, this, &MainWindow::DiagramBeforeItemDeleted);
    //qDebug() << connect(scene_, &CubeDiagram::DiagramScene::ItemNameChanged, this, &MainWindow::DiagramItemNameChanged);
    //qDebug() << connect(scene_, &CubeDiagram::DiagramScene::ItemFileChanged, this, &MainWindow::DiagramItemFileChanged);
    //qDebug() << connect(scene_, &CubeDiagram::DiagramScene::ItemGroupChanged, this, &MainWindow::DiagramItemGroupChanged);
    qDebug() << connect(scene_, &CubesDiagram::DiagramScene::selectionChanged, this, &MainWindow::selectionChanged);
}

void MainWindow::CreateView()
{
    view_ = new CubesDiagram::DiagramView(this, scene_);
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
                    CubesLog::LogMessage lm{};
                    lm.type = CubesLog::MessageType::error;
                    lm.tag = 0;
                    lm.source = filename;
                    lm.description = QString::fromLocal8Bit("Файл параметров %1 не разобран. Параметры не добавлены.").arg(fullPath);
                    AddMessage(lm);
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
        CubesUnitTypes::IncludeIdNames includes;
        if (!fileItemsManager_->GetFileIncludeNames(fileId, false, includes))
            return false;
        QDir dir = QFileInfo(file.fileName).absoluteDir();

        for (const auto& includeId : includes.keys())
        {
            QString includePath;
            if (!fileItemsManager_->GetFileIncludePath(fileId, includeId, includePath))
                return false;

            QString includeName = dir.filePath(includePath);
            CubesXml::File includedFile{};
            if (!CubesXml::Helper::Parse(includeName, includedFile, this))
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
        CubesUnitTypes::IncludeIdNames includes;
        if (!fileItemsManager_->GetFileIncludeNames(fileId, false, includes))
            return false;

        for (const auto& includeId : includes.keys())
        {
            QString includePath;
            if (!fileItemsManager_->GetFileIncludePath(fileId, includeId, includePath))
                return false;

            QByteArray byteArray;
            if (!CubesZip::UnZipFile(zipFileName, includePath, byteArray))
                return false;

            CubesXml::File includedFile{};
            if (!CubesXml::Helper::Parse(byteArray, includePath, includedFile, this))
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
                CubesLog::LogMessage lm{};
                lm.type = CubesLog::MessageType::error;
                lm.tag = 0;
                lm.source = QFileInfo(file.fileName).fileName();
                lm.description = QString::fromLocal8Bit("Нет файла параметров для юнита %1 (%2). Юнит не добавлен.").arg(u.name, u.id);
                AddMessage(lm);
            }
        }
    }

    // Get fileNames list
    CubesUnitTypes::FileIdNames fileNames = fileItemsManager_->GetFileNames();
    QString fileName = fileItemsManager_->GetFileName(fileId);

    // Transform
    CubesDiagram::DiagramItem* di = nullptr;
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
            pi->SetFileIdNames(fileNames);
            pi->SetFileIdName(fileId, fileName);
            //if (includedFileName != "")
            {
                CubesUnitTypes::IncludeIdNames includes;
                if (!fileItemsManager_->GetFileIncludeNames(fileId, true, includes))
                    return false;

                QString includeName;
                if (!fileItemsManager_->GetFileIncludeName(fileId, includeId, includeName))
                    includeName = "<not selected>";

                pi->SetIncludeIdNames(includes);
                pi->SetIncludeIdName(includeId, includeName);
            }

            PropertiesForDrawing pfd{};
            if (!GetPropetiesForDrawing(propertiesId, pfd))
            {
                qDebug() << "ERROR GetPropeties: " << propertiesId;
            }

            di = new CubesDiagram::DiagramItem(propertiesId, pfd.pixmap, pfd.name, pfd.fileName, pfd.includeName, pfd.color);
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
        CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);

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
        CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);

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
        pi->SetPosition(di->pos());

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
            CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);
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
            CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);
            QPoint position(c * 200, r * 80);
            di->setPos(position);

            auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
            pi->SetPosition(di->pos());

            if (++c == columns) { ++r; c = 0; };
        }
        QPointF center = scene_->itemsBoundingRect().center();
        view_->centerOn(center);

        scene_->invalidate();
    }

    return true;
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

bool MainWindow::Test()
{
    log_table_model_->Clear();
    analysisManager_->Test();
    return true;
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
    CubesUnitTypes::IncludeIdNames includes;
    if (!fileItemsManager_->GetFileIncludeNames(fileId, true, includes))
        return {};

    return includes;
}

//QColor MainWindow::GetFileColor(const QString& fileName)
//{
//    return fileItemsManager_->GetFileColor(fileName);
//}

//QString MainWindow::GetDisplayName(const QString& baseName)
//{
//    CubesUnitTypes::VariableIdVariables variables;
//    for (const auto& item : scene_->items())
//    {
//        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
//        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
//        if (pi->GetIncludeName() != "<not selected>")
//        {
//            auto fileId = fileItemsManager_->GetFileId(pi->GetName());
//            if (!fileItemsManager_->GetFileIncludeVariables(fileId, pi->GetIncludeId(), variables))
//                return {};
//        }
//    }
//
//    QString realName = baseName;
//    for (const auto& v : variables)
//    {
//        QString replace = QString("@%1@").arg(v.first);
//        realName.replace(replace, v.second);
//    }
//
//    return realName;
//}

void MainWindow::UpdateFileState(const QString& path, bool modified)
{
    QString title = path.isEmpty() ? "untitled" : path;
    if (modified) title += "*";
    setWindowTitle(title);
    path_ = path;
    modified_ = modified;
}

bool MainWindow::SaveFileInternal(const QString& path)
{
    log_table_model_->Clear();

    // Получаем список главных файлов
    bool is_first = true;
    CubesUnitTypes::FileIdNames fileNames = fileItemsManager_->GetFileNames();
    for (const auto& kvpFile : fileNames.toStdMap())
    {
        auto xmlFile = fileItemsManager_->GetXmlFile(kvpFile.first);
        QFileInfo xmlFileInfo(xmlFile.fileName);
        const auto xmlFileName = xmlFileInfo.fileName();
        const auto xmlZipFilePath = path;

        {
            auto xmlGroups = propertiesItemsManager_->GetXmlGroups(kvpFile.first);
            xmlFile.config.groups = std::move(xmlGroups);

            QByteArray byteArray;
            if (!CubesXml::Helper::Write(byteArray, xmlFile))
                return false;

            if (is_first)
            {
                if (!CubesZip::ZipFile(byteArray, xmlFileName, xmlZipFilePath, CubesZip::ZipMethod::Create))
                    return false;

                is_first = false;
            }
            else
            {
                if (!CubesZip::ZipFile(byteArray, xmlFileName, xmlZipFilePath, CubesZip::ZipMethod::Append))
                    return false;
            }
            //CubesXml::Writer::Write(xmlFileName, xmlFile);
            //CubesZip::ZipFile(xmlFileName, xmlZipFileName, CubesZip::ZipMethod::Create);
        }

        CubesUnitTypes::IncludeIdNames includes;
        if (!fileItemsManager_->GetFileIncludeNames(kvpFile.first, false, includes))
            return false;

        for (const auto& kvpInclude : includes.toStdMap())
        {
            auto includeGroups = propertiesItemsManager_->GetXmlGroups(kvpFile.first, kvpInclude.first);
            CubesXml::File includeXmlFile{};

            const auto item = fileItemsManager_->GetItem(kvpFile.first);
            const auto includeName = item->GetIncludeName(kvpInclude.first);
            const auto includePath = item->GetIncludePath(kvpInclude.first);

            ///includeXmlFile.name = includeName;
            //includeXmlFile.platform = xmlFile.platform;
            includeXmlFile.fileName = includePath;

            includeXmlFile.config.logIsSet = false;
            includeXmlFile.config.networkingIsSet = false;
            includeXmlFile.config.groups = includeGroups;


            QFileInfo includeXmlFileInfo(includeXmlFile.fileName);
            const auto includeXmlFileName = includeXmlFileInfo.fileName();


            QByteArray byteArray;
            if (!CubesXml::Helper::Write(byteArray, includeXmlFile))
                return false;

            if (!CubesZip::ZipFile(byteArray, includeXmlFileName, xmlZipFilePath, CubesZip::ZipMethod::Append))
                return false;

            //CubesXml::Writer::Write(includeXmlFileName, includeXmlFile);
            //CubesZip::ZipFile(includeXmlFileName, xmlZipFilePath, CubesZip::ZipMethod::Append);
        }
    }

    AddRecent(path);
    UpdateFileState(path, false);

    return true;
}

bool MainWindow::SaveFolderInternal(const QString& path)
{
    log_table_model_->Clear();

    QFileInfo fi(path);
    if (!fi.exists() || !fi.isDir())
        return false;

    QDir dir(path);

    // Получаем список главных файлов
    bool is_first = true;
    CubesUnitTypes::FileIdNames fileNames = fileItemsManager_->GetFileNames();
    for (const auto& kvpFile : fileNames.toStdMap())
    {
        auto xmlFile = fileItemsManager_->GetXmlFile(kvpFile.first);
        QFileInfo xmlFileInfo(xmlFile.fileName);
        const auto xmlFileName = xmlFileInfo.fileName();
        const auto xmlZipFilePath = path;

        {
            auto xmlGroups = propertiesItemsManager_->GetXmlGroups(kvpFile.first);
            xmlFile.config.groups = std::move(xmlGroups);

            if (!CubesXml::Helper::Write(dir.filePath(xmlFileName), xmlFile))
                return false;


            //QByteArray byteArray;
            //if (!CubesXml::Helper::Write(byteArray, xmlFile))
            //    return false;

            //if (is_first)
            //{
            //    if (!CubesZip::ZipFile(byteArray, xmlFileName, xmlZipFilePath, CubesZip::ZipMethod::Create))
            //        return false;

            //    is_first = false;
            //}
            //else
            //{
            //    if (!CubesZip::ZipFile(byteArray, xmlFileName, xmlZipFilePath, CubesZip::ZipMethod::Append))
            //        return false;
            //}
            //CubesXml::Writer::Write(xmlFileName, xmlFile);
            //CubesZip::ZipFile(xmlFileName, xmlZipFileName, CubesZip::ZipMethod::Create);
        }

        CubesUnitTypes::IncludeIdNames includes;
        if (!fileItemsManager_->GetFileIncludeNames(kvpFile.first, false, includes))
            return false;
        for (const auto& kvpInclude : includes.toStdMap())
        {
            auto includeGroups = propertiesItemsManager_->GetXmlGroups(kvpFile.first, kvpInclude.first);
            CubesXml::File includeXmlFile{};

            const auto item = fileItemsManager_->GetItem(kvpFile.first);
            const auto includeName = item->GetIncludeName(kvpInclude.first);
            const auto includePath = item->GetIncludePath(kvpInclude.first);

            ///includeXmlFile.name = includeName;
            //includeXmlFile.platform = xmlFile.platform;
            includeXmlFile.fileName = includePath;

            includeXmlFile.config.logIsSet = false;
            includeXmlFile.config.networkingIsSet = false;
            includeXmlFile.config.groups = includeGroups;


            QFileInfo includeXmlFileInfo(includeXmlFile.fileName);
            const auto includeXmlFileName = includeXmlFileInfo.fileName();

            if (!CubesXml::Helper::Write(dir.filePath(includeXmlFileName), includeXmlFile))
                return false;


            //QByteArray byteArray;
            //if (!CubesXml::Helper::Write(byteArray, includeXmlFile))
            //    return false;

            //if (!CubesZip::ZipFile(byteArray, includeXmlFileName, xmlZipFilePath, CubesZip::ZipMethod::Append))
            //    return false;

            //CubesXml::Writer::Write(includeXmlFileName, includeXmlFile);
            //CubesZip::ZipFile(includeXmlFileName, xmlZipFilePath, CubesZip::ZipMethod::Append);
        }
    }

    //AddRecent(path);
    //UpdateFileState(path, false);

    return true;
}

bool MainWindow::OpenFileInternal(const QString& path)
{
    scene_->clear();
    propertiesItemsManager_->Clear();
    fileItemsManager_->Clear();
    log_table_model_->Clear();

    UpdateFileState("", false);

    QFileInfo fi(path);
    if (!fi.exists() || !fi.isFile())
        return false;

    QList<QString> fileNames;
    if (!CubesZip::GetZippedFileNames(path, fileNames))
        return false;

    for (const auto& fileName : fileNames)
    {
        QByteArray byteArray;
        if (!CubesZip::UnZipFile(path, fileName, byteArray))
            return false;

        CubesXml::File f{};
        CubesXml::Helper::Parse(byteArray, fileName, f, this);

        if (f.config.networkingIsSet)
        {
            if (!AddMainFile(f, path))
                return false;
        }
        else
        {
            continue;
        }
    }

    for (auto& item : scene_->items())
    {
        CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);

        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
        const auto position = pi->GetPosition();
        di->setPos(position);
    }

    AddRecent(path);
    UpdateFileState(path, false);

    return true;
}

bool MainWindow::OpenFolderInternal(const QString& path)
{
    scene_->clear();
    propertiesItemsManager_->Clear();
    fileItemsManager_->Clear();
    log_table_model_->Clear();

    UpdateFileState("", false);

    QFileInfo fi(path);
    if (!fi.exists() || !fi.isDir())
        return false;

    QDir dir(path);
    QStringList fileNames = dir.entryList({ "*.xml" }, QDir::Files);

    for (const auto& fileName : fileNames)
    {
        CubesXml::File f{};
        CubesXml::Helper::Parse(dir.filePath(fileName), f, this);

        if (f.config.networkingIsSet)
        {
            if (!AddMainFile(f, ""))
                return false;
        }
        else
        {
            continue;
        }
    }

    for (auto& item : scene_->items())
    {
        CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);

        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
        const auto position = pi->GetPosition();
        di->setPos(position);
    }

    UpdateFileState("", true);

    //AddRecent(path);
    //UpdateFileState(path, false);

    return true;
}

void MainWindow::UpdateRecent()
{
    recentMenu_->clear();

    QString iniFileName = QDir(QCoreApplication::applicationDirPath()).filePath("settings.ini");
    QSettings app_settings(iniFileName, QSettings::IniFormat);
    int recent_count = app_settings.value("recent/count", "0").toInt();
    if (recent_count == 0)
    {
        QAction* recentAct = new QAction(QString::fromLocal8Bit("<список пуст>"), this);
        recentAct->setStatusTip(QString::fromLocal8Bit("Список пуст"));
        recentMenu_->addAction(recentAct);
    }
    else
    {
        for (int i = 0; i < recent_count; i++)
        {
            QString name = QString("recent/filename_%1").arg(i);
            QString path = app_settings.value(name, "").toString();
            QAction* recentAct = new QAction(path, this);
            recentAct->setStatusTip(QString::fromLocal8Bit("Открыть файл %1").arg(path));
            connect(recentAct, &QAction::triggered, this, &MainWindow::OnRecentAction);
            recentMenu_->addAction(recentAct);
        }
    }
}

void MainWindow::AddRecent(QString fileName)
{
    QString iniFileName = QDir(QCoreApplication::applicationDirPath()).filePath("settings.ini");
    QSettings app_settings(iniFileName, QSettings::IniFormat);
    int recent_count = app_settings.value("recent/count", "0").toInt();
    QList<QString> list;
    for (int i = 0; i < recent_count; i++)
    {
        QString name = QString("recent/filename_%1").arg(i);
        QString path = app_settings.value(name, "").toString();
        path.replace("\\", "/");
        list.push_back(path);
    }

    app_settings.beginGroup("recent");
    app_settings.remove(""); //removes the group, and all it keys
    app_settings.endGroup();

    fileName.replace("\\", "/");
    if (list.contains(fileName))
        list.removeAll(fileName);
    list.push_front(fileName);
    while (list.size() > 10)
        list.pop_back();

    app_settings.setValue("recent/count", list.size());
    for (int i = 0; i < list.size(); i++)
    {
        QString name = QString("recent/filename_%1").arg(i);
        app_settings.setValue(name, list[i]);
    }
    app_settings.sync();

    UpdateRecent();
}

void MainWindow::RemoveRecent(QString fileName)
{
    QString iniFileName = QDir(QCoreApplication::applicationDirPath()).filePath("settings.ini");
    QSettings app_settings(iniFileName, QSettings::IniFormat);
    int recent_count = app_settings.value("recent/count", "0").toInt();
    QList<QString> list;
    for (int i = 0; i < recent_count; i++)
    {
        QString name = QString("recent/filename_%1").arg(i);
        QString path = app_settings.value(name, "").toString();
        path.replace("\\", "/");
        list.push_back(path);
    }

    app_settings.beginGroup("recent");
    app_settings.remove(""); //removes the group, and all it keys
    app_settings.endGroup();

    fileName.replace("\\", "/");
    if (list.contains(fileName))
        list.removeAll(fileName);
    while (list.size() > 10)
        list.pop_back();

    app_settings.setValue("recent/count", list.size());
    for (int i = 0; i < list.size(); i++)
    {
        QString name = QString("recent/filename_%1").arg(i);
        app_settings.setValue(name, list[i]);
    }
    app_settings.sync();

    UpdateRecent();
}

// DiagramScene (as manager)
void MainWindow::DiagramItemPositionChanged(CubesDiagram::DiagramItem* di)
{
    auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
    pi->SetPosition(di->pos());
    
    UpdateFileState(path_, true);
}

void MainWindow::DiagramAfterItemCreated(CubesDiagram::DiagramItem* di)
{
    propertiesItemsManager_->Select(di->propertiesId_);

    UpdateFileState(path_, true);
}

void MainWindow::DiagramBeforeItemDeleted(CubesDiagram::DiagramItem* di)
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
        CubesDiagram::DiagramItem* di = (CubesDiagram::DiagramItem*)(scene_->selectedItems()[0]);
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
    auto fileName = fileItemsManager_->GetFileName(fileId);
    for (auto& item : scene_->items())
    {
        CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);
        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);

        if (pi->GetFileId() == fileId)
            pi->SetFileIdName(fileId, fileName);
    }

    UpdateFileState(path_, true);

    scene_->invalidate();
}

void MainWindow::FileListChanged(const CubesUnitTypes::FileIdNames& fileNames)
{
    for (auto& item : scene_->items())
    {
        CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);
        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
        pi->SetFileIdNames(fileNames);

        // Если item был добавлен, когда нет ни одного файла, pm->key будет не задан
        // После добавления, у них изменится имя файла и цвет
        // TODO: возможно изменится имя - но не должно
        const auto color = fileItemsManager_->GetFileColor(pi->GetFileId());
        di->InformColorChanged(color);
    }

    UpdateFileState(path_, true);

    scene_->invalidate();
}

void MainWindow::FileIncludeNameChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId& includeId)
{
    QString includeName;
    if (!fileItemsManager_->GetFileIncludeName(fileId, includeId, includeName))
        return;

    for (auto& item : scene_->items())
    {
        CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);
        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
        if (pi->GetFileId() == fileId && pi->GetIncludeId() == includeId)
            pi->SetIncludeIdName(includeId, includeName);

        //const auto includeFileId = pi->GetIncludeFileId();
        //pi->SetIncludeNames(fileIncludeNames);
        //if (includeFileId == oldIncludeId)
        //{
        //    const auto includeName = fileItemsManager_->GetFileIncludeName();
        //    pi->SetIncludeName(includeName);
        //}
    }

    UpdateFileState(path_, true);

    scene_->invalidate();
}

void MainWindow::FileIncludesListChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeIdNames& includeNames)
{
    for (auto& item : scene_->items())
    {
        CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);
        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
        if (pi->GetFileId() == fileId)
            pi->SetIncludeIdNames(includeNames);
    }

    UpdateFileState(path_, true);

    scene_->invalidate();
}

void MainWindow::FileVariableNameChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId& includeId,
    const QString& variableName, const QString& oldVariableName)
{
    propertiesItemsManager_->InformVariableChanged();

    UpdateFileState(path_, true);
}

void MainWindow::FileVariablesListChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId& includeId,
    const CubesUnitTypes::VariableIdVariables& variables)
{
    propertiesItemsManager_->InformVariableChanged();

    UpdateFileState(path_, true);
}

void MainWindow::FileColorChanged(const CubesUnitTypes::FileId& fileId, const QColor& color)
{
    for (auto& item : scene_->items())
    {
        CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);

        auto pi = propertiesItemsManager_->GetItem(di->GetPropertiesId());
        if (pi->GetFileId() == fileId)
            di->color_ = color;
    }

    UpdateFileState(path_, true);
    
    scene_->invalidate();
}

void MainWindow::FilePropertiesChanged()
{
    UpdateFileState(path_, true);
}

//void MainWindow::FileVariableChanged(const QString& fileName, const QString& includeName, const QList<QPair<QString, QString>>& variables)
//{
//    propertiesItemsManager_->InformVariableChanged();
//}

// PropertiesItemsManager
void MainWindow::PropertiesBasePropertiesChanged(const uint32_t propertiesId, const QString& name,
    const QString& fileName, const QString& includeName)
{
    for (auto& item : scene_->items())
    {
        CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);
        if (di->propertiesId_ == propertiesId)
        {
            di->name_ = name;
            di->fileName_ = fileName;
            di->includeName_ = includeName;
            auto fileId = fileItemsManager_->GetFileId(fileName);
            di->color_ = fileItemsManager_->GetFileColor(fileId);
            di->InformNameChanged(name, "");
            di->InformIncludeChanged();
        }
    }

    UpdateFileState(path_, true);
    
    scene_->invalidate();
}

void MainWindow::PropertiesSelectedItemChanged(const uint32_t propertiesId)
{
    QGraphicsItem* item_to_select = nullptr;
    for (auto& item : scene_->items())
    {
        CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);
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
        CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);
        if (di->propertiesId_ == propertiesId)
        {
            di->setPos(QPointF(posX, posY));
            di->setZValue(posZ);
            break;
        }
    }

    UpdateFileState(path_, true);
}

void MainWindow::PropertiesError(const uint32_t propertiesId, const QString& message)
{
    CubesLog::LogMessage lm{};
    lm.type = CubesLog::MessageType::error;
    lm.tag = 0;
    lm.source = QString("%1").arg(propertiesId);
    lm.description = message;
    AddMessage(lm);
}

void MainWindow::PropertiesConnectionChanged(const uint32_t propertiesId)
{
    scene_->invalidate();

    UpdateFileState(path_, true);
}

void MainWindow::PropertiesPropertiesChanged()
{
    UpdateFileState(path_, true);
}

// Кнопки
void MainWindow::OnNewFileAction()
{
    if (modified_)
    {
        QMessageBox::StandardButton resBtn = QMessageBox::question(this, "cubes",
            QString::fromLocal8Bit("Вы действительно хотите создать новый файл?\nВсе несохраненные изменения будут потеряны!"), QMessageBox::No | QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes)
            return;
    }

    scene_->clear();
    propertiesItemsManager_->Clear();
    fileItemsManager_->Clear();
    log_table_model_->Clear();

    UpdateFileState("", false);
}

void MainWindow::OnOpenFileAction()
{
    if (modified_)
    {
        QMessageBox::StandardButton resBtn = QMessageBox::question(this, "cubes",
            QString::fromLocal8Bit("Вы действительно хотите открыть файл?\nВсе несохраненные изменения будут потеряны!"), QMessageBox::No | QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes)
            return;
    }

    QFileDialog dialog(this);
    dialog.setNameFilter("Parameters Archive Files (*.xlmx)");
    dialog.setAcceptMode(QFileDialog::AcceptOpen);

    QStringList selectedFileNames;
    if (dialog.exec())
        selectedFileNames = dialog.selectedFiles();

    if (selectedFileNames.size() == 0)
        return;

    if (!OpenFileInternal(selectedFileNames[0]))
    {
        QMessageBox::critical(this, "Error", QString::fromLocal8Bit("Ошибка открытия файла!"));
        RemoveRecent(selectedFileNames[0]);
        return;
    }
}

void MainWindow::OnOpenFolderAction()
{
    if (modified_)
    {
        QMessageBox::StandardButton resBtn = QMessageBox::question(this, "cubes",
            QString::fromLocal8Bit("Вы действительно хотите открыть файл?\nВсе несохраненные изменения будут потеряны!"), QMessageBox::No | QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes)
            return;
    }

    QString folderPath = QFileDialog::getExistingDirectory(0, ("Select Output Folder"), QDir::currentPath());
    if (folderPath.isEmpty())
        return;

    if (!OpenFolderInternal(folderPath))
    {
        QMessageBox::critical(this, "Error", QString::fromLocal8Bit("Ошибка открытия каталога!"));
        return;
    }
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

        CubesXml::Helper::Parse(fileNames[0], f, this);
    }

    if (f.config.networkingIsSet)
    {
        if (!AddMainFile(f, ""))
        {
            QMessageBox::critical(this, "Error", QString::fromLocal8Bit("Ошибка импорта файла!"));
            return;
        }
    }
    else
    {
        QMessageBox::critical(this, "Error", QString::fromLocal8Bit("Это подключаемый файл, нельзя его импортировать."));
    }
}

void MainWindow::OnSaveFileAction()
{
    if (!modified_ && !path_.isEmpty())
        return;

    QString selectedFileName = path_;
    if (path_.isEmpty())
    {
        QFileDialog dialog(this);
        dialog.setNameFilters({ "Parameters Archive Files (*.xlmx)" });
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        dialog.setDefaultSuffix("xmlx");

        QStringList selectedFileNames;
        if (dialog.exec())
            selectedFileNames = dialog.selectedFiles();
        if (selectedFileNames.size() <= 0)
            return;

        selectedFileName = selectedFileNames[0];
    }

    if (!SaveFileInternal(selectedFileName))
    {
        QMessageBox::critical(this, "Error", QString::fromLocal8Bit("Ошибка сохранения файла!"));
        return;
    }
}

void MainWindow::OnSaveAsFileAction()
{
    QFileDialog dialog(this);
    dialog.setNameFilters({ "Parameters Archive Files (*.xlmx)" });
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("xmlx");

    QStringList selectedFileNames;
    if (dialog.exec())
        selectedFileNames = dialog.selectedFiles();
    if (selectedFileNames.size() <= 0)
        return;

    QString selectedFileName = selectedFileNames[0];

    if (!SaveFileInternal(selectedFileName))
    {
        QMessageBox::critical(this, "Error", QString::fromLocal8Bit("Ошибка сохранения файла!"));
        return;
    }
}

void MainWindow::OnSaveFolderAction()
{
    QString folderPath = QFileDialog::getExistingDirectory(0, ("Select Output Folder"), QDir::currentPath());
    if (folderPath.isEmpty())
        return;

    if (!SaveFolderInternal(folderPath))
    {
        QMessageBox::critical(this, "Error", QString::fromLocal8Bit("Ошибка сохранения каталога!"));
        return;
    }
}

void MainWindow::OnQuitAction()
{
    if (modified_)
    {
        QMessageBox::StandardButton resBtn = QMessageBox::question(this, "cubes",
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

void MainWindow::OnRecentAction()
{
    if (modified_)
    {
        QMessageBox::StandardButton resBtn = QMessageBox::question(this, "cubes",
            QString::fromLocal8Bit("Вы действительно хотите открыть файл?\nВсе несохраненные изменения будут потеряны!"), QMessageBox::No | QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes)
            return;
    }

    QAction* act = qobject_cast<QAction*>(sender());
    if (!OpenFileInternal(act->text()))
    {
        QMessageBox::critical(this, "Error", QString::fromLocal8Bit("Ошибка открытия файла!"));
        RemoveRecent(act->text());
        return;
    }
}

void MainWindow::OnTestAction()
{
    Test();
}

// Лог
void MainWindow::OnErrorButtonClicked(bool checked)
{
    if (checked)
        sort_filter_model_->AddToFilter(CubesLog::MessageType::error);
    else
        sort_filter_model_->RemoveFromFilter(CubesLog::MessageType::error);
}

void MainWindow::OnWarningButtonClicked(bool checked)
{
    if (checked)
        sort_filter_model_->AddToFilter(CubesLog::MessageType::warning);
    else
        sort_filter_model_->RemoveFromFilter(CubesLog::MessageType::warning);
}

void MainWindow::OnInformationButtonClicked(bool checked)
{
    if (checked)
        sort_filter_model_->AddToFilter(CubesLog::MessageType::information);
    else
        sort_filter_model_->RemoveFromFilter(CubesLog::MessageType::information);
}

void MainWindow::OnDoubleClicked(const QModelIndex& index)
{
    CubesLog::LogMessage m{};
    const auto mapped = sort_filter_model_->mapToSource(index);
    if (log_table_model_->GetMessage(mapped.row(), m))
    {
        // TODO: Properties Manager и т.п. в enum
        if (m.source == "Properties Manager" || m.source == "Properties analysis")
        {
            CubesUnitTypes::PropertiesId propertiesId = m.tag;
            propertiesItemsManager_->Select(propertiesId);
        }
    }
}
