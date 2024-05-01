#pragma once

#include <QObject>
#include "file_items_manager_interface.h"
#include "../xml/xml_types.h"
#include "../analysis/analysis_types.h"
#include "../unit_types.h"
#include "../property_browser/properties_editor.h"

namespace CubesFile
{
    class FileItem : public QObject
    {
        Q_OBJECT

    private:
        // enum
        enum class ArrayType
        {
            Includes,
            Variables,
            Connect
        };

    private:
        // Params
        IFileItemsManager* fileItemsManager_;
        QPointer<PropertiesEditor> editor_;
        uint32_t fileId_;

        // Модель параметров
        CubesUnitTypes::ParametersModel model_;

        // Свойства верхнего уровня
        QList<QtProperty*> topLevelProperties_;

        // Соответствие между свойствами и id модели
        QMap<const QtProperty*, CubesUnitTypes::ParameterModelId> propertyToId_;
        QMap<CubesUnitTypes::ParameterModelId, const QtProperty*> idToProperty_;

        // При добавлении свойства приходит событие, что оно развернуто, надо его игнорировать
        bool ignoreEvents_;

        // Значения имен параметров
        CubesUnitTypes::ParameterModelIds ids_;

        // Флаг, надо ли информировать менеджер о событиях
        bool notifyManager_;

        // Уникальное число для формирования id
        CubesUnitTypes::IncludeId uniqueNumber_;

    public:
        FileItem(IFileItemsManager* fileItemsManager, PropertiesEditor* editor, uint32_t fileId);
        FileItem(IFileItemsManager* fileItemsManager, PropertiesEditor* editor, const CubesXml::File& xmlFile, uint32_t fileId);

    public:
        CubesUnitTypes::FileId GetFileId() { return fileId_; };

        void Select();
        void UnSelect();
        void ExpandedChanged(const QtProperty* property, bool is_expanded);

        void SetName(QString name, bool setOldName = false, QString oldName = "");
        QString GetName();

        void SetPlatform(QString platform);
        void SetPath(QString name, bool setOldName = false, QString oldName = "");

        void AddInclude(const CubesUnitTypes::IncludeId includeId, const CubesUnitTypes::VariableIdVariables& variables);
        CubesUnitTypes::IncludeIdNames GetIncludes();
        CubesUnitTypes::VariableIdVariables GetIncludeVariables(const CubesUnitTypes::IncludeId includeId);
        QString GetIncludeName(const QString& includePath);
        QString GetIncludeName(const CubesUnitTypes::IncludeId includeId);
        QString GetIncludePath(const CubesUnitTypes::IncludeId includeId);

        void SetColor(QColor color);
        QColor GetColor();

        File GetFile();
        CubesXml::File GetXmlFile();
        std::vector<CubesAnalysis::File> GetAnalysisFiles();

    private slots:
        void ValueChanged(QtProperty* property, const QVariant& value);
        void StringEditingFinished(QtProperty* property, const QString& value, const QString& oldValue);

    private:
        void CreateParametersModel(const CubesXml::File* xmlFile);
        void CreateProperties();
        void UpdateIncludesArrayModel(const CubesXml::File* xmlFile, CubesUnitTypes::ParameterModel& model, int& count);
        void UpdateVariablesArrayModel(const CubesXml::Include* xmlInclude, CubesUnitTypes::IncludeId includeId,
            CubesUnitTypes::ParameterModel& model, int& count);
        void UpdateConnectArrayModel(const CubesXml::Networking* xmlNetworking, CubesUnitTypes::ParameterModel& model, int& count);
        void RegisterProperty(const QtProperty* property, const CubesUnitTypes::ParameterModelId& id);
        void UnregisterProperty(const CubesUnitTypes::ParameterModelId& id);
        void UnregisterProperty(const QtProperty* property);
        QtProperty* GetProperty(const CubesUnitTypes::ParameterModelId& id);
        CubesUnitTypes::ParameterModelId GetPropertyId(const QtProperty* property);
        CubesUnitTypes::ParameterModel* GetParameterModel(const CubesUnitTypes::ParameterModelId& id);
        CubesUnitTypes::ParameterModel* GetParameterModel(const QtProperty* property);
        void ApplyExpandState();
        void ApplyExpandState(QtBrowserItem* index);
    };
}
