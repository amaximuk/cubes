#pragma once

#include <QtXml>
#include "xml_types.h"

namespace CubesXml
{
	class Parser
	{
	public:
		static bool Parse(const QString& filename, File& fi);
		static int GetItemsCount(Unit& unit, const QString& id);
		static Param* GetParam(Unit& unit, const QString& id);
		static Item* GetItem(Unit& unit, const QString& id);

	private:
		static bool GetFile(const QDomElement& node, File& file);
		static bool GetIncludes(const QDomElement& node, QList<Include>& includes);
		static bool GetConfig(const QDomElement& node, Config& config);
		static bool GetNetworking(const QDomElement& node, Networking& networking);
		static bool GetLog(const QDomElement& node, Log& log);
		static bool GetUnits(const QDomElement& node, QList<Group>& groups);
		static bool GetGroup(const QDomElement& node, Group& group);
		static bool GetUnit(const QDomElement& node, Unit& unit);
		static bool GetParam(const QDomElement& node, Param& param);
		static bool GetArray(const QDomElement& node, Array& array);
		static bool GetDepends(const QDomElement& node, QList<QString>& depends);
		static bool GetItem(const QDomElement& node, const QString& type, Item& item);

		static QList<QDomElement> ElementsByTagName(const QDomElement& node, const QString& tagname);
	};
}