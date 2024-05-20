#pragma once

namespace CubesLog
{
    enum class SourceType
    {
        unknown = 0,
        topManager,
        fileManager,
        propertiesManager,
        xmlHelper,
        xmlParser,
        xmlWriter,
        fileAnalysis,
        propertiesAnalysis
    };

    inline QString SourceTypeToStaring(SourceType type)
    {
        switch (type)
        {
        case SourceType::unknown:
            return "-";
        case SourceType::topManager:
            return "Top Manager";
        case SourceType::fileManager:
            return "File Manager";
        case SourceType::propertiesManager:
            return "Properties Manager";
        case SourceType::xmlHelper:
            return "XML Helper";
        case SourceType::xmlParser:
            return "XML Parser";
        case SourceType::fileAnalysis:
            return "File Analysis";
        case SourceType::propertiesAnalysis:
            return "Properties Analysis";
        default:
            return QString("%1").arg(static_cast<int>(type));
        }
    }

    inline uint qHash(SourceType key, uint seed)
    {
        return ::qHash(static_cast<uint>(key), seed);
    }

    enum class MessageType
    {
        information,
        warning,
        error
    };

    //enum class MessageType
    //{
    //    information = 1,
    //    warning = 2,
    //    error = 4
    //};

    //inline MessageType operator|(MessageType lhs, MessageType rhs)
    //{
    //    return static_cast<MessageType>(
    //        static_cast<std::underlying_type<MessageType>::type>(lhs) |
    //        static_cast<std::underlying_type<MessageType>::type>(rhs));
    //}

    inline uint qHash(MessageType key, uint seed)
    {
        return ::qHash(static_cast<uint>(key), seed);
    }

    struct LogMessage
    {
        MessageType type;
        QString code;
        SourceType source;
        QString description;
        uint32_t tag;
    };
}
