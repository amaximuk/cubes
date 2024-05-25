#pragma once

#include "log_types.h"

namespace CubesLog
{
    class ILogManager
    {
    public:
        virtual ~ILogManager() = default;

    public:
        virtual void AddMessage(const Message& m) = 0;
    };
}
