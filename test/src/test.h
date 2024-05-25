#ifndef TEST_H_
#define TEST_H_

#include <gtest/gtest.h>
#include <vector>
#include <list>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include "xml/xml_types.h"
#include "mock_window.h"

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

TEST(CUBES, XmlReaderTest1)
{
	MockWindow mockWindow("../../test/resources/test1/units");
	ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/test1/invalid_name.xml"));
	const auto messages = mockWindow.GetMessages();
	ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
		static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::fileOpenFailed)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

#endif // TEST_H_
/*
	enum class ParserErrorCode
	{
		ok = 0,
		fileParseFailed = 1000,
		fileOpenFailed,
		getIncludesFailed,
		getConfigFailed,
		includesChildUnknown,
		includesIncludeValEmpty,
		includesIncludeChildUnknown,
		includesIncludeVariableNameEmpty,
		includesIncludeVariableValEmpty,
		includesIncludeVariableNameDuplicate,
		getNetworkFailed,
		getLogFailed,
		getUnitsFailed,
		configChildUnknown,
		networkingIdEmpty,
		networkingAcceptPortEmpty,
		networkingKeepAliveSecEmpty,
		networkingChildUnknown,
		networkingConnectPortEmpty,
		networkingConnectIpEmpty,
		logChildUnknown,
		logParamUnknown,
		unitsChildUnknown,
		getGroupFailed,
		groupParamNotSingle,
		groupParamUnknown,
		getUnitFailed,
		unitNameEmpty,
		unitIdEmpty,
		getParamFailed,
		getArrayFailed,
		getDependsFailed,
		unitParamNameEmpty,
		unitParamTypeEmpty,
		unitParamValEmpty,
		getItemFailed,
		unitDependsItemEmpty
	};*/