#pragma once

namespace CubesAnalysis
{
	class IAnalysisManager
	{
	public:
		virtual ~IAnalysisManager() = default;

	public:
		virtual void Test() = 0;
		virtual void Error(const CubesUnitTypes::PropertiesId propertiesId, const QString& message) = 0;
	};
}
