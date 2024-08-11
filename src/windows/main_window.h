#pragma once

#include <QMainWindow>
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
namespace CubesAnalysis { class AnalysisManager; }

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

class MainWindow : public QMainWindow, public CubesTop::TopManager
{
    Q_OBJECT

private:
    bool modified_;
    QString path_;

    QPointer<CubesDiagram::DiagramScene> scene_;
    QPointer<CubesDiagram::DiagramView> view_;
    QPointer<QTreeView> tree_view_;
    QPointer<QTableView> table_view_log_;
    QPointer<QTreeView> tree_;
    QPointer<QMenu> recentMenu_;
    QPointer<CubesLog::LogTableModel> log_table_model_;
    QPointer<CubesLog::SortFilterModel> sort_filter_model_;

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

public:
    // ITopManager
    bool CreateDiagramItem(CubesUnit::PropertiesId propertiesId) override;
    bool EnshureVisible(CubesUnit::PropertiesId propertiesId) override;
    bool AddUnits(const QList<CubesXml::Unit>& units, QList<CubesUnit::PropertiesId>& addedPropertiesIds) override;
    bool UnitsContextMenuRequested(QPoint globalPosition, QList<CubesUnit::PropertiesId>& propertiesIds) override;
    bool GetVisibleSceneRect(QRectF& rect) override;

    // ILogManager
    void AddMessage(const CubesLog::Message& m) override;

private:
    void closeEvent(QCloseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

protected:
    // UI
    void CreateUi();
    void CreateMenu();
    QWidget* CreateMainWidget();
    QWidget* CreateLogWidget();
    void CreateScene();
    void CreateView();
    void CreateTreeView();
    QWidget* CreatePropertiesPanelWidget();
    void FillTreeView();

    // Units
    bool SortUnitsBoost() override;
    bool SortUnitsRectangular(bool check) override;

    // Modified
    void UpdateFileState(const QString& path, bool modified);

    // External
    bool NewFile() override;
    bool SaveFile(const QString& path) override;
    bool SaveFolder(const QString& path) override;
    bool OpenFile(const QString& path) override;
    bool OpenFolder(const QString& path) override;
    bool ImportXml(const QString& path) override;
    
    // Recent
    void UpdateRecent();
    void AddRecent(QString fileName);
    void RemoveRecent(QString fileName);

public slots:
    // DiagramScene (as manager)
    void DiagramItemPositionChanged(CubesDiagram::DiagramItem* item);
    void DiagramAfterItemCreated(CubesDiagram::DiagramItem* item);
    void DiagramBeforeItemDeleted(CubesDiagram::DiagramItem* item);
    void selectionChanged(); // QGraphicsScene

    // FileItemsManager
    void FileNameChanged(CubesUnit::FileId fileId) override;
    void FileListChanged(const CubesUnit::FileIdNames& fileIdNames) override;
    void FileIncludeNameChanged(CubesUnit::FileId fileId, CubesUnit::IncludeId includeId) override;
    void FileIncludesListChanged(CubesUnit::FileId fileId, const CubesUnit::IncludeIdNames& includeNames) override;
    void FileVariableNameChanged(CubesUnit::FileId fileId, CubesUnit::IncludeId includeId,
        const QString& variableName, const QString& oldVariableName) override;
    void FileVariablesListChanged(CubesUnit::FileId fileId, CubesUnit::IncludeId includeId,
        const CubesUnit::VariableIdVariables& variables) override;
    void FileColorChanged(CubesUnit::FileId fileId, const QColor& color) override;
    void FilePropertiesChanged() override;

    // PropertiesItemsManager
    void PropertiesBasePropertiesChanged(CubesUnit::PropertiesId propertiesId, const QString& name,
        CubesUnit::FileId fileId, CubesUnit::IncludeId includeId) override;
    void PropertiesSelectedItemChanged(CubesUnit::PropertiesId propertiesId) override;
    void PropertiesPositionChanged(CubesUnit::PropertiesId propertiesId, double posX, double posY, double posZ) override;
    void PropertiesError(CubesUnit::PropertiesId propertiesId, const QString& message) override;
    void PropertiesConnectionChanged(CubesUnit::PropertiesId propertiesId) override;
    void PropertiesPropertiesChanged() override;

private slots:
    //  ÌÓÔÍË
    void OnNewFileAction();
    void OnOpenFileAction();
    void OnOpenFolderAction();
    void OnImportXmlFileAction();
    void OnSaveFileAction();
    void OnSaveAsFileAction();
    void OnSaveFolderAction();
    void OnQuitAction();
    void OnSortBoostAction();
    void OnSortRectAction();
    void OnRecentAction();
    void OnTestAction();

    // ContextMenu
    void OnSetFileMenuAction(bool checked);

    // ÀÓ„
    void OnErrorButtonClicked(bool checked);
    void OnWarningButtonClicked(bool checked);
    void OnInformationButtonClicked(bool checked);
    void OnDoubleClicked(const QModelIndex& index);
};
