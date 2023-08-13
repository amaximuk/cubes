#pragma once

#include <QObject>
#include "properties_items_manager_interface.h"
#include "../unit_types.h"
#include "../xml_parser.h"
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
        PropertiesItem(IPropertiesItemsManager* propertiesItemsManager, PropertiesEditor* editor,
            CubesUnitTypes::UnitParameters unitParameters, uint32_t propertiesId);

    public:
        void Select();
        void UnSelect();

    public:
        //void ApplyToBrowser(QtTreePropertyBrowser* propertyEditor);
        //void UnApplyToBrowser();
        QPixmap GetPixmap();
        void PositionChanged(QPointF point);
        void ZOrderChanged(double value);
        QString GetPropertyDescription(QtProperty* property);
        void ExpandedChanged(QtProperty* property, bool is_expanded);
        QList<QString> GetConnectedNames();
        QList<QString> GetDependentNames();
        QString GetInstanceName();
        void ApplyXmlProperties(CubesXml::Unit xu);
        void SetFileNames(QStringList fileNames);
        void SetFileName(QString fileName);
        void SetFileNameReadOnly(bool readOnly);
        QString GetFileName();
        void SetGroupNames(QStringList groupNames);
        void SetGroupName(QString groupName);
        void SetGroupNameReadOnly(bool readOnly);
        QString GetGroupName();
        void SetName(QString name);
        QString GetName();
        QString GetId() { return QString::fromStdString(unitParameters_.fileInfo.info.id); };
        //QString GetUnitName() { return parameters_compiler::helper::get_instance_name_initial(unitParameters_.fileInfo); };
        QList<QPair<QString, QString>> GetVariables();
        uint32_t GetPropertiesId() { return propertiesId_; };

    private slots:
        void ValueChanged(QtProperty* property, const QVariant& value);
        void StringEditingFinished(QtProperty* property, const QString& value, const QString& oldValue);

    private:
        void CreateParametersModel();
        void CreateProperties();
        void CreateParameterModel(const CubesUnitTypes::ParameterInfoId& parameterInfoId, const QString& parentModelId, CubesUnitTypes::ParameterModel& model);
        void FillParameterModel(CubesUnitTypes::ParameterModel& pm);
        void FillArrayModel(CubesUnitTypes::ParameterModel& pm);
        void UpdateArrayModel(CubesUnitTypes::ParameterModel& pm);

        void GetConnectedNamesInternal(const CubesUnitTypes::ParameterModel& model, QList<QString>& list);
        void GetDependentNamesInternal(const CubesUnitTypes::ParameterModel& model, QList<QString>& list);
        void ApplyXmlPropertiesInternal(CubesUnitTypes::ParameterModel& model, CubesXml::Unit& xu);

        void RegisterProperty(const QtProperty* property, const QString& id);
        void UnregisterProperty(const QString& id);
        void UnregisterProperty(const QtProperty* property);
        QtProperty* GetProperty(const QString& id);
        QString GetPropertyId(const QtProperty* property);
        CubesUnitTypes::ParameterModel* GetParameterModel(const QString& id);
        CubesUnitTypes::ParameterModel* GetParameterModel(const QtProperty* property);

        //void SaveExpandState();
        //void SaveExpandState(QtBrowserItem* index);
        void ApplyExpandState();
        void ApplyExpandState(QtBrowserItem* index);
    };
}