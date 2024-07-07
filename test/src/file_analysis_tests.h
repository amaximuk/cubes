#pragma once

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
#include "test.h"

TEST(CUBES, FileAnalysisNoMainConfig)
{
	MockWindow mockWindow;
	//ASSERT_TRUE(mockWindow.ImportXml("../../test/resources/Analysis/NoMainConfig/config1.xml"));
	//ASSERT_FALSE(mockWindow.SaveFile("../../test/resources/XmlWriter/NotNoMainConfig/config1.xml"));
	ASSERT_TRUE(mockWindow.Test());
	const auto messages = mockWindow.GetMessages();
	//ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::fileAnalysis,
		static_cast<uint32_t>(CubesAnalysis::FileAnalysisErrorCode::noMainConfig)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

//
//enum class WriterErrorCode
//{
//	ok = 0,
//	fileSetFailed = 1000,
//	bufferWriteFailed,
//	fileOpenFailed,
//	fileWriteFailed,
//	setIncludesFailed,
//	setConfigFailed,
//	setNetworkingFailed,
//	setLogFailed,
//	setGroupFailed,
//	setUnitFailed,
//	setParamFailed,
//	setDependsFailed,
//	setItemFailed,
//	setArrayFailed
//};
//
