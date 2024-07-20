#pragma once

#include <QString>
#include "../unit/unit_types.h"
#include "../log/log_types.h"

namespace CubesAnalysis
{
	//using RuleId = uint32_t;
	//constexpr RuleId InvalidRuleId = 0;
	enum class FileAnalysisErrorCode
	{
		success = CubesLog::SuccessErrorCode,
		noFiles = CubesLog::GetSourceTypeCodeOffset(CubesLog::SourceType::fileAnalysis),
		nameIsEmpty,
		nameNotUnique,
		includeNameIsEmpty,
		includeNameNotUnique,
		fileNameIsEmpty,
		fileNameNotUnique,
		connectionIdNotUnique,
		__last__
	};

	enum class PropertiesAnalysisErrorCode
	{
		success = CubesLog::SuccessErrorCode,
		nameIsEmpty = CubesLog::GetSourceTypeCodeOffset(CubesLog::SourceType::propertiesAnalysis),
		nameNotUnique,
		__last__
	};

	struct Rule
	{
		CubesLog::BaseErrorCode errorCode;
		QString description;
		QString detailes;
		bool isActive;

		Rule()
		{
			errorCode = 0;
			isActive = true;
		}
	};

	struct Endpoint
	{
		QString host;
		int port;
	};

	struct File
	{
		QString path;
		bool is_include;
		struct
		{
			CubesUnitTypes::FileId fileId;
			QString name;
			int id;
			int port;
			std::vector<Endpoint> connect;
		} main;
		struct
		{
			CubesUnitTypes::IncludeId includeId;
			QString name;
		} include;
	};

	struct UnitProperty
	{
		QString id;
		QString name;
		bool depends;
		bool dontSet;
		QString category;
		QStringList ids;
	};

	struct Properties
	{
		CubesUnitTypes::PropertiesId propertiesId;
		QString unitId;
		QString name;
		QVector<UnitProperty> connections;
		QVector<QString> dependencies;
	};
}