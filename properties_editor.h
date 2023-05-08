#pragma once

#include <QObject>
#include <QPointer>
#include <QSharedPointer>
#include "qttreepropertybrowser.h"
#include "qtpropertybrowser.h"
#include "qtpropertymanager.h"

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
        qDebug() << connect(enumManager_, &QtEnumPropertyManager::valueChanged, this, &properties_editor::EnumValueChangedInternal);
        qDebug() << connect(boolManager_, &QtBoolPropertyManager::valueChanged, this, &properties_editor::BoolValueChangedInternal);
        qDebug() << connect(colorManager_, &QtColorPropertyManager::valueChanged, this, &properties_editor::ColorValueChangedInternal);
    }

public:
    QSharedPointer<QtTreePropertyBrowser> GetPropertyEditor()
    {
        return propertyEditor_;
    }

signals:
    void Collapsed(QtBrowserItem* item);
    void Expanded(QtBrowserItem* item);
    void ContextMenuRequested(const QPoint& pos);
    void IntValueChanged(QtProperty* property, int value);
    void DoubleValueChanged(QtProperty* property, double value);
    void StringValueChanged(QtProperty* property, const QString& value);
    void EnumValueChanged(QtProperty* property, int value);
    void BoolValueChanged(QtProperty* property, bool value);
    void ColorValueChanged(QtProperty* property, const QColor& value);

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
    }

    void DoubleValueChangedInternal(QtProperty* property, double value)
    {
        emit DoubleValueChanged(property, value);
    }

    void StringValueChangedInternal(QtProperty* property, const QString& value)
    {
        emit StringValueChanged(property, value);
    }

    void EnumValueChangedInternal(QtProperty* property, int value)
    {
        emit EnumValueChanged(property, value);
    }

    void BoolValueChangedInternal(QtProperty* property, bool value)
    {
        emit BoolValueChanged(property, value);
    }

    void ColorValueChangedInternal(QtProperty* property, const QColor& value)
    {
        emit ColorValueChanged(property, value);
    }
};
