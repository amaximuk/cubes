#pragma once

#include <QMainWindow>
#include <QPointer>
#include "properties_item/properties_item.h" // !!!!!!!!!!!!!!! used directly not top_manager_interface
#include "top_manager_interface.h"

namespace CubeDiagram
{
    class DiagramItem;
    class DiagramScene;
    class DiagramView;
}

namespace CubesUnitTypes
{
    struct UnitParameters;
}

namespace CubeLog
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

class ArrayWindow : public QMainWindow, ITopManager
{
    Q_OBJECT

private:
    bool modified_;
    uint32_t unique_number_;

    QPointer<CubeDiagram::DiagramScene> scene_;
    QPointer<CubeDiagram::DiagramView> view_;
    //QPointer<QTreeView> tree_view_;
    //QPointer<QTableView> table_view_log_;
    //QPointer<QTreeView> tree_;
    //QPointer<QPlainTextEdit> plainTextEditHint_;
    QMap<QString, CubesUnitTypes::UnitParameters> unitParameters_;
    //QPointer<CubeLog::LogTableModel> log_table_model_;
    //QPointer<CubeLog::SortFilterModel> sort_filter_model_;

    //QPointer<CubesFile::FileItemsManager> fileItemsManager_;
    QPointer<CubesProperties::PropertiesItemsManager> propertiesItemsManager_;

    // Значения имен параметров
    CubesUnitTypes::ParameterModelIds ids_;

public:
    explicit ArrayWindow(QWidget* parent = nullptr);
    ~ArrayWindow() override;

public:
    // ITopManager
    void GetUnitsInFileList(const QString& fileName, QStringList& unitNames) override;
    void GetUnitsInFileIncludeList(const QString& fileName, const QString& includeName, QStringList& unitNames) override;
    void GetUnitParameters(const QString& unitId, CubesUnitTypes::UnitParameters& unitParameters) override;
    void GetFileIncludeList(const QString& fileName, QStringList& includeNames) override;
    void GetFileIncludeVariableList(const QString& fileName, const QString& includeName, QList<QPair<QString, QString>>& variables) override;
    QMap<QString, QStringList> GetUnitsConnections() override;
    QMap<QString, QStringList> GetDependsConnections() override;
    // ISimpleTopManager
    bool CreatePropetiesItem(const QString& unitId, uint32_t& propertiesId) override;
    bool GetPropetiesForDrawing(const uint32_t propertiesId, PropertiesForDrawing& pfd) override;
    bool GetPropetiesUnitParameters(const uint32_t propertiesId, CubesUnitTypes::UnitParameters& unitParameters) override;
    bool GetPropetiesUnitId(const uint32_t propertiesId, QString& unitId) override;
    QString GetNewUnitName(const QString& baseName) override;


    void SetItemModel(parameters::file_info afi, CubesUnitTypes::ParameterModel pm, QSharedPointer<CubesProperties::PropertiesItem> pi);

public:
    void closeEvent(QCloseEvent* event) override;

signals:
    void BeforeClose(const bool result, CubesUnitTypes::ParameterModel pm, QSharedPointer<CubesProperties::PropertiesItem> pi);

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
    //QWidget* CreateHintWidget();
    void FillTreeView();
    void FillParametersInfo();

    // Units
    bool AddMainFile(CubesXml::File& file);
    bool AddUnits(const QString& fileName, const QString& includedFileName, const CubesXml::File& file);
    bool SortUnits();
    bool SortUnitsRectangular();
    QMap<QString, QStringList> GetConnectionsInternal(bool depends);
    CubesUnitTypes::UnitParameters* GetUnitParameters(const QString& id);

    // Files
    QStringList GetFileNames();
    QString GetCurrentFileName();
    QStringList GetCurrentFileIncludeNames();
    //QColor GetFileColor(const QString& fileName);
    QString GetDisplayName(const QString& baseName);

public slots:
    // DiagramScene (as manager)
    void DiagramItemPositionChanged(CubeDiagram::DiagramItem* item);
    void DiagramAfterItemCreated(CubeDiagram::DiagramItem* item);
    void DiagramBeforeItemDeleted(CubeDiagram::DiagramItem* item);
    //void DiagramItemNameChanged(CubeDiagram::DiagramItem* item, QString oldName);
    //void DiagramItemFileChanged(CubeDiagram::DiagramItem* item);
    //void DiagramItemGroupChanged(CubeDiagram::DiagramItem* item);
    void selectionChanged(); // QGraphicsScene

    // FileItemsManager
    void FileNameChanged(const QString& fileName, const QString& oldFileName);
    void FileListChanged(const QStringList& fileNames);
    void FileIncludeNameChanged(const QString& fileName, const QString& includeName, const QString& oldIncludeName);
    void FileIncludesListChanged(const QString& fileName, const QStringList& includeNames);
    //void FileVariableChanged(const QString& fileName, const QString& includeName, const QList<QPair<QString, QString>>& variables);
    void FileVariableNameChanged(const QString& fileName, const QString& includeName, const QString& variableName, const QString& oldVariableName);
    void FileVariablesListChanged(const QString& fileName, const QString& includeName, const QList<QPair<QString, QString>>& variables);

    // PropertiesItemsManager
    void PropertiesBasePropertiesChanged(const uint32_t propertiesId, const QString& name, const QString& fileName, const QString& groupName);
    void PropertiesSelectedItemChanged(const uint32_t propertiesId);
    void PropertiesPositionChanged(const uint32_t propertiesId, double posX, double posY, double posZ);
    void PropertiesOnError(const uint32_t propertiesId, const QString& message);

private slots:
    // Кнопки
    void OnNewFileAction();
    void OnOpenFileAction();
    void OnImportXmlFileAction();
    void OnSaveFileAction();
    void OnSaveAsFileAction();
    void OnQuitAction();
    void OnSortAction();

    // TODO: Перенести подсказку в менеджер
    //void currentItemChanged(QtBrowserItem* item);

    // Лог
    void OnErrorButtonClicked(bool checked);
    void OnWarningButtonClicked(bool checked);
    void OnInformationButtonClicked(bool checked);
};
