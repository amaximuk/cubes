#pragma once

#include <QString>
#include <QImage>
#include <QColor>
#include "../diagram/diagram_item_types.h"

namespace CubesTop
{
  //  struct PropertiesForDrawing
  //  {
  //      QImage pixmap;
  //      QString name;
  //      QString fileName;
  //      QString includeName;
  //      QColor color;
		//CubesDiagram::ItemType itemType;
  //  };

	enum class TopManagerErrorCode : CubesLog::BaseErrorCode
	{
		success = CubesLog::SuccessErrorCode,
		parametersFileInvalid = CubesLog::GetSourceTypeCodeOffset(CubesLog::SourceType::topManager),
		noParametersFile,
		zipFileError,
		__last__
	};

	inline const CubesLog::BaseErrorCodeDescriptions& GetTopManagerErrorDescriptions()
	{
		static CubesLog::BaseErrorCodeDescriptions descriptions;
		if (descriptions.empty())
		{
			descriptions[static_cast<CubesLog::BaseErrorCode>(TopManagerErrorCode::success)] = QString::fromLocal8Bit("�����");
			descriptions[static_cast<CubesLog::BaseErrorCode>(TopManagerErrorCode::parametersFileInvalid)] = QString::fromLocal8Bit("������ ������� ����� ����������");
			descriptions[static_cast<CubesLog::BaseErrorCode>(TopManagerErrorCode::noParametersFile)] = QString::fromLocal8Bit("����������� ���� ����������");
			descriptions[static_cast<CubesLog::BaseErrorCode>(TopManagerErrorCode::zipFileError)] = QString::fromLocal8Bit("������ ������ �����");
		}

		assert((static_cast<CubesLog::BaseErrorCode>(TopManagerErrorCode::__last__) - CubesLog::GetSourceTypeCodeOffset(CubesLog::SourceType::topManager) + 1) == descriptions.size());

		return descriptions;
	}

	inline QString GetTopManagerErrorDescription(TopManagerErrorCode errorCode)
	{
		const auto& descriptions = GetTopManagerErrorDescriptions();
		if (descriptions.contains(static_cast<CubesLog::BaseErrorCode>(errorCode)))
			return descriptions[static_cast<CubesLog::BaseErrorCode>(errorCode)];
		return QString("%1").arg(static_cast<uint32_t>(errorCode));
	}
}
