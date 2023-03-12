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

namespace files_types
{

	struct UnitParameters
	{
		parameters_compiler::file_info fiileInfo;
		QSet<QString> platforms;
	};

	enum class EditorType
	{
		None,
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

	struct ParameterModel
	{
	public:
		QString id; // id path, separated by /
		QVariant value;
		//QString name;
		//bool is_array;
		parameters_compiler::parameter_info parameterInfo;
		EditorSettings editorSettings;
		QList<ParameterModel> parameters;

	public:
		ParameterModel()
		{
			parameterInfo = {};
		}
	};

	struct ParametersModel
	{
	public:
		QList<ParameterModel> parameters;
	};
}