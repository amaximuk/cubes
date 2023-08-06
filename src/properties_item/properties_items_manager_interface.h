#pragma once

#include <QString>

namespace PropertiesItem
{
	class properties_item;

	class properties_items_manager_interface
	{
	public:
		virtual void AfterNameChanged(properties_item* item) = 0;
		virtual void AfterFileNameChanged(properties_item* item, QStringList& includeNames) = 0;
		virtual void AfterIncludeNameChanged(properties_item* item, QList<QPair<QString, QString>>& variables) = 0;
	};
}
