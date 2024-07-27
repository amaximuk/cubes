#pragma once

#include <QObject>
#include <QSharedPointer>
#include "analysis_manager_interface.h"
#include "analysis_types.h"
#include "../log/log_types.h"
#include "../unit/unit_parameter_model.h"
#include "../unit/unit_parameter_model_ids.h"

namespace CubesLog { class ILogManager; }
namespace CubesLog { class LogHelper; }
namespace CubesFile { class FileItem; }

namespace CubesAnalysis
{
    class FileItemsAnalysis : public QObject
    {
        Q_OBJECT

    private:
        CubesLog::ILogManager* logManager_;
        QVector<Rule> rules_;
        QMap<CubesLog::BaseErrorCode, std::function<bool()>> delegates_;
        CubesUnit::FileIdParameterModels fileIdParameterModels_;
        QMap<CubesUnit::FileId, QSharedPointer<CubesFile::FileItem>> fileItems_;

        // Значения имен параметров
        CubesUnit::ParameterModelIds ids_;

        // Log
        QSharedPointer<CubesLog::LogHelper> logHelper_;

    public:
        FileItemsAnalysis(CubesLog::ILogManager* logManager);

    public:
        void SetFiles(const CubesUnit::FileIdParameterModels& files);
        void SetFileItems(QMap<CubesUnit::FileId, QSharedPointer<CubesFile::FileItem>> files);
        QVector<Rule> GetAllRules();
        bool RunRuleTest(uint32_t errorCode);
        bool RunAllTests();

    public:
        bool TestNoFiles(Rule rule);
        bool TestNameIsEmpty(Rule rule);
        bool TestNameNotUnique(Rule rule);
        bool TestIncludeNameIsEmpty(Rule rule);
        bool TestIncludeNameNotUnique(Rule rule);
        bool TestFileNameIsEmpty(Rule rule);
        bool TestFileNameNotUnique(Rule rule);
        bool TestConnectionIdNotUnique(Rule rule);

    private:
        void CreateRules();
    };
}
