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
		// Проверяем главный файл
		{
			const auto path = CubesUnitTypes::GetParameterModel(file.second, ids_.base + ids_.path)->value.toString();
			const auto name = CubesUnitTypes::GetParameterModel(file.second, ids_.base + ids_.name)->value.toString();
			QFileInfo fi(path);
			const auto fn = fi.fileName();
			if (filenames.contains(name))
			{
				logHelper_->LogError(rule.errorCode, { {QString::fromLocal8Bit("Имя"), name},
					{QString::fromLocal8Bit("Имя файла"), fn} }, file.first);
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
		// Проверяем главный файл
		{
			const auto path = CubesUnitTypes::GetParameterModel(file.second, ids_.base + ids_.path)->value.toString();
			const auto name = CubesUnitTypes::GetParameterModel(file.second, ids_.base + ids_.name)->value.toString();
			QFileInfo fi(path);
			const auto fn = fi.fileName();
			if (filenames.contains(fn))
			{
				logHelper_->LogError(rule.errorCode, { {QString::fromLocal8Bit("Имя"), name},
					{QString::fromLocal8Bit("Имя файла"), fn} }, file.first);
				result = false;
			}
			else
			{
				filenames.insert(fn);
			}
		}

		// Проверяем включаемые файлы
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
					logHelper_->LogError(rule.errorCode, { {QString::fromLocal8Bit("Имя"), name},
						{QString::fromLocal8Bit("Имя файла"), fn} }, file.first);
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
		// Проверяем только главные файлы
		{
			const auto path = CubesUnitTypes::GetParameterModel(file.second, ids_.base + ids_.path)->value.toString();
			const auto name = CubesUnitTypes::GetParameterModel(file.second, ids_.base + ids_.name)->value.toString();
			const auto id = CubesUnitTypes::GetParameterModel(file.second, ids_.parameters + ids_.networking + ids_.id)->value.toUInt();
			QFileInfo fi(path);
			const auto fn = fi.fileName();
			if (fileIds.contains(id))
			{
				logHelper_->LogError(rule.errorCode, { {QString::fromLocal8Bit("Имя"), name},
					{QString::fromLocal8Bit("Имя файла"), fn} }, file.first);
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
		rule.description = QString::fromLocal8Bit("Файлы отсутствуют");
		rule.detailes = QString::fromLocal8Bit("В проекте должен быть как миинимум один файл");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&FileItemsAnalysis::IsHaveAtLeastOneMainConfig, this, rule);
	}

	{
		Rule rule{};
		rule.errorCode = static_cast<CubesLog::BaseErrorCode>(FileAnalysisErrorCode::nameNotUnique);
		rule.description = QString::fromLocal8Bit("Имя не уникально");
		rule.detailes = QString::fromLocal8Bit("В проекте у всех файлов должны быть уникальные имена");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&FileItemsAnalysis::IsFileNamesUnique, this, rule);
	}

	{
		Rule rule{};
		rule.errorCode = static_cast<CubesLog::BaseErrorCode>(FileAnalysisErrorCode::fileNameNotUnique);
		rule.description = QString::fromLocal8Bit("Имя файла не уникально");
		rule.detailes = QString::fromLocal8Bit("В проекте у всех файлов, в том числе у включаемых, должны быть уникальные имена файлов");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&FileItemsAnalysis::IsFileFileNamesUnique, this, rule);
	}

	{
		Rule rule{};
		rule.errorCode = static_cast<CubesLog::BaseErrorCode>(FileAnalysisErrorCode::connectionIdNotUnique);
		rule.description = QString::fromLocal8Bit("Идентификатор хоста не уникален (соединение)");
		rule.detailes = QString::fromLocal8Bit("В проекте каждый основной файл должен иметь уникальный идентификатор хоста в параметрах соединения");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&FileItemsAnalysis::IsFileIdUnique, this, rule);
	}

	assert((static_cast<CubesLog::BaseErrorCode>(FileAnalysisErrorCode::__last__) -
		CubesLog::GetSourceTypeCodeOffset(CubesLog::SourceType::fileAnalysis)) == rules_.size());

	// Проверка, что ip/port соединений указывает на существующий сервер с учетом 127.0.0.1
	// Цвета файлов должны различаться
	// Имена файлов должны содержать только латинские буквы, цифры и знак .
	// Имена файлов не должны быть длинее 260 символов
}

QMap<CubesLog::BaseErrorCode, QString> FileItemsAnalysis::GetRuleDescriptions()
{
	QMap<CubesLog::BaseErrorCode, QString> result;
	result[CubesLog::SuccessErrorCode] = "Успех";
	for (const auto& rule : rules_)
		result[rule.errorCode] = rule.description;

	return result;
}

QMap<CubesLog::BaseErrorCode, QString> FileItemsAnalysis::GetRuleDetailes()
{
	QMap<CubesLog::BaseErrorCode, QString> result;
	result[CubesLog::SuccessErrorCode] = "Успех";
	for (const auto& rule : rules_)
		result[rule.errorCode] = rule.detailes;

	return result;
}
