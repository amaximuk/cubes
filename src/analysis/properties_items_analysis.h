#pragma once

#include <QObject>
#include <QSharedPointer>
#include "../unit/unit_parameter_model.h"
#include "../unit/unit_parameter_model_ids.h"
#include "analysis_manager_interface.h"
#include "analysis_types.h"

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
        CubesUnit::FileIdParameterModelPtrs fileIdParameterModelPtrs_;
        CubesUnit::PropertiesIdParameterModelPtrs propertiesIdParameterModelPtrs_;
        CubesUnit::UnitIdUnitParametersPtr unitIdUnitParametersPtr_;

        // Значения имен параметров
        CubesUnit::ParameterModelIds ids_;

        // Log
        QSharedPointer<CubesLog::LogHelper> logHelper_;

    public:
        PropertiesItemsAnalysis(CubesLog::ILogManager* logManager);

    public:
        void SetProperties(CubesUnit::FileIdParameterModelPtrs fileIdParameterModelPtrs,
            CubesUnit::PropertiesIdParameterModelPtrs propertiesIdParameterModelPtrs,
            CubesUnit::UnitIdUnitParametersPtr unitIdUnitParametersPtr);
        QVector<Rule> GetAllRules();
        bool RunRuleTest(uint32_t id);
        bool RunAllTests();

    public:
        bool TestNameIsEmpty(Rule rule);
        bool TestNameNotUnique(Rule rule);
        bool TestUnitCategoryMismatch(Rule rule);
        bool TestUnitIdMismatch(Rule rule);
        bool TestCyclicDependency(Rule rule);

    private:
        void CreateRules();
    };
}
