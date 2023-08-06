#pragma once

#include <QObject>
#include <QMap>
#include <QList>
#include <QSharedPointer>

#include "file_items_manager_interface.h"
#include "../unit_types.h"
#include "../property_browser/properties_editor.h"

namespace FileItem
{
    class file_item : public QObject
    {
        Q_OBJECT

    private:
        // Params
        IFileItemsManager* fileItemsManager_;
        QPointer<properties_editor> editor_;

        // ћодель параметров
        unit_types::ParametersModel model_;

        // —войства верхнего уровн€
        QList<QtProperty*> topLevelProperties_;

        // —оответствие между свойствами и id модели
        QMap<const QtProperty*, QString> propertyToId_;
        QMap<QString, const QtProperty*> idToProperty_;

        // ѕри добавлении свойства приходит событие, что оно развернуто, надо его игнорировать
        bool ignoreEvents_;

    public:
        file_item(IFileItemsManager* fileItemsManager, properties_editor* editor);

    public:
        void Select();
        void UnSelect();

        void SetName(QString name, bool setOldName = false, QString oldName = "");
        void SetColor(QColor color);
        void ExpandedChanged(const QtProperty* property, bool is_expanded);

        QString GetName();
        QColor GetColor();
        QString GetPropertyDescription(const QtProperty* property);
        QStringList GetIncludeNames();
        QList<QPair<QString, QString>> GetIncludeVariables(const QString& includeName);

    private slots:
        void ValueChanged(QtProperty* property, const QVariant& value);
        void StringEditingFinished(QtProperty* property, const QString& value, const QString& oldValue);

    private:
        void CreateParametersModel();
        void CreateProperties();
        void UpdateIncludesArrayModel(unit_types::ParameterModel& pm, int& count);
        void UpdateVariablesArrayModel(unit_types::ParameterModel& pm, int& count);
        void RegisterProperty(const QtProperty* property, const QString& id);
        void UnregisterProperty(const QString& id);
        void UnregisterProperty(const QtProperty* property);
        QtProperty* GetProperty(const QString& id);
        QString GetPropertyId(const QtProperty* property);
        unit_types::ParameterModel* GetParameterModel(const QString& id);
        unit_types::ParameterModel* GetParameterModel(const QtProperty* property);
        void ApplyExpandState();
        void ApplyExpandState(QtBrowserItem* index);
    };
}
