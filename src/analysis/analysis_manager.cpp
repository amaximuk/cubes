#include "../top/top_manager_interface.h"
#include "../unit/unit_types.h"
#include "../log/log_manager_interface.h"
#include "file_items_analysis.h"
#include "properties_items_analysis.h"
#include "analysis_types.h"
#include "analysis_manager.h"

using namespace CubesAnalysis;

AnalysisManager::AnalysisManager(CubesLog::ILogManager* logManager)
{
	logManager_ = logManager;
	fileItemsAnalysis_.reset(new FileItemsAnalysis(logManager));
	propertiesItemsAnalysis_.reset(new PropertiesItemsAnalysis(logManager));
}

void AnalysisManager::Test(CubesUnit::FileIdParameterModelPtrs fileIdParameterModelPtrs,
	CubesUnit::PropertiesIdParameterModelPtrs propertiesIdParameterModelPtrs,
	CubesUnit::UnitIdUnitParametersPtr unitIdUnitParametersPtr)
{
	fileItemsAnalysis_->SetFiles(fileIdParameterModelPtrs);
	//fileItemsAnalysis_->SetFileItems(fileModels);
	fileItemsAnalysis_->RunAllTests();

	propertiesItemsAnalysis_->SetProperties(fileIdParameterModelPtrs, propertiesIdParameterModelPtrs, unitIdUnitParametersPtr);
	propertiesItemsAnalysis_->RunAllTests();
}

//void AnalysisManager::AfterFileError(const CubesUnit::FileId fileId, CubesAnalysis::RuleId id, const QString& message)
//{
//	if (logManager_ != nullptr)
//	{
//		CubesLog::Message lm{};
//		lm.type = CubesLog::MessageType::error;
//		lm.code = CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::fileAnalysis, id);
//		lm.source = CubesLog::SourceType::fileAnalysis;
//		lm.description = message;
//		lm.tag = fileId;
//		logManager_->AddMessage(lm);
//	}
//}
//
//void AnalysisManager::AfterPropertiesError(const CubesUnit::PropertiesId propertiesId,
//	CubesAnalysis::RuleId id, const QString& message)
//{
//	if (logManager_ != nullptr)
//	{
//		CubesLog::Message lm{};
//		lm.type = CubesLog::MessageType::error;
//		lm.code = CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::propertiesAnalysis, id);
//		lm.source = CubesLog::SourceType::propertiesAnalysis;
//		lm.description = message;
//		lm.tag = propertiesId;
//		logManager_->AddMessage(lm);
//	}
//}
