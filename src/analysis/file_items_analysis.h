#pragma once

#include <QObject>
#include "analysis_manager_interface.h"
#include "analysis_types.h"
#include "../unit_types.h"

class ITopManager;
namespace CubesLog
{
    class ILogManager;
}

namespace CubesAnalysis
{
    class FileItemsAnalysis : public QObject
    {
        Q_OBJECT

    private:
        CubesLog::ILogManager* logManager_;
        QVector<Rule> rules_;
        QMap<RuleId, std::function<bool()>> delegates_;
        QVector<File> files_;

    public:
        FileItemsAnalysis(CubesLog::ILogManager* logManager);

    public:
        void SetFiles(const QVector<File>& files);
        QVector<Rule> GetAllRules();
        bool RunRuleTest(RuleId id);

    public:
        bool IsHaveAtLeastOneMainConfig(Rule rule);
        bool IsFileNamesUnique(Rule rule);
    };
}
