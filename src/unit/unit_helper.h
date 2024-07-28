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
			ParameterModelPtr GetParameterModelPtr(ParameterModelPtrs parameterModelPtrs,
				const ParameterModelId& parameterModelId);
			ParameterModelPtr GetParameterModelPtr(ParameterModelPtrs parameterModelPtrs,
				const ParameterModelId& parameterModelId);
		}

		namespace File
		{
			bool GetIncludes(ParameterModelPtrs parameterModelPtrs, IncludeIdNames& includeIdNames);
			IncludeIdNames GetIncludes(ParameterModelPtrs parameterModelPtrs);
			bool GetIncludeName(ParameterModelPtrs parameterModelPtrs, const IncludeId includeId, QString& includeName);
			QString GetIncludeName(ParameterModelPtrs parameterModelPtrs, const IncludeId includeId);
			bool GetIncludePath(ParameterModelPtrs parameterModelPtrs, const IncludeId includeId, QString& includeName);
			QString GetIncludePath(ParameterModelPtrs parameterModelPtrs, const IncludeId includeId);
			bool GetIncludeVariables(ParameterModelPtrs parameterModelPtrs, const IncludeId includeId,
				VariableIdVariables& variableIdVariables);
			VariableIdVariables GetIncludeVariables(ParameterModelPtrs parameterModelPtrs, const IncludeId includeId);
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

			UnitName GetResolvedUnitName(ParameterModelPtrs parameterModelPtrs,
				FileIdParameterModelPtrs fileIdParametersModelPtrs);
			PropertiesIdUnitNames GetResolvedUnitNames(PropertiesIdParameterModelPtrs propertiesIdParameterModelPtrs,
				FileIdParameterModelPtrs fileIdParametersModelPtrs);
			UnitName GetResolvedUnitName(ParameterModelPtrs parameterModelPtrs,
				FileIdParameterModelPtrs fileIdParametersModelPtrs, QString name);
			QVector<UnitProperty> GetParameterModelsUnitProperties(ParameterModelPtrs parameterModelPtrs,
				const UnitIdUnitParameters& unitIdUnitParameters);
			QVector<UnitProperty> GetParameterModelUnitProperties(ParameterModelPtr parameterModelPtr,
				ParameterModelPtrs parameterModelPtrs, const UnitParameters& unitParameters);
			QString GetUnitId(PropertiesId propertiesId, PropertiesIdParameterModelPtrs propertiesIdParameterModelPtrs,
				const UnitIdUnitParameters& unitIdUnitParameters);
			QString GetUnitCategory(PropertiesId propertiesId, PropertiesIdParameterModelPtrs propertiesIdParameterModelPtrs,
				const UnitIdUnitParameters& unitIdUnitParameters);
			QVector<Analyse::UnitDependency> GetParameterModelsDependencies(ParameterModelPtrs parameterModelPtrs,
				FileIdParameterModelPtrs fileIdParametersModelPtrs, const UnitIdUnitParameters& unitIdUnitParameters);
		}
	}
}