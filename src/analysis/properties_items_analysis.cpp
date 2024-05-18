#include <QFileInfo>
#include "analysis_types.h"
#include "properties_items_analysis.h"

using namespace CubesAnalysis;

PropertiesItemsAnalysis::PropertiesItemsAnalysis(IAnalysisManager* analysisManager)
{
	analysisManager_ = analysisManager;

	{
		Rule rule{};
		rule.id = 2000;
		rule.name = QString::fromLocal8Bit("Имена юнитов заданы");
		rule.description = QString::fromLocal8Bit("Каждый юнит должен иметь имя");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.id] = std::bind(&PropertiesItemsAnalysis::IsAllUnitsHaveName, this, rule);
	}

	{
		Rule rule{};
		rule.id = 2001;
		rule.name = QString::fromLocal8Bit("Уникальность имен файлов");
		rule.description = QString::fromLocal8Bit("В проекте у всех файлов, в том числе у включаемых, должны быть уникальные имена");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.id] = std::bind(&PropertiesItemsAnalysis::IsFileNamesUnique, this, rule);
	}

	{
		Rule rule{};
		rule.id = 2002;
		rule.name = QString::fromLocal8Bit("Уникальность идентификатора хоста (соединение)");
		rule.description = QString::fromLocal8Bit("В проекте каждый основной файл должен иметь уникальный идентификатор хоста в параметрах соединения");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.id] = std::bind(&PropertiesItemsAnalysis::IsFileIdUnique, this, rule);
	}

	// Имена юнитов должжны быть заданы
	// Имя содержит только английские буквы, цифры, знак / и паременные в формате @xxx@
	// Имена юнитов должны быть уникальны (с учетом переменных)
	// Категория юнита должна соответствовать ограничениям
	// Тип юнита должен соответствовать ограничениям
	// Имя переменной допускается только во включаемых файлах
	// Имя переменной должно быть в списке переменных включаемого файла юнита
	// Имя обязательного параметра типа юнит должно быть задано
	// Имя необязательного параметра типа юнит совпадает со значением по умолчанию
	// Имя параметра типа юнит указывает на несуществующий юнит (включая необязательные)
	// Установлен флаг не задавать, но параметр задан
	// Юнит есть в зависимостях, и для него установлен флаг зависимость
	// Юнит есть в зависимостях, но можно установить вместо этого флаг зависимость
	// Зависимости не должны быть циклическими
	// Юнит должет существовать под выбранную в файле платформу
	// В проекте обязан быть юнит task manager
	// В проекте должно быть не более одного юнита task manager
	// В проекте обязан быть юнит hardware manager
	// В проекте должно быть не более одного юнита hardware manager
	// В проекте обязан быть юнит БД
	// Если в проекте есть ntp клиент, то ы файле не должна быть включена синхронизация времени
	// Если в проекте нет ntp клиента, то ы файле должна быть включена синхронизация времени
	// Позиции юнитов должны различаться
	// Позиции элементов массива должны различаться
	// Имена элементов массива должны быть заданы
}

void PropertiesItemsAnalysis::SetProperties(const CubesUnitTypes::PropertiesIdParameterModels& properties)
{
	properties_ = properties;
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

bool PropertiesItemsAnalysis::IsAllUnitsHaveName(Rule rule)
{
	bool result = true;
	for (const auto& properties : properties_)
	{
		//if (properties.name.isEmpty())
		//{
		//	QString message = rule.description + QString::fromLocal8Bit("\nТип юнита: %1").
		//		arg(properties.unitId);
		//	analysisManager_->AfterPropertiesError(properties.propertiesId, message);
		//	result = false;
		//}
	}

	return result;
}

bool PropertiesItemsAnalysis::IsFileNamesUnique(Rule rule)
{
	QSet<QString> filenames;
	bool result = true;
	//for (const auto& file : files_)
	//{
	//	QFileInfo fi(file.path);
	//	const auto fn = fi.fileName();
	//	if (filenames.contains(fn))
	//	{
	//		QString message = rule.description + QString::fromLocal8Bit("\nИмя файла: %1, путь к файлу: %2").
	//			arg(file.is_include ? file.include.name : file.main.name).arg(fn);
	//		analysisManager_->AfterFileError(file.is_include ? file.include.includeId : file.main.fileId, message);
	//		result = false;
	//	}
	//	else
	//	{
	//		filenames.insert(fn);
	//	}
	//}

	return result;
}

bool PropertiesItemsAnalysis::IsFileIdUnique(Rule rule)
{
	QSet<int> fileIds;
	bool result = true;
	//for (const auto& file : files_)
	//{
	//	if (!file.is_include)
	//	{
	//		if (fileIds.contains(file.main.id))
	//		{
	//			QString message = rule.description + QString::fromLocal8Bit("\nИмя файла: %1, ID хоста: %2").
	//				arg(file.main.name).arg(file.main.id);
	//			analysisManager_->AfterFileError(file.main.fileId, message);
	//			result = false;
	//		}
	//		else
	//		{
	//			fileIds.insert(file.main.id);
	//		}
	//	}
	//}

	return result;
}
