#pragma once

#include <QMainWindow>
#include <QPointer>
#include "log_table/log_table_interface.h"
#include "top_manager_interface.h"

namespace CubesDiagram
{
    class DiagramItem;
    class DiagramScene;
    class DiagramView;
}

namespace CubesUnitTypes
{
    struct UnitParameters;
}

namespace CubesLog
{
    class LogTableModel;
    class SortFilterModel;
}

namespace CubesFile
{
    class FileItemsManager;
}

namespace CubesProperties
{
    class PropertiesItemsManager;
}

namespace CubesAnalysis
{
    class AnalysisManager;
}

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

class MainWindow : public QMainWindow, ITopManager, CubesLog::ILogManager
{
    Q_OBJECT

private:
    bool modified_;
    QString path_;
    uint32_t uniqueNumber_;

    QPointer<CubesDiagram::DiagramScene> scene_;
    QPointer<CubesDiagram::DiagramView> view_;
    QPointer<QTreeView> tree_view_;
    QPointer<QTableView> table_view_log_;
    QPointer<QTreeView> tree_;
    QPointer<QMenu> recentMenu_;
    //QPointer<QPlainTextEdit> plainTextEditHint_;
    QMap<QString, CubesUnitTypes::UnitParameters> unitParameters_;
    QPointer<CubesLog::LogTableModel> log_table_model_;
    QPointer<CubesLog::SortFilterModel> sort_filter_model_;

    QPointer<CubesFile::FileItemsManager> fileItemsManager_;
    QPointer<CubesProperties::PropertiesItemsManager> propertiesItemsManager_;
    QPointer<CubesAnalysis::AnalysisManager> analysisManager_;

    // Значения имен параметров
    CubesUnitTypes::ParameterModelIds ids_;

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

public:
    // ITopManager
    bool GetUnitsInFileList(const CubesUnitTypes::FileId& fileId, QStringList& unitNames) override;
    bool GetUnitsInFileIncludeList(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId includeId,
        QStringList& unitNames) override;
    bool GetUnitParameters(const QString& unitId, CubesUnitTypes::UnitParameters& unitParameters) override;
    bool GetFileIncludeList(const CubesUnitTypes::FileId& fileId, CubesUnitTypes::IncludeIdNames& includeNames) override;
    bool GetFileIncludeVariableList(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId includeId,
        CubesUnitTypes::VariableIdVariables& variables) override;
    bool CreatePropetiesItem(const QString& unitId, CubesUnitTypes::PropertiesId& propertiesId) override;
    bool GetPropetiesForDrawing(const CubesUnitTypes::PropertiesId propertiesId, PropertiesForDrawing& pfd) override;
    bool GetPropetiesUnitParameters(const CubesUnitTypes::PropertiesId propertiesId, CubesUnitTypes::UnitParameters& unitParameters) override;
    bool GetPropetiesUnitId(const CubesUnitTypes::PropertiesId propertiesId, QString& unitId) override;
    bool GetUnitsConnections(QMap<QString, QStringList>& connections) override;
    bool GetDependsConnections(QMap<QString, QStringList>& connections) override;
    bool CreateDiagramItem(CubesUnitTypes::PropertiesId propertiesId, const PropertiesForDrawing& pfd, QPointF pos) override { return true; };
    bool EnshureVisible(CubesUnitTypes::PropertiesId propertiesId) override;
    bool GetAnalysisFiles(QVector<CubesAnalysis::File>& files) override;
    bool GetAnalysisProperties(QVector<CubesAnalysis::Properties>& properties) override;

    // ILogManager
    void AddMessage(const CubesLog::LogMessage& m);

protected:
    void FillParametersInfo();

    // Units
    bool AddMainFile(const CubesXml::File& file, const QString& zipFileName);
    bool AddUnits(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId includeId, const CubesXml::File& file);
    bool SortUnits();
    bool SortUnitsRectangular(bool check);
    CubesUnitTypes::UnitParameters* GetUnitParameters(const QString& id);
    bool Test();

    // Files
    CubesUnitTypes::FileIdNames GetFileNames();
    CubesUnitTypes::IncludeIdNames GetCurrentFileIncludeNames();

    // Modified
    void UpdateFileState(const QString& path, bool modified);
    bool SaveFileInternal(const QString& path);
    bool SaveFolderInternal(const QString& path);
    bool OpenFileInternal(const QString& path);
    bool OpenFolderInternal(const QString& path);
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
    void PropertiesBasePropertiesChanged(const uint32_t propertiesId, const QString& name,
        const QString& fileName, const QString& includeName);
    void PropertiesSelectedItemChanged(const uint32_t propertiesId);
    void PropertiesPositionChanged(const uint32_t propertiesId, double posX, double posY, double posZ);
    void PropertiesError(const uint32_t propertiesId, const QString& message);
    void PropertiesConnectionChanged(const uint32_t propertiesId);
    void PropertiesPropertiesChanged();

private slots:
    // Кнопки
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

    // Лог
    void OnErrorButtonClicked(bool checked);
    void OnWarningButtonClicked(bool checked);
    void OnInformationButtonClicked(bool checked);
    void OnDoubleClicked(const QModelIndex& index);
};
