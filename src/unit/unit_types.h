#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <regex>
#include <QVariant>
#include <QSet>
#include <QString>
#include <QVector>
#include <QSharedPointer>
#include "parameters/types.h"

namespace CubesUnit
{
	using BaseId = uint32_t;
	constexpr BaseId InvalidBaseId = 0;

	using FileId = BaseId;
	constexpr FileId InvalidFileId = InvalidBaseId;
	using FileIdNames = QMap<FileId, QString>;

	using IncludeId = BaseId;
	constexpr IncludeId InvalidIncludeId = InvalidBaseId;
	using IncludeIdNames = QMap<IncludeId, QString>;

	using Variable = QPair<QString, QString>;
	using VariableId = BaseId;
	constexpr VariableId InvalidVariableId = InvalidBaseId;
	using VariableIdVariables = QMap<VariableId, Variable>;

	using PropertiesId = BaseId;
	constexpr PropertiesId InvalidPropertiesId = InvalidBaseId;

	using UnitId = QString;

	const std::vector<std::string> platform_names_ = {
		"Windows x32",
		"Windows x64",
		"Astra 15 x64",
		"Astra 16 x64",
		"Sigma a32"
	};

	struct UnitParameters
	{
		parameters::file_info fileInfo;
		QSet<QString> platforms;
	};

	using UnitParametersPtr = QSharedPointer<UnitParameters>;
	using UnitParametersPtrs = QVector<UnitParametersPtr>;

	using PropertiesIdUnitParametersPtr = QMap<PropertiesId, UnitParametersPtr>;
	using UnitIdUnitParametersPtr = QMap<UnitId, UnitParametersPtr>;

	//using UnitIdUnitParameters = QMap<UnitId, UnitParameters>;

	inline UnitParametersPtr CreateUnitParametersPtr()
	{
		return UnitParametersPtr(new UnitParameters());
	}

	inline UnitParametersPtr CreateUnitParametersPtr(const UnitParameters& other)
	{
		UnitParametersPtr unitParametersPtr = UnitParametersPtr(new UnitParameters());
		*unitParametersPtr = other;
		return unitParametersPtr;
	}
}
