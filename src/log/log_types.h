#pragma once

#include <QString>
#include <QMap>

namespace CubesLog
{
    enum class SourceType
    {
        unknown = 0,
        topManager,
        fileManager,
        fileItem,
        propertiesManager,
        propertiesItem,
        xmlHelper,
        xmlParser,
        xmlWriter,
        fileAnalysis,
        propertiesAnalysis,
        __last__
    };


    struct SourceTypeDescription
    {
        QString name;
        QString prefix;
        uint32_t offset;
    };

    using SourceTypeDescriptions = QMap<SourceType, SourceTypeDescription>;

    inline constexpr uint32_t GetSourceTypeCodeOffset(SourceType sourceType)
    {
        if (sourceType == SourceType::unknown)
            return 0;
        else
            return 10000 + 1000 * static_cast<uint32_t>(sourceType);
    }

    inline const SourceTypeDescriptions& GetSourceTypeDescriptions()
    {
        static CubesLog::SourceTypeDescriptions descriptions;
        if (descriptions.empty())
        {
            descriptions[SourceType::unknown] = { QString::fromLocal8Bit("-"), QString::fromLocal8Bit("U"), GetSourceTypeCodeOffset(SourceType::unknown)};
            descriptions[SourceType::topManager] = { QString::fromLocal8Bit("Top Manager"), QString::fromLocal8Bit("TM"), GetSourceTypeCodeOffset(SourceType::topManager) };
            descriptions[SourceType::fileManager] = { QString::fromLocal8Bit("File Manager"), QString::fromLocal8Bit("FM"), GetSourceTypeCodeOffset(SourceType::fileManager) };
            descriptions[SourceType::fileItem] = { QString::fromLocal8Bit("File Item"), QString::fromLocal8Bit("FI"), GetSourceTypeCodeOffset(SourceType::fileItem) };
            descriptions[SourceType::propertiesManager] = { QString::fromLocal8Bit("Properties Manager"), QString::fromLocal8Bit("PM"), GetSourceTypeCodeOffset(SourceType::propertiesManager) };
            descriptions[SourceType::propertiesItem] = { QString::fromLocal8Bit("Properties Item"), QString::fromLocal8Bit("PI"), GetSourceTypeCodeOffset(SourceType::propertiesItem) };
            descriptions[SourceType::xmlHelper] = { QString::fromLocal8Bit("XML Helper"), QString::fromLocal8Bit("XH"), GetSourceTypeCodeOffset(SourceType::xmlHelper) };
            descriptions[SourceType::xmlParser] = { QString::fromLocal8Bit("XML Parser"), QString::fromLocal8Bit("XP"), GetSourceTypeCodeOffset(SourceType::xmlParser) };
            descriptions[SourceType::xmlWriter] = { QString::fromLocal8Bit("XML Writer"), QString::fromLocal8Bit("XW"), GetSourceTypeCodeOffset(SourceType::xmlWriter) };
            descriptions[SourceType::fileAnalysis] = { QString::fromLocal8Bit("File Analysis"), QString::fromLocal8Bit("FA"), GetSourceTypeCodeOffset(SourceType::fileAnalysis) };
            descriptions[SourceType::propertiesAnalysis] = { QString::fromLocal8Bit("Properties Analysis"), QString::fromLocal8Bit("PA"), GetSourceTypeCodeOffset(SourceType::propertiesAnalysis) };
        }

        assert((static_cast<uint32_t>(SourceType::__last__) - static_cast<uint32_t>(SourceType::unknown)) == descriptions.size());

        return descriptions;
    }

    inline SourceTypeDescription GetSourceTypeDescription(SourceType sourceType)
    {
        const auto& descriptions = GetSourceTypeDescriptions();
        if (descriptions.contains(sourceType))
            return descriptions[sourceType];
        return descriptions[SourceType::unknown];
    }

    inline QString SourceTypeToString(SourceType sourceType)
    {
        return GetSourceTypeDescription(sourceType).name;
    }

    inline QString GetSourceTypePrefix(SourceType sourceType)
    {
        return GetSourceTypeDescription(sourceType).prefix;
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
    constexpr BaseErrorCode SuccessErrorCode = 0;
    constexpr char* DefaultDescription = "";
    constexpr char* DefaultDetails = "";

    using BaseErrorCodeDescriptions = QMap<BaseErrorCode, QString>;
    using BaseErrorCodeDetails = QMap<BaseErrorCode, QString>;

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
