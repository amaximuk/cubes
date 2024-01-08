#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "parameters.h"
#include "../main_window.h"
#include "../diagram/diagram_scene.h"
#include "../diagram/diagram_item.h"
#include "../parameters_compiler/variant_converter.h"
#include "../parameters_compiler/base64.h"
#include "properties_item.h"

using namespace CubesProperties;

const QString CubesProperties::baseGroupName = QString::fromLocal8Bit("$BASE");
const QString CubesProperties::parametersGroupName = QString::fromLocal8Bit("$PARAMETERS");
const QString CubesProperties::editorGroupName = QString::fromLocal8Bit("$EDITOR");
const QString CubesProperties::itemGroupName = QString::fromLocal8Bit("$ITEM");
const QString CubesProperties::dependsParameterName = QString::fromLocal8Bit("$DEPENDS");
const QString CubesProperties::optionalParameterName = QString::fromLocal8Bit("$OPTIONAL");

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
    CubesUnitTypes::UnitParameters unitParameters, uint32_t propertiesId, CubesUnitTypes::ParametersModel pm)
{
    propertiesItemsManager_ = propertiesItemsManager;
    editor_ = editor;
    unitParameters_ = unitParameters;
    propertiesId_ = propertiesId;
    model_ = {};
    ignoreEvents_ = false;

    model_.parameters = pm.parameters;
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
        base_group.id = baseGroupName;
        base_group.name = QString::fromLocal8Bit("Базовые");
        base_group.value = QVariant();
        //base_group.valueType = "none";
        //base_group.parameterInfoId = "";
        base_group.editorSettings.type = CubesUnitTypes::EditorType::None;
        base_group.editorSettings.is_expanded = true;

        CubesUnitTypes::ParameterModel instance_name;
        instance_name.id = baseGroupName + "/NAME";
        instance_name.name = QString::fromLocal8Bit("Имя");
        if (xmlUnit == nullptr)
            instance_name.value = QString::fromStdString(parameters::helper::file::get_display_name(unitParameters_.fileInfo));
        else
            instance_name.value = QString(xmlUnit->name);
        //instance_name.valueType = "string";
        //instance_name.parameterInfoId = "";
        instance_name.editorSettings.type = CubesUnitTypes::EditorType::String;
        instance_name.editorSettings.is_expanded = false;
        base_group.parameters.push_back(std::move(instance_name));

        CubesUnitTypes::ParameterModel file;
        file.id = baseGroupName + "/FILE_NAME";
        file.name = QString::fromLocal8Bit("Файл");
        file.value = QString();
        //file.valueType = "string";
        //file.parameterInfoId = "";
        file.editorSettings.type = CubesUnitTypes::EditorType::ComboBox;
        file.editorSettings.is_expanded = false;
        base_group.parameters.push_back(std::move(file));

        CubesUnitTypes::ParameterModel group;
        group.id = baseGroupName + "/INCLUDE_NAME";
        group.name = QString::fromLocal8Bit("Включаемый файл");
        group.value = QString();
        //group.valueType = "string";
        //group.parameterInfoId = "";
        group.editorSettings.type = CubesUnitTypes::EditorType::ComboBox;
        group.editorSettings.is_expanded = false;
        base_group.parameters.push_back(std::move(group));

        model_.parameters.push_back(std::move(base_group));
    }

    if (unitParameters_.fileInfo.parameters.size() > 0)
    {
        CubesUnitTypes::ParameterModel properties_group;
        properties_group.id = parametersGroupName;
        properties_group.name = QString::fromLocal8Bit("Параметры");
        properties_group.value = QVariant();
        //properties_group.valueType = "none";
        //properties_group.parameterInfoId = "";
        properties_group.editorSettings.type = CubesUnitTypes::EditorType::None;
        properties_group.editorSettings.is_expanded = true;

        for (const auto& pi : unitParameters_.fileInfo.parameters)
        {
            CubesUnitTypes::ParameterModel pm;
            CreateParameterModel({ "Main", QString::fromStdString(pi.name) }, parametersGroupName, xmlUnit, pm);
            properties_group.parameters.push_back(std::move(pm));
        }

        model_.parameters.push_back(std::move(properties_group));
    }

    {
        CubesUnitTypes::ParameterModel editor_group;
        editor_group.id = editorGroupName;
        editor_group.name = QString::fromLocal8Bit("Редактор");
        editor_group.value = QVariant();
        //editor_group.valueType = "none";
        //editor_group.parameterInfoId = "";
        editor_group.editorSettings.type = CubesUnitTypes::EditorType::None;
        editor_group.editorSettings.is_expanded = true;

        {
            CubesUnitTypes::ParameterModel pm;
            pm.id = editorGroupName + "/POSITION_X";
            pm.name = QString::fromLocal8Bit("Позиция X");
            pm.value = double{ xmlUnit == nullptr ? 0.0 : xmlUnit->x };
            //pm.valueType = "double";
            //pm.parameterInfoId = "";
            pm.editorSettings.type = CubesUnitTypes::EditorType::SpinDouble;
            pm.editorSettings.SpinDoubleMin = -10000;
            pm.editorSettings.SpinDoubleMax = 10000;
            pm.editorSettings.SpinDoubleSingleStep = 20;
            editor_group.parameters.push_back(std::move(pm));
        }

        {
            CubesUnitTypes::ParameterModel pm;
            pm.id = editorGroupName + "/POSITION_Y";
            pm.name = QString::fromLocal8Bit("Позиция Y");
            pm.value = double{ xmlUnit == nullptr ? 0.0 : xmlUnit->y };
            //pm.valueType = "double";
            //pm.parameterInfoId = "";
            pm.editorSettings.type = CubesUnitTypes::EditorType::SpinDouble;
            pm.editorSettings.SpinDoubleMin = -10000;
            pm.editorSettings.SpinDoubleMax = 10000;
            pm.editorSettings.SpinDoubleSingleStep = 20;
            editor_group.parameters.push_back(std::move(pm));
        }

        {
            CubesUnitTypes::ParameterModel pm;
            pm.id = editorGroupName + "/POSITION_Z";
            pm.name = QString::fromLocal8Bit("Позиция Z");
            pm.value = double{ xmlUnit == nullptr ? 0.0 : xmlUnit->z };
            //pm.valueType = "double";
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

void PropertiesItem::CreateParameterModel(const CubesUnitTypes::ParameterInfoId& parameterInfoId,
    const QString& parentModelId, const CubesXml::Unit* xmlUnit, CubesUnitTypes::ParameterModel& model)
{
    // Создание модели для параметра по его info ID (тип и имя из yml файла)
    // Модель включает все вложенные параметры и массивы
    // Каждому параметру назначается model ID (путь к параметру в модели, разделенный /)

    auto& pi = *parameters::helper::parameter::get_parameter_info(unitParameters_.fileInfo,
        parameterInfoId.type.toStdString(), parameterInfoId.name.toStdString());

    CubesUnitTypes::ParameterModel pm;
    pm.id = QString("%1/%2").arg(parentModelId, QString::fromStdString(pi.name));
    pm.name = QString::fromStdString(parameters::helper::parameter::get_display_name(pi));
    pm.parameterInfoId = parameterInfoId;

    bool is_array = parameters::helper::common::get_is_array_type(pi.type);
    if (is_array)
    {
        FillArrayModel(xmlUnit, pm);
        UpdateArrayModel(xmlUnit, pm);
    }
    else
    {
        FillParameterModel(xmlUnit, pm, false);
    }

    model = pm;
}

void PropertiesItem::FillParameterModel(const CubesXml::Unit* xmlUnit, CubesUnitTypes::ParameterModel& model, bool isItem)
{
    // Заполнение модели параметра, не являющегося массивом, с учетом ограничений
    // Поля id, name, parameterInfoId должны быть предварительно заполнены
    // parameterInfoId может быть не заполнен для дополнительных полей (BASE, EDITOR и т.п.)
    // Если xmlUnit != nullptr, значит создаем юнит из файла xml

    // Предварительно получаем значение параметра из xml файла, если он доступен
    CubesXml::Param* xmlParam = nullptr;
    if (xmlUnit != nullptr)
        xmlParam = CubesXml::Parser::GetParam(*const_cast<CubesXml::Unit*>(xmlUnit), model.id);

    // Предварительно получаем значение элемента массива из xml файла, если он доступен
    CubesXml::Item* xmlItem = nullptr;
    QString xmlItemType;
    if (xmlUnit != nullptr)
        xmlItem = CubesXml::Parser::GetItem(*const_cast<CubesXml::Unit*>(xmlUnit), model.id, xmlItemType);

    // Вычисляем значение из xml файла (параметра или элемента массива)
    QString xmlValueString;
    QString xmlValueTypeString;
    bool haveXmlValue = false;
    if (xmlParam != nullptr)
    {
        xmlValueString = xmlParam->val;
        xmlValueTypeString = xmlParam->type;
        haveXmlValue = true;
    }
    else if (xmlItem != nullptr)
    {
        xmlValueString = xmlItem->val;
        xmlValueTypeString = xmlItemType;
        haveXmlValue = true;
    }

    // Конвертируем в QVariant
    QVariant xmlValue;
    if (haveXmlValue)
    {
        const auto xmlBaseItemType = parameters::helper::common::get_xml_base_item_type(xmlValueTypeString.toStdString());
        switch (xmlBaseItemType)
        {
        case parameters::base_item_types::string:
            xmlValue = QString(xmlValueString);
            break;
        case parameters::base_item_types::integer:
            xmlValue = std::stoi(xmlValueString.toStdString());
            break;
        case parameters::base_item_types::floating:
            xmlValue = std::stod(xmlValueString.toStdString());
            break;
        case parameters::base_item_types::boolean:
            xmlValue = (xmlValueString == "true");
            break;
        case parameters::base_item_types::none:
        case parameters::base_item_types::user:
        default:
            assert(false);
            break;
        }
    }

    // Получаем описание параметра из его yml файла
    auto& pi = *parameters::helper::parameter::get_parameter_info(unitParameters_.fileInfo,
        model.parameterInfoId.type.toStdString(), model.parameterInfoId.name.toStdString());
    auto v = parameters::helper::parameter::get_initial_value(unitParameters_.fileInfo, pi, isItem);
    const bool res = CubesParameters::convert_variant(v, model.value);

    // Параметр не должен быть массивом, здесь была проверка
    // Проблема с типизированными массивами. В них каждый item хранит значение и привязку к
    // базовому типу массива, т.е. при получении parameter_info получим, что каждый item это массив,
    // что на самом деле не так
    // На всякий случай возьмем тип элемента
    const auto itemType = parameters::helper::common::get_item_type(pi.type);

    const auto isUnitType = parameters::helper::common::get_is_unit_type(itemType);
    const auto baseItemType = parameters::helper::common::get_base_item_type(itemType);

    if (xmlParam != nullptr)
    {
        // Проверяем совместимость типов параметров из xml и из описания yml
        auto xmlBaseType = parameters::helper::common::get_xml_base_item_type(xmlParam->type.toStdString());
        if (xmlBaseType != baseItemType)
        {
            propertiesItemsManager_->AfterError(this, QString::fromLocal8Bit("Тип данных в xml не совместим с типом параметра"));
            // Ошибка! Тип данных в xml не совместим с типом параметра
            // TODO: вернуть ошибку
        }
    }

    if (pi.restrictions.set_.size() > 0)
    {
        // Задан список допустимых значений
        model.editorSettings.type = CubesUnitTypes::EditorType::ComboBox;

        // Заполняем допустимые значения из ограничений в yml файле
        for (const auto& s : pi.restrictions.set_)
            model.editorSettings.ComboBoxValues.push_back(QString::fromStdString(s));

        // Проверяем допустимость значений из xml файла (параметра или элемента массива)
        if (haveXmlValue)
        {
            // Проверяем ограничения на список элементов
            if (!model.editorSettings.ComboBoxValues.contains(xmlValue.toString()))
            {
                propertiesItemsManager_->AfterError(this, QString::fromLocal8Bit("Значение параметра в xml не удовлетворяет ограничениям"));
                // Ошибка! Значение параметра в xml не удовлетворяет ограничениям
                // TODO: вернуть ошибку
            }
        }
    }
    else
    {
        // Ограничения на список значений не заданы, заполняем модель в зависимости от базового типа
        if (isUnitType)
        {
            // Это особый случай - тип unit, его базовый тип строка
            model.editorSettings.type = CubesUnitTypes::EditorType::String;

            {
                // Для типа unit добавляется дополнительное поле - зависимость (depends)
                CubesUnitTypes::ParameterModel pm_depends;
                pm_depends.id = QString("%1/%2").arg(model.id, dependsParameterName);
                pm_depends.name = QString::fromLocal8Bit("Зависимость");
                // Если есть значение в xml, заполняем его в модели зависимости
                if (xmlParam == nullptr)
                    pm_depends.value = bool{ false };
                else
                    pm_depends.value = bool{ xmlParam->depends };
                //pm_depends.valueType = "bool";
                pm_depends.editorSettings.type = CubesUnitTypes::EditorType::CheckBox;

                model.parameters.push_back(std::move(pm_depends));
            }
        }
        else if (baseItemType == parameters::base_item_types::string)
        {
            // Остальные типы, основанные на строках (path, string и т.п.)
            model.editorSettings.type = CubesUnitTypes::EditorType::String;
        }
        else if (baseItemType == parameters::base_item_types::boolean)
        {
            // Тип bool
            model.editorSettings.type = CubesUnitTypes::EditorType::CheckBox;
        }
        else if (baseItemType == parameters::base_item_types::integer)
        {
            // Типы, основанные на целочисленных значениях int (int8_t, uint16_t и т.п.)
            model.editorSettings.type = CubesUnitTypes::EditorType::SpinInterger;

            // Устанавливаем ограничения из yml файла юнита
            if (pi.restrictions.min != "")
                model.editorSettings.SpinIntergerMin = std::stoi(pi.restrictions.min);
            else
                model.editorSettings.SpinIntergerMin = parameters::helper::common::get_min_for_integral_type(itemType);
            if (pi.restrictions.max != "")
                model.editorSettings.SpinIntergerMax = std::stoi(pi.restrictions.max);
            else
                model.editorSettings.SpinIntergerMax = parameters::helper::common::get_max_for_integral_type(itemType);
        }
        else if (baseItemType == parameters::base_item_types::floating)
        {
            // Типы, основанные на значениях с плавающей точкой double (float, double)
            model.editorSettings.type = CubesUnitTypes::EditorType::SpinDouble;

            // Устанавливаем ограничения из yml файла юнита
            if (pi.restrictions.min != "")
                model.editorSettings.SpinDoubleMin = std::stod(pi.restrictions.min);
            else
                model.editorSettings.SpinDoubleMin = parameters::helper::common::get_min_for_floating_point_type(itemType);
            if (pi.restrictions.max != "")
                model.editorSettings.SpinDoubleMax = std::stod(pi.restrictions.max);
            else
                model.editorSettings.SpinDoubleMax = parameters::helper::common::get_max_for_floating_point_type(itemType);
        }
        else if (baseItemType == parameters::base_item_types::user)
        {
            // Пользовательский тип данных
            // Поскольку параметр не является массивом, единственный допустимый тип это enum
            const auto pti = parameters::helper::type::get_type_info(unitParameters_.fileInfo, itemType);
            const auto typeCategory = parameters::helper::type::get_category(*pti);
            if (typeCategory != parameters::type_category::user_cpp)
                assert(false);

            model.editorSettings.type = CubesUnitTypes::EditorType::ComboBox;

            // Заполняем допустимые значения перечисления из описания юнита в yml файле в ComboBox
            // TODO: могут быть еще ограничения на список значений, это не учтено
            if (pti->values.size() > 0)
            {
                for (const auto v : pti->values)
                    model.editorSettings.ComboBoxValues.push_back(QString::fromStdString(v.first));
            }

            // Проверяем допустимость значений из xml файла
            if (haveXmlValue)
            {
                if (!model.editorSettings.ComboBoxValues.contains(xmlValue.toString()))
                {
                    propertiesItemsManager_->AfterError(this, QString::fromLocal8Bit("Значение параметра в xml не удовлетворяет ограничениям"));
                    // Ошибка! Значение параметра в xml не удовлетворяет ограничениям
                    // TODO: вернуть ошибку
                }
            }
        }
        else
        {
            // Тип неизвестен
            assert(false);
        }
    }

    // Установка значения из xml в модель (если не задано, берется значение по-умолчанию для типа)
    if (haveXmlValue)
        model.value = xmlValue; // it's QVariant

    // Для опциональных параметров добавляем дополнительное поле - не задавать
    if (parameters::helper::parameter::get_is_optional(pi))
    {
        CubesUnitTypes::ParameterModel pmo;
        pmo.id = QString("%1/%2").arg(model.id, optionalParameterName);
        pmo.name = QString::fromLocal8Bit("Не задавать");
        pmo.value = bool{ false };
        //pmo.valueType = "bool";
        pmo.editorSettings.type = CubesUnitTypes::EditorType::CheckBox;

        // Если xml файл есть, устанавливаем значение флага
        if (xmlUnit != nullptr)
        {
            if (xmlParam == nullptr)
            {
                // Параметр отсутствует, ставим флаг не задавать
                // TODO: надо как-то помечать не заданные параметры,
                // вариант - model.value = QString::fromLocal8Bit("не задано"); - плохой, не учитывает тип данных
                pmo.value = bool{ true };
            }
            else
            {
                // Параметр есть, сбрасываем флаг не задавать
                pmo.value = bool{ false };
            }
        }

        model.parameters.push_back(std::move(pmo));
    }
}

void PropertiesItem::SetFileNames(QStringList fileNames)
{
    const auto pm = GetParameterModel(baseGroupName + "/FILE_NAME");
    if (pm != nullptr)
    {
        pm->editorSettings.ComboBoxValues = fileNames;

        // Блокируем сигналы, чтобы не сбрасывался выбранный включаемыый файл, т.к. по сигналу
        // изменения файла автоматически устанавливается значение <не установлено>,
        // даже если фактически файл остается тем же
        editor_->blockSignals(true);
        editor_->SetEnumValues(GetProperty(pm->id), fileNames);
        editor_->blockSignals(false);
    }
}

void PropertiesItem::SetFileName(QString fileName)
{
    const auto pm = GetParameterModel(baseGroupName + "/FILE_NAME");
    if (pm != nullptr)
    {
        pm->value = QString(fileName);
        editor_->SetEnumValue(GetProperty(pm->id), pm->value);
    }
}

void PropertiesItem::SetFileNameReadOnly(bool readOnly)
{
    const auto pm = GetParameterModel(baseGroupName + "/FILE_NAME");
    if (pm != nullptr)
    {
        pm->readOnly = readOnly;
        editor_->SetReadOnly(GetProperty(pm->id), readOnly);
    }
}

void PropertiesItem::SetIncludeNameReadOnly(bool readOnly)
{
    const auto pm = GetParameterModel(baseGroupName + "/INCLUDE_NAME");
    if (pm != nullptr)
    {
        pm->readOnly = readOnly;
        editor_->SetReadOnly(GetProperty(pm->id), readOnly);
    }
}

void PropertiesItem::SetIncludeNames(QStringList includeNames)
{
    QString oldName = GetIncludeName();
    if (!includeNames.contains(oldName))
        oldName = includeNames[0]; // <not selected>
    const auto pm = GetParameterModel(baseGroupName + "/INCLUDE_NAME");
    if (pm != nullptr)
    {
        pm->editorSettings.ComboBoxValues = includeNames;
        editor_->SetEnumValues(GetProperty(pm->id), includeNames);
    }
    SetIncludeName(oldName);
}

void PropertiesItem::SetIncludeName(QString groupName)
{
    const auto pm = GetParameterModel(baseGroupName + "/INCLUDE_NAME");
    if (pm != nullptr)
    {
        pm->value = QString(groupName);
        editor_->SetEnumValue(GetProperty(pm->id), pm->value);
    }
}

QString PropertiesItem::GetFileName()
{
    const auto pm = GetParameterModel(baseGroupName + "/FILE_NAME");
    if (pm != nullptr)
        return pm->value.toString();
    else
        return "";
}

QString PropertiesItem::GetIncludeName()
{
    const auto pm = GetParameterModel(baseGroupName + "/INCLUDE_NAME");
    if (pm != nullptr)
        return pm->value.toString();
    else
        return "";
}

void PropertiesItem::SetName(QString name)
{
    const auto pm = GetParameterModel(baseGroupName + "/NAME");
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
    const auto pm = GetParameterModel(baseGroupName + "/NAME");
    if (pm != nullptr)
        return pm->value.toString();
    else
        return "";
}

void PropertiesItem::GetXmlProperties(const CubesUnitTypes::ParameterModel& pm,
    QList<CubesXml::Param>& params, QList<CubesXml::Array>& arrays)
{
    /*struct Param
    {
        QString name;
        QString type;
        QString val;
        bool depends;
    };

    struct Item
    {
        QString val;
        QList<Param> params;
        QList<Array> arrays;
    };

    struct Array
    {
        QString name;
        QString type;
        QList<Item> items;
    };*/
}

bool PropertiesItem::GetXmlParam(const CubesUnitTypes::ParameterModel& pm, CubesXml::Param& param)
{
    // Получает параметр в формате xml файла (CubesXml::Param) по данным из модели
    // Если параметр помечен как не выдаваемый (OPTIONAL), возвращаем false
    // Модель можетт содержать дополнительные параметры, например для модели pm:
    //     PARAMETERS/CHANNELS/ITEM_0/PARAMETERS/COMMUTATOR_NAME
    // pm.parameters может содержать:
    //     PARAMETERS/CHANNELS/ITEM_0/PARAMETERS/COMMUTATOR_NAME/DEPENDS
    //     PARAMETERS/CHANNELS/ITEM_0/PARAMETERS/COMMUTATOR_NAME/OPTIONAL
    // Массивы в данной функции не обрабатываются, для получения массива используется GetXmlArrray

    auto& pi = *parameters::helper::parameter::get_parameter_info(unitParameters_.fileInfo,
        pm.parameterInfoId.type.toStdString(), pm.parameterInfoId.name.toStdString());

    auto typeName = parameters::helper::parameter::get_type_xml(unitParameters_.fileInfo, pi);

    bool depends{false};
    bool notSet{false};
    for (auto& pmParameter : pm.parameters)
    {
        if (pmParameter.id == pm.id + "/" + dependsParameterName)
            depends = pmParameter.value.toBool();
        else if (pmParameter.id == pm.id + "/" + optionalParameterName)
            notSet = pmParameter.value.toBool();
    }

    if (notSet)
        return false;

    param.name = QString::fromStdString(pi.name);
    param.type = QString::fromStdString(typeName);
    param.val = pm.value.toString();
    param.depends = depends;

    return true;
}

bool PropertiesItem::GetXmlArrray(const CubesUnitTypes::ParameterModel& pm, CubesXml::Array& array)
{
    // Получает массив в формате xml файла (CubesXml::Array) по данным из модели
    // На входе pm - это модель параметра массива, например
    //     PARAMETERS/CHANNELS

    if (pm.parameters.size() == 0)
        return false;

    auto& pi = *parameters::helper::parameter::get_parameter_info(unitParameters_.fileInfo,
        pm.parameterInfoId.type.toStdString(), pm.parameterInfoId.name.toStdString());

    array.name = QString::fromStdString(pi.name);

    bool is_inner_type = parameters::helper::common::get_is_inner_type(pi.type);
    if (is_inner_type)
    {
        // Для внутренних типов значения в хранятся в самих item:
        //     PARAMETERS/CHANNELS/ITEM_0
        //     PARAMETERS/CHANNELS/ITEM_1

        auto itemTypeXmlName = parameters::helper::parameter::get_item_type_xml(unitParameters_.fileInfo, pi);

        array.type = QString::fromStdString(itemTypeXmlName);

        // TODO: depends pm.valueType = int????????
        // depends - no type (have int)

        for (const auto& pmItem : pm.parameters)
        {
            CubesXml::Item item{};
            item.val = pmItem.value.toString();
            array.items.push_back(item);
        }
    }
    else
    {
        // Для типов yml значения в хранятся структуре item:
        //     PARAMETERS/CHANNELS/ITEM_0/BASE/NAME
        //     PARAMETERS/CHANNELS/ITEM_0/PARAMETERS/COMMUTATOR_NAME
        //     ...
        //     PARAMETERS/CHANNELS/ITEM_0/EDITOR/POSITION_X
        //     PARAMETERS/CHANNELS/ITEM_0/EDITOR/POSITION_Y
        //     PARAMETERS/CHANNELS/ITEM_0/EDITOR/POSITION_Z

        array.type = "";

        for (const auto& pmItem : pm.parameters)
        {
            CubesXml::Item item{};
            for (const auto& pmGroup : pmItem.parameters)
            {
                QString id = pmGroup.id;
                id.replace(pmItem.id + "/", "");
                if (id == baseGroupName)
                {
                    for (auto& pmBase : pmGroup.parameters)
                    {
                        QString idBase = pmBase.id;
                        idBase.replace(pmItem.id + "/", "");
                        if (idBase == baseGroupName + "/NAME")
                            item.name = pmBase.value.toString();
                    }
                }
                else if (id.startsWith(parametersGroupName))
                {
                    for (auto& pmParameter : pmGroup.parameters)
                    {
                        auto& pi = *parameters::helper::parameter::get_parameter_info(unitParameters_.fileInfo,
                            pmParameter.parameterInfoId.type.toStdString(), pmParameter.parameterInfoId.name.toStdString());
                        bool is_array = parameters::helper::common::get_is_array_type(pi.type);
                        if (is_array)
                        {
                            CubesXml::Array array{};
                            if (GetXmlArrray(pmParameter, array))
                                item.arrays.push_back(array);
                        }
                        else
                        {
                            CubesXml::Param param{};
                            if (GetXmlParam(pmParameter, param))
                                item.params.push_back(param);
                        }
                    }
                }
                else if (id == editorGroupName)
                {
                    for (auto& pmEditor : pmGroup.parameters)
                    {
                        QString idEditor = pmEditor.id;
                        idEditor.replace(pmItem.id + "/", "");

                        if (idEditor == editorGroupName + "/POSITION_X")
                            item.x = pmEditor.value.toInt();
                        else if (idEditor == editorGroupName + "/POSITION_Y")
                            item.y = pmEditor.value.toInt();
                        else if (idEditor == editorGroupName + "/POSITION_Z")
                            item.z = pmEditor.value.toInt();
                    }
                }
            }
            array.items.push_back(item);
        }
    }

    return true;
}

void PropertiesItem::GetXml(CubesXml::Unit& xmlUnit)
{
    xmlUnit.id = QString::fromStdString(unitParameters_.fileInfo.info.id);
    
    for (auto& pmGroup : model_.parameters)
    {
        if (pmGroup.id == baseGroupName)
        {
            for (auto& pmBase : pmGroup.parameters)
            {
                if (pmBase.id == baseGroupName + "/NAME")
                    xmlUnit.name = pmBase.value.toString();
            }
        }
        else if (pmGroup.id.startsWith(parametersGroupName))
        {
            for (auto& pmParameter : pmGroup.parameters)
            {
                auto& pi = *parameters::helper::parameter::get_parameter_info(unitParameters_.fileInfo,
                    pmParameter.parameterInfoId.type.toStdString(), pmParameter.parameterInfoId.name.toStdString());
                bool is_array = parameters::helper::common::get_is_array_type(pi.type);
                if (is_array)
                {
                    CubesXml::Array array{};
                    if (GetXmlArrray(pmParameter, array))
                        xmlUnit.arrays.push_back(array);
                }
                else
                {
                    CubesXml::Param param{};
                    if (GetXmlParam(pmParameter, param))
                        xmlUnit.params.push_back(param);
                }
            }
        }
        else if (pmGroup.id == editorGroupName)
        {
            for (auto& pmEditor: pmGroup.parameters)
            {
                if (pmEditor.id == editorGroupName + "/POSITION_X")
                    xmlUnit.x = pmEditor.value.toInt();
                else if (pmEditor.id == editorGroupName + "/POSITION_Y")
                    xmlUnit.y = pmEditor.value.toInt();
                else if (pmEditor.id == editorGroupName + "/POSITION_Z")
                    xmlUnit.z = pmEditor.value.toInt();
            }
        }
    }
}

void PropertiesItem::RemoveSubProperties(QtProperty* property)
{
    auto collect = [](QtProperty* property, QList<QtProperty*>& list, auto&& collect) -> void {
        list.push_back(property);
        for (const auto& p : property->subProperties())
        {
            collect(p, list, collect);
            property->removeSubProperty(p);
        }
    };

    QList<QtProperty*> toUnregister;
    collect(property, toUnregister, collect);

    for (auto& p : toUnregister)
    {
        if (p != property)
            UnregisterProperty(p);
    }
}

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
    auto pm_x = GetParameterModel(editorGroupName + "/POSITION_X");
    pm_x->value = point.x();
    editor_->SetDoubleValue(GetProperty(pm_x->id), point.x());

    auto pm_y = GetParameterModel(editorGroupName + "/POSITION_Y");
    pm_y->value = point.y();
    editor_->SetDoubleValue(GetProperty(pm_y->id), point.y());
}

void PropertiesItem::ZOrderChanged(double value)
{
    auto pm = GetParameterModel(editorGroupName + "/POSITION_Z");
    pm->value = value;
    editor_->SetDoubleValue(GetProperty(pm->id), value);
}

QString PropertiesItem::GetPropertyDescription(QtProperty* property)
{
    QString id = GetPropertyId(property);
    return id;
}

void PropertiesItem::GetConnectedNamesInternal(const CubesUnitTypes::ParameterModel& model, QList<QString>& list)
{
    auto pi = parameters::helper::parameter::get_parameter_info(unitParameters_.fileInfo,
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
    auto pi = parameters::helper::parameter::get_parameter_info(unitParameters_.fileInfo,
        model.parameterInfoId.type.toStdString(), model.parameterInfoId.name.toStdString());

    if (model.id == parametersGroupName + "/" + dependsParameterName)
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
            if (sub.id.endsWith(dependsParameterName) &&
                sub.value.type() == QVariant::Type::Bool &&
                sub.value.toBool() == true)
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

QString PropertiesItem::GetInstanceName()
{
    const auto pm = GetParameterModel(baseGroupName + "/NAME");
    if (pm != nullptr)
        return pm->value.toString();
    return QString();
}

QList<QString> PropertiesItem::GetConnectedNames()
{
    QList<QString> list;
    for (const auto& pm : model_.parameters)
    {
        if (pm.id == parametersGroupName)
            GetConnectedNamesInternal(pm, list);
    }
    return list;
}

QList<QString> PropertiesItem::GetDependentNames()
{
    QList<QString> list;
    for (const auto& pm : model_.parameters)
    {
        if (pm.id == parametersGroupName)
            GetDependentNamesInternal(pm, list);
    }
    return list;
}

void PropertiesItem::FillArrayModel(const CubesXml::Unit* xmlUnit, CubesUnitTypes::ParameterModel& model)
{
    // Созадем модель для параметра, хранящего количество элементов массива
    // Если задан перечень элементов, используем ComboBox, если нет - SpinBox
    // Если ограничения не заданы - количество элементов равно нулю
    // Поля id, name, parameterInfoId должны быть предварительно заполнены
    // Если xmlUnit != nullptr, значит создаем юнит из файла xml

    auto& pi = *parameters::helper::parameter::get_parameter_info(unitParameters_.fileInfo, model.parameterInfoId.type.toStdString(), model.parameterInfoId.name.toStdString());
    auto v = parameters::helper::parameter::get_initial_value(unitParameters_.fileInfo, pi, false);
    bool res = CubesParameters::convert_variant(v, model.value);
    //model.valueType = "int";

    int xmlCount = 0;
    if (xmlUnit != nullptr)
        xmlCount = CubesXml::Parser::GetItemsCount(*const_cast<CubesXml::Unit*>(xmlUnit), model.id);

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
            model.editorSettings.SpinIntergerMax = 1000; // !!! TODO: make a define for a const

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

    // Получаем описание параметра из его yml файла
    auto& pi = *parameters::helper::parameter::get_parameter_info(unitParameters_.fileInfo,
        model.parameterInfoId.type.toStdString(), model.parameterInfoId.name.toStdString());

    auto itemType = parameters::helper::common::get_item_type(pi.type);
    int itemsCount = model.value.toInt();

    // Определяем тип массива - типизированный или нет
    // Если тип массива - внутренний тип или перечисление (enum), то
    // элементы массива простые и сами хранят свое значение
    // Если это полльзовательский yml тип, то каждый элемент массива - это структура,
    // имеющая также дополнительный поля BASE и EDITOR и параметры внутри PARAMETERS

    bool isSimpleType = false;
    bool isEnumType = false;
    bool isYmlType = false;
    if (parameters::helper::common::get_is_inner_type(itemType))
    {
        isSimpleType = true;
    }
    else
    {
        auto pti = parameters::helper::type::get_type_info(unitParameters_.fileInfo, itemType);
        if (pti != nullptr)
        {
            const auto typeCategory = parameters::helper::type::get_category(*pti);
            if (typeCategory == parameters::type_category::user_cpp)
            {
                isSimpleType = true;
                isEnumType = true;
            }
            else if (typeCategory == parameters::type_category::user_yml)
            {
                isYmlType = true;
            }
        }
    }

    // Заполняем модель
    if (isSimpleType || isEnumType)
    {
        for (int i = model.parameters.size(); i < itemsCount; ++i)
        {
            CubesUnitTypes::ParameterModel itemModel;
            itemModel.id = QString("%1/%2_%3").arg(model.id, itemGroupName).arg(i);
            itemModel.name = QString::fromLocal8Bit("Элемент %1").arg(i);
            itemModel.parameterInfoId = model.parameterInfoId;
            // Значение из xml заполняется в FillParameterModel, при наличии
            FillParameterModel(xmlUnit, itemModel, true);
            model.parameters.push_back(itemModel);
        }
    }
    else if (isYmlType)
    {
        for (int i = model.parameters.size(); i < itemsCount; ++i)
        {
            CubesUnitTypes::ParameterModel group_model;
            group_model.id = QString("%1/%2_%3").arg(model.id, itemGroupName).arg(i);
            group_model.name = QString::fromLocal8Bit("Элемент %1").arg(i);
            group_model.value = QVariant();
            //group_model.valueType = "none";
            group_model.editorSettings.type = CubesUnitTypes::EditorType::None;

            // Получаем значение из xml файла
            CubesXml::Item* xmlItem = nullptr;
            QString xmlItemType;
            if (xmlUnit != nullptr)
                xmlItem = CubesXml::Parser::GetItem(*const_cast<CubesXml::Unit*>(xmlUnit), group_model.id, xmlItemType);

            //// Вычисляем значение из xml файла (параметра или элемента массива)
            //QString xmlValueString;
            //QString xmlValueTypeString;
            //bool haveXmlValue = false;
            //if (xmlItem != nullptr)
            //{
            //    xmlValueString = xmlItem->val;
            //    xmlValueTypeString = xmlItemType;
            //    haveXmlValue = true;
            //}

            //// Конвертируем в QVariant
            //QVariant xmlValue;
            //if (haveXmlValue)
            //{
            //    const auto xmlBaseItemType = parameters::helper::common::get_xml_base_item_type(xmlValueTypeString.toStdString());
            //    switch (xmlBaseItemType)
            //    {
            //    case parameters::base_item_types::string:
            //        xmlValue = QString(xmlValueString);
            //        break;
            //    case parameters::base_item_types::integer:
            //        xmlValue = std::stoi(xmlValueString.toStdString());
            //        break;
            //    case parameters::base_item_types::floating:
            //        xmlValue = std::stod(xmlValueString.toStdString());
            //        break;
            //    case parameters::base_item_types::boolean:
            //        xmlValue = (xmlValueString == "true");
            //        break;
            //    case parameters::base_item_types::none:
            //    case parameters::base_item_types::user:
            //    default:
            //        break;
            //    }
            //}

            // Заполняем базовые параметры
            {
                CubesUnitTypes::ParameterModel base_group;
                base_group.id = group_model.id + "/" + baseGroupName;
                base_group.name = QString::fromLocal8Bit("Базовые");
                base_group.value = QVariant();
                //base_group.valueType = "none";
                //base_group.parameterInfoId = "";
                base_group.editorSettings.type = CubesUnitTypes::EditorType::None;

                CubesUnitTypes::ParameterModel instance_name;
                instance_name.id = group_model.id + "/" + baseGroupName + "/NAME";
                instance_name.name = QString::fromLocal8Bit("Имя");
                if (xmlItem == nullptr || xmlItem->name == "")
                    instance_name.value = QString(group_model.name);
                else
                    instance_name.value = QString(xmlItem->name);
                //instance_name.valueType = "string";
                //instance_name.parameterInfoId = "";
                instance_name.editorSettings.type = CubesUnitTypes::EditorType::String;
                //instance_name.editorSettings.is_expanded = true;

                base_group.parameters.push_back(std::move(instance_name));

                group_model.parameters.push_back(std::move(base_group));
            }

            // Получаем описание типа, проверили ранее, что такой тип существует
            const auto ti = *parameters::helper::type::get_type_info(unitParameters_.fileInfo, itemType);

            // Заполняем yml параметры
            if (!ti.parameters.empty())
            {
                CubesUnitTypes::ParameterModel properties_group;
                properties_group.id = group_model.id + "/" + parametersGroupName;
                properties_group.name = QString::fromLocal8Bit("Параметры");
                properties_group.value = QVariant();
                //properties_group.valueType = "none";
                //properties_group.parameterInfoId = "";
                properties_group.editorSettings.type = CubesUnitTypes::EditorType::None;
                properties_group.editorSettings.is_expanded = true;

                for (const auto& pi : ti.parameters)
                {
                    CubesUnitTypes::ParameterModel pm;
                    CreateParameterModel({ QString::fromStdString(ti.name), QString::fromStdString(pi.name) },
                        group_model.id + "/" + parametersGroupName, xmlUnit, pm);
                    properties_group.parameters.push_back(std::move(pm));
                }

                group_model.parameters.push_back(std::move(properties_group));
            }

            // Заполняем параметры редактора
            {
                CubesUnitTypes::ParameterModel editor_group;
                editor_group.id = group_model.id + "/" + editorGroupName;
                editor_group.name = QString::fromLocal8Bit("Редактор");
                editor_group.value = QVariant();
                //editor_group.valueType = "none";
                //editor_group.parameterInfoId = "";
                editor_group.editorSettings.type = CubesUnitTypes::EditorType::None;
                //editor_group.editorSettings.is_expanded = true;

                {
                    CubesUnitTypes::ParameterModel pm;
                    pm.id = group_model.id + "/" + editorGroupName + "/POSITION_X";
                    pm.name = QString::fromLocal8Bit("Позиция X");
                    pm.value = double{ xmlItem == nullptr ? 0.0 : xmlItem->x };
                    //pm.valueType = "double";
                    //pm.parameterInfoId = "";
                    pm.editorSettings.type = CubesUnitTypes::EditorType::SpinDouble;
                    pm.editorSettings.SpinDoubleMin = -10000;
                    pm.editorSettings.SpinDoubleMax = 10000;
                    pm.editorSettings.SpinDoubleSingleStep = 20;
                    editor_group.parameters.push_back(std::move(pm));
                }

                {
                    CubesUnitTypes::ParameterModel pm;
                    pm.id = group_model.id + "/" + editorGroupName + "/POSITION_Y";
                    pm.name = QString::fromLocal8Bit("Позиция Y");
                    pm.value = double{ xmlItem == nullptr ? 0.0 : xmlItem->y };
                    //pm.valueType = "double";
                    //pm.parameterInfoId = "";
                    pm.editorSettings.type = CubesUnitTypes::EditorType::SpinDouble;
                    pm.editorSettings.SpinDoubleMin = -10000;
                    pm.editorSettings.SpinDoubleMax = 10000;
                    pm.editorSettings.SpinDoubleSingleStep = 20;
                    editor_group.parameters.push_back(std::move(pm));
                }

                {
                    CubesUnitTypes::ParameterModel pm;
                    pm.id = group_model.id + "/" + editorGroupName + "/POSITION_Z";
                    pm.name = QString::fromLocal8Bit("Позиция Z");
                    pm.value = double{ xmlItem == nullptr ? 0.0 : xmlItem->z };
                    //pm.valueType = "double";
                    //pm.parameterInfoId = "";
                    pm.editorSettings.type = CubesUnitTypes::EditorType::SpinDouble;
                    pm.editorSettings.SpinDoubleMin = -10000;
                    pm.editorSettings.SpinDoubleMax = 10000;
                    editor_group.parameters.push_back(std::move(pm));
                }

                group_model.parameters.push_back(std::move(editor_group));
            }

            model.parameters.push_back(std::move(group_model));
        }
    }

    while (model.parameters.size() > itemsCount)
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

    if (pm->id.startsWith(baseGroupName))
    {
        if (pm->id == baseGroupName + "/NAME")
        {
            // Используется модифицированная версия редактора строк qtexpropertymanager
            // Обработка завершения редактирования строки происходит в StringEditingFinished
        }
        else if (pm->id == baseGroupName + "/FILE_NAME")
        {
            pm->value = property->valueText();

            QStringList includeNames;
            propertiesItemsManager_->AfterFileNameChanged(this, includeNames);

            SetIncludeNames(includeNames);
            SetIncludeName("<not selected>"); // TODO: убрать <not selected> в константы
        }
        else if (pm->id == baseGroupName + "/INCLUDE_NAME")
        {
            pm->value = property->valueText();

            propertiesItemsManager_->AfterIncludeNameChanged(this);
        }
    }
    else if (pm->id.startsWith(parametersGroupName))
    {
        QStringList path = pm->id.split("/");
        if (path.size() < 2)
            return;

        // У некоторых параметров есть дополнительные параметры DEPENDS у unit,
        // у всех остальных может быть параметр OPTIONAL

        // У элементов массива имеются группы параметров BASE, PARAMETERS, EDITOR
        // Реально же у юнита есть только параметры, находящиеся в группе PARAMETERS
        // Например: $PARAMETERS/CHANNELS/$ITEM_0/$BASE/NAME,
        // $PARAMETERS/CHANNELS/$ITEM_0/$PARAMETERS/FIRMWARE и т.п.
        // Поэтому дополнительно проверяем предпоследний пункт в id

        if (path.size() > 0 && (path[path.size() - 1] == dependsParameterName || path[path.size() - 1] == optionalParameterName))
        {
            // Получаем дополнительные свойства
            // У них нет описания в parameter_info, т.к. они добавлены нами для служебных целей
            bool b = false;
            auto boolString = property->valueText().toLower().toStdString();
            std::istringstream(boolString) >> std::boolalpha >> b;
            pm->value = b;
        }
        else if (path.size() > 2 && (path[path.size() - 2] == baseGroupName || path[path.size() - 2] == editorGroupName))
        {
            // Если это элемент массива типа yml, получаем дополнительные свойства
            // У них нет описания в parameter_info, т.к. они добавлены нами для служебных целей
            if (path[path.size() - 2] == baseGroupName)
            {
                pm->value = property->valueText();
            }
            else if (path[path.size() - 2] == editorGroupName)
            {
                pm->value = property->valueText().toDouble();
            }
        }
        else
        {
            // Получаем описание параметра из его yml файла
            auto& pi = *parameters::helper::parameter::get_parameter_info(unitParameters_.fileInfo,
                pm->parameterInfoId.type.toStdString(), pm->parameterInfoId.name.toStdString());

            bool isArray = parameters::helper::common::get_is_array_type(pi.type);
            auto itemType = parameters::helper::common::get_item_type(pi.type);

            // isArray нужен для определения, что именно поменялось - количество элементов массива или
            // значение параметра. В модели параметра есть привязка к описанию параметра - parameterInfoId.
            // В массивах типа yml items не хранят значение, а параметры имеют свое описание и привязку к типу.
            // Проблема с типизированными массивами. В них каждый item хранит значение и привязку к
            // базовому типу массива, т.е. при получении parameter_info получим, что каждый item это массив,
            // что на самом деле не так. Поэтому, дополнительно проверяем такую ситуацию.
            // P.S. Тип нужен для корректной обработки элементов массива
            // TODO: Возможно надо добавить в модель флаг на этот случай

            // При редактировании элемента типизированного массива (например, типа array<int>)
            // pm->id = $PARAMETERS/CHANNELS/$ITEM_0/$PARAMETERS/BLOCKS/$ITEM_0
            if (path.size() > 2 && path.back().startsWith(itemGroupName))
                isArray = false;

            if (isArray)
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
                auto baseItemType = parameters::helper::common::get_base_item_type(pi.type);

                switch (baseItemType)
                {
                case parameters::base_item_types::string:
                    pm->value = property->valueText();
                    break;
                case parameters::base_item_types::integer:
                    pm->value = std::stoi(property->valueText().toStdString());
                    break;
                case parameters::base_item_types::floating:
                    pm->value = std::stod(property->valueText().toStdString());
                    break;
                case parameters::base_item_types::boolean:
                {
                    bool b = false;
                    auto boolString = property->valueText().toLower().toStdString();
                    std::istringstream(boolString) >> std::boolalpha >> b;
                    pm->value = b;
                    break;
                }
                case parameters::base_item_types::user:
                case parameters::base_item_types::none:
                default:
                    pm->value = property->valueText();
                    break;
                }
            }
        }
    }
    else if (pm->id.startsWith(editorGroupName))
    {
        if (pm->id == editorGroupName + "/POSITION_X")
        {
            pm->value = property->valueText().toDouble();
            double posX = property->valueText().toDouble();

            auto pmY = GetParameterModel(editorGroupName + "/POSITION_Y");
            double posY = pmY->value.toDouble();

            auto pmZ = GetParameterModel(editorGroupName + "/POSITION_Z");
            double posZ = pmZ->value.toDouble();

            propertiesItemsManager_->AfterPositionChanged(this, posX, posY, posZ);
        }
        else if (pm->id == editorGroupName + "/POSITION_Y")
        {
            auto pmX = GetParameterModel(editorGroupName + "/POSITION_X");
            double posX = pmX->value.toDouble();

            pm->value = property->valueText().toDouble();
            double posY = property->valueText().toDouble();

            auto pmZ = GetParameterModel(editorGroupName + "/POSITION_Z");
            double posZ = pmZ->value.toDouble();

            propertiesItemsManager_->AfterPositionChanged(this, posX, posY, posZ);
        }
        else if (pm->id == editorGroupName + "/POSITION_Z")
        {
            auto pmX = GetParameterModel(editorGroupName + "/POSITION_X");
            double posX = pmX->value.toDouble();

            auto pmY = GetParameterModel(editorGroupName + "/POSITION_Y");
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

    if (pm->id == baseGroupName + "/NAME")
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
}

QString PropertiesItem::GetPropertyId(const QtProperty* property)
{
    auto it = propertyToId_.find(property);
    if (it != propertyToId_.end())
        return *it;
    return QString();
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

    return pm;
}

CubesUnitTypes::ParameterModel* PropertiesItem::GetParameterModel(const QtProperty* property)
{
    QString id = GetPropertyId(property);
    if (id == "")
        return nullptr;

    return GetParameterModel(id);
}

CubesUnitTypes::UnitParameters* GetPropertyUnitParameters(const QtProperty* property)
{
    return nullptr;
}

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
