#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <regex>
#include <QTextStream>
#include "xml_writer.h"

using namespace CubesXml;

// !!! remove -> last error???
#define ELRF(message) do { std::cout << message << std::endl; return false; } while(0)

bool Writer::Write(const QString& filename, const File& fi)
{
	QDomDocument document{};
	if (!SetFile(fi, document))
		ELRF("File info set failed");

	QFile xmlFile(filename);
	if (!xmlFile.open(QIODevice::WriteOnly | QFile::Text))
	{
		ELRF("File " << filename.toStdString() << " create failed");
	}
	QTextStream xmlContent(&xmlFile);
	xmlContent << "<?xml version=\"1.0\" encoding=\"windows-1251\"?>\n";
	xmlContent << document.toString(4);
	xmlFile.close();

	return true;
}

bool Writer::SetFile(const File& file, QDomDocument& document)
{
	//<Includes>
	//	<Include val="include1.xml">
	//		<Variable name="BoxId" val="1808310"/>
	//	</Include>
	//</Includes>
	
	if (!SetIncludes(file.includes, document))
		ELRF("Set Includes failed");

	QDomElement elementRoot = document.createElement("Config");

	if (!SetConfig(file.config, document))
		ELRF("Set Config failed");

	//if (file.includes.size() > 0)
	//{
	//	QDomElement elementIncludes = document.createElement("Includes");

	//	for (const auto& include : file.includes)
	//	{
	//		QDomElement elementInclude = document.createElement("Include");;
	//		elementInclude.setAttribute("val", include.fileName);
	//		for (const auto& variable : include.variables.toStdMap())
	//		{
	//			QDomElement elementVariable = document.createElement("Variable");;
	//			elementVariable.setAttribute("name", variable.first);
	//			elementVariable.setAttribute("val", variable.second);
	//			elementInclude.appendChild(elementVariable);
	//		}
	//		elementIncludes.appendChild(elementInclude);
	//	}

	//	elementRoot.appendChild(elementIncludes);
	//}
	
	document.appendChild(elementRoot);


	//QDomNode n = node.firstChild();
	//while (!n.isNull())
	//{
	//	QDomElement ne = n.toElement();
	//	if (!ne.isNull())
	//	{
	//		qDebug() << ne.tagName();

	//		if (ne.tagName() == "Includes")
	//		{
	//			if (!GetIncludes(ne, fi.includes))
	//				ELRF("Get Includes failed");
	//		}
	//		else if (ne.tagName() == "Config")
	//		{
	//			if (!GetConfig(ne, fi.config))
	//				ELRF("Get Config failed");
	//		}
	//	}
	//	n = n.nextSibling();
	//}

	return true;
}

bool Writer::SetIncludes(const QList<Include>& includes, QDomDocument& document)
{
	if (includes.size() > 0)
	{
		QDomElement elementIncludes = document.createElement("Includes");

		for (const auto& include : includes)
		{
			QDomElement elementInclude = document.createElement("Include");;
			elementInclude.setAttribute("val", include.fileName);
			for (const auto& variable : include.variables)
			{
				QDomElement elementVariable = document.createElement("Variable");;
				elementVariable.setAttribute("name", variable.first);
				elementVariable.setAttribute("val", variable.second);
				elementInclude.appendChild(elementVariable);
			}
			elementIncludes.appendChild(elementInclude);
		}

		document.appendChild(elementIncludes);
	}

	//QDomNode i = node.firstChild();
	//while (!i.isNull())
	//{
	//	QDomElement ei = i.toElement();
	//	if (!ei.isNull())
	//	{
	//		if (ei.tagName() != "Include")
	//			ELRF("Includes have unknown child");

	//		Include include{};

	//		QString i_val = ei.attribute("val", "");
	//		if (i_val == "")
	//			ELRF("Includes/Include val is empty");
	//		include.fileName = i_val;

	//		QDomNode v = i.firstChild();
	//		while (!v.isNull())
	//		{
	//			QDomElement ev = v.toElement();
	//			if (!ev.isNull())
	//			{
	//				if (ev.tagName() != "Variable")
	//					ELRF("Includes/Include have unknown child");

	//				QString v_name = ev.attribute("name", "");
	//				QString v_val = ev.attribute("val", "");

	//				if (v_name == "")
	//					ELRF("Includes/Include/Variable name is empty");
	//				if (v_val == "")
	//					ELRF("Includes/Include/Variable val is empty");

	//				if (include.variables.contains(v_name))
	//					ELRF("Includes/Include/Variable name duplicate");

	//				include.variables[v_name] = v_val;
	//			}
	//			v = v.nextSibling();
	//		}
	//		includes.push_back(std::move(include));
	//	}
	//	i = i.nextSibling();
	//}

	return true;
}

bool Writer::SetConfig(const Config& config, QDomDocument& document)
{
	QDomElement elementConfig = document.createElement("Config");

	if (config.networkingIsSet)
	{
		if (!SetNetworking(config.networking, elementConfig))
			ELRF("Set Networking failed");
	}

	document.appendChild(elementConfig);

	//QDomNode i = node.firstChild();
	//while (!i.isNull())
	//{
	//	QDomElement ei = i.toElement();
	//	if (!ei.isNull())
	//	{
	//		if (ei.tagName() == "Networking")
	//		{
	//			config.networkingIsSet = true;
	//			if (!GetNetworking(ei, config.networking))
	//				ELRF("Get Networking failed");
	//		}
	//		else if (ei.tagName() == "Log")
	//		{
	//			config.logIsSet = true;
	//			if (!GetLog(ei, config.log))
	//				ELRF("Get Log failed");
	//		}
	//		else if (ei.tagName() == "Units")
	//		{
	//			if (!GetUnits(ei, config.groups))
	//				ELRF("Get Units failed");
	//		}
	//		else
	//			ELRF("Config have unknown child");
	//	}
	//	i = i.nextSibling();
	//}

	return true;
}

bool Writer::SetNetworking(const Networking& networking, QDomElement& node)
{
	//result.network.id = GetParameterModel("PARAMETERS/NETWORKING/ID")->value.toInt();
	//result.network.accept_port = GetParameterModel("PARAMETERS/NETWORKING/ACCEPT_PORT")->value.toInt();
	//result.network.keep_alive_sec = GetParameterModel("PARAMETERS/NETWORKING/KEEP_ALIVE_SEC")->value.toInt();
	//result.network.time_client = GetParameterModel("PARAMETERS/NETWORKING/TIME_CLIENT")->value.toBool();
	//result.network.network_threads = GetParameterModel("PARAMETERS/NETWORKING/NETWORK_THREADS")->value.toInt();
	//result.network.broadcast_threads = GetParameterModel("PARAMETERS/NETWORKING/BROADCAST_THREADS")->value.toInt();
	//result.network.clients_threads = GetParameterModel("PARAMETERS/NETWORKING/CLIENTS_THREADS")->value.toInt();
	//result.network.notify_ready_clients = GetParameterModel("PARAMETERS/NETWORKING/NOTIFY_READY_CLIENTS")->value.toBool();
	//result.network.notify_ready_servers = GetParameterModel("PARAMETERS/NETWORKING/NOTIFY_READY_SERVERS")->value.toBool();

	//int count = GetParameterModel("PARAMETERS/NETWORKING/CONNECT")->value.toInt();
	//for (int i = 0; i < count; i++)
	//{
	//	Connect connect{};
	//	connect.port = GetParameterModel(QString("PARAMETERS/NETWORKING/CONNECT/ITEM_%1/PORT").arg(i))->value.toInt();
	//	connect.ip = GetParameterModel(QString("PARAMETERS/NETWORKING/CONNECT/ITEM_%1/IP").arg(i))->value.toString();
	//	result.network.connect.push_back(connect);
	//}

	//result.log.level = static_cast<LoggingLevel>(GetParameterModel("PARAMETERS/LOG/LOGGING_LEVEL")->value.toInt());
	//result.log.limit_mb = GetParameterModel("PARAMETERS/LOG/TOTAL_LOG_LIMIT_MB")->value.toInt();
	//result.log.directory_path = GetParameterModel("PARAMETERS/LOG/LOG_DIR")->value.toString();

	QDomElement elementNetworking = node.ownerDocument().createElement("Networking");
	elementNetworking.setAttribute("id", networking.id);
	elementNetworking.setAttribute("accept_port", networking.acceptPort);
	elementNetworking.setAttribute("keep_alive_sec", networking.keepAliveSec);
	if (networking.timeClient != NetworkingDefaults::timeClient)
		elementNetworking.setAttribute("time_client", networking.timeClient);
	if (networking.networkThreads != NetworkingDefaults::networkThreads)
		elementNetworking.setAttribute("network_threads", networking.networkThreads);
	if (networking.broadcastThreads != NetworkingDefaults::broadcastThreads)
		elementNetworking.setAttribute("broadcast_threads", networking.broadcastThreads);
	if (networking.clientsThreads != NetworkingDefaults::clientsThreads)
		elementNetworking.setAttribute("clients_threads", networking.clientsThreads);
	if (networking.notifyReadyClients != NetworkingDefaults::notifyReadyClients)
		elementNetworking.setAttribute("notify_ready_clients", networking.notifyReadyClients);
	if (networking.notifyReadyServers != NetworkingDefaults::notifyReadyServers)
		elementNetworking.setAttribute("notify_ready_servers", networking.notifyReadyServers);

	for (const auto& connect : networking.connects)
	{
		QDomElement elementConnect = node.ownerDocument().createElement("connect");
		elementConnect.setAttribute("port", connect.port);
		elementConnect.setAttribute("ip", connect.ip);
		elementNetworking.appendChild(elementConnect);
	}

	node.appendChild(elementNetworking);

	//QString id = node.attribute("id", "");
	//QString accept_port = node.attribute("accept_port", "");
	//QString keep_alive_sec = node.attribute("keep_alive_sec", "");

	//if (id == "")
	//	ELRF("Networking id is empty");
	//if (accept_port == "")
	//	ELRF("Networking accept_port is empty");
	//if (keep_alive_sec == "")
	//	ELRF("Networking keep_alive_sec is empty");

	//networking.id = id.toInt();
	//networking.acceptPort = accept_port.toInt();
	//networking.keepAlive = keep_alive_sec.toInt();

	//QDomNode v = node.firstChild();
	//while (!v.isNull())
	//{
	//	QDomElement ev = v.toElement();
	//	if (!ev.isNull())
	//	{
	//		if (ev.tagName() != "connect")
	//			ELRF("Networking have unknown child");

	//		QString v_port = ev.attribute("port", "");
	//		QString v_ip = ev.attribute("ip", "");

	//		if (v_port == "")
	//			ELRF("Networking/connect port is empty");
	//		if (v_ip == "")
	//			ELRF("Networking/connect ip is empty");

	//		Connect connect{};
	//		connect.host = v_ip;
	//		connect.port = v_port.toInt();

	//		networking.connects.push_back(std::move(connect));
	//	}
	//	v = v.nextSibling();
	//}

	return true;
}

bool Writer::GetLog(const QDomElement& node, Log& log)
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

bool Writer::GetUnits(const QDomElement& node, QList<Group>& groups)
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

bool Writer::GetGroup(const QDomElement& node, Group& group)
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

bool Writer::GetUnit(const QDomElement& node, Unit& unit)
{
	QString name = node.attribute("Name", "");
	QString id = node.attribute("Id", "");
	int32_t x = node.attribute("X", "0").toInt();
	int32_t y = node.attribute("Y", "0").toInt();
	int32_t z = node.attribute("Z", "0").toInt();

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

bool Writer::GetParam(const QDomElement& node, Param& param)
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

bool Writer::GetArray(const QDomElement& node, Array& array)
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
		if (!GetItem(ei, type, item))
			ELRF("Get Item failed");
		array.items.push_back(std::move(item));
	}

	return true;
}

bool Writer::GetDepends(const QDomElement& node, QList<QString>& depends)
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

bool Writer::GetItem(const QDomElement& node, const QString& type, Item& item)
{
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


	//if (type != "")
	//{
	//	QString val = node.attribute("val", "");

	//	if (val == "")
	//		ELRF("Unit/Param val is empty");

	//	item.val = val;
	//}
	//else
	//{
	//	auto paramNodes = elementsByTagName(node, "Param");
	//	for (const auto& ep : paramNodes)
	//	{
	//		Param param{};
	//		if (!get_param(ep, param))
	//			ELRF("Get Param failed");
	//		item.params.push_back(std::move(param));
	//	}

	//	auto arrayNodes = elementsByTagName(node, "Array");
	//	for (const auto& ea : arrayNodes)
	//	{
	//		Array array{};
	//		if (!get_array(ea, array))
	//			ELRF("Get Array failed");
	//		item.arrays.push_back(std::move(array));
	//	}
	//}

	return true;
}

QList<QDomElement> Writer::ElementsByTagName(const QDomElement& node, const QString& tagname)
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

int Writer::GetItemsCount(Unit& unit, const QString& id)
{
	QList<QString> ss = id.split("/");
	if (ss.size() < 2)
		return false;
	if (ss.front() != "PARAMETERS")
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
			if (s.startsWith("ITEM_") && s.size() > 4)
			{
				int index = s.mid(5).toInt();
				if (array->items.size() > index)
				{
					params = &array->items[index].params;
					arrays = &array->items[index].arrays;
				}
				else
					return -1;
			}
			else
				return -1;

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

Param* Writer::GetParam(Unit& unit, const QString& id)
{
	QList<QString> ss = id.split("/");
	if (ss.size() < 2)
		return false;
	if (ss.front() != "PARAMETERS")
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
			if (s.startsWith("ITEM_") && s.size() > 4)
			{
				int index = s.mid(5).toInt();
				if (array->items.size() > index)
				{
					params = &array->items[index].params;
					arrays = &array->items[index].arrays;
				}
				else
					return nullptr;
			}
			else
				return nullptr;

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

Item* Writer::GetItem(Unit& unit, const QString& id)
{
	QList<QString> ss = id.split("/");
	if (ss.size() < 2)
		return false;
	if (ss.front() != "PARAMETERS")
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
			if (s.startsWith("ITEM_") && s.size() > 4)
			{
				int index = s.mid(5).toInt();
				if (array->items.size() > index)
				{
					if (ss.size() == 1)
						return &array->items[index];
					params = &array->items[index].params;
					arrays = &array->items[index].arrays;
				}
				else
					return nullptr;
			}
			else
				return nullptr;

			array = nullptr;
			inside_array = false;
		}
		else
		{
			//for (auto& p : *params)
			//{
			//	if (s == p.name)
			//	{
			//		if (ss.size() != 1)
			//			return nullptr;
			//		return &p;
			//	}
			//}

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

//QList<QString> parser::getConnections(Unit u)
//{
//	return {};
//}
//
//QList<QString> parser::getDependencies(Unit u)
//{
//	return {};
//}
