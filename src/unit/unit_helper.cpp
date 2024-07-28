#pragma once

#include "unit_types.h"
#include "unit_parameter_model.h"
#include "unit_parameter_model_id.h"
#include "unit_parameter_model_ids.h"
#include "parameters/helper_parameter.h"
#include "parameters/helper_common.h"
#include "unit_helper.h"

using namespace CubesUnit;
using namespace CubesUnit::Helper;

const static ParameterModelIds ids;

QString Common::GetUniqueName(QString baseName, QString delimiter, QStringList busyNames)
{
	QString name = baseName;
	int counter = 0;
	while (busyNames.contains(name))
		name = QString("%1%2%3").arg(baseName, delimiter).arg(++counter);
	return name;
}

ParameterModelPtr Common::GetParameterModelPtr(ParameterModelPtrs parameterModelPtrs,
	const ParameterModelId& parameterModelId)
{
	ParameterModelPtr pm = nullptr;

	auto sl = parameterModelId.split();
	ParameterModelPtrs& ql = parameterModelPtrs;
	ParameterModelId idt;
	while (sl.size() > 0)
	{
		idt += sl[0];
		bool found = false;
		for (auto& x : ql)
		{
			if (x->id == idt)
			{
				pm = x;
				ql = x->parameters;
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

	return pm;
}

//ParameterModelPtr Common::GetParameterModelPtr(ParameterModelPtrs parameterModelPtrs,
//	const ParameterModelId& parameterModelId)
//{
//	ParameterModelPtrs parameterModelPtrs;
//	for (const auto& item : parameterModelPtrs)
//		parameterModelPtrs.push_back(QSharedPointer<ParameterModel>(const_cast<ParameterModel*>(item.get())));
//	return GetParameterModelPtr(parameterModelPtrs, parameterModelId);
//}

bool File::GetIncludes(ParameterModelPtrs parameterModelPtrs, IncludeIdNames& includeIdNames)
{
	const static ParameterModelIds ids;

	const auto pm = Common::GetParameterModelPtr(parameterModelPtrs, ids.includes);
	if (pm == nullptr)
		return false;

	for (int i = 0; i < pm->value.toInt(); i++)
	{
		const auto pmItem = Common::GetParameterModelPtr(parameterModelPtrs, ids.includes + ids.Item(i));
		const auto includeId = pmItem->key;

		const auto pmItemName = Common::GetParameterModelPtr(parameterModelPtrs, ids.includes + ids.Item(i) + ids.name);
		const auto includeName = pmItemName->value.toString();

		includeIdNames[includeId] = includeName;
	}

	return true;
}

IncludeIdNames File::GetIncludes(ParameterModelPtrs parameterModelPtrs)
{
	CubesUnit::IncludeIdNames includeIdNames;
	if (!File::GetIncludes(parameterModelPtrs, includeIdNames))
		return {};

	return includeIdNames;
}

bool File::GetIncludeName(ParameterModelPtrs parameterModelPtrs, const IncludeId includeId, QString& includeName)
{
	const static ParameterModelIds ids;

	const auto pm = Common::GetParameterModelPtr(parameterModelPtrs, ids.includes);
	if (pm == nullptr)
		return false;

	for (int i = 0; i < pm->value.toInt(); i++)
	{
		const auto pmin = Common::GetParameterModelPtr(parameterModelPtrs, ids.includes + ids.Item(i));
		if (pmin->key == includeId)
		{
			const auto pmif = Common::GetParameterModelPtr(parameterModelPtrs, ids.includes + ids.Item(i) + ids.name);
			includeName = pmif->value.toString();
			return true;
		}
	}

	return true;
}

QString File::GetIncludeName(ParameterModelPtrs parameterModelPtrs, const IncludeId includeId)
{
	QString includeName;
	if (!GetIncludeName(parameterModelPtrs, includeId, includeName))
		return {};

	return includeName;
}

bool File::GetIncludePath(ParameterModelPtrs parameterModelPtrs, const IncludeId includeId, QString& includeName)
{
	const static ParameterModelIds ids;

	const auto pm = Common::GetParameterModelPtr(parameterModelPtrs, ids.includes);
	if (pm == nullptr)
		return false;

	for (int i = 0; i < pm->value.toInt(); i++)
	{
		const auto pmin = Common::GetParameterModelPtr(parameterModelPtrs, ids.includes + ids.Item(i));
		if (pmin->key == includeId)
		{
			const auto pmif = Common::GetParameterModelPtr(parameterModelPtrs, ids.includes + ids.Item(i) + ids.filePath);
			includeName = pmif->value.toString();
			return true;
		}
	}

	return true;
}

QString File::GetIncludePath(ParameterModelPtrs parameterModelPtrs, const IncludeId includeId)
{
	QString includePath;
	if (!GetIncludePath(parameterModelPtrs, includeId, includePath))
		return {};

	return includePath;
}

bool File::GetIncludeVariables(ParameterModelPtrs parameterModelPtrs, const IncludeId includeId,
	VariableIdVariables& variablesIdVariables)
{
	const static ParameterModelIds ids;

	const auto pm = Common::GetParameterModelPtr(parameterModelPtrs, ids.includes);
	if (pm == nullptr)
		return false;

	for (int i = 0; i < pm->value.toInt(); i++)
	{
		const auto pmi = Common::GetParameterModelPtr(parameterModelPtrs, ids.includes + ids.Item(i));
		if (pmi->key == includeId)
		{
			const auto pmiv = Common::GetParameterModelPtr(parameterModelPtrs, ids.includes + ids.Item(i) + ids.variables);
			for (int j = 0; j < pmiv->value.toInt(); j++)
			{
				auto pmiv = Common::GetParameterModelPtr(parameterModelPtrs, ids.includes + ids.Item(i) + ids.variables + ids.Item(j));
				auto pmivn = Common::GetParameterModelPtr(parameterModelPtrs, ids.includes + ids.Item(i) + ids.variables + ids.Item(j) + ids.name);
				auto pmivv = Common::GetParameterModelPtr(parameterModelPtrs, ids.includes + ids.Item(i) + ids.variables + ids.Item(j) + ids.value);
				variablesIdVariables[pmiv->key] = { pmivn->value.toString(), pmivv->value.toString() };
			}
			break;
		}
	}

	return true;
}

VariableIdVariables File::GetIncludeVariables(ParameterModelPtrs parameterModelPtrs, const IncludeId includeId)
{
	VariableIdVariables variableIdVariables;
	if (!GetIncludeVariables(parameterModelPtrs, includeId, variableIdVariables))
		return {};

	return variableIdVariables;
}

bool Unit::GetUnitParameters(const QString& unitId, const UnitIdUnitParameters& unitIdUnitParameters,
	CubesUnit::UnitParameters& unitParameters)
{
	// ��� �������� - �� �� ����� ��������, �� � ��� ����� ���� ������� � ������
	if (unitId.isEmpty() && unitIdUnitParameters.size() == 1)
	{
		unitParameters = *unitIdUnitParameters.begin();
	}
	else
	{
		const auto it = unitIdUnitParameters.find(unitId);
		if (it == unitIdUnitParameters.end())
			return false;
		unitParameters = *it;
	}

	return true;
}

Analyse::UnitName Analyse::GetResolvedUnitName(ParameterModelPtrs parameterModelPtrs,
	FileIdParameterModelPtrs fileIdParametersModelPtrs)
{
	const static ParameterModelIds ids;

	const auto name = Common::GetParameterModelPtr(parameterModelPtrs, ids.base + ids.name)->value.toString();
	const auto fileId = Common::GetParameterModelPtr(parameterModelPtrs, ids.base + ids.fileName)->key;
	const auto includeId = Common::GetParameterModelPtr(parameterModelPtrs, ids.base + ids.includeName)->key;

	QString realName = name;
	if (fileId != CubesUnit::InvalidFileId && includeId != CubesUnit::InvalidIncludeId)
	{
		const auto it = fileIdParametersModelPtrs.find(fileId);
		if (it != fileIdParametersModelPtrs.end())
		{
			const auto vars = CubesUnit::Helper::File::GetIncludeVariables(*it, includeId);
			for (const auto& kvpVars : vars.toStdMap())
				realName.replace(QString("@%1@").arg(kvpVars.second.first), kvpVars.second.second);
		}
	}

	return { name, realName };
}

Analyse::PropertiesIdUnitNames Analyse::GetResolvedUnitNames(PropertiesIdParameterModelPtrs propertiesIdParameterModelPtrs,
	FileIdParameterModelPtrs fileIdParametersModelPtrs)
{
	PropertiesIdUnitNames result;
	for (auto& kvp : propertiesIdParameterModelPtrs.toStdMap())
	{
		auto& parameterModels = kvp.second;
		result[kvp.first] = (GetResolvedUnitName(parameterModels, fileIdParametersModelPtrs));
	}

	return result;
}

Analyse::UnitName Analyse::GetResolvedUnitName(ParameterModelPtrs parameterModelPtrs,
	FileIdParameterModelPtrs fileIdParametersModelPtrs, QString name)
{
	const static ParameterModelIds ids;

	const auto fileId = Common::GetParameterModelPtr(parameterModelPtrs, ids.base + ids.fileName)->key;
	const auto includeId = Common::GetParameterModelPtr(parameterModelPtrs, ids.base + ids.includeName)->key;

	QString realName = name;
	if (fileId != InvalidFileId && includeId != InvalidIncludeId)
	{
		const auto it = fileIdParametersModelPtrs.find(fileId);
		if (it != fileIdParametersModelPtrs.end())
		{
			const auto vars = File::GetIncludeVariables(*it, includeId);
			for (const auto& kvpVars : vars.toStdMap())
				realName.replace(QString("@%1@").arg(kvpVars.second.first), kvpVars.second.second);
		}
	}

	return { name, realName };
}

QVector<Analyse::UnitProperty> Analyse::GetParameterModelsUnitProperties(ParameterModelPtrs parameterModelPtrs,
	const UnitIdUnitParameters& unitIdUnitParameters)
{
	QVector<Analyse::UnitProperty> result;

	const auto unitId = Common::GetParameterModelPtr(parameterModelPtrs, ids.base + ids.unitId)->value.toString();

	UnitParameters unitParameters{};
	Unit::GetUnitParameters(unitId, unitIdUnitParameters, unitParameters);

	for (const auto& pm : parameterModelPtrs)
	{
		if (pm->id == ids.parameters)
		{
			const auto list = GetParameterModelUnitProperties(pm, parameterModelPtrs, unitParameters);
			for (const auto& item : list)
				result.push_back(item);
		}
	}

	return result;
}

QVector<Analyse::UnitProperty> Analyse::GetParameterModelUnitProperties(ParameterModelPtr parameterModelPtr,
	ParameterModelPtrs parameterModelPtrs, const UnitParameters& unitParameters)
{
	QVector<Analyse::UnitProperty> result;

	auto pi = parameters::helper::parameter::get_parameter_info(unitParameters.fileInfo,
		parameterModelPtr->parameterInfoId.type.toStdString(), parameterModelPtr->parameterInfoId.name.toStdString());

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
		if (parameterModelPtr->id.size() > 2 && ids.IsItem(parameterModelPtr->id.right(1)))
			isArray = false;

		if (isUnitType && !isArray)
		{
			UnitProperty unit{};
			unit.id = QString::fromStdString(pi->name);
			unit.name = parameterModelPtr->value.toString();
			const auto pmDepends = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs,
				parameterModelPtr->id + ids.depends);
			unit.depends = pmDepends->value.toBool();
			unit.dontSet = false;
			bool isOptional = parameters::helper::parameter::get_is_optional(*pi);
			if (isOptional)
			{
				const auto pmOptional = CubesUnit::Helper::Common::GetParameterModelPtr(parameterModelPtrs,
					parameterModelPtr->id + ids.optional);
				unit.dontSet = pmOptional->value.toBool();
			}
			unit.category = QString::fromStdString(pi->restrictions.category);
			for (const auto& id : pi->restrictions.ids)
				unit.ids.push_back(QString::fromStdString(id));

			result.push_back(unit);
		}
	}

	for (const auto& pm : parameterModelPtr->parameters)
	{
		const auto list = GetParameterModelUnitProperties(pm, parameterModelPtrs, unitParameters);
		for (const auto& item : list)
			result.push_back(item);
	}

	return result;
}

QString Analyse::GetUnitId(PropertiesId propertiesId, PropertiesIdParameterModelPtrs propertiesIdParameterModelPtrs,
	const UnitIdUnitParameters& unitIdUnitParameters)
{
	if (propertiesIdParameterModelPtrs.contains(propertiesId))
	{
		const auto& parameterModelPtrs = propertiesIdParameterModelPtrs[propertiesId];
		const auto unitId = Common::GetParameterModelPtr(parameterModelPtrs, ids.base + ids.unitId)->value.toString();
		return unitId;
	}

	return {};
}

QString Analyse::GetUnitCategory(PropertiesId propertiesId, PropertiesIdParameterModelPtrs propertiesIdParameterModelPtrs,
	const UnitIdUnitParameters& unitIdUnitParameters)
{
	if (propertiesIdParameterModelPtrs.contains(propertiesId))
	{
		const auto& parameterModelPtrs = propertiesIdParameterModelPtrs[propertiesId];
		const auto unitId = Common::GetParameterModelPtr(parameterModelPtrs, ids.base + ids.unitId)->value.toString();

		if (unitIdUnitParameters.contains(unitId))
		{
			const auto& unitParameters = unitIdUnitParameters[unitId];
			return QString::fromStdString(unitParameters.fileInfo.info.category);
		}
	}

	return {};
}

//QVector<Analyse::UnitProperty> Analyse::GetParameterModelsDependencies(ParameterModelPtrs parameterModelPtrs,
//	const UnitIdUnitParameters& unitIdUnitParameters)
//{
//	QVector<Analyse::UnitProperty> result;
//
//	const auto unitId = Common::GetParameterModelPtr(parameterModelPtrs, ids.base + ids.unitId)->value.toString();
//
//	UnitParameters unitParameters{};
//	Unit::GetUnitParameters(unitId, unitIdUnitParameters, unitParameters);
//
//	for (const auto& pm : parameterModels)
//	{
//		if (pm.id == ids.parameters)
//		{
//			const auto list = GetParameterModelUnitProperties(pm, parameterModels, unitParameters);
//			for (const auto& item : list)
//				result.push_back(item);
//		}
//	}
//
//	return result;
//}

QVector<Analyse::UnitDependency> Analyse::GetParameterModelsDependencies(ParameterModelPtrs parameterModelPtrs,
	FileIdParameterModelPtrs fileIdParametersModelPtrs, const UnitIdUnitParameters& unitIdUnitParameters)
{
	QVector<Analyse::UnitDependency> result;

	const auto pmDepends = Common::GetParameterModelPtr(parameterModelPtrs, ids.parameters + ids.dependencies);
	if (pmDepends != nullptr)
	{
		for (const auto& sub : pmDepends->parameters)
		{
			QString name = sub->value.toString();

			UnitDependency ud{};
			ud.name = GetResolvedUnitName(parameterModelPtrs, fileIdParametersModelPtrs, name);
			ud.isUnitLevel = true;
			result.push_back(ud);
		}
	}

	const auto pmUnitId = Common::GetParameterModelPtr(parameterModelPtrs, ids.base + ids.unitId);
	if (pmUnitId != nullptr)
	{
		// � ��������� �������� ��� ������ ���������
		const auto unitId = pmUnitId->value.toString();

		UnitParameters unitParameters{};
		Unit::GetUnitParameters(unitId, unitIdUnitParameters, unitParameters);

		for (const auto& pm : parameterModelPtrs)
		{
			const auto list = GetParameterModelUnitProperties(pm, parameterModelPtrs, unitParameters);
			for (const auto& item : list)
			{
				if (item.depends)
				{
					UnitDependency ud{};
					ud.name = GetResolvedUnitName(parameterModelPtrs, fileIdParametersModelPtrs, item.name);
					ud.isUnitLevel = false;
					result.push_back(ud);
				}
			}
		}
	}

	return result;
}
