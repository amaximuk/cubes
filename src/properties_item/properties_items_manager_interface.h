#pragma once

namespace CubesProperties
{
	class PropertiesItem;

	class IPropertiesItemsManagerBoss
	{
	public:
		virtual ~IPropertiesItemsManagerBoss() = default;
		virtual void AfterNameChanged(const uint32_t propertiesId) = 0;
		virtual void AfterFileNameChanged(const uint32_t propertiesId, QStringList& includeNames) = 0;
		virtual void AfterIncludeNameChanged(const uint32_t propertiesId) = 0;
		//virtual void AfterIncludeNameChanged(const uint32_t propertiesId, QList<QPair<QString, QString>>& variables) = 0;
		virtual void AfterPositionChanged(const uint32_t propertiesId, double posX, double posY, double posZ) = 0;
		virtual void AfterError(const uint32_t propertiesId, const QString& message) = 0;
		virtual void AfterConnectionChanged(const uint32_t propertiesId) = 0;
	};

	class IPropertiesItemsManagerWorker
	{
	public:
		virtual ~IPropertiesItemsManagerWorker() = default;
	};
}
