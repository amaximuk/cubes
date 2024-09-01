#pragma once

#include "unit_parameter_model_id.h"

namespace CubesUnit
{
	class ParameterModelIds
	{
		// BASE
		// BASE/NAME
		// BASE/PLATFORM
		// BASE/PATH
		// INCLUDES
		// INCLUDES/ITEM_0
		// INCLUDES/ITEM_0/NAME
		// INCLUDES/ITEM_0/VARIABLES
		// INCLUDES/ITEM_0/VARIABLES/ITEM_0/NAME
		// INCLUDES/ITEM_0/VARIABLES/ITEM_0/VALUE
		// PARAMETERS
		// PARAMETERS/NETWORKING
		// PARAMETERS/NETWORKING/ID
		// PARAMETERS/NETWORKING/ACCEPT_PORT
		// PARAMETERS/NETWORKING/KEEP_ALIVE_SEC
		// PARAMETERS/NETWORKING/TIME_CLIENT
		// PARAMETERS/NETWORKING/NETWORK_THREADS
		// PARAMETERS/NETWORKING/BROADCAST_THREADS
		// PARAMETERS/NETWORKING/CLIENTS_THREADS
		// PARAMETERS/NETWORKING/NOTIFY_READY_CLIENTS
		// PARAMETERS/NETWORKING/NOTIFY_READY_SERVERS
		// PARAMETERS/NETWORKING/CONNECT
		// PARAMETERS/NETWORKING/CONNECT/PORT
		// PARAMETERS/NETWORKING/CONNECT/IP
		// PARAMETERS/LOG
		// PARAMETERS/LOG/LOGGING_LEVEL
		// PARAMETERS/LOG/TOTAL_LOG_LIMIT_MB
		// PARAMETERS/LOG/LOG_DIR
		// PARAMETERS/DEPENDENCIES
		
		// PARAMETERS/TEXT
		// PARAMETERS/HEIGHT
		// PARAMETERS/WIDTH

		// EDITOR
		// EDITOR/COLOR

		// BASE
		// BASE/NAME
		// BASE/UNIT_ID
		// BASE/FILE_NAME
		// BASE/INCLUDE_NAME
		// PARAMETERS
		// PARAMETERS/...
		// PARAMETERS/DEPENDENCIES
		// EDITOR
		// EDITOR/POSITION_X
		// EDITOR/POSITION_Y
		// EDITOR/POSITION_Z

	public:
		const CubesUnit::ParameterModelId base;
		const CubesUnit::ParameterModelId name;
		const CubesUnit::ParameterModelId platform;
		const CubesUnit::ParameterModelId path;
		const CubesUnit::ParameterModelId unitId;
		const CubesUnit::ParameterModelId fileName;
		const CubesUnit::ParameterModelId includeName;


		const CubesUnit::ParameterModelId includes;
		const CubesUnit::ParameterModelId item;
		const CubesUnit::ParameterModelId filePath;
		const CubesUnit::ParameterModelId variables;
		const CubesUnit::ParameterModelId value;

		const CubesUnit::ParameterModelId parameters;
		const CubesUnit::ParameterModelId networking;
		const CubesUnit::ParameterModelId id;
		const CubesUnit::ParameterModelId acceptPort;
		const CubesUnit::ParameterModelId keepAliveSec;
		const CubesUnit::ParameterModelId timeClient;
		const CubesUnit::ParameterModelId networkThreads;
		const CubesUnit::ParameterModelId broadcastThreads;
		const CubesUnit::ParameterModelId clientsThreads;
		const CubesUnit::ParameterModelId notifyReadyClients;
		const CubesUnit::ParameterModelId notifyReadyServers;
		const CubesUnit::ParameterModelId connect;
		const CubesUnit::ParameterModelId port;
		const CubesUnit::ParameterModelId ip;
		const CubesUnit::ParameterModelId log;
		const CubesUnit::ParameterModelId loggingLevel;
		const CubesUnit::ParameterModelId totalLogLimitMb;
		const CubesUnit::ParameterModelId logDir;
		const CubesUnit::ParameterModelId dependencies;

		const CubesUnit::ParameterModelId text;
		const CubesUnit::ParameterModelId height;
		const CubesUnit::ParameterModelId width;
		const CubesUnit::ParameterModelId fontSize;
		const CubesUnit::ParameterModelId showBorder;
		const CubesUnit::ParameterModelId hAlignment;
		const CubesUnit::ParameterModelId vAlignment;

		const CubesUnit::ParameterModelId editor;
		const CubesUnit::ParameterModelId color;
		const CubesUnit::ParameterModelId depends;
		const CubesUnit::ParameterModelId optional;
		const CubesUnit::ParameterModelId positionX;
		const CubesUnit::ParameterModelId positionY;
		const CubesUnit::ParameterModelId positionZ;

	public:
		ParameterModelIds() :
			base("$BASE"),
			name("$NAME"),
			platform("$PLATFORM"),
			path("$PATH"),
			unitId("$UNIT_ID"),
			fileName("$FILE_NAME"),
			includeName("$INCLUDE_NAME"),

			includes("$INCLUDES"),
			item("$ITEM"),
			filePath("$FILE_PATH"),
			variables("$VARIABLES"),
			value("$VALUE"),

			parameters("$PARAMETERS"),
			networking("$NETWORKING"),
			id("$ID"),
			acceptPort("$ACCEPT_PORT"),
			keepAliveSec("$KEEP_ALIVE_SEC"),
			timeClient("$TIME_CLIENT"),
			networkThreads("$NETWORK_THREADS"),
			broadcastThreads("$BROADCAST_THREADS"),
			clientsThreads("$CLIENTS_THREADS"),
			notifyReadyClients("$NOTIFY_READY_CLIENTS"),
			notifyReadyServers("$NOTIFY_READY_SERVERS"),
			connect("$CONNECT"),
			port("$PORT"),
			ip("$IP"),
			log("$LOG"),
			loggingLevel("$LOGGING_LEVEL"),
			totalLogLimitMb("$TOTAL_LOG_LIMIT_MB"),
			logDir("$LOG_DIR"),
			dependencies("$DEPENDENCIES"),

			text("$TEXT"),
			height("$HEIGHT"),
			width("$WIDTH"),
			fontSize("$FONT_SIZE"),
			showBorder("$SHOW_BORDER"),
			hAlignment("$H_ALIGNMENT"),
			vAlignment("$V_ALIGNMENT"),

			editor("$EDITOR"),
			color("$COLOR"),
			depends("$DEPENDS"),
			optional("$OPTIONAL"),
			positionX("$POSITION_X"),
			positionY("$POSITION_Y"),
			positionZ("$POSITION_Z")
		{}

	public:
		const CubesUnit::ParameterModelId Item(int n) const
		{
			return QString("%1_%2").arg(item.toString()).arg(n);
		}

		const int ItemIndex(CubesUnit::ParameterModelId id) const
		{
			if (id.empty())
				return -1;

			auto idString = id.mid(0, 1).toString();
			auto itemGroupString = item.toString();
			if (idString.startsWith(itemGroupString)
				&& idString.size() > itemGroupString.size() + 1)
			{
				return idString.mid(itemGroupString.size() + 1).toInt();
			}
			return -1;
		}

		const bool IsItem(CubesUnit::ParameterModelId id) const
		{
			return id.toString().startsWith(item.toString());
		}
	};
}
