#pragma once

#include <QObject>
#include <QString>
#include "properties_items_manager_interface.h"
#include "../unit/unit_types.h"
#include "../analysis/analysis_types.h"
#include "../xml/xml_parser.h"
#include "../property_browser/properties_editor.h"

namespace CubesLog { class ILogManager; }
namespace CubesLog { class LogHelper; }

namespace CubesProperties
{
    class PropertiesItem : public QObject
    {
        Q_OBJECT

    private:
        // Params
        IPropertiesItemsManager* propertiesItemsManager_;
        CubesLog::ILogManager* logManager_;
        QPointer<PropertiesEditor> editor_;
        CubesUnit::PropertiesId propertiesId_;
        CubesUnit::UnitParameters unitParameters_;

        // ������ ����������
        CubesUnit::ParameterModels parameterModels_;

        // �������� �������� ������
        QList<QtProperty*> topLevelProperties_;

        // ������������ ����� ���������� � id ������
        QMap<const QtProperty*, CubesUnit::ParameterModelId> propertyToId_;
        QMap<CubesUnit::ParameterModelId, const QtProperty*> idToProperty_;

        // ��� ���������� �������� �������� �������, ��� ��� ����������, ���� ��� ������������
        bool ignoreEvents_;

        // �������� ���� ����������
        CubesUnit::ParameterModelIds ids_;

        // Log
        QSharedPointer<CubesLog::LogHelper> logHelper_;

    public:
        PropertiesItem(IPropertiesItemsManager* propertiesItemsManager, CubesLog::ILogManager* logManager, PropertiesEditor* editor,
            CubesUnit::UnitParameters unitParameters, bool isArrayUnit, CubesUnit::PropertiesId propertiesId);
        PropertiesItem(IPropertiesItemsManager* propertiesItemsManager, CubesLog::ILogManager* logManager, PropertiesEditor* editor,
            CubesUnit::UnitParameters unitParameters, CubesUnit::PropertiesId propertiesId, CubesUnit::ParameterModels pm);
        PropertiesItem(IPropertiesItemsManager* propertiesItemsManager, CubesLog::ILogManager* logManager, PropertiesEditor* editor,
            CubesUnit::UnitParameters unitParameters, const CubesXml::Unit& xmlUnit, bool isArrayUnit, CubesUnit::PropertiesId propertiesId);

    public:
        uint32_t GetPropertiesId() const;

        void Select();
        void UnSelect();
        void ExpandedChanged(const QtProperty* property, bool is_expanded);







        void SetName(QString name);
        QString GetName();

        QImage GetPixmap();

        void SetPosition(QPointF point);
        QPointF GetPosition();
        void SetZOrder(double value);
        double GetZOrder();

        void SetFileIdNames(CubesUnit::FileIdNames fileNames);
        void SetFileIdName(CubesUnit::FileId fileId, QString fileName);
        CubesUnit::FileId GetFileId();
        QString GetFileName();

        void SetIncludeIdNames(CubesUnit::IncludeIdNames includeNames);
        void SetIncludeIdName(CubesUnit::IncludeId includeId, QString includeName);
        CubesUnit::IncludeId GetIncludeId();
        QString GetIncludeName();

        // TODO: ���� �� ��� ����������
        CubesUnit::UnitParameters GetUnitParameters() { return unitParameters_; };
        QString GetUnitId() { return QString::fromStdString(unitParameters_.fileInfo.info.id); };
        QString GetUnitCategory() { return QString::fromStdString(unitParameters_.fileInfo.info.category); };


        QString GetPropertyDescription(QtProperty* property);
        QList<QString> GetConnectedNames();
        QList<QString> GetDependentNames();




        void GetXml(CubesXml::Unit& xmlUnit);
        void RemoveItems(const CubesUnit::ParameterModelId& id);
        void AddItems(const CubesUnit::ParameterModel& model);
        CubesAnalysis::Properties GetAnalysisProperties();
        CubesUnit::ParameterModels GetParameterModels();

    private:
        // TODO: move to private
        bool GetXmlParam(const CubesUnit::ParameterModel& pm, CubesXml::Param& param);
        bool GetXmlArrray(const CubesUnit::ParameterModel& pm, CubesXml::Array& array);

    private slots:
        void ValueChanged(QtProperty* property, const QVariant& value);
        void StringEditingFinished(QtProperty* property, const QString& value, const QString& oldValue);

    private:
        void CreateParametersModel(const CubesXml::Unit* xmlUnit, bool isArrayUnit);
        void CreateProperties();
        void CreateParameterModel(const CubesUnit::ParameterInfoId& parameterInfoId,
            const CubesUnit::ParameterModelId& parentModelId, const CubesXml::Unit* xmlUnit,
            CubesUnit::ParameterModel& model);
        void FillParameterModel(const CubesXml::Unit* xmlUnit, CubesUnit::ParameterModel& model, bool isItem);
        void FillArrayModel(const CubesXml::Unit* xmlUnit, CubesUnit::ParameterModel& model);
        void UpdateArrayModel(const CubesXml::Unit* xmlUnit, CubesUnit::ParameterModel& model);

        void GetConnectedNamesInternal(const CubesUnit::ParameterModel& model, QList<QString>& list);
        void GetDependentNamesInternal(const CubesUnit::ParameterModel& model, QList<QString>& list);
        void GetAnalysisPropertiesInternal(const CubesUnit::ParameterModel& model, QVector<CubesAnalysis::UnitProperty>& list);

        void RegisterProperty(const QtProperty* property, const CubesUnit::ParameterModelId& id);
        void UnregisterProperty(const CubesUnit::ParameterModelId& id);
        void UnregisterProperty(const QtProperty* property);
        QtProperty* GetProperty(const CubesUnit::ParameterModelId& id);
        CubesUnit::ParameterModelId GetPropertyId(const QtProperty* property);
        //CubesUnit::ParameterModel* GetParameterModel(const CubesUnit::ParameterModelId& id);
        //CubesUnit::ParameterModel* GetParameterModel(const QtProperty* property);
        CubesUnit::UnitParameters* GetPropertyUnitParameters(const QtProperty* property);

        void ApplyExpandState();
        void ApplyExpandState(QtBrowserItem* index);

    public:
        CubesUnit::ParameterModel* GetParameterModel(const QtProperty* property);

    private:
        bool CheckParametersMatching(const CubesXml::Unit* xmlUnit, const QString& type, const CubesUnit::ParameterModelId& id);
    };
}