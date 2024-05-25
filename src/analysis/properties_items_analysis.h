#pragma once

#include <QObject>
#include "analysis_manager_interface.h"
#include "analysis_types.h"
#include "../unit/unit_types.h"

namespace CubesLog { class ILogManager; }

namespace CubesAnalysis
{
    class PropertiesItemsAnalysis : public QObject
    {
        Q_OBJECT

    private:
        CubesLog::ILogManager* logManager_;
        QVector<Rule> rules_;
        QMap<RuleId, std::function<bool()>> delegates_;
        CubesUnitTypes::FileIdParameterModels fileModels_;
        CubesUnitTypes::PropertiesIdParameterModels propertiesModels_;
        CubesUnitTypes::UnitIdUnitParameters unitParameters_;

        // Значения имен параметров
        CubesUnitTypes::ParameterModelIds ids_;

    public:
        PropertiesItemsAnalysis(CubesLog::ILogManager* logManager);

    public:
        void SetProperties(const CubesUnitTypes::FileIdParameterModels& fileModels,
            const CubesUnitTypes::PropertiesIdParameterModels& propertiesModels,
            const CubesUnitTypes::UnitIdUnitParameters& unitParameters);
        QVector<Rule> GetAllRules();
        bool RunRuleTest(RuleId id);
        bool RunAllTests();

    public:
        bool IsAllUnitsHaveName(Rule rule);
        bool IsFileNamesUnique(Rule rule);
        bool IsFileIdUnique(Rule rule);

    private:
        void LogError(const CubesUnitTypes::PropertiesId propertiesId, CubesAnalysis::RuleId id, const QString& message);
    };
}
