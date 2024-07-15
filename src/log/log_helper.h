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
        // Error

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

        void LogError(BaseErrorCode errorCode, const QString& description, const QString& details,
            const QVector<CubesLog::Variable>& variables)
        {
            LogError(errorCode, description, details, variables, CubesUnitTypes::InvalidBaseId);
        }

        void LogError(BaseErrorCode errorCode, const QString& description, const QString& details,
            CubesUnitTypes::BaseId tag)
        {
            LogError(errorCode, description, details, {}, tag);
        }

        void LogError(BaseErrorCode errorCode, const QString& description, const QString& details)
        {
            LogError(errorCode, description, details, {}, CubesUnitTypes::InvalidBaseId);
        }

        void LogError(BaseErrorCode errorCode, const QVector<CubesLog::Variable>& variables,
            CubesUnitTypes::BaseId tag)
        {
            LogError(errorCode, {}, {}, variables, tag);
        }

        void LogError(BaseErrorCode errorCode, const QVector<CubesLog::Variable>& variables)
        {
            LogError(errorCode, {}, {}, variables, CubesUnitTypes::InvalidBaseId);
        }

        void LogError(BaseErrorCode errorCode, CubesUnitTypes::BaseId tag)
        {
            LogError(errorCode, {}, {}, {}, tag);
        }

        void LogError(BaseErrorCode errorCode)
        {
            LogError(errorCode, {}, {}, {}, CubesUnitTypes::InvalidBaseId);
        }

        // Information

        void LogInformation(BaseErrorCode errorCode, const QString& description, const QString& details,
            const QVector<CubesLog::Variable>& variables, CubesUnitTypes::BaseId tag)
        {
            if (logManager_ != nullptr)
            {
                CubesLog::Message lm{};
                lm.type = CubesLog::MessageType::information;
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

        void LogInformation(BaseErrorCode errorCode, const QString& description, const QString& details,
            const QVector<CubesLog::Variable>& variables)
        {
            LogInformation(errorCode, description, details, variables, CubesUnitTypes::InvalidBaseId);
        }

        void LogInformation(BaseErrorCode errorCode, const QString& description, const QString& details,
            CubesUnitTypes::BaseId tag)
        {
            LogInformation(errorCode, description, details, {}, tag);
        }

        void LogInformation(BaseErrorCode errorCode, const QString& description, const QString& details)
        {
            LogInformation(errorCode, description, details, {}, CubesUnitTypes::InvalidBaseId);
        }

        void LogInformation(BaseErrorCode errorCode, const QVector<CubesLog::Variable>& variables,
            CubesUnitTypes::BaseId tag)
        {
            LogInformation(errorCode, {}, {}, variables, tag);
        }

        void LogInformation(BaseErrorCode errorCode, const QVector<CubesLog::Variable>& variables)
        {
            LogInformation(errorCode, {}, {}, variables, CubesUnitTypes::InvalidBaseId);
        }

        void LogInformation(BaseErrorCode errorCode, CubesUnitTypes::BaseId tag)
        {
            LogInformation(errorCode, {}, {}, {}, tag);
        }

        void LogInformation(BaseErrorCode errorCode)
        {
            LogInformation(errorCode, {}, {}, {}, CubesUnitTypes::InvalidBaseId);
        }
    };
}
