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
#include "mock_window.h"
#include "test.h"

TEST(CUBES, FileAnalysisNoMainConfig)
{
	MockWindow mockWindow;
	ASSERT_TRUE(mockWindow.Test());
	const auto messages = mockWindow.GetMessages();
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::fileAnalysis,
		static_cast<uint32_t>(CubesAnalysis::FileAnalysisErrorCode::noMainConfig)));
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
	CubesUnitTypes::ParameterModelIds ids_;

	auto fileModels = mockWindow.GetFileIdParameterModelsRef();
	ASSERT_EQ(fileModels.size(), 2);
	for (auto item : fileModels)
	{
		ASSERT_NE(item, nullptr);
		CubesUnitTypes::GetParameterModel(*item, ids_.base + ids_.name)->value = QString::fromLocal8Bit("FileName1");
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


//fileNameNotUnique,
//fileIdNotUnique