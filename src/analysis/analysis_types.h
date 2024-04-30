#pragma once

#include <QString>
#include "../unit_types.h"

namespace CubesAnalysis
{
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
			Endpoint accept;
			std::vector<Endpoint> connect;
		} main;
		struct
		{
			CubesUnitTypes::IncludeId includeId;
		} include;
	};
}