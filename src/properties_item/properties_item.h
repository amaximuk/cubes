#pragma once

#include <QObject>
#include <QString>
#include "properties_items_manager_interface.h"
#include "../unit_types.h"
#include "../xml/xml_parser.h"
#include "../property_browser/properties_editor.h"

namespace CubesProperties
{
    class PropertiesItem : public QObject
    {
        Q_OBJECT

    private:
        // Params
        IPropertiesItemsManagerBoss* propertiesItemsManager_;
        QPointer<PropertiesEditor> editor_;
        uint32_t propertiesId_;
        CubesUnitTypes::UnitParameters unitParameters_;

        // ћодель параметров
        CubesUnitTypes::ParametersModel model_;

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
        PropertiesItem(IPropertiesItemsManagerBoss* propertiesItemsManager, PropertiesEditor* editor,
            CubesUnitTypes::UnitParameters unitParameters, bool isArrayUnit, uint32_t propertiesId);
        PropertiesItem(IPropertiesItemsManagerBoss* propertiesItemsManager, PropertiesEditor* editor,
            CubesUnitTypes::UnitParameters unitParameters, uint32_t propertiesId, CubesUnitTypes::ParametersModel pm);
        PropertiesItem(IPropertiesItemsManagerBoss* propertiesItemsManager, PropertiesEditor* editor,
            CubesUnitTypes::UnitParameters unitParameters, const CubesXml::Unit& xmlUnit, bool isArrayUnit, uint32_t propertiesId);

    public:
        void Select();
        void UnSelect();
        void ExpandedChanged(const QtProperty* property, bool is_expanded);

    public:
        QPixmap GetPixmap();
        void PositionChanged(QPointF point);
        void ZOrderChanged(double value);
        QString GetPropertyDescription(QtProperty* property);
        QList<QString> GetConnectedNames();
        QList<QString> GetDependentNames();
        QString GetInstanceName();
        void SetFileNames(CubesUnitTypes::FileIdNames fileNames);
        void SetFileName(QString fileName);
        void SetFileNameReadOnly(bool readOnly);
        QString GetFileName();
        CubesUnitTypes::FileId GetFileId();
        CubesUnitTypes::IncludeId GetIncludeId();
        void SetIncludeNames(CubesUnitTypes::IncludeIdNames includeNames);
        void SetIncludeName(QString includeName);
        void SetIncludeNameReadOnly(bool readOnly);
        QString GetIncludeName();
        void SetName(QString name);
        QString GetName();
        QPointF GetPosition();
        QString GetId() { return QString::fromStdString(unitParameters_.fileInfo.info.id); };
        uint32_t GetPropertiesId() { return propertiesId_; };
        CubesUnitTypes::UnitParameters GetUnitParameters() { return unitParameters_; };
        CubesUnitTypes::ParametersModel GetParametersModel() { return model_; };
        QString GetUnitId() { return QString::fromStdString(unitParameters_.fileInfo.info.id); };
        QString GetUnitCategory() { return QString::fromStdString(unitParameters_.fileInfo.info.category); };

        // TODO: move to private
        void GetXmlProperties(const CubesUnitTypes::ParameterModel& pm,
            QList<CubesXml::Param>& params, QList<CubesXml::Array>& arrays);
        bool GetXmlParam(const CubesUnitTypes::ParameterModel& pm, CubesXml::Param& param);
        bool GetXmlArrray(const CubesUnitTypes::ParameterModel& pm, CubesXml::Array& array);

        void GetXml(CubesXml::Unit& xmlUnit);
        //void RemoveSubProperties(QtProperty* property);
        void RemoveItems(const CubesUnitTypes::ParameterModelId& id);
        void AddItems(const CubesUnitTypes::ParameterModel& model);
        void AddSubProperties(const CubesUnitTypes::ParameterModel& model);

    private slots:
        void ValueChanged(QtProperty* property, const QVariant& value);
        void StringEditingFinished(QtProperty* property, const QString& value, const QString& oldValue);

    public:
        void CreateParametersModel(const CubesXml::Unit* xmlUnit, bool isArrayUnit);
        void CreateProperties();
        void CreateParameterModel(const CubesUnitTypes::ParameterInfoId& parameterInfoId,
            const CubesUnitTypes::ParameterModelId& parentModelId, const CubesXml::Unit* xmlUnit,
            CubesUnitTypes::ParameterModel& model);
        //void CreateParameterModel(const CubesUnitTypes::ParameterInfoId& parameterInfoId,
        //    const QString& parentModelId, const CubesXml::Unit* xmlUnit, CubesUnitTypes::ParameterModel& model);
        void FillParameterModel(const CubesXml::Unit* xmlUnit, CubesUnitTypes::ParameterModel& model, bool isItem);
        void FillArrayModel(const CubesXml::Unit* xmlUnit, CubesUnitTypes::ParameterModel& model);
        void UpdateArrayModel(const CubesXml::Unit* xmlUnit, CubesUnitTypes::ParameterModel& model);

        void GetConnectedNamesInternal(const CubesUnitTypes::ParameterModel& model, QList<QString>& list);
        void GetDependentNamesInternal(const CubesUnitTypes::ParameterModel& model, QList<QString>& list);

        void RegisterProperty(const QtProperty* property, const CubesUnitTypes::ParameterModelId& id);
        void UnregisterProperty(const CubesUnitTypes::ParameterModelId& id);
        void UnregisterProperty(const QtProperty* property);
        QtProperty* GetProperty(const CubesUnitTypes::ParameterModelId& id);
        CubesUnitTypes::ParameterModelId GetPropertyId(const QtProperty* property);
        CubesUnitTypes::ParameterModel* GetParameterModel(const CubesUnitTypes::ParameterModelId& id);
        //CubesUnitTypes::ParameterModel* GetParameterModel(const QString& id);
        CubesUnitTypes::ParameterModel* GetParameterModel(const QtProperty* property);
        CubesUnitTypes::UnitParameters* GetPropertyUnitParameters(const QtProperty* property);

        void ApplyExpandState();
        void ApplyExpandState(QtBrowserItem* index);
    };
}