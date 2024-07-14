#pragma once

#include <QString>
#include <QMap>
#include "../unit/unit_types.h"
#include "log_types.h"
#include "log_manager_interface.h"

namespace CubesLog
{
    class LogHelper
    {
    private:
        CubesLog::ILogManager* logManager_;
        CubesLog::SourceType sourceType_;
        QMap<BaseErrorCode, QString> descriptions_;
        QMap<BaseErrorCode, QString> details_;

    public:
        LogHelper(ILogManager* logManager, SourceType sourceType,
            const QMap<BaseErrorCode, QString>& descriptions,
            const QMap<BaseErrorCode, QString>& detailes)
        {
            logManager_ = logManager;
            sourceType_ = sourceType;
            descriptions_ = descriptions;
        }
        
        LogHelper(ILogManager* logManager, SourceType sourceType,
            const QMap<BaseErrorCode, QString>& descriptions):
            LogHelper(logManager, sourceType, descriptions, {})
        {}

        LogHelper(ILogManager* logManager, SourceType sourceType):
            LogHelper(logManager, sourceType, {}, {})
        {}

    public:
        void LogError(BaseErrorCode errorCode, const QString& description, const QString& details,
            const QVector<CubesLog::Variable>& variables, CubesUnitTypes::BaseId tag)
        {
            if (logManager_ != nullptr)
            {
                CubesLog::Message lm{};
                lm.type = CubesLog::MessageType::error;
                lm.code = CubesLog::CreateCode(CubesLog::MessageType::error, sourceType_, errorCode);
                lm.source = sourceType_;
                if (description.isEmpty() && descriptions_.contains(errorCode))
                    lm.description = descriptions_[errorCode];
                else
                    lm.description = description;
                if (details.isEmpty() && details_.contains(errorCode))
                    lm.details = details_[errorCode];
                else
                    lm.details = details;
                lm.variables = variables;
                lm.tag = tag;
                logManager_->AddMessage(lm);
            }
        }

        void LogError(BaseErrorCode errorCode, const QVector<CubesLog::Variable>& variables, CubesUnitTypes::BaseId tag)
        {
            LogError(errorCode, {}, {}, variables, tag);
        }

        void LogError(BaseErrorCode errorCode)
        {
            LogError(errorCode, {}, {}, {}, CubesUnitTypes::InvalidBaseId);
        }
    };

    //void LogError(const Rule& rule, const QVector<CubesLog::Variable>& variables, uint32_t tag)
    //{
    //    CubesLog::Message lm{};
    //    lm.type = CubesLog::MessageType::error;
    //    lm.code = CubesLog::CreateCode(CubesLog::MessageType::error,
    //        CubesLog::SourceType::fileAnalysis, static_cast<uint32_t>(rule.errorCode));
    //    lm.source = CubesLog::SourceType::fileAnalysis;
    //    lm.description = rule.description;
    //    lm.details = rule.detailes;
    //    lm.variables = variables;
    //    lm.tag = tag;
    //    logManager_->AddMessage(lm);
    //}
  /*  if (logManager_ != nullptr)
    {
        CubesLog::Message lm{};
        lm.type = CubesLog::MessageType::error;
        lm.code = CubesLog::CreateCode(CubesLog::MessageType::error,
            CubesLog::SourceType::xmlParser, static_cast<uint32_t>(errorCode));
        lm.source = CubesLog::SourceType::xmlParser;
        lm.description = CubesXml::GetParserErrorDescription(errorCode);
        lm.details = details;
        lm.variables = variables;
        lm.tag = CubesUnitTypes::InvalidBaseId;
        logManager_->AddMessage(lm);
    }
    
        CubesLog::Message lm{};
    lm.type = CubesLog::MessageType::error;
    lm.code = CubesLog::CreateCode(CubesLog::MessageType::error,
        CubesLog::SourceType::topManager, static_cast<uint32_t>(errorCode));
    lm.source = CubesLog::SourceType::topManager;
    lm.description = GetTopManagerErrorDescription(errorCode);
    lm.details = details;
    lm.variables = variables;
    lm.tag = CubesUnitTypes::InvalidBaseId;
    AddMessage(lm);
    
    */
}
