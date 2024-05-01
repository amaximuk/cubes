#pragma once

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
}
