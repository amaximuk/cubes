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
#include "file/file_item.h"
#include "unit/unit_helper.h"
#include "mock_window.h"
#include "test.h"

TEST(CUBES, FileAnalysisNoFiles)
{
	MockWindow mockWindow;
	ASSERT_TRUE(mockWindow.Test());
	const auto messages = mockWindow.GetMessages();
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::fileAnalysis,
		static_cast<uint32_t>(CubesAnalysis::FileAnalysisErrorCode::noFiles)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, FileAnalysisNameNotUnique)
{
	MockWindow mockWindow("../../test/resources/FileAnalysis/NameNotUnique/units");
	ASSERT_TRUE(mockWindow.ImportXml("../../test/resources/FileAnalysis/NameNotUnique/config1.xml"));
	ASSERT_TRUE(mockWindow.ImportXml("../../test/resources/FileAnalysis/NameNotUnique/config2.xml"));
	auto fileItems = mockWindow.GetFileItems();


	{
		ASSERT_TRUE(mockWindow.Test());
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(CheckCode(messages, CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::fileAnalysis,
			static_cast<uint32_t>(CubesAnalysis::FileAnalysisErrorCode::nameNotUnique))));
	}


	// ???????????????????????
	// Переделать на набор model вместо объектов FileItem

	// Значения имен параметров
	CubesUnit::ParameterModelIds ids_;

	auto fileModels = mockWindow.GetFileIdParameterModelsRef();
	ASSERT_EQ(fileModels.size(), 2);
	for (auto item : fileModels)
	{
		ASSERT_NE(item, nullptr);
		CubesUnit::Helper::Common::GetParameterModel(*item, ids_.base + ids_.name)->value = QString::fromLocal8Bit("Name1");
	}



	//ASSERT_EQ(fileItems.size(), 2);
	//for (auto item : fileItems)
	//	item->SetName("FileName1");

	ASSERT_TRUE(mockWindow.Test());
	const auto messages = mockWindow.GetMessages();
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::fileAnalysis,
		static_cast<uint32_t>(CubesAnalysis::FileAnalysisErrorCode::nameNotUnique)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, FileAnalysisFileNameNotUnique)
{
	MockWindow mockWindow;
	ASSERT_TRUE(mockWindow.ImportXml("../../test/resources/FileAnalysis/FileNameNotUnique/config1.xml"));
	ASSERT_TRUE(mockWindow.ImportXml("../../test/resources/FileAnalysis/FileNameNotUnique/config1.xml"));
	auto fileItems = mockWindow.GetFileItems();

	// ???????????????????????
	// Переделать на набор model вместо объектов FileItem

	// Значения имен параметров
	CubesUnit::ParameterModelIds ids_;

	auto fileModels = mockWindow.GetFileIdParameterModelsRef();
	ASSERT_EQ(fileModels.size(), 2);
	for (auto item : fileModels)
	{
		ASSERT_NE(item, nullptr);
		CubesUnit::Helper::Common::GetParameterModel(*item, ids_.base + ids_.path)->value = QString::fromLocal8Bit("FileName1");
	}



	//ASSERT_EQ(fileItems.size(), 2);
	//for (auto item : fileItems)
	//	item->SetName("FileName1");

	ASSERT_TRUE(mockWindow.Test());
	const auto messages = mockWindow.GetMessages();
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::fileAnalysis,
		static_cast<uint32_t>(CubesAnalysis::FileAnalysisErrorCode::fileNameNotUnique)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, FileAnalysisConnectionIdNotUnique)
{
	MockWindow mockWindow;
	ASSERT_TRUE(mockWindow.ImportXml("../../test/resources/FileAnalysis/FileNameNotUnique/config1.xml"));
	ASSERT_TRUE(mockWindow.ImportXml("../../test/resources/FileAnalysis/FileNameNotUnique/config1.xml"));
	auto fileItems = mockWindow.GetFileItems();

	// ???????????????????????
	// Переделать на набор model вместо объектов FileItem

	// Значения имен параметров
	CubesUnit::ParameterModelIds ids_;

	auto fileModels = mockWindow.GetFileIdParameterModelsRef();
	ASSERT_EQ(fileModels.size(), 2);
	for (auto item : fileModels)
	{
		ASSERT_NE(item, nullptr);
		CubesUnit::Helper::Common::GetParameterModel(*item, ids_.base + ids_.path)->value = QString::fromLocal8Bit("FileName1");
	}



	//ASSERT_EQ(fileItems.size(), 2);
	//for (auto item : fileItems)
	//	item->SetName("FileName1");

	ASSERT_TRUE(mockWindow.Test());
	const auto messages = mockWindow.GetMessages();
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::fileAnalysis,
		static_cast<uint32_t>(CubesAnalysis::FileAnalysisErrorCode::connectionIdNotUnique)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

//fileNameNotUnique,
//fileFileNameNotUnique,
//fileConnectionIdNotUnique
