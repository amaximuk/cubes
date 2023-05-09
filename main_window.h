#pragma once

#include <QWidget>
#include <QPointer>
#include <QMap>
#include <QSet>
#include <QPoint>
#include <QMainWindow>
#include <QVector>

#include "parameters_compiler_types.h"
#include "file_item.h"
#include "xml_parser.h"
#include "unit_types.h"
#include "log_table_model.h"
#include "sort_filter_model.h"
#include "file_items_manager.h"
#include "properties_editor.h"

class QPlainTextEdit;
class QGraphicsScene;
class QGraphicsView;
class QTableView;
class QTreeView;
class QToolBox;
class diagram_view;
class QSplitter;
class QtProperty;
class diagram_scene;
class diagram_item;
class QtTreePropertyBrowser;
class QtBrowserItem;
class QComboBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

//private:
//    QVector<QColor> defaultColorsFile_ = { QColor("Red"), QColor("Green"), QColor("Blue"), QColor("Orange"), QColor("Violet"), QColor("Yellow") };
//    int defaultColorFileIndex_;
//    QVector<QColor> defaultColorsGroup_ = { QColor("Red"), QColor("Green"), QColor("Blue"), QColor("Orange"), QColor("Violet"), QColor("Yellow") };
//    int defaultColorGroupIndex_;

private:
    bool modified_;

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void CreateUi();
    void CreateMenu();
    QWidget* CreateMainWidget();
    QWidget* CreateTabWidget(int index);
    QWidget* CreateLogWidget();
    void CreateScene(int index);
    void CreateView(int index);
    //void CreateFilesPropertyBrowser();
    void CreatePropertyBrowser();
    void CreateTreeView();
    QWidget* CreatePropertiesPanelWidget();
    //QWidget* CreateFilesPropertiesWidget();
    QWidget* CreatePropertiesWidget();
    //QWidget* CreateFilesButtonsWidget();
    QWidget* CreateUnitsButtonsWidget();
    QWidget* CreateHintWidget();
    void FillTreeView();
    void FillParametersInfo();

    bool AddUnits(const QString& groupName, const QString& fileName, const xml::File& file);
    bool SortUnits(const QString& groupName);
    bool AddMainFile(xml::File& file);

public:
    unit_types::UnitParameters* GetUnitParameters(const QString& id);
    QStringList GetFileNames();
    QString GetCurrentFileName();
    QStringList GetCurrentFileIncludeNames();
    QColor GetFileColor(const QString& fileName);
    //QStringList GetFileGroups(const QString& fileId);
    //QColor GetGroupColor(const QString& groupId);
    //QString GetGroupFile(const QString& groupId);
    QString GetNewUnitName(const QString& baseName, const QString& groupName);
    QString GetDisplayName(const QString& baseName, const QString& groupName);
    QString GetCurrentGroup();
    void ActivateGroup(const QString& groupName);
    //void AddLogMessage(QString message);
    QMap<QString, QStringList> GetGroupUnitsConnections(const QString& groupName);
    QMap<QString, QStringList> GetGroupDependsConnections(const QString& groupName);
    QMap<QString, QStringList> GetGroupUnitsConnections(int groupId);
    QMap<QString, QStringList> GetGroupDependsConnections(int groupId);
    int GetTabIndex(const QString& groupName);
    diagram_item* GetGroupItem(const QString& groupName);

private:
    QStringList GetGroupUnitsNames(const QString& groupName);
    QStringList GetGroupConnectedNames(const QString& groupName, bool depends);
    QMap<QString, QStringList> GetGroupConnectionsInternal(const QString& groupName, bool depends);

private:
    QVector<QPair<QPointer<diagram_scene>, QPointer<diagram_view>>> panes_;
    //diagram_scene* scene_;
    //QPointer<diagram_view> view_;
    QPointer<QTreeView> tree_view_;
    QPointer<QTableView> table_view_log_;
    QPointer<QTreeView> tree_;
    QPointer<QTabWidget> tabWidget_;
    QPointer<QPlainTextEdit> plainTextEditHint_;
    QMap<QString, unit_types::UnitParameters> unitParameters_;
    //QPointer<properties_editor> filePropertiesEditor_;
    //QPointer<QtTreePropertyBrowser> filesPropertyEditor_;
    //QPointer<QtTreePropertyBrowser> groupsPropertyEditor_;
    QPointer<QtTreePropertyBrowser> propertyEditor_;
    //QPointer<QComboBox> comboBoxFiles_;
    //QPointer<QComboBox> comboBoxGroups_;
    QPointer<QComboBox> comboBoxUnits_;
    QPointer<log_table_model> log_table_model_;
    QPointer<sort_filter_model> sort_filter_model_;
    QPointer<file_items_manager> file_items_manager_;

private slots:
    void on_NewFile_action();
    void on_OpenFile_action();
    void on_ImportXmlFile_action();
    void on_SaveFile_action();
    void on_SaveAsFile_action();
    void on_Quit_action();
    void on_Sort_action();

    //void on_AddFile_clicked();
    //void on_RemoveFile_clicked();
    //void on_Files_currentIndexChanged(int index);
    void on_RemoveGroup_clicked();
    void on_Units_currentIndexChanged(int index);

    void currentItemChanged(QtBrowserItem* item);

    void selectionChanged();
    void test2(QPointF ppp);

    void on_Tab_currentChanged(int index);

    void on_ErrorButton_clicked(bool checked);
    void on_WarningButton_clicked(bool checked);
    void on_InformationButton_clicked(bool checked);

    void on_DeleteFileInclude_action(bool checked = false);

public slots:
    void itemPositionChanged(diagram_item* item);
    void afterItemCreated(diagram_item* item);
    void beforeItemDeleted(diagram_item* item);
    void itemNameChanged(diagram_item* item, QString oldName);
    void itemFileChanged(diagram_item* item);
    void itemGroupChanged(diagram_item* item);

    //void collapsed(QtBrowserItem* item);
    //void expanded(QtBrowserItem* item);

    void showFileContextMenu(const QPoint& pos);

    void fileNameChanged(QString fileName, QString oldFileName);
    void fileIncludeChanged(QString fileName, QStringList includeNames);
    void fileIncludeNameChanged(QString fileName, QString includeName, QString oldIncludeName);
};
