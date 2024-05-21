#include "../top/top_manager_interface.h"
#include "../unit/unit_types.h"
#include "../log/log_table_interface.h"
#include "file_items_analysis.h"
#include "properties_items_analysis.h"
#include "analysis_types.h"
#include "analysis_manager.h"

using namespace CubesAnalysis;

AnalysisManager::AnalysisManager(CubesLog::ILogManager* logManager)
{
	logManager_ = logManager;
	fileItemsAnalysis_.reset(new FileItemsAnalysis(this));
	propertiesItemsAnalysis_.reset(new PropertiesItemsAnalysis(this));
}

void AnalysisManager::Test(const CubesUnitTypes::FileIdParameterModels& fileModels,
	const CubesUnitTypes::PropertiesIdParameterModels& propertiesModels,
	const CubesUnitTypes::UnitIdUnitParameters& unitParameters)
{
	fileItemsAnalysis_->SetFiles(fileModels);
	fileItemsAnalysis_->RunAllTests();

	propertiesItemsAnalysis_->SetProperties(fileModels, propertiesModels, unitParameters);
	propertiesItemsAnalysis_->RunAllTests();
}

void AnalysisManager::AfterFileError(const CubesUnitTypes::FileId fileId, CubesAnalysis::RuleId id, const QString& message)
{
	if (logManager_ != nullptr)
	{
		CubesLog::LogMessage lm{};
		lm.type = CubesLog::MessageType::error;
		lm.code = CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::fileAnalysis, id);
		lm.source = CubesLog::SourceType::fileAnalysis;
		lm.description = message;
		lm.tag = fileId;
		logManager_->AddMessage(lm);
	}
}

void AnalysisManager::AfterPropertiesError(const CubesUnitTypes::PropertiesId propertiesId,
	CubesAnalysis::RuleId id, const QString& message)
{
	if (logManager_ != nullptr)
	{
		CubesLog::LogMessage lm{};
		lm.type = CubesLog::MessageType::error;
		lm.code = CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::propertiesAnalysis, id);
		lm.source = CubesLog::SourceType::propertiesAnalysis;
		lm.description = message;
		lm.tag = propertiesId;
		logManager_->AddMessage(lm);
	}
}
