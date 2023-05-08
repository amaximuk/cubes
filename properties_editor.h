#pragma once

#include <QObject>
#include <QPointer>
#include <QSharedPointer>
#include <QDebug>

#include "qttreepropertybrowser.h"
#include "qtpropertybrowser.h"
#include "qtpropertymanager.h"
#include "unit_types.h"

class properties_editor : public QObject
{
Q_OBJECT

private:
    QSharedPointer<QtTreePropertyBrowser> propertyEditor_;
    QPointer<QtGroupPropertyManager> groupManager_;
    QPointer<QtIntPropertyManager> intManager_;
    QPointer<QtDoublePropertyManager> doubleManager_;
    QPointer<QtStringPropertyManager> stringManager_;
    QPointer<QtEnumPropertyManager> enumManager_;
    QPointer<QtBoolPropertyManager> boolManager_;
    QPointer<QtColorPropertyManager> colorManager_;

public:
    properties_editor()
    {
        propertyEditor_.reset(new QtTreePropertyBrowser());
        propertyEditor_->setContextMenuPolicy(Qt::CustomContextMenu);

        qDebug() << connect(propertyEditor_.get(), &QtTreePropertyBrowser::collapsed, this, &properties_editor::CollapsedInternal);
        qDebug() << connect(propertyEditor_.get(), &QtTreePropertyBrowser::expanded, this, &properties_editor::ExpandedInternal);
        qDebug() << connect(propertyEditor_.get(), &QWidget::customContextMenuRequested, this, &properties_editor::ContextMenuRequestedInternal);

        groupManager_ = new QtGroupPropertyManager(this);
        intManager_ = new QtIntPropertyManager(this);
        doubleManager_ = new QtDoublePropertyManager(this);
        stringManager_ = new QtStringPropertyManager(this);
        enumManager_ = new QtEnumPropertyManager(this);
        boolManager_ = new QtBoolPropertyManager(this);
        colorManager_ = new QtColorPropertyManager(this);

        qDebug() << connect(intManager_, &QtIntPropertyManager::valueChanged, this, &properties_editor::IntValueChangedInternal);
        qDebug() << connect(doubleManager_, &QtDoublePropertyManager::valueChanged, this, &properties_editor::DoubleValueChangedInternal);
        qDebug() << connect(stringManager_, &QtStringPropertyManager::valueChanged, this, &properties_editor::StringValueChangedInternal);
        qDebug() << connect(stringManager_, qOverload<QtProperty*, const QString&, const QString&>(&QtStringPropertyManager::editingFinished),
            this, &properties_editor::StringEditingFinishedInternal);
        qDebug() << connect(enumManager_, &QtEnumPropertyManager::valueChanged, this, &properties_editor::EnumValueChangedInternal);
        qDebug() << connect(boolManager_, &QtBoolPropertyManager::valueChanged, this, &properties_editor::BoolValueChangedInternal);
        qDebug() << connect(colorManager_, &QtColorPropertyManager::valueChanged, this, &properties_editor::ColorValueChangedInternal);
    }

public:
    QSharedPointer<QtTreePropertyBrowser> GetPropertyEditor()
    {
        return propertyEditor_;
    }

    void AddPropertyForModel(unit_types::ParameterModel& model)
    {
        // None, String, SpinInterger, SpinDouble, ComboBox, CheckBox

        QtProperty* pr = nullptr;
        if (model.editorSettings.type == unit_types::EditorType::None)
        {
            pr = groupManager_->addProperty(model.name);
            groupManager_->blockSignals(true);
            groupManager_->blockSignals(false);
        }
        else if (model.editorSettings.type == unit_types::EditorType::String)
        {
            pr = stringManager_->addProperty(model.name);
            stringManager_->blockSignals(true);
            stringManager_->setOldValue(pr, model.value.toString());
            stringManager_->setValue(pr, model.value.toString());
            stringManager_->blockSignals(false);
        }
        else if (model.editorSettings.type == unit_types::EditorType::SpinInterger)
        {
            pr = intManager_->addProperty(model.name);
            intManager_->blockSignals(true);
            intManager_->setRange(pr, model.editorSettings.SpinIntergerMin, model.editorSettings.SpinIntergerMax);
            intManager_->setValue(pr, model.value.toInt());
            intManager_->blockSignals(false);
        }
        else if (model.editorSettings.type == unit_types::EditorType::SpinDouble)
        {
            pr = doubleManager_->addProperty(model.name);
            doubleManager_->blockSignals(true);
            doubleManager_->setRange(pr, model.editorSettings.SpinDoubleMin, model.editorSettings.SpinDoubleMax);
            doubleManager_->setSingleStep(pr, model.editorSettings.SpinDoubleSingleStep);
            doubleManager_->setValue(pr, model.value.toDouble());
            doubleManager_->blockSignals(false);
        }
        else if (model.editorSettings.type == unit_types::EditorType::ComboBox)
        {
            pr = enumManager_->addProperty(model.name);
            enumManager_->blockSignals(true);
            enumManager_->setEnumNames(pr, model.editorSettings.ComboBoxValues);

            int pos = 0;
            for (; pos < model.editorSettings.ComboBoxValues.size(); ++pos)
            {
                if (model.valueType == "double" && model.value.toDouble() == std::stod(model.editorSettings.ComboBoxValues[pos].toStdString()))
                    break;
                else if (model.valueType == "int" && model.value.toInt() == std::stoi(model.editorSettings.ComboBoxValues[pos].toStdString()))
                    break;
                else if (model.valueType == "bool" && model.value.toBool() == (model.editorSettings.ComboBoxValues[pos] == "true"))
                    break;
                else if (model.valueType == "string" && model.value.toString() == model.editorSettings.ComboBoxValues[pos])
                    break;
            }

            if (pos == model.editorSettings.ComboBoxValues.size())
                pos = 0;

            enumManager_->setValue(pr, pos);
            enumManager_->blockSignals(false);
        }
        else if (model.editorSettings.type == unit_types::EditorType::CheckBox)
        {
            pr = boolManager_->addProperty(model.name);
            boolManager_->blockSignals(true);
            boolManager_->setValue(pr, model.value.toBool());
            boolManager_->blockSignals(false);
        }
        else if (model.editorSettings.type == unit_types::EditorType::Color)
        {
            pr = colorManager_->addProperty(model.name);
            colorManager_->blockSignals(true);
            colorManager_->setValue(pr, QColor::fromRgba(model.value.toUInt()));
            colorManager_->blockSignals(false);
        }
        else assert(false);

        propertyEditor_->addProperty(pr);
    }

signals:
    void Collapsed(QtBrowserItem* item);
    void Expanded(QtBrowserItem* item);
    void ContextMenuRequested(const QPoint& pos);

    void IntValueChanged(QtProperty* property, int value);
    void DoubleValueChanged(QtProperty* property, double value);
    void StringValueChanged(QtProperty* property, const QString& value);
    void StringEditingFinished(QtProperty* property, const QString& value, const QString& oldValue);
    void EnumValueChanged(QtProperty* property, int value);
    void BoolValueChanged(QtProperty* property, bool value);
    void ColorValueChanged(QtProperty* property, const QColor& value);

    void ValueChanged(QtProperty* property, const QVariant& value);

private:
    void CollapsedInternal(QtBrowserItem* item)
    {
        emit Collapsed(item);
    }

    void ExpandedInternal(QtBrowserItem* item)
    {
        emit Expanded(item);
    }

    void ContextMenuRequestedInternal(const QPoint& pos)
    {
        emit ContextMenuRequested(pos);
    }

    void IntValueChangedInternal(QtProperty* property, int value)
    {
        emit IntValueChanged(property, value);
        emit ValueChanged(property, value);
    }

    void DoubleValueChangedInternal(QtProperty* property, double value)
    {
        emit DoubleValueChanged(property, value);
        emit ValueChanged(property, value);
    }

    void StringValueChangedInternal(QtProperty* property, const QString& value)
    {
        emit StringValueChanged(property, value);
        emit ValueChanged(property, value);
    }

    void StringEditingFinishedInternal(QtProperty* property, const QString& value, const QString& oldValue)
    {
        emit StringEditingFinished(property, value, oldValue);
    }

    void EnumValueChangedInternal(QtProperty* property, int value)
    {
        emit EnumValueChanged(property, value);
        emit ValueChanged(property, value);
    }

    void BoolValueChangedInternal(QtProperty* property, bool value)
    {
        emit BoolValueChanged(property, value);
        emit ValueChanged(property, value);
    }

    void ColorValueChangedInternal(QtProperty* property, const QColor& value)
    {
        emit ColorValueChanged(property, value);
        emit ValueChanged(property, value.rgba());
    }
};
