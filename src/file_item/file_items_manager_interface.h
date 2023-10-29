#pragma once

namespace CubesFile
{
	struct Connect
	{
		uint16_t port;
		QString ip;
	};

	struct Network
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
		Network network;
		Log log;
		QList<Include> includes;
	};

	class IFileItemsManager
	{
	public:
		// “ут значени€ мен€ютс€ по одному
		virtual void BeforeFileNameChanged(const QString& fileName, const QString& oldFileName, bool& cancel) = 0;
		virtual void AfterFileNameChanged(const QString& fileName, const QString& oldFileName) = 0;
		virtual void BeforeIncludeNameChanged(const QString& fileName, const QString& includeName, const QString& oldIncludeName, bool& cancel) = 0;
		virtual void AfterIncludeNameChanged(const QString& fileName, const QString& includeName, const QString& oldIncludeName) = 0;

		// “ут могут мен€тьс€ значени€ не по одному, а сразу список, поэтому передаем сразу все имена
		virtual void BeforeIncludesAdd(const QString& fileName, const QStringList& includeNames, bool& cancel) = 0;
		virtual void BeforeIncludesRemoved(const QString& fileName, const QStringList& includeNames, bool& cancel) = 0;
		virtual void AfterIncludesListChanged(const QString& fileName, const QStringList& includeNames) = 0;
		//virtual void AfterVariableChanged(const QString& fileName, const QString& includeName, const QList<QPair<QString, QString>>& variables) = 0;
		
		virtual void AfterVariableNameChanged(const QString& fileName, const QString& includeName, const QString& variableName, const QString& oldVariableName) = 0;
		virtual void AfterVariablesListChanged(const QString& fileName, const QString& includeName, const QList<QPair<QString, QString>>& variables) = 0;
	};
}
