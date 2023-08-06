#pragma once

#include <QString>
#include <QList>

namespace CubesProperties
{
	class PropertiesItem;

	class IPropertiesItemsManager
	{
	public:
		virtual void AfterNameChanged(PropertiesItem* item) = 0;
		virtual void AfterFileNameChanged(PropertiesItem* item, QStringList& includeNames) = 0;
		virtual void AfterIncludeNameChanged(PropertiesItem* item, QList<QPair<QString, QString>>& variables) = 0;
	};
}
