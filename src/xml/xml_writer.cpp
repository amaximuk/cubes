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
	QFile xmlFile(filename);
	if (!xmlFile.open(QIODevice::WriteOnly | QFile::Text))
	{
		ELRF("File " << filename.toStdString() << " create failed");
	}

	QXmlStreamWriter xmlWriter;
	xmlWriter.setDevice(&xmlFile);
	xmlWriter.setAutoFormatting(true);
	xmlWriter.setAutoFormattingIndent(4);
	xmlWriter.setCodec("windows-1251");
	xmlWriter.writeStartDocument();
	
	if (!SetFile(fi, xmlWriter))
		ELRF("File info set failed");

	xmlWriter.writeEndDocument();

	//QTextStream xmlContent(&xmlFile);
	//xmlContent << "<?xml version=\"1.0\" encoding=\"windows-1251\"?>\n";
	//xmlContent << document.toString(4);
	xmlFile.close();

	return true;
}

bool Writer::SetFile(const File& file, QXmlStreamWriter& xmlWriter)
{
	//<Includes>
	//	<Include val="include1.xml">
	//		<Variable name="BoxId" val="1808310"/>
	//	</Include>
	//</Includes>
	
	if (!SetIncludes(file.includes, xmlWriter))
		ELRF("Set Includes failed");

	if (!SetConfig(file.config, xmlWriter))
		ELRF("Set Config failed");

	return true;
}

bool Writer::SetIncludes(const QList<Include>& includes, QXmlStreamWriter& xmlWriter)
{
	if (includes.size() > 0)
	{
		xmlWriter.writeStartElement("Includes");
		for (const auto& include : includes)
		{
			xmlWriter.writeStartElement("Include");
			xmlWriter.writeAttribute("name", include.name);
			xmlWriter.writeAttribute("val", include.fileName);
			for (const auto& variable : include.variables)
			{
				xmlWriter.writeStartElement("Variable");
				xmlWriter.writeAttribute("name", variable.first);
				xmlWriter.writeAttribute("val", variable.second);
				xmlWriter.writeEndElement();
			}
			xmlWriter.writeEndElement();
		}
		xmlWriter.writeEndElement();
	}

	return true;
}

bool Writer::SetConfig(const Config& config, QXmlStreamWriter& xmlWriter)
{
	xmlWriter.writeStartElement("Config");

	if (config.networkingIsSet)
	{
		if (!SetNetworking(config.networking, xmlWriter))
			ELRF("Set Networking failed");
	}

	if (config.logIsSet)
	{
		if (!SetLog(config.log, xmlWriter))
			ELRF("Set Log failed");
	}

	if (!SetUnits(config.groups, xmlWriter))
		ELRF("Set Log failed");

	xmlWriter.writeEndElement();

	return true;
}

bool Writer::SetNetworking(const Networking& networking, QXmlStreamWriter& xmlWriter)
{
	xmlWriter.writeStartElement("Networking");
	xmlWriter.writeAttribute("id", QString("%1").arg(networking.id));
	xmlWriter.writeAttribute("accept_port", QString("%1").arg(networking.acceptPort));
	xmlWriter.writeAttribute("keep_alive_sec", QString("%1").arg(networking.keepAliveSec));
	if (networking.timeClient != NetworkingDefaults::timeClient)
		xmlWriter.writeAttribute("time_client", QString("%1").arg(networking.timeClient));
	if (networking.networkThreads != NetworkingDefaults::networkThreads)
		xmlWriter.writeAttribute("network_threads", QString("%1").arg(networking.networkThreads));
	if (networking.broadcastThreads != NetworkingDefaults::broadcastThreads)
		xmlWriter.writeAttribute("broadcast_threads", QString("%1").arg(networking.broadcastThreads));
	if (networking.clientsThreads != NetworkingDefaults::clientsThreads)
		xmlWriter.writeAttribute("clients_threads", QString("%1").arg(networking.clientsThreads));
	if (networking.notifyReadyClients != NetworkingDefaults::notifyReadyClients)
		xmlWriter.writeAttribute("notify_ready_clients", QString("%1").arg(networking.notifyReadyClients));
	if (networking.notifyReadyServers != NetworkingDefaults::notifyReadyServers)
		xmlWriter.writeAttribute("notify_ready_servers", QString("%1").arg(networking.notifyReadyServers));

	for (const auto& connect : networking.connects)
	{
		xmlWriter.writeStartElement("connect");
		xmlWriter.writeAttribute("port", QString("%1").arg(connect.port));
		xmlWriter.writeAttribute("ip", QString("%1").arg(connect.ip));
		xmlWriter.writeEndElement();
	}

	xmlWriter.writeEndElement();

	return true;
}

bool Writer::SetLog(const Log& log, QXmlStreamWriter& xmlWriter)
{
	//<Log>
	//	<Param name="LoggingLevel" type="int" val="0"/>
	//	<Param name="TOTAL_LOG_LIMIT_MB" type="int" val="500"/>
	//</Log>

	xmlWriter.writeStartElement("Log");

	xmlWriter.writeStartElement("Param");
	xmlWriter.writeAttribute("name", "LoggingLevel");
	xmlWriter.writeAttribute("type", "int");
	xmlWriter.writeAttribute("val", QString("%1").arg(log.loggingLevel));
	xmlWriter.writeEndElement();

	xmlWriter.writeStartElement("Param");
	xmlWriter.writeAttribute("name", "TOTAL_LOG_LIMIT_MB");
	xmlWriter.writeAttribute("type", "int");
	xmlWriter.writeAttribute("val", QString("%1").arg(log.totalLogLimit));
	xmlWriter.writeEndElement();

	if (log.logDir != "")
	{
		xmlWriter.writeStartElement("Param");
		xmlWriter.writeAttribute("name", "LogDir");
		xmlWriter.writeAttribute("type", "str");
		xmlWriter.writeAttribute("val", log.logDir);
		xmlWriter.writeEndElement();
	}

	xmlWriter.writeEndElement();

	return true;
}

bool Writer::SetUnits(const QList<Group>& groups, QXmlStreamWriter& xmlWriter)
{
	//<Units>
	//	<Group>
	//		<Param name="Path" type="str" val="handlers"/>
	//		<Unit Name="Sigma IQ Source" Id="sigma_iq_source" X="20" Y="40" Z="0">
	//		</Unit>
	//	</Group>
	//</Units>
	
	xmlWriter.writeStartElement("Units");

	for (const auto& group : groups)
	{
		if (!SetGroup(group, xmlWriter))
			ELRF("Set Group failed");
	}

	xmlWriter.writeEndElement();

	return true;
}

bool Writer::SetGroup(const Group& group, QXmlStreamWriter& xmlWriter)
{
	xmlWriter.writeStartElement("Group");

	xmlWriter.writeStartElement("Param");
	xmlWriter.writeAttribute("name", "Path");
	xmlWriter.writeAttribute("type", "str");
	xmlWriter.writeAttribute("val", group.path);
	xmlWriter.writeEndElement();

	for (const auto& unit : group.units)
	{
		if (!SetUnit(unit, xmlWriter))
			ELRF("Set Unit failed");
	}

	xmlWriter.writeEndElement();

	return true;
}

bool Writer::SetUnit(const Unit& unit, QXmlStreamWriter& xmlWriter)
{
	//<Unit Name="Configurator Sigma" Id="sigma_poly" X="20" Y="140" Z="0">
	//	<Param name="DRIVER_NAME" type="str" val="Driver" depends="true"/>
	//	<Array name="CHANNELS">
	//		<Item>
	//			<Param name="RECEIVER_NAME" type="str" val="Receiver" depends="true"/>
	//		</Item>
	//	</Array>
	//</Unit>

	xmlWriter.writeStartElement("Unit");
	xmlWriter.writeAttribute("Name", unit.name);
	xmlWriter.writeAttribute("Id", unit.id);
	xmlWriter.writeAttribute("X", QString("%1").arg(unit.x));
	xmlWriter.writeAttribute("Y", QString("%1").arg(unit.y));
	xmlWriter.writeAttribute("Z", QString("%1").arg(unit.z));

	for (const auto& param : unit.params)
	{
		if (!SetParam(param, xmlWriter))
			ELRF("Set Param failed");
	}

	for (const auto& array : unit.arrays)
	{
		if (array.name != "DEPENDS")
		{
			if (!SetArray(array, xmlWriter))
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
			if (!SetDepends(depends, xmlWriter))
				ELRF("Set Depends failed");
		}
	}

	xmlWriter.writeEndElement();

	return true;
}

bool Writer::SetParam(const Param& param, QXmlStreamWriter& xmlWriter)
{
	//<Param name="DRIVER_NAME" type="str" val="Driver" depends="true"/>

	xmlWriter.writeStartElement("Param");
	xmlWriter.writeAttribute("name", param.name);
	xmlWriter.writeAttribute("type", param.type);
	xmlWriter.writeAttribute("val", param.val);
	if (param.depends)
		xmlWriter.writeAttribute("depends", QVariant(param.depends).toString());

	xmlWriter.writeEndElement();

	return true;
}

bool Writer::SetArray(const Array& array, QXmlStreamWriter& xmlWriter)
{
	//<Array name="CHANNELS">
	//	<Item>
	//		<Param name="RECEIVER_NAME" type="str" val="Receiver" depends="true"/>
	//	</Item>
	//</Array>

	xmlWriter.writeStartElement("Array");
	xmlWriter.writeAttribute("name", array.name);
	if (array.type != "")
		xmlWriter.writeAttribute("type", array.type);

	for (const auto& item : array.items)
	{
		if (!SetItem(item, xmlWriter))
			ELRF("Set Item failed");
	}

	xmlWriter.writeEndElement();

	return true;
}

bool Writer::SetDepends(const QList<QString>& depends, QXmlStreamWriter& xmlWriter)
{
	//<Depends>
	//	<Item name="FL_DB_PG_1"/>
	//	<Item name="LRK_1"/>
	//</Depends>

	xmlWriter.writeStartElement("Depends");

	for (const auto& item : depends)
	{
		xmlWriter.writeStartElement("Item");
		xmlWriter.writeAttribute("name", item);
		xmlWriter.writeEndElement();
	}

	xmlWriter.writeEndElement();

	return true;
}

bool Writer::SetItem(const Item& item, QXmlStreamWriter& xmlWriter)
{
	//<Array name="BlockingStates" type="int">
	//	<Item val="0"/>
	//	<Item val="1"/>
	//</Array>

	xmlWriter.writeStartElement("Item");

	if (item.params.size() == 0 && item.arrays.size() == 0)
	{
		xmlWriter.writeAttribute("val", item.val);
	}
	else
	{
		for (const auto& param : item.params)
		{
			if (!SetParam(param, xmlWriter))
				ELRF("Set Param failed");
		}

		for (const auto& array : item.arrays)
		{
			if (!SetArray(array, xmlWriter))
				ELRF("Set Array failed");
		}
	}

	xmlWriter.writeEndElement();

	return true;
}
