#pragma once

#include <QWidget>
#include <QPointer>
#include <QMap>
#include <QSet>
#include <QPoint>
#include <QMainWindow>
#include <QVector>

#include "parameters_compiler_types.h"
#include "files_item.h"
#include "xml_parser.h"
#include "unit_types.h"

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

private:
    QVector<QColor> defaultColors_ = { QColor("Red"), QColor("Green"), QColor("Blue"), QColor("Orange"), QColor("Violet"), QColor("Yellow") };
    int defaultColorIndex_;

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
    void CreateScene(int index);
    void CreateView(int index);
    void CreateFilesPropertyBrowser();
    void CreatePropertyBrowser();
    void CreateTreeView();
    QWidget* CreatePropertiesPanelWidget();
    QWidget* CreateFilesPropertiesWidget();
    QWidget* CreatePropertiesWidget();
    QWidget* CreateFilesButtonsWidget();
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
    QColor GetFileColor(const QString& fileId);
    QString GetNewUnitName(const QString& baseName, const QString& groupName);
    QString GetDisplayName(const QString& baseName, const QString& groupName);
    QString GetCurrentGroup();
    void ActivateGroup(const QString& groupName);

private:
    QVector<QPair<diagram_scene*, diagram_view*>> panes_;
    //diagram_scene* scene_;
    //QPointer<diagram_view> view_;
    QPointer<QTreeView> tree_view_;
    QPointer<QTableView> table_view_log_;
    QPointer<QTreeView> tree_;
    QTabWidget* tabWidget_;
    QPlainTextEdit* plainTextEditHint_;
    QMap<QString, unit_types::UnitParameters> unitParameters_;
    QtTreePropertyBrowser* filesPropertyEditor_;
    QtTreePropertyBrowser* propertyEditor_;
    QList<files_item*> files_items_;
    QComboBox* comboBoxFiles_;
    QComboBox* comboBoxUnits_;

private slots:
    void on_NewFile_action();
    void on_OpenFile_action();
    void on_ImportXmlFile_action();
    void on_SaveFile_action();
    void on_SaveAsFile_action();
    void on_Quit_action();
    void on_Sort_action();

    void on_AddFile_clicked();
    void on_RemoveFile_clicked();
    void on_Files_currentIndexChanged(int index);
    void on_Units_currentIndexChanged(int index);

    void currentItemChanged(QtBrowserItem* item);
    void MyFirstBtnClicked();

    void selectionChanged();
    void test2(QPointF ppp);

    void on_Tab_currentChanged(int index);

public slots:
    void itemPositionChanged(diagram_item* item);
    void afterItemCreated(diagram_item* item);
    void beforeItemDeleted(diagram_item* item);
    void itemNameChanged(diagram_item* item, QString oldName);
    void itemFileChanged(diagram_item* item);

    void collapsed(QtBrowserItem* item);
    void expanded(QtBrowserItem* item);
};
