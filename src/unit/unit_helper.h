#pragma once

#include "unit_types.h"
#include "unit_parameter_model.h"
#include "unit_parameter_model_id.h"
#include "unit_parameter_model_ids.h"

namespace CubesUnit
{
	namespace Helper
	{
		namespace Common
		{
			QString GetUniqueName(QString baseName, QString delimiter, QStringList busyNames);
			ParameterModel* GetParameterModel(ParameterModels& parameterModels, const ParameterModelId& parameterModelId);
			const ParameterModel* GetParameterModel(const ParameterModels& parameterModels, const ParameterModelId& parameterModelId);
		}

		namespace File
		{
			bool GetIncludes(const ParameterModels& parameterModels, IncludeIdNames& includeIdNames);
			IncludeIdNames GetIncludes(const ParameterModels& parameterModels);
			bool GetIncludeName(const ParameterModels& parameterModels, const IncludeId includeId, QString& includeName);
			QString GetIncludeName(const ParameterModels& parameterModels, const IncludeId includeId);
			bool GetIncludePath(const ParameterModels& parameterModels, const IncludeId includeId, QString& includeName);
			QString GetIncludePath(const ParameterModels& parameterModels, const IncludeId includeId);
			bool GetIncludeVariables(const ParameterModels& parameterModels, const IncludeId includeId,
				VariableIdVariables& variableIdVariables);
			VariableIdVariables GetIncludeVariables(const ParameterModels& parameterModels, const IncludeId includeId);
		}

		namespace Unit
		{
			bool GetUnitParameters(const QString& unitId, const UnitIdUnitParameters& unitIdUnitParameters,
				CubesUnit::UnitParameters& unitParameters);
		}

		namespace Analyse
		{
			struct UnitName
			{
				QString original;
				QString resolved;
			};

			using PropertiesIdUnitNames = QMap<PropertiesId, UnitName>;

			struct UnitProperty
			{
				QString id;
				QString name;
				bool depends;
				bool dontSet;
				QString category;
				QStringList ids;
			};

			struct UnitDependency
			{
				UnitName name;
				bool isUnitLevel;
			};

			UnitName GetResolvedUnitName(const ParameterModels& parameterModels, const FileIdParameterModels& fileIdParameterModels);
			PropertiesIdUnitNames GetResolvedUnitNames(const PropertiesIdParameterModels& propertiesIdParameterModels,
				const FileIdParameterModels& fileIdParametersModels);
			UnitName GetResolvedUnitName(const ParameterModels& parameterModels, const FileIdParameterModels& fileIdParameterModels,
				QString name);
			QVector<UnitProperty> GetParameterModelsUnitProperties(const ParameterModels& parameterModels,
				const UnitIdUnitParameters& unitIdUnitParameters);
			QVector<UnitProperty> GetParameterModelUnitProperties(const ParameterModel& parameterModel,
				const ParameterModels& parameterModels, const UnitParameters& unitParameters);
			QString GetUnitId(PropertiesId propertiesId, const PropertiesIdParameterModels& propertiesIdParameterModels,
				const UnitIdUnitParameters& unitIdUnitParameters);
			QString GetUnitCategory(PropertiesId propertiesId, const PropertiesIdParameterModels& propertiesIdParameterModels,
				const UnitIdUnitParameters& unitIdUnitParameters);
			QVector<Analyse::UnitDependency> GetParameterModelsDependencies(const ParameterModels& parameterModels,
				const FileIdParameterModels& fileIdParametersModels, const UnitIdUnitParameters& unitIdUnitParameters);
		}
	}
}
