#pragma once

#include <QMainWindow>
#include <QPointer>
#include "../properties/properties_item.h" // !!!!!!!!!!!!!!! used directly not top_manager_interface
#include "../top/top_manager.h"

namespace CubesTop { class ITopManager; }
namespace CubesDiagram { class DiagramItem; }
namespace CubesDiagram { class DiagramScene; }
namespace CubesDiagram { class DiagramView; }
namespace CubesUnitTypes { class UnitParameters; }
namespace CubesLog { class LogTableModel; }
namespace CubesLog { class SortFilterModel; }
namespace CubesFile { class FileItemsManager; }
namespace CubesProperties { class PropertiesItemsManager; }

class QPlainTextEdit;
class QGraphicsScene;
class QGraphicsView;
class QTableView;
class QTreeView;
class QToolBox;
class QSplitter;
class QtProperty;
class QtTreePropertyBrowser;
class QtBrowserItem;
class QComboBox;

class ArrayWindow : public QMainWindow, public CubesTop::TopManager
{
    Q_OBJECT

private:
    bool modified_;
    uint32_t uniqueNumber_;

    QPointer<CubesDiagram::DiagramScene> scene_;
    QPointer<CubesDiagram::DiagramView> view_;

    CubesUnitTypes::ParameterModel pm_{};
    QSharedPointer<CubesProperties::PropertiesItem> pi_;
    parameters::restrictions_info ri_{};

public:
    explicit ArrayWindow(QWidget* parent = nullptr);
    ~ArrayWindow() override;

public:
    // ITopManager
    bool CreateDiagramItem(CubesUnitTypes::PropertiesId propertiesId) override;
    bool EnshureVisible(CubesUnitTypes::PropertiesId propertiesId) override;

    // ArrayWindow
    void SetItemModel(parameters::file_info afi, CubesUnitTypes::ParameterModel pm,
        parameters::restrictions_info ri, QSharedPointer<CubesProperties::PropertiesItem> pi);

public:
    void closeEvent(QCloseEvent* event) override;

signals:
    void BeforeClose(const bool result, CubesUnitTypes::ParameterModel pm, QSharedPointer<CubesProperties::PropertiesItem> pi);

protected:
    // UI
    void CreateUi();
    void CreateMenu();
    QWidget* CreateMainWidget();
    void CreateScene();
    void CreateView();
    QWidget* CreatePropertiesPanelWidget();

    // Units
    //bool AddMainFile(CubesXml::File& file);
    //bool AddUnits(const QString& fileName, const QString& includedFileName, const CubesXml::File& file);
    bool SortUnitsBoost() override;
    bool SortUnitsRectangular(bool check) override;

    // Files
    CubesUnitTypes::FileIdNames GetFileNames() override;
    CubesUnitTypes::IncludeIdNames GetCurrentFileIncludeNames() override;

public slots:
    // DiagramScene (as manager)
    void DiagramItemPositionChanged(CubesDiagram::DiagramItem* item);
    void DiagramAfterItemCreated(CubesDiagram::DiagramItem* item);
    void DiagramBeforeItemDeleted(CubesDiagram::DiagramItem* item);
    void selectionChanged(); // QGraphicsScene

    // PropertiesItemsManager
    void PropertiesBasePropertiesChanged(CubesUnitTypes::PropertiesId propertiesId, const QString& name,
        CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId includeId) override;
    void PropertiesSelectedItemChanged(CubesUnitTypes::PropertiesId propertiesId) override;
    void PropertiesPositionChanged(CubesUnitTypes::PropertiesId propertiesId, double posX, double posY, double posZ) override;
    void PropertiesError(CubesUnitTypes::PropertiesId propertiesId, const QString& message) override;

private slots:
    // Кнопки
    void OnSortBoostAction();
    void OnSortRectAction();
};
