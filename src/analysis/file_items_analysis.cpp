#include "analysis_types.h"
#include "file_items_analysis.h"

using namespace CubesAnalysis;

FileItemsAnalysis::FileItemsAnalysis(IAnalysisManager* analysisManager)
{
	analysisManager_ = analysisManager;
	
	{
		Rule rule{};
		rule.id = 1000;
		rule.name = QString::fromLocal8Bit("Наличие основного конфигурационного файла");
		rule.description = QString::fromLocal8Bit("В проекте должен быть как миинимум один файл");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.id] = std::bind(&FileItemsAnalysis::IsHaveAtLeastOneMainConfig, this, rule);
	}

	{
		Rule rule{};
		rule.id = 1001;
		rule.name = QString::fromLocal8Bit("Уникальность имен файлов");
		rule.description = QString::fromLocal8Bit("В проекте у всех файлов, в том числе у включаемых, должны быть уникальные имена");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.id] = std::bind(&FileItemsAnalysis::IsFileNamesUnique, this, rule);
	}

}

void FileItemsAnalysis::SetFiles(const QVector<File>& files)
{
	files_ = files;
}

QVector<Rule> FileItemsAnalysis::GetAllRules()
{
	return rules_;
}

bool FileItemsAnalysis::RunRuleTest(RuleId id)
{
	const auto& delegate = delegates_.find(id);
	if (delegate != delegates_.end())
		return (*delegate)();
	
	return false;
}

bool FileItemsAnalysis::IsHaveAtLeastOneMainConfig(Rule rule)
{
	for (const auto& file : files_)
	{
		if (!file.is_include)
			return true;
	}

	analysisManager_->AfterFileError(CubesUnitTypes::InvalidFileId, rule.description);

	return false;
}

bool FileItemsAnalysis::IsFileNamesUnique(Rule rule)
{
	return true;
}
