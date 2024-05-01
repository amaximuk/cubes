#pragma once

namespace CubesAnalysis
{
	class IAnalysisManager
	{
	public:
		virtual ~IAnalysisManager() = default;

	public:
		virtual void AfterFileError(const CubesUnitTypes::FileId fileId, const QString& message) = 0;
		virtual void AfterPropertiesError(const CubesUnitTypes::PropertiesId propertiesId, const QString& message) = 0;
	};
}
