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

    inline QString SourceTypeToString(SourceType sourceType)
    {
        switch (sourceType)
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
            return QString("%1").arg(static_cast<int>(sourceType));
        }
    }

    inline QString GetSourceTypePrefix(SourceType sourceType)
    {
        switch (sourceType)
        {
        case SourceType::unknown:
            return "U";
        case SourceType::topManager:
            return "TM";
        case SourceType::fileManager:
            return "FM";
        case SourceType::propertiesManager:
            return "PM";
        case SourceType::xmlHelper:
            return "XH";
        case SourceType::xmlParser:
            return "XP";
        case SourceType::fileAnalysis:
            return "FA";
        case SourceType::propertiesAnalysis:
            return "PA";
        default:
            return QString("%1").arg(static_cast<int>(sourceType));
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

    inline QString GetMessageTypePrefix(MessageType messageType)
    {
        switch (messageType)
        {
        case MessageType::information:
            return "I";
        case MessageType::warning:
            return "W";
        case MessageType::error:
            return "E";
        default:
            return QString("%1").arg(static_cast<int>(messageType));
        }
    }

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

    inline QString CreateCode(MessageType messageType, SourceType sourceType, uint32_t id)
    {
        return QString("%1%2%3").arg(GetMessageTypePrefix(messageType)).arg(GetSourceTypePrefix(sourceType)).arg(id);
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
