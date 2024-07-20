#include <QFileInfo>
#include "../log/log_manager_interface.h"
#include "../log/log_helper.h"
#include "../file/file_item.h"
#include "analysis_types.h"
#include "file_items_analysis.h"

using namespace CubesAnalysis;

FileItemsAnalysis::FileItemsAnalysis(CubesLog::ILogManager* logManager)
{
	logManager_ = logManager;

	CreateRules();

	logHelper_.reset(new CubesLog::LogHelper(logManager_, CubesLog::SourceType::fileAnalysis,
		GetRuleDescriptions(), GetRuleDetailes()));
}

void FileItemsAnalysis::SetFiles(const CubesUnitTypes::FileIdParameterModels& files)
{
	fileModels_ = files;
}

void FileItemsAnalysis::SetFileItems(QMap<CubesUnitTypes::FileId, QSharedPointer<CubesFile::FileItem>> files)
{
	fileItems_ = files;
}

QVector<Rule> FileItemsAnalysis::GetAllRules()
{
	return rules_;
}

bool FileItemsAnalysis::RunRuleTest(uint32_t errorCode)
{
	const auto& delegate = delegates_.find(errorCode);
	if (delegate != delegates_.end())
		return (*delegate)();
	
	return false;
}

bool FileItemsAnalysis::RunAllTests()
{
	bool result = true;
	for(const auto& rule : rules_)
	{
		if (!RunRuleTest(rule.errorCode))
			result = false;
	}

	return result;
}

bool FileItemsAnalysis::IsHaveAtLeastOneMainConfig(Rule rule)
{
	if (!fileModels_.empty())
		return true;

	logHelper_->LogError(rule.errorCode);

	return false;
}

bool FileItemsAnalysis::IsFileNamesUnique(Rule rule)
{
	QSet<QString> filenames;
	bool result = true;
	for (auto& file : fileModels_.toStdMap())
	{
		// ��������� ������� ����
		{
			const auto path = CubesUnitTypes::GetParameterModel(file.second, ids_.base + ids_.path)->value.toString();
			const auto name = CubesUnitTypes::GetParameterModel(file.second, ids_.base + ids_.name)->value.toString();
			QFileInfo fi(path);
			const auto fn = fi.fileName();
			if (filenames.contains(name))
			{
				logHelper_->LogError(rule.errorCode, { {QString::fromLocal8Bit("���"), name},
					{QString::fromLocal8Bit("��� �����"), fn} }, file.first);
				result = false;
			}
			else
			{
				filenames.insert(name);
			}
		}
	}

	return result;
}

bool FileItemsAnalysis::IsFileFileNamesUnique(Rule rule)
{
	QSet<QString> filenames;
	bool result = true;
	for (auto& file : fileModels_.toStdMap())
	{
		// ��������� ������� ����
		{
			const auto path = CubesUnitTypes::GetParameterModel(file.second, ids_.base + ids_.path)->value.toString();
			const auto name = CubesUnitTypes::GetParameterModel(file.second, ids_.base + ids_.name)->value.toString();
			QFileInfo fi(path);
			const auto fn = fi.fileName();
			if (filenames.contains(fn))
			{
				logHelper_->LogError(rule.errorCode, { {QString::fromLocal8Bit("���"), name},
					{QString::fromLocal8Bit("��� �����"), fn} }, file.first);
				result = false;
			}
			else
			{
				filenames.insert(fn);
			}
		}

		// ��������� ���������� �����
		{
			const auto pm = GetParameterModel(file.second, ids_.includes);
			if (pm == nullptr)
				return result;

			const auto count = pm->value.toInt();
			for (int i = 0; i < count; i++)
			{
				const auto path = CubesUnitTypes::GetParameterModel(file.second, ids_.includes + ids_.Item(i) + ids_.filePath)->value.toString();
				const auto name = CubesUnitTypes::GetParameterModel(file.second, ids_.includes + ids_.Item(i) + ids_.name)->value.toString();
				QFileInfo fi(path);
				const auto fn = fi.fileName();
				if (filenames.contains(fn))
				{
					logHelper_->LogError(rule.errorCode, { {QString::fromLocal8Bit("���"), name},
						{QString::fromLocal8Bit("��� �����"), fn} }, file.first);
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
	for (auto& file : fileModels_.toStdMap())
	{
		// ��������� ������ ������� �����
		{
			const auto path = CubesUnitTypes::GetParameterModel(file.second, ids_.base + ids_.path)->value.toString();
			const auto name = CubesUnitTypes::GetParameterModel(file.second, ids_.base + ids_.name)->value.toString();
			const auto id = CubesUnitTypes::GetParameterModel(file.second, ids_.parameters + ids_.networking + ids_.id)->value.toUInt();
			QFileInfo fi(path);
			const auto fn = fi.fileName();
			if (fileIds.contains(id))
			{
				logHelper_->LogError(rule.errorCode, { {QString::fromLocal8Bit("���"), name},
					{QString::fromLocal8Bit("��� �����"), fn} }, file.first);
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

void FileItemsAnalysis::CreateRules()
{
	{
		Rule rule{};
		rule.errorCode = static_cast<CubesLog::BaseErrorCode>(FileAnalysisErrorCode::noMainConfig);
		rule.description = QString::fromLocal8Bit("����� �����������");
		rule.detailes = QString::fromLocal8Bit("� ������� ������ ���� ��� �������� ���� ����");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&FileItemsAnalysis::IsHaveAtLeastOneMainConfig, this, rule);
	}

	{
		Rule rule{};
		rule.errorCode = static_cast<CubesLog::BaseErrorCode>(FileAnalysisErrorCode::nameNotUnique);
		rule.description = QString::fromLocal8Bit("��� �� ���������");
		rule.detailes = QString::fromLocal8Bit("� ������� � ���� ������ ������ ���� ���������� �����");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&FileItemsAnalysis::IsFileNamesUnique, this, rule);
	}

	{
		Rule rule{};
		rule.errorCode = static_cast<CubesLog::BaseErrorCode>(FileAnalysisErrorCode::fileNameNotUnique);
		rule.description = QString::fromLocal8Bit("��� ����� �� ���������");
		rule.detailes = QString::fromLocal8Bit("� ������� � ���� ������, � ��� ����� � ����������, ������ ���� ���������� ����� ������");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&FileItemsAnalysis::IsFileFileNamesUnique, this, rule);
	}

	{
		Rule rule{};
		rule.errorCode = static_cast<CubesLog::BaseErrorCode>(FileAnalysisErrorCode::connectionIdNotUnique);
		rule.description = QString::fromLocal8Bit("������������� ����� �� �������� (����������)");
		rule.detailes = QString::fromLocal8Bit("� ������� ������ �������� ���� ������ ����� ���������� ������������� ����� � ���������� ����������");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&FileItemsAnalysis::IsFileIdUnique, this, rule);
	}

	assert((static_cast<CubesLog::BaseErrorCode>(FileAnalysisErrorCode::__last__) -
		CubesLog::GetSourceTypeCodeOffset(CubesLog::SourceType::fileAnalysis)) == rules_.size());

	// ��������, ��� ip/port ���������� ��������� �� ������������ ������ � ������ 127.0.0.1
	// ����� ������ ������ �����������
	// ����� ������ ������ ��������� ������ ��������� �����, ����� � ���� .
	// ����� ������ �� ������ ���� ������ 260 ��������
}

QMap<CubesLog::BaseErrorCode, QString> FileItemsAnalysis::GetRuleDescriptions()
{
	QMap<CubesLog::BaseErrorCode, QString> result;
	result[CubesLog::SuccessErrorCode] = "�����";
	for (const auto& rule : rules_)
		result[rule.errorCode] = rule.description;

	return result;
}

QMap<CubesLog::BaseErrorCode, QString> FileItemsAnalysis::GetRuleDetailes()
{
	QMap<CubesLog::BaseErrorCode, QString> result;
	result[CubesLog::SuccessErrorCode] = "�����";
	for (const auto& rule : rules_)
		result[rule.errorCode] = rule.detailes;

	return result;
}
