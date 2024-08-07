#pragma once

#include <QPointer>
#include "../unit/unit_parameter_model.h"

class QtProperty;
class QtBrowserItem;
class QtTreePropertyBrowser;
class QtGroupPropertyManager;
class QtIntPropertyManager;
class QtDoublePropertyManager;
class QtExStringPropertyManager;
class QtEnumPropertyManager;
class QtBoolPropertyManager;
class QtColorPropertyManager;

class PropertiesEditor : public QObject
{
    Q_OBJECT

private:
    QPointer<QtTreePropertyBrowser> propertyEditor_;

    QPointer<QtGroupPropertyManager> groupManager_;
    QPointer<QtIntPropertyManager> intManager_;
    QPointer<QtDoublePropertyManager> doubleManager_;
    QPointer<QtExStringPropertyManager> stringManager_;
    QPointer<QtEnumPropertyManager> enumManager_;
    QPointer<QtBoolPropertyManager> boolManager_;
    QPointer<QtColorPropertyManager> colorManager_;

public:
    PropertiesEditor();

public:
    QtTreePropertyBrowser* GetPropertyEditor();
    QtProperty* CreatePropertyForModel(CubesUnit::ParameterModelPtr parameterModelPtr,
        QMap<CubesUnit::ParameterModelId, const QtProperty*>& idToProperty);
    void SetPropertyValue(QtProperty* property, CubesUnit::ParameterModelPtr parameterModelPtr);
    void SetIntValue(QtProperty* property, int value);
    void SetDoubleValue(QtProperty* property, double value);
    void SetStringValue(QtProperty* property, const QString& value, bool setOldValue = false, const QString& oldValue = "");
    void SetEnumValue(QtProperty* property, int value);
    void SetEnumValues(QtProperty* property, QStringList values);
    //void SetEnumValue(QtProperty* property, QVariant value);
    void SetBoolValue(QtProperty* property, bool value);
    void SetColorValue(QtProperty* property, QColor value);
    void SetReadOnly(QtProperty* property, bool readOnly);

    int GetEnumValue(QtProperty* property);

signals:
    void Collapsed(QtBrowserItem* item);
    void Expanded(QtBrowserItem* item);
    void ContextMenuRequested(const QPoint& pos);
    void CurrentItemChanged(QtBrowserItem* item);

    void IntValueChanged(QtProperty* property, int value);
    void DoubleValueChanged(QtProperty* property, double value);
    void StringValueChanged(QtProperty* property, const QString& value);
    void StringEditingFinished(QtProperty* property, const QString& value, const QString& oldValue);
    void EnumValueChanged(QtProperty* property, int value);
    void BoolValueChanged(QtProperty* property, bool value);
    void ColorValueChanged(QtProperty* property, const QColor& value);

    void ValueChanged(QtProperty* property, const QVariant& value);

private:
    void CollapsedInternal(QtBrowserItem* item);
    void ExpandedInternal(QtBrowserItem* item);
    void ContextMenuRequestedInternal(const QPoint& pos);
    void CurrentItemChangedInternal(QtBrowserItem* item);
    void IntValueChangedInternal(QtProperty* property, int value);
    void DoubleValueChangedInternal(QtProperty* property, double value);
    void StringValueChangedInternal(QtProperty* property, const QString& value);
    void StringEditingFinishedInternal(QtProperty* property, const QString& value, const QString& oldValue);
    void EnumValueChangedInternal(QtProperty* property, int value);
    void BoolValueChangedInternal(QtProperty* property, bool value);
    void ColorValueChangedInternal(QtProperty* property, const QColor& value);
};
