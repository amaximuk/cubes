#pragma once

#include <QObject>
#include "file_items_manager_interface.h"
#include "../xml/xml_types.h"
#include "../analysis/analysis_types.h"
#include "../unit/unit_types.h"
#include "../property_browser/properties_editor.h"
#include "../unit/unit_parameter_model_ids.h"

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
        CubesUnit::FileId fileId_;

        // ������ ����������
        CubesUnit::ParameterModelPtrs parameterModelPtrs_;

        // �������� �������� ������
        QList<QtProperty*> topLevelProperties_;

        // ������������ ����� ���������� � id ������
        QMap<const QtProperty*, CubesUnit::ParameterModelId> propertyToId_;
        QMap<CubesUnit::ParameterModelId, const QtProperty*> idToProperty_;

        // ��� ���������� �������� �������� �������, ��� ��� ����������, ���� ��� ������������
        bool ignoreEvents_;

        // �������� ���� ����������
        CubesUnit::ParameterModelIds ids_;

        // ����, ���� �� ������������� �������� � ��������
        bool notifyManager_;

        // ���������� ����� ��� ������������ id
        CubesUnit::IncludeId uniqueNumber_;

    public:
        FileItem(IFileItemsManager* fileItemsManager, PropertiesEditor* editor, CubesUnit::FileId fileId);
        FileItem(IFileItemsManager* fileItemsManager, PropertiesEditor* editor, const CubesXml::File& xmlFile, CubesUnit::FileId fileId);

    public:
        CubesUnit::FileId GetFileId() const;

        void Select();
        void UnSelect();
        void ExpandedChanged(const QtProperty* property, bool is_expanded);

        void SetName(QString name, bool setOldName = false, QString oldName = "");
        QString GetName();

        void SetPath(QString name, bool setOldName = false, QString oldName = "");

        void AddInclude(const CubesUnit::IncludeId includeId, const CubesUnit::VariableIdVariables& variables);
        CubesUnit::IncludeIdNames GetIncludes();
        bool GetIncludeVariables(const CubesUnit::IncludeId includeId, CubesUnit::VariableIdVariables& variables);
        QString GetIncludeName(const CubesUnit::IncludeId includeId);
        QString GetIncludePath(const CubesUnit::IncludeId includeId);

        void SetColor(QColor color);
        QColor GetColor();

        File GetFile();
        CubesXml::File GetXmlFile();
        QVector<CubesAnalysis::File> GetAnalysisFiles();
        CubesUnit::ParameterModelPtrs GetParameterModelPtrs();

    private slots:
        void ValueChanged(QtProperty* property, const QVariant& value);
        void StringEditingFinished(QtProperty* property, const QString& value, const QString& oldValue);

    private:
        void CreateParametersModel(const CubesXml::File* xmlFile);
        void CreateProperties();
        void UpdateIncludesArrayModel(const CubesXml::File* xmlFile, CubesUnit::ParameterModelPtr model, int& count);
        void UpdateVariablesArrayModel(const CubesXml::Include* xmlInclude, CubesUnit::IncludeId includeId,
            CubesUnit::ParameterModelPtr model, int& count);
        void UpdateConnectArrayModel(const CubesXml::Networking* xmlNetworking, CubesUnit::ParameterModelPtr model, int& count);
        void RegisterProperty(const QtProperty* property, const CubesUnit::ParameterModelId& id);
        void UnregisterProperty(const CubesUnit::ParameterModelId& id);
        void UnregisterProperty(const QtProperty* property);
        QtProperty* GetProperty(const CubesUnit::ParameterModelId& id);
        CubesUnit::ParameterModelId GetPropertyId(const QtProperty* property);
        CubesUnit::ParameterModelPtr GetParameterModelPtr(const QtProperty* property);
        void ApplyExpandState();
        void ApplyExpandState(QtBrowserItem* index);
    };
}
