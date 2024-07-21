#pragma once

#include "unit_types.h"

namespace CubesUnit
{
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

		bool operator!=(const ParameterModelId& rhs) const
		{
			return !(*this == rhs);
		}

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
	};
}
