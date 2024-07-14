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
#include "xml/xml_parser.h"
#include "mock_window.h"
#include "test.h"

TEST(CUBES, XmlReaderFileParseFailed)
{
	LogManager logManager;
	CubesXml::Parser p(&logManager);
	ASSERT_FALSE(p.Parse("../../test/resources/XmlParser/FileParseFailed/config1.xml"));
	const auto messages = logManager.GetMessages();


	//MockWindow mockWindow;
	//ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/FileParseFailed/config1.xml"));
	//const auto messages = mockWindow.GetMessages();
	//ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	//codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
	//	static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::fileParseFailed)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, XmlReaderFileOpenFailed)
{
	MockWindow mockWindow;
	ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlReader/FileOpenFailed/invalid_name.xml"));
	const auto messages = mockWindow.GetMessages();
	ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
		static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::fileOpenFailed)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, XmlReaderGetIncludesFailed)
{
	MockWindow mockWindow;
	ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/GetIncludesFailed/config1.xml"));
	const auto messages = mockWindow.GetMessages();
	ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
		static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getIncludesFailed)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, XmlReaderGetConfigFailed)
{
	MockWindow mockWindow;
	ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/GetConfigFailed/config1.xml"));
	const auto messages = mockWindow.GetMessages();
	ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
		static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getConfigFailed)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, XmlReaderIncludesChildUnknown)
{
	MockWindow mockWindow;
	ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/IncludesChildUnknown/config1.xml"));
	const auto messages = mockWindow.GetMessages();
	ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
		static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getIncludesFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::includesChildUnknown)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, XmlReaderIncludesIncludeValEmpty)
{
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/IncludesIncludeValEmpty/config1.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getIncludesFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::includesIncludeValEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/IncludesIncludeValEmpty/config2.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getIncludesFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::includesIncludeValEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
}

TEST(CUBES, XmlReaderIncludesIncludeVariableNameEmpty)
{
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/IncludesIncludeVariableNameEmpty/config1.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getIncludesFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::includesIncludeVariableNameEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/IncludesIncludeVariableNameEmpty/config2.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getIncludesFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::includesIncludeVariableNameEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
}

TEST(CUBES, XmlReaderIncludesIncludeVariableValEmpty)
{
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/IncludesIncludeVariableValEmpty/config1.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getIncludesFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::includesIncludeVariableValEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/IncludesIncludeVariableValEmpty/config2.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getIncludesFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::includesIncludeVariableValEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
}

TEST(CUBES, XmlReaderIncludesIncludeVariableNameDuplicate)
{
	MockWindow mockWindow;
	ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/IncludesIncludeVariableNameDuplicate/config1.xml"));
	const auto messages = mockWindow.GetMessages();
	ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
		static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getIncludesFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::includesIncludeVariableNameDuplicate)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, XmlReaderGetNetworkingFailed)
{
	MockWindow mockWindow;
	ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/GetNetworkingFailed/config1.xml"));
	const auto messages = mockWindow.GetMessages();
	ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
		static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getNetworkingFailed)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, XmlReaderGetLogFailed)
{
	MockWindow mockWindow;
	ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/GetLogFailed/config1.xml"));
	const auto messages = mockWindow.GetMessages();
	ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
		static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getLogFailed)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, XmlReaderGetUnitsFailed)
{
	MockWindow mockWindow;
	ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/GetUnitsFailed/config1.xml"));
	const auto messages = mockWindow.GetMessages();
	ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
		static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitsFailed)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, XmlReaderGetConfigChildUnknown)
{
	MockWindow mockWindow;
	ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/GetConfigChildUnknown/config1.xml"));
	const auto messages = mockWindow.GetMessages();
	ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
		static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getConfigFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::configChildUnknown)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, XmlReaderNetworkingIdEmpty)
{
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/NetworkingIdEmpty/config1.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getNetworkingFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::networkingIdEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/NetworkingIdEmpty/config2.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getNetworkingFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::networkingIdEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
}

TEST(CUBES, XmlReaderNetworkingAcceptPortEmpty)
{
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/NetworkingAcceptPortEmpty/config1.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getNetworkingFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::networkingAcceptPortEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/NetworkingAcceptPortEmpty/config2.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getNetworkingFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::networkingAcceptPortEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
}

TEST(CUBES, XmlReaderNetworkingKeepAliveSecEmpty)
{
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/NetworkingKeepAliveSecEmpty/config1.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getNetworkingFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::networkingKeepAliveSecEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/NetworkingKeepAliveSecEmpty/config2.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getNetworkingFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::networkingKeepAliveSecEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
}

TEST(CUBES, XmlReaderNetworkingChildUnknown)
{
	MockWindow mockWindow;
	ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/NetworkingChildUnknown/config1.xml"));
	const auto messages = mockWindow.GetMessages();
	ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
		static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getNetworkingFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::networkingChildUnknown)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, XmlReaderNetworkingConnectPortEmpty)
{
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/NetworkingConnectPortEmpty/config1.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getNetworkingFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::networkingConnectPortEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/NetworkingConnectPortEmpty/config2.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getNetworkingFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::networkingConnectPortEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
}

TEST(CUBES, XmlReaderNetworkingConnectIpEmpty)
{
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/NetworkingConnectIpEmpty/config1.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getNetworkingFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::networkingConnectIpEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/NetworkingConnectIpEmpty/config2.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getNetworkingFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::networkingConnectIpEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
}

TEST(CUBES, XmlReaderLogChildUnknown)
{
	MockWindow mockWindow;
	ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/LogChildUnknown/config1.xml"));
	const auto messages = mockWindow.GetMessages();
	ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
		static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getLogFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::logChildUnknown)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, XmlReaderLogParamUnknown)
{
	MockWindow mockWindow;
	ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/LogParamUnknown/config1.xml"));
	const auto messages = mockWindow.GetMessages();
	ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
		static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getLogFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::logParamUnknown)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, XmlReaderUnitsChildUnknown)
{
	MockWindow mockWindow;
	ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/UnitsChildUnknown/config1.xml"));
	const auto messages = mockWindow.GetMessages();
	ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
		static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitsFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::unitsChildUnknown)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, XmlReaderGetGroupFailed)
{
	MockWindow mockWindow;
	ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/GetGroupFailed/config1.xml"));
	const auto messages = mockWindow.GetMessages();
	ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
		static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getGroupFailed)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, XmlReaderGroupParamNotFound)
{
	MockWindow mockWindow;
	ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/GroupParamNotFound/config1.xml"));
	const auto messages = mockWindow.GetMessages();
	ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
		static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getGroupFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::groupParamNotFound)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, XmlReaderGroupParamDuplicate)
{
	MockWindow mockWindow;
	ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/GroupParamDuplicate/config1.xml"));
	const auto messages = mockWindow.GetMessages();
	ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
		static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getGroupFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::groupParamDuplicate)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, XmlReaderGroupParamUnknown)
{
	MockWindow mockWindow;
	ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/GroupParamUnknown/config1.xml"));
	const auto messages = mockWindow.GetMessages();
	ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
		static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getGroupFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::groupParamUnknown)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, XmlReaderGetUnitFailed)
{
	MockWindow mockWindow;
	ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/GetUnitFailed/config1.xml"));
	const auto messages = mockWindow.GetMessages();
	ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
		static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitsFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitFailed)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, XmlReaderUnitNameEmpty)
{
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/UnitNameEmpty/config1.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitsFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::unitNameEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/UnitNameEmpty/config2.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitsFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::unitNameEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
}

TEST(CUBES, XmlReaderUnitIdEmpty)
{
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/UnitIdEmpty/config1.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitsFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::unitIdEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/UnitIdEmpty/config2.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitsFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::unitIdEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
}

TEST(CUBES, XmlReaderGetParamFailed)
{
	MockWindow mockWindow;
	ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/GetParamFailed/config1.xml"));
	const auto messages = mockWindow.GetMessages();
	ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
		static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitsFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getParamFailed)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, XmlReaderGetArrayFailed)
{
	MockWindow mockWindow;
	ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/GetArrayFailed/config1.xml"));
	const auto messages = mockWindow.GetMessages();
	ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
		static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitsFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getArrayFailed)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, XmlReaderGetDependsFailed)
{
	MockWindow mockWindow;
	ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/GetDependsFailed/config1.xml"));
	const auto messages = mockWindow.GetMessages();
	ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
		static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitsFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getDependsFailed)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, XmlReaderUnitParamNameEmpty)
{
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/UnitParamNameEmpty/config1.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitsFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getParamFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::unitParamNameEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/UnitParamNameEmpty/config2.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitsFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getParamFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::unitParamNameEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
}

TEST(CUBES, XmlReaderUnitParamTypeEmpty)
{
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/UnitParamTypeEmpty/config1.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitsFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getParamFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::unitParamTypeEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/UnitParamTypeEmpty/config2.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitsFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getParamFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::unitParamTypeEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
}

TEST(CUBES, XmlReaderUnitParamValEmpty)
{
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/UnitParamValEmpty/config1.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitsFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getParamFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::unitParamValEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/UnitParamValEmpty/config2.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitsFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getParamFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::unitParamValEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
}

TEST(CUBES, XmlReaderGetItemFailed)
{
	MockWindow mockWindow;
	ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/GetItemFailed/config1.xml"));
	const auto messages = mockWindow.GetMessages();
	ASSERT_FALSE(messages.isEmpty());
	QVector<QString> codes;
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
		static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitsFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getArrayFailed)));
	codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
		static_cast<uint32_t>(CubesXml::ParserErrorCode::getItemFailed)));
	ASSERT_TRUE(CheckAllCodes(messages, codes));
}

TEST(CUBES, XmlReaderUnitDependsItemEmpty)
{
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/UnitDependsItemEmpty/config1.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitsFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getDependsFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::unitDependsItemEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
	{
		MockWindow mockWindow;
		ASSERT_FALSE(mockWindow.ImportXml("../../test/resources/XmlParser/UnitDependsItemEmpty/config2.xml"));
		const auto messages = mockWindow.GetMessages();
		ASSERT_FALSE(messages.isEmpty());
		QVector<QString> codes;
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper,
			static_cast<uint32_t>(CubesXml::HelperErrorCode::fileParseFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitsFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getUnitFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::getDependsFailed)));
		codes.push_back(CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlParser,
			static_cast<uint32_t>(CubesXml::ParserErrorCode::unitDependsItemEmpty)));
		ASSERT_TRUE(CheckAllCodes(messages, codes));
	}
}
