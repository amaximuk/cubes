#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <regex>
#include <QTextStream>
#include "../properties_item/properties_item_types.h"
#include "../properties_item/properties_item.h"
#include "../unit_types.h"
#include "xml_parser.h"

using namespace CubesXml;

// !!! remove -> last error???
#define ELRF(message) do { std::cout << message << std::endl; return false; } while(0)

const CubesUnitTypes::ParameterModelIds Parser::ids_;

bool Parser::Parse(const QString& filename, File& fi)
{
	fi.fileName = filename;
	//fi.fileName = QFileInfo(filename).fileName();

	//QFile xmlFile(filename);
	//if (!xmlFile.open(QIODevice::ReadOnly))
	//{
	//	ELRF("File " << filename.toStdString() << " load failed");
	//}
	QFile xmlFile(filename);
	if (!xmlFile.open(QIODevice::ReadOnly | QFile::Text))
	{
		ELRF("File " << filename.toStdString() << " load failed");
	}

	QTextStream in(&xmlFile);
	QString xmlText = in.readAll();

	int pos = xmlText.indexOf("<Includes");
	if (pos == -1)
		pos = xmlText.indexOf("<Config");
	if (pos != -1)
	{
		xmlText.insert(pos, "<FakeRoot>\n");
		xmlText.append("\n</FakeRoot>");
	}

	QDomDocument doc;
	//doc.setContent(&xmlFile);
	doc.setContent(xmlText);
	xmlFile.close();

	QDomElement root = doc.documentElement();

	//if (root.tagName() != "Config")
	//	ELRF("File have no Config or doc malformed");

	if (!GetFile(root, fi))
		ELRF("File info parse failed");

	return true;
}

bool Parser::GetFile(const QDomElement& node, File& fi)
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
				if (!GetIncludes(ne, fi.includes))
					ELRF("Get Includes failed");
			}
			else if (ne.tagName() == "Config")
			{
				QString name;
				QString platform;
				if (!GetConfig(ne, name, platform, fi.config))
					ELRF("Get Config failed");
				fi.name = name;
				fi.platform = platform;
			}
		}
		n = n.nextSibling();
	}

	return true;
}

bool Parser::GetIncludes(const QDomElement& node, QList<Include>& includes)
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

			QMap<QString, QString> variables;
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

					if (variables.contains(v_name))
						ELRF("Includes/Include/Variable name duplicate");

					variables[v_name] = v_val;
				}
				v = v.nextSibling();
			}
			for (const auto& variable : variables.toStdMap())
				include.variables.push_back(QPair<QString, QString>(variable.first, variable.second));
			includes.push_back(std::move(include));
		}
		i = i.nextSibling();
	}

	return true;
}

bool Parser::GetConfig(const QDomElement& node, QString& name, QString& platform, Config& config)
{
	name = node.attribute("_name", "");
	platform = node.attribute("_platform", "");

	QDomNode i = node.firstChild();
	while (!i.isNull())
	{
		QDomElement ei = i.toElement();
		if (!ei.isNull())
		{
			if (ei.tagName() == "Networking")
			{
				config.networkingIsSet = true;
				if (!GetNetworking(ei, config.networking))
					ELRF("Get Networking failed");
			}
			else if (ei.tagName() == "Log")
			{
				config.logIsSet = true;
				if (!GetLog(ei, config.log))
					ELRF("Get Log failed");
			}
			else if (ei.tagName() == "Units")
			{
				if (!GetUnits(ei, config.groups))
					ELRF("Get Units failed");
			}
			else
				ELRF("Config have unknown child");
		}
		i = i.nextSibling();
	}

	return true;
}

bool Parser::GetNetworking(const QDomElement& node, Networking& networking)
{
	QString id = node.attribute("id", "");
	QString accept_port = node.attribute("accept_port", "");
	QString keep_alive_sec = node.attribute("keep_alive_sec", "");
	QString time_client = node.attribute("time_client", QString("%1").arg(CubesXml::Networking::Defaults().timeClient));
	QString network_threads = node.attribute("network_threads", QString("%1").arg(CubesXml::Networking::Defaults().networkThreads));
	QString broadcast_threads = node.attribute("broadcast_threads", QString("%1").arg(CubesXml::Networking::Defaults().broadcastThreads));
	QString clients_threads = node.attribute("clients_threads", QString("%1").arg(CubesXml::Networking::Defaults().clientsThreads));
	QString notify_ready_clients = node.attribute("notify_ready_clients", QString("%1").arg(CubesXml::Networking::Defaults().notifyReadyClients));
	QString notify_ready_servers = node.attribute("notify_ready_servers", QString("%1").arg(CubesXml::Networking::Defaults().notifyReadyServers));

	if (id == "")
		ELRF("Networking id is empty");
	if (accept_port == "")
		ELRF("Networking accept_port is empty");
	if (keep_alive_sec == "")
		ELRF("Networking keep_alive_sec is empty");

	networking.id = id.toInt();
	networking.acceptPort = accept_port.toInt();
	networking.keepAliveSec = keep_alive_sec.toInt();
	networking.timeClient = QVariant(time_client).toBool();
	networking.networkThreads = network_threads.toInt();
	networking.broadcastThreads = broadcast_threads.toInt();
	networking.clientsThreads = clients_threads.toInt();
	networking.notifyReadyClients = QVariant(notify_ready_clients).toBool();
	networking.notifyReadyServers = QVariant(notify_ready_servers).toBool();

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
			connect.ip = v_ip;
			connect.port = v_port.toInt();

			networking.connects.push_back(std::move(connect));
		}
		v = v.nextSibling();
	}

	return true;
}

bool Parser::GetLog(const QDomElement& node, Log& log)
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

			if (name == "LoggingLevel" && type == "int")
			{
				QString val = ei.attribute("val", QString("%1").arg(CubesXml::Log::Defaults().loggingLevel));
				log.loggingLevel = val.toInt();
			}
			else if (name == "TOTAL_LOG_LIMIT_MB" && type == "int")
			{
				QString val = ei.attribute("val", QString("%1").arg(CubesXml::Log::Defaults().totalLogLimit));
				log.totalLogLimit = val.toInt();
			}
			else if (name == "LogDir" && type == "str")
			{
				QString val = ei.attribute("val", QString("%1").arg(CubesXml::Log::Defaults().logDir));
				log.logDir = val;
			}
			else
				ELRF("Log/Param is unknown");
		}
		i = i.nextSibling();
	}

	return true;
}

bool Parser::GetUnits(const QDomElement& node, QList<Group>& groups)
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
			if (!GetGroup(ei, group))
				ELRF("Get Group failed");

			groups.push_back(std::move(group));
		}
		i = i.nextSibling();
	}

	return true;
}

bool Parser::GetGroup(const QDomElement& node, Group& group)
{
	auto paramNodes = ElementsByTagName(node, "Param");

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

	auto unitNodes = ElementsByTagName(node, "Unit");
	for (const auto& eu : unitNodes)
	{
		Unit unit{};
		if (!GetUnit(eu, unit))
			ELRF("Get Unit failed");
		group.units.push_back(std::move(unit));
	}

	return true;
}

bool Parser::GetUnit(const QDomElement& node, Unit& unit)
{
	QString name = node.attribute("Name", "");
	QString id = node.attribute("Id", "");
	int32_t x = node.attribute("_x", "0").toInt();
	int32_t y = node.attribute("_y", "0").toInt();
	int32_t z = node.attribute("_z", "0").toInt();

	if (name == "")
		ELRF("Unit Name is empty");
	if (id == "")
		ELRF("Unit Id is empty");

	unit.name = name;
	unit.id = id;
	unit.x = x;
	unit.y = y;
	unit.z = z;

	auto paramNodes = ElementsByTagName(node, "Param");
	for (const auto& ep : paramNodes)
	{
		Param param{};
		if (!GetParam(ep, param))
			ELRF("Get Param failed");
		unit.params.push_back(std::move(param));
	}

	auto arrayNodes = ElementsByTagName(node, "Array");
	for (const auto& ea : arrayNodes)
	{
		Array array{};
		if (!GetArray(ea, array))
			ELRF("Get Array failed");
		unit.arrays.push_back(std::move(array));
	}

	// Интерпретируем зависимости юнита как массив строк
	// При загрузке параметров автоматически добавляется массив строк с именем DEPENDS
	// Значения для зависимостей хранятся в поле name вместо val, учитываем это при загрузке

	auto dependsNodes = ElementsByTagName(node, "Depends");
	for (const auto& ed : dependsNodes)
	{
		QList<QString> depends;
		if (!GetDepends(ed, depends))
			ELRF("Get Depends failed");

		Array array{};
		array.name = "DEPENDS";
		array.type = "str";
		QList<Item> items;
		for (const auto& d : depends)
		{
			Item item{};
			item.val = d;
			items.push_back(item);
		}
		array.items = items;
		unit.arrays.push_back(std::move(array));
	}

	return true;
}

bool Parser::GetParam(const QDomElement& node, Param& param)
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

bool Parser::GetArray(const QDomElement& node, Array& array)
{
	QString name = node.attribute("name", "");
	QString type = node.attribute("type", "");

	if (name == "")
		ELRF("Unit/Param name is empty");
	// type is optional

	array.name = name;
	array.type = type;

	auto itemNodes = ElementsByTagName(node, "Item");
	for (const auto& ei : itemNodes)
	{
		Item item;
		if (!GetItem(ei, item))
			ELRF("Get Item failed");
		array.items.push_back(std::move(item));
	}

	return true;
}

bool Parser::GetDepends(const QDomElement& node, QList<QString>& depends)
{
	auto itemNodes = ElementsByTagName(node, "Item");
	for (const auto& ei : itemNodes)
	{
		QString name = ei.attribute("name", "");

		if (name == "")
			ELRF("Unit/Depends/Item name is empty");

		depends.push_back(name);
	}

	return true;
}

bool Parser::GetItem(const QDomElement& node, Item& item)
{
	QString name = node.attribute("_name", "");
	int32_t x = node.attribute("_x", "0").toInt();
	int32_t y = node.attribute("_y", "0").toInt();
	int32_t z = node.attribute("_z", "0").toInt();

	item.name = name;
	item.x = x;
	item.y = y;
	item.z = z;

	QString val = node.attribute("val", "");
	item.val = val;

	auto paramNodes = ElementsByTagName(node, "Param");
	for (const auto& ep : paramNodes)
	{
		Param param{};
		if (!GetParam(ep, param))
			ELRF("Get Param failed");
		item.params.push_back(std::move(param));
	}

	auto arrayNodes = ElementsByTagName(node, "Array");
	for (const auto& ea : arrayNodes)
	{
		Array array{};
		if (!GetArray(ea, array))
			ELRF("Get Array failed");
		item.arrays.push_back(std::move(array));
	}

	return true;
}

QList<QDomElement> Parser::ElementsByTagName(const QDomElement& node, const QString& tagname)
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

int Parser::GetItemsCount(Unit& unit, const CubesUnitTypes::ParameterModelId& id)
{
	auto ss = id.split();
	if (ss.size() < 2)
		return false;
	if (ss.front() != ids_.parameters)
		return false;
	ss.pop_front();

	bool inside_array = false;
	Array* array = nullptr;
	QList<Param>* params = &unit.params;
	QList<Array>* arrays = &unit.arrays;
	while (ss.size() > 0)
	{
		const auto& s = ss.front();
		if (inside_array)
		{
			auto index = ids_.ItemIndex(s);
			if (index != -1 && array->items.size() > index)
			{
				params = &array->items[index].params;
				arrays = &array->items[index].arrays;
			}
			else
				return -1;

			//if (s.startsWith(CubesUnitTypes::ParameterModelIds::Defaults().itemGroupName) && s.size() > 4)
			//{
			//	int index = s.mid(5).toInt();
			//	if (array->items.size() > index)
			//	{
			//		params = &array->items[index].params;
			//		arrays = &array->items[index].arrays;
			//	}
			//	else
			//		return -1;
			//}
			//else
			//	return -1;

			array = nullptr;
			inside_array = false;
		}
		else
		{
			for (auto& p : *params)
			{
				if (s == p.name)
				{
					return -1;
				}
			}

			for (auto& a : *arrays)
			{
				if (s == a.name)
				{
					array = &a;
					inside_array = true;
					break;
				}
			}
		}
		ss.pop_front();
	}
	
	if (array != nullptr)
		return array->items.size();

	return -1;
}

Param* Parser::GetParam(Unit& unit, const CubesUnitTypes::ParameterModelId& id)
{
	auto ss = id.split();
	if (ss.size() < 2)
		return false;
	if (ss.front() != ids_.parameters)
		return false;
	ss.pop_front();

	bool inside_array = false;
	Array* array = nullptr;
	QList<Param>* params = &unit.params;
	QList<Array>* arrays = &unit.arrays;
	while (ss.size() > 0)
	{
		const auto& s = ss.front();
		if (inside_array)
		{
			auto index = ids_.ItemIndex(s);
			if (index != -1 && array->items.size() > index)
			{
				params = &array->items[index].params;
				arrays = &array->items[index].arrays;
			}
			else
				return nullptr;

			//if (s.startsWith(CubesUnitTypes::ParameterModelIds::Defaults().itemGroupName) &&
			//	s.size() > CubesUnitTypes::ParameterModelIds::Defaults().itemGroupName.size() + 1)
			//{
			//	int index = s.mid(CubesUnitTypes::ParameterModelIds::Defaults().itemGroupName.size() + 1).toInt();
			//	if (array->items.size() > index)
			//	{
			//		params = &array->items[index].params;
			//		arrays = &array->items[index].arrays;
			//	}
			//	else
			//		return nullptr;
			//}
			//else
			//	return nullptr;

			array = nullptr;
			inside_array = false;
		}
		else
		{
			for (auto& p : *params)
			{
				if (s == p.name)
				{
					if (ss.size() != 1)
						return nullptr;
					return &p;
				}
			}

			for (auto& a : *arrays)
			{
				if (s == a.name)
				{
					array = &a;
					inside_array = true;
					break;
				}
			}
		}
		ss.pop_front();
	}
	return nullptr;
}

Item* Parser::GetItem(Unit& unit, const CubesUnitTypes::ParameterModelId& id, QString& type)
{
	auto ss = id.split();
	if (ss.size() < 2)
		return false;
	if (ss.front() != ids_.parameters)
		return false;
	ss.pop_front();

	bool inside_array = false;
	Array* array = nullptr;
	QString array_type;
	QList<Param>* params = &unit.params;
	QList<Array>* arrays = &unit.arrays;
	while (ss.size() > 0)
	{
		const auto& s = ss.front();
		if (inside_array)
		{
			auto index = ids_.ItemIndex(s);
			if (index != -1 && array->items.size() > index)
			{
				if (ss.size() == 1)
				{
					type = array_type;
					return &array->items[index];
				}

				params = &array->items[index].params;
				arrays = &array->items[index].arrays;
			}
			else
				return nullptr;

			//if (s.startsWith(CubesUnitTypes::ParameterModelIds::Defaults().itemGroupName) &&
			//	s.size() > CubesUnitTypes::ParameterModelIds::Defaults().itemGroupName.size() + 1)
			//{
			//	int index = s.mid(CubesUnitTypes::ParameterModelIds::Defaults().itemGroupName.size() + 1).toInt();
			//	if (array->items.size() > index)
			//	{
			//		if (ss.size() == 1)
			//		{
			//			type = array_type;
			//			return &array->items[index];
			//		}
			//		params = &array->items[index].params;
			//		arrays = &array->items[index].arrays;
			//	}
			//	else
			//		return nullptr;
			//}
			//else
			//	return nullptr;

			array = nullptr;
			inside_array = false;
		}
		else
		{
			for (auto& a : *arrays)
			{
				if (s == a.name)
				{
					array = &a;
					inside_array = true;
					array_type = a.type;
					break;
				}
			}
		}
		ss.pop_front();
	}
	return nullptr;
}

//QList<QString> parser::getConnections(Unit u)
//{
//	return {};
//}
//
//QList<QString> parser::getDependencies(Unit u)
//{
//	return {};
//}
