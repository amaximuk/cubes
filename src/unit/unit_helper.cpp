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

ParameterModel* Common::GetParameterModel(ParameterModels& parameterModels, const ParameterModelId& parameterModelId)
{
	ParameterModel* pm = nullptr;

	{
		auto sl = parameterModelId.split();
		auto ql = &parameterModels;
		ParameterModelId idt;
		while (sl.size() > 0)
		{
			idt += sl[0];
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

const ParameterModel* Common::GetParameterModel(const ParameterModels& parameterModels, const ParameterModelId& id)
{
	return GetParameterModel(*const_cast<ParameterModels*>(&parameterModels), id);
}

bool File::GetIncludes(const ParameterModels& parameterModels, IncludeIdNames& includeIdNames)
{
	const static ParameterModelIds ids;

	const auto pm = Common::GetParameterModel(parameterModels, ids.includes);
	if (pm == nullptr)
		return false;

	for (int i = 0; i < pm->value.toInt(); i++)
	{
		const auto pmItem = Common::GetParameterModel(parameterModels, ids.includes + ids.Item(i));
		const auto includeId = pmItem->key;

		const auto pmItemName = Common::GetParameterModel(parameterModels, ids.includes + ids.Item(i) + ids.name);
		const auto includeName = pmItemName->value.toString();

		includeIdNames[includeId] = includeName;
	}

	return true;
}

IncludeIdNames File::GetIncludes(const ParameterModels& parameterModels)
{
	CubesUnit::IncludeIdNames includeIdNames;
	if (!File::GetIncludes(parameterModels, includeIdNames))
		return {};

	return includeIdNames;
}

bool File::GetIncludeName(const ParameterModels& parameterModels, const IncludeId includeId, QString& includeName)
{
	const static ParameterModelIds ids;

	const auto pm = Common::GetParameterModel(parameterModels, ids.includes);
	if (pm == nullptr)
		return false;

	for (int i = 0; i < pm->value.toInt(); i++)
	{
		const auto pmin = Common::GetParameterModel(parameterModels, ids.includes + ids.Item(i));
		if (pmin->key == includeId)
		{
			const auto pmif = Common::GetParameterModel(parameterModels, ids.includes + ids.Item(i) + ids.name);
			includeName = pmif->value.toString();
			return true;
		}
	}

	return true;
}

QString File::GetIncludeName(const ParameterModels& parameterModels, const IncludeId includeId)
{
	QString includeName;
	if (!GetIncludeName(parameterModels, includeId, includeName))
		return {};

	return includeName;
}

bool File::GetIncludePath(const ParameterModels& parameterModels, const IncludeId includeId, QString& includeName)
{
	const static ParameterModelIds ids;

	const auto pm = Common::GetParameterModel(parameterModels, ids.includes);
	if (pm == nullptr)
		return false;

	for (int i = 0; i < pm->value.toInt(); i++)
	{
		const auto pmin = Common::GetParameterModel(parameterModels, ids.includes + ids.Item(i));
		if (pmin->key == includeId)
		{
			const auto pmif = Common::GetParameterModel(parameterModels, ids.includes + ids.Item(i) + ids.filePath);
			includeName = pmif->value.toString();
			return true;
		}
	}

	return true;
}

QString File::GetIncludePath(const ParameterModels& parameterModels, const IncludeId includeId)
{
	QString includePath;
	if (!GetIncludePath(parameterModels, includeId, includePath))
		return {};

	return includePath;
}

bool File::GetIncludeVariables(const ParameterModels& parameterModels, const IncludeId includeId,
	VariableIdVariables& variablesIdVariables)
{
	const static ParameterModelIds ids;

	const auto pm = Common::GetParameterModel(parameterModels, ids.includes);
	if (pm == nullptr)
		return false;

	for (int i = 0; i < pm->value.toInt(); i++)
	{
		const auto pmi = Common::GetParameterModel(parameterModels, ids.includes + ids.Item(i));
		if (pmi->key == includeId)
		{
			const auto pmiv = Common::GetParameterModel(parameterModels, ids.includes + ids.Item(i) + ids.variables);
			for (int j = 0; j < pmiv->value.toInt(); j++)
			{
				auto pmiv = Common::GetParameterModel(parameterModels, ids.includes + ids.Item(i) + ids.variables + ids.Item(j));
				auto pmivn = Common::GetParameterModel(parameterModels, ids.includes + ids.Item(i) + ids.variables + ids.Item(j) + ids.name);
				auto pmivv = Common::GetParameterModel(parameterModels, ids.includes + ids.Item(i) + ids.variables + ids.Item(j) + ids.value);
				variablesIdVariables[pmiv->key] = { pmivn->value.toString(), pmivv->value.toString() };
			}
			break;
		}
	}

	return true;
}

VariableIdVariables File::GetIncludeVariables(const ParameterModels& parameterModels, const IncludeId includeId)
{
	VariableIdVariables variableIdVariables;
	if (!GetIncludeVariables(parameterModels, includeId, variableIdVariables))
		return {};

	return variableIdVariables;
}

bool Unit::GetUnitParameters(const QString& unitId, const UnitIdUnitParameters& unitIdUnitParameters,
	CubesUnit::UnitParameters& unitParameters)
{
	// ƒл€ массивов - мы не знаем название, но у нас всего один элемент в списке
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

Analyse::UnitName Analyse::GetResolvedUnitName(const ParameterModels& parameterModels,
	const FileIdParameterModels& fileIdParametersModels)
{
	const static ParameterModelIds ids;

	const auto name = Common::GetParameterModel(parameterModels, ids.base + ids.name)->value.toString();
	const auto fileId = Common::GetParameterModel(parameterModels, ids.base + ids.fileName)->key;
	const auto includeId = Common::GetParameterModel(parameterModels, ids.base + ids.includeName)->key;

	QString realName = name;
	if (fileId != CubesUnit::InvalidFileId && includeId != CubesUnit::InvalidIncludeId)
	{
		const auto it = fileIdParametersModels.find(fileId);
		if (it != fileIdParametersModels.end())
		{
			const auto vars = CubesUnit::Helper::File::GetIncludeVariables(*it, includeId);
			for (const auto& kvpVars : vars.toStdMap())
				realName.replace(QString("@%1@").arg(kvpVars.second.first), kvpVars.second.second);
		}
	}

	return { name, realName };
}

Analyse::PropertiesIdUnitNames Analyse::GetResolvedUnitNames(const PropertiesIdParameterModels& propertiesModels,
	const FileIdParameterModels& fileIdParametersModels)
{
	PropertiesIdUnitNames result;
	for (auto& kvp : propertiesModels.toStdMap())
	{
		auto& parameterModels = kvp.second;
		result[kvp.first] = (GetResolvedUnitName(parameterModels, fileIdParametersModels));
	}

	return result;
}

Analyse::UnitName Analyse::GetResolvedUnitName(const ParameterModels& parameterModels,
	const FileIdParameterModels& fileIdParametersModels, QString name)
{
	const static ParameterModelIds ids;

	const auto fileId = Common::GetParameterModel(parameterModels, ids.base + ids.fileName)->key;
	const auto includeId = Common::GetParameterModel(parameterModels, ids.base + ids.includeName)->key;

	QString realName = name;
	if (fileId != InvalidFileId && includeId != InvalidIncludeId)
	{
		const auto it = fileIdParametersModels.find(fileId);
		if (it != fileIdParametersModels.end())
		{
			const auto vars = File::GetIncludeVariables(*it, includeId);
			for (const auto& kvpVars : vars.toStdMap())
				realName.replace(QString("@%1@").arg(kvpVars.second.first), kvpVars.second.second);
		}
	}

	return { name, realName };
}

QVector<Analyse::UnitProperty> Analyse::GetParameterModelsUnitProperties(const ParameterModels& parameterModels,
	const UnitIdUnitParameters& unitIdUnitParameters)
{
	QVector<Analyse::UnitProperty> result;

	const auto unitId = Common::GetParameterModel(parameterModels, ids.base + ids.unitId)->value.toString();

	UnitParameters unitParameters{};
	Unit::GetUnitParameters(unitId, unitIdUnitParameters, unitParameters);

	for (const auto& pm : parameterModels)
	{
		if (pm.id == ids.parameters)
		{
			const auto list = GetParameterModelUnitProperties(pm, parameterModels, unitParameters);
			for (const auto& item : list)
				result.push_back(item);
		}
	}

	return result;
}

QVector<Analyse::UnitProperty> Analyse::GetParameterModelUnitProperties(const ParameterModel& parameterModel,
	const ParameterModels& parameterModels, const UnitParameters& unitParameters)
{
	QVector<Analyse::UnitProperty> result;

	auto pi = parameters::helper::parameter::get_parameter_info(unitParameters.fileInfo,
		parameterModel.parameterInfoId.type.toStdString(), parameterModel.parameterInfoId.name.toStdString());

	if (pi != nullptr)
	{
		bool isArray = parameters::helper::common::get_is_array_type(pi->type);
		auto itemType = parameters::helper::common::get_item_type(pi->type);
		auto isUnitType = parameters::helper::common::get_is_unit_type(itemType);

		// isArray нужен дл€ определени€, что именно помен€лось - количество элементов массива или
		// значение параметра. ¬ модели параметра есть прив€зка к описанию параметра - parameterInfoId.
		// ¬ массивах типа yml items не хран€т значение, а параметры имеют свое описание и прив€зку к типу.
		// ѕроблема с типизированными массивами. ¬ них каждый item хранит значение и прив€зку к
		// базовому типу массива, т.е. при получении parameter_info получим, что каждый item это массив,
		// что на самом деле не так. ѕоэтому, дополнительно провер€ем такую ситуацию.
		// P.S. “ип нужен дл€ корректной обработки элементов массива
		// TODO: ¬озможно надо добавить в модель флаг на этот случай

		// ѕри редактировании элемента типизированного массива (например, типа array<int>)
		// pm->id = $PARAMETERS/CHANNELS/$ITEM_0/$PARAMETERS/BLOCKS/$ITEM_0
		if (parameterModel.id.size() > 2 && ids.IsItem(parameterModel.id.right(1)))
			isArray = false;

		if (isUnitType && !isArray)
		{
			UnitProperty unit{};
			unit.id = QString::fromStdString(pi->name);
			unit.name = parameterModel.value.toString();
			const auto pmDepends = CubesUnit::Helper::Common::GetParameterModel(parameterModels, parameterModel.id + ids.depends);
			unit.depends = pmDepends->value.toBool();
			unit.dontSet = false;
			bool isOptional = parameters::helper::parameter::get_is_optional(*pi);
			if (isOptional)
			{
				const auto pmOptional = CubesUnit::Helper::Common::GetParameterModel(parameterModels, parameterModel.id + ids.optional);
				unit.dontSet = pmOptional->value.toBool();
			}
			unit.category = QString::fromStdString(pi->restrictions.category);
			for (const auto& id : pi->restrictions.ids)
				unit.ids.push_back(QString::fromStdString(id));

			result.push_back(unit);
		}
	}

	for (const auto& pm : parameterModel.parameters)
	{
		const auto list = GetParameterModelUnitProperties(pm, parameterModels, unitParameters);
		for (const auto& item : list)
			result.push_back(item);
	}

	return result;
}

QString Analyse::GetUnitId(PropertiesId propertiesId, const PropertiesIdParameterModels& propertiesIdParameterModels,
	const UnitIdUnitParameters& unitIdUnitParameters)
{
	if (propertiesIdParameterModels.contains(propertiesId))
	{
		const auto& parameterModels = propertiesIdParameterModels[propertiesId];
		const auto unitId = Common::GetParameterModel(parameterModels, ids.base + ids.unitId)->value.toString();
		return unitId;
	}

	return {};
}

QString Analyse::GetUnitCategory(PropertiesId propertiesId, const PropertiesIdParameterModels& propertiesIdParameterModels,
	const UnitIdUnitParameters& unitIdUnitParameters)
{
	if (propertiesIdParameterModels.contains(propertiesId))
	{
		const auto& parameterModels = propertiesIdParameterModels[propertiesId];
		const auto unitId = Common::GetParameterModel(parameterModels, ids.base + ids.unitId)->value.toString();

		if (unitIdUnitParameters.contains(unitId))
		{
			const auto& unitParameters = unitIdUnitParameters[unitId];
			return QString::fromStdString(unitParameters.fileInfo.info.category);
		}
	}

	return {};
}

//QVector<Analyse::UnitProperty> Analyse::GetParameterModelsDependencies(const ParameterModels& parameterModels,
//	const UnitIdUnitParameters& unitIdUnitParameters)
//{
//	QVector<Analyse::UnitProperty> result;
//
//	const auto unitId = Common::GetParameterModel(parameterModels, ids.base + ids.unitId)->value.toString();
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

QVector<Analyse::UnitDependency> Analyse::GetParameterModelsDependencies(const ParameterModels& parameterModels,
	const FileIdParameterModels& fileIdParametersModels, const UnitIdUnitParameters& unitIdUnitParameters)
{
	QVector<Analyse::UnitDependency> result;

	const auto pmDepends = Common::GetParameterModel(parameterModels, ids.parameters + ids.dependencies);
	for (const auto& sub : pmDepends->parameters)
	{
		QString name = sub.value.toString();

		UnitDependency ud{};
		ud.name = GetResolvedUnitName(parameterModels, fileIdParametersModels, name);
		ud.isUnitLevel = true;
		result.push_back(ud);
	}

	const auto unitId = Common::GetParameterModel(parameterModels, ids.base + ids.unitId)->value.toString();

	UnitParameters unitParameters{};
	Unit::GetUnitParameters(unitId, unitIdUnitParameters, unitParameters);

	for (const auto& pm : parameterModels)
	{
		const auto list = GetParameterModelUnitProperties(pm, parameterModels, unitParameters);
		for (const auto& item : list)
		{
			if (item.depends)
			{
				UnitDependency ud{};
				ud.name = GetResolvedUnitName(parameterModels, fileIdParametersModels, item.name);
				ud.isUnitLevel = false;
				result.push_back(ud);
			}
		}
	}

	return result;
}
