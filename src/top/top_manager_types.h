#pragma once

#include <QString>
#include <QImage>
#include <QColor>

namespace CubesTop
{
    struct PropertiesForDrawing
    {
        QImage pixmap;
        QString name;
        QString fileName;
        QString includeName;
        QColor color;
    };

	enum class TopManagerErrorCode
	{
		ok = 0,
		parametersFileInvalid = CubesLog::GetSourceTypeCodeOffset(CubesLog::SourceType::topManager),
		noParametersFile,
		zipFileError
	};

	inline QString GetTopManagerErrorDescription(TopManagerErrorCode errorCode)
	{
		switch (errorCode)
		{
		case TopManagerErrorCode::ok: return "Ok";
		case TopManagerErrorCode::parametersFileInvalid: return "Parameters file invalid";
		case TopManagerErrorCode::noParametersFile: return "No parameters file";
		case TopManagerErrorCode::zipFileError: return "Zip file error";
		default: return QString("%1").arg(static_cast<uint32_t>(errorCode));
		}
	}
}
