#pragma once

#include <QString>
#include <QRegExp>
#include "../unit/unit_types.h"
#include "../log/log_types.h"

namespace CubesProperties
{
	enum class PropertiesManagerErrorCode : CubesLog::BaseErrorCode
	{
		success = CubesLog::SuccessErrorCode,
		itemCreated = CubesLog::GetSourceTypeCodeOffset(CubesLog::SourceType::fileManager),
		itemRemoved,
		allItemsRemoved,
		itemError,
		__last__
	};

	inline const CubesLog::BaseErrorCodeDescriptions& GetPropertiesManagerErrorDescriptions()
	{
		static CubesLog::BaseErrorCodeDescriptions descriptions;
		if (descriptions.empty())
		{
			descriptions[static_cast<CubesLog::BaseErrorCode>(PropertiesManagerErrorCode::success)] = QString::fromLocal8Bit("�����");
			descriptions[static_cast<CubesLog::BaseErrorCode>(PropertiesManagerErrorCode::itemCreated)] = QString::fromLocal8Bit("���� ������ (PropertiesItem)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(PropertiesManagerErrorCode::itemRemoved)] = QString::fromLocal8Bit("���� ������ (PropertiesItem)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(PropertiesManagerErrorCode::allItemsRemoved)] = QString::fromLocal8Bit("��� ����� ������� (PropertiesItem)");
			descriptions[static_cast<CubesLog::BaseErrorCode>(PropertiesManagerErrorCode::itemError)] = QString::fromLocal8Bit("������ � ����� (PropertiesItem)");
		}

		assert((static_cast<CubesLog::BaseErrorCode>(PropertiesManagerErrorCode::__last__) - CubesLog::GetSourceTypeCodeOffset(CubesLog::SourceType::fileManager) + 1) == descriptions.size());

		return descriptions;
	}

	inline QString GetPropertiesManagerErrorDescription(PropertiesManagerErrorCode errorCode)
	{
		const auto& descriptions = GetPropertiesManagerErrorDescriptions();
		if (descriptions.contains(static_cast<CubesLog::BaseErrorCode>(errorCode)))
			return descriptions[static_cast<CubesLog::BaseErrorCode>(errorCode)];
		return QString("%1").arg(static_cast<uint32_t>(errorCode));
	}

	enum class PropertiesItemErrorCode : CubesLog::BaseErrorCode
	{
		success = CubesLog::SuccessErrorCode,
		typeMismatch = CubesLog::GetSourceTypeCodeOffset(CubesLog::SourceType::fileItem),
		convertFailed,
		restrictionFailed,
		notFound,
		__last__
	};

	inline const CubesLog::BaseErrorCodeDescriptions& GetPropertiesItemErrorDescriptions()
	{
		static CubesLog::BaseErrorCodeDescriptions descriptions;
		if (descriptions.empty())
		{
			descriptions[static_cast<CubesLog::BaseErrorCode>(PropertiesItemErrorCode::success)] = QString::fromLocal8Bit("�����");
			descriptions[static_cast<CubesLog::BaseErrorCode>(PropertiesItemErrorCode::typeMismatch)] = QString::fromLocal8Bit("��������������� ���� ���������");
			descriptions[static_cast<CubesLog::BaseErrorCode>(PropertiesItemErrorCode::convertFailed)] = QString::fromLocal8Bit("������ ����������� ��������");
			descriptions[static_cast<CubesLog::BaseErrorCode>(PropertiesItemErrorCode::restrictionFailed)] = QString::fromLocal8Bit("�������� �� ������������� ������������");
			descriptions[static_cast<CubesLog::BaseErrorCode>(PropertiesItemErrorCode::notFound)] = QString::fromLocal8Bit("�������� �� ������");
		}

		assert((static_cast<CubesLog::BaseErrorCode>(PropertiesItemErrorCode::__last__) - CubesLog::GetSourceTypeCodeOffset(CubesLog::SourceType::fileItem) + 1) == descriptions.size());

		return descriptions;
	}

	inline QString GetPropertiesItemErrorDescription(PropertiesManagerErrorCode errorCode)
	{
		const auto& descriptions = GetPropertiesItemErrorDescriptions();
		if (descriptions.contains(static_cast<CubesLog::BaseErrorCode>(errorCode)))
			return descriptions[static_cast<CubesLog::BaseErrorCode>(errorCode)];
		return QString("%1").arg(static_cast<uint32_t>(errorCode));
	}
}
