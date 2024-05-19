#pragma once

#include <QObject>
#include <QString>
#include "properties_items_manager_interface.h"
#include "../unit/unit_types.h"
#include "../analysis/analysis_types.h"
#include "../xml/xml_parser.h"
#include "../property_browser/properties_editor.h"

namespace CubesProperties
{
    class PropertiesItem : public QObject
    {
        Q_OBJECT

    private:
        // Params
        IPropertiesItemsManager* propertiesItemsManager_;
        QPointer<PropertiesEditor> editor_;
        CubesUnitTypes::PropertiesId propertiesId_;
        CubesUnitTypes::UnitParameters unitParameters_;

        // ћодель параметров
        CubesUnitTypes::ParameterModels parameterModels_;

        // —войства верхнего уровн€
        QList<QtProperty*> topLevelProperties_;

        // —оответствие между свойствами и id модели
        QMap<const QtProperty*, CubesUnitTypes::ParameterModelId> propertyToId_;
        QMap<CubesUnitTypes::ParameterModelId, const QtProperty*> idToProperty_;

        // ѕри добавлении свойства приходит событие, что оно развернуто, надо его игнорировать
        bool ignoreEvents_;

        // «начени€ имен параметров
        CubesUnitTypes::ParameterModelIds ids_;

    public:
        PropertiesItem(IPropertiesItemsManager* propertiesItemsManager, PropertiesEditor* editor,
            CubesUnitTypes::UnitParameters unitParameters, bool isArrayUnit, CubesUnitTypes::PropertiesId propertiesId);
        PropertiesItem(IPropertiesItemsManager* propertiesItemsManager, PropertiesEditor* editor,
            CubesUnitTypes::UnitParameters unitParameters, CubesUnitTypes::PropertiesId propertiesId, CubesUnitTypes::ParameterModels pm);
        PropertiesItem(IPropertiesItemsManager* propertiesItemsManager, PropertiesEditor* editor,
            CubesUnitTypes::UnitParameters unitParameters, const CubesXml::Unit& xmlUnit, bool isArrayUnit, CubesUnitTypes::PropertiesId propertiesId);

    public:
        uint32_t GetPropertiesId() { return propertiesId_; };

        void Select();
        void UnSelect();
        void ExpandedChanged(const QtProperty* property, bool is_expanded);







        void SetName(QString name);
        QString GetName();

        QPixmap GetPixmap();

        void SetPosition(QPointF point);
        QPointF GetPosition();
        void SetZOrder(double value);
        double GetZOrder();

        void SetFileIdNames(CubesUnitTypes::FileIdNames fileNames);
        void SetFileIdName(CubesUnitTypes::FileId fileId, QString fileName);
        CubesUnitTypes::FileId GetFileId();
        QString GetFileName();

        void SetIncludeIdNames(CubesUnitTypes::IncludeIdNames includeNames);
        void SetIncludeIdName(CubesUnitTypes::IncludeId includeId, QString includeName);
        CubesUnitTypes::IncludeId GetIncludeId();
        QString GetIncludeName();

        // TODO: Ќадо от них избавитьс€
        CubesUnitTypes::UnitParameters GetUnitParameters() { return unitParameters_; };
        QString GetUnitId() { return QString::fromStdString(unitParameters_.fileInfo.info.id); };
        QString GetUnitCategory() { return QString::fromStdString(unitParameters_.fileInfo.info.category); };


        QString GetPropertyDescription(QtProperty* property);
        QList<QString> GetConnectedNames();
        QList<QString> GetDependentNames();




        void GetXml(CubesXml::Unit& xmlUnit);
        void RemoveItems(const CubesUnitTypes::ParameterModelId& id);
        void AddItems(const CubesUnitTypes::ParameterModel& model);
        CubesAnalysis::Properties GetAnalysisProperties();
        CubesUnitTypes::ParameterModels GetParameterModels();

    private:
        // TODO: move to private
        bool GetXmlParam(const CubesUnitTypes::ParameterModel& pm, CubesXml::Param& param);
        bool GetXmlArrray(const CubesUnitTypes::ParameterModel& pm, CubesXml::Array& array);

    private slots:
        void ValueChanged(QtProperty* property, const QVariant& value);
        void StringEditingFinished(QtProperty* property, const QString& value, const QString& oldValue);

    private:
        void CreateParametersModel(const CubesXml::Unit* xmlUnit, bool isArrayUnit);
        void CreateProperties();
        void CreateParameterModel(const CubesUnitTypes::ParameterInfoId& parameterInfoId,
            const CubesUnitTypes::ParameterModelId& parentModelId, const CubesXml::Unit* xmlUnit,
            CubesUnitTypes::ParameterModel& model);
        void FillParameterModel(const CubesXml::Unit* xmlUnit, CubesUnitTypes::ParameterModel& model, bool isItem);
        void FillArrayModel(const CubesXml::Unit* xmlUnit, CubesUnitTypes::ParameterModel& model);
        void UpdateArrayModel(const CubesXml::Unit* xmlUnit, CubesUnitTypes::ParameterModel& model);

        void GetConnectedNamesInternal(const CubesUnitTypes::ParameterModel& model, QList<QString>& list);
        void GetDependentNamesInternal(const CubesUnitTypes::ParameterModel& model, QList<QString>& list);
        void GetAnalysisPropertiesInternal(const CubesUnitTypes::ParameterModel& model, QVector<CubesAnalysis::UnitProperty>& list);

        void RegisterProperty(const QtProperty* property, const CubesUnitTypes::ParameterModelId& id);
        void UnregisterProperty(const CubesUnitTypes::ParameterModelId& id);
        void UnregisterProperty(const QtProperty* property);
        QtProperty* GetProperty(const CubesUnitTypes::ParameterModelId& id);
        CubesUnitTypes::ParameterModelId GetPropertyId(const QtProperty* property);
        //CubesUnitTypes::ParameterModel* GetParameterModel(const CubesUnitTypes::ParameterModelId& id);
        //CubesUnitTypes::ParameterModel* GetParameterModel(const QtProperty* property);
        CubesUnitTypes::UnitParameters* GetPropertyUnitParameters(const QtProperty* property);

        void ApplyExpandState();
        void ApplyExpandState(QtBrowserItem* index);

    public:
        CubesUnitTypes::ParameterModel* GetParameterModel(const CubesUnitTypes::ParameterModelId& id);
        CubesUnitTypes::ParameterModel* GetParameterModel(const QtProperty* property);

    private:
        bool CheckParametersMatching(const CubesXml::Unit* xmlUnit, const QString& type, const CubesUnitTypes::ParameterModelId& id);
    };
}