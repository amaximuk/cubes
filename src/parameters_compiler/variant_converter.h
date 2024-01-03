#pragma once

#include <QVariant>
#include "yaml_helper_parameter.h"

namespace CubesParameters
{
	inline bool convert_variant(const yaml::helper::variant v, QVariant& qv)
	{
		switch (v.get_type())
        {
        case yaml::base_types::string:
            qv = QString::fromStdString(v.get<std::string>());
            break;
        case yaml::base_types::integer:
            qv = v.get<int>();
            break;
        case yaml::base_types::floating:
            qv = v.get<double>();
            break;
        case yaml::base_types::bool_:
            qv = v.get<bool>();
            break;
        default:
            return false;
        }
        return true;
	}
}
