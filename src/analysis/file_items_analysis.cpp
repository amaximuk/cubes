#include <QFileInfo>
#include "../log/log_manager_interface.h"
#include "../log/log_helper.h"
#include "../file/file_item.h"
#include "../unit/unit_helper.h"
#include "analysis_types.h"
#include "file_items_analysis.h"

using namespace CubesAnalysis;

FileItemsAnalysis::FileItemsAnalysis(CubesLog::ILogManager* logManager)
{
	logManager_ = logManager;

	CreateRules();

	logHelper_.reset(new CubesLog::LogHelper(logManager_, CubesLog::SourceType::fileAnalysis,
		CubesAnalysis::GetRuleDescriptions(rules_), CubesAnalysis::GetRuleDetailes(rules_),
		CubesAnalysis::GetRuleTypes(rules_)));
}

void FileItemsAnalysis::SetFiles(CubesUnit::FileIdParameterModelPtrs fileIdParameterModelPtrs)
{
	fileIdParameterModelPtrs_ = fileIdParameterModelPtrs;
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

bool FileItemsAnalysis::TestNoFiles(Rule rule)
{
	if (!fileIdParameterModelPtrs_.empty())
		return true;

	logHelper_->Log(rule.errorCode);

	return false;
}

bool FileItemsAnalysis::TestNameIsEmpty(Rule rule)
{
	for (auto& file : fileIdParameterModelPtrs_.toStdMap())
	{
		// ��������� ������� ����
		{
			const auto path = CubesUnit::Helper::Common::GetParameterModelPtr(file.second, ids_.base + ids_.path)->value.toString();
			const auto name = CubesUnit::Helper::Common::GetParameterModelPtr(file.second, ids_.base + ids_.name)->value.toString();
			if (name.isEmpty())
			{
				logHelper_->Log(rule.errorCode, { {QString::fromLocal8Bit("��� �����"), path} }, file.first);
				return false;
			}
		}
	}

	return true;
}

bool FileItemsAnalysis::TestNameNotUnique(Rule rule)
{
	QSet<QString> names;
	bool result = true;
	for (auto& file : fileIdParameterModelPtrs_.toStdMap())
	{
		const auto main_path = CubesUnit::Helper::Common::GetParameterModelPtr(file.second, ids_.base + ids_.path)->value.toString();
		const auto main_name = CubesUnit::Helper::Common::GetParameterModelPtr(file.second, ids_.base + ids_.name)->value.toString();
		
		// ��������� ������� ����
		{
			if (names.contains(main_name))
			{
				logHelper_->Log(rule.errorCode, { {QString::fromLocal8Bit("���"), main_name},
					{QString::fromLocal8Bit("��� �����"), main_path} }, file.first);
				result = false;
			}
			else
			{
				names.insert(main_name);
			}
		}
	}

	return result;
}

bool FileItemsAnalysis::TestIncludeNameIsEmpty(Rule rule)
{
	for (auto& file : fileIdParameterModelPtrs_.toStdMap())
	{
		const auto main_path = CubesUnit::Helper::Common::GetParameterModelPtr(file.second, ids_.base + ids_.path)->value.toString();
		const auto main_name = CubesUnit::Helper::Common::GetParameterModelPtr(file.second, ids_.base + ids_.name)->value.toString();

		// ��������� ���������� �����
		{
			const auto pm = CubesUnit::Helper::Common::GetParameterModelPtr(file.second, ids_.includes);
			if (pm == nullptr)
				continue;

			const auto count = pm->value.toInt();
			for (int i = 0; i < count; i++)
			{
				const auto path = CubesUnit::Helper::Common::GetParameterModelPtr(file.second, ids_.includes + ids_.Item(i) + ids_.filePath)->value.toString();
				const auto name = CubesUnit::Helper::Common::GetParameterModelPtr(file.second, ids_.includes + ids_.Item(i) + ids_.name)->value.toString();
				if (name.isEmpty())
				{
					logHelper_->Log(rule.errorCode, { {QString::fromLocal8Bit("���"), main_name + "/" + name} }, file.first);
					return false;
				}
			}
		}
	}

	return true;
}

bool FileItemsAnalysis::TestIncludeNameNotUnique(Rule rule)
{
	bool result = true;
	for (auto& file : fileIdParameterModelPtrs_.toStdMap())
	{
		const auto main_path = CubesUnit::Helper::Common::GetParameterModelPtr(file.second, ids_.base + ids_.path)->value.toString();
		const auto main_name = CubesUnit::Helper::Common::GetParameterModelPtr(file.second, ids_.base + ids_.name)->value.toString();

		// ��������� ���������� �����
		{
			const auto pm = CubesUnit::Helper::Common::GetParameterModelPtr(file.second, ids_.includes);
			if (pm == nullptr)
				continue;

			QMap<QString, QString> includeNames;
			const auto count = pm->value.toInt();
			for (int i = 0; i < count; i++)
			{
				const auto path = CubesUnit::Helper::Common::GetParameterModelPtr(file.second, ids_.includes + ids_.Item(i) + ids_.filePath)->value.toString();
				const auto name = CubesUnit::Helper::Common::GetParameterModelPtr(file.second, ids_.includes + ids_.Item(i) + ids_.name)->value.toString();
				if (includeNames.contains(name))
				{
					logHelper_->Log(rule.errorCode, { {QString::fromLocal8Bit("���"), main_name + "/" + name},
						{QString::fromLocal8Bit("����������"), includeNames[name]} }, file.first);
					result = false;
				}
				else
				{
					includeNames[name] = main_name + "/" + name;
				}
			}
		}
	}

	return result;
}

bool FileItemsAnalysis::TestFileNameIsEmpty(Rule rule)
{
	for (auto& file : fileIdParameterModelPtrs_.toStdMap())
	{
		const auto main_path = CubesUnit::Helper::Common::GetParameterModelPtr(file.second, ids_.base + ids_.path)->value.toString();
		const auto main_name = CubesUnit::Helper::Common::GetParameterModelPtr(file.second, ids_.base + ids_.name)->value.toString();

		// ��������� ������� ����
		{
			if (main_path.isEmpty())
			{
				logHelper_->Log(rule.errorCode, { {QString::fromLocal8Bit("���"), main_name} }, file.first);
				return false;
			}
		}
	}

	return true;
}

bool FileItemsAnalysis::TestFileNameNotUnique(Rule rule)
{
	QMap<QString, QString> fileNames;
	bool result = true;
	for (auto& file : fileIdParameterModelPtrs_.toStdMap())
	{
		const auto main_path = CubesUnit::Helper::Common::GetParameterModelPtr(file.second, ids_.base + ids_.path)->value.toString();
		const auto main_name = CubesUnit::Helper::Common::GetParameterModelPtr(file.second, ids_.base + ids_.name)->value.toString();

		// ��������� ������� ����
		{
			const auto file_name = QFileInfo(main_path).fileName();
			if (fileNames.contains(file_name))
			{
				logHelper_->Log(rule.errorCode, { {QString::fromLocal8Bit("��� �����"), main_path},
					{QString::fromLocal8Bit("���"), main_name},
					{QString::fromLocal8Bit("����������"), fileNames[file_name]} }, file.first);
				result = false;
			}
			else
			{
				fileNames[file_name] = main_name;
			}
		}

		// ��������� ���������� �����
		{
			const auto pm = CubesUnit::Helper::Common::GetParameterModelPtr(file.second, ids_.includes);
			if (pm == nullptr)
				continue;

			const auto count = pm->value.toInt();
			for (int i = 0; i < count; i++)
			{
				const auto path = CubesUnit::Helper::Common::GetParameterModelPtr(file.second, ids_.includes + ids_.Item(i) + ids_.filePath)->value.toString();
				const auto name = CubesUnit::Helper::Common::GetParameterModelPtr(file.second, ids_.includes + ids_.Item(i) + ids_.name)->value.toString();
				const auto file_name = QFileInfo(path).fileName();
				if (fileNames.contains(file_name))
				{
					logHelper_->Log(rule.errorCode, { {QString::fromLocal8Bit("��� �����"), path},
						{QString::fromLocal8Bit("���"), main_name + "/" + name},
						{QString::fromLocal8Bit("����������"), fileNames[file_name]} }, file.first);
					result = false;
				}
				else
				{
					fileNames[file_name] = main_name + "/" + name;
				}
			}
		}
	}

	return result;
}

bool FileItemsAnalysis::TestConnectionIdNotUnique(Rule rule)
{
	QMap<int, QString> fileIds;
	bool result = true;
	for (auto& file : fileIdParameterModelPtrs_.toStdMap())
	{
		const auto main_path = CubesUnit::Helper::Common::GetParameterModelPtr(file.second, ids_.base + ids_.path)->value.toString();
		const auto main_name = CubesUnit::Helper::Common::GetParameterModelPtr(file.second, ids_.base + ids_.name)->value.toString();

		// ��������� ������ ������� �����
		{
			const auto id = CubesUnit::Helper::Common::GetParameterModelPtr(file.second, ids_.parameters + ids_.networking + ids_.id)->value.toUInt();
			if (fileIds.contains(id))
			{
				logHelper_->Log(rule.errorCode, { {QString::fromLocal8Bit("ID ����������"), QString("%1").arg(id)},
					{QString::fromLocal8Bit("���"), main_name},
					{QString::fromLocal8Bit("����������"), fileIds[id]} }, file.first);
				result = false;
			}
			else
			{
				fileIds[id] = main_name;
			}
		}
	}

	return result;
}

void FileItemsAnalysis::CreateRules()
{
	{
		Rule rule{};
		rule.errorCode = static_cast<CubesLog::BaseErrorCode>(FileAnalysisErrorCode::noFiles);
		rule.type = CubesLog::MessageType::error;
		rule.description = QString::fromLocal8Bit("����� �����������");
		rule.detailes = QString::fromLocal8Bit("� ������� ������ ���� ��� �������� ���� ����");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&FileItemsAnalysis::TestNoFiles, this, rule);
	}

	{
		Rule rule{};
		rule.errorCode = static_cast<CubesLog::BaseErrorCode>(FileAnalysisErrorCode::nameIsEmpty);
		rule.type = CubesLog::MessageType::error;
		rule.description = QString::fromLocal8Bit("��� �� ������");
		rule.detailes = QString::fromLocal8Bit("��� ������ ���� ������");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&FileItemsAnalysis::TestNameIsEmpty, this, rule);
	}

	{
		Rule rule{};
		rule.errorCode = static_cast<CubesLog::BaseErrorCode>(FileAnalysisErrorCode::nameNotUnique);
		rule.type = CubesLog::MessageType::error;
		rule.description = QString::fromLocal8Bit("��� �� ���������");
		rule.detailes = QString::fromLocal8Bit("��� ������ ���� ����������");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&FileItemsAnalysis::TestNameNotUnique, this, rule);
	}

	{
		Rule rule{};
		rule.errorCode = static_cast<CubesLog::BaseErrorCode>(FileAnalysisErrorCode::includeNameIsEmpty);
		rule.description = QString::fromLocal8Bit("��� �� ������");
		rule.detailes = QString::fromLocal8Bit("��� ����������� ����� ������ ���� ������");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&FileItemsAnalysis::TestIncludeNameIsEmpty, this, rule);
	}

	{
		Rule rule{};
		rule.errorCode = static_cast<CubesLog::BaseErrorCode>(FileAnalysisErrorCode::includeNameNotUnique);
		rule.type = CubesLog::MessageType::error;
		rule.description = QString::fromLocal8Bit("��� �� ���������");
		rule.detailes = QString::fromLocal8Bit("��� ����������� ����� ������ ���� ���������� � �������� ��������� �����");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&FileItemsAnalysis::TestIncludeNameNotUnique, this, rule);
	}

	{
		Rule rule{};
		rule.errorCode = static_cast<CubesLog::BaseErrorCode>(FileAnalysisErrorCode::fileNameIsEmpty);
		rule.type = CubesLog::MessageType::error;
		rule.description = QString::fromLocal8Bit("��� ����� �� ������");
		rule.detailes = QString::fromLocal8Bit("��� ����� ������ ���� ������");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&FileItemsAnalysis::TestFileNameIsEmpty, this, rule);
	}

	{
		Rule rule{};
		rule.errorCode = static_cast<CubesLog::BaseErrorCode>(FileAnalysisErrorCode::fileNameNotUnique);
		rule.type = CubesLog::MessageType::error;
		rule.description = QString::fromLocal8Bit("��� ����� �� ���������");
		rule.detailes = QString::fromLocal8Bit("��� ����� ������ ���� ����������");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&FileItemsAnalysis::TestFileNameNotUnique, this, rule);
	}

	{
		Rule rule{};
		rule.errorCode = static_cast<CubesLog::BaseErrorCode>(FileAnalysisErrorCode::connectionIdNotUnique);
		rule.type = CubesLog::MessageType::error;
		rule.description = QString::fromLocal8Bit("������������� ����� �� ��������");
		rule.detailes = QString::fromLocal8Bit("������������� ����� � ���������� ���������� ������ ���� ����������");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&FileItemsAnalysis::TestConnectionIdNotUnique, this, rule);
	}

	assert((static_cast<CubesLog::BaseErrorCode>(FileAnalysisErrorCode::__last__) -
		CubesLog::GetSourceTypeCodeOffset(CubesLog::SourceType::fileAnalysis)) == rules_.size());

	// ��������, ��� ip/port ���������� ��������� �� ������������ ������ � ������ 127.0.0.1
	// ����� ������ ������ �����������
	// ����� ������ ������ ��������� ������ ��������� �����, ����� � ���� .
	// ����� ������ �� ������ ���� ������ 260 ��������
}
