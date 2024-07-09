#pragma once

#include <QObject>
#include "analysis_manager_interface.h"
#include "analysis_types.h"
#include "../log/log_types.h"
#include "../unit/unit_types.h"

namespace CubesLog { class ILogManager; }
namespace CubesFile { class FileItem; }

namespace CubesAnalysis
{
    class FileItemsAnalysis : public QObject
    {
        Q_OBJECT

    private:
        CubesLog::ILogManager* logManager_;
        QVector<Rule> rules_;
        QMap<uint32_t, std::function<bool()>> delegates_;
        CubesUnitTypes::FileIdParameterModels fileModels_;
        QMap<CubesUnitTypes::FileId, QSharedPointer<CubesFile::FileItem>> fileItems_;

        // Значения имен параметров
        CubesUnitTypes::ParameterModelIds ids_;

    public:
        FileItemsAnalysis(CubesLog::ILogManager* logManager);

    public:
        void SetFiles(const CubesUnitTypes::FileIdParameterModels& files);
        void SetFileItems(QMap<CubesUnitTypes::FileId, QSharedPointer<CubesFile::FileItem>> files);
        QVector<Rule> GetAllRules();
        bool RunRuleTest(uint32_t errorCode);
        bool RunAllTests();

    public:
        bool IsHaveAtLeastOneMainConfig(Rule rule);
        bool IsFileNamesUnique(Rule rule);
        bool IsFileIdUnique(Rule rule);

    private:
        void LogError(const Rule& rule, const QVector<CubesLog::Variable>& variables, uint32_t id);
        void LogError(const Rule& rule);
    };
}
