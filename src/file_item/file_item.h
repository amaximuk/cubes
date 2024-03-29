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
        // enum
        enum class ArrayType
        {
            Includes,
            Variables,
            Connect
        };

    private:
        // Params
        IFileItemsManagerBoss* fileItemsManager_;
        QPointer<PropertiesEditor> editor_;
        uint32_t fileId_;

        // ������ ����������
        CubesUnitTypes::ParametersModel model_;

        // �������� �������� ������
        QList<QtProperty*> topLevelProperties_;

        // ������������ ����� ���������� � id ������
        QMap<const QtProperty*, CubesUnitTypes::ParameterModelId> propertyToId_;
        QMap<CubesUnitTypes::ParameterModelId, const QtProperty*> idToProperty_;

        // ��� ���������� �������� �������� �������, ��� ��� ����������, ���� ��� ������������
        bool ignoreEvents_;

        // �������� ���� ����������
        CubesUnitTypes::ParameterModelIds ids_;

        // ����, ���� �� ������������� �������� � ��������
        bool notifyManager_;

        // ���������� ����� ��� ������������ id
        CubesUnitTypes::IncludeId uniqueNumber_;

    public:
        FileItem(IFileItemsManagerBoss* fileItemsManager, PropertiesEditor* editor, uint32_t fileId);
        FileItem(IFileItemsManagerBoss* fileItemsManager, PropertiesEditor* editor, const CubesXml::File& xmlFile, uint32_t fileId);

    public:
        void Select();
        void UnSelect();

        void SetName(QString name, bool setOldName = false, QString oldName = "");
        void SetPlatform(QString platform);
        void SetPath(QString name, bool setOldName = false, QString oldName = "");
        void SetColor(QColor color);
        void AddInclude(const CubesUnitTypes::IncludeId includeId, const CubesUnitTypes::VariableIdVariables& variables);
        void ExpandedChanged(const QtProperty* property, bool is_expanded);

        QString GetName();
        QColor GetColor();
        QString GetPropertyDescription(const QtProperty* property);
        CubesUnitTypes::IncludeIdNames GetIncludes();
        CubesUnitTypes::VariableIdVariables GetIncludeVariables(const CubesUnitTypes::IncludeId includeId);
        QString GetIncludeName(const QString& includePath);
        QString GetIncludeName(const CubesUnitTypes::IncludeId includeId);
        CubesUnitTypes::FileId GetFileId() { return fileId_; };

        QString GetIncludePath(const CubesUnitTypes::IncludeId includeId);
        File GetFile();
        CubesXml::File GetXmlFile();

    private slots:
        void ValueChanged(QtProperty* property, const QVariant& value);
        void StringEditingFinished(QtProperty* property, const QString& value, const QString& oldValue);

    private:
        void CreateParametersModel(const CubesXml::File* xmlFile);
        void CreateProperties();
        void UpdateIncludesArrayModel(const CubesXml::File* xmlFile, CubesUnitTypes::ParameterModel& model, int& count);
        void UpdateVariablesArrayModel(const CubesXml::Include* xmlInclude, CubesUnitTypes::ParameterModel& model, int& count);
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
