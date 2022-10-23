#pragma once

#include <QWidget>
#include <QPointer>
#include <QMap>

class QGraphicsScene;
class QGraphicsView;
class QTableView;
class QTreeView;
class QToolBox;
class diagram_view;
class QSplitter;
class QtProperty;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void CreateUi();
    void CreateToolBox();
    void CreateTreeView();

private:
    QPointer<QToolBox> tool_box_;
    QPointer<QGraphicsScene> sp_scene_;
    QPointer<diagram_view> view_;
    QPointer<QTreeView> tree_view_;
    QPointer<QTableView> table_view_log_;
    QPointer<QTableView> table_view_info_;
    QPointer<QTableView> table_view_properties_;
    QPointer<QSplitter> splitter_tool_box_;
    QPointer<QSplitter> splitter_log_;
    QPointer<QSplitter> splitter_info_;
    QPointer<QSplitter> splitter_info_properties_;

private slots:
    void valueChanged(QtProperty *property, double value);
    void valueChanged(QtProperty *property, const QString &value);
    void valueChanged(QtProperty *property, const QColor &value);
    void valueChanged(QtProperty *property, const QFont &value);
    void valueChanged(QtProperty *property, const QPoint &value);
    void valueChanged(QtProperty *property, const QSize &value);

private:
    QMap<QtProperty*, QString> propertyToId;
    QMap<QString, QtProperty*> idToProperty;
    QMap<QString, bool> idToExpanded;

private:
    void addProperty(QtProperty *property, const QString &id);
    void updateExpandState();
    class QtDoublePropertyManager *doubleManager;
    class QtStringPropertyManager *stringManager;
    class QtColorPropertyManager *colorManager;
    class QtFontPropertyManager *fontManager;
    class QtPointPropertyManager *pointManager;
    class QtSizePropertyManager *sizeManager;
    class QtTreePropertyBrowser *propertyEditor;
};
