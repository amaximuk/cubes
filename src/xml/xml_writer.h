#pragma once

#include <QtXml>
#include "xml_types.h"

namespace CubesXml
{
	class Writer
	{
	public:
		static bool Write(const QString& filename, const File& fi);
		static int GetItemsCount(Unit& unit, const QString& id);
		static Param* GetParam(Unit& unit, const QString& id);
		static Item* GetItem(Unit& unit, const QString& id);

	private:
		static bool SetFile(const File& file, QDomDocument& document);
		static bool SetIncludes(const QList<Include>& includes, QDomDocument& document);
		static bool SetConfig(const Config& config, QDomDocument& document);
		static bool SetNetworking(const Networking& networking, QDomElement& node);
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
