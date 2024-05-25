#pragma once

#include <QtXml>
#include "../log/log_manager_interface.h"
#include "../unit/unit_types.h"
#include "xml_types.h"

namespace CubesXml
{
	namespace Helper
	{
		bool Parse(QByteArray& byteArray, const QString& fileName, File& fi, CubesLog::ILogManager* logManager = nullptr);
		bool Parse(const QString& fileName, File& fi, CubesLog::ILogManager* logManager = nullptr);
		bool GetElement(Unit& unit, const CubesUnitTypes::ParameterModelId& id, Element& element, CubesLog::ILogManager* logManager = nullptr);
		//int GetItemsCount(Unit& unit, const CubesUnitTypes::ParameterModelId& id, CubesLog::ILogManager* logManager = nullptr);
		//Param* GetParam(Unit& unit, const CubesUnitTypes::ParameterModelId& id, CubesLog::ILogManager* logManager = nullptr);
		//Item* GetItem(Unit& unit, const CubesUnitTypes::ParameterModelId& id, QString& type, CubesLog::ILogManager* logManager = nullptr);
	
		bool Write(QByteArray& buffer, const File& fi, CubesLog::ILogManager* logManager = nullptr);
		bool Write(const QString& filename, const File& fi, CubesLog::ILogManager* logManager = nullptr);
	};
}
