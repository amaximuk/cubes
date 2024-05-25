#pragma once

#include <QObject>
#include "analysis_manager_interface.h"
#include "analysis_types.h"
#include "../unit/unit_types.h"

namespace CubesLog { class ILogManager; }

namespace CubesAnalysis
{
    class FileItemsAnalysis : public QObject
    {
        Q_OBJECT

    private:
        CubesLog::ILogManager* logManager_;
        QVector<Rule> rules_;
        QMap<RuleId, std::function<bool()>> delegates_;
        CubesUnitTypes::FileIdParameterModels fileModels_;

        // Значения имен параметров
        CubesUnitTypes::ParameterModelIds ids_;

    public:
        FileItemsAnalysis(CubesLog::ILogManager* logManager);

    public:
        void SetFiles(const CubesUnitTypes::FileIdParameterModels& files);
        QVector<Rule> GetAllRules();
        bool RunRuleTest(RuleId id);
        bool RunAllTests();

    public:
        bool IsHaveAtLeastOneMainConfig(Rule rule);
        bool IsFileNamesUnique(Rule rule);
        bool IsFileIdUnique(Rule rule);

    private:
        void LogError(const CubesUnitTypes::FileId fileId, CubesAnalysis::RuleId id, const QString& message);
    };
}
