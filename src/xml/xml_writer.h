#pragma once

#include <QtXml>
#include "xml_types.h"

namespace CubesXml
{
	class Writer
	{
	public:
		static bool Write(const QString& filename, const File& fi);

	private:
		static bool SetFile(const File& file, QXmlStreamWriter& xmlWriter);
		static bool SetIncludes(const QList<Include>& includes, QXmlStreamWriter& xmlWriter);
		static bool SetConfig(const Config& config, const QString& name, const QString& platform,
			const QString& color, QXmlStreamWriter& xmlWriter);
		static bool SetNetworking(const Networking& networking, QXmlStreamWriter& xmlWriter);
		static bool SetLog(const Log& log, QXmlStreamWriter& xmlWriter);
		static bool SetUnits(const QList<Group>& groups, QXmlStreamWriter& xmlWriter);
		static bool SetGroup(const Group& group, QXmlStreamWriter& xmlWriter);
		static bool SetUnit(const Unit& unit, QXmlStreamWriter& xmlWriter);
		static bool SetParam(const Param& param, QXmlStreamWriter& xmlWriter);
		static bool SetArray(const Array& array, QXmlStreamWriter& xmlWriter);
		static bool SetDepends(const QList<QString>& depends, QXmlStreamWriter& xmlWriter);
		static bool SetItem(const Item& item, QXmlStreamWriter& xmlWriter);
	};
}
