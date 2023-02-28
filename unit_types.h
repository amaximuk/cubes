#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <regex>

#include <QString>
#include <QSet>

#include "parameters_compiler_types.h"

namespace unit_types
{
	struct UnitParameters
	{
		parameters_compiler::file_info fiileInfo;
		QSet<QString> platforms;
	};

}