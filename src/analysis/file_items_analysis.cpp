#include <QFileInfo>
#include "../log/log_manager_interface.h"
#include "analysis_types.h"
#include "file_items_analysis.h"

using namespace CubesAnalysis;

FileItemsAnalysis::FileItemsAnalysis(CubesLog::ILogManager* logManager)
{
	logManager_ = logManager;
	//constexpr uint32_t start_id = CubesLog::GetSourceTypeCodeOffset(CubesLog::SourceType::fileAnalysis);

	{
		Rule rule{};
		rule.errorCode = static_cast<uint32_t>(FileAnalysisErrorCode::noMainConfig);
		rule.description = QString::fromLocal8Bit("Наличие основного конфигурационного файла");
		rule.detailes = QString::fromLocal8Bit("В проекте должен быть как миинимум один файл");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&FileItemsAnalysis::IsHaveAtLeastOneMainConfig, this, rule);
	}

	{
		Rule rule{};
		rule.errorCode = static_cast<uint32_t>(FileAnalysisErrorCode::fileNameNotUnique);
		rule.description = QString::fromLocal8Bit("Уникальность имени файлов");
		rule.detailes = QString::fromLocal8Bit("В проекте у всех файлов, в том числе у включаемых, должны быть уникальные имена");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&FileItemsAnalysis::IsFileNamesUnique, this, rule);
	}

	{
		Rule rule{};
		rule.errorCode = static_cast<uint32_t>(FileAnalysisErrorCode::fileIdNotUnique);
		rule.description = QString::fromLocal8Bit("Уникальность идентификатора хоста (соединение)");
		rule.detailes = QString::fromLocal8Bit("В проекте каждый основной файл должен иметь уникальный идентификатор хоста в параметрах соединения");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&FileItemsAnalysis::IsFileIdUnique, this, rule);
	}

	// Проверка, что ip/port соединений указывает на существующий сервер с учетом 127.0.0.1
	// Цвета файлов должны различаться
	// Имена файлов должны содержать только латинские буквы, цифры и знак .
	// Имена файлов не должны быть длинее 260 символов
}

void FileItemsAnalysis::SetFiles(const CubesUnitTypes::FileIdParameterModels& files)
{
	fileModels_ = files;
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

	LogError(rule);

	return false;
}

bool FileItemsAnalysis::IsFileNamesUnique(Rule rule)
{
	QSet<QString> filenames;
	bool result = true;
	for (auto& file : fileModels_)
	{
		// Проверяем главный файл
		{
			auto path = CubesUnitTypes::GetParameterModel(file, ids_.base + ids_.path)->value.toString();
			QFileInfo fi(path);
			const auto fn = fi.fileName();
			if (filenames.contains(fn))
			{
				const auto name = CubesUnitTypes::GetParameterModel(file, ids_.base + ids_.name)->value.toString();
				const auto id = CubesUnitTypes::GetParameterModel(file, ids_.parameters + ids_.networking + ids_.id)->value.toUInt();
				LogError(rule, { {"Имя файла", name}, {"Путь к файлу", fn} }, id);
				result = false;
			}
			else
			{
				filenames.insert(fn);
			}
		}

		// Проверяем включаемые файлы
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
					LogError(rule, { {"Имя файла", name}, {"Путь к файлу", fn} }, id);
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
		// Проверяем только главные файлы
		{
			const auto id = CubesUnitTypes::GetParameterModel(file, ids_.parameters + ids_.networking + ids_.id)->value.toUInt();
			if (fileIds.contains(id))
			{
				const auto name = CubesUnitTypes::GetParameterModel(file, ids_.base + ids_.name)->value.toString();
				const auto id = CubesUnitTypes::GetParameterModel(file, ids_.parameters + ids_.networking + ids_.id)->value.toUInt();
				LogError(rule, { {"Имя файла", name}, {"Id хоста", id} }, id);
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

void FileItemsAnalysis::LogError(const Rule& rule, const QVector<CubesLog::Variable>& variables, uint32_t tag)
{
	CubesLog::Message lm{};
	lm.type = CubesLog::MessageType::error;
	lm.code = CubesLog::CreateCode(CubesLog::MessageType::error,
		CubesLog::SourceType::fileAnalysis, static_cast<uint32_t>(rule.errorCode));
	lm.source = CubesLog::SourceType::fileAnalysis;
	lm.description = rule.description;
	lm.details = rule.detailes;
	lm.variables = variables;
	lm.tag = tag;
	logManager_->AddMessage(lm);
}

void FileItemsAnalysis::LogError(const Rule& rule)
{
	LogError(rule, {}, {});
}
