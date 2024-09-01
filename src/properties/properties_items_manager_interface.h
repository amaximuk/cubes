#pragma once

#include "../unit/unit_types.h"
#include "../diagram/diagram_item_types.h"

namespace CubesProperties { class PropertiesItem; }

namespace CubesProperties
{
	using BasePropertiesChangedDelegate = std::function<void(CubesUnit::PropertiesId, const QString&, CubesUnit::FileId, CubesUnit::IncludeId)>;
	using PositionChangedDelegate = std::function<void(CubesUnit::PropertiesId, double, double, double)>;
	using SizeChangedDelegate = std::function<void(CubesUnit::PropertiesId, QSizeF size)>;
	using TextChangedDelegate = std::function<void(CubesUnit::PropertiesId, QString text, uint32_t fontSize, bool showBorder,
		CubesDiagram::HorizontalAlignment horizontalAlignment, CubesDiagram::VerticalAlignment verticalAlignment)>;
	using SelectedItemChangedDelegate = std::function<void(CubesUnit::PropertiesId)>;
	using ErrorDelegate = std::function<void(CubesUnit::PropertiesId, const QString&)>;
	using ConnectionChangedDelegate = std::function<void(CubesUnit::PropertiesId)>;
	using PropertiesChangedDelegate = std::function<void(void)>;

	class IPropertiesItemsManager
	{
	public:
		virtual ~IPropertiesItemsManager() = default;

	public:
		virtual void AfterNameChanged(CubesUnit::PropertiesId propertiesId) = 0;
		virtual void AfterFileNameChanged(CubesUnit::PropertiesId propertiesId,
			CubesUnit::IncludeIdNames& includeNames) = 0;
		virtual void AfterIncludeNameChanged(CubesUnit::PropertiesId propertiesId) = 0;
		virtual void AfterPositionChanged(CubesUnit::PropertiesId propertiesId,
			double posX, double posY, double posZ) = 0;
		virtual void AfterSizeChanged(CubesUnit::PropertiesId propertiesId, QSizeF size) = 0;
		virtual void AfterTextChanged(CubesUnit::PropertiesId propertiesId, QString text, uint32_t fontSize, bool showBorder,
			CubesDiagram::HorizontalAlignment horizontalAlignment, CubesDiagram::VerticalAlignment verticalAlignment) = 0;
		virtual void AfterConnectionChanged(CubesUnit::PropertiesId propertiesId) = 0;
		virtual void AfterPropertiesChanged() = 0;
	};
}
