#pragma once

#include <QtXml>
#include "../unit/unit_parameter_model_ids.h"
#include "../log/log_manager_interface.h"
#include "xml_types.h"

namespace CubesLog { class LogHelper; }

namespace CubesXml
{
	class Parser
	{
	private:
		const CubesUnit::ParameterModelIds ids_;
		CubesLog::ILogManager* logManager_;
		File fi_;

		// Log
		QSharedPointer<CubesLog::LogHelper> logHelper_;

	public:
		Parser(CubesLog::ILogManager* logManager = nullptr);

	public:
		bool Parse(QByteArray& byteArray, const QString& fileName);
		bool Parse(const QString& fileName);
		const File& GetFile();

	private:
		bool GetFile(const QDomElement& node, File& file);
		bool GetIncludes(const QDomElement& node, QList<Include>& includes);
		bool GetConfig(const QDomElement& node, QString& name, QString& platform, QString& color, Config& config);
		bool GetNetworking(const QDomElement& node, Networking& networking);
		bool GetLog(const QDomElement& node, Log& log);
		bool GetUnits(const QDomElement& node, QList<Group>& groups);
		bool GetGroup(const QDomElement& node, Group& group);
		bool GetUnit(const QDomElement& node, Unit& unit);
		bool GetParam(const QDomElement& node, Param& param);
		bool GetArray(const QDomElement& node, Array& array);
		bool GetDepends(const QDomElement& node, QList<QString>& depends);
		bool GetItem(const QDomElement& node, Item& item);

		QList<QDomElement> ElementsByTagName(const QDomElement& node, const QString& tagname);
	};
}
