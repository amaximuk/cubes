#pragma once

#include <QtXml>
#include "../log/log_manager_interface.h"
#include "xml_types.h"

namespace CubesXml
{
	class Writer
	{
	private:
		const CubesUnitTypes::ParameterModelIds ids_;
		CubesLog::ILogManager* logManager_;
		File fi_;

	public:
		Writer(CubesLog::ILogManager* logManager = nullptr);

	public:
		bool Write(QByteArray& buffer, const File& fi);
		bool Write(const QString& filename, const File& fi);

	private:
		bool SetFile(const File& file, QXmlStreamWriter& xmlWriter);
		bool SetIncludes(const QList<Include>& includes, QXmlStreamWriter& xmlWriter);
		bool SetConfig(const Config& config, const QString& name, const QString& platform,
			const QString& color, QXmlStreamWriter& xmlWriter);
		bool SetNetworking(const Networking& networking, QXmlStreamWriter& xmlWriter);
		bool SetLog(const Log& log, QXmlStreamWriter& xmlWriter);
		bool SetUnits(const QList<Group>& groups, QXmlStreamWriter& xmlWriter);
		bool SetGroup(const Group& group, QXmlStreamWriter& xmlWriter);
		bool SetUnit(const Unit& unit, QXmlStreamWriter& xmlWriter);
		bool SetParam(const Param& param, QXmlStreamWriter& xmlWriter);
		bool SetArray(const Array& array, QXmlStreamWriter& xmlWriter);
		bool SetDepends(const QList<QString>& depends, QXmlStreamWriter& xmlWriter);
		bool SetItem(const Item& item, QXmlStreamWriter& xmlWriter);

	private:
		void LogError(CubesXml::WriterErrorCode errorCode, const QString& message);
	};
}
