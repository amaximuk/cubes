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
#include "qttreepropertybrowser.h" // потом убрать
#include "../diagram/diagram_view.h"
#include "../diagram/diagram_scene.h"
#include "../diagram/diagram_item.h"
#include "../file/file_item.h"
#include "../file/file_items_manager.h"
#include "../log/log_table_model.h"
#include "../log/sort_filter_model.h"
#include "../parameters/base64.h"
#include "../properties/properties_item.h"
#include "../properties/properties_items_manager.h"
#include "../analysis/analysis_manager.h"
#include "../tree/tree_item_model.h"
#include "../xml/xml_parser.h"
#include "../xml/xml_writer.h"
#include "../xml/xml_helper.h"
#include "../utils/zip.h"
#include "main_window.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), TopManager(false)
{
    modified_ = false;
    uniqueNumber_ = 0;

    setWindowIcon(QIcon(":/images/cubes.png"));
    UpdateFileState("", false);

    //fileItemsManager_ = new CubesFile::FileItemsManager(this, this);
    //connect(fileItemsManager_, &CubesFile::FileItemsManager::FileNameChanged, this, &MainWindow::FileNameChanged);
    //connect(fileItemsManager_, &CubesFile::FileItemsManager::FilesListChanged, this, &MainWindow::FileListChanged);
    //connect(fileItemsManager_, &CubesFile::FileItemsManager::IncludeNameChanged, this, &MainWindow::FileIncludeNameChanged);
    //connect(fileItemsManager_, &CubesFile::FileItemsManager::IncludesListChanged, this, &MainWindow::FileIncludesListChanged);
    //connect(fileItemsManager_, &CubesFile::FileItemsManager::VariableNameChanged, this, &MainWindow::FileVariableNameChanged);
    //connect(fileItemsManager_, &CubesFile::FileItemsManager::VariablesListChanged, this, &MainWindow::FileVariablesListChanged);
    //connect(fileItemsManager_, &CubesFile::FileItemsManager::ColorChanged, this, &MainWindow::FileColorChanged);
    //connect(fileItemsManager_, &CubesFile::FileItemsManager::PropertiesChanged, this, &MainWindow::FilePropertiesChanged);
    //
    //propertiesItemsManager_ = new CubesProperties::PropertiesItemsManager(this, this, false);
    //connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::BasePropertiesChanged, this, &MainWindow::PropertiesBasePropertiesChanged);
    //connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::SelectedItemChanged, this, &MainWindow::PropertiesSelectedItemChanged);
    //connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::PositionChanged, this, &MainWindow::PropertiesPositionChanged);
    //connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::Error, this, &MainWindow::PropertiesError);
    //connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::ConnectionChanged, this, &MainWindow::PropertiesConnectionChanged);
    //connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::PropertiesChanged, this, &MainWindow::PropertiesPropertiesChanged);

    //analysisManager_ = new CubesAnalysis::AnalysisManager(this, this);

    CreateUi();
}

MainWindow::~MainWindow()
{
}
/*
// ITopManager
bool MainWindow::GetUnitsInFileList(const CubesUnit::FileId& fileId, QStringList& unitNames)
{
    return propertiesItemsManager_->GetUnitsInFileList(fileId, unitNames);
}

bool MainWindow::GetUnitsInFileIncludeList(const CubesUnit::FileId& fileId,
    const CubesUnit::IncludeId includeId, QStringList& unitNames)
{
    return propertiesItemsManager_->GetUnitsInFileIncludeList(fileId, includeId, unitNames);
}

bool MainWindow::GetUnitParameters(const QString& unitId, CubesUnit::UnitParameters& unitParameters)
{
    unitParameters = unitParameters_[unitId];
    return true;
}

bool MainWindow::GetFileIncludeList(const CubesUnit::FileId& fileId, CubesUnit::IncludeIdNames& includeNames)
{
    return fileItemsManager_->GetFileIncludeNames(fileId, true, includeNames);
}

bool MainWindow::GetFileIncludeVariableList(const CubesUnit::FileId& fileId,
    const CubesUnit::IncludeId includeId, CubesUnit::VariableIdVariables& variables)
{
    return fileItemsManager_->GetFileIncludeVariables(fileId, includeId, variables);
}

bool MainWindow::CreatePropetiesItem(const QString& unitId, uint32_t& propertiesId)
{
    propertiesItemsManager_->Create(unitId, propertiesId);
    auto pi = propertiesItemsManager_->GetItem(propertiesId);
    pi->SetFileIdNames(GetFileNames());
    pi->SetFileIdName(fileItemsManager_->GetCurrentFileId(), fileItemsManager_->GetCurrentFileName());
    pi->SetIncludeIdNames(GetCurrentFileIncludeNames());
    pi->SetIncludeIdName(CubesUnit::InvalidIncludeId, "<not selected>");
    return true;
}

bool MainWindow::GetPropetiesForDrawing(const CubesUnit::PropertiesId propertiesId, CubesTop::PropertiesForDrawing& pfd)
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

bool MainWindow::GetPropetiesUnitParameters(const CubesUnit::PropertiesId propertiesId, CubesUnit::UnitParameters& unitParameters)
{
    return propertiesItemsManager_->GetUnitParameters(propertiesId, unitParameters);
}

bool MainWindow::GetPropetiesUnitId(CubesUnit::PropertiesId propertiesId, QString& unitId)
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
*/
bool MainWindow::CreateDiagramItem(CubesUnit::PropertiesId propertiesId)
{
    TopManager::CreateDiagramItem(propertiesId);

    CubesTop::PropertiesForDrawing pfd{};

    const auto pi = propertiesItemsManager_->GetItem(propertiesId);
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

    const auto pos = pi->GetPosition();
    const auto z = pi->GetZOrder();

    auto di = new CubesDiagram::DiagramItem(propertiesId, pfd.pixmap, pfd.name, pfd.fileName, pfd.includeName, pfd.color);
    di->setX(pos.x());
    di->setY(pos.y());
    di->setZValue(z);
    scene_->addItem(di);

    scene_->clearSelection();
    propertiesItemsManager_->Select(CubesUnit::InvalidPropertiesId);
    DiagramAfterItemCreated(di);

    return true;
}

bool MainWindow::EnshureVisible(CubesUnit::PropertiesId propertiesId)
{
    TopManager::EnshureVisible(propertiesId);

    for (const auto& item : scene_->items())
    {
        CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);
        if (di->GetPropertiesId() == propertiesId)
        {
            QPointF center = di->GetLineAncorPosition();
            view_->centerOn(center);
            scene_->invalidate();
        }
    }

    return true;
}
/*
bool MainWindow::GetAnalysisFiles(QVector<CubesAnalysis::File>& files)
{
    return fileItemsManager_->GetAnalysisFiles(files);
}

bool MainWindow::GetAnalysisProperties(QVector<CubesAnalysis::Properties>& properties)
{
    return propertiesItemsManager_->GetAnalysisProperties(properties);
}
*/
// ILogManager
void MainWindow::AddMessage(const CubesLog::Message& m)
{
    TopManager::AddMessage(m);

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
    CreateTreeView();
    FillTreeView();

    CreateScene();
    CreateView();

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

    QSplitter* tabVSplitter = new QSplitter(Qt::Vertical);
    tabVSplitter->addWidget(filesWidget);
    tabVSplitter->addWidget(propertiesWidget);
    tabVSplitter->setStretchFactor(0, 0);
    tabVSplitter->setStretchFactor(1, 1);

    QVBoxLayout* propertiesPaneLayout = new QVBoxLayout;
    propertiesPaneLayout->addWidget(tabVSplitter);
    propertiesPaneLayout->setContentsMargins(0, 0, 0, 0);

    propertiesPanelWidget->setLayout(propertiesPaneLayout);

    return propertiesPanelWidget;
}

void MainWindow::FillTreeView()
{
    TreeItemModel* model = new TreeItemModel();

    QMap<QString, QSet<QString>> categoriesMap;
    for (const auto& up : unitIdUnitParametersPtr_)
    {
        QString category = "default";
        if (up->fileInfo.info.category != "")
            category = QString::fromStdString(up->fileInfo.info.category).toLower();
        categoriesMap[category].insert(QString::fromStdString(up->fileInfo.info.id));
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
            if (unitIdUnitParametersPtr_[id]->fileInfo.info.pictogram != "")
            {
                std::string s = base64_decode(unitIdUnitParametersPtr_[id]->fileInfo.info.pictogram);
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
            
            item->appendRow(child);
            col++;
        }
        model->setItem(row, 0, item);
        row++;
    }

    tree_->setModel(model);
    tree_->expandAll();
}

// Units
bool MainWindow::SortUnitsBoost()
{
    if (!TopManager::SortUnitsBoost())
        return false;

    QPointF center = scene_->itemsBoundingRect().center();
    view_->centerOn(center);
    scene_->invalidate();
    
    return true;
}

bool MainWindow::SortUnitsRectangular(bool check)
{
    if (!TopManager::SortUnitsRectangular(check))
        return false;

    QPointF center = scene_->itemsBoundingRect().center();
    view_->centerOn(center);
    scene_->invalidate();
    
    return true;
}

// Files
void MainWindow::UpdateFileState(const QString& path, bool modified)
{
    QString title = path.isEmpty() ? "untitled" : path;
    if (modified) title += "*";
    setWindowTitle(title);
    path_ = path;
    modified_ = modified;
}

bool MainWindow::NewFile()
{
    if (!TopManager::NewFile())
        return false;

    return true;
}

bool MainWindow::SaveFile(const QString& path)
{
    log_table_model_->Clear();

    if (!TopManager::SaveFile(path))
        return false;

    AddRecent(path);
    UpdateFileState(path, false);

    return true;
}

bool MainWindow::SaveFolder(const QString& path)
{
    log_table_model_->Clear();

    if (!TopManager::SaveFolder(path))
        return false;

    return true;
}

bool MainWindow::OpenFile(const QString& path)
{
    scene_->clear();
    log_table_model_->Clear();

    UpdateFileState("", false);

    if (!TopManager::OpenFile(path))
        return false;

    AddRecent(path);
    UpdateFileState(path, false);

    return true;
}

bool MainWindow::OpenFolder(const QString& path)
{
    scene_->clear();
    log_table_model_->Clear();

    UpdateFileState("", false);

    if (!TopManager::OpenFolder(path))
        return false;

    UpdateFileState("", true);

    return true;
}

bool MainWindow::ImportXml(const QString& path)
{
    scene_->clear();
    log_table_model_->Clear();

    UpdateFileState("", false);

    if (!TopManager::ImportXml(path))
        return false;

    UpdateFileState("", true);

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
}

void MainWindow::selectionChanged()
{
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if (scene_->selectedItems().count() > 0)
    {
        CubesDiagram::DiagramItem* di = (CubesDiagram::DiagramItem*)(scene_->selectedItems()[0]);
        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
        if (propertiesItemsManager_->GetCurrentPropertiesId() == 0)
            propertiesItemsManager_->Select(di->propertiesId_);
    }
    else
    {
        propertiesItemsManager_->Select(CubesUnit::InvalidPropertiesId);
    }
}

// FileItemsManager
void MainWindow::FileNameChanged(CubesUnit::FileId fileId)
{
    TopManager::FileNameChanged(fileId);

    UpdateFileState(path_, true);
    scene_->invalidate();
}

void MainWindow::FileListChanged(const CubesUnit::FileIdNames& fileNames)
{
    TopManager::FileListChanged(fileNames);

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //! InformColorChanged
    //! 
    //! 
    //! 
    //for (auto& item : scene_->items())
    //{
    //    CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);
    //    auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
    //    pi->SetFileIdNames(fileNames);

    //    // Если item был добавлен, когда нет ни одного файла, pm->key будет не задан
    //    // После добавления, у них изменится имя файла и цвет
    //    // TODO: возможно изменится имя - но не должно
    //    const auto color = fileItemsManager_->GetFileColor(pi->GetFileId());
    //    di->InformColorChanged(color);
    //}

    UpdateFileState(path_, true);
    scene_->invalidate();
}

void MainWindow::FileIncludeNameChanged(CubesUnit::FileId fileId, CubesUnit::IncludeId includeId)
{
    TopManager::FileIncludeNameChanged(fileId, includeId);

    UpdateFileState(path_, true);
    scene_->invalidate();
}

void MainWindow::FileIncludesListChanged(CubesUnit::FileId fileId, const CubesUnit::IncludeIdNames& includeNames)
{
    TopManager::FileIncludesListChanged(fileId, includeNames);

    UpdateFileState(path_, true);
    scene_->invalidate();
}

void MainWindow::FileVariableNameChanged(CubesUnit::FileId fileId, CubesUnit::IncludeId includeId,
    const QString& variableName, const QString& oldVariableName)
{
    TopManager::FileVariableNameChanged(fileId, includeId, variableName, oldVariableName);

    UpdateFileState(path_, true);
}

void MainWindow::FileVariablesListChanged(CubesUnit::FileId fileId, CubesUnit::IncludeId includeId,
    const CubesUnit::VariableIdVariables& variables)
{
    TopManager::FileVariablesListChanged(fileId, includeId, variables);

    UpdateFileState(path_, true);
}

void MainWindow::FileColorChanged(CubesUnit::FileId fileId, const QColor& color)
{
    TopManager::FileColorChanged(fileId, color);

    UpdateFileState(path_, true);
    scene_->invalidate();
}

void MainWindow::FilePropertiesChanged()
{
    UpdateFileState(path_, true);
}

// PropertiesItemsManager
void MainWindow::PropertiesBasePropertiesChanged(CubesUnit::PropertiesId propertiesId, const QString& name,
    CubesUnit::FileId fileId, CubesUnit::IncludeId includeId)
{
    TopManager::PropertiesBasePropertiesChanged(propertiesId, name, fileId, includeId);

    const auto fileName = fileItemsManager_->GetFileName(fileId);
    QString includeName;
    if (!fileItemsManager_->GetFileIncludeName(fileId, includeId, includeName))
        includeName = "";
    const auto color = fileItemsManager_->GetFileColor(fileId);

    scene_->InformBasePropertiesChanged(propertiesId, name, fileName, includeName, color);

    UpdateFileState(path_, true);
    scene_->invalidate();
}

void MainWindow::PropertiesSelectedItemChanged(CubesUnit::PropertiesId propertiesId)
{
    TopManager::PropertiesSelectedItemChanged(propertiesId);

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

void MainWindow::PropertiesPositionChanged(CubesUnit::PropertiesId propertiesId, double posX, double posY, double posZ)
{
    TopManager::PropertiesPositionChanged(propertiesId, posX, posY, posZ);

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

void MainWindow::PropertiesError(CubesUnit::PropertiesId propertiesId, const QString& message)
{
    TopManager::PropertiesError(propertiesId, message);

    CubesLog::Message lm{};
    lm.type = CubesLog::MessageType::error;
    lm.source = CubesLog::SourceType::propertiesManager;
    lm.description = message;
    lm.tag = propertiesId;
    AddMessage(lm);
}

void MainWindow::PropertiesConnectionChanged(CubesUnit::PropertiesId propertiesId)
{
    TopManager::PropertiesConnectionChanged(propertiesId);

    scene_->invalidate();
    UpdateFileState(path_, true);
}

void MainWindow::PropertiesPropertiesChanged()
{
    TopManager::PropertiesPropertiesChanged();

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

    if (!NewFile())
    {
        QMessageBox::critical(this, "Error", QString::fromLocal8Bit("Ошибка создания файла!"));
        return;
    }

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
    dialog.setNameFilter("Parameters Archive Files (*.xmlx)");
    dialog.setAcceptMode(QFileDialog::AcceptOpen);

    QStringList selectedFileNames;
    if (dialog.exec())
        selectedFileNames = dialog.selectedFiles();

    if (selectedFileNames.size() == 0)
        return;

    if (!OpenFile(selectedFileNames[0]))
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

    if (!OpenFolder(folderPath))
    {
        QMessageBox::critical(this, "Error", QString::fromLocal8Bit("Ошибка открытия каталога!"));
        return;
    }
}

void MainWindow::OnImportXmlFileAction()
{
    QFileDialog dialog(this);
    dialog.setNameFilter("Settings XML Files (*.xml)");
    dialog.setAcceptMode(QFileDialog::AcceptOpen);

    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();

    if (fileNames.size() == 0)
        return;

    if (!ImportXml(fileNames[0]))
    {
        QMessageBox::critical(this, "Error", QString::fromLocal8Bit("Ошибка импорта файла!"));
        return;
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
        dialog.setNameFilters({ "Parameters Archive Files (*.xmlx)" });
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        dialog.setDefaultSuffix("xmlx");

        QStringList selectedFileNames;
        if (dialog.exec())
            selectedFileNames = dialog.selectedFiles();
        if (selectedFileNames.size() <= 0)
            return;

        selectedFileName = selectedFileNames[0];
    }

    if (!SaveFile(selectedFileName))
    {
        QMessageBox::critical(this, "Error", QString::fromLocal8Bit("Ошибка сохранения файла!"));
        return;
    }
}

void MainWindow::OnSaveAsFileAction()
{
    QFileDialog dialog(this);
    dialog.setNameFilters({ "Parameters Archive Files (*.xmlx)" });
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("xmlx");

    QStringList selectedFileNames;
    if (dialog.exec())
        selectedFileNames = dialog.selectedFiles();
    if (selectedFileNames.size() <= 0)
        return;

    QString selectedFileName = selectedFileNames[0];

    if (!SaveFile(selectedFileName))
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

    if (!SaveFolder(folderPath))
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
    SortUnitsBoost();
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
    if (!OpenFile(act->text()))
    {
        QMessageBox::critical(this, "Error", QString::fromLocal8Bit("Ошибка открытия файла!"));
        RemoveRecent(act->text());
        return;
    }
}

void MainWindow::OnTestAction()
{
    log_table_model_->Clear();
    Test();
}

// Лог
void MainWindow::OnErrorButtonClicked(bool checked)
{
    if (checked)
        sort_filter_model_->AddToFilter(CubesLog::MessageType::error);
    else
        sort_filter_model_->RemoveFromFilter(CubesLog::MessageType::error);

    table_view_log_->resizeColumnsToContents();
    table_view_log_->horizontalHeader()->setStretchLastSection(false);
    table_view_log_->horizontalHeader()->setStretchLastSection(true);
    table_view_log_->resizeRowsToContents();
}

void MainWindow::OnWarningButtonClicked(bool checked)
{
    if (checked)
        sort_filter_model_->AddToFilter(CubesLog::MessageType::warning);
    else
        sort_filter_model_->RemoveFromFilter(CubesLog::MessageType::warning);

    table_view_log_->resizeColumnsToContents();
    table_view_log_->horizontalHeader()->setStretchLastSection(false);
    table_view_log_->horizontalHeader()->setStretchLastSection(true);
    table_view_log_->resizeRowsToContents();
}

void MainWindow::OnInformationButtonClicked(bool checked)
{
    if (checked)
        sort_filter_model_->AddToFilter(CubesLog::MessageType::information);
    else
        sort_filter_model_->RemoveFromFilter(CubesLog::MessageType::information);

    table_view_log_->resizeColumnsToContents();
    table_view_log_->horizontalHeader()->setStretchLastSection(false);
    table_view_log_->horizontalHeader()->setStretchLastSection(true);
    table_view_log_->resizeRowsToContents();
}

void MainWindow::OnDoubleClicked(const QModelIndex& index)
{
    CubesLog::Message m{};
    const auto mapped = sort_filter_model_->mapToSource(index);
    if (log_table_model_->GetMessage(mapped.row(), m))
    {
        // TODO: Properties Manager и т.п. в enum
        if (m.source == CubesLog::SourceType::propertiesManager ||
            m.source == CubesLog::SourceType::propertiesAnalysis ||
            m.source == CubesLog::SourceType::propertiesItem)
        {
            CubesUnit::PropertiesId propertiesId = m.tag;
            propertiesItemsManager_->Select(propertiesId);
        }
    }
}
