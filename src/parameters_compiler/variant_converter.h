#pragma once

#include <QVariant>
#include "parameters/helper_parameter.h"

namespace CubesParameters
{
	inline bool convert_variant(const parameters::variant v, QVariant& qv)
	{
		switch (v.get_type())
        {
        case parameters::base_types::string:
            qv = QString::fromStdString(v.get<std::string>());
            break;
        case parameters::base_types::integer:
            qv = v.get<int>();
            break;
        case parameters::base_types::floating:
            qv = v.get<double>();
            break;
        case parameters::base_types::bool_:
            qv = v.get<bool>();
            break;
        default:
            return false;
        }
        return true;
	}
}
