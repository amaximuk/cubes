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
#include "xml/xml_writer.h"
#include "mock_window.h"
#include "test.h"

TEST(CUBES, XmlWriterFileOpenFailed)
{
	CubesXml::Parser p(nullptr);
	ASSERT_TRUE(p.Parse("../../test/resources/XmlWriter/FileOpenFailed/config1.xml"));

	LogManager logManager;
	CubesXml::Writer w(&logManager);
	ASSERT_FALSE(w.Write("../../test/resources/XmlWriter/NotFileOpenFailed/config2.xml", p.GetFile()));
	const auto messages = logManager.GetMessages();


	//MockWindow mockWindow;
	//ASSERT_TRUE(mockWindow.ImportXml("../../test/resources/XmlWriter/FileSetFailed/config1.xml"));
	//ASSERT_FALSE(mockWindow.SaveFile("../../test/resources/XmlWriter/NotFileSetFailed/config2.xml"));
	//const auto messages = mockWindow.GetMessages();
	//ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	//codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
	//	static_cast<uint32_t>(CubesXml::HelperErrorCode::fileWriteFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlWriter,
		static_cast<uint32_t>(CubesXml::WriterErrorCode::fileOpenFailed)));
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
