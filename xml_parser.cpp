#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <regex>
#include <QTextStream>
#include "xml_parser.h"

using namespace xml;

// !!! remove -> last error???
#define ELRF(message) do { std::cout << message << std::endl; return false; } while(0)

bool parser::parse(const QString& filename, File& fi)
{
	QFile xmlFile(filename);
	if (!xmlFile.open(QIODevice::ReadOnly))
	{
		ELRF("File " << filename.toStdString() << " load failed");
	}
	QDomDocument doc;
	doc.setContent(&xmlFile);
	xmlFile.close();

	QDomElement root = doc.documentElement();

	if (root.tagName() != "Config")
		ELRF("File have no Config or doc malformed");

	if (!get_file(root, fi))
		ELRF("File info parse failed");

	return true;
}

bool parser::get_file(const QDomElement& node, File& fi)
{
	QDomNode n = node.firstChild();
	while (!n.isNull())
	{
		QDomElement ne = n.toElement();
		if (!ne.isNull())
		{
			qDebug() << ne.tagName();
			if (ne.tagName() == "Includes")
			{
				if (!get_includes(ne, fi.includes))
					ELRF("Get Includes failed");
			}
			else if (ne.tagName() == "Networking")
			{
				if (!get_networking(ne, fi.networking))
					ELRF("Get Networking failed");
			}
			else if (ne.tagName() == "Log")
			{
				if (!get_log(ne, fi.log))
					ELRF("Get Log failed");
			}
			else if (ne.tagName() == "Units")
			{
				if (!get_units(ne, fi.groups))
					ELRF("Get Units failed");
			}
			else
				ELRF("Config have unknown child");
		}
		n = n.nextSibling();
	}

	return true;
}

bool parser::get_includes(const QDomElement& node, QList<Include>& includes)
{
	QDomNode i = node.firstChild();
	while (!i.isNull())
	{
		QDomElement ei = i.toElement();
		if (!ei.isNull())
		{
			if (ei.tagName() != "Include")
				ELRF("Includes have unknown child");

			Include include{};

			QString i_val = ei.attribute("val", "");
			if (i_val == "")
				ELRF("Includes/Include val is empty");
			include.fileName = i_val;

			QDomNode v = i.firstChild();
			while (!v.isNull())
			{
				QDomElement ev = v.toElement();
				if (!ev.isNull())
				{
					if (ev.tagName() != "Variable")
						ELRF("Includes/Include have unknown child");

					QString v_name = ev.attribute("name", "");
					QString v_val = ev.attribute("val", "");

					if (v_name == "")
						ELRF("Includes/Include/Variable name is empty");
					if (v_val == "")
						ELRF("Includes/Include/Variable val is empty");

					if (include.variables.contains(v_name))
						ELRF("Includes/Include/Variable name duplicate");

					include.variables[v_name] = v_val;
				}
				v = v.nextSibling();
			}
			includes.push_back(std::move(include));
		}
		i = i.nextSibling();
	}

	return true;
}

bool parser::get_networking(const QDomElement& node, Networking& networking)
{
	QString id = node.attribute("id", "");
	QString accept_port = node.attribute("accept_port", "");
	QString keep_alive_sec = node.attribute("keep_alive_sec", "");

	if (id == "")
		ELRF("Networking id is empty");
	if (accept_port == "")
		ELRF("Networking accept_port is empty");
	if (keep_alive_sec == "")
		ELRF("Networking keep_alive_sec is empty");

	networking.id = id.toInt();
	networking.acceptPort = accept_port.toInt();
	networking.keepAlive = keep_alive_sec.toInt();

	QDomNode v = node.firstChild();
	while (!v.isNull())
	{
		QDomElement ev = v.toElement();
		if (!ev.isNull())
		{
			if (ev.tagName() != "connect")
				ELRF("Networking have unknown child");

			QString v_port = ev.attribute("port", "");
			QString v_ip = ev.attribute("ip", "");

			if (v_port == "")
				ELRF("Networking/connect port is empty");
			if (v_ip == "")
				ELRF("Networking/connect ip is empty");

			Connect connect{};
			connect.host = v_ip;
			connect.port = v_port.toInt();

			networking.connects.push_back(std::move(connect));
		}
		v = v.nextSibling();
	}

	return true;
}

bool parser::get_log(const QDomElement& node, Log& log)
{
	QDomNode i = node.firstChild();
	while (!i.isNull())
	{
		QDomElement ei = i.toElement();
		if (!ei.isNull())
		{
			if (ei.tagName() != "Param")
				ELRF("Log have unknown child");

			QString name = ei.attribute("name", "");
			QString type = ei.attribute("type", "");
			QString val = ei.attribute("val", "");

			if (name == "LoggingLevel" && type == "int")
				log.loggingLevel = val.toInt();
			else if (name == "TOTAL_LOG_LIMIT_MB" && type == "int")
				log.totalLogLimit = val.toInt();
			else
				ELRF("Log/Param is unknown");
		}
		i = i.nextSibling();
	}

	return true;
}

bool parser::get_units(const QDomElement& node, QList<Group>& groups)
{
	QDomNode i = node.firstChild();
	while (!i.isNull())
	{
		QDomElement ei = i.toElement();
		if (!ei.isNull())
		{
			if (ei.tagName() != "Group")
				ELRF("Units have unknown child");

			Group group{};
			if (!get_group(ei, group))
				ELRF("Get Group failed");

			groups.push_back(std::move(group));
		}
		i = i.nextSibling();
	}

	return true;
}

bool parser::get_group(const QDomElement& node, Group& group)
{
	auto paramNodes = elementsByTagName(node, "Param");

	if (paramNodes.size() != 1)
		ELRF("Group/Param not found or more then one");

	const auto& ep = paramNodes[0];
	QString name = ep.attribute("name", "");
	QString type = ep.attribute("type", "");
	QString val = ep.attribute("val", "");

	if (name == "Path" && type == "str")
		group.path = val;
	else
		ELRF("Group/Param is unknown");

	auto unitNodes = elementsByTagName(node, "Unit");
	for (const auto& eu : unitNodes)
	{
		Unit unit{};
		if (!get_unit(eu, unit))
			ELRF("Get Unit failed");
		group.units.push_back(std::move(unit));
	}

	return true;
}

bool parser::get_unit(const QDomElement& node, Unit& unit)
{
	QString name = node.attribute("Name", "");
	QString id = node.attribute("Id", "");

	if (name == "")
		ELRF("Unit Name is empty");
	if (id == "")
		ELRF("Unit Id is empty");

	unit.name = name;
	unit.id = id;

	auto paramNodes = elementsByTagName(node, "Param");
	for (const auto& ep : paramNodes)
	{
		Param param{};
		if (!get_param(ep, param))
			ELRF("Get Param failed");
		unit.params.push_back(std::move(param));
	}

	auto arrayNodes = elementsByTagName(node, "Array");
	for (const auto& ea : arrayNodes)
	{
		Array array{};
		if (!get_array(ea, array))
			ELRF("Get Array failed");
		unit.arrays.push_back(std::move(array));
	}

	auto dependsNodes = elementsByTagName(node, "Depends");
	for (const auto& ed : dependsNodes)
	{
		QList<QString> depends;
		if (!get_depends(ed, depends))
			ELRF("Get Depends failed");
		unit.depends.append(depends);
	}

	return true;
}

bool parser::get_param(const QDomElement& node, Param& param)
{
	QString name = node.attribute("name", "");
	QString type = node.attribute("type", "");
	QString val = node.attribute("val", "");
	QString depends = node.attribute("depends", "");

	if (name == "")
		ELRF("Unit/Param name is empty");
	if (type == "")
		ELRF("Unit/Param type is empty");
	if (val == "")
		ELRF("Unit/Param val is empty");
	// depends is optional

	param.name = name;
	param.type = type;
	param.val = val;
	param.depends = QVariant(depends).toBool();

	return true;
}

bool parser::get_array(const QDomElement& node, Array& array)
{
	QString name = node.attribute("name", "");
	QString type = node.attribute("type", "");

	if (name == "")
		ELRF("Unit/Param name is empty");
	// type is optional

	array.name = name;
	array.type = type;

	auto itemNodes = elementsByTagName(node, "Item");
	for (const auto& ei : itemNodes)
	{
		Item item;
		if (!get_item(ei, type, item))
			ELRF("Get Item failed");
		array.items.push_back(std::move(item));
	}

	return true;
}

bool parser::get_depends(const QDomElement& node, QList<QString>& depends)
{
	auto itemNodes = elementsByTagName(node, "Item");
	for (const auto& ei : itemNodes)
	{
		QString name = ei.attribute("name", "");

		if (name == "")
			ELRF("Unit/Depends/Item name is empty");

		depends.push_back(name);
	}

	return true;
}

bool parser::get_item(const QDomElement& node, const QString& type, Item& item)
{
	if (type != "")
	{
		QString val = node.attribute("val", "");

		if (val == "")
			ELRF("Unit/Param val is empty");

		item.val = val;
	}
	else
	{
		auto paramNodes = elementsByTagName(node, "Param");
		for (const auto& ep : paramNodes)
		{
			Param param{};
			if (!get_param(ep, param))
				ELRF("Get Param failed");
			item.params.push_back(std::move(param));
		}

		auto arrayNodes = elementsByTagName(node, "Array");
		for (const auto& ea : arrayNodes)
		{
			Array array{};
			if (!get_array(ea, array))
				ELRF("Get Array failed");
			item.arrays.push_back(std::move(array));
		}
	}

	return true;
}

QList<QDomElement> parser::elementsByTagName(const QDomElement& node, const QString& tagname)
{
	QList<QDomElement> list;

	QDomNode i = node.firstChild();
	while (!i.isNull())
	{
		QDomElement ei = i.toElement();
		if (!ei.isNull())
		{
			if (ei.tagName() == tagname)
				list.push_back(ei);
		}
		i = i.nextSibling();
	}

	return list;
}
