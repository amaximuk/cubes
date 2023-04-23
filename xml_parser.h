#pragma once

#include <QString>
#include <QMap>
#include <QList>
#include <QtXml>

#include "parameters_compiler_types.h"

namespace xml
{
	/*<Includes>
		<Include val="config_fhss_searcher_task_factory_test_sigma_include.xml">
			<Variable name="BoxId" val="1808310"/>
		</Include>
	</Includes>*/
	struct Include
	{
		QString fileName;
		QMap<QString, QString> variables;
	};

	/*<Networking id="1000" accept_port="62000" keep_alive_sec="30000">
		<connect port="50000" ip="sigma-1808310"/>
		<!-- <connect port="50000" ip="172.18.10.115"/> -->
	</Networking>*/
	struct Connect
	{
		QString host;
		int port;
	};

	struct Networking
	{
		int id;
		int acceptPort;
		int keepAlive;
		QList<Connect> connects;
	};

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
	};

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
		QList<QString> depends;
	};

	struct Group
	{
		QString path;
		QList<Unit> units;
	};

	//struct File
	//{
	//	QList<Include> includes;
	//	Networking networking;
	//	Log log;
	//	QList<Group> groups;
	//};

	struct Config
	{
		Networking networking;
		bool networking_is_set;
		Log log;
		bool log_is_set;
		QList<Group> groups;
	};

	struct File
	{
		QString fileName;
		QList<Include> includes;
		Config config;
	};

	class parser
	{
	public:
		static bool parse(const QString& filename, File& fi);
		static int getItemsCount(Unit& unit, const QString& id);
		static Param* getParam(Unit& unit, const QString& id);
		//static QList<QString> getConnections(Unit u);
		//static QList<QString> getDependencies(Unit u);

	private:
		//template<typename T> static bool try_get_yaml_value(const QDomElement& node, const std::string& name, T& value);

		static bool get_file(const QDomElement& node, File& file);
		static bool get_includes(const QDomElement& node, QList<Include>& includes);
		static bool get_config(const QDomElement& node, Config& config);
		static bool get_networking(const QDomElement& node, Networking& networking);
		static bool get_log(const QDomElement& node, Log& log);
		static bool get_units(const QDomElement& node, QList<Group>& groups);
		static bool get_group(const QDomElement& node, Group& group);
		static bool get_unit(const QDomElement& node, Unit& unit);
		static bool get_param(const QDomElement& node, Param& param);
		static bool get_array(const QDomElement& node, Array& array);
		static bool get_depends(const QDomElement& node, QList<QString>& depends);
		static bool get_item(const QDomElement& node, const QString& type, Item& item);

		static QList<QDomElement> elementsByTagName(const QDomElement& node, const QString& tagname);
	};
}
