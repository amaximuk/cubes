#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <regex>

#include <QString>
#include <QVariant>
#include <QSet>

#include "parameters_compiler_types.h"

namespace unit_types
{
	// "unit", "path", "string", "double", "int", "bool", "float", "int8_t", "int16_t", "int32_t", "int64_t", "uint8_t", "uint16_t", "uint32_t", "uint64_t"

	static int GetMinForIntegralType(QString type)
	{
		if (type == "uint8_t" || type == "uint16_t" || type == "uint32_t" || type == "uint64_t")
			return 0;
		else if (type == "int" || type == "int32_t" || type == "int64_t")
			return std::numeric_limits<int32_t>::min();
		else if (type == "int8_t")
			return std::numeric_limits<int8_t>::min();
		else if (type == "int16_t")
			return std::numeric_limits<int16_t>::min();
		return std::numeric_limits<int32_t>::min();
	}

	static int GetMaxForIntegralType(QString type)
	{
		if (type == "int" || type == "int32_t" || type == "int64_t" || type == "uint32_t" || type == "uint64_t")
			return std::numeric_limits<int32_t>::max();
		else if (type == "int8_t")
			return std::numeric_limits<int8_t>::max();
		else if (type == "int16_t")
			return std::numeric_limits<int16_t>::max();
		else if (type == "uint8_t")
			return std::numeric_limits<uint8_t>::max();
		else if (type == "uint16_t")
			return std::numeric_limits<uint16_t>::max();
		return std::numeric_limits<int32_t>::max();
	}

	static double GetMinForFloatingPointType(QString type)
	{
		if (type == "float")
			return std::numeric_limits<float>::lowest();
		else if (type == "double")
			return std::numeric_limits<double>::lowest();
		return std::numeric_limits<double>::lowest();
	}

	static double GetMaxForFloatingPointType(QString type)
	{
		if (type == "float")
			return std::numeric_limits<float>::max();
		else if (type == "double")
			return std::numeric_limits<double>::max();
		return std::numeric_limits<double>::max();
	}

	struct UnitParameters
	{
		parameters_compiler::file_info fiileInfo;
		QSet<QString> platforms;
	};

	enum class EditorType
	{
		String,
		SpinInterger,
		SpinDouble,
		ComboBox,
		CheckBox
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

	public:
		EditorSettings()
		{
			type = EditorType::String;
			SpinIntergerMin = 0;
			SpinIntergerMax = 0;
			SpinDoubleMin = 0;
			SpinDoubleMax = 0;
			SpinDoubleSingleStep = 1;
		};
	};

	struct ParameterModel
	{
	public:
		QString id; // id path, separated by /
		QString name;
		QVariant value;
		EditorSettings editorSettings;
		QList<ParameterModel> parameters;
		bool is_array;
	};

	struct ParametersModel
	{
	public:
		QList<ParameterModel> parameters;
	};
}