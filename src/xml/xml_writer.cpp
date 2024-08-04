#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <regex>
#include <QTextStream>
#include "../unit/unit_types.h"
#include "../log/log_helper.h"
#include "xml_writer.h"

using namespace CubesXml;

#define CFRC(retcode, function) do { function; return retcode; } while(0)

Writer::Writer(CubesLog::ILogManager* logManager)
{
	logManager_ = logManager;

	logHelper_.reset(new CubesLog::LogHelper(logManager_, CubesLog::SourceType::xmlWriter,
		CubesXml::GetWriterErrorDescriptions()));
}

bool Writer::Write(QByteArray& byteArray, const File& fi)
{
	fi_ = fi;

	QBuffer buffer(&byteArray);
	buffer.open(QIODevice::WriteOnly);

	QXmlStreamWriter xmlWriter;
	xmlWriter.setDevice(&buffer);
	xmlWriter.setAutoFormatting(true);
	xmlWriter.setAutoFormattingIndent(4);
	xmlWriter.setCodec("windows-1251");
	xmlWriter.writeStartDocument();

	if (!SetFile(fi, xmlWriter))
		CFRC(false, logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>(WriterErrorCode::fileSetFailed),
			{ {"File name", fi_.fileName} }));

	xmlWriter.writeEndDocument();

	buffer.close();

	return true;
}

bool Writer::Write(const QString& filename, const File& fi)
{
	fi_ = fi;

	QByteArray byteArray;
	if (!Write(byteArray, fi))
		CFRC(false, logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>( WriterErrorCode::bufferWriteFailed),
			{ {"File name", fi_.fileName} }));

	QFile xmlFile(filename);
	if (!xmlFile.open(QIODevice::WriteOnly | QFile::Text))
		CFRC(false, logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>( WriterErrorCode::fileOpenFailed),
			{ {"File name", fi_.fileName} }));

	if (xmlFile.write(byteArray) == -1)
		CFRC(false, logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>( WriterErrorCode::fileWriteFailed),
			{ {"File name", fi_.fileName} }));

	xmlFile.close();

	return true;
}

bool Writer::WriteUnits(QByteArray& byteArray, const std::vector<Unit>& units)
{
	QBuffer buffer(&byteArray);
	buffer.open(QIODevice::WriteOnly);

	QXmlStreamWriter xmlWriter;
	xmlWriter.setDevice(&buffer);
	xmlWriter.setAutoFormatting(true);
	xmlWriter.setAutoFormattingIndent(4);
	xmlWriter.setCodec("windows-1251");
	xmlWriter.writeStartDocument();

	QList<Group> groups;
	Group group{};
	group.path = "clipboard";
	for (const auto& unit : units)
		group.units.push_back(unit);
	groups.push_back(group);

	if (!SetUnits(groups, xmlWriter))
		CFRC(false, logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>(WriterErrorCode::fileSetFailed)));

	xmlWriter.writeEndDocument();

	buffer.close();

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
		CFRC(false, logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>( WriterErrorCode::setIncludesFailed),
			{ {"File name", fi_.fileName} }));

	if (!SetConfig(file.config, file.name, file.platform, file.color, xmlWriter))
		CFRC(false, logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>( WriterErrorCode::setConfigFailed),
			{ {"File name", fi_.fileName} }));

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
			xmlWriter.writeAttribute("val", include.fileName);
			xmlWriter.writeAttribute("_name", include.name);
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

bool Writer::SetConfig(const Config& config, const QString& name, const QString& platform,
	const QString& color, QXmlStreamWriter& xmlWriter)
{
	xmlWriter.writeStartElement("Config");

	if (name != "")
		xmlWriter.writeAttribute("_name", name);
	if (platform != "")
		xmlWriter.writeAttribute("_platform", platform);
	if (color != "")
		xmlWriter.writeAttribute("_color", color);

	if (config.networkingIsSet)
	{
		if (!SetNetworking(config.networking, xmlWriter))
			CFRC(false, logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>( WriterErrorCode::setNetworkingFailed),
				{ {"File name", fi_.fileName} }));
	}

	if (config.logIsSet)
	{
		if (!SetLog(config.log, xmlWriter))
			CFRC(false, logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>( WriterErrorCode::setLogFailed),
				{ {"File name", fi_.fileName} }));
	}

	if (!SetUnits(config.groups, xmlWriter))
		CFRC(false, logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>( WriterErrorCode::setLogFailed),
			{ {"File name", fi_.fileName} }));

	xmlWriter.writeEndElement();

	return true;
}

bool Writer::SetNetworking(const Networking& networking, QXmlStreamWriter& xmlWriter)
{
	xmlWriter.writeStartElement("Networking");
	xmlWriter.writeAttribute("id", QString("%1").arg(networking.id));
	xmlWriter.writeAttribute("accept_port", QString("%1").arg(networking.acceptPort));
	xmlWriter.writeAttribute("keep_alive_sec", QString("%1").arg(networking.keepAliveSec));
	if (networking.timeClient != Networking::Defaults().timeClient)
		xmlWriter.writeAttribute("time_client", QString("%1").arg(networking.timeClient));
	if (networking.networkThreads != Networking::Defaults().networkThreads)
		xmlWriter.writeAttribute("network_threads", QString("%1").arg(networking.networkThreads));
	if (networking.broadcastThreads != Networking::Defaults().broadcastThreads)
		xmlWriter.writeAttribute("broadcast_threads", QString("%1").arg(networking.broadcastThreads));
	if (networking.clientsThreads != Networking::Defaults().clientsThreads)
		xmlWriter.writeAttribute("clients_threads", QString("%1").arg(networking.clientsThreads));
	if (networking.notifyReadyClients != Networking::Defaults().notifyReadyClients)
		xmlWriter.writeAttribute("notify_ready_clients", QString("%1").arg(networking.notifyReadyClients));
	if (networking.notifyReadyServers != Networking::Defaults().notifyReadyServers)
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
			CFRC(false, logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>( WriterErrorCode::setGroupFailed),
				{ {"File name", fi_.fileName} }));
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
			CFRC(false, logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>( WriterErrorCode::setUnitFailed),
				{ {"File name", fi_.fileName} }));
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
	xmlWriter.writeAttribute("_x", QString("%1").arg(unit.x));
	xmlWriter.writeAttribute("_y", QString("%1").arg(unit.y));
	xmlWriter.writeAttribute("_z", QString("%1").arg(unit.z));

	for (const auto& param : unit.params)
	{
		if (!SetParam(param, xmlWriter))
			CFRC(false, logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>( WriterErrorCode::setParamFailed),
				{ {"File name", fi_.fileName} }));
	}

	for (const auto& array : unit.arrays)
	{
		if (array.name != ids_.dependencies.toString())
		{
			if (!SetArray(array, xmlWriter))
				CFRC(false, logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>( WriterErrorCode::setParamFailed),
					{ {"File name", fi_.fileName} }));
		}
	}

	for (const auto& array : unit.arrays)
	{
		if (array.name == ids_.dependencies.toString())
		{
			QList<QString> depends;
			for (const auto& depend : array.items)
				depends.push_back(depend.val);
			if (!SetDepends(depends, xmlWriter))
				CFRC(false, logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>( WriterErrorCode::setDependsFailed),
					{ {"File name", fi_.fileName} }));
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
			CFRC(false, logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>( WriterErrorCode::setItemFailed),
				{ {"File name", fi_.fileName} }));
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

	if (item.name != "")
		xmlWriter.writeAttribute("_name", item.name);
	if (item.x != 0 || item.y != 0 || item.z != 0)
	{
		xmlWriter.writeAttribute("_x", QString("%1").arg(item.x));
		xmlWriter.writeAttribute("_y", QString("%1").arg(item.y));
		xmlWriter.writeAttribute("_z", QString("%1").arg(item.z));
	}

	if (item.params.size() == 0 && item.arrays.size() == 0)
	{
		xmlWriter.writeAttribute("val", item.val);
	}
	else
	{
		for (const auto& param : item.params)
		{
			if (!SetParam(param, xmlWriter))
				CFRC(false, logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>( WriterErrorCode::setParamFailed),
					{ {"File name", fi_.fileName} }));
		}

		for (const auto& array : item.arrays)
		{
			if (!SetArray(array, xmlWriter))
				CFRC(false, logHelper_->LogError(static_cast<CubesLog::BaseErrorCode>( WriterErrorCode::setArrayFailed),
					{ {"File name", fi_.fileName} }));
		}
	}

	xmlWriter.writeEndElement();

	return true;
}
