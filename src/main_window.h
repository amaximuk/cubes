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
    bool EnshureVisible(CubesUnitTypes::PropertiesId propertiesId) override;
//    bool GetAnalysisFiles(QVector<CubesAnalysis::File>& files) override;
//    bool GetAnalysisProperties(QVector<CubesAnalysis::Properties>& properties) override;
//
    // ILogManager
    void AddMessage(const CubesLog::LogMessage& m) override;

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
    bool SortUnits() override;
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
    //void DiagramItemNameChanged(CubeDiagram::DiagramItem* item, QString oldName);
    //void DiagramItemFileChanged(CubeDiagram::DiagramItem* item);
    //void DiagramItemGroupChanged(CubeDiagram::DiagramItem* item);
    void selectionChanged(); // QGraphicsScene

    // FileItemsManager
    virtual void FileNameChanged(CubesUnitTypes::FileId fileId);
    virtual void FileListChanged(const CubesUnitTypes::FileIdNames& fileIdNames);
    virtual void FileIncludeNameChanged(CubesUnitTypes::FileId fileId, CubesUnitTypes::IncludeId includeId);
    virtual void FileIncludesListChanged(CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeIdNames& includeNames);
    virtual void FileVariableNameChanged(CubesUnitTypes::FileId fileId, CubesUnitTypes::IncludeId includeId,
        const QString& variableName, const QString& oldVariableName);
    virtual void FileVariablesListChanged(CubesUnitTypes::FileId fileId, CubesUnitTypes::IncludeId includeId,
        const CubesUnitTypes::VariableIdVariables& variables);
    virtual void FileColorChanged(CubesUnitTypes::FileId fileId, const QColor& color);
    virtual void FilePropertiesChanged();

    // PropertiesItemsManager
    virtual void PropertiesBasePropertiesChanged(CubesUnitTypes::PropertiesId propertiesId, const QString& name,
        CubesUnitTypes::FileId fileId, CubesUnitTypes::IncludeId includeId);
    virtual void PropertiesSelectedItemChanged(CubesUnitTypes::PropertiesId propertiesId);
    virtual void PropertiesPositionChanged(CubesUnitTypes::PropertiesId propertiesId, double posX, double posY, double posZ);
    virtual void PropertiesError(CubesUnitTypes::PropertiesId propertiesId, const QString& message);
    virtual void PropertiesConnectionChanged(CubesUnitTypes::PropertiesId propertiesId);
    virtual void PropertiesPropertiesChanged();

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
