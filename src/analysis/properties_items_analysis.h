#pragma once

#include <QObject>
#include <QSharedPointer>
#include "analysis_manager_interface.h"
#include "analysis_types.h"
#include "../unit/unit_types.h"

namespace CubesLog { class ILogManager; }
namespace CubesLog { class LogHelper; }

namespace CubesAnalysis
{
    class PropertiesItemsAnalysis : public QObject
    {
        Q_OBJECT

    private:
        CubesLog::ILogManager* logManager_;
        QVector<Rule> rules_;
        QMap<uint32_t, std::function<bool()>> delegates_;
        CubesUnitTypes::FileIdParameterModels fileModels_;
        CubesUnitTypes::PropertiesIdParameterModels propertiesModels_;
        CubesUnitTypes::UnitIdUnitParameters unitParameters_;

        // Значения имен параметров
        CubesUnitTypes::ParameterModelIds ids_;

        // Log
        QSharedPointer<CubesLog::LogHelper> logHelper_;

    public:
        PropertiesItemsAnalysis(CubesLog::ILogManager* logManager);

    public:
        void SetProperties(const CubesUnitTypes::FileIdParameterModels& fileModels,
            const CubesUnitTypes::PropertiesIdParameterModels& propertiesModels,
            const CubesUnitTypes::UnitIdUnitParameters& unitParameters);
        QVector<Rule> GetAllRules();
        bool RunRuleTest(uint32_t id);
        bool RunAllTests();

    public:
        bool IsAllUnitsHaveName(Rule rule);
        bool IsFileNamesUnique(Rule rule);
        bool IsFileIdUnique(Rule rule);

    private:
        void LogError(const Rule& rule, const QVector<CubesLog::Variable>& variables, uint32_t id);
        void LogError(const Rule& rule);
    };
}
