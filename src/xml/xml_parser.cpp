#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <regex>
#include <QTextStream>
#include "../properties/properties_item_types.h"
#include "../properties/properties_item.h"
#include "../unit/unit_types.h"
#include "xml_types.h"
#include "xml_parser.h"

using namespace CubesXml;

#define ELRC(retcode, errcode, message) do {\
		std::stringstream ss;\
		ss << message;\
		if (logManager_ != nullptr)\
		{\
			CubesLog::LogMessage lm{};\
			lm.type = CubesLog::MessageType::error;\
			lm.code = CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser, static_cast<uint32_t>(errcode));\
			lm.source = CubesLog::SourceType::xmlParser;\
			lm.description = QString::fromStdString(ss.str());\
			lm.tag = CubesUnitTypes::InvalidUniversalId;\
			logManager_->AddMessage(lm);\
		}\
		std::cout << ss.str() << std::endl; return retcode;\
	} while(0)

Parser::Parser(CubesLog::ILogManager* logManager)
{
	logManager_ = logManager;
}

bool Parser::Parse(QByteArray& byteArray, const QString& fileName)
{
	fi_ = {};
	fi_.fileName = fileName;

	QTextStream in(&byteArray);
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
	doc.setContent(xmlText);

	QDomElement root = doc.documentElement();
	if (!GetFile(root, fi_))
		ELRC(false, ParserErrorType::fileParseFailed, "File parse failed");

	return true;
}

bool Parser::Parse(const QString& fileName)
{
	QFile xmlFile(fileName);
	if (!xmlFile.open(QIODevice::ReadOnly))
	{
		ELRC(false, ParserErrorType::fileOpenFailed, "File " << fileName.toStdString() << " load failed");
	}

	QByteArray byteArray = xmlFile.readAll();

	return Parse(byteArray, fileName);
}

const File& Parser::GetFile()
{
	return fi_;
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
					ELRC(false, ParserErrorType::getIncludesFailed, "Get Includes failed");
			}
			else if (ne.tagName() == "Config")
			{
				QString name;
				QString platform;
				QString color;
				if (!GetConfig(ne, name, platform, color, fi.config))
					ELRC(false, ParserErrorType::getConfigFailed, "Get Config failed");
				fi.name = name;
				fi.platform = platform;
				fi.color = color;
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
				ELRC(false, ParserErrorType::includesChildUnknown, "Includes have unknown child");

			Include include{};

			QString i_val = ei.attribute("val", "");
			if (i_val == "")
				ELRC(false, ParserErrorType::includesIncludeValEmpty, "Includes/Include val is empty");
			include.fileName = i_val;

			QString i_name = ei.attribute("_name", "");
			include.name = i_name;

			QMap<QString, QString> variables;
			QDomNode v = i.firstChild();
			while (!v.isNull())
			{
				QDomElement ev = v.toElement();
				if (!ev.isNull())
				{
					if (ev.tagName() != "Variable")
						ELRC(false, ParserErrorType::includesIncludeChildUnknown, "Includes/Include have unknown child");

					QString v_name = ev.attribute("name", "");
					QString v_val = ev.attribute("val", "");

					if (v_name == "")
						ELRC(false, ParserErrorType::includesIncludeVariableNameEmpty, "Includes/Include/Variable name is empty");
					if (v_val == "")
						ELRC(false, ParserErrorType::includesIncludeVariableValEmpty, "Includes/Include/Variable val is empty");

					if (variables.contains(v_name))
						ELRC(false, ParserErrorType::includesIncludeVariableNameDuplicate, "Includes/Include/Variable name duplicate");

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

bool Parser::GetConfig(const QDomElement& node, QString& name, QString& platform, QString& color, Config& config)
{
	name = node.attribute("_name", "");
	platform = node.attribute("_platform", "");
	color = node.attribute("_color", "");

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
					ELRC(false, ParserErrorType::getNetworkFailed, "Get Networking failed");
			}
			else if (ei.tagName() == "Log")
			{
				config.logIsSet = true;
				if (!GetLog(ei, config.log))
					ELRC(false, ParserErrorType::getLogFailed, "Get Log failed");
			}
			else if (ei.tagName() == "Units")
			{
				if (!GetUnits(ei, config.groups))
					ELRC(false, ParserErrorType::getUnitsFailed, "Get Units failed");
			}
			else
				ELRC(false, ParserErrorType::configChildUnknown, "Config have unknown child");
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
		ELRC(false, ParserErrorType::networkingIdEmpty, "Networking id is empty");
	if (accept_port == "")
		ELRC(false, ParserErrorType::networkingAcceptPortEmpty, "Networking accept_port is empty");
	if (keep_alive_sec == "")
		ELRC(false, ParserErrorType::networkingKeepAliveSecEmpty, "Networking keep_alive_sec is empty");

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
				ELRC(false, ParserErrorType::networkingChildUnknown, "Networking have unknown child");

			QString v_port = ev.attribute("port", "");
			QString v_ip = ev.attribute("ip", "");

			if (v_port == "")
				ELRC(false, ParserErrorType::networkingConnectPortEmpty, "Networking/connect port is empty");
			if (v_ip == "")
				ELRC(false, ParserErrorType::networkingConnectPortIp, "Networking/connect ip is empty");

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
				ELRC(false, ParserErrorType::logChildUnknown, "Log have unknown child");

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
				ELRC(false, ParserErrorType::logParamUnknown, "Log/Param is unknown");
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
				ELRC(false, ParserErrorType::unitsChildUnknown, "Units have unknown child");

			Group group{};
			if (!GetGroup(ei, group))
				ELRC(false, ParserErrorType::getGroupFailed, "Get Group failed");

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
		ELRC(false, ParserErrorType::groupParamNotSingle, "Group/Param not found or more then one");

	const auto& ep = paramNodes[0];
	QString name = ep.attribute("name", "");
	QString type = ep.attribute("type", "");
	QString val = ep.attribute("val", "");

	if (name == "Path" && type == "str")
		group.path = val;
	else
		ELRC(false, ParserErrorType::groupParamUnknown, "Group/Param is unknown");

	auto unitNodes = ElementsByTagName(node, "Unit");
	for (const auto& eu : unitNodes)
	{
		Unit unit{};
		if (!GetUnit(eu, unit))
			ELRC(false, ParserErrorType::getUnitFailed, "Get Unit failed");
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
		ELRC(false, ParserErrorType::unitNameEmpty, "Unit Name is empty");
	if (id == "")
		ELRC(false, ParserErrorType::unitIdEmpty, "Unit Id is empty");

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
			ELRC(false, ParserErrorType::getParamFailed, "Get Param failed");
		unit.params.push_back(std::move(param));
	}

	auto arrayNodes = ElementsByTagName(node, "Array");
	for (const auto& ea : arrayNodes)
	{
		Array array{};
		if (!GetArray(ea, array))
			ELRC(false, ParserErrorType::getArrayFailed, "Get Array failed");
		unit.arrays.push_back(std::move(array));
	}

	// Интерпретируем зависимости юнита как массив строк
	// При загрузке параметров автоматически добавляется массив строк с именем $DEPENEDCIES
	// Значения для зависимостей хранятся в поле name вместо val, учитываем это при загрузке

	auto dependsNodes = ElementsByTagName(node, "Depends");
	for (const auto& ed : dependsNodes)
	{
		QList<QString> depends;
		if (!GetDepends(ed, depends))
			ELRC(false, ParserErrorType::getDependsFailed, "Get Depends failed");

		Array array{};
		array.name = ids_.dependencies.toString();
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
		ELRC(false, ParserErrorType::unitParamNameEmpty, "Unit/Param name is empty");
	if (type == "")
		ELRC(false, ParserErrorType::unitParamTypeEmpty, "Unit/Param type is empty");
	if (val == "")
		ELRC(false, ParserErrorType::unitParamValEmpty, "Unit/Param val is empty");
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
		ELRC(false, ParserErrorType::unitParamNameEmpty, "Unit/Param name is empty");
	// type is optional

	array.name = name;
	array.type = type;

	auto itemNodes = ElementsByTagName(node, "Item");
	for (const auto& ei : itemNodes)
	{
		Item item;
		if (!GetItem(ei, item))
			ELRC(false, ParserErrorType::getItemFailed, "Get Item failed");
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
			ELRC(false, ParserErrorType::unitDependsItemEmpty, "Unit/Depends/Item name is empty");

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
			ELRC(false, ParserErrorType::getParamFailed, "Get Param failed");
		item.params.push_back(std::move(param));
	}

	auto arrayNodes = ElementsByTagName(node, "Array");
	for (const auto& ea : arrayNodes)
	{
		Array array{};
		if (!GetArray(ea, array))
			ELRC(false, ParserErrorType::getArrayFailed, "Get Array failed");
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
