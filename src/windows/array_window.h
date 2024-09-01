#pragma once

#include <QMainWindow>
#include <QPointer>
#include "../properties/properties_item.h" // !!!!!!!!!!!!!!! used directly not top_manager_interface
#include "../top/top_manager.h"

namespace CubesTop { class ITopManager; }
namespace CubesDiagram { class DiagramItem; }
namespace CubesDiagram { class DiagramScene; }
namespace CubesDiagram { class DiagramView; }
namespace CubesUnit { class UnitParameters; }
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

    CubesUnit::ParameterModelPtr pm_{};
    QSharedPointer<CubesProperties::PropertiesItem> pi_;
    parameters::restrictions_info ri_{};

public:
    explicit ArrayWindow(QWidget* parent = nullptr);
    ~ArrayWindow() override;

public:
    // ITopManager
    bool CreateDiagramItem(CubesUnit::PropertiesId propertiesId) override;
    bool EnshureVisible(CubesUnit::PropertiesId propertiesId) override;
    bool AddUnits(const QList<CubesXml::Unit>& units, QList<CubesUnit::PropertiesId>& addedPropertiesIds) override;
    bool GetVisibleSceneRect(QRectF& rect) override;

    // ArrayWindow
    void SetItemModel(parameters::file_info afi, CubesUnit::ParameterModelPtr parameterModelPtr,
        parameters::restrictions_info ri, QSharedPointer<CubesProperties::PropertiesItem> pi);

public:
    void closeEvent(QCloseEvent* event) override;

signals:
    void BeforeClose(const bool result, CubesUnit::ParameterModelPtr parameterModelPtr,
        QSharedPointer<CubesProperties::PropertiesItem> pi);

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
    CubesUnit::FileIdNames GetFileNames() override;
    CubesUnit::IncludeIdNames GetCurrentFileIncludeNames() override;

public slots:
    // DiagramScene (as manager)
    void DiagramItemPositionChanged(CubesDiagram::DiagramItem* item);
    void DiagramAfterItemCreated(CubesDiagram::DiagramItem* item);
    void DiagramBeforeItemDeleted(CubesDiagram::DiagramItem* item);
    void selectionChanged(); // QGraphicsScene

    // PropertiesItemsManager
    void PropertiesBasePropertiesChanged(CubesUnit::PropertiesId propertiesId, const QString& name,
        CubesUnit::FileId fileId, const CubesUnit::IncludeId includeId) override;
    void PropertiesSelectedItemChanged(CubesUnit::PropertiesId propertiesId) override;
    void PropertiesPositionChanged(CubesUnit::PropertiesId propertiesId, double posX, double posY, double posZ) override;

private slots:
    // Кнопки
    void OnSortBoostAction();
    void OnSortRectAction();
};
