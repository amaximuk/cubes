#pragma once

#include <QObject>
#include <QString>
#include "properties_items_manager_interface.h"
#include "../unit_types.h"
#include "../xml/xml_parser.h"
#include "../property_browser/properties_editor.h"

namespace CubesProperties
{
    extern const QString baseGroupName;
    extern const QString parametersGroupName;
    extern const QString editorGroupName;
    extern const QString itemGroupName;
    extern const QString dependsParameterName;
    extern const QString optionalParameterName;


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
        QMap<const QtProperty*, QString> propertyToId_;
        QMap<QString, const QtProperty*> idToProperty_;

        // ѕри добавлении свойства приходит событие, что оно развернуто, надо его игнорировать
        bool ignoreEvents_;

    public:
        PropertiesItem(IPropertiesItemsManagerBoss* propertiesItemsManager, PropertiesEditor* editor,
            CubesUnitTypes::UnitParameters unitParameters, uint32_t propertiesId);
        PropertiesItem(IPropertiesItemsManagerBoss* propertiesItemsManager, PropertiesEditor* editor,
            CubesUnitTypes::UnitParameters unitParameters, uint32_t propertiesId, CubesUnitTypes::ParametersModel pm);
        PropertiesItem(IPropertiesItemsManagerBoss* propertiesItemsManager, PropertiesEditor* editor,
            CubesUnitTypes::UnitParameters unitParameters, const CubesXml::Unit& xmlUnit, uint32_t propertiesId);

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
        void SetFileNames(QStringList fileNames);
        void SetFileName(QString fileName);
        void SetFileNameReadOnly(bool readOnly);
        QString GetFileName();
        void SetIncludeNames(QStringList includeNames);
        void SetIncludeName(QString includeName);
        void SetIncludeNameReadOnly(bool readOnly);
        QString GetIncludeName();
        void SetName(QString name);
        QString GetName();
        QString GetId() { return QString::fromStdString(unitParameters_.fileInfo.info.id); };
        uint32_t GetPropertiesId() { return propertiesId_; };
        CubesUnitTypes::UnitParameters GetUnitParameters() { return unitParameters_; };
        QString GetUnitId() { return QString::fromStdString(unitParameters_.fileInfo.info.id); };
        QString GetUnitCategory() { return QString::fromStdString(unitParameters_.fileInfo.info.category); };

        // TODO: move to private
        void GetXmlProperties(const CubesUnitTypes::ParameterModel& pm,
            QList<CubesXml::Param>& params, QList<CubesXml::Array>& arrays);
        bool GetXmlParam(const CubesUnitTypes::ParameterModel& pm, CubesXml::Param& param);
        bool GetXmlArrray(const CubesUnitTypes::ParameterModel& pm, CubesXml::Array& array);

        void GetXml(CubesXml::Unit& xmlUnit);
        void RemoveSubProperties(QtProperty* property);

    private slots:
        void ValueChanged(QtProperty* property, const QVariant& value);
        void StringEditingFinished(QtProperty* property, const QString& value, const QString& oldValue);

    public:
        void CreateParametersModel(const CubesXml::Unit* xmlUnit);
        void CreateProperties();
        void CreateParameterModel(const CubesUnitTypes::ParameterInfoId& parameterInfoId,
            const QString& parentModelId, const CubesXml::Unit* xmlUnit, CubesUnitTypes::ParameterModel& model);
        void FillParameterModel(const CubesXml::Unit* xmlUnit, CubesUnitTypes::ParameterModel& model, bool isItem);
        void FillArrayModel(const CubesXml::Unit* xmlUnit, CubesUnitTypes::ParameterModel& model);
        void UpdateArrayModel(const CubesXml::Unit* xmlUnit, CubesUnitTypes::ParameterModel& model);
        void GetConnectedNamesInternal(const CubesUnitTypes::ParameterModel& model, QList<QString>& list);
        void GetDependentNamesInternal(const CubesUnitTypes::ParameterModel& model, QList<QString>& list);

        void RegisterProperty(const QtProperty* property, const QString& id);
        void UnregisterProperty(const QString& id);
        void UnregisterProperty(const QtProperty* property);
        QtProperty* GetProperty(const QString& id);
        QString GetPropertyId(const QtProperty* property);
        CubesUnitTypes::ParameterModel* GetParameterModel(const QString& id);
        CubesUnitTypes::ParameterModel* GetParameterModel(const QtProperty* property);
        CubesUnitTypes::UnitParameters* GetPropertyUnitParameters(const QtProperty* property);

        void ApplyExpandState();
        void ApplyExpandState(QtBrowserItem* index);
    };
}