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

	if (!SetConfig(file.config, document))
		ELRF("Set Config failed");

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

	if (config.logIsSet)
	{
		if (!SetLog(config.log, elementConfig))
			ELRF("Set Log failed");
	}

	if (!SetUnits(config.groups, elementConfig))
		ELRF("Set Log failed");

	document.appendChild(elementConfig);

	return true;
}

bool Writer::SetNetworking(const Networking& networking, QDomElement& node)
{
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

	return true;
}

bool Writer::SetLog(const Log& log, QDomElement& node)
{
	//<Log>
	//	<Param name="LoggingLevel" type="int" val="0"/>
	//	<Param name="TOTAL_LOG_LIMIT_MB" type="int" val="500"/>
	//</Log>

	QDomElement elementLog = node.ownerDocument().createElement("Log");

	QDomElement elementLoggingLevelParam = node.ownerDocument().createElement("Param");
	elementLoggingLevelParam.setAttribute("name", "LoggingLevel");
	elementLoggingLevelParam.setAttribute("type", "int");
	elementLoggingLevelParam.setAttribute("val", log.loggingLevel);
	elementLog.appendChild(elementLoggingLevelParam);

	QDomElement elementTotalLogLimitMbParam = node.ownerDocument().createElement("Param");
	elementTotalLogLimitMbParam.setAttribute("name", "TOTAL_LOG_LIMIT_MB");
	elementTotalLogLimitMbParam.setAttribute("type", "int");
	elementTotalLogLimitMbParam.setAttribute("val", log.totalLogLimit);
	elementLog.appendChild(elementTotalLogLimitMbParam);

	if (log.logDir != "")
	{
		QDomElement elementLogDirParam = node.ownerDocument().createElement("Param");
		elementLogDirParam.setAttribute("name", "LogDir");
		elementLogDirParam.setAttribute("type", "str");
		elementLogDirParam.setAttribute("val", log.logDir);
		elementLog.appendChild(elementLogDirParam);
	}

	node.appendChild(elementLog);

	return true;
}

bool Writer::SetUnits(const QList<Group>& groups, QDomElement& node)
{
	//<Units>
	//	<Group>
	//		<Param name="Path" type="str" val="handlers"/>
	//		<Unit Name="Sigma IQ Source" Id="sigma_iq_source" X="20" Y="40" Z="0">
	//		</Unit>
	//	</Group>
	//</Units>
	
	QDomElement elementUnits = node.ownerDocument().createElement("Units");
	for (const auto& group : groups)
	{
		if (!SetGroup(group, elementUnits))
			ELRF("Set Group failed");

		//QDomElement elementGroup = node.ownerDocument().createElement("Group");

		//QDomElement elementGroupParam = node.ownerDocument().createElement("Param");
		//elementGroupParam.setAttribute("name", "Path");
		//elementGroupParam.setAttribute("type", "str");
		//elementGroupParam.setAttribute("val", group.path);
		//elementGroup.appendChild(elementGroupParam);

		//if (!SetGroup(group, elementGroup))
		//	ELRF("Set Group failed");

		//elementUnits.appendChild(elementGroup);
	}

	node.appendChild(elementUnits);

	return true;
}

bool Writer::SetGroup(const Group& group, QDomElement& node)
{
	QDomElement elementGroup = node.ownerDocument().createElement("Group");

	QDomElement elementGroupParam = node.ownerDocument().createElement("Param");
	elementGroupParam.setAttribute("name", "Path");
	elementGroupParam.setAttribute("type", "str");
	elementGroupParam.setAttribute("val", group.path);
	elementGroup.appendChild(elementGroupParam);

	for (const auto& unit : group.units)
	{
		if (!SetUnit(unit, elementGroup))
			ELRF("Set Unit failed");
	}

	node.appendChild(elementGroup);

	return true;
}

bool Writer::SetUnit(const Unit& unit, QDomElement& node)
{
	//<Unit Name="Configurator Sigma" Id="sigma_poly" X="20" Y="140" Z="0">
	//	<Param name="DRIVER_NAME" type="str" val="Driver" depends="true"/>
	//	<Array name="CHANNELS">
	//		<Item>
	//			<Param name="RECEIVER_NAME" type="str" val="Receiver" depends="true"/>
	//		</Item>
	//	</Array>
	//</Unit>

	QDomElement elementUnit = node.ownerDocument().createElement("Unit");
	elementUnit.setAttribute("Name", unit.name);
	elementUnit.setAttribute("Id", unit.id);
	elementUnit.setAttribute("X", unit.x);
	elementUnit.setAttribute("Y", unit.y);
	elementUnit.setAttribute("Z", unit.z);

	for (const auto& param : unit.params)
	{
		if (!SetParam(param, elementUnit))
			ELRF("Set Param failed");
	}

	for (const auto& array : unit.arrays)
	{
		if (array.name != "DEPENDS")
		{
			if (!SetArray(array, elementUnit))
				ELRF("Set Param failed");
		}
	}

	for (const auto& array : unit.arrays)
	{
		if (array.name == "DEPENDS")
		{
			QList<QString> depends;
			for (const auto& depend : array.items)
				depends.push_back(depend.val);
			if (!SetDepends(depends, elementUnit))
				ELRF("Set Depends failed");
		}
	}

	node.appendChild(elementUnit);

	return true;
}

bool Writer::SetParam(const Param& param, QDomElement& node)
{
	//<Param name="DRIVER_NAME" type="str" val="Driver" depends="true"/>

	QDomElement elementParam = node.ownerDocument().createElement("Param");
	elementParam.setAttribute("name", param.name);
	elementParam.setAttribute("type", param.type);
	elementParam.setAttribute("val", param.val);
	if (param.depends)
		elementParam.setAttribute("depends", param.depends);

	node.appendChild(elementParam);

	return true;
}

bool Writer::SetArray(const Array& array, QDomElement& node)
{
	//<Array name="CHANNELS">
	//	<Item>
	//		<Param name="RECEIVER_NAME" type="str" val="Receiver" depends="true"/>
	//	</Item>
	//</Array>

	QDomElement elementArray = node.ownerDocument().createElement("Array");
	elementArray.setAttribute("name", array.name);
	if (array.type != "")
		elementArray.setAttribute("type", array.type);

	for (const auto& item : array.items)
	{
		if (!SetItem(item, elementArray))
			ELRF("Set Param failed");
	}

	node.appendChild(elementArray);

	return true;
}

bool Writer::SetDepends(const QList<QString>& depends, QDomElement& node)
{
	//<Depends>
	//	<Item name="FL_DB_PG_1"/>
	//	<Item name="LRK_1"/>
	//</Depends>

	QDomElement elementDepends = node.ownerDocument().createElement("Depends");

	for (const auto& item : depends)
	{
		QDomElement elementItem = node.ownerDocument().createElement("Item");
		elementItem.setAttribute("name", item);
		elementDepends.appendChild(elementItem);
	}

	node.appendChild(elementDepends);

	return true;
}

bool Writer::SetItem(const Item& item, QDomElement& node)
{
	//<Array name="BlockingStates" type="int">
	//	<Item val="0"/>
	//	<Item val="1"/>
	//</Array>

	QDomElement elementItem = node.ownerDocument().createElement("Item");

	if (item.params.size() == 0 && item.arrays.size() == 0)
	{
		elementItem.setAttribute("val", item.val);
	}
	else
	{
		for (const auto& param : item.params)
		{
			if (!SetParam(param, elementItem))
				ELRF("Set Param failed");
		}

		for (const auto& array : item.arrays)
		{
			if (!SetArray(array, elementItem))
				ELRF("Set Array failed");
		}
	}

	node.appendChild(elementItem);

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
