#include <QFileInfo>
#include "analysis_types.h"
#include "properties_items_analysis.h"

using namespace CubesAnalysis;

PropertiesItemsAnalysis::PropertiesItemsAnalysis(IAnalysisManager* analysisManager)
{
	analysisManager_ = analysisManager;

	{
		Rule rule{};
		rule.id = 1000;
		rule.name = QString::fromLocal8Bit("������� ��������� ����������������� �����");
		rule.description = QString::fromLocal8Bit("� ������� ������ ���� ��� �������� ���� ����");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.id] = std::bind(&PropertiesItemsAnalysis::IsHaveAtLeastOneMainConfig, this, rule);
	}

	{
		Rule rule{};
		rule.id = 1001;
		rule.name = QString::fromLocal8Bit("������������ ���� ������");
		rule.description = QString::fromLocal8Bit("� ������� � ���� ������, � ��� ����� � ����������, ������ ���� ���������� �����");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.id] = std::bind(&PropertiesItemsAnalysis::IsFileNamesUnique, this, rule);
	}

	{
		Rule rule{};
		rule.id = 1002;
		rule.name = QString::fromLocal8Bit("������������ �������������� ����� (����������)");
		rule.description = QString::fromLocal8Bit("� ������� ������ �������� ���� ������ ����� ���������� ������������� ����� � ���������� ����������");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.id] = std::bind(&PropertiesItemsAnalysis::IsFileIdUnique, this, rule);
	}

	// ��������, ��� ip/port ���������� ��������� �� ������������ ������
	// � ������ 127.0.0.1

}

void PropertiesItemsAnalysis::SetFiles(const QVector<File>& files)
{
	files_ = files;
}

QVector<Rule> PropertiesItemsAnalysis::GetAllRules()
{
	return rules_;
}

bool PropertiesItemsAnalysis::RunRuleTest(RuleId id)
{
	const auto& delegate = delegates_.find(id);
	if (delegate != delegates_.end())
		return (*delegate)();

	return false;
}

bool PropertiesItemsAnalysis::RunAllTests()
{
	bool result = true;
	for (const auto& rule : rules_)
	{
		if (!RunRuleTest(rule.id))
			result = false;
	}

	return result;
}

bool PropertiesItemsAnalysis::IsHaveAtLeastOneMainConfig(Rule rule)
{
	for (const auto& file : files_)
	{
		if (!file.is_include)
			return true;
	}

	analysisManager_->AfterFileError(CubesUnitTypes::InvalidFileId, rule.description);

	return false;
}

bool PropertiesItemsAnalysis::IsFileNamesUnique(Rule rule)
{
	QSet<QString> filenames;
	bool result = true;
	for (const auto& file : files_)
	{
		QFileInfo fi(file.path);
		const auto fn = fi.fileName();
		if (filenames.contains(fn))
		{
			QString message = rule.description + QString::fromLocal8Bit("\n��� �����: %1, ���� � �����: %2").
				arg(file.is_include ? file.include.name : file.main.name).arg(fn);
			analysisManager_->AfterFileError(file.is_include ? file.include.includeId : file.main.fileId, message);
			result = false;
		}
		else
		{
			filenames.insert(fn);
		}
	}

	return result;
}

bool PropertiesItemsAnalysis::IsFileIdUnique(Rule rule)
{
	QSet<int> fileIds;
	bool result = true;
	for (const auto& file : files_)
	{
		if (!file.is_include)
		{
			if (fileIds.contains(file.main.id))
			{
				QString message = rule.description + QString::fromLocal8Bit("\n��� �����: %1, ID �����: %2").
					arg(file.main.name).arg(file.main.id);
				analysisManager_->AfterFileError(file.main.fileId, message);
				result = false;
			}
			else
			{
				fileIds.insert(file.main.id);
			}
		}
	}

	return result;
}
