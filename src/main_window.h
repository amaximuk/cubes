#pragma once

#include <QWidget>
#include <QPointer>
#include <QMap>
#include <QSet>
#include <QPoint>
#include <QMainWindow>
#include <QVector>

#include "parameters_compiler/parameters_compiler_types.h"
#include "file_item/file_item.h"
#include "xml_parser.h"
#include "unit_types.h"
#include "log_table/log_table_model.h"
#include "log_table/sort_filter_model.h"
#include "file_item/file_items_manager.h"
#include "properties_item/properties_items_manager.h"
#include "property_browser/properties_editor.h"
#include "top_manager_interface.h"

class QPlainTextEdit;
class QGraphicsScene;
class QGraphicsView;
class QTableView;
class QTreeView;
class QToolBox;
class DiagramView;
class QSplitter;
class QtProperty;
class DiagramScene;
class DiagramItem;
class QtTreePropertyBrowser;
class QtBrowserItem;
class QComboBox;

class MainWindow : public QMainWindow, ITopManager
{
    Q_OBJECT

private:
    bool modified_;
    uint32_t unique_number_;

    QPointer<DiagramScene> scene_;
    QPointer<DiagramView> view_;
    QPointer<QTreeView> tree_view_;
    QPointer<QTableView> table_view_log_;
    QPointer<QTreeView> tree_;
    QPointer<QPlainTextEdit> plainTextEditHint_;
    QMap<QString, unit_types::UnitParameters> unitParameters_;
    QPointer<CubeLog::LogTableModel> log_table_model_;
    QPointer<CubeLog::SortFilterModel> sort_filter_model_;

    QPointer<CubesFile::FileItemsManager> fileItemsManager_;
    QPointer<CubesProperties::PropertiesItemsManager> propertiesItemsManager_;

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void CreateUi();
    void CreateMenu();
    QWidget* CreateMainWidget();
    QWidget* CreateLogWidget();
    void CreateScene();
    void CreateView();
    void CreateTreeView();
    QWidget* CreatePropertiesPanelWidget();
    QWidget* CreateHintWidget();
    void FillTreeView();
    void FillParametersInfo();

    bool AddUnits(const QString& fileName, const xml::File& file);
    bool SortUnits();
    bool AddMainFile(xml::File& file);

public:
    unit_types::UnitParameters* GetUnitParameters(const QString& id);
    QStringList GetFileNames();
    QString GetCurrentFileName();
    QStringList GetCurrentFileIncludeNames();
    QColor GetFileColor(const QString& fileName);
    QString GetNewUnitName(const QString& baseName);
    QString GetDisplayName(const QString& baseName);
    QMap<QString, QStringList> GetUnitsConnections();
    QMap<QString, QStringList> GetDependsConnections();

    // ITopManager
    void GetUnitsInFileList(const QString& fileName, QStringList& unitNames) override;
    void GetUnitsInFileIncludeList(const QString& fileName, const QString& includeName, QStringList& unitNames) override;
    void GetUnitParameters(const QString& unitId, unit_types::UnitParameters& unitParameters) override;
    void GetFileIncludeList(const QString& fileName, QStringList& includeNames) override;
    void GetFileIncludeVariableList(const QString& fileName, const QString& includeName, QList<QPair<QString, QString>>& variables) override;

    // to add to interface
    bool CreatePropetiesItem(const QString& unitId, uint32_t& propertiesId);
    bool GetPropeties(const uint32_t propertiesId, PropertiesForDrawing& pfd);

private:
    QMap<QString, QStringList> GetConnectionsInternal(bool depends);

private slots:
    void on_NewFile_action();
    void on_OpenFile_action();
    void on_ImportXmlFile_action();
    void on_SaveFile_action();
    void on_SaveAsFile_action();
    void on_Quit_action();
    void on_Sort_action();
    void on_RemoveGroup_clicked();
    void on_Units_currentIndexChanged(int index);

    void currentItemChanged(QtBrowserItem* item);
    void selectionChanged();
    void test2(QPointF ppp);

    void on_ErrorButton_clicked(bool checked);
    void on_WarningButton_clicked(bool checked);
    void on_InformationButton_clicked(bool checked);
    void on_DeleteFileInclude_action(bool checked = false);

public slots:
    void itemPositionChanged(DiagramItem* item);
    void afterItemCreated(DiagramItem* item);
    void beforeItemDeleted(DiagramItem* item);
    void itemNameChanged(DiagramItem* item, QString oldName);
    void itemFileChanged(DiagramItem* item);
    void showFileContextMenu(const QPoint& pos);

    // FileItem::FileItemsManager
    void fileNameChanged(const QString& fileName, const QString& oldFileName);
    void fileListChanged(const QStringList& fileNames);
    void fileIncludeNameChanged(const QString& fileName, const QString& includeName, const QString& oldIncludeName);
    void fileIncludesListChanged(const QString& fileName, const QStringList& includeNames);
    // PropertiesItemsManager
    void propertiesBasePropertiesChanged(const uint32_t propertiesId, const QString& name, const QString& fileName, const QString& groupName);
    //void propertiesFileNameChanged(const uint32_t propertiesId, const QString& fileName);
    void propertiesSelectedItemChanged(const uint32_t propertiesId);
};
