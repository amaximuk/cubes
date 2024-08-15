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
#include "qttreepropertybrowser.h" // потом убрать
#include "../diagram/diagram_view.h"
#include "../diagram/diagram_scene.h"
#include "../diagram/diagram_item.h"
#include "../file/file_item.h"
#include "../file/file_items_manager.h"
#include "../log/log_table_model.h"
#include "../log/sort_filter_model.h"
#include "../parameters/base64.h"
#include "../properties/properties_item_types.h"
#include "../properties/properties_item.h"
#include "../properties/properties_items_manager.h"
#include "../tree/tree_item_model.h"
#include "../xml/xml_parser.h"
#include "../xml/xml_writer.h"
#include "../unit/unit_types.h"
#include "array_window.h"

ArrayWindow::ArrayWindow(QWidget *parent)
    : QMainWindow(parent), TopManager(true)
{
    modified_ = false;
    uniqueNumber_ = 0;

    setWindowIcon(QIcon(":/images/cubes.png"));

    CreateUi();
}

ArrayWindow::~ArrayWindow()
{
}

// ITopManager
bool ArrayWindow::CreateDiagramItem(CubesUnit::PropertiesId propertiesId)
{
    TopManager::CreateDiagramItem(propertiesId);

    CubesDiagram::PropertiesForDrawing pfd{};

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

    pfd.itemType = CubesDiagram::ItemType::Unit;

    auto di = new CubesDiagram::DiagramItem(propertiesId, pfd);
    di->setX(pos.x());
    di->setY(pos.y());
    di->setZValue(z);
    scene_->addItem(di);

    scene_->clearSelection();
    propertiesItemsManager_->Select(CubesUnit::InvalidPropertiesId);
    DiagramAfterItemCreated(di);

    //scene_->clearSelection();
    //di->setPos(pos);
    //scene_->InformItemPositionChanged(di);

    //di->setSelected(true);
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

bool ArrayWindow::EnshureVisible(CubesUnit::PropertiesId propertiesId)
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

bool ArrayWindow::AddUnits(const QList<CubesXml::Unit>& units, QList<CubesUnit::PropertiesId>& addedPropertiesIds)
{
    TopManager::AddUnits(units, addedPropertiesIds);

    scene_->clearSelection();
    if (!addedPropertiesIds.isEmpty())
        propertiesItemsManager_->Select(addedPropertiesIds.first());
    for (const auto& item : scene_->items())
    {
        CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);
        if (addedPropertiesIds.contains(di->GetPropertiesId()))
            di->setSelected(true);
    }
    scene_->invalidate();

    return true;
}

bool ArrayWindow::GetVisibleSceneRect(QRectF& rect)
{
    // TopManager::GetVisibleSceneRect(rect);

    rect = view_->GetVisibleSceneRect();

    return true;
}

void ArrayWindow::SetItemModel(parameters::file_info afi, CubesUnit::ParameterModelPtr parameterModelPtr,
    parameters::restrictions_info ri, QSharedPointer<CubesProperties::PropertiesItem> pi)
{
    pm_ = parameterModelPtr;
    pi_ = pi;
    ri_ = ri;

    CubesUnit::UnitParameters up{ afi, {} };
    unitIdUnitParametersPtr_[QString::fromStdString(afi.info.id)] = CubesUnit::CreateUnitParametersPtr(up);


//            auto& up = unitParameters_[QString::fromStdString(fi.info.id)];
//            up.fileInfo = fi;
//            up.platforms.insert(QFileInfo(platformDir).baseName());
    CubesDiagram::DiagramItem* di = nullptr;

    for (auto& item : parameterModelPtr->parameters)
    {
        //auto& up = unitParameters_[QString::fromStdString(afi.info.id)];
        //up.fileInfo = afi;

        auto rename = [](CubesUnit::ParameterModelPtrs parameterModelPtrs,
            const CubesUnit::ParameterModelId to_remove, auto&& rename) -> void
        {
            for (auto& parameter : parameterModelPtrs)
            {
                parameter->id = parameter->id.mid(to_remove.size());
                rename(parameter->parameters, to_remove, rename);
            }
        };

        //auto rename = [](QList<CubesUnit::ParameterModel>& parameters, QString to_remove, auto&& rename) -> void {
        //    for (auto& parameter : parameters)
        //    {
        //        parameter.id = parameter.id.mid(to_remove.length() + 1);
        //        //parameter.id = "PARAMETERS/" + parameter.id.mid(to_remove.length() + 1);
        //        rename(parameter.parameters, to_remove, rename);
        //    }
        //};

        //auto id_renamer = [](QList<CubesUnit::ParameterModel>& parameters, QString to_remove)->void {
        //    auto id_renamer_impl = [](QList<CubesUnit::ParameterModel>& parameters, QString to_remove, auto& id_renamer_ref)->void {
        //        for (auto& parameter : parameters)
        //        {
        //            parameter.id = "PARAMETERS/" + parameter.id.mid(to_remove.length() + 1);
        //            id_renamer_ref(parameter.parameters, to_remove, id_renamer_ref);
        //        }
        //    };
        //    return id_renamer_impl(parameters, to_remove, id_renamer_impl);
        //};

        //id_renamer(item.parameters, item.id);

        rename(item->parameters, item->id, rename);

        for (auto& group : item->parameters)
        {
            if (group->id == ids_.parameters)
            {
                for (auto& parameter : group->parameters)
                {
                    parameter->parameterInfoId.type = QString::fromStdString(parameters::helper::type::main_type);

                    auto& pi = *parameters::helper::parameter::get_parameter_info(afi,
                        parameters::helper::type::main_type, parameter->parameterInfoId.name.toStdString());

                    bool isArray = parameters::helper::common::get_is_array_type(pi.type);
                    auto itemType = parameters::helper::common::get_item_type(pi.type);
                    bool isInner = parameters::helper::common::get_is_inner_type(itemType);

                    if (isArray && isInner)
                    {
                        for (auto& arrayParameter : parameter->parameters)
                            arrayParameter->parameterInfoId.type = QString::fromStdString(parameters::helper::type::main_type);
                    }
                }
            }
        }

        CubesUnit::ParameterModelPtrs m = item->parameters;

        CubesUnit::PropertiesId propertiesId{ CubesUnit::InvalidPropertiesId };
        propertiesItemsManager_->Create(QString::fromStdString(afi.info.id), m, propertiesId);
        auto pi = propertiesItemsManager_->GetItem(propertiesId);

        CubesDiagram::PropertiesForDrawing pfd{};
        if (!GetPropetiesForDrawing(propertiesId, pfd))
        {
            qDebug() << "ERROR GetPropeties: " << propertiesId;
        }
        const auto position = pi->GetPosition();
        const auto zOrder = pi->GetZOrder();

        pfd.itemType = CubesDiagram::ItemType::Unit;

        di = new CubesDiagram::DiagramItem(propertiesId, pfd);
        di->setX(position.x());
        di->setY(position.y());
        di->setZValue(zOrder);
        scene_->addItem(di);
    }

    if (di != nullptr)
    {
        scene_->clearSelection();
        propertiesItemsManager_->Select(CubesUnit::InvalidPropertiesId);
        DiagramAfterItemCreated(di);
    }

    SortUnitsRectangular(true);
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

    pm_->parameters.clear();

    int item_index = 0;

    //const auto ids = propertiesItemsManager_->GetPropertyIds();
    for (auto& id : ids)
    {
        auto item = propertiesItemsManager_->GetItem(id);
        auto pm = item->GetParameterModelPtrs();

        const auto up = item->GetUnitParametersPtr();
        const auto type = QString::fromStdString(up->fileInfo.info.id);
        for (auto& group : pm)
        {
            if (group->id == ids_.parameters)
            {
                for (auto& parameter : group->parameters)
                {
                    parameter->parameterInfoId.type = type;

                    auto& pi = *parameters::helper::parameter::get_parameter_info(up->fileInfo,
                        parameters::helper::type::main_type, parameter->parameterInfoId.name.toStdString());
                    

                    bool isArray = parameters::helper::common::get_is_array_type(pi.type);
                    auto itemType = parameters::helper::common::get_item_type(pi.type);
                    bool isInner = parameters::helper::common::get_is_inner_type(itemType);

                    if (isArray && isInner)
                    {
                        for (auto& arrayParameter : parameter->parameters)
                            arrayParameter->parameterInfoId.type = type;
                    }
                }
            }
        }

        auto rename = [](CubesUnit::ParameterModelPtrs parameterModelPtrs,
            const CubesUnit::ParameterModelId to_add, auto&& rename) -> void
        {
            for (auto& parameter : parameterModelPtrs)
            {
                parameter->id = to_add + parameter->id;
                rename(parameter->parameters, to_add, rename);
            }
        };

        rename(pm, pm_->id + ids_.Item(item_index), rename); // !!!!!!!!!!!! item_index

        CubesUnit::ParameterModelPtr itemPm = CubesUnit::CreateParameterModelPtr();
        itemPm->id = pm_->id + ids_.Item(item_index);
        itemPm->name = QString::fromLocal8Bit("Элемент %1").arg(item_index);
        itemPm->value = QVariant();
        itemPm->editorSettings.type = CubesUnit::EditorType::None;
        itemPm->parameters = pm;

        pm_->parameters.push_back(itemPm);

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
{
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
    CreateScene();
    CreateView();

    QWidget* propertiesPanelWidget = CreatePropertiesPanelWidget();
    propertiesPanelWidget->setMinimumWidth(400);
    QSplitter* splitterMain = new QSplitter(Qt::Horizontal);
    splitterMain->addWidget(view_);
    splitterMain->addWidget(propertiesPanelWidget);
    splitterMain->setStretchFactor(0, 1);
    splitterMain->setStretchFactor(1, 0);

    QWidget* mainWidget = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(splitterMain);
    mainWidget->setLayout(mainLayout);
    return mainWidget;
}

void ArrayWindow::CreateScene()
{
    scene_ = new CubesDiagram::DiagramScene(this, this, true);
    scene_->setSceneRect(-10000, -10000, 20032, 20032);

    qDebug() << connect(scene_, &CubesDiagram::DiagramScene::ItemPositionChanged, this, &ArrayWindow::DiagramItemPositionChanged);
    qDebug() << connect(scene_, &CubesDiagram::DiagramScene::AfterItemCreated, this, &ArrayWindow::DiagramAfterItemCreated);
    qDebug() << connect(scene_, &CubesDiagram::DiagramScene::BeforeItemDeleted, this, &ArrayWindow::DiagramBeforeItemDeleted);
    qDebug() << connect(scene_, &CubesDiagram::DiagramScene::selectionChanged, this, &ArrayWindow::selectionChanged);
}

void ArrayWindow::CreateView()
{
    view_ = new CubesDiagram::DiagramView(this, scene_);
    view_->setDragMode(QGraphicsView::RubberBandDrag);
}

QWidget* ArrayWindow::CreatePropertiesPanelWidget()
{
    QWidget* propertiesPanelWidget = new QWidget;

    QWidget* propertiesWidget = propertiesItemsManager_->GetWidget();

    QVBoxLayout* propertiesPaneLayout = new QVBoxLayout;
    propertiesPaneLayout->addWidget(propertiesWidget);
    propertiesPaneLayout->setContentsMargins(0, 0, 0, 0);

    propertiesPanelWidget->setLayout(propertiesPaneLayout);

    return propertiesPanelWidget;
}

// Units
//bool ArrayWindow::AddMainFile(CubesXml::File& file)
//{
//    // Специально переопредилили, чтобы ничего не добавлять
//    return true;
//}
//
//bool ArrayWindow::AddUnits(const QString& fileName, const QString& includedFileName, const CubesXml::File& file)
//{
//    // Специально переопредилили, чтобы ничего не добавлять
//    return true;
//}
//
bool ArrayWindow::SortUnitsBoost()
{
    if (!TopManager::SortUnitsBoost())
        return false;

    QPointF center = scene_->itemsBoundingRect().center();
    view_->centerOn(center);
    scene_->invalidate();

    return true;
}

bool ArrayWindow::SortUnitsRectangular(bool check)
{
    if (!TopManager::SortUnitsRectangular(check))
        return false;

    QPointF center = scene_->itemsBoundingRect().center();
    view_->centerOn(center);
    scene_->invalidate();

    return true;
}

//bool ArrayWindow::SortUnitsBoost()
//{
//    // Prepare sort
//    int nextIndex = 0;
//    QMap<QString, int> nameToIndex;
//    QMap<int, QString> indexToName;
//    QMap<QString, QSet<QString>> connectedNames;
//    for (auto& item : scene_->items())
//    {
//        CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);
//
//        if (!nameToIndex.contains(di->name_))
//        {
//            nameToIndex[di->name_] = nextIndex;
//            indexToName[nextIndex] = di->name_;
//            nextIndex++;
//        }
//
//        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
//        auto connected = pi->GetConnectedNames();
//        connectedNames[di->name_].unite(QSet<QString>(connected.begin(), connected.end()));
//    }
//
//    // Sort
//    std::vector<std::pair<int, int>> edges;
//
//    for (const auto& kvp : connectedNames.toStdMap())
//    {
//        for (const auto& se : kvp.second)
//        {
//            if (nameToIndex.contains(kvp.first) && nameToIndex.contains(se))
//                edges.push_back({ nameToIndex[kvp.first], nameToIndex[se] });
//        }
//    }
//
//    std::vector<std::pair<int, int>> coordinates;
//    if (!CubesGraph::RearrangeGraph(nameToIndex.size(), edges, coordinates))
//    {
//        return false;
//    }
//
//    auto vr = view_->mapToScene(view_->viewport()->geometry()).boundingRect();
//    for (auto& item : scene_->items())
//    {
//        CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);
//
//        int i = nameToIndex[di->name_];
//
//        //QPoint position(vr.left() + 60 + coordinates[i].first * 60,
//        //    vr.top() + 60 + coordinates[i].second * 60);
//
//        int gridSize = 20;
//        //qreal xV = round(position.x() / gridSize) * gridSize;
//        //qreal yV = round(position.y() / gridSize) * gridSize;
//
//        //QPoint position(round(vr.left() / gridSize) * gridSize, round(vr.top() / gridSize) * gridSize);
//        //position += QPoint(60 + coordinates[i].first * 60, 60 + coordinates[i].second * 60);
//
//
//
//
//        QPoint position(80 + coordinates[i].first * 80, 80 + coordinates[i].second * 80);
//        //QPoint position(60 + coordinates[i].first * 60, 60 + coordinates[i].second * 60);
//        di->setPos(position);
//
//        auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
//        pi->SetPosition(di->pos());
//
//
//        //di->setSelected(true);
//    }
//    QPointF center = scene_->itemsBoundingRect().center();
//    //QPointF centerMapped = view_->mapFromScene(center);
//    view_->centerOn(center);
//
//    scene_->invalidate();
//    return true;
//}
//
//bool ArrayWindow::SortUnitsRectangular(bool check)
//{
//    if (scene_->items().size() == 0)
//        return true;
//
//    bool sort = true;
//    if (check)
//    {
//        int count = 0;
//        for (auto& item : scene_->items())
//        {
//            CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);
//            QPointF p = di->pos();
//            if (qFuzzyIsNull(p.x()) && qFuzzyIsNull(p.y()))
//                ++count;
//        }
//
//        // Все нулевые, распределяем по сетке
//        if (count != scene_->items().size())
//            sort = false;
//    }
//
//
//    if (sort)
//    {
//        // Все нулевые, распределяем по сетке
//        int size = scene_->items().size();
//        int rows = std::sqrt(scene_->items().size());
//        int columns = (scene_->items().size() + rows - 1) / rows;
//
//        int c = 0;
//        int r = 0;
//        for (auto& item : scene_->items(Qt::AscendingOrder))
//        {
//            CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);
//            QPoint position(c * 200, r * 80);
//            di->setPos(position);
//
//            auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
//            pi->SetPosition(di->pos());
//
//            if (++c == columns) { ++r; c = 0; };
//        }
//    }
//
//    QPointF center = scene_->itemsBoundingRect().center();
//    view_->centerOn(center);
//
//    scene_->invalidate();
//
//    return true;
//}

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

// Files
CubesUnit::FileIdNames ArrayWindow::GetFileNames()
{
    return {};
}

CubesUnit::IncludeIdNames ArrayWindow::GetCurrentFileIncludeNames()
{
    return {};
}

// DiagramScene (as manager)
void ArrayWindow::DiagramItemPositionChanged(CubesDiagram::DiagramItem* di)
{
    auto pi = propertiesItemsManager_->GetItem(di->GetPropertiesId());
    pi->SetPosition(di->pos());
}

void ArrayWindow::DiagramAfterItemCreated(CubesDiagram::DiagramItem* di)
{
    propertiesItemsManager_->Select(di->GetPropertiesId());
}

void ArrayWindow::DiagramBeforeItemDeleted(CubesDiagram::DiagramItem* di)
{
    propertiesItemsManager_->Remove(di->GetPropertiesId());
}

void ArrayWindow::selectionChanged()
{
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if (scene_->selectedItems().count() > 0)
    {
        CubesDiagram::DiagramItem* di = (CubesDiagram::DiagramItem*)(scene_->selectedItems()[0]);
        auto pi = propertiesItemsManager_->GetItem(di->GetPropertiesId());
        //pi->PositionChanged(di->pos());
        //pi->ZOrderChanged(di->zValue());
        if (scene_->selectedItems().count() == 1)
            propertiesItemsManager_->Select(di->GetPropertiesId());
    }
    else
    {
        //plainTextEditHint_->setPlainText("");
        propertiesItemsManager_->Select(CubesUnit::InvalidPropertiesId);
    }
}

// PropertiesItemsManager
void ArrayWindow::PropertiesBasePropertiesChanged(CubesUnit::PropertiesId propertiesId, const QString& name,
    const CubesUnit::FileId fileId, const CubesUnit::IncludeId includeId)
{
    TopManager::PropertiesBasePropertiesChanged(propertiesId, name, fileId, includeId);

    const auto fileName = fileItemsManager_->GetFileName(fileId);
    QString includeName;
    if (!fileItemsManager_->GetFileIncludeName(fileId, includeId, includeName))
        includeName = "";
    const auto color = fileItemsManager_->GetFileColor(fileId);

    scene_->InformBasePropertiesChanged(propertiesId, name, fileName, includeName, color);

    scene_->invalidate();
}

void ArrayWindow::PropertiesSelectedItemChanged(CubesUnit::PropertiesId propertiesId)
{
    TopManager::PropertiesSelectedItemChanged(propertiesId);

    QGraphicsItem* item_to_select = nullptr;
    for (auto& item : scene_->items())
    {
        CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);
        if (di->GetPropertiesId() == propertiesId)
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

void ArrayWindow::PropertiesPositionChanged(CubesUnit::PropertiesId propertiesId, double posX, double posY, double posZ)
{
    TopManager::PropertiesPositionChanged(propertiesId, posX, posY, posZ);

    for (auto& item : scene_->items())
    {
        CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);
        if (di->GetPropertiesId() == propertiesId)
        {
            di->setPos(QPointF(posX, posY));
            di->setZValue(posZ);
            break;
        }
    }
}

void ArrayWindow::PropertiesError(CubesUnit::PropertiesId propertiesId, const QString& message)
{
    TopManager::PropertiesError(propertiesId, message);
}

// Кнопки
void ArrayWindow::OnSortBoostAction()
{
    SortUnitsBoost();
}

void ArrayWindow::OnSortRectAction()
{
    SortUnitsRectangular(false);
}
