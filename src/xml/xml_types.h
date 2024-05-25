#pragma once

#include <QString>
#include <QPair>
#include <QList>

namespace CubesXml
{
	/*<Includes>
		<Include val="config_fhss_searcher_task_factory_test_sigma_include.xml">
			<Variable name="BoxId" val="1808310"/>
		</Include>
	</Includes>*/
	struct Include
	{
		QString name; // TODO: Add to xml format - short name of file
		QString fileName;
		QList<QPair<QString, QString>> variables;
	};

	/*<Networking id="1000" accept_port="62000" keep_alive_sec="30000">
		<connect port="50000" ip="sigma-1808310"/>
		<!-- <connect port="50000" ip="172.18.10.115"/> -->
	</Networking>*/
	struct Connect
	{
		QString ip;
		int port;

		static const Connect& Defaults()
		{
			static const Connect connect{ "127.0.0.1", 50000 };
			return connect;
		}
	};

	struct Networking
	{
		int id;
		int acceptPort;
		int keepAliveSec;
		bool timeClient;
		int networkThreads;
		int broadcastThreads;
		int clientsThreads;
		bool notifyReadyClients;
		bool notifyReadyServers;
		QList<Connect> connects;

		static const Networking& Defaults()
		{
			static const Networking networking{ 1000, 60000, 10, false, 4, 1, 1, true, false };
			return networking;
		}
	};

	//struct NetworkingDefaults
	//{
	//	static const int id = 1000;
	//	static const int acceptPort = 60000;
	//	static const int keepAliveSec = 10;
	//	static const bool timeClient = false;
	//	static const int networkThreads = 4;
	//	static const int broadcastThreads = 1;
	//	static const int clientsThreads = 1;
	//	static const bool notifyReadyClients = true;
	//	static const bool notifyReadyServers = false;
	//};

	/*<Log>
		<!-- 
		0 - LOG_TRACE  
		1 - LOG_DEBUG  
		2 - LOG_INFO   
		3 - LOG_WARNING
		4 - LOG_ERROR  
		5 - LOG_FATAL  
		-->
		<Param name="LoggingLevel" type="int" val="0"/>
		<Param name="TOTAL_LOG_LIMIT_MB" type="int" val="500"/>
	</Log>*/
	struct Log
	{
		int loggingLevel;
		int totalLogLimit;
		QString logDir;

		static const Log& Defaults()
		{
			static const Log log{ 0, 500, QString() };
			return log;
		}
	};

	//struct LogDefaults
	//{
	//	static const int loggingLevel = 0;
	//	static const int totalLogLimit = 500;
	//	static const QString logDir;
	//};

	/*<Units>
		<!--Modules-->
		<Group>
			<Param name="Path" type="str" val="modules"/>

			<Unit Name="HardwareManagerUnit" Id="hardware_manager">
				<Param name="SchemeManagerName" type="str" val="HardwareScheme"/>
				<Param name="InitThreadNum" type="int" val="2"/>
				<Param name="StartDeviceTimesSum" type="int" val="2"/>
				<Param name="LogHarvester" type="str" val="LOG_HARVESTER"/>
				<Depends>
					<Item name="LOG_HARVESTER"/>
				</Depends>
			</Unit>
		</Group>
	</Units>*/

	struct Param
	{
		QString name;
		QString type;
		QString val;
		bool depends;
	};

	struct Array;

	struct Item
	{
		QString val;
		QList<Param> params;
		QList<Array> arrays;
		// Additional parameters for yml arrays
		// TODO: Add to xml format
		QString name;
		int32_t x;
		int32_t y;
		int32_t z;
	};

	struct Array
	{
		QString name;
		QString type;
		QList<Item> items;
	};

	struct Unit
	{
		QString name;
		QString id;
		QList<Param> params;
		QList<Array> arrays;
		// Additional parameters
		// TODO: Add to xml format
		int32_t x;
		int32_t y;
		int32_t z;
	};

	struct Group
	{
		QString path;
		QList<Unit> units;
	};

	struct Config
	{
		Networking networking;
		bool networkingIsSet;
		Log log;
		bool logIsSet;
		QList<Group> groups;
	};

	struct File
	{
		QString name; // TODO: Add to xml format - short name of file
		QString platform; // TODO: Add to xml format - target patform
		QString color; // TODO: Add to xml format - color
		QString fileName;
		QList<Include> includes;
		Config config;
	};

	enum class ElementType
	{
		None,
		Param,
		Array,
		Item,
		Service
	};

	struct Element
	{
		ElementType type;
		int itemsCount; // For array elements
		QString arrayType; // For item element
		Param* param;
		Array* array;
		Item* item;
		QList<Param>* params; // For service elements
		QList<Array>* arrays; // For service elements

		Element()
		{
			type = ElementType::None;
			itemsCount = 0;
			param = nullptr;
			array = nullptr;
			item = nullptr;
			params = nullptr;
			arrays = nullptr;
		}
	};

	enum class ParserErrorCode
	{
		ok = 0,
		fileParseFailed = 1000,
		fileOpenFailed,
		getIncludesFailed,
		getConfigFailed,
		includesChildUnknown,
		includesIncludeValEmpty,
		includesIncludeChildUnknown,
		includesIncludeVariableNameEmpty,
		includesIncludeVariableValEmpty,
		includesIncludeVariableNameDuplicate,
		getNetworkFailed,
		getLogFailed,
		getUnitsFailed,
		configChildUnknown,
		networkingIdEmpty,
		networkingAcceptPortEmpty,
		networkingKeepAliveSecEmpty, 
		networkingChildUnknown,
		networkingConnectPortEmpty,
		networkingConnectIpEmpty,
		logChildUnknown,
		logParamUnknown,
		unitsChildUnknown,
		getGroupFailed,
		groupParamNotSingle,
		groupParamUnknown,
		getUnitFailed,
		unitNameEmpty,
		unitIdEmpty,
		getParamFailed,
		getArrayFailed,
		getDependsFailed,
		unitParamNameEmpty,
		unitParamTypeEmpty,
		unitParamValEmpty,
		getItemFailed,
		unitDependsItemEmpty
	};

	inline QString GetParserErrorDescription(ParserErrorCode errorCode)
	{
		switch (errorCode)
		{
		case ParserErrorCode::ok: return "Ok";
		case ParserErrorCode::fileParseFailed: return "Parsing failed";
		case ParserErrorCode::fileOpenFailed: return "File open failed";
		case ParserErrorCode::getIncludesFailed: return "Get Includes failed";
		case ParserErrorCode::getConfigFailed: return "Get Config failed";
		case ParserErrorCode::includesChildUnknown: return "Includes have unknown child";
		case ParserErrorCode::includesIncludeValEmpty: return "Includes/Include val is empty";
		case ParserErrorCode::includesIncludeChildUnknown: return "Includes/Include have unknown child";
		case ParserErrorCode::includesIncludeVariableNameEmpty: return "Includes/Include/Variable name is empty";
		case ParserErrorCode::includesIncludeVariableValEmpty: return "Includes/Include/Variable val is empty";
		case ParserErrorCode::includesIncludeVariableNameDuplicate: return "Includes/Include/Variable name duplicate";
		case ParserErrorCode::getNetworkFailed: return "Get Networking failed";
		case ParserErrorCode::getLogFailed: return "Get Log failed";
		case ParserErrorCode::getUnitsFailed: return "Get Units failed";
		case ParserErrorCode::configChildUnknown: return "Config have unknown child";
		case ParserErrorCode::networkingIdEmpty: return "Networking id is empty";
		case ParserErrorCode::networkingAcceptPortEmpty: return "Networking accept_port is empty";
		case ParserErrorCode::networkingKeepAliveSecEmpty: return "Networking keep_alive_sec is empty";
		case ParserErrorCode::networkingChildUnknown: return "Networking have unknown child";
		case ParserErrorCode::networkingConnectPortEmpty: return "Networking/connect port is empty";
		case ParserErrorCode::networkingConnectIpEmpty: return "Networking/connect ip is empty";
		case ParserErrorCode::logChildUnknown: return "Log have unknown child";
		case ParserErrorCode::logParamUnknown: return "Log/Param is unknown";
		case ParserErrorCode::unitsChildUnknown: return "Units have unknown child";
		case ParserErrorCode::getGroupFailed: return "Get Group failed";
		case ParserErrorCode::groupParamNotSingle: return "Group/Param not found or more then one";
		case ParserErrorCode::groupParamUnknown: return "Group/Param is unknown";
		case ParserErrorCode::getUnitFailed: return "Get Unit failed";
		case ParserErrorCode::unitNameEmpty: return "Unit Name is empty";
		case ParserErrorCode::unitIdEmpty: return "Unit Id is empty";
		case ParserErrorCode::getParamFailed: return "Get Param failed";
		case ParserErrorCode::getArrayFailed: return "Get Array failed";
		case ParserErrorCode::getDependsFailed: return "Get Depends failed";
		case ParserErrorCode::unitParamNameEmpty: return "Unit/Param name is empty";
		case ParserErrorCode::unitParamTypeEmpty: return "Unit/Param type is empty";
		case ParserErrorCode::unitParamValEmpty: return "Unit/Param val is empty";
		case ParserErrorCode::getItemFailed: return "Get Item failed";
		case ParserErrorCode::unitDependsItemEmpty: return "Unit/Depends/Item name is empty";
		default: return QString("%1").arg(static_cast<uint32_t>(errorCode));
		}
	}

	enum class WriterErrorCode
	{
		ok = 0,
		fileSetFailed = 1000,
		bufferWriteFailed,
		fileOpenFailed,
		fileWriteFailed,
		setIncludesFailed,
		setConfigFailed,
		setNetworkingFailed,
		setLogFailed,
		setGroupFailed,
		setUnitFailed,
		setParamFailed,
		setDependsFailed,
		setItemFailed,
		setArrayFailed
	};

	inline QString GetWriterErrorDescription(WriterErrorCode errorCode)
	{
		switch (errorCode)
		{
		case WriterErrorCode::ok: return "Ok";
		case WriterErrorCode::fileSetFailed: return "File set failed";
		case WriterErrorCode::bufferWriteFailed: return "Buffer write failed";
		case WriterErrorCode::fileOpenFailed: return "File open failed";
		case WriterErrorCode::fileWriteFailed: return "File write failed";
		case WriterErrorCode::setIncludesFailed: return "Set Includes failed";
		case WriterErrorCode::setConfigFailed: return "Set Config failed";
		case WriterErrorCode::setNetworkingFailed: return "Set Networking failed";
		case WriterErrorCode::setLogFailed: return "Set Log failed";
		case WriterErrorCode::setGroupFailed: return "Set Group failed";
		case WriterErrorCode::setUnitFailed: return "Set Unit failed";
		case WriterErrorCode::setParamFailed: return "Set Param failed";
		case WriterErrorCode::setDependsFailed: return "Set Depends failed";
		case WriterErrorCode::setItemFailed: return "Set Item failed";
		case WriterErrorCode::setArrayFailed: return "Set Array failed";
		default: return QString("%1").arg(static_cast<uint32_t>(errorCode));
		}
	}

	enum class HelperErrorCode
	{
		ok = 0,
		fileParseFailed = 1000,
		invalidArgument,
		arrayItemNotFound,
		unitParametersMalformed,
		bufferWriteFailed,
		fileWriteFailed
	};

	inline QString GetHelperErrorDescription(HelperErrorCode errorCode)
	{
		switch (errorCode)
		{
		case HelperErrorCode::ok: return "Ok";
		case HelperErrorCode::fileParseFailed: return "Parsing failed";
		case HelperErrorCode::invalidArgument: return "Invalid argument";
		case HelperErrorCode::unitParametersMalformed: return "Unit parameters malformed";
		case HelperErrorCode::bufferWriteFailed: return "Buffer write failed";
		case HelperErrorCode::fileWriteFailed: return "File write failed";
		default: return QString("%1").arg(static_cast<uint32_t>(errorCode));
		}
	}
}
