#ifndef PROPERTIES_ITEM_H
#define PROPERTIES_ITEM_H

#include <QGraphicsItem>
#include <QObject>
#include <QPixmap>

#include "unit_types.h"
class diagram_item;

class QtGroupPropertyManager;
class QtIntPropertyManager;
class QtDoublePropertyManager;
class QtStringPropertyManager;
class QtEnumPropertyManager;
class QtColorPropertyManager;
class QtFontPropertyManager;
class QtPointPropertyManager;
class QtSizePropertyManager;
class QtBoolPropertyManager;
class QtProperty;
class QtTreePropertyBrowser;

class properties_item : public QObject
{
    Q_OBJECT

private:
    unit_types::UnitParameters unitParameters_;
    unit_types::ParametersModel parametersModel_;
    //unit_types::ParametersModel editorModel_;

    QtGroupPropertyManager *groupManager;
    QtIntPropertyManager *intManager;
    QtDoublePropertyManager *doubleManager;
    QtStringPropertyManager *stringManager;
    QtEnumPropertyManager *enumManager;
    QtColorPropertyManager *colorManager;
    QtFontPropertyManager *fontManager;
    QtPointPropertyManager *pointManager;
    QtSizePropertyManager *sizeManager;
    QtBoolPropertyManager* boolManager;

    diagram_item* diagramItem_;

public:
    properties_item(unit_types::UnitParameters unitParameters, diagram_item* diagramItem, QObject* parent = nullptr);

private:
    void CreateParametersModel();
    void CreateParameterModel(const parameters_compiler::parameter_info pi, const QString& parent_model_id, unit_types::ParameterModel& model);
    void UpdateArrayModel(unit_types::ParameterModel& pm);
    void CreateEditorModel();
    QtProperty* GetPropertyForModel(unit_types::ParameterModel& model);
    void CreatePropertyBrowser();

private slots:
    void valueChanged(QtProperty* property, int value);
    void valueChanged(QtProperty* property, double value);
    void valueChanged(QtProperty* property, const QString& value);
    void valueChanged(QtProperty* property, const QColor& value);
    void valueChanged(QtProperty* property, const QFont& value);
    void valueChanged(QtProperty* property, const QPoint& value);
    void valueChanged(QtProperty* property, const QSize& value);
    void valueChanged(QtProperty* property, bool value);

public:
    QString getName() { return QString::fromStdString(unitParameters_.fiileInfo.info.id); };
    void ApplyToBrowser(QtTreePropertyBrowser* propertyEditor);
    QPixmap GetPixmap();
    void PositionChanged(QPointF point);
    void ZOrderChanged(double value);
    QString GetPropertyDescription(QtProperty* property);

private:
    QMap<QtProperty*, QString> propertyToId;
    QMap<QString, QtProperty*> idToProperty;
    QMap<QString, bool> idToExpanded;

private:
    void RegisterProperty(QtProperty* property, const QString& id);
    void UnregisterProperty(const QString& id);
    void UnregisterProperty(QtProperty* property);
    QtProperty* GetProperty(const QString& id);
    QString GetPropertyId(QtProperty* property);
    bool GetExpanded(QtProperty* property);
    unit_types::ParameterModel* GetParameterModel(const QString& id);
    unit_types::ParameterModel* GetParameterModel(QtProperty* property);

public:
    void updateExpandState(QtTreePropertyBrowser* propertyEditor);
    void applyExpandState(QtTreePropertyBrowser* propertyEditor);
};

#endif // PROPERTIES_ITEM_H
