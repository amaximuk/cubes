#pragma once

#include <QPointer>
#include <QDebug>
#include "qttreepropertybrowser.h"
#include "qtpropertybrowser.h"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qtexpropertymanager.h"
#include "qtexeditorfactory.h"
#include "../unit/unit_types.h"
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

QtProperty* PropertiesEditor::CreatePropertyForModel(CubesUnit::ParameterModelPtr parameterModelPtr,
    QMap<CubesUnit::ParameterModelId, const QtProperty*>& idToProperty)
{
    // None, String, SpinInterger, SpinDouble, ComboBox, CheckBox

    QtProperty* pr = nullptr;
    if (parameterModelPtr->editorSettings.type == CubesUnit::EditorType::None)
    {
        pr = groupManager_->addProperty(parameterModelPtr->name);
        groupManager_->blockSignals(true);
        groupManager_->blockSignals(false);
    }
    else if (parameterModelPtr->editorSettings.type == CubesUnit::EditorType::String)
    {
        pr = stringManager_->addProperty(parameterModelPtr->name);
        stringManager_->blockSignals(true);
        stringManager_->setOldValue(pr, parameterModelPtr->value.toString());
        stringManager_->setValue(pr, parameterModelPtr->value.toString());
        stringManager_->blockSignals(false);
    }
    else if (parameterModelPtr->editorSettings.type == CubesUnit::EditorType::SpinInterger)
    {
        pr = intManager_->addProperty(parameterModelPtr->name);
        intManager_->blockSignals(true);
        intManager_->setRange(pr, parameterModelPtr->editorSettings.spinIntergerMin, parameterModelPtr->editorSettings.spinIntergerMax);
        intManager_->setValue(pr, parameterModelPtr->value.toInt());
        intManager_->blockSignals(false);
    }
    else if (parameterModelPtr->editorSettings.type == CubesUnit::EditorType::SpinDouble)
    {
        pr = doubleManager_->addProperty(parameterModelPtr->name);
        doubleManager_->blockSignals(true);
        doubleManager_->setRange(pr, parameterModelPtr->editorSettings.spinDoubleMin, parameterModelPtr->editorSettings.spinDoubleMax);
        doubleManager_->setSingleStep(pr, parameterModelPtr->editorSettings.spinDoubleSingleStep);
        doubleManager_->setValue(pr, parameterModelPtr->value.toDouble());
        doubleManager_->blockSignals(false);
    }
    else if (parameterModelPtr->editorSettings.type == CubesUnit::EditorType::ComboBox)
    {
        pr = enumManager_->addProperty(parameterModelPtr->name);
        enumManager_->blockSignals(true);
        enumManager_->setEnumNames(pr, parameterModelPtr->GetComboBoxValues());

        //int pos = 0;
        //for (; pos < model.editorSettings.comboBoxValues.size(); ++pos)
        //{
        //    if (model.value.type() == QVariant::Type::Double &&
        //        model.value.toDouble() == model.get//std::stod(model.editorSettings.comboBoxValues[pos].toStdString()))
        //        break;
        //    else if (model.value.type() == QVariant::Type::Int &&
        //        model.value.toInt() == std::stoi(model.editorSettings.comboBoxValues[pos].toStdString()))
        //        break;
        //    else if (model.value.type() == QVariant::Type::Bool &&
        //        model.value.toBool() == (model.editorSettings.comboBoxValues[pos] == "true"))
        //        break;
        //    else if (model.value.type() == QVariant::Type::String &&
        //        model.value.toString() == model.editorSettings.comboBoxValues[pos])
        //        break;
        //}

        //if (pos == model.editorSettings.comboBoxValues.size())
        //    pos = 0;

        enumManager_->setValue(pr, parameterModelPtr->GetComboBoxIndex());
        enumManager_->blockSignals(false);
    }
    else if (parameterModelPtr->editorSettings.type == CubesUnit::EditorType::CheckBox)
    {
        pr = boolManager_->addProperty(parameterModelPtr->name);
        boolManager_->blockSignals(true);
        boolManager_->setValue(pr, parameterModelPtr->value.toBool());
        boolManager_->blockSignals(false);
    }
    else if (parameterModelPtr->editorSettings.type == CubesUnit::EditorType::Color)
    {
        pr = colorManager_->addProperty(parameterModelPtr->name);
        colorManager_->blockSignals(true);
        colorManager_->setValue(pr, QColor::fromRgba(parameterModelPtr->value.toUInt()));
        colorManager_->blockSignals(false);
    }
    else assert(false);

    // Для регистрации
    idToProperty[parameterModelPtr->id] = pr;

    // Идем по дереву
    for (auto& sp : parameterModelPtr->parameters)
        pr->addSubProperty(CreatePropertyForModel(sp, idToProperty));

    if (parameterModelPtr->readOnly)
        pr->setEnabled(false);

    return pr;
}

void PropertiesEditor::SetPropertyValue(QtProperty* property, CubesUnit::ParameterModelPtr parameterModelPtr)
{
    // None, String, SpinInterger, SpinDouble, ComboBox, CheckBox

    if (parameterModelPtr->editorSettings.type == CubesUnit::EditorType::None)
    {
    }
    else if (parameterModelPtr->editorSettings.type == CubesUnit::EditorType::String)
    {
        stringManager_->blockSignals(true);
        stringManager_->setOldValue(property, parameterModelPtr->value.toString());
        stringManager_->setValue(property, parameterModelPtr->value.toString());
        stringManager_->blockSignals(false);
    }
    else if (parameterModelPtr->editorSettings.type == CubesUnit::EditorType::SpinInterger)
    {
        intManager_->blockSignals(true);
        intManager_->setRange(property, parameterModelPtr->editorSettings.spinIntergerMin, parameterModelPtr->editorSettings.spinIntergerMax);
        intManager_->setValue(property, parameterModelPtr->value.toInt());
        intManager_->blockSignals(false);
    }
    else if (parameterModelPtr->editorSettings.type == CubesUnit::EditorType::SpinDouble)
    {
        doubleManager_->blockSignals(true);
        doubleManager_->setRange(property, parameterModelPtr->editorSettings.spinDoubleMin, parameterModelPtr->editorSettings.spinDoubleMax);
        doubleManager_->setSingleStep(property, parameterModelPtr->editorSettings.spinDoubleSingleStep);
        doubleManager_->setValue(property, parameterModelPtr->value.toDouble());
        doubleManager_->blockSignals(false);
    }
    else if (parameterModelPtr->editorSettings.type == CubesUnit::EditorType::ComboBox)
    {
        enumManager_->blockSignals(true);
        enumManager_->setEnumNames(property, parameterModelPtr->GetComboBoxValues());

        //int pos = 0;
        //for (; pos < model.editorSettings.comboBoxValues.size(); ++pos)
        //{
        //    if (model.value.type() == QVariant::Type::Double &&
        //        model.value.toDouble() == std::stod(model.editorSettings.comboBoxValues[pos].toStdString()))
        //        break;
        //    else if (model.value.type() == QVariant::Type::Int &&
        //        model.value.toInt() == std::stoi(model.editorSettings.comboBoxValues[pos].toStdString()))
        //        break;
        //    else if (model.value.type() == QVariant::Type::Bool &&
        //        model.value.toBool() == (model.editorSettings.comboBoxValues[pos] == "true"))
        //        break;
        //    else if (model.value.type() == QVariant::Type::String &&
        //        model.value.toString() == model.editorSettings.comboBoxValues[pos])
        //        break;
        //}

        //if (pos == model.editorSettings.comboBoxValues.size())
        //    pos = 0;

        enumManager_->setValue(property, parameterModelPtr->GetComboBoxIndex());
        enumManager_->blockSignals(false);
    }
    else if (parameterModelPtr->editorSettings.type == CubesUnit::EditorType::CheckBox)
    {
        boolManager_->blockSignals(true);
        boolManager_->setValue(property, parameterModelPtr->value.toBool());
        boolManager_->blockSignals(false);
    }
    else if (parameterModelPtr->editorSettings.type == CubesUnit::EditorType::Color)
    {
        colorManager_->blockSignals(true);
        colorManager_->setValue(property, QColor::fromRgba(parameterModelPtr->value.toUInt()));
        colorManager_->blockSignals(false);
    }
    else assert(false);

    if (parameterModelPtr->readOnly)
        property->setEnabled(false);
}

void PropertiesEditor::SetIntValue(QtProperty* property, int value)
{
    //!!!!!!!!!!!!
    //intManager_->blockSignals(true);
    intManager_->setValue(property, value);
    //intManager_->blockSignals(false);
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
    
//void PropertiesEditor::SetEnumValue(QtProperty* property, QVariant value)
//{
//    auto values = enumManager_->enumNames(property);
//    int pos = 0;
//    for (; pos < values.size(); ++pos)
//    {
//        if (value.type() == QVariant::Type::Double && value.toDouble() == std::stod(values[pos].toStdString()))
//            break;
//        else if (value.type() == QVariant::Type::Int && value.toInt() == std::stoi(values[pos].toStdString()))
//            break;
//        else if (value.type() == QVariant::Type::Bool && value.toBool() == (values[pos] == "true"))
//            break;
//        else if (value.type() == QVariant::Type::String && value.toString() == values[pos])
//            break;
//    }
//
//    if (pos == values.size())
//        pos = 0;
//
//    enumManager_->setValue(property, pos);
//}

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

int PropertiesEditor::GetEnumValue(QtProperty* property)
{
    return enumManager_->value(property);
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
