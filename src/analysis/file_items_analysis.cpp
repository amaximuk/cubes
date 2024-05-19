#include <QFileInfo>
#include "analysis_types.h"
#include "file_items_analysis.h"

using namespace CubesAnalysis;

FileItemsAnalysis::FileItemsAnalysis(IAnalysisManager* analysisManager)
{
	analysisManager_ = analysisManager;
	
	{
		Rule rule{};
		rule.id = 1000;
		rule.name = QString::fromLocal8Bit("������� ��������� ����������������� �����");
		rule.description = QString::fromLocal8Bit("� ������� ������ ���� ��� �������� ���� ����");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.id] = std::bind(&FileItemsAnalysis::IsHaveAtLeastOneMainConfig, this, rule);
	}

	{
		Rule rule{};
		rule.id = 1001;
		rule.name = QString::fromLocal8Bit("������������ ���� ������");
		rule.description = QString::fromLocal8Bit("� ������� � ���� ������, � ��� ����� � ����������, ������ ���� ���������� �����");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.id] = std::bind(&FileItemsAnalysis::IsFileNamesUnique, this, rule);
	}

	{
		Rule rule{};
		rule.id = 1002;
		rule.name = QString::fromLocal8Bit("������������ �������������� ����� (����������)");
		rule.description = QString::fromLocal8Bit("� ������� ������ �������� ���� ������ ����� ���������� ������������� ����� � ���������� ����������");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.id] = std::bind(&FileItemsAnalysis::IsFileIdUnique, this, rule);
	}

	// ��������, ��� ip/port ���������� ��������� �� ������������ ������ � ������ 127.0.0.1
	// ����� ������ ������ �����������
	// ����� ������ ������ ��������� ������ ��������� �����, ����� � ���� .
	// ����� ������ �� ������ ���� ������ 260 ��������
}

void FileItemsAnalysis::SetFiles(const CubesUnitTypes::FileIdParameterModels& files)
{
	fileModels_ = files;
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

bool FileItemsAnalysis::RunAllTests()
{
	bool result = true;
	for(const auto& rule : rules_)
	{
		if (!RunRuleTest(rule.id))
			result = false;
	}

	return result;
}

bool FileItemsAnalysis::IsHaveAtLeastOneMainConfig(Rule rule)
{
	if (!fileModels_.empty())
		return true;

	analysisManager_->AfterFileError(CubesUnitTypes::InvalidFileId, rule.description);

	return false;
}

bool FileItemsAnalysis::IsFileNamesUnique(Rule rule)
{
	QSet<QString> filenames;
	bool result = true;
	for (auto& file : fileModels_)
	{
		// ��������� ������� ����
		{
			auto path = CubesUnitTypes::GetParameterModel(file, ids_.base + ids_.path)->value.toString();
			QFileInfo fi(path);
			const auto fn = fi.fileName();
			if (filenames.contains(fn))
			{
				const auto name = CubesUnitTypes::GetParameterModel(file, ids_.base + ids_.name)->value.toString();
				const auto id = CubesUnitTypes::GetParameterModel(file, ids_.parameters + ids_.networking + ids_.id)->value.toUInt();
				QString message = rule.description + QString::fromLocal8Bit("\n��� �����: %1, ���� � �����: %2").
					arg(name).arg(fn);
				analysisManager_->AfterFileError(id, message);
				result = false;
			}
			else
			{
				filenames.insert(fn);
			}
		}

		// ��������� ���������� �����
		{
			const auto pm = GetParameterModel(file, ids_.includes);
			if (pm == nullptr)
				return result;

			const auto count = pm->value.toInt();
			for (int i = 0; i < count; i++)
			{
				auto path = CubesUnitTypes::GetParameterModel(file, ids_.includes + ids_.Item(i) + ids_.filePath)->value.toString();
				QFileInfo fi(path);
				const auto fn = fi.fileName();
				if (filenames.contains(fn))
				{
					const auto name = CubesUnitTypes::GetParameterModel(file, ids_.includes + ids_.Item(i) + ids_.name)->value.toString();
					const auto id = CubesUnitTypes::GetParameterModel(file, ids_.includes + ids_.Item(i))->key.includeId;
					QString message = rule.description + QString::fromLocal8Bit("\n��� �����: %1, ���� � �����: %2").
						arg(name).arg(fn);
					analysisManager_->AfterFileError(id, message);
					result = false;
				}
				else
				{
					filenames.insert(fn);
				}
			}
		}
	}

	return result;
}

bool FileItemsAnalysis::IsFileIdUnique(Rule rule)
{
	QSet<int> fileIds;
	bool result = true;
	for (auto& file : fileModels_)
	{
		// ��������� ������ ������� �����
		{
			const auto id = CubesUnitTypes::GetParameterModel(file, ids_.parameters + ids_.networking + ids_.id)->value.toUInt();
			if (fileIds.contains(id))
			{
				const auto name = CubesUnitTypes::GetParameterModel(file, ids_.base + ids_.name)->value.toString();
				const auto id = CubesUnitTypes::GetParameterModel(file, ids_.parameters + ids_.networking + ids_.id)->value.toUInt();
				QString message = rule.description + QString::fromLocal8Bit("\n��� �����: %1, ID �����: %2").
					arg(name).arg(id);
				analysisManager_->AfterFileError(id, message);
				result = false;
			}
			else
			{
				fileIds.insert(id);
			}
		}
	}

	return result;
}
