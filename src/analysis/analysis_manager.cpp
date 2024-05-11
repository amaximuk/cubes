#include "../top/top_manager_interface.h"
#include "../unit_types.h"
#include "../log_table/log_table_interface.h"
#include "file_items_analysis.h"
#include "properties_items_analysis.h"
#include "analysis_types.h"
#include "analysis_manager.h"

using namespace CubesAnalysis;

AnalysisManager::AnalysisManager(CubesTop::ITopManager* topManager, CubesLog::ILogManager* logManager)
{
	topManager_ = topManager;
	logManager_ = logManager;
	fileItemsAnalysis_.reset(new FileItemsAnalysis(this));
	propertiesItemsAnalysis_.reset(new PropertiesItemsAnalysis(this));
}

void AnalysisManager::Test()
{
	QVector<File> files;
	if (!topManager_->GetAnalysisFiles(files))
		return;

	fileItemsAnalysis_->SetFiles(files);
	fileItemsAnalysis_->RunAllTests();

	QVector<Properties> properties;
	if (!topManager_->GetAnalysisProperties(properties))
		return;

	propertiesItemsAnalysis_->SetProperties(properties);
	propertiesItemsAnalysis_->RunAllTests();
}

void AnalysisManager::AfterFileError(const CubesUnitTypes::FileId fileId, const QString& message)
{
	if (logManager_ != nullptr)
	{
		CubesLog::LogMessage lm{};
		lm.type = CubesLog::MessageType::error;
		lm.tag = fileId;
		lm.source = QString::fromLocal8Bit("File analysis");
		lm.description = message;
		logManager_->AddMessage(lm);
	}
}

void AnalysisManager::AfterPropertiesError(const CubesUnitTypes::PropertiesId propertiesId, const QString& message)
{
	if (logManager_ != nullptr)
	{
		CubesLog::LogMessage lm{};
		lm.type = CubesLog::MessageType::error;
		lm.tag = propertiesId;
		lm.source = QString::fromLocal8Bit("Properties analysis");
		lm.description = message;
		logManager_->AddMessage(lm);
	}
}
