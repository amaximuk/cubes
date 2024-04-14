#pragma once

namespace CubesLog
{
    enum class MessageType
    {
        information = 1,
        warning = 2,
        error = 4
    };

    inline MessageType operator|(MessageType lhs, MessageType rhs)
    {
        return static_cast<MessageType>(
            static_cast<std::underlying_type<MessageType>::type>(lhs) |
            static_cast<std::underlying_type<MessageType>::type>(rhs));
    }

    inline uint qHash(MessageType key, uint seed)
    {
        return ::qHash(static_cast<uint>(key), seed);
    }

    struct LogMessage
    {
        MessageType type;
        QString source;
        QString description;
    };

    class ILogManager
    {
    public:
        virtual ~ILogManager() = default;

    public:
        virtual void AddMessage(const LogMessage& m) = 0;
    };
}
