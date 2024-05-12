#pragma once

#include <QMainWindow>
#include "top/top_manager.h"

namespace CubesTop { class ITopManager; }
namespace CubesDiagram { class DiagramItem; }
namespace CubesDiagram { class DiagramScene; }
namespace CubesDiagram { class DiagramView; }
namespace CubesUnitTypes { class UnitParameters; }
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

//public:
//    // ITopManager
//    bool GetUnitsInFileList(const CubesUnitTypes::FileId& fileId, QStringList& unitNames) override;
//    bool GetUnitsInFileIncludeList(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId includeId,
//        QStringList& unitNames) override;
//    bool GetUnitParameters(const QString& unitId, CubesUnitTypes::UnitParameters& unitParameters) override;
//    bool GetFileIncludeList(const CubesUnitTypes::FileId& fileId, CubesUnitTypes::IncludeIdNames& includeNames) override;
//    bool GetFileIncludeVariableList(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId includeId,
//        CubesUnitTypes::VariableIdVariables& variables) override;
//    bool CreatePropetiesItem(const QString& unitId, CubesUnitTypes::PropertiesId& propertiesId) override;
//    bool GetPropetiesForDrawing(const CubesUnitTypes::PropertiesId propertiesId, CubesTop::PropertiesForDrawing& pfd) override;
//    bool GetPropetiesUnitParameters(const CubesUnitTypes::PropertiesId propertiesId,
//        CubesUnitTypes::UnitParameters& unitParameters) override;
//    bool GetPropetiesUnitId(const CubesUnitTypes::PropertiesId propertiesId, QString& unitId) override;
//    bool GetUnitsConnections(QMap<QString, QStringList>& connections) override;
//    bool GetDependsConnections(QMap<QString, QStringList>& connections) override;
    bool CreateDiagramItem(CubesUnitTypes::PropertiesId propertiesId) override;
//    bool EnshureVisible(CubesUnitTypes::PropertiesId propertiesId) override;
//    bool GetAnalysisFiles(QVector<CubesAnalysis::File>& files) override;
//    bool GetAnalysisProperties(QVector<CubesAnalysis::Properties>& properties) override;
//
//    // ILogManager
//    void AddMessage(const CubesLog::LogMessage& m) override;

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
    bool AddMainFile(const CubesXml::File& file, const QString& zipFileName) override;
    bool AddUnits(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId includeId, const CubesXml::File& file) override;
    bool SortUnits() override;
    bool SortUnitsRectangular(bool check) override;
    CubesUnitTypes::UnitParameters* GetUnitParameters(const QString& id) override;
    bool Test() override;

    // Files
    CubesUnitTypes::FileIdNames GetFileNames() override;
    CubesUnitTypes::IncludeIdNames GetCurrentFileIncludeNames() override;

    // Modified
    void UpdateFileState(const QString& path, bool modified) override;
    bool SaveFileInternal(const QString& path) override;
    bool SaveFolderInternal(const QString& path) override;
    bool OpenFileInternal(const QString& path) override;
    bool OpenFolderInternal(const QString& path) override;
    
    void UpdateRecent();
    void AddRecent(QString fileName);
    void RemoveRecent(QString fileName);

public slots:
    // DiagramScene (as manager)
    void DiagramItemPositionChanged(CubesDiagram::DiagramItem* item);
    void DiagramAfterItemCreated(CubesDiagram::DiagramItem* item);
    void DiagramBeforeItemDeleted(CubesDiagram::DiagramItem* item);
    //void DiagramItemNameChanged(CubeDiagram::DiagramItem* item, QString oldName);
    //void DiagramItemFileChanged(CubeDiagram::DiagramItem* item);
    //void DiagramItemGroupChanged(CubeDiagram::DiagramItem* item);
    void selectionChanged(); // QGraphicsScene

    // FileItemsManager
    void FileNameChanged(const CubesUnitTypes::FileId& fileId);
    void FileListChanged(const CubesUnitTypes::FileIdNames& fileIdNames);
    void FileIncludeNameChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId& includeId);
    void FileIncludesListChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeIdNames& includeNames);
    void FileVariableNameChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId& includeId,
        const QString& variableName, const QString& oldVariableName);
    void FileVariablesListChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId& includeId,
        const CubesUnitTypes::VariableIdVariables& variables);
    void FileColorChanged(const CubesUnitTypes::FileId& fileId, const QColor& color);
    void FilePropertiesChanged();

    // PropertiesItemsManager
    void PropertiesBasePropertiesChanged(CubesUnitTypes::PropertiesId propertiesId, const QString& name,
        CubesUnitTypes::FileId fileId, CubesUnitTypes::IncludeId includeId);
    void PropertiesSelectedItemChanged(CubesUnitTypes::PropertiesId propertiesId);
    void PropertiesPositionChanged(CubesUnitTypes::PropertiesId propertiesId, double posX, double posY, double posZ);
    void PropertiesError(CubesUnitTypes::PropertiesId propertiesId, const QString& message);
    void PropertiesConnectionChanged(CubesUnitTypes::PropertiesId propertiesId);
    void PropertiesPropertiesChanged();

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

    // ÀÓ„
    void OnErrorButtonClicked(bool checked);
    void OnWarningButtonClicked(bool checked);
    void OnInformationButtonClicked(bool checked);
    void OnDoubleClicked(const QModelIndex& index);
};
