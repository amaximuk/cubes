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
		CubesAnalysis::GetRuleDescriptions(rules_), CubesAnalysis::GetRuleDetailes(rules_),
		CubesAnalysis::GetRuleTypes(rules_)));
}

void PropertiesItemsAnalysis::SetProperties(CubesUnit::FileIdParameterModelPtrs fileIdParameterModelPtrs,
	CubesUnit::PropertiesIdParameterModelPtrs propertiesIdParameterModelPtrs,
	const CubesUnit::UnitIdUnitParameters& unitParameters)
{
	fileIdParameterModelPtrs_ = fileIdParameterModelPtrs;
	propertiesIdParameterModelPtrs_ = propertiesIdParameterModelPtrs;
	unitIdUnitParameters_ = unitParameters;
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
	for (auto& kvp : propertiesIdParameterModelPtrs_.toStdMap())
	{
		auto& properties = kvp.second;
		const auto name = CubesUnit::Helper::Common::GetParameterModelPtr(properties, ids_.base + ids_.name)->value.toString();
		const auto unitId = CubesUnit::Helper::Common::GetParameterModelPtr(properties, ids_.base + ids_.unitId)->value.toString();
		if (name.isEmpty())
		{
			logHelper_->Log(rule.errorCode, { {QString::fromLocal8Bit("��� �����"), unitId} }, kvp.first);
			result = false;
		}
	}

	return result;
}

bool PropertiesItemsAnalysis::TestNameNotUnique(Rule rule)
{
	bool result = true;

	const auto resolvedUnitNames = CubesUnit::Helper::Analyse::GetResolvedUnitNames(propertiesIdParameterModelPtrs_, fileIdParameterModelPtrs_);

	QSet<QString> names;
	for (const auto& kvp : resolvedUnitNames.toStdMap())
	{
		if (names.contains(kvp.second.resolved))
		{
			if (kvp.second.original == kvp.second.resolved)
				logHelper_->Log(rule.errorCode, { {QString::fromLocal8Bit("���"), kvp.second.resolved} }, kvp.first);
			else
				logHelper_->Log(rule.errorCode, { {QString::fromLocal8Bit("���"), kvp.second.resolved},
					{QString::fromLocal8Bit("�������� ���"), kvp.second.original}}, kvp.first);

			result = false;
		}
		else
		{
			names.insert(kvp.second.resolved);
		}
	}

	return result;
}

bool PropertiesItemsAnalysis::TestUnitCategoryMismatch(Rule rule)
{
	bool result = true;

	const auto resolvedNames = CubesUnit::Helper::Analyse::GetResolvedUnitNames(propertiesIdParameterModelPtrs_, fileIdParameterModelPtrs_);
	for (auto& kvp : propertiesIdParameterModelPtrs_.toStdMap())
	{
		const auto properties = CubesUnit::Helper::Analyse::GetParameterModelsUnitProperties(kvp.second, unitIdUnitParameters_);

		for (const auto& item : properties)
		{
			// item.category - ��������� ��������� - ��, ��� ������ ����, ������ �� ���� ��������� ����� � ��� restrictions
			// item.value - ������������ ��� ����� - �������� � �����������
			const auto resolvedName = CubesUnit::Helper::Analyse::GetResolvedUnitName(kvp.second, fileIdParameterModelPtrs_, item.name);

			// ���� ���� �� ��� �����
			auto values = resolvedNames.toStdMap();
			const auto it = std::find_if(values.cbegin(), values.cend(),
				[&resolvedName](const auto& v) {return v.second.resolved == resolvedName.resolved; });
			if (it != values.end())
			{
				const auto propertiesId = it->first;
				const auto unitCategory = CubesUnit::Helper::Analyse::GetUnitCategory(propertiesId, propertiesIdParameterModelPtrs_,
					unitIdUnitParameters_);

				if (item.category != unitCategory)
				{
					if (resolvedName.original == resolvedName.resolved)
						logHelper_->Log(rule.errorCode, { {QString::fromLocal8Bit("���"), resolvedName.resolved},
							{QString::fromLocal8Bit("��������� ���������"), item.category},
							{QString::fromLocal8Bit("��������� �����"), unitCategory} }, kvp.first);
					else
						logHelper_->Log(rule.errorCode, { {QString::fromLocal8Bit("���"), resolvedName.resolved},
							{QString::fromLocal8Bit("�������� ���"), resolvedName.original},
							{QString::fromLocal8Bit("��������� ���������"), item.category},
							{QString::fromLocal8Bit("��������� �����"), unitCategory} }, kvp.first);

					result = false;
				}
			}
		}
	}

	return result;
}

void PropertiesItemsAnalysis::CreateRules()
{
	{
		Rule rule{};
		rule.errorCode = static_cast<uint32_t>(PropertiesAnalysisErrorCode::nameIsEmpty);
		rule.type = CubesLog::MessageType::error;
		rule.description = QString::fromLocal8Bit("��� �� ������");
		rule.detailes = QString::fromLocal8Bit("��� ������ ���� ������");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&PropertiesItemsAnalysis::TestNameIsEmpty, this, rule);
	}

	{
		Rule rule{};
		rule.errorCode = static_cast<uint32_t>(PropertiesAnalysisErrorCode::nameNotUnique);
		rule.type = CubesLog::MessageType::error;
		rule.description = QString::fromLocal8Bit("��� �� ���������");
		rule.detailes = QString::fromLocal8Bit("��� ������ ���� ����������, � ������ ����������");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&PropertiesItemsAnalysis::TestNameNotUnique, this, rule);
	}

	{
		Rule rule{};
		rule.errorCode = static_cast<uint32_t>(PropertiesAnalysisErrorCode::unitCategoryMismatch);
		rule.type = CubesLog::MessageType::warning;
		rule.description = QString::fromLocal8Bit("��������� ����� �� ���������");
		rule.detailes = QString::fromLocal8Bit("��������� ����� �� ��������� � ���������� ��� ������� ���������");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&PropertiesItemsAnalysis::TestUnitCategoryMismatch, this, rule);
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

	// +����� ������ ������� ���� ������
	// +����� ������ ������ ���� ��������� (� ������ ����������)
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

