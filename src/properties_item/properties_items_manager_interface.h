#pragma once

#include <QString>
#include <QList>

namespace CubesProperties
{
	class properties_item;

	class IPropertiesItemsManager
	{
	public:
		virtual void AfterNameChanged(properties_item* item) = 0;
		virtual void AfterFileNameChanged(properties_item* item, QStringList& includeNames) = 0;
		virtual void AfterIncludeNameChanged(properties_item* item, QList<QPair<QString, QString>>& variables) = 0;
	};
}
