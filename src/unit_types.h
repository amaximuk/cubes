#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <regex>
#include <QVariant>
#include <QSet>
#include "parameters/types.h"

namespace CubesUnitTypes
{
	// "unit", "path", "string", "double", "int", "bool", "float", "int8_t", "int16_t", "int32_t", "int64_t", "uint8_t", "uint16_t", "uint32_t", "uint64_t"

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

	enum class EditorType
	{
		None,
		String,
		SpinInterger,
		SpinDouble,
		ComboBox,
		CheckBox,
		Color
	};

	struct EditorSettings
	{
	public:
		EditorType type;
		int SpinIntergerMin;
		int SpinIntergerMax;
		double SpinDoubleMin;
		double SpinDoubleMax;
		double SpinDoubleSingleStep;
		QList<QString> ComboBoxValues;
		bool is_expanded;

	public:
		EditorSettings()
		{
			type = EditorType::String;
			SpinIntergerMin = 0;
			SpinIntergerMax = 0;
			SpinDoubleMin = 0;
			SpinDoubleMax = 0;
			SpinDoubleSingleStep = 1;
			is_expanded = false;
		};
	};

	struct ParameterInfoId
	{
		QString type;
		QString name;

		ParameterInfoId() = default;
		ParameterInfoId(QString type, QString name)
		{
			this->type = type;
			this->name = name;
		}
	};

	struct ParameterModel
	{
	public:
		QString id; // id path, separated by /
		QString name;
		QVariant value;
		ParameterInfoId parameterInfoId;
		EditorSettings editorSettings;
		QList<ParameterModel> parameters;
		bool readOnly;

	public:
		ParameterModel()
		{
			readOnly = false;
		}
	};

	struct ParametersModel
	{
	public:
		QList<ParameterModel> parameters;
	};

	static QString GetUniqueName(QString baseName, QString delimiter, QStringList busyNames)
	{
		QString name = baseName;
		int counter = 0;
		while (busyNames.contains(name))
			name = QString("%1%2%3").arg(baseName, delimiter).arg(++counter);
		return name;
	}
}