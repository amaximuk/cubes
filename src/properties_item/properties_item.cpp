#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "../main_window.h"
#include "../diagram/diagram_scene.h"
#include "../diagram/diagram_item.h"
#include "../parameters_compiler/base64.h"
#include "../parameters_compiler/parameters_compiler_helper.h"
#include "properties_item.h"

using namespace CubesProperties;

PropertiesItem::PropertiesItem(IPropertiesItemsManagerBoss* propertiesItemsManager, PropertiesEditor* editor,
    CubesUnitTypes::UnitParameters unitParameters, uint32_t propertiesId)
{
    propertiesItemsManager_ = propertiesItemsManager;
    editor_ = editor;
    unitParameters_ = unitParameters;
    propertiesId_ = propertiesId;
    model_ = {};
    ignoreEvents_ = false;

    CreateParametersModel(nullptr);
    CreateProperties();
}

PropertiesItem::PropertiesItem(IPropertiesItemsManagerBoss* propertiesItemsManager, PropertiesEditor* editor,
    CubesUnitTypes::UnitParameters unitParameters, const CubesXml::Unit& xmlUnit, uint32_t propertiesId)
{
    propertiesItemsManager_ = propertiesItemsManager;
    editor_ = editor;
    unitParameters_ = unitParameters;
    propertiesId_ = propertiesId;
    model_ = {};
    ignoreEvents_ = false;

    CreateParametersModel(&xmlUnit);
    CreateProperties();
}

void PropertiesItem::Select()
{
    qDebug() << connect(editor_, &PropertiesEditor::ValueChanged, this, &PropertiesItem::ValueChanged);
    qDebug() << connect(editor_, &PropertiesEditor::StringEditingFinished, this, &PropertiesItem::StringEditingFinished);

    auto pe = editor_->GetPropertyEditor();
    pe->clear();
    ignoreEvents_ = true;
    for (auto& pr : topLevelProperties_)
        pe->addProperty(pr);
    ignoreEvents_ = false;

    ApplyExpandState();
}

void PropertiesItem::UnSelect()
{
    qDebug() << disconnect(editor_, &PropertiesEditor::ValueChanged, this, &PropertiesItem::ValueChanged);
    qDebug() << disconnect(editor_, &PropertiesEditor::StringEditingFinished, this, &PropertiesItem::StringEditingFinished);
}

void PropertiesItem::ExpandedChanged(const QtProperty* property, bool is_expanded)
{
    if (!ignoreEvents_)
    {
        auto pm = GetParameterModel(property);
        if (pm != nullptr)
            pm->editorSettings.is_expanded = is_expanded;
    }
}

void PropertiesItem::CreateParametersModel(const CubesXml::Unit* xmlUnit)
{
    // BASE
    // BASE/NAME
    // BASE/FILE
    // BASE/INCLUDE_NAME
    // PARAMETERS
    // PARAMETERS/...
    // EDITOR
    // EDITOR/POSITION_X
    // EDITOR/POSITION_Y
    // EDITOR/POSITION_Z

    {
        CubesUnitTypes::ParameterModel base_group;
        base_group.id = "BASE";
        base_group.name = QString::fromLocal8Bit("Базовые");
        base_group.value = "";
        base_group.valueType = "none";
        //base_group.parameterInfoId = "";
        base_group.editorSettings.type = CubesUnitTypes::EditorType::None;
        base_group.editorSettings.is_expanded = true;

        CubesUnitTypes::ParameterModel instance_name;
        instance_name.id = "BASE/NAME";
        instance_name.name = QString::fromLocal8Bit("Имя");
        instance_name.value = parameters_compiler::helper::get_instance_name_initial(unitParameters_.fileInfo);
        instance_name.valueType = "string";
        //instance_name.parameterInfoId = "";
        instance_name.editorSettings.type = CubesUnitTypes::EditorType::String;
        instance_name.editorSettings.is_expanded = false;
        base_group.parameters.push_back(std::move(instance_name));

        CubesUnitTypes::ParameterModel file;
        file.id = "BASE/FILE_NAME";
        file.name = QString::fromLocal8Bit("Файл");
        file.value = "";
        file.valueType = "string";
        //file.parameterInfoId = "";
        file.editorSettings.type = CubesUnitTypes::EditorType::ComboBox;
        file.editorSettings.is_expanded = false;
        base_group.parameters.push_back(std::move(file));

        CubesUnitTypes::ParameterModel group;
        group.id = "BASE/INCLUDE_NAME";
        group.name = QString::fromLocal8Bit("Включаемый файл");
        group.value = "";
        group.valueType = "string";
        //group.parameterInfoId = "";
        group.editorSettings.type = CubesUnitTypes::EditorType::ComboBox;
        group.editorSettings.is_expanded = false;
        base_group.parameters.push_back(std::move(group));

        model_.parameters.push_back(std::move(base_group));
    }

    if (unitParameters_.fileInfo.parameters.size() > 0)
    {
        CubesUnitTypes::ParameterModel properties_group;
        properties_group.id = "PARAMETERS";
        properties_group.name = QString::fromLocal8Bit("Параметры");
        properties_group.value = "";
        properties_group.valueType = "none";
        //properties_group.parameterInfoId = "";
        properties_group.editorSettings.type = CubesUnitTypes::EditorType::None;
        properties_group.editorSettings.is_expanded = true;

        for (const auto& pi : unitParameters_.fileInfo.parameters)
        {
            CubesUnitTypes::ParameterModel pm;
            CreateParameterModel({ "Main", QString::fromStdString(pi.name) }, "PARAMETERS", xmlUnit, pm);
            properties_group.parameters.push_back(std::move(pm));
        }

        //CubesUnitTypes::ParameterModel pmd;
        //pmd.id = "PARAMETERS/DEPENDS";
        //pmd.name = QString::fromLocal8Bit("Зависимости");
        //pmd.value = 0;
        //pmd.valueType = "string";
        ////properties_group.parameterInfoId = "";
        //pmd.editorSettings.type = CubesUnitTypes::EditorType::SpinInterger;
        //pmd.editorSettings.SpinIntergerMax = 100;
        //pmd.editorSettings.is_expanded = false;
        //properties_group.parameters.push_back(std::move(pmd));

        model_.parameters.push_back(std::move(properties_group));
    }

    {
        CubesUnitTypes::ParameterModel editor_group;
        editor_group.id = "EDITOR";
        editor_group.name = QString::fromLocal8Bit("Редактор");
        editor_group.value = "";
        editor_group.valueType = "none";
        //editor_group.parameterInfoId = "";
        editor_group.editorSettings.type = CubesUnitTypes::EditorType::None;
        editor_group.editorSettings.is_expanded = true;

        {
            CubesUnitTypes::ParameterModel pm;
            pm.id = "EDITOR/POSITION_X";
            pm.name = QString::fromLocal8Bit("Позиция X");
            pm.value = xmlUnit == nullptr ? 0 : xmlUnit->x;
            pm.valueType = "double";
            //pm.parameterInfoId = "";
            pm.editorSettings.type = CubesUnitTypes::EditorType::SpinDouble;
            pm.editorSettings.SpinDoubleMin = -10000;
            pm.editorSettings.SpinDoubleMax = 10000;
            pm.editorSettings.SpinDoubleSingleStep = 20;
            editor_group.parameters.push_back(std::move(pm));
        }

        {
            CubesUnitTypes::ParameterModel pm;
            pm.id = "EDITOR/POSITION_Y";
            pm.name = QString::fromLocal8Bit("Позиция Y");
            pm.value = xmlUnit == nullptr ? 0 : xmlUnit->y;
            pm.valueType = "double";
            //pm.parameterInfoId = "";
            pm.editorSettings.type = CubesUnitTypes::EditorType::SpinDouble;
            pm.editorSettings.SpinDoubleMin = -10000;
            pm.editorSettings.SpinDoubleMax = 10000;
            pm.editorSettings.SpinDoubleSingleStep = 20;
            editor_group.parameters.push_back(std::move(pm));
        }

        {
            CubesUnitTypes::ParameterModel pm;
            pm.id = "EDITOR/POSITION_Z";
            pm.name = QString::fromLocal8Bit("Позиция Z");
            pm.value = xmlUnit == nullptr ? 0 : xmlUnit->z;
            pm.valueType = "double";
            //pm.parameterInfoId = "";
            pm.editorSettings.type = CubesUnitTypes::EditorType::SpinDouble;
            pm.editorSettings.SpinDoubleMin = -10000;
            pm.editorSettings.SpinDoubleMax = 10000;
            editor_group.parameters.push_back(std::move(pm));
        }

        model_.parameters.push_back(std::move(editor_group));
    }
}

void PropertiesItem::CreateProperties()
{
    QMap<QString, const QtProperty*> idToProperty;
    for (auto& pm : model_.parameters)
        topLevelProperties_.push_back(editor_->CreatePropertyForModel(pm, idToProperty));
    for (const auto& kvp : idToProperty.toStdMap())
        RegisterProperty(kvp.second, kvp.first);
}

//QVariant GetValue(const QString& type, const QString& value)
//{
//    bool is_array = parameters_compiler::helper::is_array_type(type.toStdString());
//    if (is_array)
//    {
//        value = parameters_compiler::helper::get_parameter_initial<int>(unitParameters_.fileInfo, pi);
//    }
//}

void PropertiesItem::CreateParameterModel(const CubesUnitTypes::ParameterInfoId& parameterInfoId,
    const QString& parentModelId, const CubesXml::Unit* xmlUnit, CubesUnitTypes::ParameterModel& model)
{
    // Создание модели для параметра по его info ID (тип и имя из yml файла)
    // Модель включает все вложенные параметры и массивы
    // Каждому параметру назначается model ID (путь к параметру в модели, разделенный /)

    auto& pi = *parameters_compiler::helper::get_parameter_info(unitParameters_.fileInfo, parameterInfoId.type.toStdString(), parameterInfoId.name.toStdString());

    CubesUnitTypes::ParameterModel pm;
    pm.id = QString("%1/%2").arg(parentModelId, QString::fromStdString(pi.name));
    pm.name = QString::fromStdString(parameters_compiler::helper::get_parameter_display_name(pi));
    pm.parameterInfoId = parameterInfoId;

    bool is_array = parameters_compiler::helper::is_array_type(pi.type);
    if (is_array)
    {
        FillArrayModel(xmlUnit, pm);
        UpdateArrayModel(xmlUnit, pm);
    }
    else
    {
        FillParameterModel(xmlUnit, pm);
    }





    //if (xmlUnit != nullptr)
    //{
    //    int i = 0;
    //    for (const auto& d : xmlUnit->depends)
    //    {
    //        CubesUnitTypes::ParameterModel pmo;
    //        pmo.id = QString("%1/%2/ITEM_%3").arg(model.id, "DEPENDS").arg(i++);
    //        pmo.name = "";
    //        pmo.value = d;
    //        pmo.valueType = "string";
    //        //pm_optional.parameterInfo.display_name = QString::fromLocal8Bit("Не задавать").toStdString();
    //        pmo.editorSettings.type = CubesUnitTypes::EditorType::String;

    //        pm_depends.parameters.push_back(std::move(pmo));
    //    }
    //}






    model = pm;
}

void PropertiesItem::FillParameterModel(const CubesXml::Unit* xmlUnit, CubesUnitTypes::ParameterModel& model)
{
    // Заполнение модели параметра, не являющегося массивом, с учетом ограничений
    // Поля id, name, parameterInfoId должны быть предварительно заполнены
    // Если xmlUnit != nullptr, значит создаем юнит из файла xml

    CubesXml::Param* xmlParam = nullptr;
    if (xmlUnit != nullptr)
        xmlParam = CubesXml::parser::getParam(*const_cast<CubesXml::Unit*>(xmlUnit), model.id);

    CubesXml::Item* xmlItem = nullptr;
    if (xmlUnit != nullptr)
        xmlItem = CubesXml::parser::getItem(*const_cast<CubesXml::Unit*>(xmlUnit), model.id);

    auto& pi = *parameters_compiler::helper::get_parameter_info(unitParameters_.fileInfo, model.parameterInfoId.type.toStdString(), model.parameterInfoId.name.toStdString());
    model.value = parameters_compiler::helper::get_parameter_initial(unitParameters_.fileInfo, pi);

    auto piType = pi.type;
    if (parameters_compiler::helper::is_array_type(piType))
        piType = parameters_compiler::helper::get_array_type(pi.type);

    if (piType == "unit")
        model.valueType = "string";
    else if (piType == "path" || piType == "string")
        model.valueType = "string";
    else if (piType == "bool")
        model.valueType = "bool";
    else if (piType == "int" || piType == "int8_t" || piType == "int16_t" || piType == "int32_t" ||
        piType == "int64_t" || piType == "uint8_t" || piType == "uint16_t" || piType == "uint32_t" || piType == "uint64_t")
        model.valueType = "int";
    else if (piType == "double" || piType == "float")
        model.valueType = "double";
    else // enum user type
        model.valueType = "string";
    
    if (xmlUnit != nullptr && xmlParam != nullptr)
    {
        QString typeName;
        if (model.valueType == "string") typeName = "str";
        else if (model.valueType == "int") typeName = "int";
        else if (model.valueType == "double") typeName = "dbl";
        else if (model.valueType == "bool") typeName = "bool";
        else if (model.valueType == "library") typeName = "lib";

        if (xmlParam->type != typeName)
        {
            propertiesItemsManager_->AfterError(this, QString::fromLocal8Bit("Тип данных в xml не совместим с типом параметра"));
            // Ошибка! Тип данных в xml не совместим с типом параметра
            // TODO: вернуть ошибку
        }
    }

    if (pi.restrictions.set_.size() > 0)
    {
        model.editorSettings.type = CubesUnitTypes::EditorType::ComboBox;
        for (const auto& s : pi.restrictions.set_)
            model.editorSettings.ComboBoxValues.push_back(QString::fromStdString(s));

        if (xmlUnit != nullptr && xmlParam != nullptr)
        {
            if (!model.editorSettings.ComboBoxValues.contains(xmlParam->val))
            {
                propertiesItemsManager_->AfterError(this, QString::fromLocal8Bit("Значение параметра в xml не удовлетворяет ограничениям"));
                // Ошибка! Значение параметра в xml не удовлетворяет ограничениям
                // TODO: вернуть ошибку
            }
            model.value = xmlParam->val;
        }
        else if (xmlItem != nullptr)
        {
            model.value = xmlItem->val;
        }
    }
    else
    {
        if (piType == "unit")
        {
            model.editorSettings.type = CubesUnitTypes::EditorType::String;

            CubesUnitTypes::ParameterModel pm_depends;
            pm_depends.id = QString("%1/%2").arg(model.id, "DEPENDS");
            pm_depends.name = QString::fromLocal8Bit("Зависимость");
            pm_depends.value = false;
            pm_depends.valueType = "bool";
            //pm_depends.parameterInfo.display_name = QString::fromLocal8Bit("Зависимость").toStdString();
            pm_depends.editorSettings.type = CubesUnitTypes::EditorType::CheckBox;

            if (xmlUnit != nullptr && xmlParam != nullptr)
                pm_depends.value = xmlParam->depends;












            //if (xmlUnit != nullptr)
            //{
            //    int i = 0;
            //    for (const auto& d : xmlUnit->depends)
            //    {
            //        CubesUnitTypes::ParameterModel pmo;
            //        pmo.id = QString("%1/%2/ITEM_%3").arg(model.id, "DEPENDS").arg(i++);
            //        pmo.name = "";
            //        pmo.value = d;
            //        pmo.valueType = "string";
            //        //pm_optional.parameterInfo.display_name = QString::fromLocal8Bit("Не задавать").toStdString();
            //        pmo.editorSettings.type = CubesUnitTypes::EditorType::String;

            //        pm_depends.parameters.push_back(std::move(pmo));
            //    }
            //}













            model.parameters.push_back(std::move(pm_depends));

            if (xmlUnit != nullptr && xmlParam != nullptr)
                model.value = xmlParam->val;
            else if (xmlItem != nullptr)
            {
                model.value = xmlItem->val;
            }






        }
        else if (piType == "path" || piType == "string")
        {
            model.editorSettings.type = CubesUnitTypes::EditorType::String;

            if (xmlUnit != nullptr && xmlParam != nullptr)
                model.value = xmlParam->val;
            else if (xmlItem != nullptr)
            {
                model.value = xmlItem->val;
            }
        }
        else if (piType == "bool")
        {
            model.editorSettings.type = CubesUnitTypes::EditorType::CheckBox;

            if (xmlUnit != nullptr && xmlParam != nullptr)
                model.value = xmlParam->val;
            else if (xmlItem != nullptr)
            {
                model.value = xmlItem->val;
            }
        }
        else if (piType == "int" || piType == "int8_t" || piType == "int16_t" || piType == "int32_t" ||
            piType == "int64_t" || piType == "uint8_t" || piType == "uint16_t" || piType == "uint32_t" || piType == "uint64_t")
        {
            model.editorSettings.type = CubesUnitTypes::EditorType::SpinInterger;

            if (pi.restrictions.min != "")
                model.editorSettings.SpinIntergerMin = std::stoi(pi.restrictions.min);
            else
                model.editorSettings.SpinIntergerMin = CubesUnitTypes::GetMinForIntegralType(QString::fromStdString(piType));

            if (pi.restrictions.max != "")
                model.editorSettings.SpinIntergerMax = std::stoi(pi.restrictions.max);
            else
                model.editorSettings.SpinIntergerMax = CubesUnitTypes::GetMaxForIntegralType(QString::fromStdString(piType));

            if (xmlUnit != nullptr && xmlParam != nullptr)
                model.value = xmlParam->val;
            else if (xmlItem != nullptr)
            {
                model.value = xmlItem->val;
            }
        }
        else if (piType == "double" || piType == "float")
        {
            model.editorSettings.type = CubesUnitTypes::EditorType::SpinDouble;

            if (pi.restrictions.min != "")
                model.editorSettings.SpinDoubleMin = std::stod(pi.restrictions.min);
            else
                model.editorSettings.SpinDoubleMin = CubesUnitTypes::GetMinForFloatingPointType(QString::fromStdString(piType));

            if (pi.restrictions.max != "")
                model.editorSettings.SpinDoubleMax = std::stod(pi.restrictions.max);
            else
                model.editorSettings.SpinDoubleMax = CubesUnitTypes::GetMaxForFloatingPointType(QString::fromStdString(piType));

            if (xmlUnit != nullptr && xmlParam != nullptr)
                model.value = xmlParam->val;
            else if (xmlItem != nullptr)
            {
                model.value = xmlItem->val;
            }
        }
        else
        {
            // enum user type
            const auto pti = parameters_compiler::helper::get_type_info(unitParameters_.fileInfo, piType);
            if (pti->type == "enum")
            {
                model.editorSettings.type = CubesUnitTypes::EditorType::ComboBox;
                if (pti->values.size() > 0)
                {
                    for (const auto v : pti->values)
                        model.editorSettings.ComboBoxValues.push_back(QString::fromStdString(v.first));
                }

                if (xmlUnit != nullptr && xmlParam != nullptr)
                {
                    if (!model.editorSettings.ComboBoxValues.contains(xmlParam->val))
                    {
                        propertiesItemsManager_->AfterError(this, QString::fromLocal8Bit("Значение параметра в xml не удовлетворяет ограничениям"));
                        // Ошибка! Значение параметра в xml не удовлетворяет ограничениям
                        // TODO: вернуть ошибку
                    }
                    model.value = xmlParam->val;
                }
                else if (xmlItem != nullptr)
                {
                    model.value = xmlItem->val;
                }
            }
            else assert(false);
        }
    }

    if (parameters_compiler::helper::get_parameter_optional(pi))
    {
        CubesUnitTypes::ParameterModel pmo;
        pmo.id = QString("%1/%2").arg(model.id, "OPTIONAL");
        pmo.name = QString::fromLocal8Bit("Не задавать");
        pmo.value = false;
        pmo.valueType = "bool";
        //pm_optional.parameterInfo.display_name = QString::fromLocal8Bit("Не задавать").toStdString();
        pmo.editorSettings.type = CubesUnitTypes::EditorType::CheckBox;

        if (xmlUnit != nullptr)
        {
            if (xmlParam == nullptr)
            {
                model.value = QString::fromLocal8Bit("не задано");
                pmo.value = true;
            }
            else
            {
                pmo.value = false;
            }
        }

        model.parameters.push_back(std::move(pmo));
    }
}

//
//QtProperty* properties_item::GetPropertyForModel(unit_types::ParameterModel& model)
//{
//    // None, String, SpinInterger, SpinDouble, ComboBox, CheckBox
//
//    QtProperty* pr = nullptr;
//    if (model.editorSettings.type == unit_types::EditorType::None)
//    {
//        pr = groupManager->addProperty(model.name);
//        groupManager->blockSignals(true);
//        groupManager->blockSignals(false);
//    }
//    else if (model.editorSettings.type == unit_types::EditorType::String)
//    {
//        pr = stringManager->addProperty(model.name);
//        stringManager->blockSignals(true);
//        //stringManager->setRegExp(pr, QRegExp("-?\\d{1,3}"));
//        //stringManager->setOldValue(pr, model.value.toString());
//        stringManager->setValue(pr, model.value.toString());
//        stringManager->blockSignals(false);
//    }
//    else if (model.editorSettings.type == unit_types::EditorType::SpinInterger)
//    {
//        pr = intManager->addProperty(model.name);
//        intManager->blockSignals(true);
//        intManager->setRange(pr, model.editorSettings.SpinIntergerMin, model.editorSettings.SpinIntergerMax);
//        intManager->setValue(pr, model.value.toInt());
//        intManager->blockSignals(false);
//    }
//    else if (model.editorSettings.type == unit_types::EditorType::SpinDouble)
//    {
//        pr = doubleManager->addProperty(model.name);
//        doubleManager->blockSignals(true);
//        doubleManager->setRange(pr, model.editorSettings.SpinDoubleMin, model.editorSettings.SpinDoubleMax);
//        doubleManager->setSingleStep(pr, model.editorSettings.SpinDoubleSingleStep);
//        doubleManager->setValue(pr, model.value.toDouble());
//        doubleManager->blockSignals(false);
//    }
//    else if (model.editorSettings.type == unit_types::EditorType::ComboBox)
//    {
//        pr = enumManager->addProperty(model.name);
//        enumManager->blockSignals(true);
//        enumManager->setEnumNames(pr, model.editorSettings.ComboBoxValues);
//
//
//        //enumManager->setValue(pr, model.value.toInt());
//        int pos = 0;
//        for (; pos < model.editorSettings.ComboBoxValues.size(); ++pos)
//        {
//            if (model.valueType == "double" && model.value.toDouble() == std::stod(model.editorSettings.ComboBoxValues[pos].toStdString()))
//                break;
//            else if (model.valueType == "int" && model.value.toInt() == std::stoi(model.editorSettings.ComboBoxValues[pos].toStdString()))
//                break;
//            else if (model.valueType == "bool" && model.value.toBool() == (model.editorSettings.ComboBoxValues[pos] == "true"))
//                break;
//            else if (model.valueType == "string" && model.value.toString() == model.editorSettings.ComboBoxValues[pos])
//                break;
//        }
//        //int pos = model.editorSettings.ComboBoxValues.indexOf(model.value.toString(), 0);
//        if (pos == model.editorSettings.ComboBoxValues.size())
//            pos = 0;
//
//        enumManager->setValue(pr, pos);
//        enumManager->blockSignals(false);
//    }
//    else if (model.editorSettings.type == unit_types::EditorType::CheckBox)
//    {
//        pr = boolManager->addProperty(model.name);
//        boolManager->blockSignals(true);
//        boolManager->setValue(pr, model.value.toBool());
//        boolManager->blockSignals(false);
//    }
//    else assert(false);
//
//    RegisterProperty(pr, model.id);
//
//    for (auto& sp : model.parameters)
//    {
//        pr->addSubProperty(GetPropertyForModel(sp));
//    }
//
//    if (model.readOnly)
//        pr->setEnabled(false);
//
//    return pr;
//}
//
//void properties_item::CreatePropertyBrowser()
//{
//    groupManager.reset(new QtGroupPropertyManager(this));
//    intManager.reset(new QtIntPropertyManager(this));
//    doubleManager.reset(new QtDoublePropertyManager(this));
//    stringManager.reset(new QtStringPropertyManager(this));
//    enumManager.reset(new QtEnumPropertyManager(this));
//    boolManager.reset(new QtBoolPropertyManager(this));
//
//    qDebug() << connect(intManager.get(), SIGNAL(valueChanged(QtProperty*, int)), this, SLOT(valueChanged(QtProperty*, int)));
//    qDebug() << connect(doubleManager.get(), SIGNAL(valueChanged(QtProperty*, double)), this, SLOT(valueChanged(QtProperty*, double)));
//    qDebug() << connect(stringManager.get(), SIGNAL(valueChanged(QtProperty*, const QString&)), this, SLOT(valueChanged(QtProperty*, const QString&)));
//    qDebug() << connect(stringManager.get(), SIGNAL(editingFinished(QtProperty*, const QString&, const QString&)), this, SLOT(editingFinished(QtProperty*, const QString&, const QString&)));
//    qDebug() << connect(enumManager.get(), SIGNAL(valueChanged(QtProperty*, int)), this, SLOT(valueChanged(QtProperty*, int)));
//    qDebug() << connect(boolManager.get(), SIGNAL(valueChanged(QtProperty*, bool)), this, SLOT(valueChanged(QtProperty*, bool)));
//
//    intSpinBoxFactory.reset(new QtSpinBoxFactory(this));
//    doubleSpinBoxFactory.reset(new QtDoubleSpinBoxFactory(this));
//    checkBoxFactory.reset(new QtCheckBoxFactory(this));
//    spinBoxFactory.reset(new QtSpinBoxFactory(this));
//    lineEditFactory.reset(new QtLineEditFactory(this));
//    comboBoxFactory.reset(new QtEnumEditorFactory(this));
//}

void PropertiesItem::SetFileNames(QStringList fileNames)
{
    const auto pm = GetParameterModel("BASE/FILE_NAME");
    //int index = pm->editorSettings.ComboBoxValues.indexOf(pm->value.toString());
    if (pm != nullptr)
    {
        pm->editorSettings.ComboBoxValues = fileNames;
        //?????????
        //if (pm->value.toString() == "" && fileNames.size() > 0)
        //    pm->value = fileNames[0];
        editor_->SetEnumValues(GetProperty(pm->id), fileNames);
    }
}

void PropertiesItem::SetFileName(QString fileName)
{
    const auto pm = GetParameterModel("BASE/FILE_NAME");
    if (pm != nullptr)
    {
        pm->value = fileName;
        editor_->SetEnumValue(GetProperty(pm->id), pm->valueType, fileName);
    }
}

void PropertiesItem::SetFileNameReadOnly(bool readOnly)
{
    const auto pm = GetParameterModel("BASE/FILE_NAME");
    if (pm != nullptr)
    {
        pm->readOnly = readOnly;
        editor_->SetReadOnly(GetProperty(pm->id), readOnly);
    }
}

void PropertiesItem::SetGroupNameReadOnly(bool readOnly)
{
    const auto pm = GetParameterModel("BASE/INCLUDE_NAME");
    if (pm != nullptr)
    {
        pm->readOnly = readOnly;
        editor_->SetReadOnly(GetProperty(pm->id), readOnly);
    }
}

void PropertiesItem::SetGroupNames(QStringList groupNames)
{
    QString oldName = GetGroupName();
    if (!groupNames.contains(oldName))
        oldName = groupNames[0]; // <not selected>
    const auto pm = GetParameterModel("BASE/INCLUDE_NAME");
    //int index = pm->editorSettings.ComboBoxValues.indexOf(pm->value.toString());
    if (pm != nullptr)
    {
        pm->editorSettings.ComboBoxValues = groupNames;
        //if (!groupNames.contains(pm->value.toString()) && groupNames.size() > 0)
        //    pm->value = groupNames[0];
        editor_->SetEnumValues(GetProperty(pm->id), groupNames);
    }
    SetGroupName(oldName);
}

void PropertiesItem::SetGroupName(QString groupName)
{
    const auto pm = GetParameterModel("BASE/INCLUDE_NAME");
    if (pm != nullptr)
    {
        pm->value = groupName;
        editor_->SetEnumValue(GetProperty(pm->id), pm->valueType, groupName);
    }
    //diagramItem_->InformGroupChanged();
}

QString PropertiesItem::GetFileName()
{
    const auto pm = GetParameterModel("BASE/FILE_NAME");
    //int index = pm->editorSettings.ComboBoxValues.indexOf(pm->value.toString());
    if (pm != nullptr)
    {
        return pm->value.toString();
        //if (pm->value.toInt() < pm->editorSettings.ComboBoxValues.size())
        //    return pm->editorSettings.ComboBoxValues[pm->value.toInt()];
    }
    return "";
}

QString PropertiesItem::GetGroupName()
{
    const auto pm = GetParameterModel("BASE/INCLUDE_NAME");
    //int index = pm->editorSettings.ComboBoxValues.indexOf(pm->value.toString());
    if (pm != nullptr)
    {
        return pm->value.toString();
        //if (pm->value.toInt() < pm->editorSettings.ComboBoxValues.size())
        //    return pm->editorSettings.ComboBoxValues[pm->value.toInt()];
    }
    return "";
}

void PropertiesItem::SetName(QString name)
{
    const auto pm = GetParameterModel("BASE/NAME");
    if (pm != nullptr)
    {
        QString oldName = pm->value.toString();
        pm->value = name;

        auto pr = GetProperty(pm->id);
        editor_->SetStringValue(pr, name);
    }
}

QString PropertiesItem::GetName()
{
    const auto pm = GetParameterModel("BASE/NAME");
    if (pm != nullptr)
        return pm->value.toString();
    return "";
}

void PropertiesItem::GetXml(CubesXml::Unit& xmlUnit)
{
    for (auto& pm : model_.parameters)
    {
        if (pm.id.startsWith("PARAMETERS"))
        {
            for (auto& pm : pm.parameters)
            {
                xmlUnit.id = pm.parameterInfoId.name;
            }
        }
    }
}

//QList<QPair<QString, QString>> PropertiesItem::GetVariables()
//{
//    QList<QPair<QString, QString>> result;
//    const auto pm = GetParameterModel("PARAMETERS/VARIABLES");
//    if (pm != nullptr)
//    {
//        int count = pm->value.toInt();
//        for (int i = 0; i < count; i++)
//        {
//            const auto pm_n = GetParameterModel(QString("PARAMETERS/VARIABLES/ITEM_%1/NAME").arg(i));
//            const auto pm_v = GetParameterModel(QString("PARAMETERS/VARIABLES/ITEM_%1/VALUE").arg(i));
//            if (pm_n != nullptr && pm_v != nullptr)
//                result.push_back({ pm_n->value.toString(), pm_v->value.toString() });
//        }
//    }
//    return result;
//}
//
//void properties_item::ApplyToBrowser(QtTreePropertyBrowser* propertyEditor)
//{
//    propertyEditor_ = propertyEditor;
//
//    //QtSpinBoxFactory* intSpinBoxFactory = new QtSpinBoxFactory(this);
//    //QtDoubleSpinBoxFactory* doubleSpinBoxFactory = new QtDoubleSpinBoxFactory(this);
//    //QtCheckBoxFactory* checkBoxFactory = new QtCheckBoxFactory(this);
//    //QtSpinBoxFactory* spinBoxFactory = new QtSpinBoxFactory(this);
//    //QtLineEditFactory* lineEditFactory = new QtLineEditFactory(this);
//    //QtEnumEditorFactory* comboBoxFactory = new QtEnumEditorFactory(this);
//
//    propertyEditor->setFactoryForManager(intManager.get(), intSpinBoxFactory.get());
//    propertyEditor->setFactoryForManager(doubleManager.get(), doubleSpinBoxFactory.get());
//    propertyEditor->setFactoryForManager(stringManager.get(), lineEditFactory.get());
//    propertyEditor->setFactoryForManager(enumManager.get(), comboBoxFactory.get());
//    propertyEditor->setFactoryForManager(boolManager.get(), checkBoxFactory.get());
//
//
//    propertyEditor->setResizeMode(QtTreePropertyBrowser::ResizeMode::Interactive);
//    propertyEditor->setSplitterPosition(250);
//    propertyEditor->setHeaderVisible(false);
//
//    //propertyEditor->setResizeMode(QtTreePropertyBrowser::ResizeMode::ResizeToContents);
//    //propertyEditor->setPropertiesWithoutValueMarked(true);
//
//    propertyEditor->clear();
//
//    //QtProperty* mainGroup = groupManager->addProperty(QString::fromStdString(unitParameters_.fileInfo.info.id));
//    //
//    //QtProperty* propertiesGroup = groupManager->addProperty(QString::fromLocal8Bit("Свойства"));
//    //mainGroup->addSubProperty(propertiesGroup);
//
//    //for (auto& pm : parametersModel_.parameters)
//    //    propertiesGroup->addSubProperty(GetPropertyForModel(pm));
//    
//    //QtProperty* editorGroup = groupManager->addProperty(QString::fromLocal8Bit("Редактор"));
//    //mainGroup->addSubProperty(editorGroup);
//
//    //for (auto& pm : editorModel_.parameters)
//    //    editorGroup->addSubProperty(GetPropertyForModel(pm));
//
//    ignoreEvents_ = true;
//    for (auto& pm : parametersModel_.parameters)
//        propertyEditor->addProperty(GetPropertyForModel(pm));
//
//    // propertyEditor->addProperty(mainGroup);
//    ignoreEvents_ = false;
//
//    ApplyExpandState();
//}
//
//void properties_item::UnApplyToBrowser()
//{
//    if (propertyEditor_ == nullptr)
//        return;
//
//    propertyEditor_->unsetFactoryForManager(intManager.get());
//    propertyEditor_->unsetFactoryForManager(doubleManager.get());
//    propertyEditor_->unsetFactoryForManager(stringManager.get());
//    propertyEditor_->unsetFactoryForManager(enumManager.get());
//    propertyEditor_->unsetFactoryForManager(boolManager.get());
//}

QPixmap PropertiesItem::GetPixmap()
{
    QPixmap px;
    bool loaded = false;
    if (unitParameters_.fileInfo.info.pictogram != "")
    {
        std::string s = base64_decode(unitParameters_.fileInfo.info.pictogram);
        QByteArray ba(s.c_str(), static_cast<int>(s.size()));
        try
        {
            loaded = px.loadFromData(ba);
        }
        catch (...)
        {
            loaded = false;
        }
    }
    if (!loaded)
        px.load(":/images/ice.png");
    return px;
}

void PropertiesItem::PositionChanged(QPointF point)
{
    auto pm_x = GetParameterModel("EDITOR/POSITION_X");
    pm_x->value = point.x();
    editor_->SetDoubleValue(GetProperty(pm_x->id), point.x());

    auto pm_y = GetParameterModel("EDITOR/POSITION_Y");
    pm_y->value = point.y();
    editor_->SetDoubleValue(GetProperty(pm_y->id), point.y());
}

void PropertiesItem::ZOrderChanged(double value)
{
    auto pm = GetParameterModel("EDITOR/POSITION_Z");
    pm->value = value;
    editor_->SetDoubleValue(GetProperty(pm->id), value);
}

QString PropertiesItem::GetPropertyDescription(QtProperty* property)
{
    QString id = GetPropertyId(property);
    return id;
}

//void PropertiesItem::ExpandedChanged(QtProperty* property, bool is_expanded)
//{
//    if (!ignoreEvents_)
//    {
//        auto pm = GetParameterModel(property);
//        if (pm != nullptr)
//            pm->editorSettings.is_expanded = is_expanded;
//    }
//}

void PropertiesItem::GetConnectedNamesInternal(const CubesUnitTypes::ParameterModel& model, QList<QString>& list)
{
    auto pi = parameters_compiler::helper::get_parameter_info(unitParameters_.fileInfo,
        model.parameterInfoId.type.toStdString(), model.parameterInfoId.name.toStdString());

    if (pi != nullptr && pi->type == "unit")
    {
        QString name = model.value.toString();
        if (name != "")
            list.push_back(name);
    }
    for (const auto& pm : model.parameters)
        GetConnectedNamesInternal(pm, list);
}

void PropertiesItem::GetDependentNamesInternal(const CubesUnitTypes::ParameterModel& model, QList<QString>& list)
{
    auto pi = parameters_compiler::helper::get_parameter_info(unitParameters_.fileInfo,
        model.parameterInfoId.type.toStdString(), model.parameterInfoId.name.toStdString());

    if (model.id == "PARAMETERS/DEPENDS")
    {
        for (const auto& sub : model.parameters)
        {
            QString name = sub.value.toString();
            list.push_back(name);
        }
    }

    if (pi != nullptr && pi->type == "unit")
    {
        for (const auto& sub : model.parameters)
        {
            if (sub.id.endsWith("/DEPENDS") && sub.valueType == "bool" && sub.value.toBool() == true)
            {
                QString name = model.value.toString();
                list.push_back(name);
                break;
            }
        }
    }
    for (const auto& pm : model.parameters)
        GetDependentNamesInternal(pm, list);
}

//void PropertiesItem::ApplyXmlPropertiesInternal(CubesUnitTypes::ParameterModel& model, CubesXml::Unit& xu)
//{
//    auto pi = parameters_compiler::helper::get_parameter_info(unitParameters_.fileInfo,
//        model.parameterInfoId.type.toStdString(), model.parameterInfoId.name.toStdString());
//
//    if (pi != nullptr)
//    {
//        if (parameters_compiler::helper::is_array_type(pi->type))
//        {
//            int i = CubesXml::parser::getItemsCount(xu, model.id);
//            qDebug() << i;
//            if (i != -1)
//            {
//                //if (pi->restrictions.set_count.size() > 0)
//                //{
//                //    int pos = 0;
//                //    for (; pos < pi->restrictions.set_count.size(); ++pos)
//                //    {
//                //        if (i == std::stoi(pi->restrictions.set_count[pos]))
//                //            break;
//                //    }
//                //    if (pos == pi->restrictions.set_count.size())
//                //        pos = 0;
//                //    model.value = pos;
//                //}
//                //else
//                //    model.value = i;
//                model.value = i;
//                ApplyXmlArrayModel(model, xu);
//            }
//        }
//        else
//        {
//            CubesXml::Param* xp = CubesXml::parser::getParam(xu, model.id);
//            if (xp != nullptr)
//            {
//                qDebug() << xp->name;
//                model.value = xp->val;
//
//                QtProperty* p = GetProperty(model.id);
//                editor_->SetPropertyValue(p, model);
//            }
//        }
//    }
//
//    for (auto& pm : model.parameters)
//        ApplyXmlPropertiesInternal(pm, xu);
//}

//void PropertiesItem::ApplyXmlArrayModel(CubesUnitTypes::ParameterModel& model, CubesXml::Unit& xu)
//{
//    auto& pi = *parameters_compiler::helper::get_parameter_info(unitParameters_.fileInfo, model.parameterInfoId.type.toStdString(), model.parameterInfoId.name.toStdString());
//
//    int count = CubesXml::parser::getItemsCount(xu, model.id);
//    model.value = count;
//    QtProperty* p = GetProperty(model.id);
//    editor_->SetPropertyValue(p, model);
//
//    auto at = parameters_compiler::helper::get_array_type(pi.type);
//    auto ti = parameters_compiler::helper::get_type_info(unitParameters_.fileInfo, at);
//    if (parameters_compiler::helper::is_inner_type(at) || (ti != nullptr && ti->type == "enum"))
//    {
//        for (int i = model.parameters.size(); i < count; ++i)
//        {
//            CubesUnitTypes::ParameterModel modelXXX;
//            modelXXX.id = QString("%1/%2_%3").arg(model.id, "ITEM").arg(i);
//            modelXXX.name = QString::fromLocal8Bit("Элемент %1").arg(i);
//            //!!!!!!!!!!!!!!!!!!!!!!!
//                        //model.value = "";
//                        //model.valueType = "none";
//            modelXXX.parameterInfoId = model.parameterInfoId;
//            FillParameterModel(xmlUnit, modelXXX);
//            ApplyXmlPropertiesInternal(modelXXX, xu);
//            model.parameters.push_back(modelXXX);
//
//            //parameters_compiler::parameter_info pi_new = pm.parameterInfo;
//            //pi_new.type = at;
//            //pi_new.name = QString("%1/%2_%3").arg(pm.id, "ITEM").arg(i).toStdString();
//            //pi_new.display_name = QString::fromLocal8Bit("Элемент %1").arg(i).toStdString();
//            //unit_types::ParameterModel model;
//            //CreateParameterModel(pi_new, pm.id, model);
//            //pm.parameters.push_back(model);
//        }
//    }
//    else if (ti != nullptr) // yml type
//    {
//        for (int i = model.parameters.size(); i < count; ++i)
//        {
//            CubesUnitTypes::ParameterModel group_model;
//            group_model.id = QString("%1/%2_%3").arg(model.id, "ITEM").arg(i);
//            group_model.name = QString::fromLocal8Bit("Элемент %1").arg(i);
//            group_model.value = "";
//            group_model.valueType = "none";
//            group_model.editorSettings.type = CubesUnitTypes::EditorType::None;
//            for (auto p : ti->parameters)
//            {
//                CubesUnitTypes::ParameterModel modelYYY;
//                CreateParameterModel({ QString::fromStdString(ti->name), QString::fromStdString(p.name) }, group_model.id, modelYYY);
//                ApplyXmlPropertiesInternal(modelYYY, xu);
//                group_model.parameters.push_back(std::move(modelYYY));
//            }
//            model.parameters.push_back(std::move(group_model));
//
//            //unit_types::ParameterModel group_model;
//            //group_model.editorSettings.type = unit_types::EditorType::None;
//            //group_model.id = QString("%1/%2_%3").arg(pm.id, "ITEM").arg(i);
//            //group_model.parameterInfo.display_name = QString::fromLocal8Bit("Элемент %1").arg(i).toStdString();
//            //for (auto p : ti->parameters)
//            //{
//            //    unit_types::ParameterModel model;
//            //    CreateParameterModel(p, group_model.id, model);
//            //    group_model.parameters.push_back(model);
//            //}
//            //pm.parameters.push_back(group_model);
//        }
//    }
//
//    while (model.parameters.size() > count)
//        model.parameters.pop_back();
//}

QString PropertiesItem::GetInstanceName()
{
    const auto pm = GetParameterModel("BASE/NAME");
    if (pm != nullptr)
        return pm->value.toString();
    return QString();
}

//void PropertiesItem::ApplyXmlProperties(CubesXml::Unit xu)
//{
//    SetName(xu.name);
//    //auto pm = GetParameterModel("BASE/NAME");
//    //if (pm == nullptr)
//    //    return;
//    //pm->value = xu.name;
//    //auto pr = GetProperty(pm->id);
//    //editor_->SetStringValue(pr, xu.name);
//
//    for (auto& pm : model_.parameters)
//    {
//        ApplyXmlPropertiesInternal(pm, xu);
//    }
//
//    //QString oldName = pm->value.toString();
//    //diagramItem_->InformNameChanged(xu.name, "");
//}

QList<QString> PropertiesItem::GetConnectedNames()
{
    QList<QString> list;
    for (const auto& pm : model_.parameters)
    {
        if (pm.id == "PARAMETERS")
            GetConnectedNamesInternal(pm, list);
    }
    return list;
}

QList<QString> PropertiesItem::GetDependentNames()
{
    QList<QString> list;
    for (const auto& pm : model_.parameters)
    {
        if (pm.id == "PARAMETERS")
            GetDependentNamesInternal(pm, list);
    }
    return list;
}

void PropertiesItem::FillArrayModel(const CubesXml::Unit* xmlUnit, CubesUnitTypes::ParameterModel& model)
{
    // Созадем модель для параметра, хранящего количество элементов массива
    // Если задан перечень элементов, используем ComboBox, если нет - SpinBox
    // Если ограничения не заданы, берется - количество элементов равно нулю
    // Поля id, name, parameterInfoId должны быть предварительно заполнены
    // Если xmlUnit != nullptr, значит создаем юнит из файла xml

    auto& pi = *parameters_compiler::helper::get_parameter_info(unitParameters_.fileInfo, model.parameterInfoId.type.toStdString(), model.parameterInfoId.name.toStdString());
    model.value = parameters_compiler::helper::get_parameter_initial(unitParameters_.fileInfo, pi);
    model.valueType = "int";

    int xmlCount = 0;
    if (xmlUnit != nullptr)
        xmlCount = CubesXml::parser::getItemsCount(*const_cast<CubesXml::Unit*>(xmlUnit), model.id);

    if (pi.restrictions.set_count.size() > 0)
    {
        model.editorSettings.type = CubesUnitTypes::EditorType::ComboBox;
        for (const auto& s : pi.restrictions.set_count)
            model.editorSettings.ComboBoxValues.push_back(QString::fromStdString(s));
    
        if (xmlUnit != nullptr && xmlCount != -1)
        {
            if (!model.editorSettings.ComboBoxValues.contains(QString("%1").arg(xmlCount)))
            {
                propertiesItemsManager_->AfterError(this, QString::fromLocal8Bit("Количество элементов %1 не соответствует ограничениям").arg(model.name));
                // Ошибка! Количество элементов в xml не соответствует ограничениям
                // TODO: вернуть ошибку
            }
            else
                model.value = QString("%1").arg(xmlCount);
        }
    }
    else
    {
        model.editorSettings.type = CubesUnitTypes::EditorType::SpinInterger;

        if (pi.restrictions.min_count != "")
            model.editorSettings.SpinIntergerMin = std::stoi(pi.restrictions.min_count);
        else
            model.editorSettings.SpinIntergerMin = 0;

        if (pi.restrictions.max_count != "")
            model.editorSettings.SpinIntergerMax = std::stoi(pi.restrictions.max_count);
        else
            model.editorSettings.SpinIntergerMax = 1000; // !!! todo: make a define for a const

        if (xmlUnit != nullptr && xmlCount != -1)
        {
            if (xmlCount < model.editorSettings.SpinIntergerMin || xmlCount > model.editorSettings.SpinIntergerMax)
            {
                propertiesItemsManager_->AfterError(this, QString::fromLocal8Bit("Количество элементов %1 не соответствует ограничениям").arg(model.name));
                // Ошибка! Количество элементов в xml не соответствует ограничениям
                // TODO: вернуть ошибку
            }
            else
                model.value = QString("%1").arg(xmlCount);
        }
    }
}

void PropertiesItem::UpdateArrayModel(const CubesXml::Unit* xmlUnit, CubesUnitTypes::ParameterModel& model)
{
    // Обновляем модель массива, по фактическому количеству элементов в нем
    // Поля id, name, parameterInfoId должны быть предварительно заполнены
    // Если xmlUnit != nullptr, значит создаем юнит из файла xml

    auto& pi = *parameters_compiler::helper::get_parameter_info(unitParameters_.fileInfo,
        model.parameterInfoId.type.toStdString(), model.parameterInfoId.name.toStdString());

    int count = model.value.toInt();
    //if (pi.restrictions.set_count.size() > 0 && count < pi.restrictions.set_count.size())
    //    count = std::stoi(pi.restrictions.set_count[count]);

    auto at = parameters_compiler::helper::get_array_type(pi.type);
    auto ti = parameters_compiler::helper::get_type_info(unitParameters_.fileInfo, at);
    if (parameters_compiler::helper::is_inner_type(at) || (ti != nullptr && ti->type == "enum"))
    {
        for (int i = model.parameters.size(); i < count; ++i)
        {
            CubesUnitTypes::ParameterModel modelXXX;
            modelXXX.id = QString("%1/%2_%3").arg(model.id, "ITEM").arg(i);
            modelXXX.name = QString::fromLocal8Bit("Элемент %1").arg(i);
//!!!!!!!!!!!!!!!!!!!!!!!
            //model.value = "";
            //model.valueType = "none";
            modelXXX.parameterInfoId = model.parameterInfoId;
            FillParameterModel(xmlUnit, modelXXX);
            model.parameters.push_back(modelXXX);

            //parameters_compiler::parameter_info pi_new = pm.parameterInfo;
            //pi_new.type = at;
            //pi_new.name = QString("%1/%2_%3").arg(pm.id, "ITEM").arg(i).toStdString();
            //pi_new.display_name = QString::fromLocal8Bit("Элемент %1").arg(i).toStdString();
            //unit_types::ParameterModel model;
            //CreateParameterModel(pi_new, pm.id, model);
            //pm.parameters.push_back(model);
        }
    }
    else if (ti != nullptr) // yml type
    {
        for (int i = model.parameters.size(); i < count; ++i)
        {
            CubesUnitTypes::ParameterModel group_model;
            group_model.id = QString("%1/%2_%3").arg(model.id, "ITEM").arg(i);
            group_model.name = QString::fromLocal8Bit("Элемент %1").arg(i);
            group_model.value = "";
            group_model.valueType = "none";
            group_model.editorSettings.type = CubesUnitTypes::EditorType::None;
            for (auto p : ti->parameters)
            {
                CubesUnitTypes::ParameterModel modelYYY;
                CreateParameterModel({ QString::fromStdString(ti->name), QString::fromStdString(p.name) }, group_model.id, xmlUnit, modelYYY);
                group_model.parameters.push_back(std::move(modelYYY));
            }
            model.parameters.push_back(std::move(group_model));

            //unit_types::ParameterModel group_model;
            //group_model.editorSettings.type = unit_types::EditorType::None;
            //group_model.id = QString("%1/%2_%3").arg(pm.id, "ITEM").arg(i);
            //group_model.parameterInfo.display_name = QString::fromLocal8Bit("Элемент %1").arg(i).toStdString();
            //for (auto p : ti->parameters)
            //{
            //    unit_types::ParameterModel model;
            //    CreateParameterModel(p, group_model.id, model);
            //    group_model.parameters.push_back(model);
            //}
            //pm.parameters.push_back(group_model);
        }
    }

    while (model.parameters.size() > count)
        model.parameters.pop_back();
}

void PropertiesItem::ValueChanged(QtProperty* property, const QVariant& value)
{
    // BASE
    // BASE/NAME
    // BASE/FILE_NAME
    // BASE/INCLUDE_NAME
    // PARAMETERS
    // PARAMETERS/...
    // EDITOR
    // EDITOR/POSITION_X
    // EDITOR/POSITION_Y
    // EDITOR/POSITION_Z

    qDebug() << "ValueChanged value = " << value;

    auto pm = GetParameterModel(property);
    if (pm == nullptr)
        return;

    if (pm->id.startsWith("BASE"))
    {
        if (pm->id == "BASE/NAME")
        {
            //QString oldName = pm->value.toString();
            //pm->value = value;
            //file_items_manager_->InformNameChanged(value.toString(), oldName);
        }
        else if (pm->id == "BASE/FILE_NAME")
        {
            pm->value = property->valueText();

            QStringList includeNames;
            propertiesItemsManager_->AfterFileNameChanged(this, includeNames);

            SetGroupNames(includeNames);
            SetGroupName("<not selected>");
        }
        else if (pm->id == "BASE/INCLUDE_NAME")
        {
            pm->value = property->valueText();

            //QList<QPair<QString, QString>> variables;
            //propertiesItemsManager_->AfterIncludeNameChanged(this, variables);
            propertiesItemsManager_->AfterIncludeNameChanged(this);



            //SetGroupNames(includeNames);
            //SetGroupName("<not selected>");
        }
    }
    else if (pm->id.startsWith("PARAMETERS"))
    {
        auto& pi = *parameters_compiler::helper::get_parameter_info(unitParameters_.fileInfo, pm->parameterInfoId.type.toStdString(), pm->parameterInfoId.name.toStdString());
        bool is_array = parameters_compiler::helper::is_array_type(pi.type);
        std::string pi_type = pi.type;

        QStringList path = pm->id.split("/");
        if (path.size() > 2 && path.back().startsWith("ITEM_"))
        {
            is_array = false;
            pi_type = parameters_compiler::helper::get_array_type(pi.type);
        }

        if (is_array/* && pm->id == QString("%1/%2").arg("PARAMETERS", pm->parameterInfoId.name)*/)
        {
            int count = std::stoi(property->valueText().toStdString());
            pm->value = count;
            UpdateArrayModel(nullptr, *pm);
        
            QMap<QString, const QtProperty*> idToProperty;
            for (int i = property->subProperties().size(); i < count; ++i)
                property->addSubProperty(editor_->CreatePropertyForModel(pm->parameters[i], idToProperty));
            for (const auto& kvp : idToProperty.toStdMap())
                RegisterProperty(kvp.second, kvp.first);

            auto collect = [](QtProperty* property, QList<QtProperty*>& list, auto&& collect) -> void {
                list.push_back(property);
                for (const auto& p : property->subProperties())
                    collect(p, list, collect);
            };

            QList<QtProperty*> toRemove;
            QList<QtProperty*> toUnregister;
            const auto& subProperties = property->subProperties();
            for (int i = count; i < subProperties.size(); ++i)
            {
                collect(subProperties[i], toUnregister, collect);
                toRemove.push_back(subProperties[i]);
            }

            for (auto& p : toRemove)
                property->removeSubProperty(p);

            for (auto& p : toUnregister)
                UnregisterProperty(p);

            ApplyExpandState();
        }
        else
        {
            if (pi_type == "unit" || pi_type == "path" || pi_type == "string")
                pm->value = property->valueText();
            else if (pi_type == "int" || pi_type == "int8_t" || pi_type == "int16_t" || pi_type == "int32_t" ||
                pi_type == "int64_t" || pi_type == "uint8_t" || pi_type == "uint16_t" || pi_type == "uint32_t" || pi_type == "uint64_t")
                pm->value = std::stoi(property->valueText().toStdString());
            else if (pi_type == "double" || pi_type == "float")
                pm->value = std::stod(property->valueText().toStdString());
            else // enum
                pm->value = property->valueText();
        }
    }
    else if (pm->id.startsWith("EDITOR"))
    {
        if (pm->id == "EDITOR/POSITION_X")
        {
            pm->value = property->valueText().toDouble();
            double posX = property->valueText().toDouble();

            auto pmY = GetParameterModel("EDITOR/POSITION_Y");
            double posY = pmY->value.toDouble();

            auto pmZ = GetParameterModel("EDITOR/POSITION_Z");
            double posZ = pmZ->value.toDouble();

            propertiesItemsManager_->AfterPositionChanged(this, posX, posY, posZ);
        }
        else if (pm->id == "EDITOR/POSITION_Y")
        {
            auto pmX = GetParameterModel("EDITOR/POSITION_X");
            double posX = pmX->value.toDouble();

            pm->value = property->valueText().toDouble();
            double posY = property->valueText().toDouble();

            auto pmZ = GetParameterModel("EDITOR/POSITION_Z");
            double posZ = pmZ->value.toDouble();

            propertiesItemsManager_->AfterPositionChanged(this, posX, posY, posZ);
        }
        else if (pm->id == "EDITOR/POSITION_Z")
        {
            auto pmX = GetParameterModel("EDITOR/POSITION_X");
            double posX = pmX->value.toDouble();

            auto pmY = GetParameterModel("EDITOR/POSITION_Y");
            double posY = pmY->value.toDouble();

            pm->value = property->valueText().toDouble();
            double posZ = property->valueText().toDouble();

            propertiesItemsManager_->AfterPositionChanged(this, posX, posY, posZ);
        }
    }
}

void PropertiesItem::StringEditingFinished(QtProperty* property, const QString& value, const QString& oldValue)
{
    qDebug() << "StringEditingFinished value = " << value << ", oldValue = " << oldValue;

    auto pm = GetParameterModel(property);
    if (pm == nullptr)
        return;

    if (pm->id == "BASE/NAME")
    {
        pm->value = value;
        propertiesItemsManager_->AfterNameChanged(this);

        //bool cancel = false;
        //properties_items_manager_->BeforeNameChanged(propertiesId_, value, oldValue, cancel);

        //if (!cancel)
        //{
        //    QString oldName = pm->value.toString();
        //    pm->value = value;
        //    properties_items_manager_->AfterNameChanged(propertiesId_, value, oldName);
        //}
        //else
        //{
        //    // Отмена
        //    editor_->SetStringValue(property, oldValue);
        //}
    }

}

//////
//////void properties_item::valueChanged(QtProperty* property, const QVariant& value)
//////{
//////    qDebug() << "valueChanged value = " << value;
//////
//////    auto pm = GetParameterModel(property);
//////    if (pm == nullptr)
//////        return;
//////
//////    if (pm->id.startsWith("BASE"))
//////    {
//////        if (pm->id == "BASE/FILE")
//////        {
//////            pm->value = property->valueText();
//////            diagramItem_->InformFileChanged();
//////        }
//////        else if (pm->id == "BASE/INCLUDE_NAME")
//////        {
//////            //diagramItem_->InformGroupChanged();
//////            //if (property->valueText() == "<not selected>")
//////            //{
//////            //    SetFileNameReadOnly(false);
//////            //}
//////            //else
//////            //{
//////            //    //if (diagramItem_->scene() != nullptr)
//////            //    //{
//////            //    //    // Не обновляется заменить на Inform!!!
//////            //    //    QString group = reinterpret_cast<diagram_scene*>(diagramItem_->scene())->getMain()->GetGroupFile(property->valueText());
//////            //    //    SetGroupName(group);
//////            //    //}
//////            //    //SetFileNameReadOnly(true);
//////            //}
//////            pm->value = property->valueText();
//////            diagramItem_->InformGroupChanged();
//////        }
//////
//////    }
//////    else if (pm->id.startsWith("PARAMETERS"))
//////    {
//////        auto& pi = *parameters_compiler::helper::get_parameter_info(unitParameters_.fileInfo, pm->parameterInfoId.type.toStdString(), pm->parameterInfoId.name.toStdString());
//////
//////        bool is_array = parameters_compiler::helper::is_array_type(pi.type);
//////        if (is_array && pm->id == QString("%1/%2").arg("PARAMETERS", pm->parameterInfoId.name))
//////        {
//////            SaveExpandState();
//////
//////            int count = std::stoi(property->valueText().toStdString());
//////            pm->value = count;
//////            UpdateArrayModel(*pm);
//////
//////            for (int i = property->subProperties().size(); i < count; ++i)
//////                property->addSubProperty(GetPropertyForModel(pm->parameters[i]));
//////
//////            QList<QtProperty*> to_remove;
//////            for (int i = count; i < property->subProperties().size(); ++i)
//////            {
//////                auto p = property->subProperties()[i];
//////                to_remove.push_back(p);
//////                UnregisterProperty(p);
//////            }
//////
//////            for (auto& p : to_remove)
//////                property->removeSubProperty(p);
//////
//////            ApplyExpandState();
//////        }
//////        else
//////        {
//////            if (pi.type == "unit" || pi.type == "path" || pi.type == "string")
//////                pm->value = property->valueText();
//////            else if (pi.type == "int" || pi.type == "int8_t" || pi.type == "int16_t" || pi.type == "int32_t" ||
//////                pi.type == "int64_t" || pi.type == "uint8_t" || pi.type == "uint16_t" || pi.type == "uint32_t" || pi.type == "uint64_t")
//////                pm->value = std::stoi(property->valueText().toStdString());
//////            else if (pi.type == "double" || pi.type == "float")
//////                pm->value = std::stod(property->valueText().toStdString());
//////            else // enum
//////                pm->value = property->valueText();
//////        }
//////    }
//////}
//////
//////void properties_item::valueChanged(QtProperty* property, double value)
//////{
//////    auto pm = GetParameterModel(property);
//////    if (pm == nullptr)
//////        return;
//////
//////    qDebug() << "valueChanged " << pm->id << " = " << value;
//////    pm->value = value;
//////
//////    int gridSize = 20;
//////    if (pm->id == "EDITOR/POSITION_X")
//////    {
//////        qreal xV = round(value / gridSize) * gridSize;
//////        if (xV != value)
//////            doubleManager->setValue(property, xV);
//////        diagramItem_->InformPositionXChanged(xV);
//////        pm->value = xV;
//////    }
//////    else if (pm->id == "EDITOR/POSITION_Y")
//////    {
//////        qreal yV = round(value / gridSize) * gridSize;
//////        if (yV != value)
//////            doubleManager->setValue(property, yV);
//////        diagramItem_->InformPositionYChanged(yV);
//////        pm->value = yV;
//////    }
//////    else if (pm->id == "EDITOR/POSITION_Z")
//////    {
//////        diagramItem_->InformPositionZChanged(value);
//////    }
//////
//////    
//////    //if (!propertyToId_.contains(property))
//////    //    return;
//////
//////    ////if (!currentItem)
//////    ////    return;
//////
//////    //if (scene_->selectedItems().count() > 0 && !scene_->isItemMoving())
//////    //{
//////    //    //diagram_item* gi = qobject_cast<diagram_item*>(sp_scene_->selectedItems()[0]);
//////    //    diagram_item* gi = (diagram_item*)(scene_->selectedItems()[0]);
//////    //    qDebug() << gi->getName();
//////
//////    //    QString id = propertyToId_[property];
//////    //    if (id == "Position X")
//////    //        gi->setX(value);
//////    //    else if (id == "Position Y")
//////    //        gi->setY(value);
//////    //}
//////}
//////
//////void properties_item::valueChanged(QtProperty* property, const QString& value)
//////{
//////    auto pm = GetParameterModel(property);
//////    if (pm == nullptr)
//////        return;
//////
//////    QString oldValue = pm->value.toString();
//////    qDebug() << "valueChanged " << pm->id << " = " << value;
//////    pm->value = value;
//////
//////    if (pm->id == "BASE/NAME")
//////    {
//////        diagramItem_->InformNameChanged(value, oldValue);
//////    }
//////    //    if (!propertyToId_.contains(property))
//////    //        return;
//////
//////    //    if (!currentItem)
//////    //        return;
//////
//////    //    QString id = propertyToId_[property];
//////    //    if (id == QLatin1String("text")) {
//////    //        if (currentItem->rtti() == QtCanvasItem::Rtti_Text) {
//////    //            QtCanvasText *i = (QtCanvasText *)currentItem;
//////    //            i->setText(value);
//////    //        }
//////    //    }
//////    //    canvas->update();
//////}
//////
//////void properties_item::valueChanged(QtProperty* property, const QColor& value)
//////{
//////    //    if (!propertyToId_.contains(property))
//////    //        return;
//////
//////    //    if (!currentItem)
//////    //        return;
//////
//////    //    QString id = propertyToId_[property];
//////    //    if (id == QLatin1String("color")) {
//////    //        if (currentItem->rtti() == QtCanvasItem::Rtti_Text) {
//////    //            QtCanvasText *i = (QtCanvasText *)currentItem;
//////    //            i->setColor(value);
//////    //        }
//////    //    } else if (id == QLatin1String("brush")) {
//////    //        if (currentItem->rtti() == QtCanvasItem::Rtti_Rectangle ||
//////    //                currentItem->rtti() == QtCanvasItem::Rtti_Ellipse) {
//////    //            QtCanvasPolygonalItem *i = (QtCanvasPolygonalItem *)currentItem;
//////    //            QBrush b = i->brush();
//////    //            b.setColor(value);
//////    //            i->setBrush(b);
//////    //        }
//////    //    } else if (id == QLatin1String("pen")) {
//////    //        if (currentItem->rtti() == QtCanvasItem::Rtti_Rectangle ||
//////    //                currentItem->rtti() == QtCanvasItem::Rtti_Line) {
//////    //            QtCanvasPolygonalItem *i = (QtCanvasPolygonalItem *)currentItem;
//////    //            QPen p = i->pen();
//////    //            p.setColor(value);
//////    //            i->setPen(p);
//////    //        }
//////    //    }
//////    //    canvas->update();
//////}
//////
//////void properties_item::valueChanged(QtProperty* property, const QFont& value)
//////{
//////    //    if (!propertyToId_.contains(property))
//////    //        return;
//////
//////    //    if (!currentItem)
//////    //        return;
//////
//////    //    QString id = propertyToId_[property];
//////    //    if (id == QLatin1String("font")) {
//////    //        if (currentItem->rtti() == QtCanvasItem::Rtti_Text) {
//////    //            QtCanvasText *i = (QtCanvasText *)currentItem;
//////    //            i->setFont(value);
//////    //        }
//////    //    }
//////    //    canvas->update();
//////}
//////
//////void properties_item::valueChanged(QtProperty* property, const QPoint& value)
//////{
//////    //    if (!propertyToId_.contains(property))
//////    //        return;
//////
//////    //    if (!currentItem)
//////    //        return;
//////
//////    //    QString id = propertyToId_[property];
//////    //    if (currentItem->rtti() == QtCanvasItem::Rtti_Line) {
//////    //        QtCanvasLine *i = (QtCanvasLine *)currentItem;
//////    //        if (id == QLatin1String("endpoint")) {
//////    //            i->setPoints(i->startPoint().x(), i->startPoint().y(), value.x(), value.y());
//////    //        }
//////    //    }
//////    //    canvas->update();
//////}
//////
//////void properties_item::valueChanged(QtProperty* property, const QSize& value)
//////{
//////    //    if (!propertyToId_.contains(property))
//////    //        return;
//////
//////    //    if (!currentItem)
//////    //        return;
//////
//////    //    QString id = propertyToId_[property];
//////    //    if (id == QLatin1String("size")) {
//////    //        if (currentItem->rtti() == QtCanvasItem::Rtti_Rectangle) {
//////    //            QtCanvasRectangle *i = (QtCanvasRectangle *)currentItem;
//////    //            i->setSize(value.width(), value.height());
//////    //        } else if (currentItem->rtti() == QtCanvasItem::Rtti_Ellipse) {
//////    //            QtCanvasEllipse *i = (QtCanvasEllipse *)currentItem;
//////    //            i->setSize(value.width(), value.height());
//////    //        }
//////    //    }
//////    //    canvas->update();
//////}
//////
//////void properties_item::valueChanged(QtProperty* property, bool value)
//////{
//////    auto pm = GetParameterModel(property);
//////    if (pm == nullptr)
//////        return;
//////
//////    qDebug() << "valueChanged " << pm->id << " = " << value;
//////    pm->value = value;
//////
//////    if (pm->id.endsWith("/DEPENDS") && pm->valueType == "bool")
//////    {
//////        diagramItem_->InformDependencyChanged();
//////    }
//////}
//////
//////void properties_item::editingFinished(QtProperty* property, const QString& value, const QString& oldValue)
//////{
//////    qDebug() << "!!!" << value << "   -   " << oldValue;
//////}

void PropertiesItem::RegisterProperty(const QtProperty* property, const QString& id)
{
    propertyToId_[property] = id;
    idToProperty_[id] = property;
}

void PropertiesItem::UnregisterProperty(const QString& id)
{
    UnregisterProperty(idToProperty_[id]);
}

void PropertiesItem::UnregisterProperty(const QtProperty* property)
{
    for (auto p : property->subProperties())
        UnregisterProperty(p);

    idToProperty_.remove(propertyToId_[property]);
    propertyToId_.remove(property);
}

QtProperty* PropertiesItem::GetProperty(const QString& id)
{
    auto it = idToProperty_.find(id);
    if (it != idToProperty_.end())
        return const_cast<QtProperty*>(*it);
    return nullptr;

    //if (idToProperty_.contains(id))
    //    return idToProperty_[id];
    //else
    //    return nullptr;
}

QString PropertiesItem::GetPropertyId(const QtProperty* property)
{
    auto it = propertyToId_.find(property);
    if (it != propertyToId_.end())
        return *it;
    return QString();

    //if (propertyToId_.contains(property))
    //    return propertyToId_[property];
    //else
    //    return QString();
}

CubesUnitTypes::ParameterModel* PropertiesItem::GetParameterModel(const QString& id)
{
    CubesUnitTypes::ParameterModel* pm = nullptr;

    {
        QStringList sl = id.split("/");
        auto ql = &model_.parameters;
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
            {
                pm = nullptr;
                break;
            }
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

CubesUnitTypes::ParameterModel* PropertiesItem::GetParameterModel(const QtProperty* property)
{
    QString id = GetPropertyId(property);
    if (id == "")
        return nullptr;

    return GetParameterModel(id);
}

//bool properties_item::GetExpanded(QtProperty* property)
//{
//    return false;
//}
//
//void properties_item::SaveExpandState(QtBrowserItem* index)
//{
//    if (propertyEditor_ == nullptr)
//        return;
//
//    QList<QtBrowserItem*> children = index->children();
//    QListIterator<QtBrowserItem*> itChild(children);
//    while (itChild.hasNext())
//        SaveExpandState(itChild.next());
//    QtProperty* prop = index->property();
//
//    auto pm = GetParameterModel(prop);
//    if (pm != nullptr)
//        pm->editorSettings.is_expanded = propertyEditor_->isExpanded(index);
//}
//
//void properties_item::SaveExpandState()
//{
//    if (propertyEditor_ == nullptr)
//        return;
//
//    QList<QtBrowserItem*> indexes = propertyEditor_->topLevelItems();
//    QListIterator<QtBrowserItem*> itItem(indexes);
//    while (itItem.hasNext())
//        SaveExpandState(itItem.next());
//}
//
//void properties_item::ApplyExpandState(QtBrowserItem* index)
//{
//    if (propertyEditor_ == nullptr)
//        return;
//
//    QList<QtBrowserItem*> children = index->children();
//    QListIterator<QtBrowserItem*> itChild(children);
//    while (itChild.hasNext())
//        ApplyExpandState(itChild.next());
//    QtProperty* prop = index->property();
//
//    auto pm = GetParameterModel(prop);
//    if (pm != nullptr)
//        propertyEditor_->setExpanded(index, pm->editorSettings.is_expanded);
//}
//
//void properties_item::ApplyExpandState()
//{
//    if (propertyEditor_ == nullptr)
//        return;
//
//    QList<QtBrowserItem*> indexes = propertyEditor_->topLevelItems();
//    QListIterator<QtBrowserItem*> itItem(indexes);
//    while (itItem.hasNext())
//        ApplyExpandState(itItem.next());
//}

void PropertiesItem::ApplyExpandState(QtBrowserItem* index)
{
    auto pe = editor_->GetPropertyEditor();

    QList<QtBrowserItem*> children = index->children();
    QListIterator<QtBrowserItem*> itChild(children);
    while (itChild.hasNext())
        ApplyExpandState(itChild.next());
    QtProperty* prop = index->property();

    auto pm = GetParameterModel(prop);
    if (pm != nullptr)
        pe->setExpanded(index, pm->editorSettings.is_expanded);
}

void PropertiesItem::ApplyExpandState()
{
    auto pe = editor_->GetPropertyEditor();

    QList<QtBrowserItem*> indexes = pe->topLevelItems();
    QListIterator<QtBrowserItem*> itItem(indexes);
    while (itItem.hasNext())
        ApplyExpandState(itItem.next());
}
