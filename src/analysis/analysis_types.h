#pragma once

#include <QString>
#include "../unit_types.h"

namespace CubesAnalysis
{
	using RuleId = uint32_t;
	constexpr RuleId InvalidRuleId = 0;

	struct Rule
	{
		RuleId id;
		QString name;
		QString description;
		bool isActive;

		Rule()
		{
			id = InvalidRuleId;
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
			int id;
			int port;
			std::vector<Endpoint> connect;
		} main;
		struct
		{
			CubesUnitTypes::IncludeId includeId;
		} include;
	};
}