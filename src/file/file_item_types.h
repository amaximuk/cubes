#pragma once

#include <QString>
#include "../unit/unit_types.h"
#include "../log/log_types.h"

namespace CubesFile
{
	struct Connect
	{
		uint16_t port;
		QString ip;
	};

	struct Networking
	{
		uint32_t id;
		uint16_t accept_port;
		uint32_t keep_alive_sec;
		bool time_client;
		uint32_t network_threads;
		uint32_t broadcast_threads;
		uint32_t clients_threads;
		bool notify_ready_clients;
		bool notify_ready_servers;
		QList<Connect> connect;
	};

	enum LoggingLevel
	{
		LOG_TRACE,
		LOG_DEBUG,
		LOG_INFO,
		LOG_WARNING,
		LOG_ERROR,
		LOG_FATAL
	};

	struct Log
	{
		LoggingLevel level;
		uint32_t limit_mb;
		QString directory_path;
	};

	struct Include
	{
		QString name;
		QString path;
		QList<QPair<QString, QString>> variables;
	};

	struct File
	{
		QString name;
		QString platform;
		QString path;
		Networking network;
		Log log;
		QList<Include> includes;
	};

	enum class FileManagerErrorCode : CubesLog::BaseErrorCode
	{
		success = CubesLog::SuccessErrorCode,
		itemCreated = CubesLog::GetSourceTypeCodeOffset(CubesLog::SourceType::fileManager),
		itemRemoved,
		allItemsRemoved,
		last
	};

	inline const CubesLog::BaseErrorCodeDescriptions& GetFileManagerErrorDescriptions()
	{
		static CubesLog::BaseErrorCodeDescriptions descriptions;
		if (descriptions.empty())
		{
			descriptions[static_cast<CubesLog::BaseErrorCode>(FileManagerErrorCode::success)] = QString::fromLocal8Bit("Успех");
			descriptions[static_cast<CubesLog::BaseErrorCode>(FileManagerErrorCode::itemCreated)] = QString::fromLocal8Bit("Файл создан (FileItem)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(FileManagerErrorCode::itemRemoved)] = QString::fromLocal8Bit("Файл удален (FileItem)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(FileManagerErrorCode::allItemsRemoved)] = QString::fromLocal8Bit("Все файлы удалены (FileItem)");
		}

		assert((static_cast<CubesLog::BaseErrorCode>(FileManagerErrorCode::last) - CubesLog::GetSourceTypeCodeOffset(CubesLog::SourceType::fileManager) + 1) == descriptions.size());

		return descriptions;
	}

	inline QString GetFileManagerErrorDescription(FileManagerErrorCode errorCode)
	{
		const auto& descriptions = GetFileManagerErrorDescriptions();
		if (descriptions.contains(static_cast<CubesLog::BaseErrorCode>(errorCode)))
			return descriptions[static_cast<CubesLog::BaseErrorCode>(errorCode)];
		return QString("%1").arg(static_cast<uint32_t>(errorCode));
	}
}