#pragma once

#include <QWidget>
#include <QPointer>
#include <QMap>
#include <QSet>
#include <QPoint>
#include <QMainWindow>

#include "parameters_compiler_types.h"
#include "files_item.h"
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

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    bool modified_;

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void CreateUi();
    void CreateMenu();
    QWidget* CreateMainWidget();
    QWidget* CreateMainTabWidget();
    void CreateScene();
    void CreateView();
    void CreateFilesPropertyBrowser();
    void CreatePropertyBrowser();
    void CreateTreeView();
    QWidget* CreatePropertiesPanelWidget();
    QWidget* CreateFilesPropertiesWidget();
    QWidget* CreatePropertiesWidget();
    QWidget* CreateHostsButtonsWidget();
    QWidget* CreateHintWidget();
    void FillTreeView();
    void FillParametersInfo();

public:
    unit_types::UnitParameters GetUnitParameters(QString id) const;

private:
    diagram_scene* scene_;
    QPointer<diagram_view> view_;
    QPointer<QTreeView> tree_view_;
    QPointer<QTableView> table_view_log_;
    QPointer<QTreeView> tree_;
    QTabWidget* tabWidget_;
    QPlainTextEdit* plainTextEditHint_;
    QMap<QString, unit_types::UnitParameters> unitParameters_;
    QtTreePropertyBrowser* filesPropertyEditor_;
    QtTreePropertyBrowser* propertyEditor_;
    QList<files_item*> files_items_;

private slots:
    void on_NewFile_action();
    void on_OpenFile_action();
    void on_SaveFile_action();
    void on_SaveAsFile_action();
    void on_Quit_action();

    void on_AddHost_clicked();
    void on_RemoveHost_clicked();

    void currentItemChanged(QtBrowserItem* item);
    void MyFirstBtnClicked();

    void selectionChanged();
    void test2(QPointF ppp);

public slots:
    void itemPositionChanged(QString id, QPointF newPos);
    void itemCreated(QString id, diagram_item* item);

    void collapsed(QtBrowserItem* item);
    void expanded(QtBrowserItem* item);
};
