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
		unitCategoryMismatch,
		unitIdMismatch,
		cyclicDependency,
		__last__
	};

	struct Rule
	{
		CubesLog::BaseErrorCode errorCode;
		CubesLog::MessageType type;
		QString description;
		QString detailes;
		bool isActive;

		Rule()
		{
			errorCode = 0;
			type = CubesLog::MessageType::information;
			isActive = true;
		}
	};

	inline QMap<CubesLog::BaseErrorCode, CubesLog::MessageType> GetRuleTypes(const QVector<Rule>& rules)
	{
		QMap<CubesLog::BaseErrorCode, CubesLog::MessageType> result;
		result[CubesLog::SuccessErrorCode] = CubesLog::MessageType::information;
		for (const auto& rule : rules)
			result[rule.errorCode] = rule.type;

		return result;
	}

	inline QMap<CubesLog::BaseErrorCode, QString> GetRuleDescriptions(const QVector<Rule>& rules)
	{
		QMap<CubesLog::BaseErrorCode, QString> result;
		result[CubesLog::SuccessErrorCode] = "Успех";
		for (const auto& rule : rules)
			result[rule.errorCode] = rule.description;

		return result;
	}

	inline QMap<CubesLog::BaseErrorCode, QString> GetRuleDetailes(const QVector<Rule>& rules)
	{
		QMap<CubesLog::BaseErrorCode, QString> result;
		result[CubesLog::SuccessErrorCode] = "Успех";
		for (const auto& rule : rules)
			result[rule.errorCode] = rule.detailes;

		return result;
	}

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
			CubesUnit::FileId fileId;
			QString name;
			int id;
			int port;
			std::vector<Endpoint> connect;
		} main;
		struct
		{
			CubesUnit::IncludeId includeId;
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
		CubesUnit::PropertiesId propertiesId;
		QString unitId;
		QString name;
		QVector<UnitProperty> connections;
		QVector<QString> dependencies;
	};
}