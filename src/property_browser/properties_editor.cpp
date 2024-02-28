#pragma once

#include <QPointer>
#include <QDebug>
#include "qttreepropertybrowser.h"
#include "qtpropertybrowser.h"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qtexpropertymanager.h"
#include "qtexeditorfactory.h"
#include "../unit_types.h"
#include "properties_editor.h"

PropertiesEditor::PropertiesEditor()
{
    propertyEditor_ = new QtTreePropertyBrowser();
    propertyEditor_->setContextMenuPolicy(Qt::CustomContextMenu);

    qDebug() << connect(propertyEditor_, &QtTreePropertyBrowser::collapsed, this, &PropertiesEditor::CollapsedInternal);
    qDebug() << connect(propertyEditor_, &QtTreePropertyBrowser::expanded, this, &PropertiesEditor::ExpandedInternal);
    qDebug() << connect(propertyEditor_, &QWidget::customContextMenuRequested, this, &PropertiesEditor::ContextMenuRequestedInternal);
    qDebug() << connect(propertyEditor_, &QtTreePropertyBrowser::currentItemChanged, this, &PropertiesEditor::CurrentItemChangedInternal);

    groupManager_ = new QtGroupPropertyManager(this);
    intManager_ = new QtIntPropertyManager(this);
    doubleManager_ = new QtDoublePropertyManager(this);
    stringManager_ = new QtExStringPropertyManager(this);
    enumManager_ = new QtEnumPropertyManager(this);
    boolManager_ = new QtBoolPropertyManager(this);
    colorManager_ = new QtColorPropertyManager(this);

    qDebug() << connect(intManager_, &QtIntPropertyManager::valueChanged, this, &PropertiesEditor::IntValueChangedInternal);
    qDebug() << connect(doubleManager_, &QtDoublePropertyManager::valueChanged, this, &PropertiesEditor::DoubleValueChangedInternal);
    qDebug() << connect(stringManager_, &QtExStringPropertyManager::valueChanged, this, &PropertiesEditor::StringValueChangedInternal);
    qDebug() << connect(stringManager_, qOverload<QtProperty*, const QString&, const QString&>(&QtExStringPropertyManager::editingFinished),
        this, &PropertiesEditor::StringEditingFinishedInternal);
    qDebug() << connect(enumManager_, &QtEnumPropertyManager::valueChanged, this, &PropertiesEditor::EnumValueChangedInternal);
    qDebug() << connect(boolManager_, &QtBoolPropertyManager::valueChanged, this, &PropertiesEditor::BoolValueChangedInternal);
    qDebug() << connect(colorManager_, &QtColorPropertyManager::valueChanged, this, &PropertiesEditor::ColorValueChangedInternal);

    QtSpinBoxFactory* intSpinBoxFactory = new QtSpinBoxFactory(this);
    QtDoubleSpinBoxFactory* doubleSpinBoxFactory = new QtDoubleSpinBoxFactory(this);
    QtExLineEditFactory* lineEditFactory = new QtExLineEditFactory(this);
    QtEnumEditorFactory* comboBoxFactory = new QtEnumEditorFactory(this);
    QtCheckBoxFactory* checkBoxFactory = new QtCheckBoxFactory(this);
    QtColorEditorFactory* colorFactory = new QtColorEditorFactory(this);

    propertyEditor_->setFactoryForManager(intManager_.data(), intSpinBoxFactory);
    propertyEditor_->setFactoryForManager(doubleManager_.data(), doubleSpinBoxFactory);
    propertyEditor_->setFactoryForManager(stringManager_.data(), lineEditFactory);
    propertyEditor_->setFactoryForManager(enumManager_.data(), comboBoxFactory);
    propertyEditor_->setFactoryForManager(boolManager_.data(), checkBoxFactory);
    propertyEditor_->setFactoryForManager(colorManager_.data(), colorFactory);
}

QtTreePropertyBrowser* PropertiesEditor::GetPropertyEditor()
{
    return propertyEditor_;
}

QtProperty* PropertiesEditor::CreatePropertyForModel(const CubesUnitTypes::ParameterModel& model, QMap<QString, const QtProperty*>& idToProperty)
{
    // None, String, SpinInterger, SpinDouble, ComboBox, CheckBox

    QtProperty* pr = nullptr;
    if (model.editorSettings.type == CubesUnitTypes::EditorType::None)
    {
        pr = groupManager_->addProperty(model.name);
        groupManager_->blockSignals(true);
        groupManager_->blockSignals(false);
    }
    else if (model.editorSettings.type == CubesUnitTypes::EditorType::String)
    {
        pr = stringManager_->addProperty(model.name);
        stringManager_->blockSignals(true);
        stringManager_->setOldValue(pr, model.value.toString());
        stringManager_->setValue(pr, model.value.toString());
        stringManager_->blockSignals(false);
    }
    else if (model.editorSettings.type == CubesUnitTypes::EditorType::SpinInterger)
    {
        pr = intManager_->addProperty(model.name);
        intManager_->blockSignals(true);
        intManager_->setRange(pr, model.editorSettings.SpinIntergerMin, model.editorSettings.SpinIntergerMax);
        intManager_->setValue(pr, model.value.toInt());
        intManager_->blockSignals(false);
    }
    else if (model.editorSettings.type == CubesUnitTypes::EditorType::SpinDouble)
    {
        pr = doubleManager_->addProperty(model.name);
        doubleManager_->blockSignals(true);
        doubleManager_->setRange(pr, model.editorSettings.SpinDoubleMin, model.editorSettings.SpinDoubleMax);
        doubleManager_->setSingleStep(pr, model.editorSettings.SpinDoubleSingleStep);
        doubleManager_->setValue(pr, model.value.toDouble());
        doubleManager_->blockSignals(false);
    }
    else if (model.editorSettings.type == CubesUnitTypes::EditorType::ComboBox)
    {
        pr = enumManager_->addProperty(model.name);
        enumManager_->blockSignals(true);
        enumManager_->setEnumNames(pr, model.editorSettings.ComboBoxValues);

        int pos = 0;
        for (; pos < model.editorSettings.ComboBoxValues.size(); ++pos)
        {
            if (model.value.type() == QVariant::Type::Double &&
                model.value.toDouble() == std::stod(model.editorSettings.ComboBoxValues[pos].toStdString()))
                break;
            else if (model.value.type() == QVariant::Type::Int &&
                model.value.toInt() == std::stoi(model.editorSettings.ComboBoxValues[pos].toStdString()))
                break;
            else if (model.value.type() == QVariant::Type::Bool &&
                model.value.toBool() == (model.editorSettings.ComboBoxValues[pos] == "true"))
                break;
            else if (model.value.type() == QVariant::Type::String &&
                model.value.toString() == model.editorSettings.ComboBoxValues[pos])
                break;
        }

        if (pos == model.editorSettings.ComboBoxValues.size())
            pos = 0;

        enumManager_->setValue(pr, pos);
        enumManager_->blockSignals(false);
    }
    else if (model.editorSettings.type == CubesUnitTypes::EditorType::CheckBox)
    {
        pr = boolManager_->addProperty(model.name);
        boolManager_->blockSignals(true);
        boolManager_->setValue(pr, model.value.toBool());
        boolManager_->blockSignals(false);
    }
    else if (model.editorSettings.type == CubesUnitTypes::EditorType::Color)
    {
        pr = colorManager_->addProperty(model.name);
        colorManager_->blockSignals(true);
        colorManager_->setValue(pr, QColor::fromRgba(model.value.toUInt()));
        colorManager_->blockSignals(false);
    }
    else assert(false);

    // Для регистрации
    idToProperty[model.id] = pr;

    // Идем по дереву
    for (auto& sp : model.parameters)
        pr->addSubProperty(CreatePropertyForModel(sp, idToProperty));

    if (model.readOnly)
        pr->setEnabled(false);

    return pr;
}

void PropertiesEditor::SetPropertyValue(QtProperty* property, const CubesUnitTypes::ParameterModel& model)
{
    // None, String, SpinInterger, SpinDouble, ComboBox, CheckBox

    if (model.editorSettings.type == CubesUnitTypes::EditorType::None)
    {
    }
    else if (model.editorSettings.type == CubesUnitTypes::EditorType::String)
    {
        stringManager_->blockSignals(true);
        stringManager_->setOldValue(property, model.value.toString());
        stringManager_->setValue(property, model.value.toString());
        stringManager_->blockSignals(false);
    }
    else if (model.editorSettings.type == CubesUnitTypes::EditorType::SpinInterger)
    {
        intManager_->blockSignals(true);
        intManager_->setRange(property, model.editorSettings.SpinIntergerMin, model.editorSettings.SpinIntergerMax);
        intManager_->setValue(property, model.value.toInt());
        intManager_->blockSignals(false);
    }
    else if (model.editorSettings.type == CubesUnitTypes::EditorType::SpinDouble)
    {
        doubleManager_->blockSignals(true);
        doubleManager_->setRange(property, model.editorSettings.SpinDoubleMin, model.editorSettings.SpinDoubleMax);
        doubleManager_->setSingleStep(property, model.editorSettings.SpinDoubleSingleStep);
        doubleManager_->setValue(property, model.value.toDouble());
        doubleManager_->blockSignals(false);
    }
    else if (model.editorSettings.type == CubesUnitTypes::EditorType::ComboBox)
    {
        enumManager_->blockSignals(true);
        enumManager_->setEnumNames(property, model.editorSettings.ComboBoxValues);

        int pos = 0;
        for (; pos < model.editorSettings.ComboBoxValues.size(); ++pos)
        {
            if (model.value.type() == QVariant::Type::Double &&
                model.value.toDouble() == std::stod(model.editorSettings.ComboBoxValues[pos].toStdString()))
                break;
            else if (model.value.type() == QVariant::Type::Int &&
                model.value.toInt() == std::stoi(model.editorSettings.ComboBoxValues[pos].toStdString()))
                break;
            else if (model.value.type() == QVariant::Type::Bool &&
                model.value.toBool() == (model.editorSettings.ComboBoxValues[pos] == "true"))
                break;
            else if (model.value.type() == QVariant::Type::String &&
                model.value.toString() == model.editorSettings.ComboBoxValues[pos])
                break;
        }

        if (pos == model.editorSettings.ComboBoxValues.size())
            pos = 0;

        enumManager_->setValue(property, pos);
        enumManager_->blockSignals(false);
    }
    else if (model.editorSettings.type == CubesUnitTypes::EditorType::CheckBox)
    {
        boolManager_->blockSignals(true);
        boolManager_->setValue(property, model.value.toBool());
        boolManager_->blockSignals(false);
    }
    else if (model.editorSettings.type == CubesUnitTypes::EditorType::Color)
    {
        colorManager_->blockSignals(true);
        colorManager_->setValue(property, QColor::fromRgba(model.value.toUInt()));
        colorManager_->blockSignals(false);
    }
    else assert(false);

    if (model.readOnly)
        property->setEnabled(false);
}

void PropertiesEditor::SetIntValue(QtProperty* property, int value)
{
    //!!!!!!!!!!!!
    intManager_->blockSignals(true);
    intManager_->setValue(property, value);
    intManager_->blockSignals(false);
}

void PropertiesEditor::SetDoubleValue(QtProperty* property, double value)
{
    doubleManager_->setValue(property, value);
}

void PropertiesEditor::SetStringValue(QtProperty* property, const QString& value, bool setOldValue, const QString& oldValue)
{
    if (setOldValue)
        stringManager_->setOldValue(property, oldValue);
    stringManager_->setValue(property, value);
}
    
void PropertiesEditor::SetEnumValue(QtProperty* property, int value)
{
    enumManager_->setValue(property, value);
}

void PropertiesEditor::SetEnumValues(QtProperty* property, QStringList values)
{
    enumManager_->setEnumNames(property, values);
}
    
void PropertiesEditor::SetEnumValue(QtProperty* property, QVariant value)
{
    auto values = enumManager_->enumNames(property);
    int pos = 0;
    for (; pos < values.size(); ++pos)
    {
        if (value.type() == QVariant::Type::Double && value.toDouble() == std::stod(values[pos].toStdString()))
            break;
        else if (value.type() == QVariant::Type::Int && value.toInt() == std::stoi(values[pos].toStdString()))
            break;
        else if (value.type() == QVariant::Type::Bool && value.toBool() == (values[pos] == "true"))
            break;
        else if (value.type() == QVariant::Type::String && value.toString() == values[pos])
            break;
    }

    if (pos == values.size())
        pos = 0;

    enumManager_->setValue(property, pos);
}

void PropertiesEditor::SetBoolValue(QtProperty* property, bool value)
{
    boolManager_->setValue(property, value);
}
    
void PropertiesEditor::SetColorValue(QtProperty* property, QColor value)
{
    colorManager_->setValue(property, value);
}

void PropertiesEditor::SetReadOnly(QtProperty* property, bool readOnly)
{
    property->setEnabled(!readOnly);
}

void PropertiesEditor::CollapsedInternal(QtBrowserItem* item)
{
    emit Collapsed(item);
}

void PropertiesEditor::ExpandedInternal(QtBrowserItem* item)
{
    emit Expanded(item);
}

void PropertiesEditor::ContextMenuRequestedInternal(const QPoint& pos)
{
    emit ContextMenuRequested(pos);
}

void PropertiesEditor::CurrentItemChangedInternal(QtBrowserItem* item)
{
    emit CurrentItemChanged(item);
}

void PropertiesEditor::IntValueChangedInternal(QtProperty* property, int value)
{
    emit IntValueChanged(property, value);
    emit ValueChanged(property, value);
}

void PropertiesEditor::DoubleValueChangedInternal(QtProperty* property, double value)
{
    emit DoubleValueChanged(property, value);
    emit ValueChanged(property, value);
}

void PropertiesEditor::StringValueChangedInternal(QtProperty* property, const QString& value)
{
    emit StringValueChanged(property, value);
    emit ValueChanged(property, value);
}

void PropertiesEditor::StringEditingFinishedInternal(QtProperty* property, const QString& value, const QString& oldValue)
{
    emit StringEditingFinished(property, value, oldValue);
}

void PropertiesEditor::EnumValueChangedInternal(QtProperty* property, int value)
{
    emit EnumValueChanged(property, value);
    emit ValueChanged(property, value);
}

void PropertiesEditor::BoolValueChangedInternal(QtProperty* property, bool value)
{
    emit BoolValueChanged(property, value);
    emit ValueChanged(property, value);
}

void PropertiesEditor::ColorValueChangedInternal(QtProperty* property, const QColor& value)
{
    emit ColorValueChanged(property, value);
    emit ValueChanged(property, value.rgba());
}
