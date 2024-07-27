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
			ParameterModelConstPtr GetParameterModelPtr(ParameterModelConstPtrs parameterModelConstPtrs,
				const ParameterModelId& parameterModelId);
		}

		namespace File
		{
			bool GetIncludes(ParameterModelConstPtrs parameterModelConstPtrs, IncludeIdNames& includeIdNames);
			IncludeIdNames GetIncludes(ParameterModelConstPtrs parameterModelConstPtrs);
			bool GetIncludeName(ParameterModelConstPtrs parameterModelConstPtrs, const IncludeId includeId, QString& includeName);
			QString GetIncludeName(ParameterModelConstPtrs parameterModelConstPtrs, const IncludeId includeId);
			bool GetIncludePath(ParameterModelConstPtrs parameterModelConstPtrs, const IncludeId includeId, QString& includeName);
			QString GetIncludePath(ParameterModelConstPtrs parameterModelConstPtrs, const IncludeId includeId);
			bool GetIncludeVariables(ParameterModelConstPtrs parameterModelConstPtrs, const IncludeId includeId,
				VariableIdVariables& variableIdVariables);
			VariableIdVariables GetIncludeVariables(ParameterModelConstPtrs parameterModelConstPtrs, const IncludeId includeId);
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

			UnitName GetResolvedUnitName(ParameterModelConstPtrs parameterModelConstPtrs,
				FileIdParameterModelPtrs fileIdParametersModelPtrs);
			PropertiesIdUnitNames GetResolvedUnitNames(PropertiesIdParameterModelPtrs propertiesIdParameterModelPtrs,
				FileIdParameterModelPtrs fileIdParametersModelPtrs);
			UnitName GetResolvedUnitName(ParameterModelConstPtrs parameterModelConstPtrs,
				FileIdParameterModelPtrs fileIdParametersModelPtrs, QString name);
			QVector<UnitProperty> GetParameterModelsUnitProperties(ParameterModelConstPtrs parameterModelConstPtrs,
				const UnitIdUnitParameters& unitIdUnitParameters);
			QVector<UnitProperty> GetParameterModelUnitProperties(ParameterModelPtr parameterModelPtr,
				ParameterModelConstPtrs parameterModelConstPtrs, const UnitParameters& unitParameters);
			QString GetUnitId(PropertiesId propertiesId, PropertiesIdParameterModelPtrs propertiesIdParameterModelPtrs,
				const UnitIdUnitParameters& unitIdUnitParameters);
			QString GetUnitCategory(PropertiesId propertiesId, PropertiesIdParameterModelPtrs propertiesIdParameterModelPtrs,
				const UnitIdUnitParameters& unitIdUnitParameters);
			QVector<Analyse::UnitDependency> GetParameterModelsDependencies(ParameterModelConstPtrs parameterModelConstPtrs,
				FileIdParameterModelPtrs fileIdParametersModelPtrs, const UnitIdUnitParameters& unitIdUnitParameters);
		}
	}
}
