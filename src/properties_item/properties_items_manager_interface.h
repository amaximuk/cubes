#pragma once

namespace CubesProperties
{
	class PropertiesItem;

	class IPropertiesItemsManagerBoss
	{
	public:
		virtual void AfterNameChanged(PropertiesItem* item) = 0;
		virtual void AfterFileNameChanged(PropertiesItem* item, QStringList& includeNames) = 0;
		virtual void AfterIncludeNameChanged(PropertiesItem* item) = 0;
		//virtual void AfterIncludeNameChanged(PropertiesItem* item, QList<QPair<QString, QString>>& variables) = 0;
		virtual void AfterPositionChanged(PropertiesItem* item, double posX, double posY, double posZ) = 0;
		virtual void AfterError(PropertiesItem* item, const QString& message) = 0;
	};

	class IPropertiesItemsManagerWorker
	{
	public:
	};
}
