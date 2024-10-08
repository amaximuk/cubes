#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "parameters.h"
#include "../windows/main_window.h"
#include "../diagram/diagram_scene.h"
#include "../diagram/diagram_item.h"
#include "../parameters/variant_converter.h"
#include "../parameters/base64.h"
#include "../xml/xml_helper.h"
#include "../log/log_helper.h"
#include "../unit/unit_helper.h"
#include "properties_item_types.h"
#include "properties_item.h"

using namespace CubesProperties;

PropertiesItem::PropertiesItem(IPropertiesItemsManager* propertiesItemsManager, CubesLog::ILogManager* logManager, PropertiesEditor* editor,
    CubesUnit::UnitParametersPtr unitParametersPtr, bool isArrayUnit, CubesUnit::PropertiesId propertiesId)
{
    propertiesItemsManager_ = propertiesItemsManager;
    logManager_ = logManager;
    editor_ = editor;
    unitParametersPtr_ = unitParametersPtr;
    propertiesId_ = propertiesId;
    parameterModelPtrs_ = {};
    ignoreEvents_ = false;

    logHelper_.reset(new CubesLog::LogHelper(logManager_, CubesLog::SourceType::propertiesItem,
        GetPropertiesItemErrorDescriptions()));

    CreateParametersModel(nullptr, isArrayUnit);
    CreateProperties();
}

PropertiesItem::PropertiesItem(IPropertiesItemsManager* propertiesItemsManager, CubesLog::ILogManager* logManager, PropertiesEditor* editor,
    CubesUnit::UnitParametersPtr unitParametersPtr, CubesUnit::PropertiesId propertiesId, CubesUnit::ParameterModelPtrs parameterModelPtrs)
{
    propertiesItemsManager_ = propertiesItemsManager;
    logManager_ = logManager;
    editor_ = editor;
    unitParametersPtr_ = unitParametersPtr;
    propertiesId_ = propertiesId;
    parameterModelPtrs_ = {};
    ignoreEvents_ = false;

    logHelper_.reset(new CubesLog::LogHelper(logManager_, CubesLog::SourceType::propertiesItem,
        GetPropertiesItemErrorDescriptions()));

    parameterModelPtrs_ = parameterModelPtrs;
    CreateProperties();
}

PropertiesItem::PropertiesItem(IPropertiesItemsManager* propertiesItemsManager, CubesLog::ILogManager* logManager, PropertiesEditor* editor,
    CubesUnit::UnitParametersPtr unitParametersPtr, const CubesXml::Unit& xmlUnit, bool isArrayUnit, CubesUnit::PropertiesId propertiesId)
{
    propertiesItemsManager_ = propertiesItemsManager;
    logManager_ = logManager;
    editor_ = editor;
    unitParametersPtr_ = unitParametersPtr;
    propertiesId_ = propertiesId;
    parameterModelPtrs_ = {};
    ignoreEvents_ = false;

    logHelper_.reset(new CubesLog::LogHelper(logManager_, CubesLog::SourceType::propertiesItem,
        GetPropertiesItemErrorDescriptions()));

    CreateParametersModel(&xmlUnit, isArrayUnit);
    CreateProperties();
}

uint32_t PropertiesItem::GetPropertiesId() const
{
    return propertiesId_;
};

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
        auto pm = GetParameterModelPtr(property);
        if (pm != nullptr)
            pm->editorSettings.isExpanded = is_expanded;
    }
}

void PropertiesItem::CreateParametersModel(const CubesXml::Unit* xmlUnit, bool isArrayUnit)
{
    // BASE
    // BASE/NAME
    // BASE/UNIT_ID
    // BASE/FILE
    // BASE/INCLUDE_NAME
    // PARAMETERS
    // PARAMETERS/...
    // EDITOR
    // EDITOR/POSITION_X
    // EDITOR/POSITION_Y
    // EDITOR/POSITION_Z

    {
        CubesUnit::ParameterModelPtr base_group = CubesUnit::CreateParameterModelPtr();
        base_group->id = ids_.base;
        base_group->name = QString::fromLocal8Bit("�������");
        base_group->value = QVariant();
        base_group->editorSettings.type = CubesUnit::EditorType::None;
        base_group->editorSettings.isExpanded = true;

        if (!isArrayUnit)
        {
            CubesUnit::ParameterModelPtr unit_id = CubesUnit::CreateParameterModelPtr();
            unit_id->id = ids_.base + ids_.unitId;
            unit_id->name = QString::fromLocal8Bit("ID �����");
            unit_id->value = QString::fromStdString(unitParametersPtr_->fileInfo.info.id);
            unit_id->editorSettings.type = CubesUnit::EditorType::String;
            unit_id->editorSettings.isExpanded = false;
            unit_id->readOnly = true;
            base_group->parameters.push_back(std::move(unit_id));
        }

        CubesUnit::ParameterModelPtr instance_name = CubesUnit::CreateParameterModelPtr();
        instance_name->id = ids_.base + ids_.name;
        instance_name->name = QString::fromLocal8Bit("���");
        if (xmlUnit == nullptr)
            instance_name->value = QString::fromStdString(parameters::helper::file::get_display_name(unitParametersPtr_->fileInfo));
        else
            instance_name->value = QString(xmlUnit->name);
        instance_name->editorSettings.type = CubesUnit::EditorType::String;
        instance_name->editorSettings.isExpanded = false;
        base_group->parameters.push_back(std::move(instance_name));

        if (!isArrayUnit)
        {
            CubesUnit::ParameterModelPtr file = CubesUnit::CreateParameterModelPtr();
            file->id = ids_.base + ids_.fileName;
            file->name = QString::fromLocal8Bit("����");
            file->key = CubesUnit::InvalidFileId;
            file->value = QString();
            file->editorSettings.type = CubesUnit::EditorType::ComboBox;
            file->editorSettings.isExpanded = false;
            base_group->parameters.push_back(std::move(file));

            CubesUnit::ParameterModelPtr group = CubesUnit::CreateParameterModelPtr();
            group->id = ids_.base + ids_.includeName;
            group->name = QString::fromLocal8Bit("���������� ����");
            group->key = CubesUnit::InvalidIncludeId;
            group->value = QString();
            group->editorSettings.type = CubesUnit::EditorType::ComboBox;
            group->editorSettings.isExpanded = false;
            base_group->parameters.push_back(std::move(group));
        }

        parameterModelPtrs_.push_back(std::move(base_group));
    }

    if (unitParametersPtr_->fileInfo.parameters.size() > 0)
    {
        CubesUnit::ParameterModelPtr properties_group = CubesUnit::CreateParameterModelPtr();
        properties_group->id = ids_.parameters;
        properties_group->name = QString::fromLocal8Bit("���������");
        properties_group->value = QVariant();
        properties_group->editorSettings.type = CubesUnit::EditorType::None;
        properties_group->editorSettings.isExpanded = true;

        CheckParametersMatching(xmlUnit, QString::fromStdString(parameters::helper::type::main_type), ids_.parameters);

        for (const auto& pi : unitParametersPtr_->fileInfo.parameters)
        {
            CubesUnit::ParameterModelPtr pm;
            CreateParameterModel({ QString::fromStdString(parameters::helper::type::main_type), QString::fromStdString(pi.name) },
                ids_.parameters, xmlUnit, pm);
            properties_group->parameters.push_back(std::move(pm));
        }

        parameterModelPtrs_.push_back(std::move(properties_group));
    }

    {
        CubesUnit::ParameterModelPtr editor_group = CubesUnit::CreateParameterModelPtr();
        editor_group->id = ids_.editor;
        editor_group->name = QString::fromLocal8Bit("��������");
        editor_group->value = QVariant();
        editor_group->editorSettings.type = CubesUnit::EditorType::None;
        editor_group->editorSettings.isExpanded = true;

        {
            CubesUnit::ParameterModelPtr pm = CubesUnit::CreateParameterModelPtr();
            pm->id = ids_.editor + ids_.positionX;
            pm->name = QString::fromLocal8Bit("������� X");
            pm->value = double{ xmlUnit == nullptr ? 0.0 : xmlUnit->x };
            pm->editorSettings.type = CubesUnit::EditorType::SpinDouble;
            pm->editorSettings.spinDoubleMin = -10000;
            pm->editorSettings.spinDoubleMax = 10000;
            pm->editorSettings.spinDoubleSingleStep = CubesDiagram::GridSize;
            editor_group->parameters.push_back(std::move(pm));
        }

        {
            CubesUnit::ParameterModelPtr pm = CubesUnit::CreateParameterModelPtr();
            pm->id = ids_.editor + ids_.positionY;
            pm->name = QString::fromLocal8Bit("������� Y");
            pm->value = double{ xmlUnit == nullptr ? 0.0 : xmlUnit->y };
            pm->editorSettings.type = CubesUnit::EditorType::SpinDouble;
            pm->editorSettings.spinDoubleMin = -10000;
            pm->editorSettings.spinDoubleMax = 10000;
            pm->editorSettings.spinDoubleSingleStep = CubesDiagram::GridSize;
            editor_group->parameters.push_back(std::move(pm));
        }

        {
            CubesUnit::ParameterModelPtr pm = CubesUnit::CreateParameterModelPtr();
            pm->id = ids_.editor + ids_.positionZ;
            pm->name = QString::fromLocal8Bit("������� Z");
            pm->value = double{ xmlUnit == nullptr ? 0.0 : xmlUnit->z };
            pm->editorSettings.type = CubesUnit::EditorType::SpinDouble;
            pm->editorSettings.spinDoubleMin = -10000;
            pm->editorSettings.spinDoubleMax = 10000;
            editor_group->parameters.push_back(std::move(pm));
        }

        parameterModelPtrs_.push_back(std::move(editor_group));
    }
}

void PropertiesItem::CreateProperties()
{
    if (editor_ != nullptr)
    {
        QMap<CubesUnit::ParameterModelId, const QtProperty*> idToProperty;
        for (auto& pm : parameterModelPtrs_)
            topLevelProperties_.push_back(editor_->CreatePropertyForModel(pm, idToProperty));
        for (const auto& kvp : idToProperty.toStdMap())
            RegisterProperty(kvp.second, kvp.first);
    }
}

void PropertiesItem::CreateParameterModel(const CubesUnit::ParameterInfoId& parameterInfoId,
    const CubesUnit::ParameterModelId& parentModelId, const CubesXml::Unit* xmlUnit,
    CubesUnit::ParameterModelPtr& model)
{
    // �������� ������ ��� ��������� �� ��� info ID (��� � ��� �� yml �����)
    // ������ �������� ��� ��������� ��������� � �������
    // ������� ��������� ����������� model ID (���� � ��������� � ������, ����������� /)

    auto& pi = *parameters::helper::parameter::get_parameter_info(unitParametersPtr_->fileInfo,
        parameterInfoId.type.toStdString(), parameterInfoId.name.toStdString());

    CubesUnit::ParameterModelPtr pm = CubesUnit::CreateParameterModelPtr();
    pm->id = parentModelId + QString::fromStdString(pi.name);
    pm->name = QString::fromStdString(parameters::helper::parameter::get_display_name(pi));
    pm->parameterInfoId = parameterInfoId;

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

void PropertiesItem::FillParameterModel(const CubesXml::Unit* xmlUnit, CubesUnit::ParameterModelPtr model, bool isItem)
{
    // ���������� ������ ���������, �� ����������� ��������, � ������ �����������
    // ���� id, name, parameterInfoId ������ ���� �������������� ���������
    // parameterInfoId ����� ���� �� �������� ��� �������������� ����� (BASE, EDITOR � �.�.)
    // ���� xmlUnit != nullptr, ������ ������� ���� �� ����� xml

    CubesXml::Element element{};
    if (xmlUnit != nullptr && !CubesXml::Helper::GetElement(*const_cast<CubesXml::Unit*>(xmlUnit), model->id, element))
        assert(false);

    // ��������� �������� �� xml ����� (��������� ��� �������� �������)
    QString xmlValueString;
    QString xmlValueTypeString;
    bool haveXmlValue = false;
    if (element.type == CubesXml::ElementType::Param)
    {
        xmlValueString = element.param->val;
        xmlValueTypeString = element.param->type;
        haveXmlValue = true;
    }
    else if (element.type == CubesXml::ElementType::Array)
    {
        // �������� ���� �� ������
        assert(false);
    }
    else if (element.type == CubesXml::ElementType::Item)
    {
        xmlValueString = element.item->val;
        xmlValueTypeString = element.arrayType;
        haveXmlValue = true;
    }
    else if (element.type == CubesXml::ElementType::None)
    {
        // ��� ���������, ���� �������� �� ����� � xml �����
    }
    else
    {
        assert(false);
    }


    // �������� �������� ��������� �� ��� yml �����
    auto& pi = *parameters::helper::parameter::get_parameter_info(unitParametersPtr_->fileInfo,
        model->parameterInfoId.type.toStdString(), model->parameterInfoId.name.toStdString());
    auto v = parameters::helper::parameter::get_initial_value(unitParametersPtr_->fileInfo, pi, isItem);
    const bool res = CubesParameters::convert_variant(v, model->value);

    // �������� �� ������ ���� ��������, ����� ���� ��������
    // �������� � ��������������� ���������. � ��� ������ item ������ �������� � �������� �
    // �������� ���� �������, �.�. ��� ��������� parameter_info �������, ��� ������ item ��� ������,
    // ��� �� ����� ���� �� ���
    // �� ������ ������ ������� ��� ��������
    const auto itemType = parameters::helper::common::get_item_type(pi.type);

    const auto isUnitType = parameters::helper::common::get_is_unit_type(itemType);
    const auto baseItemType = parameters::helper::common::get_base_item_type(pi.type);

    if (element.type == CubesXml::ElementType::Param)
    {
        auto typeName = parameters::helper::parameter::get_type_xml(unitParametersPtr_->fileInfo, pi);

        // ��������� ������������� ����� ���������� �� xml � �� �������� yml
        if (typeName != element.param->type.toStdString())
        {
            logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>(PropertiesItemErrorCode::typeMismatch),
                { {QString::fromLocal8Bit("��� ��������� xml"), element.param->name},
                {QString::fromLocal8Bit("��� ��������� xml"), element.param->type},
                {QString::fromLocal8Bit("��� �����"), GetName()},
                {QString::fromLocal8Bit("��� �����"), QString::fromStdString(unitParametersPtr_->fileInfo.info.id)},
                {QString::fromLocal8Bit("��� ��������� �����"), QString::fromStdString(pi.type)} }, propertiesId_);
        }
    }

    // ������������ � QVariant
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
        {
            bool flag{ false };
            xmlValue = xmlValueString.toInt(&flag);
            if (!flag)
            {
                logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>(PropertiesItemErrorCode::typeMismatch),
                    { {QString::fromLocal8Bit("��� ��������� xml"), element.param->name},
                    {QString::fromLocal8Bit("��� ��������� xml"), element.param->type},
                    {QString::fromLocal8Bit("��� �����"), GetName()},
                    {QString::fromLocal8Bit("��� �����"), QString::fromStdString(unitParametersPtr_->fileInfo.info.id)},
                    {QString::fromLocal8Bit("��������"), xmlValueString} }, propertiesId_);

                xmlValue = int{ 0 };
            }
            break;
        }
        case parameters::base_item_types::floating:
        {
            bool flag{ false };
            xmlValue = xmlValueString.toDouble(&flag);
            if (!flag)
            {
                logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>(PropertiesItemErrorCode::typeMismatch),
                    { {QString::fromLocal8Bit("��� ��������� xml"), element.param->name},
                    {QString::fromLocal8Bit("��� ��������� xml"), element.param->type},
                    {QString::fromLocal8Bit("��� �����"), GetName()},
                    {QString::fromLocal8Bit("��� �����"), QString::fromStdString(unitParametersPtr_->fileInfo.info.id)},
                    {QString::fromLocal8Bit("��������"), xmlValueString} }, propertiesId_);

                xmlValue = double{ 0.0 };
            }
            break;
        }
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


    if (pi.restrictions.set_.size() > 0)
    {
        // ����� ������ ���������� ��������
        model->editorSettings.type = CubesUnit::EditorType::ComboBox;

        // ��������� ���������� �������� �� ����������� � yml �����
        for (int i = 0; i < pi.restrictions.set_.size(); ++i)
        {
            model->editorSettings.comboBoxValues.push_back({ static_cast<CubesUnit::BaseId>(i),
                QString::fromStdString(pi.restrictions.set_[i]) });
        }

        // ��������� ������������ �������� �� xml ����� (��������� ��� �������� �������)
        if (haveXmlValue)
        {
            // ��������� ����������� �� ������ ���������
            if (!model->HaveComboBoxValue(xmlValue.toString()))
            {
                logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>(PropertiesItemErrorCode::restrictionFailed),
                    { {QString::fromLocal8Bit("��� ��������� xml"), element.param->name},
                    {QString::fromLocal8Bit("��� ��������� xml"), element.param->type},
                    {QString::fromLocal8Bit("��� �����"), GetName()},
                    {QString::fromLocal8Bit("��� �����"), QString::fromStdString(unitParametersPtr_->fileInfo.info.id)},
                    {QString::fromLocal8Bit("��������"), xmlValueString} }, propertiesId_);
            }
        }
    }
    else
    {
        // ����������� �� ������ �������� �� ������, ��������� ������ � ����������� �� �������� ����
        if (isUnitType)
        {
            // ��� ������ ������ - ��� unit, ��� ������� ��� ������
            model->editorSettings.type = CubesUnit::EditorType::String;

            {
                // ��� ���� unit ����������� �������������� ���� - ����������� (depends)
                CubesUnit::ParameterModelPtr pm_depends = CubesUnit::CreateParameterModelPtr();
                pm_depends->id = model->id + ids_.depends;
                pm_depends->name = QString::fromLocal8Bit("�����������");
                // ���� ���� �������� � xml, ��������� ��� � ������ �����������
                if (element.type == CubesXml::ElementType::Param)
                    pm_depends->value = bool{ element.param->depends };
                else
                    pm_depends->value = bool{ false };
                pm_depends->editorSettings.type = CubesUnit::EditorType::CheckBox;

                model->parameters.push_back(std::move(pm_depends));
            }
        }
        else if (baseItemType == parameters::base_item_types::string)
        {
            // ��������� ����, ���������� �� ������� (path, string � �.�.)
            model->editorSettings.type = CubesUnit::EditorType::String;
        }
        else if (baseItemType == parameters::base_item_types::boolean)
        {
            // ��� bool
            model->editorSettings.type = CubesUnit::EditorType::CheckBox;
        }
        else if (baseItemType == parameters::base_item_types::integer)
        {
            // ����, ���������� �� ������������� ��������� int (int8_t, uint16_t � �.�.)
            model->editorSettings.type = CubesUnit::EditorType::SpinInterger;

            // ������������� ����������� �� yml ����� �����
            if (pi.restrictions.min != "")
                model->editorSettings.spinIntergerMin = std::stoi(pi.restrictions.min);
            else
                model->editorSettings.spinIntergerMin = parameters::helper::common::get_min_for_integral_type(itemType);
            if (pi.restrictions.max != "")
                model->editorSettings.spinIntergerMax = std::stoi(pi.restrictions.max);
            else
                model->editorSettings.spinIntergerMax = parameters::helper::common::get_max_for_integral_type(itemType);
        }
        else if (baseItemType == parameters::base_item_types::floating)
        {
            // ����, ���������� �� ��������� � ��������� ������ double (float, double)
            model->editorSettings.type = CubesUnit::EditorType::SpinDouble;

            // ������������� ����������� �� yml ����� �����
            if (pi.restrictions.min != "")
                model->editorSettings.spinDoubleMin = std::stod(pi.restrictions.min);
            else
                model->editorSettings.spinDoubleMin = parameters::helper::common::get_min_for_floating_point_type(itemType);
            if (pi.restrictions.max != "")
                model->editorSettings.spinDoubleMax = std::stod(pi.restrictions.max);
            else
                model->editorSettings.spinDoubleMax = parameters::helper::common::get_max_for_floating_point_type(itemType);

            if (model->id == ids_.parameters + ids_.width || model->id == ids_.parameters + ids_.height)
                model->editorSettings.spinDoubleSingleStep = CubesDiagram::GridSize;
        }
        else if (baseItemType == parameters::base_item_types::user)
        {
            // ���������������� ��� ������
            // ��������� �������� �� �������� ��������, ������������ ���������� ��� ��� enum
            const auto pti = parameters::helper::type::get_type_info(unitParametersPtr_->fileInfo, itemType);
            const auto typeCategory = parameters::helper::type::get_category(*pti);
            if (typeCategory != parameters::type_category::user_cpp)
                assert(false);

            model->editorSettings.type = CubesUnit::EditorType::ComboBox;

            // ��������� ���������� �������� ������������ �� �������� ����� � yml ����� � ComboBox
            // TODO: ����� ���� ��� ����������� �� ������ ��������, ��� �� ������
            if (pti->values.size() > 0)
            {
                for (int i = 0; i < pti->values.size(); ++i)
                {
                    model->editorSettings.comboBoxValues.push_back({ static_cast<CubesUnit::BaseId>(i),
                        QString::fromStdString(pti->values[i].first) });
                }
            }

            // ��������� ������������ �������� �� xml �����
            if (haveXmlValue)
            {
                if (!model->HaveComboBoxValue(xmlValue.toString()))
                {
                    logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>(PropertiesItemErrorCode::restrictionFailed),
                        { {QString::fromLocal8Bit("��� ��������� xml"), element.param->name},
                        {QString::fromLocal8Bit("��� ��������� xml"), element.param->type},
                        {QString::fromLocal8Bit("��� �����"), GetName()},
                        {QString::fromLocal8Bit("��� �����"), QString::fromStdString(unitParametersPtr_->fileInfo.info.id)},
                        {QString::fromLocal8Bit("��������"), xmlValueString} }, propertiesId_);
                }
            }
        }
        else
        {
            // ��� ����������
            assert(false);
        }
    }

    // ��������� �������� �� xml � ������ (���� �� ������, ������� �������� ��-��������� ��� ����)
    if (haveXmlValue)
    {
        model->value = xmlValue; // it's QVariant

        if (!model->editorSettings.comboBoxValues.empty())
        {
            // enum
            int key = 0;
            for (int i = 0; i < model->editorSettings.comboBoxValues.size(); ++i)
            {
                if (model->editorSettings.comboBoxValues[i].value == xmlValue.toString())
                {
                    key = model->editorSettings.comboBoxValues[i].id;
                    break;
                }
            }
            model->key = key;
        }
    }

    // ��� ������������ ���������� ��������� �������������� ���� - �� ��������
    if (parameters::helper::parameter::get_is_optional(pi))
    {
        CubesUnit::ParameterModelPtr pmo = CubesUnit::CreateParameterModelPtr();
        pmo->id = model->id + ids_.optional;
        pmo->name = QString::fromLocal8Bit("�� ��������");
        pmo->value = bool{ false };
        pmo->editorSettings.type = CubesUnit::EditorType::CheckBox;

        // ���� xml ���� ����, ������������� �������� �����
        if (xmlUnit != nullptr)
        {
            if (element.type == CubesXml::ElementType::Param)
            {
                // �������� ����, ���������� ���� �� ��������
                pmo->value = bool{ false };
            }
            else
            {
                // �������� �����������, ������ ���� �� ��������
                // TODO: ���� ���-�� �������� �� �������� ���������,
                // ������� - model->value = QString::fromLocal8Bit("�� ������"); - ������, �� ��������� ��� ������
                pmo->value = bool{ true };
            }
        }

        model->parameters.push_back(std::move(pmo));
    }
}

void PropertiesItem::SetFileIdNames(CubesUnit::FileIdNames fileNames)
{
    const auto pm = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.base + ids_.fileName);
    if (pm != nullptr)
    {
        pm->SetComboBoxFileNames(fileNames);

        // ��������� �������, ����� �� ����������� ��������� ����������� ����, �.�. �� �������
        // ��������� ����� ������������� ��������������� �������� <�� �����������>,
        // ���� ���� ���������� ���� �������� ��� ��
        if (editor_ != nullptr)
        {
            editor_->blockSignals(true);
            editor_->SetEnumValues(GetProperty(pm->id), fileNames.values());
        }

        // ���� item ��� ��������, ����� ��� �� ������ �����, pm->key ����� �� �����
        // ������� ������� �������
        if (pm->key == CubesUnit::InvalidFileId && !fileNames.empty())
        {
            pm->key = fileNames.keys()[0];
            pm->value = fileNames.values()[0];
        }

        if (editor_ != nullptr)
        {
            // ��� ���������� ������ ���������� ������������ ��������� ��������
            if (fileNames.keys().contains(pm->key))
                editor_->SetEnumValue(GetProperty(pm->id), pm->GetComboBoxIndex());

            // ������������� ������ ���, ���� ������� ����, ���� �� ���������������
            editor_->blockSignals(false);
        }
    }
}

void PropertiesItem::SetFileIdName(CubesUnit::FileId fileId, QString fileName)
{
    const auto pm = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.base + ids_.fileName);
    if (pm != nullptr)
    {
        pm->key = fileId;
        pm->value = QString(fileName);
        if (editor_ != nullptr)
            editor_->SetEnumValue(GetProperty(pm->id), pm->GetComboBoxIndex());
    }
}

void PropertiesItem::SetIncludeIdNames(CubesUnit::IncludeIdNames includeNames)
{
    auto oldIncludeId = GetIncludeId();
    if (!includeNames.keys().contains(oldIncludeId))
        oldIncludeId = CubesUnit::InvalidIncludeId; // <not selected>
    const auto pm = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.base + ids_.includeName);
    if (pm != nullptr)
    {
        pm->SetComboBoxIncludeNames(includeNames);
        if (editor_ != nullptr)
            editor_->SetEnumValues(GetProperty(pm->id), includeNames.values());
    }
    const auto& includeName = includeNames[oldIncludeId];
    SetIncludeIdName(oldIncludeId, includeName);
}

void PropertiesItem::SetIncludeIdName(CubesUnit::IncludeId includeId, QString includeName)
{
    const auto pm = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.base + ids_.includeName);
    if (pm != nullptr)
    {
        pm->key = includeId;
        pm->value = QString(includeName);
        if (editor_ != nullptr)
            editor_->SetEnumValue(GetProperty(pm->id), pm->GetComboBoxIndex());
        propertiesItemsManager_->AfterIncludeNameChanged(propertiesId_);
    }
}

QString PropertiesItem::GetFileName()
{
    const auto pm = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.base + ids_.fileName);
    if (pm != nullptr)
        return pm->value.toString();
    else
        return "";
}

CubesUnit::FileId PropertiesItem::GetFileId()
{
    const auto pm = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.base + ids_.fileName);
    if (pm != nullptr)
        return pm->key;
    return 0;
}

CubesUnit::IncludeId PropertiesItem::GetIncludeId()
{
    const auto pm = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.base + ids_.includeName);
    if (pm != nullptr)
        return pm->key;
    
    return CubesUnit::InvalidIncludeId;
}

QString PropertiesItem::GetIncludeName()
{
    const auto pm = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.base + ids_.includeName);
    if (pm != nullptr)
        return pm->value.toString();
    else
        return "";
}

void PropertiesItem::SetName(QString name)
{
    const auto pm = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.base + ids_.name);
    if (pm != nullptr)
    {
        QString oldName = pm->value.toString();
        pm->value = name;

        if (editor_ != nullptr)
        {
            auto pr = GetProperty(pm->id);
            editor_->SetStringValue(pr, name);
        }
    }
}

QString PropertiesItem::GetName()
{
    const auto pm = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.base + ids_.name);
    if (pm != nullptr)
        return pm->value.toString();
    else
        return "";
}

QPointF PropertiesItem::GetPosition()
{
    const auto pmX = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.editor + ids_.positionX);
    const auto pmY = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.editor + ids_.positionY);
    if (pmX != nullptr && pmY != nullptr)
        return { pmX->value.toDouble(), pmY->value.toDouble() };
    else
        return { 0, 0 };
}

bool PropertiesItem::GetXmlParam(CubesUnit::ParameterModelPtr pm, CubesXml::Param& param)
{
    // �������� �������� � ������� xml ����� (CubesXml::Param) �� ������ �� ������
    // ���� �������� ������� ��� �� ���������� (OPTIONAL), ���������� false
    // ������ ������ ��������� �������������� ���������, �������� ��� ������ pm:
    //     PARAMETERS/CHANNELS/ITEM_0/PARAMETERS/COMMUTATOR_NAME
    // pm->parameters ����� ���������:
    //     PARAMETERS/CHANNELS/ITEM_0/PARAMETERS/COMMUTATOR_NAME/DEPENDS
    //     PARAMETERS/CHANNELS/ITEM_0/PARAMETERS/COMMUTATOR_NAME/OPTIONAL
    // ������� � ������ ������� �� ��������������, ��� ��������� ������� ������������ GetXmlArrray

    auto& pi = *parameters::helper::parameter::get_parameter_info(unitParametersPtr_->fileInfo,
        pm->parameterInfoId.type.toStdString(), pm->parameterInfoId.name.toStdString());

    auto typeName = parameters::helper::parameter::get_type_xml(unitParametersPtr_->fileInfo, pi);

    bool depends{false};
    bool notSet{false};
    for (auto& pmParameter : pm->parameters)
    {
        if (pmParameter->id == pm->id + ids_.depends)
            depends = pmParameter->value.toBool();
        else if (pmParameter->id == pm->id + ids_.optional)
            notSet = pmParameter->value.toBool();
    }

    if (notSet)
        return false;

    param.name = QString::fromStdString(pi.name);
    param.type = QString::fromStdString(typeName);
    param.val = pm->value.toString();
    param.depends = depends;

    return true;
}

bool PropertiesItem::GetXmlArrray(CubesUnit::ParameterModelPtr pm, CubesXml::Array& array)
{
    // �������� ������ � ������� xml ����� (CubesXml::Array) �� ������ �� ������
    // �� ����� pm - ��� ������ ��������� �������, ��������
    //     PARAMETERS/CHANNELS

    if (pm->parameters.size() == 0)
        return false;

    auto& pi = *parameters::helper::parameter::get_parameter_info(unitParametersPtr_->fileInfo,
        pm->parameterInfoId.type.toStdString(), pm->parameterInfoId.name.toStdString());

    array.name = QString::fromStdString(pi.name);

    bool is_inner_type = parameters::helper::common::get_is_inner_type(pi.type);
    if (is_inner_type)
    {
        // ��� ���������� ����� �������� � �������� � ����� item:
        //     PARAMETERS/CHANNELS/ITEM_0
        //     PARAMETERS/CHANNELS/ITEM_1

        auto itemTypeXmlName = parameters::helper::parameter::get_item_type_xml(unitParametersPtr_->fileInfo, pi);

        array.type = QString::fromStdString(itemTypeXmlName);

        // TODO: depends pm->valueType = int????????
        // depends - no type (have int)

        for (const auto& pmItem : pm->parameters)
        {
            CubesXml::Item item{};
            item.val = pmItem->value.toString();
            array.items.push_back(item);
        }
    }
    else
    {
        // ��� ����� yml �������� � �������� ��������� item:
        //     PARAMETERS/CHANNELS/ITEM_0/BASE/NAME
        //     PARAMETERS/CHANNELS/ITEM_0/PARAMETERS/COMMUTATOR_NAME
        //     ...
        //     PARAMETERS/CHANNELS/ITEM_0/EDITOR/POSITION_X
        //     PARAMETERS/CHANNELS/ITEM_0/EDITOR/POSITION_Y
        //     PARAMETERS/CHANNELS/ITEM_0/EDITOR/POSITION_Z

        array.type = "";

        for (const auto& pmItem : pm->parameters)
        {
            CubesXml::Item item{};
            for (const auto& pmGroup : pmItem->parameters)
            {
                auto id = pmGroup->id.mid(pmItem->id.size());
                if (id == ids_.base)
                {
                    for (auto& pmBase : pmGroup->parameters)
                    {
                        auto idBase = pmBase->id.mid(pmItem->id.size());
                        if (idBase == ids_.base + ids_.name)
                            item.name = pmBase->value.toString();
                    }
                }
                else if (id.startsWith(ids_.parameters))
                {
                    for (auto& pmParameter : pmGroup->parameters)
                    {
                        auto& pi = *parameters::helper::parameter::get_parameter_info(unitParametersPtr_->fileInfo,
                            pmParameter->parameterInfoId.type.toStdString(), pmParameter->parameterInfoId.name.toStdString());
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
                else if (id == ids_.editor)
                {
                    for (auto& pmEditor : pmGroup->parameters)
                    {
                        auto idEditor = pmEditor->id.mid(pmItem->id.size());

                        if (idEditor == ids_.editor + ids_.positionX)
                            item.x = pmEditor->value.toInt();
                        else if (idEditor == ids_.editor + ids_.positionY)
                            item.y = pmEditor->value.toInt();
                        else if (idEditor == ids_.editor + ids_.positionZ)
                            item.z = pmEditor->value.toInt();
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
    xmlUnit.id = QString::fromStdString(unitParametersPtr_->fileInfo.info.id);
    
    for (auto& pmGroup : parameterModelPtrs_)
    {
        if (pmGroup->id == ids_.base)
        {
            for (auto& pmBase : pmGroup->parameters)
            {
                if (pmBase->id == ids_.base + ids_.name)
                    xmlUnit.name = pmBase->value.toString();
            }
        }
        else if (pmGroup->id.startsWith(ids_.parameters))
        {
            for (auto& pmParameter : pmGroup->parameters)
            {
                auto& pi = *parameters::helper::parameter::get_parameter_info(unitParametersPtr_->fileInfo,
                    pmParameter->parameterInfoId.type.toStdString(), pmParameter->parameterInfoId.name.toStdString());
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
        else if (pmGroup->id == ids_.editor)
        {
            for (auto& pmEditor: pmGroup->parameters)
            {
                if (pmEditor->id == ids_.editor + ids_.positionX)
                    xmlUnit.x = pmEditor->value.toInt();
                else if (pmEditor->id == ids_.editor + ids_.positionY)
                    xmlUnit.y = pmEditor->value.toInt();
                else if (pmEditor->id == ids_.editor + ids_.positionZ)
                    xmlUnit.z = pmEditor->value.toInt();
            }
        }
    }
}

void PropertiesItem::RemoveItems(const CubesUnit::ParameterModelId& id)
{
    auto pm = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, id);
    for (auto& item : pm->parameters)
    {
        auto pi = GetProperty(item->id);
        UnregisterProperty(pi);
        delete pi;
    }
    pm->value = int{ 0 }; // !!!!!!!!!!!!!!! restrictions
    pm->parameters.clear();
}

void PropertiesItem::AddItems(CubesUnit::ParameterModelPtr model)
{
    QMap<CubesUnit::ParameterModelId, const QtProperty*> idToProperty;
    auto pi = GetProperty(model->id);
    for (auto& pm : model->parameters)
        pi->addSubProperty(editor_->CreatePropertyForModel(pm, idToProperty));
    for (const auto& kvp : idToProperty.toStdMap())
        RegisterProperty(kvp.second, kvp.first);

    auto pm = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, model->id);
    QString s = QString("%1").arg(model->parameters.size());
    if (pm->editorSettings.type == CubesUnit::EditorType::SpinInterger)
        pm->value = int{ model->parameters.size() }; // !!!!!!!!!!!!!!! restrictions
    else
        pm->SetComboBoxValue(s);
    pm->parameters = model->parameters;

    if (pm->editorSettings.type == CubesUnit::EditorType::SpinInterger)
        editor_->SetIntValue(pi, model->parameters.size());
    else
        editor_->SetEnumValue(pi, pm->GetComboBoxIndex(s));

    ApplyExpandState();
}

CubesAnalysis::Properties PropertiesItem::GetAnalysisProperties()
{
    CubesAnalysis::Properties properties{};
    properties.name = GetName();
    properties.dependencies = GetDependentNames().toVector();

    QVector<CubesAnalysis::UnitProperty> list;
    for (const auto& pm : parameterModelPtrs_)
    {
        if (pm->id == ids_.parameters)
            GetAnalysisPropertiesInternal(pm, list);
    }
    properties.connections = list;

    return properties;
}

CubesUnit::ParameterModelPtrs PropertiesItem::GetParameterModelPtrs()
{
    return parameterModelPtrs_;
}

QImage PropertiesItem::GetPixmap()
{
    QImage px;
    bool loaded = false;
    if (unitParametersPtr_->fileInfo.info.pictogram != "")
    {
        std::string s = base64_decode(unitParametersPtr_->fileInfo.info.pictogram);
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

void PropertiesItem::SetPosition(QPointF point)
{
    auto pm_x = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.editor + ids_.positionX);
    pm_x->value = point.x();
    if (editor_ != nullptr)
        editor_->SetDoubleValue(GetProperty(pm_x->id), point.x());

    auto pm_y = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.editor + ids_.positionY);
    pm_y->value = point.y();
    if (editor_ != nullptr)
        editor_->SetDoubleValue(GetProperty(pm_y->id), point.y());
}

void PropertiesItem::SetSize(QSizeF size)
{
    auto pm_x = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.parameters + ids_.width);
    pm_x->value = size.width();
    if (editor_ != nullptr)
        editor_->SetDoubleValue(GetProperty(pm_x->id), size.width());

    auto pm_y = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.parameters + ids_.height);
    pm_y->value = size.height();
    if (editor_ != nullptr)
        editor_->SetDoubleValue(GetProperty(pm_y->id), size.height());
}

void PropertiesItem::SetZOrder(double value)
{
    auto pm = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.editor + ids_.positionZ);
    pm->value = value;
    if (editor_ != nullptr)
        editor_->SetDoubleValue(GetProperty(pm->id), value);
}

double PropertiesItem::GetZOrder()
{
    const auto pmZ = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.editor + ids_.positionZ);
    if (pmZ != nullptr)
        return pmZ->value.toDouble();
    else
        return 0;
}

QSizeF PropertiesItem::GetSize()
{
    QSizeF size{ CubesDiagram::GridSize * 2, CubesDiagram::GridSize * 2 };

    const auto pm_w = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.parameters + ids_.width);
    if (pm_w != nullptr)
        size.setWidth(pm_w->value.toDouble());

    const auto pm_h = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.parameters + ids_.height);
    if (pm_h != nullptr)
        size.setHeight(pm_h->value.toDouble());

    return size;
}

CubesDiagram::ItemType PropertiesItem::GetItemType()
{
    CubesDiagram::ItemType itemType = CubesDiagram::ItemType::Unit;
    if (GetUnitCategory().compare("misc", Qt::CaseInsensitive) == 0)
    {
        if (GetUnitId().compare("text", Qt::CaseInsensitive) == 0)
            itemType = CubesDiagram::ItemType::Text;
        else if (GetUnitId().compare("group", Qt::CaseInsensitive) == 0)
            itemType = CubesDiagram::ItemType::Group;
    }
    return itemType;
}

QString PropertiesItem::GetPropertyDescription(QtProperty* property)
{
    QString id = GetPropertyId(property).toString();
    auto pm = GetParameterModelPtr(property);
    auto ui = GetUnitParametersPtr();
    auto pi = parameters::helper::parameter::get_parameter_info(ui->fileInfo,
        pm->parameterInfoId.type.toStdString(), pm->parameterInfoId.name.toStdString());

    if (pi != nullptr)
    {
        QString html = "<h3>" + QString::fromStdString(pi->display_name.c_str()) + "</h3>" +
            "<p>" + QString::fromStdString(pi->description.c_str()) + "</p>";

        //std::string description1251 = parameters::helper::common::get_description_as_cp1251(
        //    ui->fileInfo.info.description, parameters::helper::common::description_type::plain);
        return html;
    }

    return "";

    // ���������� ���������!!!
    // �� �������!!!
    // 
    //QStringList messageList;
    //messageList.push_back(QString("id: %1").arg(id));
    //if (pm->key == CubesUnit::InvalidBaseId)
    //    messageList.push_back(QString("key: %1").arg(pm->key));
    //if (pi != nullptr)
    //    messageList.push_back(QString("type: %1").arg(QString::fromStdString(pi->type)));
    //if (!pm->parameterInfoId.type.isEmpty() || !pm->parameterInfoId.name.isEmpty())
    //    messageList.push_back(QString("pi_id: %1, %2").arg(pm->parameterInfoId.type).arg(pm->parameterInfoId.name));
    //messageList.push_back(QString("fi_id: %1").arg(QString::fromStdString(ui->fileInfo.info.id)));

    //return messageList.join('\n');
    // 
    // �� �������!!!
}

void PropertiesItem::GetConnectedNamesInternal(CubesUnit::ParameterModelPtr model, QList<QString>& list)
{
    auto pi = parameters::helper::parameter::get_parameter_info(unitParametersPtr_->fileInfo,
        model->parameterInfoId.type.toStdString(), model->parameterInfoId.name.toStdString());

    if (pi != nullptr)
    {
        bool isArray = parameters::helper::common::get_is_array_type(pi->type);
        auto itemType = parameters::helper::common::get_item_type(pi->type);
        auto isUnitType = parameters::helper::common::get_is_unit_type(itemType);

        // isArray ����� ��� �����������, ��� ������ ���������� - ���������� ��������� ������� ���
        // �������� ���������. � ������ ��������� ���� �������� � �������� ��������� - parameterInfoId.
        // � �������� ���� yml items �� ������ ��������, � ��������� ����� ���� �������� � �������� � ����.
        // �������� � ��������������� ���������. � ��� ������ item ������ �������� � �������� �
        // �������� ���� �������, �.�. ��� ��������� parameter_info �������, ��� ������ item ��� ������,
        // ��� �� ����� ���� �� ���. �������, ������������� ��������� ����� ��������.
        // P.S. ��� ����� ��� ���������� ��������� ��������� �������
        // TODO: �������� ���� �������� � ������ ���� �� ���� ������

        // ��� �������������� �������� ��������������� ������� (��������, ���� array<int>)
        // pm->id = $PARAMETERS/CHANNELS/$ITEM_0/$PARAMETERS/BLOCKS/$ITEM_0
        if (model->id.size() > 2 && ids_.IsItem(model->id.right(1)))
            isArray = false;

        if (isUnitType && !isArray)
        {
            QString name = model->value.toString();
            if (name != "")
                list.push_back(name);
        }
    }

    for (const auto& pm : model->parameters)
        GetConnectedNamesInternal(pm, list);
}

void PropertiesItem::GetDependentNamesInternal(CubesUnit::ParameterModelPtr model, QList<QString>& list)
{
    auto pi = parameters::helper::parameter::get_parameter_info(unitParametersPtr_->fileInfo,
        model->parameterInfoId.type.toStdString(), model->parameterInfoId.name.toStdString());

    if (model->id == ids_.parameters + ids_.dependencies)
    {
        for (const auto& sub : model->parameters)
        {
            QString name = sub->value.toString();
            list.push_back(name);
        }
    }

    if (pi != nullptr)
    {
        bool isArray = parameters::helper::common::get_is_array_type(pi->type);
        auto itemType = parameters::helper::common::get_item_type(pi->type);
        auto isUnitType = parameters::helper::common::get_is_unit_type(itemType);

        // isArray ����� ��� �����������, ��� ������ ���������� - ���������� ��������� ������� ���
        // �������� ���������. � ������ ��������� ���� �������� � �������� ��������� - parameterInfoId.
        // � �������� ���� yml items �� ������ ��������, � ��������� ����� ���� �������� � �������� � ����.
        // �������� � ��������������� ���������. � ��� ������ item ������ �������� � �������� �
        // �������� ���� �������, �.�. ��� ��������� parameter_info �������, ��� ������ item ��� ������,
        // ��� �� ����� ���� �� ���. �������, ������������� ��������� ����� ��������.
        // P.S. ��� ����� ��� ���������� ��������� ��������� �������
        // TODO: �������� ���� �������� � ������ ���� �� ���� ������

        // ��� �������������� �������� ��������������� ������� (��������, ���� array<int>)
        // pm->id = $PARAMETERS/CHANNELS/$ITEM_0/$PARAMETERS/BLOCKS/$ITEM_0
        if (model->id.size() > 2 && ids_.IsItem(model->id.right(1)))
            isArray = false;

        if (isUnitType && !isArray)
        {
            for (const auto& sub : model->parameters)
            {
                if (sub->id.endsWith(ids_.depends) &&
                    sub->value.type() == QVariant::Type::Bool &&
                    sub->value.toBool() == true)
                {
                    QString name = model->value.toString();
                    list.push_back(name);
                    break;
                }
            }
        }
    }

    for (const auto& pm : model->parameters)
        GetDependentNamesInternal(pm, list);
}

void PropertiesItem::GetAnalysisPropertiesInternal(CubesUnit::ParameterModelPtr model, QVector<CubesAnalysis::UnitProperty>& list)
{
    auto pi = parameters::helper::parameter::get_parameter_info(unitParametersPtr_->fileInfo,
        model->parameterInfoId.type.toStdString(), model->parameterInfoId.name.toStdString());

    if (pi != nullptr)
    {
        bool isArray = parameters::helper::common::get_is_array_type(pi->type);
        auto itemType = parameters::helper::common::get_item_type(pi->type);
        auto isUnitType = parameters::helper::common::get_is_unit_type(itemType);

        // isArray ����� ��� �����������, ��� ������ ���������� - ���������� ��������� ������� ���
        // �������� ���������. � ������ ��������� ���� �������� � �������� ��������� - parameterInfoId.
        // � �������� ���� yml items �� ������ ��������, � ��������� ����� ���� �������� � �������� � ����.
        // �������� � ��������������� ���������. � ��� ������ item ������ �������� � �������� �
        // �������� ���� �������, �.�. ��� ��������� parameter_info �������, ��� ������ item ��� ������,
        // ��� �� ����� ���� �� ���. �������, ������������� ��������� ����� ��������.
        // P.S. ��� ����� ��� ���������� ��������� ��������� �������
        // TODO: �������� ���� �������� � ������ ���� �� ���� ������

        // ��� �������������� �������� ��������������� ������� (��������, ���� array<int>)
        // pm->id = $PARAMETERS/CHANNELS/$ITEM_0/$PARAMETERS/BLOCKS/$ITEM_0
        if (model->id.size() > 2 && ids_.IsItem(model->id.right(1)))
            isArray = false;

        if (isUnitType && !isArray)
        {
            CubesAnalysis::UnitProperty unit{};
            unit.id = QString::fromStdString(pi->name);
            unit.name = model->value.toString();
            const auto pmDepends = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, model->id + ids_.depends);
            unit.depends = pmDepends->value.toBool();
            unit.dontSet = false;
            bool isOptional = parameters::helper::parameter::get_is_optional(*pi);
            if (isOptional)
            {
                const auto pmOptional = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, model->id + ids_.optional);
                unit.dontSet = pmOptional->value.toBool();
            }
            unit.category = QString::fromStdString(pi->restrictions.category);
            for (const auto& id : pi->restrictions.ids)
                unit.ids.push_back(QString::fromStdString(id));

            list.push_back(unit);
        }
    }

    for (const auto& pm : model->parameters)
        GetAnalysisPropertiesInternal(pm, list);
}

QList<QString> PropertiesItem::GetConnectedNames()
{
    QList<QString> list;
    for (const auto& pm : parameterModelPtrs_)
    {
        if (pm->id == ids_.parameters)
            GetConnectedNamesInternal(pm, list);
    }
    return list;
}

QList<QString> PropertiesItem::GetDependentNames()
{
    QList<QString> list;
    for (const auto& pm : parameterModelPtrs_)
    {
        if (pm->id == ids_.parameters)
            GetDependentNamesInternal(pm, list);
    }
    return list;
}

QString PropertiesItem::GetText()
{
    const auto pm = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.parameters + ids_.text);
    if (pm != nullptr)
        return pm->value.toString();

    return "";
}

uint32_t PropertiesItem::GetFontSize()
{
    const auto pm = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.parameters + ids_.fontSize);
    if (pm != nullptr)
        return pm->value.toUInt();

    return 10;
}

bool PropertiesItem::GetShowBorder()
{
    const auto pm = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.parameters + ids_.showBorder);
    if (pm != nullptr)
        return pm->value.toBool();

    return "";
}

CubesDiagram::HorizontalAlignment PropertiesItem::GetHorizontalAlignment()
{
    const auto pm = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.parameters + ids_.hAlignment);
    if (pm != nullptr)
        return static_cast<CubesDiagram::HorizontalAlignment>(pm->key);

    return CubesDiagram::HorizontalAlignment::Left;
}

CubesDiagram::VerticalAlignment PropertiesItem::GetVerticalAlignment()
{
    const auto pm = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.parameters + ids_.vAlignment);
    if (pm != nullptr)
        return static_cast<CubesDiagram::VerticalAlignment>(pm->key);

    return CubesDiagram::VerticalAlignment::Top;
}

void PropertiesItem::FillArrayModel(const CubesXml::Unit* xmlUnit, CubesUnit::ParameterModelPtr model)
{
    // ������� ������ ��� ���������, ��������� ���������� ��������� �������
    // ���� ����� �������� ���������, ���������� ComboBox, ���� ��� - SpinBox
    // ���� ����������� �� ������ - ���������� ��������� ����� ����
    // ���� id, name, parameterInfoId ������ ���� �������������� ���������
    // ���� xmlUnit != nullptr, ������ ������� ���� �� ����� xml

    auto& pi = *parameters::helper::parameter::get_parameter_info(unitParametersPtr_->fileInfo,
        model->parameterInfoId.type.toStdString(), model->parameterInfoId.name.toStdString());
    auto v = parameters::helper::parameter::get_initial_value(unitParametersPtr_->fileInfo, pi, false);
    bool res = CubesParameters::convert_variant(v, model->value);
    //model->valueType = "int";

    //int xmlCount = 0;
    //if (xmlUnit != nullptr)
    //    xmlCount = CubesXml::Helper::GetItemsCount(*const_cast<CubesXml::Unit*>(xmlUnit), model->id);

    CubesXml::Element element{};
    if (xmlUnit != nullptr && !CubesXml::Helper::GetElement(*const_cast<CubesXml::Unit*>(xmlUnit), model->id, element))
        assert(false);

    if (pi.restrictions.set_count.size() > 0)
    {
        CubesUnit::BaseId key{CubesUnit::InvalidBaseId};
        model->editorSettings.type = CubesUnit::EditorType::ComboBox;
        for (int i = 0; i < pi.restrictions.set_count.size(); ++i)
        {
            model->editorSettings.comboBoxValues.push_back({ static_cast<CubesUnit::BaseId>(i),
                QString::fromStdString(pi.restrictions.set_count[i]) });
            if (QString::fromStdString(pi.restrictions.set_count[i]) == QString("%1").arg(element.itemsCount))
                key = static_cast<CubesUnit::BaseId>(i);
        }

        if (element.type == CubesXml::ElementType::Array)
        {
            if (!model->HaveComboBoxValue(QString("%1").arg(element.itemsCount)))
            {
                logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>(PropertiesItemErrorCode::restrictionFailed),
                    { {QString::fromLocal8Bit("��� ��������� xml"), element.param->name},
                    {QString::fromLocal8Bit("��� ��������� xml"), element.param->type},
                    {QString::fromLocal8Bit("��� �����"), GetName()},
                    {QString::fromLocal8Bit("��� �����"), QString::fromStdString(unitParametersPtr_->fileInfo.info.id)},
                    {QString::fromLocal8Bit("��������"), QString("%1").arg(element.itemsCount)} }, propertiesId_);
            }
            else
            {
                model->value = QString("%1").arg(element.itemsCount);
                model->key = key;
            }
        }
    }
    else
    {
        model->editorSettings.type = CubesUnit::EditorType::SpinInterger;

        if (pi.restrictions.min_count != "")
            model->editorSettings.spinIntergerMin = std::stoi(pi.restrictions.min_count);
        else
            model->editorSettings.spinIntergerMin = 0;

        if (pi.restrictions.max_count != "")
            model->editorSettings.spinIntergerMax = std::stoi(pi.restrictions.max_count);
        else
            model->editorSettings.spinIntergerMax = 1000; // !!! TODO: make a define for a const

        if (element.type == CubesXml::ElementType::Array)
        {
            if (element.itemsCount < model->editorSettings.spinIntergerMin || element.itemsCount > model->editorSettings.spinIntergerMax)
            {
                logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>(PropertiesItemErrorCode::restrictionFailed),
                    { {QString::fromLocal8Bit("��� ��������� xml"), element.param->name},
                    {QString::fromLocal8Bit("��� ��������� xml"), element.param->type},
                    {QString::fromLocal8Bit("��� �����"), GetName()},
                    {QString::fromLocal8Bit("��� �����"), QString::fromStdString(unitParametersPtr_->fileInfo.info.id)},
                    {QString::fromLocal8Bit("��������"), QString("%1").arg(element.itemsCount)} }, propertiesId_);
            }
            else
                model->value = QString("%1").arg(element.itemsCount);
        }
    }
}

void PropertiesItem::UpdateArrayModel(const CubesXml::Unit* xmlUnit, CubesUnit::ParameterModelPtr model)
{
    // ��������� ������ �������, �� ������������ ���������� ��������� � ���
    // ���� id, name, parameterInfoId ������ ���� �������������� ���������
    // ���� xmlUnit != nullptr, ������ ������� ���� �� ����� xml

    // �������� �������� ��������� �� ��� yml �����
    auto& pi = *parameters::helper::parameter::get_parameter_info(unitParametersPtr_->fileInfo,
        model->parameterInfoId.type.toStdString(), model->parameterInfoId.name.toStdString());

    auto itemType = parameters::helper::common::get_item_type(pi.type);
    int itemsCount = model->value.toInt();

    // ���������� ��� ������� - �������������� ��� ���
    // ���� ��� ������� - ���������� ��� ��� ������������ (enum), ��
    // �������� ������� ������� � ���� ������ ���� ��������
    // ���� ��� ����������������� yml ���, �� ������ ������� ������� - ��� ���������,
    // ������� ����� �������������� ���� BASE � EDITOR � ��������� ������ PARAMETERS

    bool isSimpleType = false;
    bool isEnumType = false;
    bool isYmlType = false;
    if (parameters::helper::common::get_is_inner_type(itemType))
    {
        isSimpleType = true;
    }
    else
    {
        auto pti = parameters::helper::type::get_type_info(unitParametersPtr_->fileInfo, itemType);
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

    // ��������� ������
    if (isSimpleType || isEnumType)
    {
        for (int i = model->parameters.size(); i < itemsCount; ++i)
        {
            CubesUnit::ParameterModelPtr itemModel = CubesUnit::CreateParameterModelPtr();
            itemModel->id = model->id + ids_.Item(i);
            itemModel->name = QString::fromLocal8Bit("������� %1").arg(i);
            itemModel->parameterInfoId = model->parameterInfoId;
            // �������� �� xml ����������� � FillParameterModel, ��� �������
            FillParameterModel(xmlUnit, itemModel, true);
            model->parameters.push_back(itemModel);
        }
    }
    else if (isYmlType)
    {
        for (int i = model->parameters.size(); i < itemsCount; ++i)
        {
            CubesUnit::ParameterModelPtr group_model = CubesUnit::CreateParameterModelPtr();
            group_model->id = model->id + ids_.Item(i);
            group_model->name = QString::fromLocal8Bit("������� %1").arg(i);
            group_model->value = QVariant();
            group_model->editorSettings.type = CubesUnit::EditorType::None;

            CubesXml::Element element;
            if (xmlUnit != nullptr && !CubesXml::Helper::GetElement(*const_cast<CubesXml::Unit*>(xmlUnit), group_model->id, element))
                assert(false);

            // ��������� ������� ���������
            {
                CubesUnit::ParameterModelPtr base_group = CubesUnit::CreateParameterModelPtr();
                base_group->id = group_model->id + ids_.base;
                base_group->name = QString::fromLocal8Bit("�������");
                base_group->value = QVariant();
                base_group->editorSettings.type = CubesUnit::EditorType::None;

                CubesUnit::ParameterModelPtr instance_name = CubesUnit::CreateParameterModelPtr();
                instance_name->id = group_model->id + ids_.base + ids_.name;
                instance_name->name = QString::fromLocal8Bit("���");
                if (element.type == CubesXml::ElementType::Item)
                    instance_name->value = QString(element.item->name);
                else
                    instance_name->value = QString(group_model->name);
                instance_name->editorSettings.type = CubesUnit::EditorType::String;

                base_group->parameters.push_back(std::move(instance_name));

                group_model->parameters.push_back(std::move(base_group));
            }

            // �������� �������� ����, ��������� �����, ��� ����� ��� ����������
            const auto ti = *parameters::helper::type::get_type_info(unitParametersPtr_->fileInfo, itemType);

            // ��������� yml ���������
            if (!ti.parameters.empty())
            {
                CubesUnit::ParameterModelPtr properties_group = CubesUnit::CreateParameterModelPtr();
                properties_group->id = group_model->id + ids_.parameters;
                properties_group->name = QString::fromLocal8Bit("���������");
                properties_group->value = QVariant();
                properties_group->editorSettings.type = CubesUnit::EditorType::None;
                properties_group->editorSettings.isExpanded = true;

                CheckParametersMatching(xmlUnit, QString::fromStdString(ti.name), properties_group->id);

                for (const auto& pi : ti.parameters)
                {
                    CubesUnit::ParameterModelPtr pm = CubesUnit::CreateParameterModelPtr();
                    CreateParameterModel({ QString::fromStdString(ti.name), QString::fromStdString(pi.name) },
                        group_model->id + ids_.parameters, xmlUnit, pm);
                    properties_group->parameters.push_back(std::move(pm));
                }

                group_model->parameters.push_back(std::move(properties_group));
            }

            // ��������� ��������� ���������
            {
                CubesUnit::ParameterModelPtr editor_group = CubesUnit::CreateParameterModelPtr();
                editor_group->id = group_model->id + ids_.editor;
                editor_group->name = QString::fromLocal8Bit("��������");
                editor_group->value = QVariant();
                editor_group->editorSettings.type = CubesUnit::EditorType::None;

                {
                    CubesUnit::ParameterModelPtr pm = CubesUnit::CreateParameterModelPtr();
                    pm->id = group_model->id + ids_.editor + ids_.positionX;
                    pm->name = QString::fromLocal8Bit("������� X");
                    pm->value = double{ element.type == CubesXml::ElementType::Item ? element.item->x : 0.0 };
                    pm->editorSettings.type = CubesUnit::EditorType::SpinDouble;
                    pm->editorSettings.spinDoubleMin = -10000;
                    pm->editorSettings.spinDoubleMax = 10000;
                    pm->editorSettings.spinDoubleSingleStep = CubesDiagram::GridSize;
                    editor_group->parameters.push_back(std::move(pm));
                }

                {
                    CubesUnit::ParameterModelPtr pm = CubesUnit::CreateParameterModelPtr();
                    pm->id = group_model->id + ids_.editor + ids_.positionY;
                    pm->name = QString::fromLocal8Bit("������� Y");
                    pm->value = double{ element.type == CubesXml::ElementType::Item ? element.item->y : 0.0 };
                    pm->editorSettings.type = CubesUnit::EditorType::SpinDouble;
                    pm->editorSettings.spinDoubleMin = -10000;
                    pm->editorSettings.spinDoubleMax = 10000;
                    pm->editorSettings.spinDoubleSingleStep = CubesDiagram::GridSize;
                    editor_group->parameters.push_back(std::move(pm));
                }

                {
                    CubesUnit::ParameterModelPtr pm = CubesUnit::CreateParameterModelPtr();
                    pm->id = group_model->id + ids_.editor + ids_.positionZ;
                    pm->name = QString::fromLocal8Bit("������� Z");
                    pm->value = double{ element.type == CubesXml::ElementType::Item ? element.item->z : 0.0 };
                    pm->editorSettings.type = CubesUnit::EditorType::SpinDouble;
                    pm->editorSettings.spinDoubleMin = -10000;
                    pm->editorSettings.spinDoubleMax = 10000;
                    editor_group->parameters.push_back(std::move(pm));
                }

                group_model->parameters.push_back(std::move(editor_group));
            }

            model->parameters.push_back(std::move(group_model));
        }
    }

    while (model->parameters.size() > itemsCount)
        model->parameters.pop_back();
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

    auto pm = GetParameterModelPtr(property);
    if (pm == nullptr)
        return;

    if (pm->id.startsWith(ids_.base))
    {
        if (pm->id == ids_.base + ids_.name)
        {
            // ������������ ���������������� ������ ��������� ����� qtexpropertymanager
            // ��������� ���������� �������������� ������ ���������� � StringEditingFinished
        }
        else if (pm->id == ids_.base + ids_.fileName)
        {
            const auto enumIndex = editor_->GetEnumValue(property);
            pm->key = pm->editorSettings.comboBoxValues[enumIndex].id;
            pm->value = property->valueText();

            CubesUnit::IncludeIdNames includeNames;
            propertiesItemsManager_->AfterFileNameChanged(propertiesId_, includeNames);

            SetIncludeIdNames(includeNames);
            SetIncludeIdName(CubesUnit::InvalidIncludeId, "<not selected>"); // TODO: ������ <not selected> � ���������
        }
        else if (pm->id == ids_.base + ids_.includeName)
        {
            const auto enumIndex = editor_->GetEnumValue(property);
            pm->key = pm->editorSettings.comboBoxValues[enumIndex].id;
            pm->value = property->valueText();

            propertiesItemsManager_->AfterIncludeNameChanged(propertiesId_);
        }
    }
    else if (pm->id.startsWith(ids_.parameters))
    {
        if (pm->id.size() < 2)
            return;

        // � ��������� ���������� ���� �������������� ��������� DEPENDS � unit,
        // � ���� ��������� ����� ���� �������� OPTIONAL

        // � ��������� ������� ������� ������ ���������� BASE, PARAMETERS, EDITOR
        // ������� �� � ����� ���� ������ ���������, ����������� � ������ PARAMETERS
        // ��������: $PARAMETERS/CHANNELS/$ITEM_0/$BASE/NAME,
        // $PARAMETERS/CHANNELS/$ITEM_0/$PARAMETERS/FIRMWARE � �.�.
        // ������� ������������� ��������� ������������� ����� � id

        if (pm->id.right(1) == ids_.depends || pm->id.right(1) == ids_.optional)
        {
            // �������� �������������� ��������
            // � ��� ��� �������� � parameter_info, �.�. ��� ��������� ���� ��� ��������� �����
            bool b = false;
            auto boolString = property->valueText().toLower().toStdString();
            std::istringstream(boolString) >> std::boolalpha >> b;
            pm->value = b;
        }
        else if (pm->id.size() > 2 && (pm->id.right(2).startsWith(ids_.base) ||
            pm->id.right(2).startsWith(ids_.editor)))
        {
            // ���� ��� ������� ������� ���� yml, �������� �������������� ��������
            // � ��� ��� �������� � parameter_info, �.�. ��� ��������� ���� ��� ��������� �����
            if (pm->id.right(2).startsWith(ids_.base))
            {
                pm->value = property->valueText();
            }
            else if (pm->id.right(2).startsWith(ids_.editor))
            {
                pm->value = property->valueText().toDouble();
            }
        }
        else
        {
            // �������� �������� ��������� �� ��� yml �����
            auto& pi = *parameters::helper::parameter::get_parameter_info(unitParametersPtr_->fileInfo,
                pm->parameterInfoId.type.toStdString(), pm->parameterInfoId.name.toStdString());

            bool isArray = parameters::helper::common::get_is_array_type(pi.type);
            auto itemType = parameters::helper::common::get_item_type(pi.type);

            // isArray ����� ��� �����������, ��� ������ ���������� - ���������� ��������� ������� ���
            // �������� ���������. � ������ ��������� ���� �������� � �������� ��������� - parameterInfoId.
            // � �������� ���� yml items �� ������ ��������, � ��������� ����� ���� �������� � �������� � ����.
            // �������� � ��������������� ���������. � ��� ������ item ������ �������� � �������� �
            // �������� ���� �������, �.�. ��� ��������� parameter_info �������, ��� ������ item ��� ������,
            // ��� �� ����� ���� �� ���. �������, ������������� ��������� ����� ��������.
            // P.S. ��� ����� ��� ���������� ��������� ��������� �������
            // TODO: �������� ���� �������� � ������ ���� �� ���� ������

            // ��� �������������� �������� ��������������� ������� (��������, ���� array<int>)
            // pm->id = $PARAMETERS/CHANNELS/$ITEM_0/$PARAMETERS/BLOCKS/$ITEM_0
            if (pm->id.size() > 2 && ids_.IsItem(pm->id.right(1)))
                isArray = false;

            if (isArray)
            {
                int count = std::stoi(property->valueText().toStdString());
                pm->value = count;
                UpdateArrayModel(nullptr, pm);

                QMap<CubesUnit::ParameterModelId, const QtProperty*> idToProperty;
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
                {
                    UnregisterProperty(p);
                    delete p;
                }

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
                {
                    // enum
                    const auto enumIndex = editor_->GetEnumValue(property);
                    pm->key = pm->editorSettings.comboBoxValues[enumIndex].id;
                    pm->value = property->valueText();
                    break;
                }
                case parameters::base_item_types::none:
                default:
                    pm->value = property->valueText();
                    break;
                }
            }
        }

        if (pm->id.endsWith(ids_.depends) || pm->id.endsWith(ids_.dependencies))
            propertiesItemsManager_->AfterConnectionChanged(propertiesId_);

        if (pm->id == ids_.parameters + ids_.text || pm->id == ids_.parameters + ids_.fontSize ||
            pm->id == ids_.parameters + ids_.showBorder || pm->id == ids_.parameters + ids_.hAlignment ||
            pm->id == ids_.parameters + ids_.vAlignment)
        {
            propertiesItemsManager_->AfterTextChanged(propertiesId_, GetText(), GetFontSize(), GetShowBorder(),
                GetHorizontalAlignment(), GetVerticalAlignment());
        }

        if (pm->id == ids_.parameters + ids_.width || pm->id == ids_.parameters + ids_.height)
            propertiesItemsManager_->AfterSizeChanged(propertiesId_, GetSize());
    }
    else if (pm->id.startsWith(ids_.editor))
    {
        if (pm->id == ids_.editor + ids_.positionX)
        {
            pm->value = property->valueText().toDouble();
            double posX = property->valueText().toDouble();

            auto pmY = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.editor + ids_.positionY);
            double posY = pmY->value.toDouble();

            auto pmZ = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.editor + ids_.positionZ);
            double posZ = pmZ->value.toDouble();

            propertiesItemsManager_->AfterPositionChanged(propertiesId_, posX, posY, posZ);
        }
        else if (pm->id == ids_.editor + ids_.positionY)
        {
            auto pmX = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.editor + ids_.positionX);
            double posX = pmX->value.toDouble();

            pm->value = property->valueText().toDouble();
            double posY = property->valueText().toDouble();

            auto pmZ = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.editor + ids_.positionZ);
            double posZ = pmZ->value.toDouble();

            propertiesItemsManager_->AfterPositionChanged(propertiesId_, posX, posY, posZ);
        }
        else if (pm->id == ids_.editor + ids_.positionZ)
        {
            auto pmX = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.editor + ids_.positionX);
            double posX = pmX->value.toDouble();

            auto pmY = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, ids_.editor + ids_.positionY);
            double posY = pmY->value.toDouble();

            pm->value = property->valueText().toDouble();
            double posZ = property->valueText().toDouble();

            propertiesItemsManager_->AfterPositionChanged(propertiesId_, posX, posY, posZ);
        }
    }

    propertiesItemsManager_->AfterPropertiesChanged();
}

void PropertiesItem::StringEditingFinished(QtProperty* property, const QString& value, const QString& oldValue)
{
    qDebug() << "StringEditingFinished value = " << value << ", oldValue = " << oldValue;

    auto pm = GetParameterModelPtr(property);
    if (pm == nullptr)
        return;

    if (pm->id == ids_.base + ids_.name)
    {
        pm->value = value;
        propertiesItemsManager_->AfterNameChanged(propertiesId_);

        // ������ ��������� ��������� ����� ����������:
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
        //    // ������
        //    editor_->SetStringValue(property, oldValue);
        //}
    }
    else if ((pm->id.startsWith(ids_.parameters) && pm->id.size() == 2) ||
        (pm->id.startsWith(ids_.parameters) && pm->id.size() > 2 && !pm->id.right(2).startsWith(ids_.base)))
    {
        // ������ ��������� ����� ���������, � ������� ����� ���� ����� � ��������� ���������
        // � ����������� ���������

        // �������� �������� ��������� �� ��� yml �����
        auto& pi = *parameters::helper::parameter::get_parameter_info(unitParametersPtr_->fileInfo,
            pm->parameterInfoId.type.toStdString(), pm->parameterInfoId.name.toStdString());

        bool isArray = parameters::helper::common::get_is_array_type(pi.type);
        auto itemType = parameters::helper::common::get_item_type(pi.type);
        auto isUnitType = parameters::helper::common::get_is_unit_type(itemType);

        // isArray ����� ��� �����������, ��� ������ ���������� - ���������� ��������� ������� ���
        // �������� ���������. � ������ ��������� ���� �������� � �������� ��������� - parameterInfoId.
        // � �������� ���� yml items �� ������ ��������, � ��������� ����� ���� �������� � �������� � ����.
        // �������� � ��������������� ���������. � ��� ������ item ������ �������� � �������� �
        // �������� ���� �������, �.�. ��� ��������� parameter_info �������, ��� ������ item ��� ������,
        // ��� �� ����� ���� �� ���. �������, ������������� ��������� ����� ��������.
        // P.S. ��� ����� ��� ���������� ��������� ��������� �������
        // TODO: �������� ���� �������� � ������ ���� �� ���� ������

        // ��� �������������� �������� ��������������� ������� (��������, ���� array<int>)
        // pm->id = $PARAMETERS/CHANNELS/$ITEM_0/$PARAMETERS/BLOCKS/$ITEM_0
        if (pm->id.size() > 2 && ids_.IsItem(pm->id.right(1)))
            isArray = false;

        if (isUnitType && !isArray)
        {
            propertiesItemsManager_->AfterConnectionChanged(propertiesId_);
        }

        if (pm->id.startsWith(ids_.parameters + ids_.dependencies))
        {
            propertiesItemsManager_->AfterConnectionChanged(propertiesId_);
        }
    }

    propertiesItemsManager_->AfterPropertiesChanged();
}

void PropertiesItem::RegisterProperty(const QtProperty* property, const CubesUnit::ParameterModelId& id)
{
    propertyToId_[property] = id;
    idToProperty_[id] = property;
}

void PropertiesItem::UnregisterProperty(const CubesUnit::ParameterModelId& id)
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

QtProperty* PropertiesItem::GetProperty(const CubesUnit::ParameterModelId& id)
{
    auto it = idToProperty_.find(id);
    if (it != idToProperty_.end())
        return const_cast<QtProperty*>(*it);
    return nullptr;
}

CubesUnit::ParameterModelId PropertiesItem::GetPropertyId(const QtProperty* property)
{
    auto it = propertyToId_.find(property);
    if (it != propertyToId_.end())
        return *it;
    return {};
}

CubesUnit::ParameterModelPtr PropertiesItem::GetParameterModelPtr(const QtProperty* property)
{
    auto id = GetPropertyId(property);
    if (id.empty())
        return nullptr;

    return CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs_, id);
}

CubesUnit::UnitParameters* GetPropertyUnitParameters(const QtProperty* property)
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

    auto pm = GetParameterModelPtr(prop);
    if (pm != nullptr)
        pe->setExpanded(index, pm->editorSettings.isExpanded);
}

void PropertiesItem::ApplyExpandState()
{
    auto pe = editor_->GetPropertyEditor();

    QList<QtBrowserItem*> indexes = pe->topLevelItems();
    QListIterator<QtBrowserItem*> itItem(indexes);
    while (itItem.hasNext())
        ApplyExpandState(itItem.next());
}

bool PropertiesItem::CheckParametersMatching(const CubesXml::Unit* xmlUnit, const QString& type, const CubesUnit::ParameterModelId& id)
{
    if (xmlUnit == nullptr)
        return true;

    CubesXml::Element element{};
    if (!CubesXml::Helper::GetElement(*const_cast<CubesXml::Unit*>(xmlUnit), id, element))
        return false;

    if (element.type != CubesXml::ElementType::Service)
        return false;
    
    if (element.params != nullptr)
    {
        for (const auto& xpi : *(element.params))
        {
            const auto pi = parameters::helper::parameter::get_parameter_info(unitParametersPtr_->fileInfo, type.toStdString(), xpi.name.toStdString());

            if (pi == nullptr)
            {
                logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>(PropertiesItemErrorCode::notFound),
                    { {QString::fromLocal8Bit("��� ��������� xml"), xpi.name},
                    {QString::fromLocal8Bit("��� ��������� xml"), type},
                    {QString::fromLocal8Bit("��� �����"), GetName()},
                    {QString::fromLocal8Bit("��� �����"), QString::fromStdString(unitParametersPtr_->fileInfo.info.id)} }, propertiesId_);
            }
        }
    }

    if (element.arrays != nullptr)
    {
        for (const auto& xai : *(element.arrays))
        {
            const auto pi = parameters::helper::parameter::get_parameter_info(unitParametersPtr_->fileInfo, type.toStdString(), xai.name.toStdString());

            if (pi == nullptr)
            {
                logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>(PropertiesItemErrorCode::notFound),
                    { {QString::fromLocal8Bit("��� ��������� xml"), xai.name},
                    {QString::fromLocal8Bit("��� ��������� xml"), type},
                    {QString::fromLocal8Bit("��� �����"), GetName()},
                    {QString::fromLocal8Bit("��� �����"), QString::fromStdString(unitParametersPtr_->fileInfo.info.id)} }, propertiesId_);
            }
        }
    }

    return true;
}