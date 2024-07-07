#pragma once

#include <QVector>
#include "log/log_types.h"

bool CheckCode(const QVector<CubesLog::Message>& messages, const QString& code)
{
	const auto it = std::find_if(messages.cbegin(), messages.cend(), [&code](const CubesLog::Message& m) {
		return m.code == code; });
	
	return it != messages.end();
}

bool CheckAllCodes(const QVector<CubesLog::Message>& messages, const QVector<QString>& codes)
{
	for (const auto& code : codes)
	{
		if (!CheckCode(messages, code))
			return false;
	}

	return true;
}

class LogManager : public CubesLog::ILogManager
{
private:
	QVector<CubesLog::Message> messages_;

public:
	void AddMessage(const CubesLog::Message& m) override
	{
		messages_.push_back(m);
	}

	QVector<CubesLog::Message> GetMessages()
	{
		return messages_;
	}
};
