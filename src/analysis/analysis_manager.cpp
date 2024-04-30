#include "../top_manager_interface.h"
#include "../unit_types.h"
#include "file_items_analysis.h"
#include "properties_items_analysis.h"
#include "analysis_manager.h"

using namespace CubesAnalysis;

AnalysisManager::AnalysisManager(ITopManager* topManager, CubesLog::ILogManager* logManager)
{
	topManager_ = topManager;
	logManager_ = logManager;
}

void AnalysisManager::Test()
{

}
