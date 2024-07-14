#pragma once

#include <QString>

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
        case SourceType::xmlWriter:
            return "XML Writer";
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
        case SourceType::xmlWriter:
            return "XW";
        case SourceType::fileAnalysis:
            return "FA";
        case SourceType::propertiesAnalysis:
            return "PA";
        default:
            return QString("%1").arg(static_cast<int>(sourceType));
        }
    }

    inline constexpr uint32_t GetSourceTypeCodeOffset(SourceType sourceType)
    {
        if (sourceType == SourceType::unknown)
            return 0;
        else if (sourceType == SourceType::topManager)
            return 10000;
        else if (sourceType == SourceType::fileManager)
            return 20000;
        else if (sourceType == SourceType::propertiesManager)
            return 30000;
        else if (sourceType == SourceType::xmlHelper)
            return 40000;
        else if (sourceType == SourceType::xmlParser)
            return 50000;
        else if (sourceType == SourceType::xmlWriter)
            return 60000;
        else if (sourceType == SourceType::fileAnalysis)
            return 70000;
        else if (sourceType == SourceType::propertiesAnalysis)
            return 80000;
        else
            return 0;
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

    using BaseErrorCode = uint32_t;
    constexpr BaseErrorCode NoErrorCode = 0;

    inline QString CreateCode(MessageType messageType, SourceType sourceType, BaseErrorCode id)
    {
        return QString("%1%2%3").arg(GetMessageTypePrefix(messageType)).arg(GetSourceTypePrefix(sourceType)).arg(id);
    }

    struct Variable
    {
        QString name;
        QString value;
    };

    struct Message
    {
        MessageType type;
        QString code;
        SourceType source;
        QString description;
        QString details;
        QVector<Variable> variables;
        uint32_t tag;
    };
}
