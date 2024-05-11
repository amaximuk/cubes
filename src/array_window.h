#pragma once

#include <QMainWindow>
#include <QPointer>
#include "properties_item/properties_item.h" // !!!!!!!!!!!!!!! used directly not top_manager_interface
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
    //QPointer<QTreeView> tree_view_;
    //QPointer<QTableView> table_view_log_;
    //QPointer<QTreeView> tree_;
    //QPointer<QPlainTextEdit> plainTextEditHint_;
    //QMap<QString, CubesUnitTypes::UnitParameters> unitParameters_;
    //QPointer<CubeLog::LogTableModel> log_table_model_;
    //QPointer<CubeLog::SortFilterModel> sort_filter_model_;

    //QPointer<CubesFile::FileItemsManager> fileItemsManager_;
    //QPointer<CubesProperties::PropertiesItemsManager> propertiesItemsManager_;

    // Значения имен параметров
    //CubesUnitTypes::ParameterModelIds ids_;





    CubesUnitTypes::ParameterModel pm_{};
    QSharedPointer<CubesProperties::PropertiesItem> pi_;
    parameters::restrictions_info ri_{};



public:
    explicit ArrayWindow(QWidget* parent = nullptr);
    ~ArrayWindow() override;

public:
    // ITopManager
    bool GetUnitsInFileList(const CubesUnitTypes::FileId& fileId, QStringList& unitNames) override;
    bool GetUnitsInFileIncludeList(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId includeId,
        QStringList& unitNames) override;
    bool GetUnitParameters(const QString& unitId, CubesUnitTypes::UnitParameters& unitParameters) override;
    bool GetFileIncludeList(const CubesUnitTypes::FileId& fileId, CubesUnitTypes::IncludeIdNames& includeNames) override;
    bool GetFileIncludeVariableList(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId includeId,
        CubesUnitTypes::VariableIdVariables& variables) override;
    bool GetUnitsConnections(QMap<QString, QStringList>& connections) override;
    bool GetDependsConnections(QMap<QString, QStringList>& connections) override;
    bool CreatePropetiesItem(const QString& unitId, uint32_t& propertiesId) override;
    bool GetPropetiesForDrawing(const CubesUnitTypes::PropertiesId propertiesId, CubesTop::PropertiesForDrawing& pfd) override;
    bool GetPropetiesUnitParameters(const CubesUnitTypes::PropertiesId propertiesId,
        CubesUnitTypes::UnitParameters& unitParameters) override;
    bool GetPropetiesUnitId(const CubesUnitTypes::PropertiesId propertiesId, QString& unitId) override;
    bool CreateDiagramItem(CubesUnitTypes::PropertiesId propertiesId) override;
    bool EnshureVisible(CubesUnitTypes::PropertiesId propertiesId) override;
    bool GetAnalysisFiles(QVector<CubesAnalysis::File>& files) override;
    bool GetAnalysisProperties(QVector<CubesAnalysis::Properties>& properties) override;



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
    CubesUnitTypes::UnitParameters* GetUnitParameters(const QString& id);

    // Files
    virtual CubesUnitTypes::FileIdNames GetFileNames();
    virtual CubesUnitTypes::IncludeIdNames GetCurrentFileIncludeNames();
    //QStringList GetFileNames();
    //QString GetCurrentFileName();
    //QStringList GetCurrentFileIncludeNames();
    ////QColor GetFileColor(const QString& fileName);
    //QString GetDisplayName(const QString& baseName);

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
    void PropertiesBasePropertiesChanged(const uint32_t propertiesId, const QString& name,
        const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId includeId);
    void PropertiesSelectedItemChanged(const uint32_t propertiesId);
    void PropertiesPositionChanged(const uint32_t propertiesId, double posX, double posY, double posZ);
    void PropertiesError(const uint32_t propertiesId, const QString& message);

private slots:
    // Кнопки
    void OnNewFileAction();
    void OnOpenFileAction();
    void OnImportXmlFileAction();
    void OnSaveFileAction();
    void OnSaveAsFileAction();
    void OnQuitAction();
    void OnSortBoostAction();
    void OnSortRectAction();

    // TODO: Перенести подсказку в менеджер
    //void currentItemChanged(QtBrowserItem* item);

    // Лог
    void OnErrorButtonClicked(bool checked);
    void OnWarningButtonClicked(bool checked);
    void OnInformationButtonClicked(bool checked);
};
