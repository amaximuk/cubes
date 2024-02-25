#pragma once

#include <QObject>
#include "file_items_manager_interface.h"
#include "../unit_types.h"
#include "../property_browser/properties_editor.h"

namespace CubesFile
{
    class FileItem : public QObject
    {
        Q_OBJECT

    private:
        // Params
        IFileItemsManagerBoss* fileItemsManager_;
        QPointer<PropertiesEditor> editor_;
        uint32_t fileId_;

        // ћодель параметров
        CubesUnitTypes::ParametersModel model_;

        // —войства верхнего уровн€
        QList<QtProperty*> topLevelProperties_;

        // —оответствие между свойствами и id модели
        QMap<const QtProperty*, QString> propertyToId_;
        QMap<QString, const QtProperty*> idToProperty_;

        // ѕри добавлении свойства приходит событие, что оно развернуто, надо его игнорировать
        bool ignoreEvents_;

    public:
        FileItem(IFileItemsManagerBoss* fileItemsManager, PropertiesEditor* editor, uint32_t fileId);

    public:
        void Select();
        void UnSelect();

        void SetName(QString name, bool setOldName = false, QString oldName = "");
        void SetPlatform(QString platform);
        void SetPath(QString name, bool setOldName = false, QString oldName = "");
        void SetColor(QColor color);
        void AddInclude(const QString& includeName, QList<QPair<QString, QString>> includeVariables);
        void ExpandedChanged(const QtProperty* property, bool is_expanded);

        QString GetName();
        QColor GetColor();
        QString GetPropertyDescription(const QtProperty* property);
        QStringList GetIncludeNames();
        QList<QPair<QString, QString>> GetIncludeVariables(const QString& includeName);
        QString GetIncludeName(const QString& includePath);
        uint32_t GetFileId() { return fileId_; };

        QString GetIncludePath(const QString& includeName);
        File GetFile();
        CubesXml::File GetXmlFile();

    private slots:
        void ValueChanged(QtProperty* property, const QVariant& value);
        void StringEditingFinished(QtProperty* property, const QString& value, const QString& oldValue);

    private:
        void CreateParametersModel();
        void CreateProperties();
        void UpdateIncludesArrayModel(CubesUnitTypes::ParameterModel& model, int& count);
        void UpdateVariablesArrayModel(CubesUnitTypes::ParameterModel& model, int& count);
        void UpdateConnectArrayModel(CubesUnitTypes::ParameterModel& model, int& count);
        void RegisterProperty(const QtProperty* property, const QString& id);
        void UnregisterProperty(const QString& id);
        void UnregisterProperty(const QtProperty* property);
        QtProperty* GetProperty(const QString& id);
        QString GetPropertyId(const QtProperty* property);
        CubesUnitTypes::ParameterModel* GetParameterModel(const QString& id);
        CubesUnitTypes::ParameterModel* GetParameterModel(const QtProperty* property);
        void ApplyExpandState();
        void ApplyExpandState(QtBrowserItem* index);
    };
}
