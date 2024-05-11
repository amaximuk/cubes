#pragma once

#include "../unit_types.h"

namespace CubesProperties { class PropertiesItem; }

namespace CubesProperties
{
	using BasePropertiesChangedDelegate = std::function<void(CubesUnitTypes::PropertiesId, const QString&, CubesUnitTypes::FileId, CubesUnitTypes::IncludeId)>;
	using PositionChangedDelegate = std::function<void(CubesUnitTypes::PropertiesId, double, double, double)>;
	using SelectedItemChangedDelegate = std::function<void(CubesUnitTypes::PropertiesId)>;
	using ErrorDelegate = std::function<void(CubesUnitTypes::PropertiesId, const QString&)>;
	using ConnectionChangedDelegate = std::function<void(CubesUnitTypes::PropertiesId)>;
	using PropertiesChangedDelegate = std::function<void(void)>;

	class IPropertiesItemsManager
	{
	public:
		virtual ~IPropertiesItemsManager() = default;

	public:
		virtual void AfterNameChanged(const CubesUnitTypes::PropertiesId propertiesId) = 0;
		virtual void AfterFileNameChanged(const CubesUnitTypes::PropertiesId propertiesId,
			CubesUnitTypes::IncludeIdNames& includeNames) = 0;
		virtual void AfterIncludeNameChanged(const CubesUnitTypes::PropertiesId propertiesId) = 0;
		virtual void AfterPositionChanged(const CubesUnitTypes::PropertiesId propertiesId,
			double posX, double posY, double posZ) = 0;
		virtual void AfterError(const CubesUnitTypes::PropertiesId propertiesId, const QString& message) = 0;
		virtual void AfterConnectionChanged(const CubesUnitTypes::PropertiesId propertiesId) = 0;
		virtual void AfterPropertiesChanged() = 0;
	};
}
