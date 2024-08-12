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
	CubesUnit::UnitIdUnitParametersPtr unitIdUnitParametersPtr)
{
	fileIdParameterModelPtrs_ = fileIdParameterModelPtrs;
	propertiesIdParameterModelPtrs_ = propertiesIdParameterModelPtrs;
	unitIdUnitParametersPtr_ = unitIdUnitParametersPtr;
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
		const auto properties = CubesUnit::Helper::Analyse::GetUnitProperties(kvp.second, unitIdUnitParametersPtr_);

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
					unitIdUnitParametersPtr_);

				if (item.category.compare(unitCategory, Qt::CaseInsensitive) != 0)
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

bool PropertiesItemsAnalysis::TestUnitIdMismatch(Rule rule)
{
	bool result = true;

	const auto resolvedNames = CubesUnit::Helper::Analyse::GetResolvedUnitNames(propertiesIdParameterModelPtrs_, fileIdParameterModelPtrs_);
	for (auto& kvp : propertiesIdParameterModelPtrs_.toStdMap())
	{
		const auto properties = CubesUnit::Helper::Analyse::GetUnitProperties(kvp.second, unitIdUnitParametersPtr_);

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
				const auto unitId = CubesUnit::Helper::Analyse::GetUnitId(propertiesId, propertiesIdParameterModelPtrs_,
					unitIdUnitParametersPtr_);

				if (!item.ids.contains(unitId, Qt::CaseInsensitive))
				{
					if (resolvedName.original == resolvedName.resolved)
						logHelper_->Log(rule.errorCode, { {QString::fromLocal8Bit("���"), resolvedName.resolved},
							{QString::fromLocal8Bit("��������� ���������"), item.ids.join(", ")},
							{QString::fromLocal8Bit("��������� �����"), unitId} }, kvp.first);
					else
						logHelper_->Log(rule.errorCode, { {QString::fromLocal8Bit("���"), resolvedName.resolved},
							{QString::fromLocal8Bit("�������� ���"), resolvedName.original},
							{QString::fromLocal8Bit("��������� ���������"), item.ids.join(", ")},
							{QString::fromLocal8Bit("��������� �����"), unitId} }, kvp.first);

					result = false;
				}
			}
		}
	}

	return result;
}

bool PropertiesItemsAnalysis::TestCyclicDependency(Rule rule)
{
	// ����������� ������ ������� ������ � �������������� �����������
	// ���� �� ������� �����, ��������� � ������ id ������, �� �������
	// ���� �����������, �����������. Id ������ ���� ���������, ���� ��� - 
	// ������ ���� ����
	// ����� ����� �����������, ���������� ��������� ���

	const auto resolvedNames = CubesUnit::Helper::Analyse::GetResolvedUnitNames(
		propertiesIdParameterModelPtrs_, fileIdParameterModelPtrs_);

	// �������� �� ������ ������ ��� id � �������� ������
	auto getAllPropertiesIds = [&](const QString& resolvedName)
		{
			QList<CubesUnit::PropertiesId> result;
			for (const auto& kvp : resolvedNames.toStdMap())
			{
				if (kvp.second.resolved == resolvedName)
					result.push_back(kvp.first);
			}
			return result;
		};

	// ��������� ����������� �� ��������� id ������
	auto checkDependencies = [&](const CubesUnit::PropertiesId propertiesIdToCheck,
		QList<CubesUnit::PropertiesId>& stack, auto&& checkDependencies) -> bool
		{
			stack.push_back(propertiesIdToCheck);
			if (propertiesIdParameterModelPtrs_.contains(propertiesIdToCheck))
			{
				const auto dependencies = CubesUnit::Helper::Analyse::GetUnitDependencies(
					propertiesIdParameterModelPtrs_[propertiesIdToCheck], fileIdParameterModelPtrs_, unitIdUnitParametersPtr_);

				for (const auto& dependency : dependencies)
				{
					const auto propertiesIds = getAllPropertiesIds(dependency.name.resolved);

					for (const auto& propertiesId : propertiesIds)
					{
						if (stack.contains(propertiesId))
							return false;

						if (!checkDependencies(propertiesId, stack, checkDependencies))
							return false;
					}
				}
			}
			return true;
		};

	// ��������� ��� �����
	bool result = true;
	for (auto& kvp : propertiesIdParameterModelPtrs_.toStdMap())
	{
		QList<CubesUnit::PropertiesId> stack;
		if (!checkDependencies(kvp.first, stack, checkDependencies))
		{
			if (resolvedNames[kvp.first].original == resolvedNames[kvp.first].resolved)
				logHelper_->Log(rule.errorCode, { {QString::fromLocal8Bit("���"), resolvedNames[kvp.first].resolved} }, kvp.first);
			else
				logHelper_->Log(rule.errorCode, { {QString::fromLocal8Bit("���"), resolvedNames[kvp.first].resolved},
					{QString::fromLocal8Bit("�������� ���"), resolvedNames[kvp.first].original} }, kvp.first);
			
			result = false;
		}
	}

	return result;
}

bool PropertiesItemsAnalysis::TestDependencyNotFound(Rule rule)
{
	// ����������� ������ ������� ������ � �������������� �����������
	// ���� �� ������� �����, ��������� � ������ id ������, �� �������
	// ���� �����������, �����������. Id ������ ���� ���������, ���� ��� - 
	// ������ ���� ����
	// ����� ����� �����������, ���������� ��������� ���

	const auto resolvedNames = CubesUnit::Helper::Analyse::GetResolvedUnitNames(
		propertiesIdParameterModelPtrs_, fileIdParameterModelPtrs_);

	// �������� �� ������ ������ ��� id � �������� ������
	auto getAllPropertiesIds = [&](const QString& resolvedName)
		{
			QList<CubesUnit::PropertiesId> result;
			for (const auto& kvp : resolvedNames.toStdMap())
			{
				if (kvp.second.resolved == resolvedName)
					result.push_back(kvp.first);
			}
			return result;
		};

	// ��������� ����������� �� ��������� id ������
	auto checkDependencies = [&](const CubesUnit::PropertiesId propertiesIdToCheck,
		CubesUnit::Helper::Analyse::UnitDependency& badDependency, auto&& checkDependencies) -> bool
		{
			if (propertiesIdParameterModelPtrs_.contains(propertiesIdToCheck))
			{
				const auto dependencies = CubesUnit::Helper::Analyse::GetUnitDependencies(
					propertiesIdParameterModelPtrs_[propertiesIdToCheck], fileIdParameterModelPtrs_, unitIdUnitParametersPtr_);

				for (const auto& dependency : dependencies)
				{
					const auto propertiesIds = getAllPropertiesIds(dependency.name.resolved);

					if (propertiesIds.isEmpty())
					{
						badDependency = dependency;
						return false;
					}
				}
			}
			return true;
		};

	// ��������� ��� �����
	bool result = true;
	for (auto& kvp : propertiesIdParameterModelPtrs_.toStdMap())
	{
		CubesUnit::Helper::Analyse::UnitDependency badDependency{};
		if (!checkDependencies(kvp.first, badDependency, checkDependencies))
		{
			if (resolvedNames[kvp.first].original == resolvedNames[kvp.first].resolved)
			{
				if (badDependency.name.original == badDependency.name.resolved)
					logHelper_->Log(rule.errorCode, { {QString::fromLocal8Bit("���"), resolvedNames[kvp.first].resolved},
						{QString::fromLocal8Bit("��� �����������"), badDependency.name.resolved} }, kvp.first);
				else
					logHelper_->Log(rule.errorCode, { {QString::fromLocal8Bit("���"), resolvedNames[kvp.first].resolved},
						{QString::fromLocal8Bit("��� �����������"), badDependency.name.resolved},
						{QString::fromLocal8Bit("�������� ��� �����������"), badDependency.name.resolved} }, kvp.first);
			}
			else
			{
				if (badDependency.name.original == badDependency.name.resolved)
					logHelper_->Log(rule.errorCode, { {QString::fromLocal8Bit("���"), resolvedNames[kvp.first].resolved},
						{QString::fromLocal8Bit("�������� ���"), resolvedNames[kvp.first].original},
						{QString::fromLocal8Bit("��� �����������"), badDependency.name.resolved} }, kvp.first);
				else
					logHelper_->Log(rule.errorCode, { {QString::fromLocal8Bit("���"), resolvedNames[kvp.first].resolved},
						{QString::fromLocal8Bit("�������� ���"), resolvedNames[kvp.first].original},
						{QString::fromLocal8Bit("��� �����������"), badDependency.name.resolved},
						{QString::fromLocal8Bit("�������� ��� �����������"), badDependency.name.resolved} }, kvp.first);
			}

			result = false;
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

	{
		Rule rule{};
		rule.errorCode = static_cast<uint32_t>(PropertiesAnalysisErrorCode::unitIdMismatch);
		rule.type = CubesLog::MessageType::warning;
		rule.description = QString::fromLocal8Bit("Id ����� �� ���������");
		rule.detailes = QString::fromLocal8Bit("Id ����� �� ��������� � ���������� ��� ������� ���������");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&PropertiesItemsAnalysis::TestUnitIdMismatch, this, rule);
	}

	{
		Rule rule{};
		rule.errorCode = static_cast<uint32_t>(PropertiesAnalysisErrorCode::cyclicDependency);
		rule.type = CubesLog::MessageType::error;
		rule.description = QString::fromLocal8Bit("����������� �����������");
		rule.detailes = QString::fromLocal8Bit("����������� ����������� ������ ���� �� �����");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&PropertiesItemsAnalysis::TestCyclicDependency, this, rule);
	}

	{
		Rule rule{};
		rule.errorCode = static_cast<uint32_t>(PropertiesAnalysisErrorCode::dependencyNotFound);
		rule.type = CubesLog::MessageType::error;
		rule.description = QString::fromLocal8Bit("����������� �� ����������");
		rule.detailes = QString::fromLocal8Bit("����������� ����� �� ����������");
		rule.isActive = true;
		rules_.push_back(rule);

		delegates_[rule.errorCode] = std::bind(&PropertiesItemsAnalysis::TestDependencyNotFound, this, rule);
	}

	assert((static_cast<CubesLog::BaseErrorCode>(PropertiesAnalysisErrorCode::__last__) -
		CubesLog::GetSourceTypeCodeOffset(CubesLog::SourceType::propertiesAnalysis)) == rules_.size());

	// +����� ������ ������� ���� ������
	// +����� ������ ������ ���� ��������� (� ������ ����������)
	// ��� �������� ������ ���������� �����, �����, ���� / � ���������� � ������� @xxx@
	// +��������� ����� ������ ��������������� ������������
	// +��� ����� ������ ��������������� ������������
	// ��� ���������� ����������� ������ �� ���������� ������
	// ��� ���������� ������ ���� � ������ ���������� ����������� ����� �����
	// ��� ������������� ��������� ���� ���� ������ ���� ������
	// ��� ��������������� ��������� ���� ���� ��������� �� ��������� �� ���������
	// ��� ��������� ���� ���� ��������� �� �������������� ���� (������� ��������������)
	// ���������� ���� �� ��������, �� �������� �����
	// ���� ���� � ������������, � ��� ���� ���������� ���� �����������
	// ���� ���� � ������������, �� ����� ���������� ������ ����� ���� �����������
	// +����������� �� ������ ���� ������������
	// +����������� ������ ������������
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

