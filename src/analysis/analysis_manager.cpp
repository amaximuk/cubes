#include "../top_manager_interface.h"
#include "../unit_types.h"
#include "../log_table/log_table_interface.h"
#include "file_items_analysis.h"
#include "properties_items_analysis.h"
#include "analysis_types.h"
#include "analysis_manager.h"

using namespace CubesAnalysis;

AnalysisManager::AnalysisManager(ITopManager* topManager, CubesLog::ILogManager* logManager)
{
	topManager_ = topManager;
	logManager_ = logManager;
	fileItemAnalysis_.reset(new FileItemsAnalysis(this));
}

void AnalysisManager::Test()
{
	QVector<File> files;
	if (!topManager_->GetAnalysisFiles(files))
		return;

	fileItemAnalysis_->SetFiles(files);
	fileItemAnalysis_->RunAllTests();
}

void AnalysisManager::AfterFileError(const CubesUnitTypes::FileId fileId, const QString& message)
{
	if (logManager_ != nullptr)
	{
		CubesLog::LogMessage lm{};
		lm.type = CubesLog::MessageType::error;
		lm.tag = fileId;
		lm.source = QString::fromLocal8Bit("AnalysisManager");
		lm.description = message;
		logManager_->AddMessage(lm);
	}
}
