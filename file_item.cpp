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
#include "file_item.h"

file_item::file_item(file_items_manager_interface* file_items_manager)
{
    file_items_manager_ = file_items_manager;
    //diagramItem_ = diagramItem;
    parametersModel_ = {};
    //editorModel_ = {};
    propertyEditor_ = nullptr;
    ignoreEvents_ = false;

    CreateParametersModel();
    CreateEditorModel();
    CreatePropertyBrowser();
}

void file_item::CreateEditorModel()
{
    unit_types::ParameterModel editor_group;
    editor_group.id = "EDITOR";
    editor_group.name = QString::fromLocal8Bit("Редактор");
    editor_group.value = "";
    editor_group.valueType = "none";
    //editor_group.parameterInfoId = "";
    editor_group.editorSettings.type = unit_types::EditorType::None;
    editor_group.editorSettings.is_expanded = true;

    {
        unit_types::ParameterModel pm;
        pm.id = "EDITOR/COLOR";
        pm.name = QString::fromLocal8Bit("Цвет");
        pm.value = QColor("Red").rgba();
        pm.valueType = "string";
        //pm.parameterInfoId = "";
        pm.editorSettings.type = unit_types::EditorType::Color;
        editor_group.parameters.push_back(std::move(pm));
    }

    parametersModel_.parameters.push_back(std::move(editor_group));
}

void file_item::CreateParametersModel()
{
    unit_types::ParameterModel base_group;
    base_group.id = "BASE";
    base_group.name = QString::fromLocal8Bit("Базовые");
    base_group.value = "";
    base_group.valueType = "none";
    //base_group.parameterInfoId = "";
    base_group.editorSettings.type = unit_types::EditorType::None;
    base_group.editorSettings.is_expanded = true;

    unit_types::ParameterModel name;
    name.id = "BASE/NAME";
    name.name = QString::fromLocal8Bit("Имя");
    name.value = QString::fromLocal8Bit("АРМ");
    name.valueType = "string";
    //name.parameterInfoId = "";
    name.editorSettings.type = unit_types::EditorType::String;
    name.editorSettings.is_expanded = false;
    base_group.parameters.push_back(std::move(name));

    unit_types::ParameterModel platform;
    platform.id = "BASE/PLATFORM";
    platform.name = QString::fromLocal8Bit("Платформа");
    platform.value = "";
    platform.valueType = "string";
    //platform.parameterInfoId = "";
    platform.editorSettings.type = unit_types::EditorType::ComboBox;
    platform.editorSettings.is_expanded = false;
    for(const auto& pl : unit_types::platform_names_)
        platform.editorSettings.ComboBoxValues.push_back(QString::fromStdString(pl));
    if (unit_types::platform_names_.size() > 0)
        platform.value = QString::fromStdString(unit_types::platform_names_[0]);
    base_group.parameters.push_back(std::move(platform));
    
    unit_types::ParameterModel file_path;
    file_path.id = "BASE/FILE_PATH";
    file_path.name = QString::fromLocal8Bit("Имя файла");
    file_path.value = QString::fromLocal8Bit("config.xml");
    file_path.valueType = "string";
    //file_path.parameterInfoId = "";
    file_path.editorSettings.type = unit_types::EditorType::String;
    file_path.editorSettings.is_expanded = false;
    base_group.parameters.push_back(std::move(file_path));
    parametersModel_.parameters.push_back(std::move(base_group));

    unit_types::ParameterModel includes;
    includes.id = "INCLUDES";
    includes.name = QString::fromLocal8Bit("Включаемые файлы");
    includes.value = 0;
    includes.valueType = "int";
    includes.editorSettings.type = unit_types::EditorType::SpinInterger;
    includes.editorSettings.is_expanded = true;
    includes.editorSettings.SpinIntergerMin = 0;
    includes.editorSettings.SpinIntergerMax = 100;
    parametersModel_.parameters.push_back(std::move(includes));


    unit_types::ParameterModel properties_group;
    properties_group.id = "PARAMETERS";
    properties_group.name = QString::fromLocal8Bit("Параметры");
    properties_group.value = "";
    properties_group.valueType = "none";
    //properties_group.parameterInfoId = "";
    properties_group.editorSettings.type = unit_types::EditorType::None;
    properties_group.editorSettings.is_expanded = true;

    {
        unit_types::ParameterModel pm_connection;
        pm_connection.id = "PARAMETERS/CONNECTION";
        pm_connection.name = QString::fromLocal8Bit("Соединение");
        pm_connection.value = "";
        pm_connection.valueType = "none";
        //pm_connection.parameterInfoId = "";
        pm_connection.editorSettings.type = unit_types::EditorType::None;

        unit_types::ParameterModel pm_host;
        pm_host.id = "PARAMETERS/CONNECTION/HOST";
        pm_host.name = QString::fromLocal8Bit("Хост");
        pm_host.value = "";
        pm_host.valueType= "string";
        //pm_host.parameterInfoId = "";
        pm_host.editorSettings.type = unit_types::EditorType::String;
        pm_host.value = "127.0.0.1";
        pm_connection.parameters.push_back(std::move(pm_host));

        unit_types::ParameterModel pm_port;
        pm_port.id = "PARAMETERS/CONNECTION/PORT";
        pm_port.name = QString::fromLocal8Bit("Порт");
        pm_port.value = 50000;
        pm_port.valueType = "int";
        //pm_port.parameterInfoId = "";
        pm_port.editorSettings.type = unit_types::EditorType::SpinInterger;
        pm_port.editorSettings.SpinIntergerMin = 1000;
        pm_port.editorSettings.SpinIntergerMin = 65535;
        pm_connection.parameters.push_back(std::move(pm_port));

        properties_group.parameters.push_back(std::move(pm_connection));
    }

    {
        unit_types::ParameterModel pm;
        pm.id = "PARAMETERS/LOG";
        pm.name = QString::fromLocal8Bit("Логирование");
        pm.value = "";
        pm.valueType = "none";
        //pm.parameterInfoId = "";
        pm.editorSettings.type = unit_types::EditorType::None;
        properties_group.parameters.push_back(pm);
    }

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

void file_item::CreateParameterModel(const parameters_compiler::parameter_info pi, const QString& parent_model_id, unit_types::ParameterModel& model)
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
    //            pm_depends.parameterInfo.display_name = QString::fromLocal8Bit("Зависимость").toStdString();
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
    //        pm_optional.parameterInfo.display_name = QString::fromLocal8Bit("Не задавать").toStdString();
    //        pm_optional.editorSettings.type = unit_types::EditorType::CheckBox;
    //        pm_optional.value = false;
    //        pm.parameters.push_back(pm_optional);
    //    }
    //}

    //model = pm;
}

QtProperty* file_item::GetPropertyForModel(unit_types::ParameterModel& model)
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
        //if (model.id == "BASE/NAME")
            stringManager->setRegExp(pr, QRegExp(model.editorSettings.RegExp));
            //stringManager->setRegExp(pr, QRegExp(model.editorSettings.RegExp, Qt::CaseSensitive, QRegExp::Wildcard));
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

void file_item::CreatePropertyBrowser()
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

QString file_item::GetName()
{
    auto pm = GetParameterModel("BASE/NAME");
    if (pm == nullptr)
        return "";

    return pm->value.toString();
};

QColor file_item::GetColor()
{
    auto pm = GetParameterModel("EDITOR/COLOR");
    if (pm == nullptr)
        return "";

    return QColor::fromRgba(pm->value.toInt());
};

void file_item::ApplyToBrowser(QtTreePropertyBrowser* propertyEditor)
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
    //QtProperty* propertiesGroup = groupManager->addProperty(QString::fromLocal8Bit("Свойства"));
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

void file_item::UpdateRegExp(QtBrowserItem* index)
{
    if (propertyEditor_ == nullptr)
        return;

    QList<QtBrowserItem*> children = index->children();
    QListIterator<QtBrowserItem*> itChild(children);
    while (itChild.hasNext())
        UpdateRegExp(itChild.next());
    QtProperty* prop = index->property();

    auto pm = GetParameterModel(prop);
    if (pm != nullptr && pm->valueType == "string")
        stringManager->setRegExp(prop, QRegExp(pm->editorSettings.RegExp));
}

void file_item::UpdateRegExp()
{
    if (propertyEditor_ == nullptr)
        return;

    QList<QtBrowserItem*> indexes = propertyEditor_->topLevelItems();
    QListIterator<QtBrowserItem*> itItem(indexes);
    while (itItem.hasNext())
        UpdateRegExp(itItem.next());
}

QPixmap file_item::GetPixmap()
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

void file_item::PositionChanged(QPointF point)
{
    doubleManager->setValue(GetProperty("EDITOR/POSITION_X"), point.x());
    doubleManager->setValue(GetProperty("EDITOR/POSITION_Y"), point.y());
}

void file_item::ZOrderChanged(double value)
{
    doubleManager->setValue(GetProperty("EDITOR/POSITION_Z"), value);
}

QString file_item::GetPropertyDescription(const QtProperty* property)
{
    QString id = GetPropertyId(property);
    return id;
}

void file_item::ExpandedChanged(const QtProperty* property, bool is_expanded)
{
    if (!ignoreEvents_)
    {
        auto pm = GetParameterModel(property);
        if (pm != nullptr)
            pm->editorSettings.is_expanded = is_expanded;
    }
}

void file_item::SetName(QString name)
{
    auto pm = GetParameterModel("BASE/NAME");
    if (pm == nullptr)
        return;
    pm->value = name;
}

void file_item::SetColor(QColor color)
{
    auto pm = GetParameterModel("EDITOR/COLOR");
    if (pm == nullptr)
        return;
    pm->value = color.rgba();
}

QStringList file_item::GetIncludeNames()
{
    QStringList result;

    const auto pm = GetParameterModel("INCLUDES");
    if (pm == nullptr)
        return result;

    for (int i = 0; i < pm->value.toInt(); i++)
    {
        const auto pmi = GetParameterModel(QString("INCLUDES/ITEM_%1/NAME").arg(i));
        result.push_back(pmi->value.toString());
    }

    return result;
}

void file_item::SetNameRegExp(QString regexp)
{
    auto pm = GetParameterModel("BASE/NAME");
    if (pm == nullptr)
        return;
    pm->editorSettings.RegExp = regexp;
}

void file_item::UpdateArrayModel(unit_types::ParameterModel& pm)
{
    if (pm.id == "INCLUDES")
    {
        for (int i = pm.parameters.size(); i < pm.value.toInt(); ++i)
        {
            unit_types::ParameterModel group_model;
            group_model.editorSettings.type = unit_types::EditorType::None;
            group_model.id = QString("%1/%2_%3").arg(pm.id, "ITEM").arg(i);
            group_model.name = QString::fromLocal8Bit("Элемент %1").arg(i);

            unit_types::ParameterModel name;
            name.editorSettings.type = unit_types::EditorType::String;
            name.id = QString("%1/%2").arg(group_model.id, "NAME");
            name.name = QString::fromLocal8Bit("Имя");
            name.value = QString::fromLocal8Bit("Файл %1").arg(i);
            name.valueType = "string";
            name.editorSettings.type = unit_types::EditorType::String;
            group_model.parameters.push_back(name);

            unit_types::ParameterModel file_path;
            file_path.editorSettings.type = unit_types::EditorType::String;
            file_path.id = QString("%1/%2").arg(group_model.id, "FILE_PATH");
            file_path.name = QString::fromLocal8Bit("Имя файла");
            file_path.value = QString::fromLocal8Bit("include_%1.xml").arg(i);
            file_path.valueType = "string";
            file_path.editorSettings.type = unit_types::EditorType::String;
            file_path.editorSettings.is_expanded = false;
            group_model.parameters.push_back(file_path);

            unit_types::ParameterModel variables;
            variables.id = QString("%1/%2").arg(group_model.id, "VARIABLES");
            variables.name = QString::fromLocal8Bit("Переменные");
            variables.value = 0;
            variables.valueType = "int";
            variables.editorSettings.type = unit_types::EditorType::SpinInterger;
            variables.editorSettings.is_expanded = true;
            variables.editorSettings.SpinIntergerMin = 0;
            variables.editorSettings.SpinIntergerMax = 100;
            group_model.parameters.push_back(std::move(variables));

            pm.parameters.push_back(group_model);
        }
    }
    else
    {
        for (int i = pm.parameters.size(); i < pm.value.toInt(); ++i)
        {
            unit_types::ParameterModel group_model;
            group_model.editorSettings.type = unit_types::EditorType::None;
            group_model.id = QString("%1/%2_%3").arg(pm.id, "ITEM").arg(i);
            group_model.name = QString::fromLocal8Bit("Элемент %1").arg(i);

            unit_types::ParameterModel name;
            name.editorSettings.type = unit_types::EditorType::String;
            name.id = QString("%1/%2").arg(group_model.id, "NAME");
            name.name = QString::fromLocal8Bit("Имя");
            group_model.parameters.push_back(name);

            unit_types::ParameterModel value;
            value.editorSettings.type = unit_types::EditorType::String;
            value.id = QString("%1/%2").arg(group_model.id, "VALUE");
            value.name = QString::fromLocal8Bit("Значение");
            group_model.parameters.push_back(value);

            pm.parameters.push_back(group_model);
        }
    }

    while (pm.parameters.size() > pm.value.toInt())
        pm.parameters.pop_back();

    //auto at = parameters_compiler::helper::get_array_type(pm.parameterInfo.type);
    //auto ti = parameters_compiler::helper::get_type_info(unitParameters_.fiileInfo, at);
    //if (parameters_compiler::helper::is_inner_type(at) || (ti != nullptr && ti->type == "enum"))
    //{
    //    for (int i = pm.parameters.size(); i < pm.value.toInt(); ++i)
    //    {
    //        parameters_compiler::parameter_info pi_new = pm.parameterInfo;
    //        pi_new.type = at;
    //        pi_new.name = QString("%1/%2_%3").arg(pm.id, "ITEM").arg(i).toStdString();
    //        pi_new.display_name = QString::fromLocal8Bit("Элемент %1").arg(i).toStdString();
    //        unit_types::ParameterModel model;
    //        CreateParameterModel(pi_new, pm.id, model);
    //        pm.parameters.push_back(model);
    //    }
    //}
    //else if (ti != nullptr) // yml type
    //{
    //    for (int i = pm.parameters.size(); i < pm.value.toInt(); ++i)
    //    {
    //        unit_types::ParameterModel group_model;
    //        group_model.editorSettings.type = unit_types::EditorType::None;
    //        group_model.id = QString("%1/%2_%3").arg(pm.id, "ITEM").arg(i);
    //        group_model.parameterInfo.display_name = QString::fromLocal8Bit("Элемент %1").arg(i).toStdString();
    //        for (auto p : ti->parameters)
    //        {
    //            unit_types::ParameterModel model;
    //            CreateParameterModel(p, group_model.id, model);
    //            group_model.parameters.push_back(model);
    //        }
    //        pm.parameters.push_back(group_model);
    //    }
    //}

    //while (pm.parameters.size() > pm.value.toInt())
    //    pm.parameters.pop_back();
}

void file_item::valueChanged(QtProperty* property, int value)
{
    qDebug() << "valueChanged value = " << value;

    auto pm = GetParameterModel(property);
    if (pm == nullptr)
        return;

    if (pm->id.startsWith("BASE"))
    {
        pm->value = property->valueText();
    }
    else if (pm->id.startsWith("INCLUDES"))
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

        // Обновляем regexp
        UpdateIncludeNameRegExp();

        // Сообщаем об изменении списка включаемых файлов
        file_items_manager_->InformIncludeChanged(GetName(), GetIncludeNames());
    }
    else if (pm->id.startsWith("PROPERTIES"))
    {
        bool is_array = parameters_compiler::helper::is_array_type(pm->valueType.toStdString());
        if (is_array)
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
}

void file_item::valueChanged(QtProperty* property, double value)
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

void file_item::valueChanged(QtProperty* property, const QString& value)
{
    auto pm = GetParameterModel(property);
    if (pm == nullptr)
        return;

    if (pm->id == "BASE/NAME")
    {
        QString oldName = pm->value.toString();
        pm->value = value;
        file_items_manager_->InformNameChanged(value, oldName);
    }
    if (pm->id.startsWith("INCLUDES/ITEM") && pm->id.endsWith("NAME"))
    {
        QString oldName = pm->value.toString();
        pm->value = value;
        file_items_manager_->InformIncludeNameChanged(GetName(), value, oldName);
    }
    else
    {
        pm->value = value;
    }
}

void file_item::valueChanged(QtProperty* property, const QColor& value)
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

void file_item::valueChanged(QtProperty* property, const QFont& value)
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

void file_item::valueChanged(QtProperty* property, const QPoint& value)
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

void file_item::valueChanged(QtProperty* property, const QSize& value)
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

void file_item::valueChanged(QtProperty* property, bool value)
{
    auto pm = GetParameterModel(property);
    if (pm == nullptr)
        return;

    qDebug() << "valueChanged " << pm->id << " = " << value;
    pm->value = value;
}

void file_item::RegisterProperty(const QtProperty* property, const QString& id)
{
    propertyToId[property] = id;
    idToProperty[id] = property;
}

void file_item::UnregisterProperty(const QString& id)
{
    UnregisterProperty(idToProperty[id]);
}

void file_item::UnregisterProperty(const QtProperty* property)
{
    for (auto p : property->subProperties())
        UnregisterProperty(p);

    idToProperty.remove(propertyToId[property]);
    propertyToId.remove(property);
}

QtProperty* file_item::GetProperty(const QString& id)
{
    auto it = idToProperty.find(id);
    if (it != idToProperty.end())
        return const_cast<QtProperty*>(*it);
    return nullptr;

    //if (idToProperty.contains(id))
    //    return idToProperty[id];
    //else
    //    return nullptr;
}

QString file_item::GetPropertyId(const QtProperty* property)
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

unit_types::ParameterModel* file_item::GetParameterModel(const QString& id)
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

unit_types::ParameterModel* file_item::GetParameterModel(const QtProperty* property)
{
    QString id = GetPropertyId(property);
    if (id == "")
        return nullptr;

    return GetParameterModel(id);
}

bool file_item::GetExpanded(const QtProperty* property)
{
    return false;
}

void file_item::SaveExpandState(QtBrowserItem* index)
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



    //if (pm != nullptr)
    //    qDebug() << propertyEditor_->isExpanded(index);
}

void file_item::SaveExpandState()
{
    if (propertyEditor_ == nullptr)
        return;

    QList<QtBrowserItem*> indexes = propertyEditor_->topLevelItems();
    QListIterator<QtBrowserItem*> itItem(indexes);
    while (itItem.hasNext())
        SaveExpandState(itItem.next());
}

void file_item::ApplyExpandState(QtBrowserItem* index)
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

void file_item::ApplyExpandState()
{
    if (propertyEditor_ == nullptr)
        return;

    QList<QtBrowserItem*> indexes = propertyEditor_->topLevelItems();
    QListIterator<QtBrowserItem*> itItem(indexes);
    while (itItem.hasNext())
        ApplyExpandState(itItem.next());
}

void file_item::UpdateIncludeNameRegExp()
{
    auto pm = GetParameterModel("INCLUDES");
    for (int i = 0; i < pm->value.toInt(); ++i)
    {
        auto pmi = GetParameterModel(QString("INCLUDES/ITEM_%1/NAME").arg(i));
        if (pm->value.toInt() > 1)
        {
            QString regexp("^(?!");
            bool insert = false;
            for (int j = 0; j < pm->value.toInt(); ++j)
            {
                if (i != j)
                {
                    if (insert)
                        regexp += "$|";
                    else
                        insert = true;
                    auto pmj = GetParameterModel(QString("INCLUDES/ITEM_%1/NAME").arg(j));
                    regexp += pmj->value.toString();
                }
            }
            regexp += "$)(.+)";
            pmi->editorSettings.RegExp = regexp;
        }
        else
            pmi->editorSettings.RegExp = "*";
    }
}
