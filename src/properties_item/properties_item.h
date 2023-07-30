#pragma once

#include <QGraphicsItem>
#include <QObject>
#include <QPixmap>

#include "properties_items_manager_interface.h"
#include "../unit_types.h"
#include "../xml_parser.h"
#include "../property_browser/properties_editor.h"

class diagram_item;

class properties_item : public QObject
{
    Q_OBJECT

private:
    // Params
    properties_items_manager_interface* properties_items_manager_;
    QPointer<properties_editor> editor_;
    uint32_t propertiesId_;
    unit_types::UnitParameters unitParameters_;

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

    //unit_types::ParametersModel parametersModel_;
    //unit_types::ParametersModel editorModel_;

    //diagram_item* diagramItem_;
    //QtTreePropertyBrowser* propertyEditor_;

    //bool ignoreEvents_;
    //QList<QPair<QString, QString>> groupList_;

public:
    properties_item(properties_items_manager_interface* properties_items_manager, properties_editor* editor,
        unit_types::UnitParameters unitParameters, uint32_t propertiesId);
    //properties_item(unit_types::UnitParameters unitParameters, diagram_item* diagramItem, QObject* parent = nullptr);
    //properties_item(const properties_item& other, diagram_item* diagramItem);
    ~properties_item();

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
    void ApplyXmlProperties(xml::Unit xu);
    void SetFileNames(QStringList fileNames);
    void SetFileName(QString fileName);
    void SetFileNameReadOnly(bool readonly);
    QString GetFileName();
    void SetGroupNames(QStringList groupNames);
    void SetGroupName(QString groupName);
    void SetGroupNameReadOnly(bool readonly);
    QString GetGroupName();
    void SetName(QString name);
    QString GetName();
    QString GetId() { return QString::fromStdString(unitParameters_.fileInfo.info.id); };
    //QString GetUnitName() { return parameters_compiler::helper::get_instance_name_initial(unitParameters_.fileInfo); };
    QList<QPair<QString, QString>> GetVariables();

private slots:
    void ValueChanged(QtProperty* property, const QVariant& value);
    void StringEditingFinished(QtProperty* property, const QString& value, const QString& oldValue);

private:
    void CreateParametersModel();
    void CreateProperties();
    void CreateParameterModel(const unit_types::ParameterInfoId& parameterInfoId, const QString& parentModelId, unit_types::ParameterModel& model);
    void FillParameterModel(unit_types::ParameterModel& pm);
    void FillArrayModel(unit_types::ParameterModel& pm);
    void UpdateArrayModel(unit_types::ParameterModel& pm);
    void CreateEditorModel();
    //QtProperty* GetPropertyForModel(unit_types::ParameterModel& model);
    //void CreatePropertyBrowser();

    void GetConnectedNamesInternal(const unit_types::ParameterModel& model, QList<QString>& list);
    void GetDependentNamesInternal(const unit_types::ParameterModel& model, QList<QString>& list);
    void ApplyXmlPropertiesInternal(unit_types::ParameterModel& model, xml::Unit& xu);

    void RegisterProperty(const QtProperty* property, const QString& id);
    void UnregisterProperty(const QString& id);
    void UnregisterProperty(const QtProperty* property);
    QtProperty* GetProperty(const QString& id);
    QString GetPropertyId(const QtProperty* property);
    unit_types::ParameterModel* GetParameterModel(const QString& id);
    unit_types::ParameterModel* GetParameterModel(const QtProperty* property);

    //void SaveExpandState();
    //void SaveExpandState(QtBrowserItem* index);
    void ApplyExpandState();
    void ApplyExpandState(QtBrowserItem* index);
};
