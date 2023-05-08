#pragma once

#include <QGraphicsItem>
#include <QObject>
#include <QPixmap>

#include "unit_types.h"
#include "file_items_manager_api.h"
#include "properties_editor.h"

class file_item : public QObject
{
Q_OBJECT

private:
    file_items_manager_interface* file_items_manager_;
    unit_types::ParametersModel parametersModel_;

    bool ignoreEvents_;

public:
    file_item(file_items_manager_interface* file_items_manager);

private:
    void CreateParametersModel();
    void CreateParameterModel(const parameters_compiler::parameter_info pi, const QString& parent_model_id, unit_types::ParameterModel& model);
    void UpdateArrayModel(unit_types::ParameterModel& pm);
    void CreateEditorModel();
    QtProperty* GetPropertyForModel(unit_types::ParameterModel& model);
    void CreatePropertyBrowser();

private slots:
    //void valueChanged(QtProperty* property, int value);
    //void valueChanged(QtProperty* property, double value);
    //void valueChanged(QtProperty* property, const QString& value);
    //void valueChanged(QtProperty* property, const QColor& value);
    //void valueChanged(QtProperty* property, const QFont& value);
    //void valueChanged(QtProperty* property, const QPoint& value);
    //void valueChanged(QtProperty* property, const QSize& value);
    //void valueChanged(QtProperty* property, bool value);
    //void IntValueChanged(QtProperty* property, int value);
    //void DoubleValueChanged(QtProperty* property, double value);
    //void StringValueChanged(QtProperty* property, const QString& value);
    //void StringEditingFinished(QtProperty* property, const QString& value, const QString& oldValue);
    //void EnumValueChanged(QtProperty* property, int value);
    //void BoolValueChanged(QtProperty* property, bool value);
    //void ColorValueChanged(QtProperty* property, const QColor& value);
    void ValueChanged(QtProperty* property, const QVariant& value);

public:
    void Select(QSharedPointer<properties_editor> editor);
    void UnSelect(QSharedPointer<properties_editor> editor);

    QString GetName();
    QColor GetColor();
    void ApplyToBrowser(QtTreePropertyBrowser* propertyEditor);

    //void UpdateRegExp(QtBrowserItem* index);
    //void UpdateRegExp();

    QPixmap GetPixmap();
    void PositionChanged(QPointF point);
    void ZOrderChanged(double value);
    QString GetPropertyDescription(const QtProperty* property);
    void ExpandedChanged(const QtProperty* property, bool is_expanded);
    void SetName(QString name);
    void SetColor(QColor color);
    QStringList GetIncludeNames();
    void SetNameRegExp(QString regexp);

private:
    QMap<const QtProperty*, QString> propertyToId;
    QMap<QString, const QtProperty*> idToProperty;

private:
    void RegisterProperty(const QtProperty* property, const QString& id);
    void UnregisterProperty(const QString& id);
    void UnregisterProperty(const QtProperty* property);
    QtProperty* GetProperty(const QString& id);
    QString GetPropertyId(const QtProperty* property);
    bool GetExpanded(const QtProperty* property);
    unit_types::ParameterModel* GetParameterModel(const QString& id);
    unit_types::ParameterModel* GetParameterModel(const QtProperty* property);

    //void SaveExpandState();
    //void SaveExpandState(QtBrowserItem* index);
    //void ApplyExpandState();
    //void ApplyExpandState(QtBrowserItem* index);
    void UpdateIncludeNameRegExp();
};
