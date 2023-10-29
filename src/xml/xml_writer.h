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
		static bool SetLog(const Log& log, QDomElement& node);
		static bool SetUnits(const QList<Group>& groups, QDomElement& node);
		static bool SetGroup(const Group& group, QDomElement& node);
		static bool SetUnit(const Unit& unit, QDomElement& node);
		static bool SetParam(const Param& param, QDomElement& node);
		static bool SetArray(const Array& array, QDomElement& node);
		static bool SetDepends(const QList<QString>& depends, QDomElement& node);
		static bool SetItem(const Item& item, QDomElement& node);

		static QList<QDomElement> ElementsByTagName(const QDomElement& node, const QString& tagname);
	};
}
