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
			UnitParametersPtr GetUnitParameters(const QString& unitId, UnitIdUnitParametersPtr unitIdUnitParametersPtr);
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
			QVector<UnitProperty> GetUnitProperties(ParameterModelPtrs parameterModelPtrs,
				UnitIdUnitParametersPtr unitIdUnitParametersPtr);
			QVector<UnitProperty> GetUnitProperties(ParameterModelPtr parameterModelPtr,
				ParameterModelPtrs parameterModelPtrs, UnitParametersPtr unitParametersPtr);
			QString GetUnitId(PropertiesId propertiesId, PropertiesIdParameterModelPtrs propertiesIdParameterModelPtrs,
				UnitIdUnitParametersPtr unitIdUnitParametersPtr);
			QString GetUnitCategory(PropertiesId propertiesId, PropertiesIdParameterModelPtrs propertiesIdParameterModelPtrs,
				UnitIdUnitParametersPtr unitIdUnitParametersPtr);
			QVector<Analyse::UnitDependency> GetUnitDependencies(ParameterModelPtrs parameterModelPtrs,
				FileIdParameterModelPtrs fileIdParametersModelPtrs, UnitIdUnitParametersPtr unitIdUnitParametersPtr);
		}
	}
}
