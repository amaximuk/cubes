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
        ILogManager* logManager_;
        SourceType sourceType_;
        QMap<BaseErrorCode, QString> descriptions_;
        QMap<BaseErrorCode, QString> details_;
        QMap<BaseErrorCode, MessageType> types_;

    public:
        LogHelper(ILogManager* logManager, SourceType sourceType,
            const QMap<BaseErrorCode, QString>& descriptions,
            const QMap<BaseErrorCode, QString>& detailes,
            const QMap<BaseErrorCode, MessageType>& types)
        {
            logManager_ = logManager;
            sourceType_ = sourceType;
            descriptions_ = descriptions;
            types_ = types;
        }

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
        // Information

        void LogInformation(BaseErrorCode errorCode, const QString& description, const QString& details,
            const QVector<Variable>& variables, CubesUnit::BaseId tag)
        {
            if (logManager_ != nullptr)
            {
                Message lm{};
                lm.type = MessageType::information;
                lm.code = CreateCode(lm.type, sourceType_, errorCode);
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
            const QVector<Variable>& variables)
        {
            LogInformation(errorCode, description, details, variables, CubesUnit::InvalidBaseId);
        }

        void LogInformation(BaseErrorCode errorCode, const QString& description, const QString& details,
            CubesUnit::BaseId tag)
        {
            LogInformation(errorCode, description, details, {}, tag);
        }

        void LogInformation(BaseErrorCode errorCode, const QString& description, const QString& details)
        {
            LogInformation(errorCode, description, details, {}, CubesUnit::InvalidBaseId);
        }

        void LogInformation(BaseErrorCode errorCode, const QVector<Variable>& variables,
            CubesUnit::BaseId tag)
        {
            LogInformation(errorCode, {}, {}, variables, tag);
        }

        void LogInformation(BaseErrorCode errorCode, const QVector<Variable>& variables)
        {
            LogInformation(errorCode, {}, {}, variables, CubesUnit::InvalidBaseId);
        }

        void LogInformation(BaseErrorCode errorCode, CubesUnit::BaseId tag)
        {
            LogInformation(errorCode, {}, {}, {}, tag);
        }

        void LogInformation(BaseErrorCode errorCode)
        {
            LogInformation(errorCode, {}, {}, {}, CubesUnit::InvalidBaseId);
        }

        // Warning

        void LogWarning(BaseErrorCode errorCode, const QString& description, const QString& details,
            const QVector<Variable>& variables, CubesUnit::BaseId tag)
        {
            if (logManager_ != nullptr)
            {
                Message lm{};
                lm.type = MessageType::warning;
                lm.code = CreateCode(lm.type, sourceType_, errorCode);
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

        void LogWarning(BaseErrorCode errorCode, const QString& description, const QString& details,
            const QVector<Variable>& variables)
        {
            LogWarning(errorCode, description, details, variables, CubesUnit::InvalidBaseId);
        }

        void LogWarning(BaseErrorCode errorCode, const QString& description, const QString& details,
            CubesUnit::BaseId tag)
        {
            LogWarning(errorCode, description, details, {}, tag);
        }

        void LogWarning(BaseErrorCode errorCode, const QString& description, const QString& details)
        {
            LogWarning(errorCode, description, details, {}, CubesUnit::InvalidBaseId);
        }

        void LogWarning(BaseErrorCode errorCode, const QVector<Variable>& variables,
            CubesUnit::BaseId tag)
        {
            LogWarning(errorCode, {}, {}, variables, tag);
        }

        void LogWarning(BaseErrorCode errorCode, const QVector<Variable>& variables)
        {
            LogWarning(errorCode, {}, {}, variables, CubesUnit::InvalidBaseId);
        }

        void LogWarning(BaseErrorCode errorCode, CubesUnit::BaseId tag)
        {
            LogWarning(errorCode, {}, {}, {}, tag);
        }

        void LogWarning(BaseErrorCode errorCode)
        {
            LogWarning(errorCode, {}, {}, {}, CubesUnit::InvalidBaseId);
        }

        // Error

        void LogError(BaseErrorCode errorCode, const QString& description, const QString& details,
            const QVector<Variable>& variables, CubesUnit::BaseId tag)
        {
            if (logManager_ != nullptr)
            {
                Message lm{};
                lm.type = MessageType::error;
                lm.code = CreateCode(lm.type, sourceType_, errorCode);
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
            const QVector<Variable>& variables)
        {
            LogError(errorCode, description, details, variables, CubesUnit::InvalidBaseId);
        }

        void LogError(BaseErrorCode errorCode, const QString& description, const QString& details,
            CubesUnit::BaseId tag)
        {
            LogError(errorCode, description, details, {}, tag);
        }

        void LogError(BaseErrorCode errorCode, const QString& description, const QString& details)
        {
            LogError(errorCode, description, details, {}, CubesUnit::InvalidBaseId);
        }

        void LogError(BaseErrorCode errorCode, const QVector<Variable>& variables,
            CubesUnit::BaseId tag)
        {
            LogError(errorCode, {}, {}, variables, tag);
        }

        void LogError(BaseErrorCode errorCode, const QVector<Variable>& variables)
        {
            LogError(errorCode, {}, {}, variables, CubesUnit::InvalidBaseId);
        }

        void LogError(BaseErrorCode errorCode, CubesUnit::BaseId tag)
        {
            LogError(errorCode, {}, {}, {}, tag);
        }

        void LogError(BaseErrorCode errorCode)
        {
            LogError(errorCode, {}, {}, {}, CubesUnit::InvalidBaseId);
        }

        // Auto MessageType

        void Log(BaseErrorCode errorCode, const QString& description, const QString& details,
            const QVector<Variable>& variables, CubesUnit::BaseId tag)
        {
            if (logManager_ != nullptr)
            {
                Message lm{};
                if (types_.contains(errorCode))
                    lm.type = types_[errorCode];
                else
                    lm.type = MessageType::error;
                lm.code = CreateCode(lm.type, sourceType_, errorCode);
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

        void Log(BaseErrorCode errorCode, const QString& description, const QString& details,
            const QVector<Variable>& variables)
        {
            Log(errorCode, description, details, variables, CubesUnit::InvalidBaseId);
        }

        void Log(BaseErrorCode errorCode, const QString& description, const QString& details,
            CubesUnit::BaseId tag)
        {
            Log(errorCode, description, details, {}, tag);
        }

        void Log(BaseErrorCode errorCode, const QString& description, const QString& details)
        {
            Log(errorCode, description, details, {}, CubesUnit::InvalidBaseId);
        }

        void Log(BaseErrorCode errorCode, const QVector<Variable>& variables,
            CubesUnit::BaseId tag)
        {
            Log(errorCode, {}, {}, variables, tag);
        }

        void Log(BaseErrorCode errorCode, const QVector<Variable>& variables)
        {
            Log(errorCode, {}, {}, variables, CubesUnit::InvalidBaseId);
        }

        void Log(BaseErrorCode errorCode, CubesUnit::BaseId tag)
        {
            Log(errorCode, {}, {}, {}, tag);
        }

        void Log(BaseErrorCode errorCode)
        {
            Log(errorCode, {}, {}, {}, CubesUnit::InvalidBaseId);
        }
    };
}
