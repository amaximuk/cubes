#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <regex>
#include <QVariant>
#include <QSet>
#include <QString>
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

	struct ParameterModelId
	{
	private:
		QStringList value_;

	public:
		ParameterModelId() = default;

		ParameterModelId(const ParameterModelId& rhs)
		{
			value_ = rhs.value_;
		};

		ParameterModelId(ParameterModelId&& rhs) noexcept
		{
			value_ = std::move(rhs.value_);
		};

		ParameterModelId(const QString& value)
		{
			QString local(value);
			local.remove(QRegExp("^([\\s|/]+)"));
			local.remove(QRegExp("([\\s|/]+)$"));
			value_ = local.split("/", Qt::SkipEmptyParts);
		}

		ParameterModelId(const char* value):
			ParameterModelId(QString(value))
		{}

	public:
		ParameterModelId& operator=(const ParameterModelId& rhs)
		{
			if (this == &rhs)
				return *this;

			value_ = rhs.value_;
			return *this;
		}

		ParameterModelId& operator=(const QString& rhs)
		{
			value_ = ParameterModelId(rhs).value_;
			return *this;
		}

		ParameterModelId& operator=(ParameterModelId&& rhs) noexcept
		{
			if (this == &rhs)
				return *this;

			value_ = std::move(rhs.value_);
			return *this;
		}

		//operator QString() const
		//{
		//	return value_.join("/");
		//}

		//operator const char* () const
		//{
		//	return value_.join("/").toStdString().c_str();
		//}

		ParameterModelId& operator+=(const ParameterModelId& rhs)
		{
			value_.append(rhs.value_);
			return *this;
		}

		ParameterModelId& operator+=(const QString& rhs)
		{
			value_.append(ParameterModelId(rhs).value_);
			return *this;
		}

		ParameterModelId& operator+=(const char* rhs)
		{
			value_.append(ParameterModelId(rhs).value_);
			return *this;
		}

		friend ParameterModelId operator+(ParameterModelId lhs, const ParameterModelId& rhs)
		{
			lhs += rhs;
			return lhs;
		}

		friend ParameterModelId operator+(ParameterModelId lhs, const QString& rhs)
		{
			lhs += rhs;
			return lhs;
		}

		friend ParameterModelId operator+(ParameterModelId lhs, const char* rhs)
		{
			lhs += rhs;
			return lhs;
		}

		bool operator==(const ParameterModelId& rhs) const
		{
			if (this == &rhs)
				return true;

			return value_ == rhs.value_;
		}

		//bool operator==(const QString& rhs)
		//{
		//	return *this == ParameterModelId(rhs);
		//}

		bool operator!=(const ParameterModelId& rhs) const
		{
			return !(*this == rhs);
		}

		//bool operator!=(const QString& rhs)
		//{
		//	return !(*this == rhs);
		//}

		//bool operator<(const ParameterModelId& rhs)
		//{
		//	if (this == &rhs)
		//		return true;

		//	return value_ < rhs.value_;
		//}

		friend bool operator<(const ParameterModelId& lhs, const ParameterModelId& rhs)
		{
			return lhs.value_ < rhs.value_;
		}

	public:
		int size() const
		{
			return value_.size();
		}

		bool empty() const
		{
			return value_.empty();
		}

		QList<ParameterModelId> split() const
		{
			QList<ParameterModelId> list;
			for (const auto& item : value_)
				list.push_back(item);
			return list;
		}

		QStringList splitToString() const
		{
			return value_;
		}

		QString toString() const
		{
			return value_.join("/");
		}

		bool startsWith(const ParameterModelId& rhs) const
		{
			if (value_.size() < rhs.value_.size())
				return false;

			for (size_t i = 0; i < rhs.value_.size(); i++)
			{
				if (value_.at(i) != rhs.value_.at(i))
					return false;
			}
			return true;
		}

		bool startsWith(const QString& value) const
		{
			return startsWith(ParameterModelId(value));
		}

		bool startsWith(const char* value) const
		{
			return startsWith(ParameterModelId(value));
		}

		bool endsWith(const ParameterModelId& rhs) const
		{
			if (value_.size() < rhs.value_.size())
				return false;

			for (size_t i = 0; i < rhs.value_.size(); ++i)
			{
				if (value_.at(i + value_.size() - rhs.value_.size()) != rhs.value_.at(i))
					return false;
			}
			return true;
		}

		bool endsWith(const QString& value) const
		{
			return startsWith(ParameterModelId(value));
		}

		bool endsWith(const char* value) const
		{
			return startsWith(ParameterModelId(value));
		}

		bool contains(const ParameterModelId& rhs) const
		{
			return (indexOf(rhs) != -1);

			/*if (rhs.value_.empty())
				return true;

			if (value_.size() < rhs.value_.size())
				return false;

			int pos = 0;
			while (pos <= value_.size() - rhs.value_.size())
			{
				pos = value_.indexOf(rhs.value_.at(0), pos);
				if (pos == -1 || pos > value_.size() - rhs.value_.size())
					return false;

				bool found = true;
				for (size_t i = pos; i < pos + rhs.value_.size(); i++)
				{
					if (value_.at(i) != rhs.value_.at(i))
					{
						found = false;
						break;
					}
				}

				if (found)
					return true;

				++pos;
			}
			return false;*/
		}

		bool contains(const QString& value) const
		{
			return startsWith(ParameterModelId(value));
		}

		bool contains(const char* value) const
		{
			return startsWith(ParameterModelId(value));
		}

		int indexOf(const ParameterModelId& rhs) const
		{
			if (rhs.value_.empty())
				return 0;

			if (value_.size() < rhs.value_.size())
				return -1;

			int pos = 0;
			while (pos <= value_.size() - rhs.value_.size())
			{
				pos = value_.indexOf(rhs.value_.at(0), pos);
				if (pos == -1 || pos > value_.size() - rhs.value_.size())
					return -1;

				bool found = true;
				for (size_t i = 0; i < rhs.value_.size(); ++i)
				{
					if (value_.at(pos + i) != rhs.value_.at(i))
					{
						found = false;
						break;
					}
				}

				if (found)
					return pos;

				++pos;
			}
			return -1;
		}

		int indexOf(const QString& value) const
		{
			return indexOf(ParameterModelId(value));
		}

		int indexOf(const char* value) const
		{
			return indexOf(ParameterModelId(value));
		}

		ParameterModelId left(int n) const
		{
			auto sub = value_.mid(0, n);
			return sub.join("/");
		}

		ParameterModelId right(int n) const
		{
			auto cnt = std::min(value_.size(), n);
			auto sub = value_.mid(value_.size() - cnt, cnt);
			return sub.join("/");
		}

		ParameterModelId mid(int pos, int n = -1) const
		{
			auto sub = value_.mid(pos, n);
			return sub.join("/");
		}

		//operator QString() const
		//{
		//    return this->value_;
		//}

		//ParameterModelId& operator+=(const ParameterModelId& rhs)
		//{
		//    this->value_ = QString("%1/%2").arg(this->value_, rhs.value_);
		//    return *this;
		//}

		//ParameterModelId& operator+=(const QString& rhs)
		//{
		//    this->value_ = QString("%1/%2").arg(this->value_, ParameterModelId(rhs));
		//    return *this;
		//}

		//friend ParameterModelId operator+(ParameterModelId lhs, const ParameterModelId& rhs)
		//{
		//    lhs += rhs;
		//    return lhs;
		//}

		//friend ParameterModelId operator+(ParameterModelId lhs, const QString& rhs)
		//{
		//    lhs += rhs;
		//    return lhs;
		//}
	};

	struct ParameterModelIds
	{
		CubesUnitTypes::ParameterModelId baseGroupName;
		CubesUnitTypes::ParameterModelId parametersGroupName;
		CubesUnitTypes::ParameterModelId editorGroupName;
		CubesUnitTypes::ParameterModelId itemGroupName;
		CubesUnitTypes::ParameterModelId dependsParameterName;
		CubesUnitTypes::ParameterModelId optionalParameterName;
		CubesUnitTypes::ParameterModelId includesGroupName;
		CubesUnitTypes::ParameterModelId variablesGroupName;

		static const ParameterModelIds& Defaults()
		{
			static const ParameterModelIds ids{
				QString::fromLocal8Bit("$BASE"),
				QString::fromLocal8Bit("$PARAMETERS"),
				QString::fromLocal8Bit("$EDITOR"),
				QString::fromLocal8Bit("$ITEM"),
				QString::fromLocal8Bit("$DEPENDS"),
				QString::fromLocal8Bit("$OPTIONAL"),
				QString::fromLocal8Bit("$INCLUDES"),
				QString::fromLocal8Bit("$VARIABLES")
			};
			return ids;
		}

		static const CubesUnitTypes::ParameterModelId GetDefaultItemId(int n)
		{
			auto itemGroupName = ParameterModelIds::Defaults().itemGroupName;
			return QString("%1_%2").arg(itemGroupName.toString()).arg(n);
		}

		static const int GetDefaultItemIndex(CubesUnitTypes::ParameterModelId id)
		{
			if (id.empty())
				return -1;

			auto idString = id.mid(1, 1).toString();
			if (idString.startsWith(CubesUnitTypes::ParameterModelIds::Defaults().itemGroupName.toString())
				&& idString.size() > CubesUnitTypes::ParameterModelIds::Defaults().itemGroupName.toString().size() + 1)
			{
				return idString.mid(5).toInt();
			}
			return -1;
		}
	};

	struct ParameterModel
	{
	public:
		ParameterModelId id; // id path, separated by /
		QString name;
		QVariant value;
		ParameterInfoId parameterInfoId;
		EditorSettings editorSettings;
		QList<ParameterModel> parameters;
		bool readOnly;

	public:
		ParameterModel()
		{

			QString ss1 = "asdfghjkl";
			auto ii1 = ss1.indexOf("");
			auto ii2 = ss1.indexOf("-");
			QString ss2 = "";
			auto ii3 = ss2.indexOf("");
			auto ii4 = ss2.indexOf("-");

			ParameterModelId a("a/b/c/d/e");
			ParameterModelId b("c/d/e");
			ParameterModelId c("a/b");


			auto x = a.left(2);
			auto z = a.right(2);
			auto sss = a.endsWith("");













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