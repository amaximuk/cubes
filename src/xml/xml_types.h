#pragma once

#include <QString>
#include <QPair>
#include <QList>
#include "../log/log_types.h"

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

	enum class ParserErrorCode : CubesLog::BaseErrorCode
	{
		success = CubesLog::SuccessErrorCode,
		fileParseFailed = CubesLog::GetSourceTypeCodeOffset(CubesLog::SourceType::xmlParser),
		fileOpenFailed,
		getIncludesFailed,
		getConfigFailed,
		includesChildUnknown,
		includesIncludeValEmpty,
		includesIncludeChildUnknown,
		includesIncludeVariableNameEmpty,
		includesIncludeVariableValEmpty,
		includesIncludeVariableNameDuplicate,
		getNetworkingFailed,
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
		groupParamNotFound,
		groupParamDuplicate,
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
		unitDependsItemEmpty,
		last
	};

	inline const CubesLog::BaseErrorCodeDescriptions& GetParserErrorDescriptions()
	{
		static CubesLog::BaseErrorCodeDescriptions descriptions;
		if (descriptions.empty())
		{
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::success)] = QString::fromLocal8Bit("Успех");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::fileParseFailed)] = QString::fromLocal8Bit("Ошибка разбора файла");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::fileOpenFailed)] = QString::fromLocal8Bit("Ошибка открытия файла");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::getIncludesFailed)] = QString::fromLocal8Bit("Ошибка получения включаемых файлов (Includes)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::getConfigFailed)] = QString::fromLocal8Bit("Ошибка получения конфигурации (Config)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::includesChildUnknown)] = QString::fromLocal8Bit("Неизвестный потомок включаемых файлов (Includes)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::includesIncludeValEmpty)] = QString::fromLocal8Bit("Значение включаемого файла не задано (Includes/Include val)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::includesIncludeChildUnknown)] = QString::fromLocal8Bit("Неизвестный потомок включаемого файла (Includes/Include)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::includesIncludeVariableNameEmpty)] = QString::fromLocal8Bit("Имя переменной включаемого файла не задано (Includes/Include/Variable name)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::includesIncludeVariableValEmpty)] = QString::fromLocal8Bit("Значение переменной включаемого файла не задано (Includes/Include/Variable val)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::includesIncludeVariableNameDuplicate)] = QString::fromLocal8Bit("Имя переменной включаемого файла уже используется (Includes/Include/Variable name)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::getNetworkingFailed)] = QString::fromLocal8Bit("Ошибка получения сети (Networking)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::getLogFailed)] = QString::fromLocal8Bit("Ошибка получения логгирования (Log)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::getUnitsFailed)] = QString::fromLocal8Bit("Ошибка получения юнитов (Units)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::configChildUnknown)] = QString::fromLocal8Bit("Неизвестный потомок конфигурации (Config)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::networkingIdEmpty)] = QString::fromLocal8Bit("Идентификатор сети не задан (Networking id)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::networkingAcceptPortEmpty)] = QString::fromLocal8Bit("Порт сети не задан (Networking accept_port)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::networkingKeepAliveSecEmpty)] = QString::fromLocal8Bit("Таймаут сети не задан (Networking keep_alive_sec)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::networkingChildUnknown)] = QString::fromLocal8Bit("Неизвестный потомок сети (Networking)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::networkingConnectPortEmpty)] = QString::fromLocal8Bit("Порт соединения не задан (Networking/connect port)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::networkingConnectIpEmpty)] = QString::fromLocal8Bit("Адрес соединения не задан (Networking/connect ip)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::logChildUnknown)] = QString::fromLocal8Bit("Неизвестный потомок логгирования (Log)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::logParamUnknown)] = QString::fromLocal8Bit("Неизвестный параметр логгирования (Log/Param)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::unitsChildUnknown)] = QString::fromLocal8Bit("Неизвестный потомок инитов (Units)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::getGroupFailed)] = QString::fromLocal8Bit("Ошибка получения группы (Group)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::groupParamNotFound)] = QString::fromLocal8Bit("Параметр группы не найден (Group/Param)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::groupParamDuplicate)] = QString::fromLocal8Bit("Параметр группы уже используется (Group/Param)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::groupParamUnknown)] = QString::fromLocal8Bit("Неизвестный параметр группы (Group/Param)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::getUnitFailed)] = QString::fromLocal8Bit("Ошибка получения юнита (Unit)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::unitNameEmpty)] = QString::fromLocal8Bit("Имя юнита не задано (Unit Name)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::unitIdEmpty)] = QString::fromLocal8Bit("Идентификатор юнита не задан (Unit Id)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::unitIdEmpty)] = QString::fromLocal8Bit("Идентификатор юнита не задан (Unit Id)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::getParamFailed)] = QString::fromLocal8Bit("Ошибка получения параметра (Param)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::getArrayFailed)] = QString::fromLocal8Bit("Ошибка получения массива (Array)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::getDependsFailed)] = QString::fromLocal8Bit("Ошибка получения зависимостей (Depends)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::unitParamNameEmpty)] = QString::fromLocal8Bit("Имя параметра не задано (Unit/Param name)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::unitParamTypeEmpty)] = QString::fromLocal8Bit("Тип параметра не задан (Unit/Param type)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::unitParamValEmpty)] = QString::fromLocal8Bit("Значение параметра не задано (Unit/Param val)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::getItemFailed)] = QString::fromLocal8Bit("Ошибка получения элемента (Item)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::unitDependsItemEmpty)] = QString::fromLocal8Bit("Имя элемента зависимости не задано (Unit/Depends/Item name)");
		}

		assert((static_cast<CubesLog::BaseErrorCode>(ParserErrorCode::last) - CubesLog::GetSourceTypeCodeOffset(CubesLog::SourceType::xmlParser) + 1) == descriptions.size());
		
		return descriptions;
	}

	inline QString GetParserErrorDescription(ParserErrorCode errorCode)
	{
		const auto& descriptions = GetParserErrorDescriptions();
		if (descriptions.contains(static_cast<CubesLog::BaseErrorCode>(errorCode)))
			return descriptions[static_cast<CubesLog::BaseErrorCode>(errorCode)];
		return QString("%1").arg(static_cast<uint32_t>(errorCode));
	}

	enum class WriterErrorCode : CubesLog::BaseErrorCode
	{
		success = CubesLog::SuccessErrorCode,
		fileSetFailed = CubesLog::GetSourceTypeCodeOffset(CubesLog::SourceType::xmlWriter),
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
		setArrayFailed,
		last
	};

	inline const CubesLog::BaseErrorCodeDescriptions& GetWriterErrorDescriptions()
	{
		static CubesLog::BaseErrorCodeDescriptions descriptions;
		if (descriptions.empty())
		{
			descriptions[static_cast<CubesLog::BaseErrorCode>(WriterErrorCode::success)] = QString::fromLocal8Bit("Успех");
			descriptions[static_cast<CubesLog::BaseErrorCode>(WriterErrorCode::fileSetFailed)] = QString::fromLocal8Bit("Ошибка установки файла (File)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(WriterErrorCode::bufferWriteFailed)] = QString::fromLocal8Bit("Ошибка записи буфера");
			descriptions[static_cast<CubesLog::BaseErrorCode>(WriterErrorCode::fileOpenFailed)] = QString::fromLocal8Bit("Ошибка открытия файла");
			descriptions[static_cast<CubesLog::BaseErrorCode>(WriterErrorCode::fileWriteFailed)] = QString::fromLocal8Bit("Ошибка записи файла");
			descriptions[static_cast<CubesLog::BaseErrorCode>(WriterErrorCode::setIncludesFailed)] = QString::fromLocal8Bit("Ошибка установки включаемых файлов (Includes)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(WriterErrorCode::setConfigFailed)] = QString::fromLocal8Bit("Ошибка установки конфигурации (Config)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(WriterErrorCode::setNetworkingFailed)] = QString::fromLocal8Bit("Ошибка установки файла (Networking)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(WriterErrorCode::setLogFailed)] = QString::fromLocal8Bit("Ошибка установки лога (Log)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(WriterErrorCode::setGroupFailed)] = QString::fromLocal8Bit("Ошибка установки группы (Group)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(WriterErrorCode::setUnitFailed)] = QString::fromLocal8Bit("Ошибка установки юнита (Unit)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(WriterErrorCode::setParamFailed)] = QString::fromLocal8Bit("Ошибка установки параметра (Param)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(WriterErrorCode::setDependsFailed)] = QString::fromLocal8Bit("Ошибка установки зависимостей (Depends)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(WriterErrorCode::setItemFailed)] = QString::fromLocal8Bit("Ошибка установки элемента (Item)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(WriterErrorCode::setArrayFailed)] = QString::fromLocal8Bit("Ошибка установки массива (Array)");
		}

		assert((static_cast<CubesLog::BaseErrorCode>(WriterErrorCode::last) - CubesLog::GetSourceTypeCodeOffset(CubesLog::SourceType::xmlWriter) + 1) == descriptions.size());

		return descriptions;
	}

	inline QString GetWriterErrorDescription(WriterErrorCode errorCode)
	{
		const auto& descriptions = GetWriterErrorDescriptions();
		if (descriptions.contains(static_cast<CubesLog::BaseErrorCode>(errorCode)))
			return descriptions[static_cast<CubesLog::BaseErrorCode>(errorCode)];
		return QString("%1").arg(static_cast<uint32_t>(errorCode));
	}

	enum class HelperErrorCode : CubesLog::BaseErrorCode
	{
		success = CubesLog::SuccessErrorCode,
		fileParseFailed = CubesLog::GetSourceTypeCodeOffset(CubesLog::SourceType::xmlHelper),
		invalidArgument,
		unitParametersMalformed,
		bufferWriteFailed,
		fileWriteFailed,
		last
	};

	inline const CubesLog::BaseErrorCodeDescriptions& GetHelperErrorDescriptions()
	{
		static CubesLog::BaseErrorCodeDescriptions descriptions;
		if (descriptions.empty())
		{
			descriptions[static_cast<CubesLog::BaseErrorCode>(HelperErrorCode::success)] = QString::fromLocal8Bit("Успех");
			descriptions[static_cast<CubesLog::BaseErrorCode>(HelperErrorCode::fileParseFailed)] = QString::fromLocal8Bit("Ошибка разбора файла");
			descriptions[static_cast<CubesLog::BaseErrorCode>(HelperErrorCode::invalidArgument)] = QString::fromLocal8Bit("Недопустимый параметр");
			descriptions[static_cast<CubesLog::BaseErrorCode>(HelperErrorCode::unitParametersMalformed)] = QString::fromLocal8Bit("Параметры юнита плохо сформированы");
			descriptions[static_cast<CubesLog::BaseErrorCode>(HelperErrorCode::bufferWriteFailed)] = QString::fromLocal8Bit("Ошибка записи буфера");
			descriptions[static_cast<CubesLog::BaseErrorCode>(HelperErrorCode::fileWriteFailed)] = QString::fromLocal8Bit("Ошибка записи файла");
		}

		assert((static_cast<CubesLog::BaseErrorCode>(HelperErrorCode::last) - CubesLog::GetSourceTypeCodeOffset(CubesLog::SourceType::xmlHelper) + 1) == descriptions.size());

		return descriptions;
	}

	inline QString GetHelperErrorDescription(HelperErrorCode errorCode)
	{
		const auto& descriptions = GetHelperErrorDescriptions();
		if (descriptions.contains(static_cast<CubesLog::BaseErrorCode>(errorCode)))
			return descriptions[static_cast<CubesLog::BaseErrorCode>(errorCode)];
		return QString("%1").arg(static_cast<uint32_t>(errorCode));
	}
}
