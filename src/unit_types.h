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
#ifdef _DEBUG
		QString debug_;
#endif
		QStringList value_;

	public:
		ParameterModelId() = default;
		~ParameterModelId() {
			int a = 0;
		};

		ParameterModelId(const ParameterModelId& rhs)
		{
			value_ = rhs.value_;
#ifdef _DEBUG
			debug_ = value_.join("/");
#endif
		};

		ParameterModelId(ParameterModelId&& rhs) noexcept
		{
			value_ = std::move(rhs.value_);
#ifdef _DEBUG
			debug_ = value_.join("/");
#endif
		};

		ParameterModelId(const QString& value)
		{
			QString local(value);
			local.remove(QRegExp("^([\\s|/]+)"));
			local.remove(QRegExp("([\\s|/]+)$"));
			value_ = local.split("/", Qt::SkipEmptyParts);
#ifdef _DEBUG
			debug_ = value_.join("/");
#endif
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
#ifdef _DEBUG
			debug_ = value_.join("/");
#endif
			return *this;
		}

		ParameterModelId& operator=(const QString& rhs)
		{
			value_ = ParameterModelId(rhs).value_;
#ifdef _DEBUG
			debug_ = value_.join("/");
#endif
			return *this;
		}

		ParameterModelId& operator=(ParameterModelId&& rhs) noexcept
		{
			if (this == &rhs)
				return *this;

			value_ = std::move(rhs.value_);
#ifdef _DEBUG
			debug_ = value_.join("/");
#endif
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
#ifdef _DEBUG
			debug_ = value_.join("/");
#endif
			return *this;
		}

		ParameterModelId& operator+=(const QString& rhs)
		{
			value_.append(ParameterModelId(rhs).value_);
#ifdef _DEBUG
			debug_ = value_.join("/");
#endif
			return *this;
		}

		ParameterModelId& operator+=(const char* rhs)
		{
			value_.append(ParameterModelId(rhs).value_);
#ifdef _DEBUG
			debug_ = value_.join("/");
#endif
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

	//namespace ParameterModelIds
	//{
	//	constexpr auto& baseGroup = "$BASE";
	//	constexpr auto& parametersGroup = "$PARAMETERS";
	//	constexpr auto& editorGroup = "$EDITOR";
	//	constexpr auto& itemGroup = "$ITEM";
	//	constexpr auto& includesGroup = "$INCLUDES";
	//	constexpr auto& variablesGroup = "$VARIABLES";
	//	constexpr auto& dependsParameter = "$DEPENDS";
	//	constexpr auto& optionalParameter = "$OPTIONAL";
	//	constexpr auto& nameParameter = "$NAME";

	//	const CubesUnitTypes::ParameterModelId Item(int n)
	//	{
	//		return QString("%1_%2").arg(itemGroup).arg(n);
	//	}

	//	const int ItemIndex(CubesUnitTypes::ParameterModelId id)
	//	{
	//		if (id.empty())
	//			return -1;

	//		auto idString = id.mid(0, 1).toString();
	//		if (idString.startsWith(itemGroup)
	//			&& idString.size() > std::strlen(itemGroup) + 1)
	//		{
	//			return idString.mid(std::strlen(itemGroup) + 1).toInt();
	//		}
	//		return -1;
	//	}
	//}

	class ParameterModelIds
	{
		// BASE
		// BASE/NAME
		// BASE/PLATFORM
		// BASE/PATH
		// INCLUDES
		// INCLUDES/ITEM_0
		// INCLUDES/ITEM_0/NAME
		// INCLUDES/ITEM_0/VARIABLES
		// INCLUDES/ITEM_0/VARIABLES/ITEM_0/NAME
		// INCLUDES/ITEM_0/VARIABLES/ITEM_0/VALUE
		// PARAMETERS
		// PARAMETERS/NETWORKING
		// PARAMETERS/NETWORKING/ID
		// PARAMETERS/NETWORKING/ACCEPT_PORT
		// PARAMETERS/NETWORKING/KEEP_ALIVE_SEC
		// PARAMETERS/NETWORKING/TIME_CLIENT
		// PARAMETERS/NETWORKING/NETWORK_THREADS
		// PARAMETERS/NETWORKING/BROADCAST_THREADS
		// PARAMETERS/NETWORKING/CLIENTS_THREADS
		// PARAMETERS/NETWORKING/NOTIFY_READY_CLIENTS
		// PARAMETERS/NETWORKING/NOTIFY_READY_SERVERS
		// PARAMETERS/NETWORKING/CONNECT
		// PARAMETERS/NETWORKING/CONNECT/PORT
		// PARAMETERS/NETWORKING/CONNECT/IP
		// PARAMETERS/LOG
		// PARAMETERS/LOG/LOGGING_LEVEL
		// PARAMETERS/LOG/TOTAL_LOG_LIMIT_MB
		// PARAMETERS/LOG/LOG_DIR
		// EDITOR
		// EDITOR/COLOR

		// BASE
		// BASE/NAME
		// BASE/FILE_NAME
		// BASE/INCLUDE_NAME
		// PARAMETERS
		// PARAMETERS/...
		// EDITOR
		// EDITOR/POSITION_X
		// EDITOR/POSITION_Y
		// EDITOR/POSITION_Z

	public:
		const CubesUnitTypes::ParameterModelId base;
		const CubesUnitTypes::ParameterModelId name;
		const CubesUnitTypes::ParameterModelId platform;
		const CubesUnitTypes::ParameterModelId path;
		const CubesUnitTypes::ParameterModelId fileName;
		const CubesUnitTypes::ParameterModelId includeName;


		const CubesUnitTypes::ParameterModelId includes;
		const CubesUnitTypes::ParameterModelId item;
		const CubesUnitTypes::ParameterModelId filePath;
		const CubesUnitTypes::ParameterModelId variables;
		const CubesUnitTypes::ParameterModelId value;

		const CubesUnitTypes::ParameterModelId parameters;
		const CubesUnitTypes::ParameterModelId networking;
		const CubesUnitTypes::ParameterModelId id;
		const CubesUnitTypes::ParameterModelId acceptPort;
		const CubesUnitTypes::ParameterModelId keepAliveSec;
		const CubesUnitTypes::ParameterModelId timeClient;
		const CubesUnitTypes::ParameterModelId networkThreads;
		const CubesUnitTypes::ParameterModelId broadcastThreads;
		const CubesUnitTypes::ParameterModelId clientsThreads;
		const CubesUnitTypes::ParameterModelId notifyReadyClients;
		const CubesUnitTypes::ParameterModelId notifyReadyServers;
		const CubesUnitTypes::ParameterModelId connect;
		const CubesUnitTypes::ParameterModelId port;
		const CubesUnitTypes::ParameterModelId ip;
		const CubesUnitTypes::ParameterModelId log;
		const CubesUnitTypes::ParameterModelId loggingLevel;
		const CubesUnitTypes::ParameterModelId totalLogLimitMb;
		const CubesUnitTypes::ParameterModelId logDir;

		const CubesUnitTypes::ParameterModelId editor;
		const CubesUnitTypes::ParameterModelId color;
		const CubesUnitTypes::ParameterModelId depends;
		const CubesUnitTypes::ParameterModelId optional;
		const CubesUnitTypes::ParameterModelId positionX;
		const CubesUnitTypes::ParameterModelId positionY;
		const CubesUnitTypes::ParameterModelId positionZ;

	public:
		ParameterModelIds():
			base("$BASE"),
			name("$NAME"),
			platform("$PLATFORM"),
			path("$PATH"),
			fileName("$FILE_NAME"),
			includeName("$INCLUDE_NAME"),

			includes("$INCLUDES"),
			item("$ITEM"),
			filePath("$FILE_PATH"),
			variables("$VARIABLES"),
			value("$VALUE"),

			parameters("$PARAMETERS"),
			networking("$NETWORKING"),
			id("$ID"),
			acceptPort("$ACCEPT_PORT"),
			keepAliveSec("$KEEP_ALIVE_SEC"),
			timeClient("$TIME_CLIENT"),
			networkThreads("$NETWORK_THREADS"),
			broadcastThreads("$BROADCAST_THREADS"),
			clientsThreads("$CLIENTS_THREADS"),
			notifyReadyClients("$NOTIFY_READY_CLIENTS"),
			notifyReadyServers("$NOTIFY_READY_SERVERS"),
			connect("$CONNECT"),
			port("$PORT"),
			ip("$IP"),
			log("$LOG"),
			loggingLevel("$LOGGING_LEVEL"),
			totalLogLimitMb("$TOTAL_LOG_LIMIT_MB"),
			logDir("$LOG_DIR"),

			editor("$EDITOR"),
			color("$COLOR"),
			depends("$DEPENDS"),
			optional("$OPTIONAL"),
			positionX("$POSITION_X"),
			positionY("$POSITION_Y"),
			positionZ("$POSITION_Z")
		{}

	//private:
	//	static const ParameterModelIds& Defaults()
	//	{
	//		static const ParameterModelIds ids{
	//			QString::fromLocal8Bit("$BASE"),
	//			QString::fromLocal8Bit("$PARAMETERS"),
	//			QString::fromLocal8Bit("$EDITOR"),
	//			QString::fromLocal8Bit("$ITEM"),
	//			QString::fromLocal8Bit("$INCLUDES"),
	//			QString::fromLocal8Bit("$VARIABLES"),
	//			QString::fromLocal8Bit("$DEPENDS"),
	//			QString::fromLocal8Bit("$OPTIONAL")
	//		};
	//		return ids;
	//	}

	public:
		//static const CubesUnitTypes::ParameterModelId DefaultItemGroup(int n)
		//{
		//	return QString("%1_%2").arg(Defaults().itemGroup.toString()).arg(n);
		//}

		//static const int DefaultItemGroupIndex(CubesUnitTypes::ParameterModelId id)
		//{
		//	if (id.empty())
		//		return -1;

		//	auto idString = id.mid(0, 1).toString();
		//	auto itemGroupString = Defaults().itemGroup.toString();
		//	if (idString.startsWith(itemGroupString)
		//		&& idString.size() > itemGroupString.size() + 1)
		//	{
		//		return idString.mid(itemGroupString.size() + 1).toInt();
		//	}
		//	return -1;
		//}

		const CubesUnitTypes::ParameterModelId Item(int n) const
		{
			return QString("%1_%2").arg(item.toString()).arg(n);
		}

		const int ItemIndex(CubesUnitTypes::ParameterModelId id) const
		{
			if (id.empty())
				return -1;

			auto idString = id.mid(0, 1).toString();
			auto itemGroupString = item.toString();
			if (idString.startsWith(itemGroupString)
				&& idString.size() > itemGroupString.size() + 1)
			{
				return idString.mid(itemGroupString.size() + 1).toInt();
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
			readOnly = false;

			//struct XXX
			//{
			//public:
			//	QStringList value_;

			//public:
			//	XXX() = default;

			//	//XXX()
			//	//{
			//	//};

			//	XXX(const XXX& rhs)
			//	{
			//		value_ = rhs.value_;
			//	};

			//	XXX(XXX&& rhs) noexcept
			//	{
			//		value_ = std::move(rhs.value_);
			//	};

			//	XXX(const QString& value)
			//	{
			//		value_.push_back(value);
			//	}

			//	XXX(const char* value) :
			//		XXX(QString(value))
			//	{}
			//};

			//XXX x("123456");

			//QList<XXX> list1;
			//{
			//	list1.push_back(x);
			//}

			//int z1 = list1.size();
			//auto zs = list1.at(0).value_;

			//ParameterModelId v("aaaa");

			//QList<ParameterModelId> list;
			//{
			//	list.push_back(v);
			//}

			//int z = list.size();
			//auto zs1 = list.at(0).value_;

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