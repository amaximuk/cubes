#pragma once

#include "../analysis/analysis_types.h"

namespace CubesAnalysis
{
	class IAnalysisManager
	{
	public:
		virtual ~IAnalysisManager() = default;

	public:
		virtual void AfterFileError(const CubesUnitTypes::FileId fileId,
			CubesAnalysis::RuleId id, const QString& message) = 0;
		virtual void AfterPropertiesError(const CubesUnitTypes::PropertiesId propertiesId,
			CubesAnalysis::RuleId id, const QString& message) = 0;
	};
}
