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
    uint32_t uniqueNumber_;

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

    // �������� ���� ����������
    CubesUnitTypes::ParameterModelIds ids_;





    CubesUnitTypes::ParameterModel pm_{};
    QSharedPointer<CubesProperties::PropertiesItem> pi_;
    parameters::restrictions_info ri_{};



public:
    explicit ArrayWindow(QWidget* parent = nullptr);
    ~ArrayWindow() override;

public:
    // ITopManager
    void GetUnitsInFileList(const CubesUnitTypes::FileId& fileId, QStringList& unitNames) override;
    void GetUnitsInFileIncludeList(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId includeId,
        QStringList& unitNames) override;
    void GetUnitParameters(const QString& unitId, CubesUnitTypes::UnitParameters& unitParameters) override;
    void GetFileIncludeList(const CubesUnitTypes::FileId& fileId, CubesUnitTypes::IncludeIdNames& includeNames) override;
    void GetFileIncludeVariableList(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId includeId,
        CubesUnitTypes::VariableIdVariables& variables) override;
    QMap<QString, QStringList> GetUnitsConnections() override;
    QMap<QString, QStringList> GetDependsConnections() override;
    // ISimpleTopManager
    bool CreatePropetiesItem(const QString& unitId, uint32_t& propertiesId) override;
    bool GetPropetiesForDrawing(const uint32_t propertiesId, PropertiesForDrawing& pfd) override;
    bool GetPropetiesUnitParameters(const uint32_t propertiesId, CubesUnitTypes::UnitParameters& unitParameters) override;
    bool GetPropetiesUnitId(const uint32_t propertiesId, QString& unitId) override;
    QString GetNewUnitName(const QString& baseName) override;

    bool CreateDiagramItem(uint32_t propertiesId, const PropertiesForDrawing& pfd, QPointF pos) override;
    void EnshureVisible(uint32_t propertiesId) override;

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
    bool SortUnitsRectangular(bool check);
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
    void FileNameChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::FileId& oldFileId);
    void FileListChanged(const CubesUnitTypes::FileIdNames& fileIdNames);
    void FileIncludeNameChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId& includeId,
        const CubesUnitTypes::IncludeId& oldIncludeId);
    void FileIncludesListChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeIdNames& includeIdNames);
    void FileVariableNameChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId& includeId,
        const QString& variableName, const QString& oldVariableName);
    void FileVariablesListChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId& includeId,
        const CubesUnitTypes::VariableIdVariables& variables);
    void FileColorChanged(const CubesUnitTypes::FileId& fileId, const QColor& color);

    // PropertiesItemsManager
    void PropertiesBasePropertiesChanged(const uint32_t propertiesId, const QString& name, const QString& fileName,
        const QString& includeName);
    void PropertiesSelectedItemChanged(const uint32_t propertiesId);
    void PropertiesPositionChanged(const uint32_t propertiesId, double posX, double posY, double posZ);
    void PropertiesError(const uint32_t propertiesId, const QString& message);

private slots:
    // ������
    void OnNewFileAction();
    void OnOpenFileAction();
    void OnImportXmlFileAction();
    void OnSaveFileAction();
    void OnSaveAsFileAction();
    void OnQuitAction();
    void OnSortBoostAction();
    void OnSortRectAction();

    // TODO: ��������� ��������� � ��������
    //void currentItemChanged(QtBrowserItem* item);

    // ���
    void OnErrorButtonClicked(bool checked);
    void OnWarningButtonClicked(bool checked);
    void OnInformationButtonClicked(bool checked);
};
