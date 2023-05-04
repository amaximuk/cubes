#include <QPainter>
#include <QDebug>
#include <QRegularExpression>

#include "diagram_scene.h"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "main_window.h"
#include "base64.h"
#include "diagram_item.h"
#include "parameters_compiler_helper.h"
#include "group_item.h"

group_item::group_item(QObject* parent):
    QObject(parent)
{
    //diagramItem_ = diagramItem;
    parametersModel_ = {};
    //editorModel_ = {};
    propertyEditor_ = nullptr;
    ignoreEvents_ = false;

    CreateParametersModel();
    CreateEditorModel();
    CreatePropertyBrowser();
}

void group_item::CreateEditorModel()
{
    unit_types::ParameterModel editor_group;
    editor_group.id = "EDITOR";
    editor_group.name = QString::fromLocal8Bit("��������");
    editor_group.value = "";
    editor_group.valueType = "none";
    //editor_group.parameterInfoId = "";
    editor_group.editorSettings.type = unit_types::EditorType::None;
    editor_group.editorSettings.is_expanded = true;

    {
        unit_types::ParameterModel pm;
        pm.id = "EDITOR/COLOR";
        pm.name = QString::fromLocal8Bit("����");
        pm.value = QColor("Red").rgba();
        pm.valueType = "string";
        //pm.parameterInfoId = "";
        pm.editorSettings.type = unit_types::EditorType::Color;
        editor_group.parameters.push_back(std::move(pm));
    }

    parametersModel_.parameters.push_back(std::move(editor_group));
}

void group_item::CreateParametersModel()
{
    unit_types::ParameterModel base_group;
    base_group.id = "BASE";
    base_group.name = QString::fromLocal8Bit("�������");
    base_group.value = "";
    base_group.valueType = "none";
    //base_group.parameterInfoId = "";
    base_group.editorSettings.type = unit_types::EditorType::None;
    base_group.editorSettings.is_expanded = true;

    unit_types::ParameterModel name;
    name.id = "BASE/NAME";
    name.name = QString::fromLocal8Bit("���");
    name.value = QString::fromLocal8Bit("���");
    name.valueType = "string";
    //name.parameterInfoId = "";
    name.editorSettings.type = unit_types::EditorType::String;
    name.editorSettings.is_expanded = false;
    base_group.parameters.push_back(std::move(name));

    unit_types::ParameterModel file;
    file.id = "BASE/FILE";
    file.name = QString::fromLocal8Bit("����");
    file.value = "";
    file.valueType = "string";
    //file.parameterInfoId = "";
    file.editorSettings.type = unit_types::EditorType::ComboBox;
    file.editorSettings.is_expanded = false;
    base_group.parameters.push_back(std::move(file));

    parametersModel_.parameters.push_back(std::move(base_group));

    unit_types::ParameterModel properties_group;
    properties_group.id = "PARAMETERS";
    properties_group.name = QString::fromLocal8Bit("���������");
    properties_group.value = "";
    properties_group.valueType = "none";
    //properties_group.parameterInfoId = "";
    properties_group.editorSettings.type = unit_types::EditorType::None;
    properties_group.editorSettings.is_expanded = true;

    unit_types::ParameterModel file_path;
    file_path.id = "PARAMETERS/FILE_PATH";
    file_path.name = QString::fromLocal8Bit("��� �����");
    file_path.value = QString::fromLocal8Bit("config.xml");
    file_path.valueType = "string";
    file_path.editorSettings.type = unit_types::EditorType::String;
    file_path.editorSettings.is_expanded = false;
    properties_group.parameters.push_back(std::move(file_path));

    unit_types::ParameterModel variables;
    variables.id = "PARAMETERS/VARIABLES";
    variables.name = QString::fromLocal8Bit("����������");
    variables.value = 0;
    variables.valueType = "int";
    variables.editorSettings.type = unit_types::EditorType::SpinInterger;
    variables.editorSettings.is_expanded = true;
    variables.editorSettings.SpinIntergerMin = 0;
    variables.editorSettings.SpinIntergerMax = 100;
    properties_group.parameters.push_back(std::move(variables));

    parametersModel_.parameters.push_back(std::move(properties_group));
}

//QVariant GetValue(const QString& type, const QString& value)
//{
//    bool is_array = parameters_compiler::helper::is_array_type(type.toStdString());
//    if (is_array)
//    {
//        value = parameters_compiler::helper::get_parameter_initial<int>(unitParameters_.fiileInfo, pi);
//    }
//}

void group_item::CreateParameterModel(const parameters_compiler::parameter_info pi, const QString& parent_model_id, unit_types::ParameterModel& model)
{
    //bool is_array = parameters_compiler::helper::is_array_type(pi.type);

    //unit_types::ParameterModel pm;
    //pm.id = QString("%1/%2").arg(parent_model_id, QString::fromStdString(pi.name));
    //pm.parameterInfo = pi;

    //if (is_array)
    //{
    //    if (pi.restrictions.set_count.size() > 0)
    //    {
    //        pm.editorSettings.type = unit_types::EditorType::ComboBox;
    //        for (const auto& s : pi.restrictions.set_count)
    //            pm.editorSettings.ComboBoxValues.push_back(QString::fromStdString(s));

    //        pm.value = parameters_compiler::helper::get_parameter_initial<int>(unitParameters_.fiileInfo, pi);
    //    }
    //    else
    //    {
    //        pm.editorSettings.type = unit_types::EditorType::SpinInterger;

    //        if (pi.restrictions.min_count != "")
    //            pm.editorSettings.SpinIntergerMin = std::stoi(pi.restrictions.min_count);
    //        else
    //            pm.editorSettings.SpinIntergerMin = 0;

    //        pm.value = parameters_compiler::helper::get_parameter_initial<int>(unitParameters_.fiileInfo, pi);

    //        if (pi.restrictions.max_count != "")
    //            pm.editorSettings.SpinIntergerMax = std::stoi(pi.restrictions.max_count);
    //        else
    //            pm.editorSettings.SpinIntergerMax = 1000; // !!! todo: make a define for a const
    //    }

    //    UpdateArrayModel(pm);
    //}
    //else
    //{
    //    // "unit", "path", "string", "double", "int", "bool", "float", "int8_t", "int16_t", "int32_t", "int64_t", "uint8_t", "uint16_t", "uint32_t", "uint64_t"
    //    if (pi.restrictions.set_.size() > 0)
    //    {
    //        pm.editorSettings.type = unit_types::EditorType::ComboBox;
    //        for (const auto& s : pi.restrictions.set_)
    //            pm.editorSettings.ComboBoxValues.push_back(QString::fromStdString(s));

    //        if (pi.type == "unit" || pi.type == "path" || pi.type == "string")
    //            pm.value = QString::fromStdString(parameters_compiler::helper::get_parameter_initial<std::string>(unitParameters_.fiileInfo, pi));
    //        else if (pi.type == "int" || pi.type == "int8_t" || pi.type == "int16_t" || pi.type == "int32_t" ||
    //            pi.type == "int64_t" || pi.type == "uint8_t" || pi.type == "uint16_t" || pi.type == "uint32_t" || pi.type == "uint64_t")
    //            pm.value = parameters_compiler::helper::get_parameter_initial<int>(unitParameters_.fiileInfo, pi);
    //        else if (pi.type == "double" || pi.type == "float")
    //            pm.value = parameters_compiler::helper::get_parameter_initial<double>(unitParameters_.fiileInfo, pi);
    //        else // enum
    //            pm.value = parameters_compiler::helper::get_parameter_initial<int>(unitParameters_.fiileInfo, pi);
    //    }
    //    else
    //    {
    //        if (pi.type == "unit")
    //        {
    //            pm.editorSettings.type = unit_types::EditorType::String;
    //            pm.value = QString::fromStdString(parameters_compiler::helper::get_parameter_initial<std::string>(unitParameters_.fiileInfo, pi));

    //            unit_types::ParameterModel pm_depends;
    //            pm_depends.id = QString("%1/%2").arg(pm.id, "DEPENDS");
    //            pm_depends.parameterInfo.display_name = QString::fromLocal8Bit("�����������").toStdString();
    //            pm_depends.editorSettings.type = unit_types::EditorType::CheckBox;
    //            pm_depends.value = false;
    //            pm.parameters.push_back(pm_depends);
    //        }
    //        else if (pi.type == "path" || pi.type == "string")
    //        {
    //            pm.editorSettings.type = unit_types::EditorType::String;
    //            pm.value = QString::fromStdString(parameters_compiler::helper::get_parameter_initial<std::string>(unitParameters_.fiileInfo, pi));
    //        }
    //        else if (pi.type == "bool")
    //        {
    //            pm.editorSettings.type = unit_types::EditorType::CheckBox;
    //            pm.value = parameters_compiler::helper::get_parameter_initial<bool>(unitParameters_.fiileInfo, pi);
    //        }
    //        else if (pi.type == "int" || pi.type == "int8_t" || pi.type == "int16_t" || pi.type == "int32_t" ||
    //            pi.type == "int64_t" || pi.type == "uint8_t" || pi.type == "uint16_t" || pi.type == "uint32_t" || pi.type == "uint64_t")
    //        {
    //            pm.editorSettings.type = unit_types::EditorType::SpinInterger;

    //            if (pi.restrictions.min != "")
    //                pm.editorSettings.SpinIntergerMin = std::stoi(pi.restrictions.min);
    //            else
    //                pm.editorSettings.SpinIntergerMin = unit_types::GetMinForIntegralType(QString::fromStdString(pi.type));

    //            pm.value = parameters_compiler::helper::get_parameter_initial<int>(unitParameters_.fiileInfo, pi);

    //            if (pi.restrictions.max != "")
    //                pm.editorSettings.SpinIntergerMax = std::stoi(pi.restrictions.max);
    //            else
    //                pm.editorSettings.SpinIntergerMax = unit_types::GetMaxForIntegralType(QString::fromStdString(pi.type));
    //        }
    //        else if (pi.type == "double" || pi.type == "float")
    //        {
    //            pm.editorSettings.type = unit_types::EditorType::SpinDouble;

    //            if (pi.restrictions.min != "")
    //                pm.editorSettings.SpinDoubleMin = std::stod(pi.restrictions.min);
    //            else
    //                pm.editorSettings.SpinDoubleMin = unit_types::GetMinForFloatingPointType(QString::fromStdString(pi.type));

    //            pm.value = parameters_compiler::helper::get_parameter_initial<double>(unitParameters_.fiileInfo, pi);

    //            if (pi.restrictions.max != "")
    //                pm.editorSettings.SpinDoubleMax = std::stod(pi.restrictions.max);
    //            else
    //                pm.editorSettings.SpinDoubleMax = unit_types::GetMinForFloatingPointType(QString::fromStdString(pi.type));
    //        }
    //        else
    //        {
    //            // enum user type
    //            const auto pti = parameters_compiler::helper::get_type_info(unitParameters_.fiileInfo, pi.type);
    //            if (pti->type == "enum")
    //            {
    //                pm.editorSettings.type = unit_types::EditorType::ComboBox;
    //                if (pti->values.size() > 0)
    //                {
    //                    for (const auto v : pti->values)
    //                        pm.editorSettings.ComboBoxValues.push_back(QString::fromStdString(v.first));

    //                    pm.value = parameters_compiler::helper::get_parameter_initial<int>(unitParameters_.fiileInfo, pi);
    //                }
    //            }
    //            else assert(false);
    //        }
    //    }

    //    if (parameters_compiler::helper::get_parameter_optional(pi))
    //    {
    //        unit_types::ParameterModel pm_optional;
    //        pm_optional.id = QString("%1/%2").arg(pm.id, "OPTIONAL");
    //        pm_optional.parameterInfo.display_name = QString::fromLocal8Bit("�� ��������").toStdString();
    //        pm_optional.editorSettings.type = unit_types::EditorType::CheckBox;
    //        pm_optional.value = false;
    //        pm.parameters.push_back(pm_optional);
    //    }
    //}

    //model = pm;
}

QtProperty* group_item::GetPropertyForModel(unit_types::ParameterModel& model)
{
    // None, String, SpinInterger, SpinDouble, ComboBox, CheckBox

    QtProperty* pr = nullptr;
    if (model.editorSettings.type == unit_types::EditorType::None)
    {
        pr = groupManager->addProperty(model.name);
        groupManager->blockSignals(true);
        groupManager->blockSignals(false);
    }
    else if (model.editorSettings.type == unit_types::EditorType::String)
    {
        pr = stringManager->addProperty(model.name);
        stringManager->blockSignals(true);
        //stringManager->setRegExp(pr, QRegExp("-?\\d{1,3}"));
        stringManager->setValue(pr, model.value.toString());
        stringManager->blockSignals(false);
    }
    else if (model.editorSettings.type == unit_types::EditorType::SpinInterger)
    {
        pr = intManager->addProperty(model.name);
        intManager->blockSignals(true);
        intManager->setRange(pr, model.editorSettings.SpinIntergerMin, model.editorSettings.SpinIntergerMax);
        intManager->setValue(pr, model.value.toInt());
        intManager->blockSignals(false);
    }
    else if (model.editorSettings.type == unit_types::EditorType::SpinDouble)
    {
        pr = doubleManager->addProperty(model.name);
        doubleManager->blockSignals(true);
        doubleManager->setRange(pr, model.editorSettings.SpinDoubleMin, model.editorSettings.SpinDoubleMax);
        doubleManager->setSingleStep(pr, model.editorSettings.SpinDoubleSingleStep);
        doubleManager->setValue(pr, model.value.toDouble());
        doubleManager->blockSignals(false);
    }
    else if (model.editorSettings.type == unit_types::EditorType::ComboBox)
    {
        pr = enumManager->addProperty(model.name);
        enumManager->blockSignals(true);
        enumManager->setEnumNames(pr, model.editorSettings.ComboBoxValues);
        
        int pos = 0;
        for (; pos < model.editorSettings.ComboBoxValues.size(); ++pos)
        {
            if (model.valueType == "double" && model.value.toDouble() == std::stod(model.editorSettings.ComboBoxValues[pos].toStdString()))
                break;
            else if (model.valueType == "int" && model.value.toInt() == std::stoi(model.editorSettings.ComboBoxValues[pos].toStdString()))
                break;
            else if (model.valueType == "bool" && model.value.toBool() == (model.editorSettings.ComboBoxValues[pos] == "true"))
                break;
            else if (model.valueType == "string" && model.value.toString() == model.editorSettings.ComboBoxValues[pos])
                break;
        }

        //int pos = model.editorSettings.ComboBoxValues.indexOf(model.value.toString(), 0);
        if (pos == model.editorSettings.ComboBoxValues.size())
            pos = 0;

        enumManager->setValue(pr, pos);
        enumManager->blockSignals(false);
    }
    else if (model.editorSettings.type == unit_types::EditorType::CheckBox)
    {
        pr = boolManager->addProperty(model.name);
        boolManager->blockSignals(true);
        boolManager->setValue(pr, model.value.toBool());
        boolManager->blockSignals(false);
    }
    else if (model.editorSettings.type == unit_types::EditorType::Color)
    {
        pr = colorManager->addProperty(model.name);
        colorManager->blockSignals(true);
        colorManager->setValue(pr, QColor::fromRgba(model.value.toUInt()));
        colorManager->blockSignals(false);
    }
    else assert(false);

    RegisterProperty(pr, model.id);

    for (auto& sp : model.parameters)
    {
        pr->addSubProperty(GetPropertyForModel(sp));
    }

    return pr;
}

void group_item::CreatePropertyBrowser()
{
    groupManager = new QtGroupPropertyManager(this);
    intManager = new QtIntPropertyManager(this);
    doubleManager = new QtDoublePropertyManager(this);
    stringManager = new QtStringPropertyManager(this);
    enumManager = new QtEnumPropertyManager(this);
    boolManager = new QtBoolPropertyManager(this);
    colorManager = new QtColorPropertyManager(this);

    qDebug() << connect(intManager, SIGNAL(valueChanged(QtProperty*, int)), this, SLOT(valueChanged(QtProperty*, int)));
    qDebug() << connect(doubleManager, SIGNAL(valueChanged(QtProperty*, double)), this, SLOT(valueChanged(QtProperty*, double)));
    qDebug() << connect(stringManager, SIGNAL(valueChanged(QtProperty*, const QString&)), this, SLOT(valueChanged(QtProperty*, const QString&)));
    qDebug() << connect(enumManager, SIGNAL(valueChanged(QtProperty*, int)), this, SLOT(valueChanged(QtProperty*, int)));
    qDebug() << connect(boolManager, SIGNAL(valueChanged(QtProperty*, bool)), this, SLOT(valueChanged(QtProperty*, bool)));
    qDebug() << connect(colorManager, SIGNAL(valueChanged(QtProperty*, const QColor&)), this, SLOT(valueChanged(QtProperty*, const QColor&)));
}

QString group_item::GetName()
{
    auto pm = GetParameterModel("BASE/NAME");
    if (pm == nullptr)
        return "";

    return pm->value.toString();
};

QColor group_item::GetColor()
{
    auto pm = GetParameterModel("EDITOR/COLOR");
    if (pm == nullptr)
        return "";

    return QColor::fromRgba(pm->value.toInt());
};

void group_item::ApplyToBrowser(QtTreePropertyBrowser* propertyEditor)
{
    propertyEditor_ = propertyEditor;

    QtSpinBoxFactory* intSpinBoxFactory = new QtSpinBoxFactory(this);
    QtDoubleSpinBoxFactory* doubleSpinBoxFactory = new QtDoubleSpinBoxFactory(this);
    QtCheckBoxFactory* checkBoxFactory = new QtCheckBoxFactory(this);
    QtSpinBoxFactory* spinBoxFactory = new QtSpinBoxFactory(this);
    QtLineEditFactory* lineEditFactory = new QtLineEditFactory(this);
    QtEnumEditorFactory* comboBoxFactory = new QtEnumEditorFactory(this);
    QtColorEditorFactory* colorEditorFactory = new QtColorEditorFactory(this);

    propertyEditor->setFactoryForManager(intManager, intSpinBoxFactory);
    propertyEditor->setFactoryForManager(doubleManager, doubleSpinBoxFactory);
    propertyEditor->setFactoryForManager(stringManager, lineEditFactory);
    propertyEditor->setFactoryForManager(enumManager, comboBoxFactory);
    propertyEditor->setFactoryForManager(boolManager, checkBoxFactory);
    propertyEditor->setFactoryForManager(colorManager, colorEditorFactory);


    propertyEditor->setResizeMode(QtTreePropertyBrowser::ResizeMode::Interactive);
    propertyEditor->setSplitterPosition(250);
    propertyEditor->setHeaderVisible(false);
    //propertyEditor->setResizeMode(QtTreePropertyBrowser::ResizeMode::ResizeToContents);
    //propertyEditor->setPropertiesWithoutValueMarked(true);

    propertyEditor->clear();

    //QtProperty* mainGroup = groupManager->addProperty("Id");
    //
    //QtProperty* propertiesGroup = groupManager->addProperty(QString::fromLocal8Bit("��������"));
    //mainGroup->addSubProperty(propertiesGroup);

    //for (auto& pm : parametersModel_.parameters)
    //    propertiesGroup->addSubProperty(GetPropertyForModel(pm));
    
    ignoreEvents_ = true;
    for (auto& pm : parametersModel_.parameters)
        propertyEditor->addProperty(GetPropertyForModel(pm));
    //propertyEditor->addProperty(mainGroup);
    ignoreEvents_ = false;

    ApplyExpandState();
}

QPixmap group_item::GetPixmap()
{
    QPixmap px;
    bool loaded = false;
    //if (unitParameters_.fiileInfo.info.pictogram != "")
    //{
    //    std::string s = base64_decode(unitParameters_.fiileInfo.info.pictogram);
    //    QByteArray ba(s.c_str(), static_cast<int>(s.size()));
    //    try
    //    {
    //        loaded = px.loadFromData(ba);
    //    }
    //    catch (...)
    //    {
    //        loaded = false;
    //    }
    //}
    if (!loaded)
        px.load(":/images/ice.png");
    return px;
}

void group_item::PositionChanged(QPointF point)
{
    doubleManager->setValue(GetProperty("EDITOR/POSITION_X"), point.x());
    doubleManager->setValue(GetProperty("EDITOR/POSITION_Y"), point.y());
}

void group_item::ZOrderChanged(double value)
{
    doubleManager->setValue(GetProperty("EDITOR/POSITION_Z"), value);
}

QString group_item::GetPropertyDescription(QtProperty* property)
{
    QString id = GetPropertyId(property);
    return id;
}

void group_item::ExpandedChanged(QtProperty* property, bool is_expanded)
{
    if (!ignoreEvents_)
    {
        auto pm = GetParameterModel(property);
        if (pm != nullptr)
            pm->editorSettings.is_expanded = is_expanded;
    }
}

void group_item::SetName(QString name)
{
    auto pm = GetParameterModel("BASE/NAME");
    if (pm == nullptr)
        return;
    pm->value = name;
}

void group_item::SetColor(QColor color)
{
    auto pm = GetParameterModel("EDITOR/COLOR");
    if (pm == nullptr)
        return;
    pm->value = color.rgba();
}

void group_item::SetFileNames(QStringList fileNames)
{
    const auto pm = GetParameterModel("BASE/FILE");
    //int index = pm->editorSettings.ComboBoxValues.indexOf(pm->value.toString());
    if (pm != nullptr)
    {
        pm->editorSettings.ComboBoxValues = fileNames;
        if (pm->value.toString() == "" && fileNames.size() > 0)
            pm->value = fileNames[0];
    }
}

void group_item::SetFileName(QString fileName)
{
    const auto pm = GetParameterModel("BASE/FILE");
    if (pm != nullptr)
        pm->value = fileName;
}

void group_item::SetFileNameReadOnly()
{
    const auto pm = GetParameterModel("BASE/FILE");
    if (pm != nullptr)
        pm->readOnly = true;
}

QString group_item::GetFileName()
{
    const auto pm = GetParameterModel("BASE/FILE");
    if (pm != nullptr)
        return pm->value.toString();
    else
        return "";
}

void group_item::UpdateArrayModel(unit_types::ParameterModel& pm)
{
    for (int i = pm.parameters.size(); i < pm.value.toInt(); ++i)
    {
        unit_types::ParameterModel group_model;
        group_model.editorSettings.type = unit_types::EditorType::None;
        group_model.id = QString("%1/%2_%3").arg(pm.id, "ITEM").arg(i);
        group_model.name = QString::fromLocal8Bit("������� %1").arg(i);

        unit_types::ParameterModel name;
        name.editorSettings.type = unit_types::EditorType::String;
        name.id = QString("%1/%2").arg(group_model.id, "NAME");
        name.name = QString::fromLocal8Bit("���");
        group_model.parameters.push_back(name);

        unit_types::ParameterModel value;
        value.editorSettings.type = unit_types::EditorType::String;
        value.id = QString("%1/%2").arg(group_model.id, "VALUE");
        value.name = QString::fromLocal8Bit("��������");
        group_model.parameters.push_back(value);

        pm.parameters.push_back(group_model);
    }

    while (pm.parameters.size() > pm.value.toInt())
        pm.parameters.pop_back();
}

void group_item::valueChanged(QtProperty* property, int value)
{
    qDebug() << "valueChanged value = " << value;

    auto pm = GetParameterModel(property);
    if (pm == nullptr)
        return;

    if (pm->id.startsWith("BASE"))
    {
        pm->value = property->valueText();
    }
    else if (pm->id == "PARAMETERS/VARIABLES")
    {
        SaveExpandState();

        int count = std::stoi(property->valueText().toStdString());
        pm->value = count;
        UpdateArrayModel(*pm);

        for (int i = property->subProperties().size(); i < count; ++i)
            property->addSubProperty(GetPropertyForModel(pm->parameters[i]));

        QList<QtProperty*> to_remove;
        for (int i = count; i < property->subProperties().size(); ++i)
        {
            auto p = property->subProperties()[i];
            to_remove.push_back(p);
            UnregisterProperty(p);
        }

        for (auto& p : to_remove)
            property->removeSubProperty(p);

        ApplyExpandState();
    }
    else
    {
        if (pm->valueType == "unit" || pm->valueType == "path" || pm->valueType == "string")
            pm->value = property->valueText();
        else if (pm->valueType == "int" || pm->valueType == "int8_t" || pm->valueType == "int16_t" || pm->valueType == "int32_t" ||
            pm->valueType == "int64_t" || pm->valueType == "uint8_t" || pm->valueType == "uint16_t" || pm->valueType == "uint32_t" || pm->valueType == "uint64_t")
            pm->value = std::stoi(property->valueText().toStdString());
        else if (pm->valueType == "double" || pm->valueType == "float")
            pm->value = std::stod(property->valueText().toStdString());
        else // enum
            pm->value = property->valueText();

    }
}

void group_item::valueChanged(QtProperty* property, double value)
{
    auto pm = GetParameterModel(property);
    if (pm == nullptr)
        return;

    qDebug() << "valueChanged " << pm->id << " = " << value;
    pm->value = value;

    int gridSize = 20;
    if (pm->id == "EDITOR/POSITION_X")
    {
        qreal xV = round(value / gridSize) * gridSize;
        if (xV != value)
            doubleManager->setValue(property, xV);
        //diagramItem_->InformPositionXChanged(xV);
        pm->value = xV;
    }
    else if (pm->id == "EDITOR/POSITION_Y")
    {
        qreal yV = round(value / gridSize) * gridSize;
        if (yV != value)
            doubleManager->setValue(property, yV);
        //diagramItem_->InformPositionYChanged(yV);
        pm->value = yV;
    }
    else if (pm->id == "EDITOR/POSITION_Z")
    {
        //diagramItem_->InformPositionZChanged(value);
    }

    
    //if (!propertyToId.contains(property))
    //    return;

    ////if (!currentItem)
    ////    return;

    //if (scene_->selectedItems().count() > 0 && !scene_->isItemMoving())
    //{
    //    //diagram_item* gi = qobject_cast<diagram_item*>(sp_scene_->selectedItems()[0]);
    //    diagram_item* gi = (diagram_item*)(scene_->selectedItems()[0]);
    //    qDebug() << gi->getName();

    //    QString id = propertyToId[property];
    //    if (id == "Position X")
    //        gi->setX(value);
    //    else if (id == "Position Y")
    //        gi->setY(value);
    //}
}

void group_item::valueChanged(QtProperty* property, const QString& value)
{
    auto pm = GetParameterModel(property);
    if (pm == nullptr)
        return;

    qDebug() << "valueChanged " << pm->id << " = " << value;
    pm->value = value;

    //    if (!propertyToId.contains(property))
    //        return;

    //    if (!currentItem)
    //        return;

    //    QString id = propertyToId[property];
    //    if (id == QLatin1String("text")) {
    //        if (currentItem->rtti() == QtCanvasItem::Rtti_Text) {
    //            QtCanvasText *i = (QtCanvasText *)currentItem;
    //            i->setText(value);
    //        }
    //    }
    //    canvas->update();
}

void group_item::valueChanged(QtProperty* property, const QColor& value)
{
    auto pm = GetParameterModel(property);
    if (pm == nullptr)
        return;

    qDebug() << "valueChanged " << pm->id << " = " << value;
    pm->value = value.rgba();


    //    if (!propertyToId.contains(property))
    //        return;

    //    if (!currentItem)
    //        return;

    //    QString id = propertyToId[property];
    //    if (id == QLatin1String("color")) {
    //        if (currentItem->rtti() == QtCanvasItem::Rtti_Text) {
    //            QtCanvasText *i = (QtCanvasText *)currentItem;
    //            i->setColor(value);
    //        }
    //    } else if (id == QLatin1String("brush")) {
    //        if (currentItem->rtti() == QtCanvasItem::Rtti_Rectangle ||
    //                currentItem->rtti() == QtCanvasItem::Rtti_Ellipse) {
    //            QtCanvasPolygonalItem *i = (QtCanvasPolygonalItem *)currentItem;
    //            QBrush b = i->brush();
    //            b.setColor(value);
    //            i->setBrush(b);
    //        }
    //    } else if (id == QLatin1String("pen")) {
    //        if (currentItem->rtti() == QtCanvasItem::Rtti_Rectangle ||
    //                currentItem->rtti() == QtCanvasItem::Rtti_Line) {
    //            QtCanvasPolygonalItem *i = (QtCanvasPolygonalItem *)currentItem;
    //            QPen p = i->pen();
    //            p.setColor(value);
    //            i->setPen(p);
    //        }
    //    }
    //    canvas->update();
}

void group_item::valueChanged(QtProperty* property, const QFont& value)
{
    //    if (!propertyToId.contains(property))
    //        return;

    //    if (!currentItem)
    //        return;

    //    QString id = propertyToId[property];
    //    if (id == QLatin1String("font")) {
    //        if (currentItem->rtti() == QtCanvasItem::Rtti_Text) {
    //            QtCanvasText *i = (QtCanvasText *)currentItem;
    //            i->setFont(value);
    //        }
    //    }
    //    canvas->update();
}

void group_item::valueChanged(QtProperty* property, const QPoint& value)
{
    //    if (!propertyToId.contains(property))
    //        return;

    //    if (!currentItem)
    //        return;

    //    QString id = propertyToId[property];
    //    if (currentItem->rtti() == QtCanvasItem::Rtti_Line) {
    //        QtCanvasLine *i = (QtCanvasLine *)currentItem;
    //        if (id == QLatin1String("endpoint")) {
    //            i->setPoints(i->startPoint().x(), i->startPoint().y(), value.x(), value.y());
    //        }
    //    }
    //    canvas->update();
}

void group_item::valueChanged(QtProperty* property, const QSize& value)
{
    //    if (!propertyToId.contains(property))
    //        return;

    //    if (!currentItem)
    //        return;

    //    QString id = propertyToId[property];
    //    if (id == QLatin1String("size")) {
    //        if (currentItem->rtti() == QtCanvasItem::Rtti_Rectangle) {
    //            QtCanvasRectangle *i = (QtCanvasRectangle *)currentItem;
    //            i->setSize(value.width(), value.height());
    //        } else if (currentItem->rtti() == QtCanvasItem::Rtti_Ellipse) {
    //            QtCanvasEllipse *i = (QtCanvasEllipse *)currentItem;
    //            i->setSize(value.width(), value.height());
    //        }
    //    }
    //    canvas->update();
}

void group_item::valueChanged(QtProperty* property, bool value)
{
    auto pm = GetParameterModel(property);
    if (pm == nullptr)
        return;

    qDebug() << "valueChanged " << pm->id << " = " << value;
    pm->value = value;
}

void group_item::RegisterProperty(QtProperty* property, const QString& id)
{
    propertyToId[property] = id;
    idToProperty[id] = property;
}

void group_item::UnregisterProperty(const QString& id)
{
    UnregisterProperty(idToProperty[id]);
}

void group_item::UnregisterProperty(QtProperty* property)
{
    for (auto p : property->subProperties())
        UnregisterProperty(p);

    idToProperty.remove(propertyToId[property]);
    propertyToId.remove(property);
}

QtProperty* group_item::GetProperty(const QString& id)
{
    auto it = idToProperty.find(id);
    if (it != idToProperty.end())
        return *it;
    return nullptr;

    //if (idToProperty.contains(id))
    //    return idToProperty[id];
    //else
    //    return nullptr;
}

QString group_item::GetPropertyId(QtProperty* property)
{
    auto it = propertyToId.find(property);
    if (it != propertyToId.end())
        return *it;
    return QString();

    //if (propertyToId.contains(property))
    //    return propertyToId[property];
    //else
    //    return QString();
}

unit_types::ParameterModel* group_item::GetParameterModel(const QString& id)
{
    unit_types::ParameterModel* pm = nullptr;

    {
        QStringList sl = id.split("/");
        auto ql = &parametersModel_.parameters;
        QString idt;
        while (sl.size() > 0)
        {
            idt = idt == "" ? sl[0] : idt + "/" + sl[0];
            bool found = false;
            for (auto& x : *ql)
            {
                if (x.id == idt)
                {
                    pm = &x;
                    ql = &x.parameters;
                    sl.pop_front();
                    found = true;
                    break;
                }
            }
            if (!found)
                break;
        }
    }

    //if (pm != nullptr)
    //    return pm;

    //{
    //    QStringList sl = id.split("/");
    //    auto ql = &editorModel_.parameters;
    //    while (sl.size() > 0)
    //    {
    //        bool found = false;
    //        for (auto x : *ql)
    //        {
    //            if (x.id == sl[0])
    //            {
    //                pm = &x;
    //                ql = &x.parameters;
    //                sl.pop_front();
    //                found = true;
    //                break;
    //            }
    //        }
    //        if (!found)
    //            break;
    //    }
    //}

    return pm;
}

unit_types::ParameterModel* group_item::GetParameterModel(QtProperty* property)
{
    QString id = GetPropertyId(property);
    if (id == "")
        return nullptr;

    return GetParameterModel(id);
}

bool group_item::GetExpanded(QtProperty* property)
{
    return false;
}

void group_item::SaveExpandState(QtBrowserItem* index)
{
    if (propertyEditor_ == nullptr)
        return;

    QList<QtBrowserItem*> children = index->children();
    QListIterator<QtBrowserItem*> itChild(children);
    while (itChild.hasNext())
        SaveExpandState(itChild.next());
    QtProperty* prop = index->property();

    auto pm = GetParameterModel(prop);
    if (pm != nullptr)
        pm->editorSettings.is_expanded = propertyEditor_->isExpanded(index);
}

void group_item::SaveExpandState()
{
    if (propertyEditor_ == nullptr)
        return;

    QList<QtBrowserItem*> indexes = propertyEditor_->topLevelItems();
    QListIterator<QtBrowserItem*> itItem(indexes);
    while (itItem.hasNext())
        SaveExpandState(itItem.next());
}

void group_item::ApplyExpandState(QtBrowserItem* index)
{
    if (propertyEditor_ == nullptr)
        return;

    QList<QtBrowserItem*> children = index->children();
    QListIterator<QtBrowserItem*> itChild(children);
    while (itChild.hasNext())
        ApplyExpandState(itChild.next());
    QtProperty* prop = index->property();

    auto pm = GetParameterModel(prop);
    if (pm != nullptr)
        propertyEditor_->setExpanded(index, pm->editorSettings.is_expanded);
}

void group_item::ApplyExpandState()
{
    if (propertyEditor_ == nullptr)
        return;

    QList<QtBrowserItem*> indexes = propertyEditor_->topLevelItems();
    QListIterator<QtBrowserItem*> itItem(indexes);
    while (itItem.hasNext())
        ApplyExpandState(itItem.next());
}