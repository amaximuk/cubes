#include <QFileInfo>
#include "../log/log_manager_interface.h"
#include "analysis_types.h"
#include "properties_items_analysis.h"

using namespace CubesAnalysis;

PropertiesItemsAnalysis::PropertiesItemsAnalysis(CubesLog::ILogManager* logManager)
{
	logManager_ = logManager;
	//constexpr uint32_t start_id = CubesLog::GetSourceTypeCodeOffset(CubesLog::SourceType::propertiesAnalysis);

	{
		Rule rule{};
		rule.errorCode = static_cast<uint32_t>(PropertiesAnalysisErrorCode::noUnitName);
		rule.description = QString::fromLocal8Bit("����� ������ ������");
		rule.detailes = QString::fromLocal8Bit("������ ���� ������ ����� ���");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&PropertiesItemsAnalysis::IsAllUnitsHaveName, this, rule);
	}

	//{
	//	Rule rule{};
	//	rule.errorCode = static_cast<uint32_t>(FileAnalysisErrorCode::noMainConfig);
	//	rule.description = QString::fromLocal8Bit("������������ ���� ������");
	//	rule.detailes = QString::fromLocal8Bit("� ������� � ���� ������, � ��� ����� � ����������, ������ ���� ���������� �����");
	//	rule.isActive = true;
	//	rules_.push_back(rule);

	//	delegates_[rule.errorCode] = std::bind(&PropertiesItemsAnalysis::IsFileNamesUnique, this, rule);
	//}

	//{
	//	Rule rule{};
	//	rule.errorCode = static_cast<uint32_t>(FileAnalysisErrorCode::noMainConfig);
	//	rule.description = QString::fromLocal8Bit("������������ �������������� ����� (����������)");
	//	rule.detailes = QString::fromLocal8Bit("� ������� ������ �������� ���� ������ ����� ���������� ������������� ����� � ���������� ����������");
	//	rule.isActive = true;
	//	rules_.push_back(rule);

	//	delegates_[rule.errorCode] = std::bind(&PropertiesItemsAnalysis::IsFileIdUnique, this, rule);
	//}

	// ����� ������ ������� ���� ������
	// ��� �������� ������ ���������� �����, �����, ���� / � ���������� � ������� @xxx@
	// ����� ������ ������ ���� ��������� (� ������ ����������)
	// ��������� ����� ������ ��������������� ������������
	// ��� ����� ������ ��������������� ������������
	// ��� ���������� ����������� ������ �� ���������� ������
	// ��� ���������� ������ ���� � ������ ���������� ����������� ����� �����
	// ��� ������������� ��������� ���� ���� ������ ���� ������
	// ��� ��������������� ��������� ���� ���� ��������� �� ��������� �� ���������
	// ��� ��������� ���� ���� ��������� �� �������������� ���� (������� ��������������)
	// ���������� ���� �� ��������, �� �������� �����
	// ���� ���� � ������������, � ��� ���� ���������� ���� �����������
	// ���� ���� � ������������, �� ����� ���������� ������ ����� ���� �����������
	// ����������� �� ������ ���� ������������
	// ���� ������ ������������ ��� ��������� � ����� ���������
	// � ������� ������ ���� ���� task manager
	// � ������� ������ ���� �� ����� ������ ����� task manager
	// � ������� ������ ���� ���� hardware manager
	// � ������� ������ ���� �� ����� ������ ����� hardware manager
	// � ������� ������ ���� ���� ��
	// ���� � ������� ���� ntp ������, �� � ����� �� ������ ���� �������� ������������� �������
	// ���� � ������� ��� ntp �������, �� � ����� ������ ���� �������� ������������� �������
	// ������� ������ ������ �����������
	// ������� ��������� ������� ������ �����������
	// ����� ��������� ������� ������ ���� ������
}

void PropertiesItemsAnalysis::SetProperties(const CubesUnitTypes::FileIdParameterModels& fileModels,
	const CubesUnitTypes::PropertiesIdParameterModels& propertiesModels,
	const CubesUnitTypes::UnitIdUnitParameters& unitParameters)
{
	fileModels_ = fileModels;
	propertiesModels_ = propertiesModels;
	unitParameters_ = unitParameters;
}

QVector<Rule> PropertiesItemsAnalysis::GetAllRules()
{
	return rules_;
}

bool PropertiesItemsAnalysis::RunRuleTest(uint32_t id)
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
		if (!RunRuleTest(rule.errorCode))
			result = false;
	}

	return result;
}

bool PropertiesItemsAnalysis::IsAllUnitsHaveName(Rule rule)
{
	bool result = true;
	for (auto& kvp : propertiesModels_.toStdMap())
	{
		auto& properties = kvp.second;
		const auto name = CubesUnitTypes::GetParameterModel(properties, ids_.base + ids_.name)->value.toString();
		const auto unitId = CubesUnitTypes::GetParameterModel(properties, ids_.base + ids_.unitId)->value.toString();
		if (name.isEmpty())
		{
			LogError(rule, { {"��� �����", unitId} }, kvp.first);
			result = false;
		}
	}

	return result;
}

bool PropertiesItemsAnalysis::IsFileNamesUnique(Rule rule)
{
	QSet<QString> filenames;
	bool result = true;
	//for (const auto& file : fileModels_)
	//{
	//	QFileInfo fi(file.path);
	//	const auto fn = fi.fileName();
	//	if (filenames.contains(fn))
	//	{
	//		QString message = rule.description + QString::fromLocal8Bit("\n��� �����: %1, ���� � �����: %2").
	//			arg(file.is_include ? file.include.name : file.main.name).arg(fn);
	//		LogError->AfterFileError(file.is_include ? file.include.includeId : file.main.fileId, message);
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
	//for (const auto& file : fileModels_)
	//{
	//	if (!file.is_include)
	//	{
	//		if (fileIds.contains(file.main.id))
	//		{
	//			QString message = rule.description + QString::fromLocal8Bit("\n��� �����: %1, ID �����: %2").
	//				arg(file.main.name).arg(file.main.id);
	//			LogError->AfterFileError(file.main.fileId, message);
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

void PropertiesItemsAnalysis::LogError(const Rule& rule, const QVector<CubesLog::Variable>& variables, uint32_t tag)
{
	CubesLog::Message lm{};
	lm.type = CubesLog::MessageType::error;
	lm.code = CubesLog::CreateCode(CubesLog::MessageType::error,
		CubesLog::SourceType::propertiesAnalysis, static_cast<uint32_t>(rule.errorCode));
	lm.source = CubesLog::SourceType::propertiesAnalysis;
	lm.description = rule.description;
	lm.details = rule.detailes;
	lm.variables = variables;
	lm.tag = tag;
	logManager_->AddMessage(lm);
}

void PropertiesItemsAnalysis::LogError(const Rule& rule)
{
	LogError(rule, {}, {});
}
