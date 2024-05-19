#pragma once

#include "log_table_types.h"

namespace CubesLog
{
    class ILogManager
    {
    public:
        virtual ~ILogManager() = default;

    public:
        virtual void AddMessage(const LogMessage& m) = 0;
    };
}