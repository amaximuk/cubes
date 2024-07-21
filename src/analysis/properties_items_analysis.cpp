#include <QFileInfo>
#include "../log/log_manager_interface.h"
#include "../log/log_helper.h"
#include "../unit/unit_helper.h"
#include "analysis_types.h"
#include "properties_items_analysis.h"

using namespace CubesAnalysis;

PropertiesItemsAnalysis::PropertiesItemsAnalysis(CubesLog::ILogManager* logManager)
{
	logManager_ = logManager;

	CreateRules();

	logHelper_.reset(new CubesLog::LogHelper(logManager_, CubesLog::SourceType::propertiesAnalysis,
		GetRuleDescriptions(), GetRuleDetailes()));
}

void PropertiesItemsAnalysis::SetProperties(const CubesUnit::FileIdParameterModels& fileModels,
	const CubesUnit::PropertiesIdParameterModels& propertiesModels,
	const CubesUnit::UnitIdUnitParameters& unitParameters)
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

bool PropertiesItemsAnalysis::TestNameIsEmpty(Rule rule)
{
	bool result = true;
	for (auto& kvp : propertiesModels_.toStdMap())
	{
		auto& properties = kvp.second;
		const auto name = CubesUnit::Helper::GetParameterModel(properties, ids_.base + ids_.name)->value.toString();
		const auto unitId = CubesUnit::Helper::GetParameterModel(properties, ids_.base + ids_.unitId)->value.toString();
		if (name.isEmpty())
		{
			logHelper_->LogError(rule.errorCode, { {QString::fromLocal8Bit("��� �����"), unitId} }, kvp.first);
			result = false;
		}
	}

	return result;
}

bool PropertiesItemsAnalysis::TestNameNotUnique(Rule rule)
{
	bool result = true;
	QSet<QString> names;

	for (auto& kvp : propertiesModels_.toStdMap())
	{
		auto& parameterModels = kvp.second;
		const auto name = CubesUnit::Helper::GetParameterModel(parameterModels, ids_.base + ids_.name)->value.toString();
		const auto fileId = CubesUnit::Helper::GetParameterModel(parameterModels, ids_.base + ids_.fileName)->key;
		const auto includeId = CubesUnit::Helper::GetParameterModel(parameterModels, ids_.base + ids_.includeName)->key;

		QString realName = name;
		if (fileId != CubesUnit::InvalidFileId && includeId != CubesUnit::InvalidIncludeId)
		{
			const auto it = fileModels_.find(fileId);
			if (it == fileModels_.end())
				continue;
			const auto vars = CubesUnit::Helper::File::GetIncludeVariables(*it, includeId);
			for (const auto& kvpVars : vars.toStdMap())
				realName.replace(QString("@%1@").arg(kvpVars.second.first), kvpVars.second.second);
		}

		if (names.contains(realName))
		{
			//QString fileName{};
			//const auto it = fileModels_.find(fileId);
			//if (it != fileModels_.end())
			//	fileName = CubesUnit::Helper::GetParameterModel(*it, ids_.base + ids_.name)->value.toString();

			if (name == realName)
				logHelper_->LogError(rule.errorCode, { {QString::fromLocal8Bit("���"), realName} }, kvp.first);
			else
				logHelper_->LogError(rule.errorCode, { {QString::fromLocal8Bit("���"), realName},
					{QString::fromLocal8Bit("�������� ���"), name}}, kvp.first);

			result = false;
		}
		else
		{
			names.insert(realName);
		}

	}

	return result;
}

//bool PropertiesItemsAnalysis::IsFileIdUnique(Rule rule)
//{
//	QSet<int> fileIds;
//	bool result = true;
//	//for (const auto& file : fileModels_)
//	//{
//	//	if (!file.is_include)
//	//	{
//	//		if (fileIds.contains(file.main.id))
//	//		{
//	//			QString message = rule.description + QString::fromLocal8Bit("\n��� �����: %1, ID �����: %2").
//	//				arg(file.main.name).arg(file.main.id);
//	//			LogError->AfterFileError(file.main.fileId, message);
//	//			result = false;
//	//		}
//	//		else
//	//		{
//	//			fileIds.insert(file.main.id);
//	//		}
//	//	}
//	//}
//
//	return result;
//}

//void PropertiesItemsAnalysis::LogError(const Rule& rule, const QVector<CubesLog::Variable>& variables, uint32_t tag)
//{
//	CubesLog::Message lm{};
//	lm.type = CubesLog::MessageType::error;
//	lm.code = CubesLog::CreateCode(CubesLog::MessageType::error,
//		CubesLog::SourceType::propertiesAnalysis, static_cast<uint32_t>(rule.errorCode));
//	lm.source = CubesLog::SourceType::propertiesAnalysis;
//	lm.description = rule.description;
//	lm.details = rule.detailes;
//	lm.variables = variables;
//	lm.tag = tag;
//	logManager_->AddMessage(lm);
//}
//
//void PropertiesItemsAnalysis::LogError(const Rule& rule)
//{
//	LogError(rule, {}, {});
//}

void PropertiesItemsAnalysis::CreateRules()
{
	{
		Rule rule{};
		rule.errorCode = static_cast<uint32_t>(PropertiesAnalysisErrorCode::nameIsEmpty);
		rule.description = QString::fromLocal8Bit("��� �� ������");
		rule.detailes = QString::fromLocal8Bit("��� ������ ���� ������");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&PropertiesItemsAnalysis::TestNameIsEmpty, this, rule);
	}

	{
		Rule rule{};
		rule.errorCode = static_cast<uint32_t>(PropertiesAnalysisErrorCode::nameNotUnique);
		rule.description = QString::fromLocal8Bit("��� �� ���������");
		rule.detailes = QString::fromLocal8Bit("��� ������ ���� ����������, � ������ ����������");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&PropertiesItemsAnalysis::TestNameNotUnique, this, rule);
	}

	//{
	//	Rule rule{};
	//	rule.errorCode = static_cast<uint32_t>(FileAnalysisErrorCode::noMainConfig);
	//	rule.description = QString::fromLocal8Bit("������������ ���� ������");
	//	rule.detailes = QString::fromLocal8Bit("� ������� � ���� ������, � ��� ����� � ����������, ������ ���� ���������� �����");
	//	rule.isActive = true;
	//	rules_.push_back(rule);

	//	delegates_[rule.errorCode] = std::bind(&PropertiesItemsAnalysis::TestFileNameNotUnique, this, rule);
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

	assert((static_cast<CubesLog::BaseErrorCode>(PropertiesAnalysisErrorCode::__last__) -
		CubesLog::GetSourceTypeCodeOffset(CubesLog::SourceType::propertiesAnalysis)) == rules_.size());

	// ����� ������ ������� ���� ������
	// ����� ������ ������ ���� ��������� (� ������ ����������)
	// ��� �������� ������ ���������� �����, �����, ���� / � ���������� � ������� @xxx@
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

QMap<CubesLog::BaseErrorCode, QString> PropertiesItemsAnalysis::GetRuleDescriptions()
{
	QMap<CubesLog::BaseErrorCode, QString> result;
	result[CubesLog::SuccessErrorCode] = "�����";
	for (const auto& rule : rules_)
		result[rule.errorCode] = rule.description;

	return result;
}

QMap<CubesLog::BaseErrorCode, QString> PropertiesItemsAnalysis::GetRuleDetailes()
{
	QMap<CubesLog::BaseErrorCode, QString> result;
	result[CubesLog::SuccessErrorCode] = "�����";
	for (const auto& rule : rules_)
		result[rule.errorCode] = rule.detailes;

	return result;
}
