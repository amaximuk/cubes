#pragma once

#include "unit_types.h"
#include "unit_parameter_model.h"
#include "unit_parameter_model_id.h"
#include "unit_parameter_model_ids.h"

namespace CubesUnit
{
	namespace Helper
	{
		inline QString GetUniqueName(QString baseName, QString delimiter, QStringList busyNames)
		{
			QString name = baseName;
			int counter = 0;
			while (busyNames.contains(name))
				name = QString("%1%2%3").arg(baseName, delimiter).arg(++counter);
			return name;
		}

		inline ParameterModel* GetParameterModel(ParameterModels& models, const ParameterModelId& id)
		{
			ParameterModel* pm = nullptr;

			{
				auto sl = id.split();
				auto ql = &models;
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

		inline const ParameterModel* GetParameterModel(const ParameterModels& models, const ParameterModelId& id)
		{
			return GetParameterModel(*const_cast<ParameterModels*>(&models), id);
		}
	}

	// TODO: добавить cpp, перенести ids
	namespace Helper
	{
		namespace File
		{
			inline bool GetIncludes(const ParameterModels& parameterModels, IncludeIdNames& includeIdNames)
			{
				const static ParameterModelIds ids;

				const auto pm = GetParameterModel(parameterModels, ids.includes);
				if (pm == nullptr)
					return false;

				for (int i = 0; i < pm->value.toInt(); i++)
				{
					const auto pmItem = GetParameterModel(parameterModels, ids.includes + ids.Item(i));
					const auto includeId = pmItem->key;

					const auto pmItemName = GetParameterModel(parameterModels, ids.includes + ids.Item(i) + ids.name);
					const auto includeName = pmItemName->value.toString();

					includeIdNames[includeId] = includeName;
				}

				return true;
			}

			inline IncludeIdNames GetIncludes(const ParameterModels& parameterModels)
			{
				CubesUnit::IncludeIdNames includeIdNames;
				if (!GetIncludes(parameterModels, includeIdNames))
					return {};

				return includeIdNames;
			}

			inline bool GetIncludeName(const ParameterModels& parameterModels, const IncludeId includeId, QString& includeName)
			{
				const static ParameterModelIds ids;

				const auto pm = GetParameterModel(parameterModels, ids.includes);
				if (pm == nullptr)
					return false;

				for (int i = 0; i < pm->value.toInt(); i++)
				{
					const auto pmin = GetParameterModel(parameterModels, ids.includes + ids.Item(i));
					if (pmin->key == includeId)
					{
						const auto pmif = GetParameterModel(parameterModels, ids.includes + ids.Item(i) + ids.name);
						includeName = pmif->value.toString();
						return true;
					}
				}

				return true;
			}

			inline QString GetIncludeName(const ParameterModels& parameterModels, const IncludeId includeId)
			{
				QString includeName;
				if (!GetIncludeName(parameterModels, includeId, includeName))
					return {};

				return includeName;
			}

			inline bool GetIncludePath(const ParameterModels& parameterModels, const IncludeId includeId, QString& includeName)
			{
				const static ParameterModelIds ids;

				const auto pm = GetParameterModel(parameterModels, ids.includes);
				if (pm == nullptr)
					return false;

				for (int i = 0; i < pm->value.toInt(); i++)
				{
					const auto pmin = GetParameterModel(parameterModels, ids.includes + ids.Item(i));
					if (pmin->key == includeId)
					{
						const auto pmif = GetParameterModel(parameterModels, ids.includes + ids.Item(i) + ids.filePath);
						includeName = pmif->value.toString();
						return true;
					}
				}

				return true;
			}

			inline QString GetIncludePath(const ParameterModels& parameterModels, const IncludeId includeId)
			{
				QString includePath;
				if (!GetIncludePath(parameterModels, includeId, includePath))
					return {};

				return includePath;
			}

			inline bool GetIncludeVariables(const ParameterModels& parameterModels, const IncludeId includeId, VariableIdVariables& variables)
			{
				const static ParameterModelIds ids;

				const auto pm = GetParameterModel(parameterModels, ids.includes);
				if (pm == nullptr)
					return false;

				for (int i = 0; i < pm->value.toInt(); i++)
				{
					const auto pmi = GetParameterModel(parameterModels, ids.includes + ids.Item(i));
					if (pmi->key == includeId)
					{
						const auto pmiv = GetParameterModel(parameterModels, ids.includes + ids.Item(i) + ids.variables);
						for (int j = 0; j < pmiv->value.toInt(); j++)
						{
							auto pmiv = GetParameterModel(parameterModels, ids.includes + ids.Item(i) + ids.variables + ids.Item(j));
							auto pmivn = GetParameterModel(parameterModels, ids.includes + ids.Item(i) + ids.variables + ids.Item(j) + ids.name);
							auto pmivv = GetParameterModel(parameterModels, ids.includes + ids.Item(i) + ids.variables + ids.Item(j) + ids.value);
							variables[pmiv->key] = { pmivn->value.toString(), pmivv->value.toString() };
						}
						break;
					}
				}

				return true;
			}

			inline VariableIdVariables GetIncludeVariables(const ParameterModels& parameterModels, const IncludeId includeId)
			{
				VariableIdVariables variableIdVariables;
				if (!GetIncludeVariables(parameterModels, includeId, variableIdVariables))
					return {};

				return variableIdVariables;
			}
		}

		namespace Analyse
		{
			struct UnitName
			{
				QString original;
				QString resolved;
			};

			using PropertiesIdUnitNames = QMap<PropertiesId, UnitName>;

			struct Unit
			{
				UnitName name;
				bool depends;
			};

			inline UnitName GetResolvedUnitName(const ParameterModels& parameterModels,
				const FileIdParameterModels& fileModels)
			{
				const static ParameterModelIds ids;

				const auto name = CubesUnit::Helper::GetParameterModel(parameterModels, ids.base + ids.name)->value.toString();
				const auto fileId = CubesUnit::Helper::GetParameterModel(parameterModels, ids.base + ids.fileName)->key;
				const auto includeId = CubesUnit::Helper::GetParameterModel(parameterModels, ids.base + ids.includeName)->key;

				QString realName = name;
				if (fileId != CubesUnit::InvalidFileId && includeId != CubesUnit::InvalidIncludeId)
				{
					const auto it = fileModels.find(fileId);
					if (it != fileModels.end())
					{
						const auto vars = CubesUnit::Helper::File::GetIncludeVariables(*it, includeId);
						for (const auto& kvpVars : vars.toStdMap())
							realName.replace(QString("@%1@").arg(kvpVars.second.first), kvpVars.second.second);
					}
				}

				return { name, realName };
			}

			inline PropertiesIdUnitNames GetResolvedUnitNames(const PropertiesIdParameterModels& propertiesModels,
				const FileIdParameterModels& fileModels)
			{
				PropertiesIdUnitNames result;
				for (auto& kvp : propertiesModels.toStdMap())
				{
					auto& parameterModels = kvp.second;
					result[kvp.first] = (GetResolvedUnitName(parameterModels, fileModels));
				}

				return result;
			}

			inline UnitName GetResolvedUnitName(const ParameterModels& parameterModels,
				const FileIdParameterModels& fileModels, QString name)
			{
				const static ParameterModelIds ids;

				const auto fileId = CubesUnit::Helper::GetParameterModel(parameterModels, ids.base + ids.fileName)->key;
				const auto includeId = CubesUnit::Helper::GetParameterModel(parameterModels, ids.base + ids.includeName)->key;

				QString realName = name;
				if (fileId != CubesUnit::InvalidFileId && includeId != CubesUnit::InvalidIncludeId)
				{
					const auto it = fileModels.find(fileId);
					if (it != fileModels.end())
					{
						const auto vars = CubesUnit::Helper::File::GetIncludeVariables(*it, includeId);
						for (const auto& kvpVars : vars.toStdMap())
							realName.replace(QString("@%1@").arg(kvpVars.second.first), kvpVars.second.second);
					}
				}

				return { name, realName };
			}

			inline QVector<Unit> GetUnitUnitParameters(const ParameterModels& parameterModels,
				const FileIdParameterModels& fileModels)
			{
				const static ParameterModelIds ids;

				const auto name = CubesUnit::Helper::GetParameterModel(parameterModels, ids.base + ids.name)->value.toString();
				const auto fileId = CubesUnit::Helper::GetParameterModel(parameterModels, ids.base + ids.fileName)->key;
				const auto includeId = CubesUnit::Helper::GetParameterModel(parameterModels, ids.base + ids.includeName)->key;

				return {};
			}

		}
	}
}
