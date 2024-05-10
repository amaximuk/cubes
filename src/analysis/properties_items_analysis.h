#pragma once

#include <QObject>
#include "analysis_manager_interface.h"
#include "analysis_types.h"
#include "../unit_types.h"

namespace CubesTop { class ITopManager; }
namespace CubesLog { class ILogManager; }

namespace CubesAnalysis
{
    class PropertiesItemsAnalysis : public QObject
    {
        Q_OBJECT

    private:
        IAnalysisManager* analysisManager_;
        QVector<Rule> rules_;
        QMap<RuleId, std::function<bool()>> delegates_;
        QVector<Properties> properties_;

    public:
        PropertiesItemsAnalysis(IAnalysisManager* analysisManager);

    public:
        void SetProperties(const QVector<Properties>& properties);
        QVector<Rule> GetAllRules();
        bool RunRuleTest(RuleId id);
        bool RunAllTests();

    public:
        bool IsAllUnitsHaveName(Rule rule);
        bool IsFileNamesUnique(Rule rule);
        bool IsFileIdUnique(Rule rule);
    };
}
