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
#include "parameters.h"
//#include "parameters/yaml_parser.h"
#include "diagram/diagram_view.h"
#include "diagram/diagram_scene.h"
#include "diagram/diagram_item.h"
#include "file_item/file_item.h"
#include "file_item/file_items_manager.h"
#include "log_table/log_table_model.h"
#include "log_table/sort_filter_model.h"
#include "parameters_compiler/base64.h"
#include "properties_item/properties_item_types.h"
#include "properties_item/properties_item.h"
#include "properties_item/properties_items_manager.h"
#include "qttreepropertybrowser.h" // потом убрать
#include "tree_item_model.h"
#include "xml/xml_parser.h"
#include "xml/xml_writer.h"
#include "graph.h"
#include "array_window.h"

ArrayWindow::ArrayWindow(QWidget *parent)
    : QMainWindow(parent), TopManager(true)
{
    modified_ = false;
    uniqueNumber_ = 0;

    setWindowIcon(QIcon(":/images/cubes.png"));

    //fileItemsManager_ = new CubesFile::FileItemsManager(this);
    //connect(fileItemsManager_, &CubesFile::FileItemsManager::FileNameChanged, this, &ArrayWindow::FileNameChanged);
    //connect(fileItemsManager_, &CubesFile::FileItemsManager::FilesListChanged, this, &ArrayWindow::FileListChanged);
    //connect(fileItemsManager_, &CubesFile::FileItemsManager::IncludeNameChanged, this, &ArrayWindow::FileIncludeNameChanged);
    //connect(fileItemsManager_, &CubesFile::FileItemsManager::IncludesListChanged, this, &ArrayWindow::FileIncludesListChanged);
    //connect(fileItemsManager_, &CubesFile::FileItemsManager::VariableNameChanged, this, &ArrayWindow::FileVariableNameChanged);
    //connect(fileItemsManager_, &CubesFile::FileItemsManager::VariablesListChanged, this, &ArrayWindow::FileVariablesListChanged);
    
    //propertiesItemsManager_ = new CubesProperties::PropertiesItemsManager(this, nullptr, true);
    //connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::BasePropertiesChanged, this, &ArrayWindow::PropertiesBasePropertiesChanged);
    //connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::SelectedItemChanged, this, &ArrayWindow::PropertiesSelectedItemChanged);
    //connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::PositionChanged, this, &ArrayWindow::PropertiesPositionChanged);
    //connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::Error, this, &ArrayWindow::PropertiesError);
    
    
    //connect(propertiesItemsManager_, &CubesProperties::PropertiesItemsManager::PropertiesChanged, this, &ArrayWindow::PropertiesPropertiesChanged);
    //connect(properties_items_manager_, &Properties::properties_items_manager::FileNameChanged, this, &ArrayWindow::propertiesFileNameChanged);
    //connect(properties_items_manager_, &Properties::properties_items_manager::FilesListChanged, this, &ArrayWindow::fileListChanged);
    //connect(properties_items_manager_, &Properties::properties_items_manager::IncludeNameChanged, this, &ArrayWindow::fileIncludeNameChanged);
    //connect(properties_items_manager_, &Properties::properties_items_manager::IncludesListChanged, this, &ArrayWindow::fileIncludesListChanged);

    CreateUi();

    //fileItemsManager_->Create(QString::fromLocal8Bit("config.xml"), QString::fromLocal8Bit("АРМ"));
}

ArrayWindow::~ArrayWindow()
{
}

// ITopManager
bool ArrayWindow::GetUnitsInFileList(const CubesUnitTypes::FileId& fileId, QStringList& unitNames)
{
    return propertiesItemsManager_->GetUnitsInFileList(fileId, unitNames);
}

bool ArrayWindow::GetUnitsInFileIncludeList(const CubesUnitTypes::FileId& fileId,
    const CubesUnitTypes::IncludeId includeId, QStringList& unitNames)
{
    return propertiesItemsManager_->GetUnitsInFileIncludeList(fileId, includeId, unitNames);
}

bool ArrayWindow::GetUnitParameters(const QString& unitId, CubesUnitTypes::UnitParameters& unitParameters)
{
    if (unitId.isEmpty() && !unitParameters_.empty())
        unitParameters = *unitParameters_.begin();
    else
        unitParameters = unitParameters_[unitId];

    return true;
}

bool ArrayWindow::GetFileIncludeList(const CubesUnitTypes::FileId& fileId, CubesUnitTypes::IncludeIdNames& includeNames)
{
    return false;
}

bool ArrayWindow::GetFileIncludeVariableList(const CubesUnitTypes::FileId& fileId,
    const CubesUnitTypes::IncludeId includeId, CubesUnitTypes::VariableIdVariables& variables)
{
    return false;
}


//void ArrayWindow::GetUnitParameters(const QString& unitId, CubesUnitTypes::UnitParameters& unitParameters)
//{
//    if (unitId.isEmpty() && !unitParameters_.empty())
//        unitParameters = *unitParameters_.begin();
//    else
//        unitParameters = unitParameters_[unitId];
//}
//
//void ArrayWindow::GetFileIncludeList(const CubesUnitTypes::FileId& fileId, CubesUnitTypes::IncludeIdNames& includeNames)
//{
//    //includeNames = fileItemsManager_->GetFileIncludeNames(fileName, true);
//}
//
//void ArrayWindow::GetFileIncludeVariableList(const CubesUnitTypes::FileId& fileId,
//    const CubesUnitTypes::IncludeId includeId, CubesUnitTypes::VariableIdVariables& variables)
//{
//    //variables = fileItemsManager_->GetFileIncludeVariables(fileName, includeName);
//}

bool ArrayWindow::CreatePropetiesItem(const QString& unitId, uint32_t& propertiesId)
{
    //instanceName = name + QString("_#%1").arg(unique_number_++);
    //uint32_t propertiesId{ 0 };
    propertiesItemsManager_->Create(unitId, propertiesId);
    //auto pi = propertiesItemsManager_->GetItem(propertiesId);
    //pi->SetFileNames(GetFileNames());
    //pi->SetFileName(GetCurrentFileName());
    //pi->SetIncludeNames(GetCurrentFileIncludeNames());
    //pi->SetIncludeName("<not selected>");
    //pi->SetName(GetNewUnitName(pi->GetName()));
    //properties_items_manager_->Select(propertiesId);
    return true;
}

bool ArrayWindow::GetPropetiesForDrawing(const CubesUnitTypes::PropertiesId propertiesId, CubesTop::PropertiesForDrawing& pfd)
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
    pfd.color = Qt::white;

    return true;
}

bool ArrayWindow::GetPropetiesUnitParameters(const CubesUnitTypes::PropertiesId propertiesId, CubesUnitTypes::UnitParameters& unitParameters)
{
    return propertiesItemsManager_->GetUnitParameters(propertiesId, unitParameters);
}

bool ArrayWindow::GetPropetiesUnitId(const CubesUnitTypes::PropertiesId propertiesId, QString& unitId)
{
    return propertiesItemsManager_->GetUnitId(propertiesId, unitId);
}

bool ArrayWindow::CreateDiagramItem(CubesUnitTypes::PropertiesId propertiesId)
{
    //TopManager::CreateDiagramItem(propertiesId);

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

    pfd.color = Qt::white;

    const auto pos = pi->GetPosition();
    const auto z = pi->GetZOrder();

    auto di = new CubesDiagram::DiagramItem(propertiesId, pfd.pixmap, pfd.name, pfd.fileName, pfd.includeName, pfd.color);
    di->setX(pos.x());
    di->setY(pos.y());
    di->setZValue(z);
    scene_->addItem(di);

    //scene_->clearSelection();
    //propertiesItemsManager_->Select(0);
    //DiagramAfterItemCreated(di);
    scene_->clearSelection();
    di->setPos(pos);
    scene_->InformItemPositionChanged(di);

    di->setSelected(true);
    //???????????
    //scene_->InformItemCreated(di);





    //CubesDiagram::DiagramItem* di = new CubesDiagram::DiagramItem(propertiesId, pfd.pixmap, pfd.name, pfd.fileName, pfd.includeName, pfd.color);

    //scene_->InformItemCreated(di);

    ////QPoint position = mapToScene(event->pos() - QPoint(24, 24)).toPoint();

    ////int gridSize = 20;
    ////qreal xV = round(position.x() / gridSize) * gridSize;
    ////qreal yV = round(position.y() / gridSize) * gridSize;
    ////position = QPoint(xV, yV);

    //scene_->addItem(di);
    //scene_->clearSelection();
    //di->setPos(pos);
    //scene_->InformItemPositionChanged(di);

    //di->setSelected(true);

    return true;
}

bool ArrayWindow::EnshureVisible(CubesUnitTypes::PropertiesId propertiesId)
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

void ArrayWindow::SetItemModel(parameters::file_info afi, CubesUnitTypes::ParameterModel pm,
    parameters::restrictions_info ri, QSharedPointer<CubesProperties::PropertiesItem> pi)
{
    pm_ = pm;
    pi_ = pi;
    ri_ = ri;

    unitParameters_[QString::fromStdString(afi.info.id)] = { afi, {} };


//            auto& up = unitParameters_[QString::fromStdString(fi.info.id)];
//            up.fileInfo = fi;
//            up.platforms.insert(QFileInfo(platformDir).baseName());
    CubesDiagram::DiagramItem* di = nullptr;

    for (auto& item : pm.parameters)
    {
        //auto& up = unitParameters_[QString::fromStdString(afi.info.id)];
        //up.fileInfo = afi;

        auto rename = [](QList<CubesUnitTypes::ParameterModel>& parameters, const CubesUnitTypes::ParameterModelId to_remove, auto&& rename) -> void {
            for (auto& parameter : parameters)
            {
                parameter.id = parameter.id.mid(to_remove.size());
                //parameter.id = "PARAMETERS/" + parameter.id.mid(to_remove.length() + 1);
                rename(parameter.parameters, to_remove, rename);
            }
        };

        //auto rename = [](QList<CubesUnitTypes::ParameterModel>& parameters, QString to_remove, auto&& rename) -> void {
        //    for (auto& parameter : parameters)
        //    {
        //        parameter.id = parameter.id.mid(to_remove.length() + 1);
        //        //parameter.id = "PARAMETERS/" + parameter.id.mid(to_remove.length() + 1);
        //        rename(parameter.parameters, to_remove, rename);
        //    }
        //};

        //auto id_renamer = [](QList<CubesUnitTypes::ParameterModel>& parameters, QString to_remove)->void {
        //    auto id_renamer_impl = [](QList<CubesUnitTypes::ParameterModel>& parameters, QString to_remove, auto& id_renamer_ref)->void {
        //        for (auto& parameter : parameters)
        //        {
        //            parameter.id = "PARAMETERS/" + parameter.id.mid(to_remove.length() + 1);
        //            id_renamer_ref(parameter.parameters, to_remove, id_renamer_ref);
        //        }
        //    };
        //    return id_renamer_impl(parameters, to_remove, id_renamer_impl);
        //};

        //id_renamer(item.parameters, item.id);

        rename(item.parameters, item.id, rename);

        for (auto& group : item.parameters)
        {
            if (group.id == ids_.parameters)
            {
                for (auto& parameter : group.parameters)
                {
                    parameter.parameterInfoId.type = QString::fromStdString(parameters::helper::type::main_type);

                    auto& pi = *parameters::helper::parameter::get_parameter_info(afi,
                        parameters::helper::type::main_type, parameter.parameterInfoId.name.toStdString());

                    bool isArray = parameters::helper::common::get_is_array_type(pi.type);
                    auto itemType = parameters::helper::common::get_item_type(pi.type);
                    bool isInner = parameters::helper::common::get_is_inner_type(itemType);

                    if (isArray && isInner)
                    {
                        for (auto& arrayParameter : parameter.parameters)
                            arrayParameter.parameterInfoId.type = QString::fromStdString(parameters::helper::type::main_type);
                    }
                }
            }
        }

        CubesUnitTypes::ParametersModel m{};
        m.parameters = item.parameters;

        uint32_t propertiesId{ 0 };
        propertiesItemsManager_->Create(QString::fromStdString(afi.info.id), m, propertiesId);
        auto pi = propertiesItemsManager_->GetItem(propertiesId);

        CubesTop::PropertiesForDrawing pfd{};
        if (!GetPropetiesForDrawing(propertiesId, pfd))
        {
            qDebug() << "ERROR GetPropeties: " << propertiesId;
        }

        auto posX = pi->GetParameterModel(ids_.editor + ids_.positionX)->value.toDouble();
        auto posY = pi->GetParameterModel(ids_.editor + ids_.positionY)->value.toDouble();
        auto posZ = pi->GetParameterModel(ids_.editor + ids_.positionZ)->value.toDouble();

        di = new CubesDiagram::DiagramItem(propertiesId, pfd.pixmap, pfd.name, pfd.fileName, pfd.includeName, pfd.color);
        di->setX(posX);
        di->setY(posY);
        di->setZValue(posZ);
        scene_->addItem(di);
    }

    if (di != nullptr)
    {
        scene_->clearSelection();
        propertiesItemsManager_->Select(0);
        DiagramAfterItemCreated(di);
    }

    SortUnitsRectangular(true);
}

bool ArrayWindow::GetAnalysisFiles(QVector<CubesAnalysis::File>& files)
{
    return false;
}

bool ArrayWindow::GetAnalysisProperties(QVector<CubesAnalysis::Properties>& properties)
{
    return false;
}

bool ArrayWindow::GetUnitsConnections(QMap<QString, QStringList>& connections)
{
    return propertiesItemsManager_->GetUnitsConnections(connections);
}

bool ArrayWindow::GetDependsConnections(QMap<QString, QStringList>& connections)
{
    return propertiesItemsManager_->GetDependsConnections(connections);
}

void ArrayWindow::closeEvent(QCloseEvent* event)
{
    //std::string min_count;
    //std::string max_count;
    //std::vector<std::string> set_count;

    const auto ids = propertiesItemsManager_->GetPropertyIds();


    if (!ri_.set_count.empty())
    {
        bool found = false;
        for (const auto& s : ri_.set_count)
        {
            const auto i = atoi(s.c_str());
            if (i == 0 && std::to_string(i) != "0") // check atoi is valid
                continue;
            if (i == ids.size())
            {
                found = true;
                break;
            }
        }

        QString ss;
        for (int i = 0; i < ri_.set_count.size(); ++i)
        {
            ss += QString::fromStdString(ri_.set_count[i]);
            if (i < ri_.set_count.size() - 1)
                ss += ", ";
        }

        if (!found)
        {
            QMessageBox::critical(this, "Validate error",
                QString::fromLocal8Bit("Количество элементов недопустимо\nДопустимо: %1").arg(ss));
            event->ignore();
            return;
        }
    }
    
    if (!ri_.min_count.empty())
    {
        const auto i = atoi(ri_.min_count.c_str());
        if (!(i == 0 && std::to_string(i) != "0")) // check atoi is valid
        {
            if (i > ids.size())
            {
                const auto ss = QString::fromStdString(ri_.min_count);
                QMessageBox::critical(this, "Validate error",
                    QString::fromLocal8Bit("Количество элементов недопустимо\nДопустимо не менее: %1").arg(ss));
                event->ignore();
                return;
            }
        }
    }

    if (!ri_.max_count.empty())
    {
        const auto i = atoi(ri_.max_count.c_str());
        if (!(i == 0 && std::to_string(i) != "0")) // check atoi is valid
        {
            if (i < ids.size())
            {
                const auto ss = QString::fromStdString(ri_.max_count);
                QMessageBox::critical(this, "Validate error",
                    QString::fromLocal8Bit("Количество элементов недопустимо\nДопустимо не более: %1").arg(ss));
                event->ignore();
                return;
            }
        }
    }

    pm_.parameters.clear();

    int item_index = 0;

    //const auto ids = propertiesItemsManager_->GetPropertyIds();
    for (auto& id : ids)
    {
        auto item = propertiesItemsManager_->GetItem(id);
        auto pm = item->GetParametersModel();

        const auto up = item->GetUnitParameters();
        const auto type = QString::fromStdString(up.fileInfo.info.id);
        for (auto& group : pm.parameters)
        {
            if (group.id == ids_.parameters)
            {
                for (auto& parameter : group.parameters)
                {
                    parameter.parameterInfoId.type = type;

                    auto& pi = *parameters::helper::parameter::get_parameter_info(up.fileInfo,
                        parameters::helper::type::main_type, parameter.parameterInfoId.name.toStdString());
                    

                    bool isArray = parameters::helper::common::get_is_array_type(pi.type);
                    auto itemType = parameters::helper::common::get_item_type(pi.type);
                    bool isInner = parameters::helper::common::get_is_inner_type(itemType);

                    if (isArray && isInner)
                    {
                        for (auto& arrayParameter : parameter.parameters)
                            arrayParameter.parameterInfoId.type = type;
                    }
                }
            }
        }

        auto rename = [](QList<CubesUnitTypes::ParameterModel>& parameters, const CubesUnitTypes::ParameterModelId to_add, auto&& rename) -> void {
            for (auto& parameter : parameters)
            {
                parameter.id = to_add + parameter.id;
                rename(parameter.parameters, to_add, rename);
            }
        };

        rename(pm.parameters, pm_.id + ids_.Item(item_index), rename); // !!!!!!!!!!!! item_index

        CubesUnitTypes::ParameterModel itemPm{};
        itemPm.id = pm_.id + ids_.Item(item_index);
        itemPm.name = QString::fromLocal8Bit("Элемент %1").arg(item_index);
        itemPm.value = QVariant();
        itemPm.editorSettings.type = CubesUnitTypes::EditorType::None;
        itemPm.parameters = pm.parameters;

        pm_.parameters.push_back(itemPm);

        ++item_index;
    }


    emit BeforeClose(true, pm_, pi_);
}

// UI
void ArrayWindow::CreateUi()
{
    resize(1000, 600);

    CreateMenu();

    QWidget* mainWidget = CreateMainWidget();
    setCentralWidget(mainWidget);
}

void ArrayWindow::CreateMenu()
{/*
    QAction* newAct = new QAction(QString::fromLocal8Bit("Создать"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(QString::fromLocal8Bit("Создать новый файл"));
    connect(newAct, &QAction::triggered, this, &ArrayWindow::OnNewFileAction);

    QAction* openAct = new QAction(QString::fromLocal8Bit("Открыть"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(QString::fromLocal8Bit("Открыть файл"));
    connect(openAct, &QAction::triggered, this, &ArrayWindow::OnOpenFileAction);

    QAction* importXmlAct = new QAction(QString::fromLocal8Bit("Импорт xml"), this);
    importXmlAct->setShortcut(QKeySequence("Ctrl+I"));
    importXmlAct->setStatusTip(QString::fromLocal8Bit("Импортировать xml файл"));
    connect(importXmlAct, &QAction::triggered, this, &ArrayWindow::OnImportXmlFileAction);

    QAction* saveAct = new QAction(QString::fromLocal8Bit("Сохранить"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(QString::fromLocal8Bit("Сохранить файл"));
    connect(saveAct, &QAction::triggered, this, &ArrayWindow::OnSaveFileAction);

    QAction* saveAsAct = new QAction(QString::fromLocal8Bit("Сохранить как..."), this);
    //saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setShortcut(QKeySequence("Ctrl+Shift+S"));
    saveAsAct->setStatusTip(QString::fromLocal8Bit("Сохранить файл как..."));
    connect(saveAsAct, &QAction::triggered, this, &ArrayWindow::OnSaveAsFileAction);

    QAction* quitAct = new QAction(QString::fromLocal8Bit("Выйти"), this);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(QString::fromLocal8Bit("Выйти из приложения"));
    connect(quitAct, &QAction::triggered, this, &ArrayWindow::OnQuitAction);

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
    */
    QAction* sortBoostAct = new QAction(QString::fromLocal8Bit("Сортировать (boost)"), this);
    sortBoostAct->setStatusTip(QString::fromLocal8Bit("Автоматическая сортировка"));
    connect(sortBoostAct, &QAction::triggered, this, &ArrayWindow::OnSortBoostAction);

    QAction* sortRectAct = new QAction(QString::fromLocal8Bit("Расставить по сетке"), this);
    sortRectAct->setStatusTip(QString::fromLocal8Bit("Автоматическая сортировка"));
    connect(sortRectAct, &QAction::triggered, this, &ArrayWindow::OnSortRectAction);

    QMenu* editMenu = menuBar()->addMenu(QString::fromLocal8Bit("Правка"));
    editMenu->addAction(sortBoostAct);
    editMenu->addAction(sortRectAct);
}

QWidget* ArrayWindow::CreateMainWidget()
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

    //qDebug() << connect(tabWidget_, &QTabWidget::currentChanged, this, &ArrayWindow::on_Tab_currentChanged);

    //QSplitter* splitterTreeTab = new QSplitter(Qt::Horizontal);
    //splitterTreeTab->addWidget(tree_);
    //splitterTreeTab->addWidget(view_);
    //splitterTreeTab->setStretchFactor(0, 0);
    //splitterTreeTab->setStretchFactor(1, 1);

    //QWidget* logWidget = CreateLogWidget();
    //QSplitter* splitterTreeTabLog = new QSplitter(Qt::Vertical);
    //splitterTreeTabLog->addWidget(splitterTreeTab);
    //splitterTreeTabLog->addWidget(logWidget);
    //splitterTreeTabLog->setStretchFactor(0, 1);
    //splitterTreeTabLog->setStretchFactor(1, 0);

    QWidget* propertiesPanelWidget = CreatePropertiesPanelWidget();
    propertiesPanelWidget->setMinimumWidth(400);
    QSplitter* splitterMain = new QSplitter(Qt::Horizontal);
    splitterMain->addWidget(view_);
    //splitterMain->addWidget(splitterTreeTabLog);
    splitterMain->addWidget(propertiesPanelWidget);
    splitterMain->setStretchFactor(0, 1);
    splitterMain->setStretchFactor(1, 0);

    QWidget* mainWidget = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(splitterMain);
    mainWidget->setLayout(mainLayout);
    return mainWidget;
}

QWidget* ArrayWindow::CreateLogWidget()
{
    //table_view_log_ = new QTableView;

    //log_table_model_ = new CubeLog::LogTableModel;
    //sort_filter_model_ = new CubeLog::SortFilterModel;
    //sort_filter_model_->setSourceModel(log_table_model_);
    //sort_filter_model_->SetFilter({ CubeLog::MessageType::error, CubeLog::MessageType::warning, CubeLog::MessageType::information });

    //table_view_log_->setModel(sort_filter_model_);
    //table_view_log_->setSortingEnabled(true);
    //table_view_log_->verticalHeader()->hide();
    //table_view_log_->horizontalHeader()->setHighlightSections(false);
    ////table_view_log_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //table_view_log_->horizontalHeader()->setStretchLastSection(true);
    //table_view_log_->setSelectionBehavior(QAbstractItemView::SelectRows);
    //table_view_log_->setSelectionMode(QAbstractItemView::SingleSelection);
    ////table_view_log_->sortByColumn(0, Qt::AscendingOrder);
    //table_view_log_->resizeColumnsToContents();

    //QToolButton* buttonError = new QToolButton;
    //buttonError->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    //buttonError->setCheckable(true);
    //buttonError->setIcon(QIcon(":/images/error.png"));
    //buttonError->setText(QString::fromLocal8Bit("Ошибка"));
    //buttonError->setChecked(true);
    //qDebug() << connect(buttonError, &QToolButton::clicked, this, &ArrayWindow::OnErrorButtonClicked);
    //QToolButton* buttonWarning = new QToolButton;
    //buttonWarning->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    //buttonWarning->setCheckable(true);
    //buttonWarning->setIcon(QIcon(":/images/warning.png"));
    //buttonWarning->setText(QString::fromLocal8Bit("Предупреждение"));
    //buttonWarning->setChecked(true);
    //qDebug() << connect(buttonWarning, &QToolButton::clicked, this, &ArrayWindow::OnWarningButtonClicked);
    //QToolButton* buttonInformation = new QToolButton;
    //buttonInformation->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    //buttonInformation->setCheckable(true);
    //buttonInformation->setIcon(QIcon(":/images/information.png"));
    //buttonInformation->setText(QString::fromLocal8Bit("Информация"));
    //buttonInformation->setChecked(true);
    //qDebug() << connect(buttonInformation, &QToolButton::clicked, this, &ArrayWindow::OnInformationButtonClicked);

    //QHBoxLayout* buttonsLayout = new QHBoxLayout;
    //buttonsLayout->addWidget(buttonError);
    //buttonsLayout->addWidget(buttonWarning);
    //buttonsLayout->addWidget(buttonInformation);
    //buttonsLayout->addStretch();
    //buttonsLayout->setContentsMargins(0, 0, 0, 0);

    //QWidget* buttonsWidget = new QWidget;
    //buttonsWidget->setLayout(buttonsLayout);

    //QWidget* mainWidget = new QWidget;
    //QVBoxLayout* mainLayout = new QVBoxLayout;
    //mainLayout->addWidget(buttonsWidget);
    //mainLayout->addWidget(table_view_log_);
    //mainLayout->setContentsMargins(0, 0, 0, 0);
    //mainWidget->setLayout(mainLayout);
    //return mainWidget;
    return nullptr;
}

void ArrayWindow::CreateScene()
{
    scene_ = new CubesDiagram::DiagramScene(this);
    scene_->setSceneRect(-10000, -10000, 20032, 20032);

    qDebug() << connect(scene_, &CubesDiagram::DiagramScene::ItemPositionChanged, this, &ArrayWindow::DiagramItemPositionChanged);
    qDebug() << connect(scene_, &CubesDiagram::DiagramScene::AfterItemCreated, this, &ArrayWindow::DiagramAfterItemCreated);
    qDebug() << connect(scene_, &CubesDiagram::DiagramScene::BeforeItemDeleted, this, &ArrayWindow::DiagramBeforeItemDeleted);
    //qDebug() << connect(scene_, &CubeDiagram::DiagramScene::ItemNameChanged, this, &ArrayWindow::DiagramItemNameChanged);
    //qDebug() << connect(scene_, &CubeDiagram::DiagramScene::ItemFileChanged, this, &ArrayWindow::DiagramItemFileChanged);
    //qDebug() << connect(scene_, &CubeDiagram::DiagramScene::ItemGroupChanged, this, &ArrayWindow::DiagramItemGroupChanged);
    qDebug() << connect(scene_, &CubesDiagram::DiagramScene::selectionChanged, this, &ArrayWindow::selectionChanged);
}

void ArrayWindow::CreateView()
{
    view_ = new CubesDiagram::DiagramView(this, scene_);
    view_->setDragMode(QGraphicsView::RubberBandDrag);
}

void ArrayWindow::CreateTreeView()
{
    //tree_ = new QTreeView();
    //tree_->setDragEnabled(true);
    //tree_->setHeaderHidden(true);
    //tree_->setMinimumWidth(300);
}

QWidget* ArrayWindow::CreatePropertiesPanelWidget()
{
    QWidget* propertiesPanelWidget = new QWidget;

    //QWidget* filesWidget = fileItemsManager_->GetWidget();
    //QWidget* propertiesWidget = propertiesItemsManager_->GetWidget();
    ////QWidget* hintWidget = CreateHintWidget();

    //QSplitter* tabVSplitter = new QSplitter(Qt::Vertical);
    //tabVSplitter->addWidget(filesWidget);
    //tabVSplitter->addWidget(propertiesWidget);
    ////tabVSplitter->addWidget(hintWidget);
    //tabVSplitter->setStretchFactor(0, 0);
    //tabVSplitter->setStretchFactor(1, 1);
    ////tabVSplitter->setStretchFactor(2, 0);

    QWidget* propertiesWidget = propertiesItemsManager_->GetWidget();

    QVBoxLayout* propertiesPaneLayout = new QVBoxLayout;
    propertiesPaneLayout->addWidget(propertiesWidget);
    //propertiesPaneLayout->addWidget(tabVSplitter);
    propertiesPaneLayout->setContentsMargins(0, 0, 0, 0);

    propertiesPanelWidget->setLayout(propertiesPaneLayout);

    return propertiesPanelWidget;
}

//QWidget* ArrayWindow::CreateHintWidget()
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

void ArrayWindow::FillTreeView()
{
    //TreeItemModel* model = new TreeItemModel();

    //QMap<QString, QSet<QString>> categoriesMap;
    //for (const auto& up : unitParameters_)
    //{
    //    QString category = "Default";
    //    if (up.fileInfo.info.category != "")
    //        category = QString::fromStdString(up.fileInfo.info.category).toLower();
    //    categoriesMap[category].insert(QString::fromStdString(up.fileInfo.info.id));
    //}

    //int row = 0;
    //for (const auto& cat : categoriesMap.toStdMap())
    //{
    //    QStandardItem* item = new QStandardItem(cat.first);
    //    item->setEditable(false);
    //    item->setDragEnabled(false);
    //    model->setItem(row, 0, item);
    //    int col = 0;
    //    for (const auto& id : cat.second)
    //    {
    //        QStandardItem* child = new QStandardItem(id);
    //        child->setEditable(false);

    //        QPixmap px;
    //        bool loaded = false;
    //        if (unitParameters_[id].fileInfo.info.pictogram != "")
    //        {
    //            std::string s = base64_decode(unitParameters_[id].fileInfo.info.pictogram);
    //            QByteArray ba(s.c_str(), static_cast<int>(s.size()));
    //            try
    //            {
    //                loaded = px.loadFromData(ba);
    //            }
    //            catch (...)
    //            {
    //                loaded = false;
    //            }
    //        }
    //        if (!loaded)
    //            px.load(":/images/ice.png");
    //        child->setIcon(QIcon(px));
    //        
    //        //child->setData(QPoint(row, col), Qt::UserRole + 1);
    //        item->appendRow(child);
    //        col++;
    //    }
    //    model->setItem(row, 0, item);
    //    row++;
    //}
    ////{
    ////    QStandardItem* item = new QStandardItem(QString::fromLocal8Bit("Служебные"));
    ////    item->setEditable(false);
    ////    item->setDragEnabled(false);
    ////    model->setItem(row, 0, item);
    ////    QStandardItem* child = new QStandardItem(QString::fromLocal8Bit("Группа"));
    ////    child->setEditable(false);

    ////    QPixmap px;
    ////    px.load(":/images/module.png");
    ////    child->setIcon(QIcon(px));
    ////    item->appendRow(child);
    ////}

    //tree_->setModel(model);
    //tree_->expandAll();

    ////tree_item_model* model = new tree_item_model();
    //////QStandardItemModel* model = new QStandardItemModel( 5, 1 );
    ////for (int r = 0; r < 5; r++)
    ////    for (int c = 0; c < 1; c++)
    ////    {
    ////        QStandardItem* item = new QStandardItem(QString("Category %0").arg(r));
    ////        item->setEditable(false);
    ////        item->setDragEnabled(false);
    ////        if (c == 0)
    ////            for (int i = 0; i < 3; i++)
    ////            {
    ////                QStandardItem* child = new QStandardItem(QString("Item %0").arg(i));
    ////                child->setEditable(false);

    ////                QFile f("c:/QtProjects/cubes/resource/plus.png");
    ////                if (!f.open(QIODevice::ReadOnly))
    ////                    return; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ////                QByteArray ba = f.readAll();
    ////                QPixmap px;
    ////                bool isLoaded = px.loadFromData(ba, "PNG", Qt::AutoColor);
    ////                QIcon ico(px);
    ////                child->setIcon(ico);
    ////                //child->setIcon(QIcon("c:/QtProjects/cubes/resource/plus.png"));

    ////                child->setData(QPoint(r, i), Qt::UserRole + 1);

    ////                item->appendRow(child);
    ////            }

    ////        model->setItem(r, c, item);
    ////    }

    ////tree_->setModel(model);
}

void ArrayWindow::FillParametersInfo()
{
    //QString directoryPath(QCoreApplication::applicationDirPath() + "/doc/all_units_solid");
    //QStringList platformDirs;
    //QDirIterator directories(directoryPath, QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);
    //while (directories.hasNext()) {
    //    directories.next();
    //    platformDirs << directories.filePath();
    //}

    //for (const auto& platformDir : platformDirs)
    //{
    //    QString ymlPath = QDir(platformDir).filePath("yml");
    //    if (QFileInfo(ymlPath).exists() && QFileInfo(ymlPath).isDir())
    //    {
    //        QStringList propFiles = QDir(ymlPath).entryList(QStringList() << "*.yml" << "*.json", QDir::Files);
    //        foreach(QString filename, propFiles)
    //        {
    //            QString fullPath = QDir(ymlPath).filePath(filename);
    //            parameters_compiler::file_info fi{};
    //            if (!yaml::parser::parse(fullPath.toStdString(), fi))
    //            {
    //                CubeLog::LogMessage m{};
    //                m.type = CubeLog::MessageType::error;
    //                m.source = filename;
    //                m.description = QString::fromLocal8Bit("Файл параметров %1 не разобран. Параметры не добавлены.").arg(fullPath);
    //                //log_table_model_->AddMessage(m);
    //            }

    //            // Добавляем параметр - зависимости, его нет в параметрах юнитов, но он может присутствовать в xml файле
    //            // Принцип обработки такой же как и у остальных параметров
    //            if (fi.info.id != "group" && fi.info.id != "group_mock")
    //            {
    //                parameters_compiler::parameter_info pi{};
    //                pi.type = "array<string>";
    //                pi.name = "DEPENDS";
    //                pi.display_name = QString::fromLocal8Bit("Зависимости").toStdString();
    //                pi.description = QString::fromLocal8Bit("Зависимости юнита от других юнитов").toStdString();
    //                pi.required = false;
    //                pi.default_ = QString::fromLocal8Bit("не задано").toStdString();
    //                fi.parameters.push_back(std::move(pi));
    //            }

    //            auto& up = unitParameters_[QString::fromStdString(fi.info.id)];
    //            up.fileInfo = fi;
    //            up.platforms.insert(QFileInfo(platformDir).baseName());
    //        }
    //    }
    //}
}

// Units
bool ArrayWindow::AddMainFile(CubesXml::File& file)
{
    //// Не очищаем, вдруг там уже что-то поменяно
    ////if (scene_->items().size() == 0)
    ////    fileItemsManager_->Clear();

    ////QString fileName = QFileInfo(file.fileName).fileName();

    //QString fileName;
    //fileItemsManager_->Create(file.fileName, fileName);
    //fileItemsManager_->Select(fileName);

    //for (const auto& include : file.includes)
    //    fileItemsManager_->AddFileInclude(fileName, include.fileName, include.variables);
    ////for (int i = 0; i < file.includes.size(); i++)
    ////{
    ////    QList<QPair<QString, QString>> variables;
    ////    for (const auto& kvp : file.includes[i].variables.toStdMap())
    ////        variables.push_back({ kvp.first, kvp.second });
    ////}

    ////QStringList fileNames = fileItemsManager_->GetFileNames();
    ////for (auto& item : scene_->items())
    ////{
    ////    CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
    ////    auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
    ////    pi->SetFileNames(fileNames);
    ////}

    //// Convert includes into unit
    ////CubesXml::Group g{};
    ////g.path = "service";
    ////for (int i = 0; i < file.includes.size(); i++)
    ////{
    ////    CubesXml::Unit u{};
    ////    u.id = "group";
    ////    u.name = QString::fromLocal8Bit("Группа %1").arg(i);
    ////    CubesXml::Param p{};
    ////    p.name = "FILE_PATH";
    ////    p.type = "str";
    ////    p.val = file.includes[i].fileName;
    ////    u.params.push_back(std::move(p));
    ////    CubesXml::Array a{};
    ////    a.name = "VARIABLES";
    ////    for (const auto& kvp : file.includes[i].variables.toStdMap())
    ////    {
    ////        CubesXml::Item i1{};
    ////        CubesXml::Param p1{};
    ////        p1.name = "NAME";
    ////        p1.type = "str";
    ////        p1.val = kvp.first;
    ////        i1.params.push_back(std::move(p1));
    ////        CubesXml::Param p2{};
    ////        p2.name = "VALUE";
    ////        p2.type = "str";
    ////        p2.val = kvp.second;
    ////        i1.params.push_back(std::move(p2));
    ////        a.items.push_back(std::move(i1));
    ////    }
    ////    u.arrays.push_back(std::move(a));
    ////    g.units.push_back(std::move(u));
    ////}

    ////if (file.includes.size() > 0)
    ////    file.config.groups.push_back(std::move(g));

    //if (!AddUnits(fileName, "", file))
    //    return false;

    //QDir dir = QFileInfo(file.fileName).absoluteDir();
    //for (int i = 0; i < file.includes.size(); i++)
    //{
    //    QString includedFileName = dir.filePath(file.includes[i].fileName);
    //    CubesXml::File includedFile{};
    //    if (!CubesXml::Parser::Parse(includedFileName, includedFile))
    //        return false;

    //    if (!AddUnits(fileName, file.includes[i].fileName, includedFile))
    //        return false;
    //}



    ////if (!SortUnitsRectangular())
    ////    return false;

    return true;
}

bool ArrayWindow::AddUnits(const QString& fileName, const QString& includedFileName, const CubesXml::File& file)
{
    //QVector<CubesXml::Unit> all_units;
    //for (const auto& g : file.config.groups)
    //{
    //    for (const auto& u : g.units)
    //    {
    //        auto up = GetUnitParameters(u.id);
    //        if (up != nullptr)
    //        {
    //            all_units.push_back(u);
    //        }
    //        else
    //        {
    //            CubeLog::LogMessage m{};
    //            m.type = CubeLog::MessageType::error;
    //            m.source = QFileInfo(file.fileName).fileName();
    //            m.description = QString::fromLocal8Bit("Нет файла параметров для юнита %1 (%2). Юнит не добавлен.").arg(u.name, u.id);
    //            log_table_model_->AddMessage(m);
    //        }
    //    }
    //}
    //log_table_model_->submit();
    //table_view_log_->resizeColumnsToContents();
    //table_view_log_->update();

    //// Get fileNames list
    //QStringList fileNames = fileItemsManager_->GetFileNames();

    //// Transform
    //CubeDiagram::DiagramItem* di = nullptr;
    //for (int i = 0; i < all_units.size(); i++)
    //{
    //    QString name = all_units[i].id;
    //    auto up = GetUnitParameters(name);

    //    if (up != nullptr)
    //    {
    //        uint32_t propertiesId{ 0 };
    //        propertiesItemsManager_->Create(all_units[i], propertiesId);
    //        auto pi = propertiesItemsManager_->GetItem(propertiesId);

    //        //pi->ApplyXmlProperties(all_units[i]);
    //        pi->SetFileNames(fileNames);
    //        pi->SetFileName(fileName);
    //        if (includedFileName != "")
    //        {
    //            QStringList fileIncludeNames = fileItemsManager_->GetFileIncludeNames(fileName, true);
    //            // TODO: Добавить в xml название файла и убрать функцию GetFileIncludeName отовсюду за ненадобностью
    //            QString fileIncludeName = fileItemsManager_->GetFileIncludeName(fileName, includedFileName);
    //            pi->SetGroupNames(fileIncludeNames);
    //            pi->SetGroupName(fileIncludeName);
    //        }

    //        PropertiesForDrawing pfd{};
    //        if (!GetPropetiesForDrawing(propertiesId, pfd))
    //        {
    //            qDebug() << "ERROR GetPropeties: " << propertiesId;
    //        }

    //        di = new CubeDiagram::DiagramItem(propertiesId, pfd.pixmap, pfd.name, pfd.fileName, pfd.groupName, pfd.color);
    //        di->setX(all_units[i].x);
    //        di->setY(all_units[i].y);
    //        di->setZValue(all_units[i].z);
    //        scene_->addItem(di);
    //    }
    //    else
    //    {
    //        // error
    //    }
    //}

    //if (all_units.size() > 0 && di != nullptr)
    //{
    //    scene_->clearSelection();
    //    propertiesItemsManager_->Select(0);
    //    DiagramAfterItemCreated(di);
    //}
    ////if (!SortUnitsRectangular())
    ////    return false;

    return true;
}

bool ArrayWindow::SortUnits()
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

bool ArrayWindow::SortUnitsRectangular(bool check)
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
        // Все нулевые, распределяем по сетке
        int size = scene_->items().size();
        int rows = std::sqrt(scene_->items().size());
        int columns = (scene_->items().size() + rows - 1) / rows;

        int c = 0;
        int r = 0;
        for (auto& item : scene_->items(Qt::AscendingOrder))
        {
            CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);
            QPoint position(c * 200, r * 80);
            di->setPos(position);

            auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
            pi->SetPosition(di->pos());

            if (++c == columns) { ++r; c = 0; };
        }
    }

    QPointF center = scene_->itemsBoundingRect().center();
    view_->centerOn(center);

    scene_->invalidate();

    return true;
}

//{
//    if (scene_->items().size() == 0)
//        return true;
//
//    int size = scene_->items().size();
//    int rows = std::sqrt(scene_->items().size());
//    int columns = (scene_->items().size() + rows - 1) / rows;
//
//    int c = 0;
//    int r = 0;
//    for (auto& item : scene_->items())
//    {
//        CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
//        QPoint position(c * 200, r * 80);
//        di->setPos(position);
//        if (++c == columns) { ++r; c = 0; };
//    }
//    QPointF center = scene_->itemsBoundingRect().center();
//    view_->centerOn(center);
//
//    scene_->invalidate();
//    return true;
//}

CubesUnitTypes::UnitParameters* ArrayWindow::GetUnitParameters(const QString& id)
{
    for (auto& up : unitParameters_)
    {
        if (QString::fromStdString(up.fileInfo.info.id) == id)
            return &up;
    }
    return nullptr;
}

// Files
CubesUnitTypes::FileIdNames ArrayWindow::GetFileNames()
{
    return {};
}

CubesUnitTypes::IncludeIdNames ArrayWindow::GetCurrentFileIncludeNames()
{
    return {};
}
//
//QStringList ArrayWindow::GetFileNames()
//{
//    //return fileItemsManager_->GetFileNames();
//    return {};
//}
//
//QString ArrayWindow::GetCurrentFileName()
//{
//    //return fileItemsManager_->GetCurrentFileName();
//    return {};
//}
//
//QStringList ArrayWindow::GetCurrentFileIncludeNames()
//{
//    //return fileItemsManager_->GetFileIncludeNames(fileItemsManager_->GetCurrentFileName(), true);
//    return {};
//}
//
////QColor ArrayWindow::GetFileColor(const QString& fileName)
////{
////    return fileItemsManager_->GetFileColor(fileName);
////}
//
//QString ArrayWindow::GetDisplayName(const QString& baseName)
//{
//    //QList<QPair<QString, QString>> variables;
//    //for (const auto& item : scene_->items())
//    //{
//    //    CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
//    //    auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
//    //    if (pi->GetGroupName() != "<not selected>")
//    //    {
//    //        variables = fileItemsManager_->GetFileIncludeVariables(pi->GetName(),
//    //            pi->GetGroupName());
//    //    }
//    //}
//
//    QString realName = baseName;
//    //for (const auto& v : variables)
//    //{
//    //    QString replace = QString("@%1@").arg(v.first);
//    //    realName.replace(replace, v.second);
//    //}
//
//    return realName;
//}

// DiagramScene (as manager)
void ArrayWindow::DiagramItemPositionChanged(CubesDiagram::DiagramItem* di)
{
    auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
    pi->SetPosition(di->pos());
}

void ArrayWindow::DiagramAfterItemCreated(CubesDiagram::DiagramItem* di)
{
    propertiesItemsManager_->Select(di->propertiesId_);
}

void ArrayWindow::DiagramBeforeItemDeleted(CubesDiagram::DiagramItem* di)
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

//void ArrayWindow::DiagramItemNameChanged(CubeDiagram::DiagramItem* di, QString oldName)
//{
//    int i = propertiesItemsManager_->GetSelector()->findText(oldName);
//    auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
//    if (i != -1)
//        propertiesItemsManager_->GetSelector()->setItemText(i, pi->GetName());
//}
//
//void ArrayWindow::DiagramItemFileChanged(CubeDiagram::DiagramItem* di)
//{
//    auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
//    QString fileName = pi->GetFileName();
//    QStringList includeNames = fileItemsManager_->GetFileIncludeNames(fileName);
//    pi->SetGroupNames(includeNames);
//    pi->SetGroupName("<not selected>");
//}
//
//void ArrayWindow::DiagramItemGroupChanged(CubeDiagram::DiagramItem* item)
//{
//}

void ArrayWindow::selectionChanged()
{
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if (scene_->selectedItems().count() > 0)
    {
        CubesDiagram::DiagramItem* di = (CubesDiagram::DiagramItem*)(scene_->selectedItems()[0]);
        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
        //pi->PositionChanged(di->pos());
        //pi->ZOrderChanged(di->zValue());
        if (scene_->selectedItems().count() == 1)
            propertiesItemsManager_->Select(di->propertiesId_);
    }
    else
    {
        //plainTextEditHint_->setPlainText("");
        propertiesItemsManager_->Select(0);
    }
}

// FileItemsManager
void ArrayWindow::FileNameChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::FileId& oldFileId)
{
    //QStringList fileNames = fileItemsManager_->GetFileNames();
    //QStringList fileIncludeNames = fileItemsManager_->GetFileIncludeNames(fileName, true);
    //for (auto& item : scene_->items())
    //{
    //    CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
    //    auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
    //    QString currentName = pi->GetFileName();
    //    pi->SetFileNames(fileNames);
    //    if (currentName == oldFileName)
    //        pi->SetFileName(fileName);

    //    //if (fileName == di->getProperties()->GetFileName())
    //    //    di->getProperties()->SetGroupNames(fileIncludeNames);
    //}

    //for (int i = 0; i < comboBoxFiles_->count(); ++i)
    //{
    //    if (comboBoxFiles_->itemText(i) == oldFileName)
    //        comboBoxFiles_->setItemText(i, fileName);
    //}

    //////if (scene_->selectedItems().size() > 0)
    //////    reinterpret_cast<diagram_item*>(scene_->selectedItems()[0])->getProperties()->ApplyToBrowser(propertyEditor_);

    scene_->invalidate();
}

void ArrayWindow::FileListChanged(const CubesUnitTypes::FileIdNames& fileNames)
{
    for (auto& item : scene_->items())
    {
        CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);
        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
        pi->SetFileIdNames(fileNames);
    }

    //////if (scene_->selectedItems().size() > 0)
    //////    reinterpret_cast<diagram_item*>(scene_->selectedItems()[0])->getProperties()->ApplyToBrowser(propertyEditor_);

    scene_->invalidate();
}

void ArrayWindow::FileIncludeNameChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId& includeId,
    const CubesUnitTypes::IncludeId& oldIncludeId)
{
    //QStringList fileIncludeNames = fileItemsManager_->GetFileIncludeNames(fileName, true);
    //for (auto& item : scene_->items())
    //{
    //    CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
    //    auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
    //    if (fileName == pi->GetFileName())
    //    {
    //        QString currentName = pi->GetGroupName();
    //        pi->SetGroupNames(fileIncludeNames);
    //        if (currentName == oldIncludeName)
    //            pi->SetGroupName(includeName);
    //    }
    //}

    //auto fi = file_items_manager_->GetItem(fileName);
    //if (fi != nullptr)
    //    fi->UpdateRegExp();

    //////if (scene_->selectedItems().size() > 0)
    //////    reinterpret_cast<diagram_item*>(scene_->selectedItems()[0])->getProperties()->ApplyToBrowser(propertyEditor_);

    scene_->invalidate();
}

void ArrayWindow::FileIncludesListChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeIdNames& includeIdNames)
{
    //for (auto& item : scene_->items())
    //{
    //    CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);
    //    auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
    //    if (fileName == pi->GetFileName())
    //        pi->SetIncludeNames(includeNames);
    //}

    //////if (scene_->selectedItems().size() > 0)
    //////    reinterpret_cast<diagram_item*>(scene_->selectedItems()[0])->getProperties()->ApplyToBrowser(propertyEditor_);

    scene_->invalidate();
}

void ArrayWindow::FileVariableNameChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId& includeId,
    const QString& variableName, const QString& oldVariableName)
{
    propertiesItemsManager_->InformVariableChanged();
}

void ArrayWindow::FileVariablesListChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId& includeId,
    const CubesUnitTypes::VariableIdVariables& variables)
{
    propertiesItemsManager_->InformVariableChanged();
}

void ArrayWindow::FileColorChanged(const CubesUnitTypes::FileId& fileId, const QColor& color)
{

}

//void ArrayWindow::FileVariableChanged(const QString& fileName, const QString& includeName, const QList<QPair<QString, QString>>& variables)
//{
//    propertiesItemsManager_->InformVariableChanged();
//}

// PropertiesItemsManager
void ArrayWindow::PropertiesBasePropertiesChanged(const uint32_t propertiesId, const QString& name,
    const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId includeId)
{
    for (auto& item : scene_->items())
    {
        CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);
        if (di->propertiesId_ == propertiesId)
        {
            di->name_ = name;
            di->fileName_ = "";
            di->includeName_ = "";
            //di->color_ = QColor("Red");
            di->color_.setAlpha(0x20);
            di->InformNameChanged(name, "");
            di->InformIncludeChanged();
        }
    }

    scene_->invalidate();
}

void ArrayWindow::PropertiesSelectedItemChanged(const uint32_t propertiesId)
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

void ArrayWindow::PropertiesPositionChanged(const uint32_t propertiesId, double posX, double posY, double posZ)
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
}

void ArrayWindow::PropertiesError(const uint32_t propertiesId, const QString& message)
{
    //CubeLog::LogMessage m{};
    //m.type = CubeLog::MessageType::error;
    //m.source = QString("%1").arg(propertiesId);
    //m.description = message;
    //log_table_model_->AddMessage(m);

    //log_table_model_->submit();
    //table_view_log_->resizeColumnsToContents();
    //table_view_log_->update();
}

// Кнопки
void ArrayWindow::OnNewFileAction()
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

void ArrayWindow::OnOpenFileAction()
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

void ArrayWindow::OnImportXmlFileAction()
{
}

void ArrayWindow::OnSaveFileAction()
{

}

void ArrayWindow::OnSaveAsFileAction()
{
}

void ArrayWindow::OnQuitAction()
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

void ArrayWindow::OnSortBoostAction()
{
    SortUnits();
}

void ArrayWindow::OnSortRectAction()
{
    SortUnitsRectangular(false);
}


// TODO: Перенести подсказку в менеджер
//void ArrayWindow::currentItemChanged(QtBrowserItem* item)
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
void ArrayWindow::OnErrorButtonClicked(bool checked)
{
    //if (checked)
    //    sort_filter_model_->AddToFilter(CubeLog::MessageType::error);
    //else
    //    sort_filter_model_->RemoveFromFilter(CubeLog::MessageType::error);
}

void ArrayWindow::OnWarningButtonClicked(bool checked)
{
    //if (checked)
    //    sort_filter_model_->AddToFilter(CubeLog::MessageType::warning);
    //else
    //    sort_filter_model_->RemoveFromFilter(CubeLog::MessageType::warning);
}

void ArrayWindow::OnInformationButtonClicked(bool checked)
{
    //if (checked)
    //    sort_filter_model_->AddToFilter(CubeLog::MessageType::information);
    //else
    //    sort_filter_model_->RemoveFromFilter(CubeLog::MessageType::information);
}
