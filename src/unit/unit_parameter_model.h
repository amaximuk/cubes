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
#include "unit_parameter_model_id.h"
#include "unit_types.h"

namespace CubesUnit
{
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

	struct ComboBoxValue
	{
		BaseId id;
		QString value;
	};

	struct EditorSettings
	{
		EditorType type;
		int spinIntergerMin;
		int spinIntergerMax;
		double spinDoubleMin;
		double spinDoubleMax;
		double spinDoubleSingleStep;
		QVector<ComboBoxValue> comboBoxValues;
		bool isExpanded;

		EditorSettings()
		{
			type = EditorType::String;
			spinIntergerMin = 0;
			spinIntergerMax = 0;
			spinDoubleMin = 0;
			spinDoubleMax = 0;
			spinDoubleSingleStep = 1;
			isExpanded = false;
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

	struct ParameterModel;
	using ParameterModels = QVector<ParameterModel>;
	using ParameterModelPtr = QSharedPointer<ParameterModel>;
	using ParameterModelPtrs = QVector<ParameterModelPtr>;

	struct ParameterModel
	{
		ParameterModelId id; // id path, separated by /
		QString name;
		BaseId key; // Для хранения id списков и т.п.
		QVariant value;
		ParameterInfoId parameterInfoId;
		EditorSettings editorSettings;
		ParameterModelPtrs parameters;
		bool readOnly;

		ParameterModel()
		{
			key = InvalidBaseId;
			readOnly = false;
		}

		ParameterModel Clone()
		{
			ParameterModel parameterModel{};
			parameterModel.id = id;
			parameterModel.name = name;
			parameterModel.key = key;
			parameterModel.value = value;
			parameterModel.parameterInfoId = parameterInfoId;
			parameterModel.editorSettings = editorSettings;
			for (const auto& item : parameters)
			{
				ParameterModelPtr parameterModelPtr = ParameterModelPtr(new ParameterModel());
				*parameterModelPtr = item->Clone();
				parameterModel.parameters.push_back(parameterModelPtr);
			}
			parameterModel.readOnly = readOnly;

			return parameterModel;
		}

		void SetComboBoxFileNames(FileIdNames fileNames)
		{
			editorSettings.comboBoxValues.clear();
			for (const auto& kvp : fileNames.toStdMap())
				editorSettings.comboBoxValues.push_back({ kvp.first, kvp.second });
		}

		void SetComboBoxIncludeNames(IncludeIdNames includeNames)
		{
			editorSettings.comboBoxValues.clear();
			for (const auto& kvp : includeNames.toStdMap())
				editorSettings.comboBoxValues.push_back({ kvp.first, kvp.second });
		}

		void SetComboBoxValue(BaseId id, const QString& value)
		{
			if (editorSettings.comboBoxValues.empty())
				return;

			auto it = std::find_if(editorSettings.comboBoxValues.begin(),
				editorSettings.comboBoxValues.end(), [&id](const auto& cbv) { return cbv.id == id; });
			if (it != editorSettings.comboBoxValues.end())
			{
				it->value = value;
				this->key = it->id;
				this->value = it->value;
			}
		}

		void SetComboBoxValueDefault()
		{
			if (editorSettings.comboBoxValues.empty())
				return;

			key = editorSettings.comboBoxValues[0].id;
			value = editorSettings.comboBoxValues[0].value;
		}

		void SetComboBoxValue(BaseId id)
		{
			if (editorSettings.comboBoxValues.empty())
				return;

			const auto it = std::find_if(editorSettings.comboBoxValues.cbegin(),
				editorSettings.comboBoxValues.cend(), [&id](const auto& cbv) { return cbv.id == id; });
			if (it == editorSettings.comboBoxValues.end())
			{
				key = editorSettings.comboBoxValues[0].id;
				value = editorSettings.comboBoxValues[0].value;
			}
			else
			{
				key = it->id;
				value = it->value;
			}
		}

		void SetComboBoxValue(const QString& value)
		{
			if (editorSettings.comboBoxValues.empty())
				return;

			const auto it = std::find_if(editorSettings.comboBoxValues.cbegin(),
				editorSettings.comboBoxValues.cend(), [&value](const auto& cbv) { return cbv.value == value; });
			if (it == editorSettings.comboBoxValues.end())
			{
				this->key = editorSettings.comboBoxValues[0].id;
				this->value = editorSettings.comboBoxValues[0].value;
			}
			else
			{
				this->key = it->id;
				this->value = it->value;
			}
		}

		QStringList GetComboBoxValues() const
		{
			QStringList result;
			for (const auto& item : editorSettings.comboBoxValues)
				result.push_back(item.value);

			return result;
		}
	
		bool HaveComboBoxValue(const QString& value) const
		{
			if (editorSettings.comboBoxValues.empty())
				return false;

			const auto it = std::find_if(editorSettings.comboBoxValues.cbegin(),
				editorSettings.comboBoxValues.cend(), [&value](const auto& cbv) { return cbv.value == value; });
			return it != editorSettings.comboBoxValues.end();
		}

		int GetComboBoxIndex() const
		{
			if (editorSettings.comboBoxValues.empty())
				return -1;

			const auto it = std::find_if(editorSettings.comboBoxValues.cbegin(),
				editorSettings.comboBoxValues.cend(), [&](const auto& cbv) { return cbv.id == key; });
			if (it == editorSettings.comboBoxValues.end())
				return -1;
			else
				return std::distance(editorSettings.comboBoxValues.cbegin(), it);
		}

		int GetComboBoxIndex(const QString& value) const
		{
			if (editorSettings.comboBoxValues.empty())
				return -1;

			const auto it = std::find_if(editorSettings.comboBoxValues.cbegin(),
				editorSettings.comboBoxValues.cend(), [&value](const auto& cbv) { return cbv.value == value; });
			if (it == editorSettings.comboBoxValues.end())
				return -1;
			else
				return std::distance(editorSettings.comboBoxValues.cbegin(), it);
		}

		private:

	};

	//using FileIdParameterModels = QMap<FileId, ParameterModels>;
	using FileIdParameterModelPtrs = QMap<FileId, ParameterModelPtrs>;
	//using PropertiesIdParameterModels = QMap<PropertiesId, ParameterModels>;
	using PropertiesIdParameterModelPtrs = QMap<PropertiesId, ParameterModelPtrs>;
	//using PropertiesIdUnitParameters = QMap<PropertiesId, UnitParameters>;

	inline ParameterModelPtr CreateParameterModelPtr()
	{
		return ParameterModelPtr(new ParameterModel());
	}

	inline ParameterModelPtr CreateParameterModelPtr(const ParameterModel& other)
	{
		ParameterModelPtr parameterModelPtr = ParameterModelPtr(new ParameterModel());
		*parameterModelPtr = other;
		return parameterModelPtr;
	}
}
