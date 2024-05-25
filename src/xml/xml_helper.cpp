#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <regex>
#include <QTextStream>
#include "../properties/properties_item_types.h"
#include "../properties/properties_item.h"
#include "../unit/unit_types.h"
#include "xml_parser.h"
#include "xml_writer.h"
#include "xml_helper.h"

using namespace CubesXml;

#define CFRC(retcode, function) do { function; return retcode; } while(0)

static void LogError(CubesLog::ILogManager* logManager, CubesXml::HelperErrorCode errorCode,
	const QString& details, const QVector<CubesLog::Variable>& variables)
{
	if (logManager != nullptr)
	{
		CubesLog::Message lm{};
		lm.type = CubesLog::MessageType::error;
		lm.code = CubesLog::CreateCode(CubesLog::MessageType::error,
			CubesLog::SourceType::xmlHelper, static_cast<uint32_t>(errorCode));
		lm.source = CubesLog::SourceType::xmlHelper;
		lm.description = CubesXml::GetHelperErrorDescription(errorCode);
		lm.details = details;
		lm.variables = variables;
		lm.tag = CubesUnitTypes::InvalidUniversalId;
		logManager->AddMessage(lm);
	}
}

static void LogError(CubesLog::ILogManager* logManager, CubesXml::HelperErrorCode errorCode)
{
	LogError(logManager, errorCode, {}, {});
}

static void LogError(CubesLog::ILogManager* logManager, CubesXml::HelperErrorCode errorCode,
	const QString& details)
{
	LogError(logManager, errorCode, details, {});
}

static void LogError(CubesLog::ILogManager* logManager, CubesXml::HelperErrorCode errorCode,
	const QVector<CubesLog::Variable>& variables)
{
	LogError(logManager, errorCode, {}, variables);
}

bool Helper::Parse(QByteArray& byteArray, const QString& fileName, File& fi, CubesLog::ILogManager* logManager)
{
	Parser parser(logManager);
	if (!parser.Parse(byteArray, fileName))
		CFRC(false, LogError(logManager, HelperErrorCode::fileParseFailed, { { "File name", fileName } }));
	fi = parser.GetFile(); // const& extends lifetime of object
	return true;
}

bool Helper::Parse(const QString& fileName, File& fi, CubesLog::ILogManager* logManager)
{
	Parser parser(logManager);
	if (!parser.Parse(fileName))
		CFRC(false, LogError(logManager, HelperErrorCode::fileParseFailed, { { "File name", fileName } }));
	fi = parser.GetFile(); // const& extends lifetime of object
	return true;
}

bool Helper::GetElement(Unit& unit, const CubesUnitTypes::ParameterModelId& id, Element& element, CubesLog::ILogManager* logManager)
{
	const static CubesUnitTypes::ParameterModelIds ids;

	auto ss = id.split();
	if (ss.isEmpty())
		CFRC(false, LogError(logManager, HelperErrorCode::invalidArgument, "ParameterModelId is empty"));
	if (ss.front() != ids.parameters)
		CFRC(false, LogError(logManager, HelperErrorCode::invalidArgument, "Must be started with parameters"));
	ss.pop_front();

	// ������������ ���� � ��������� �� id, ������ ��������� Unit
	// ���� ������ ���������� �� ������ ����� � ������ ��������
	// ���� ������ ���������� � ${PARAMETERS}, � �����, �� ���������
	// ������������, ���� ����� ��������� ������ ${ITEM_N} � ${PARAMETERS}
	// ��������� �� ����� ���� ���������� � ������ ��������, ������ � ������
	// �������, ��� ��������� ������ ��������� �������� ������ ���� ��� � �����
	// ����������� �������� ������ ����� �������

	bool inside_array = false;
	Array* array = nullptr;
	QString array_type;
	QList<Param>* params = &unit.params;
	QList<Array>* arrays = &unit.arrays;
	while (ss.size() > 0)
	{
		const auto& s = ss.front();
		if (inside_array)
		{
			// ������ ������� �������� ���� ${ITEM_N}
			// ������� ����������� � ��������� ������ ���� � id � ���� ������
			auto index = ids.ItemIndex(s);
			if (index != -1 && array->items.size() > index)
			{
				if (ss.size() == 1)
				{
					// ����� ������� ������� �������
					element = {};
					element.type = ElementType::Item;
					element.arrayType = array_type;
					element.item = &array->items[index];
					return true;
				}

				params = &array->items[index].params;
				arrays = &array->items[index].arrays;
			}
			else
				CFRC(false, LogError(logManager, HelperErrorCode::unitParametersMalformed,
					"Array item not found", { {"Item", s.toString()} }));

			array = nullptr;
			inside_array = false;
		}
		else
		{
			// ��������� �� ����� ���� ���������� � ������ ��������, ������ � ������
			// �������, ��� ��������� ������ ��������� �������� ������ ���� ��� � �����
			// ����������� �������� ������ ����� �������

			// ��������� ���������� ����� ��������� � ��������� ������ ���� � id
			for (auto& p : *params)
			{
				if (s == p.name)
				{
					if (ss.size() != 1)
						CFRC(false, LogError(logManager, HelperErrorCode::unitParametersMalformed,
							"Not last part of id must be array", { {"Item", s.toString()} }));

					// ����� ������� ��������
					element = {};
					element.type = ElementType::Param;
					element.param = &p;
					return true;
				}
			}

			// ��������� ���������� ����� ������� � ��������� ������ ���� � id
			for (auto& a : *arrays)
			{
				if (s == a.name)
				{
					if (ss.size() == 1)
					{
						// ����� ��������-������
						element = {};
						element.type = ElementType::Array;
						element.arrayType = a.type;
						element.itemsCount = a.items.size();
						element.array = &a;
						return true;
					}

					// ����� ������, ���� � ����
					array = &a;
					array_type = a.type;
					array->items.size();
					inside_array = true;
					break;
				}
			}
		}
		ss.pop_front();
	}

	if (id.endsWith(ids.parameters))
	{
		// ����� ���������, ���������� ��� ������� ������ ����������
		element = {};
		element.type = ElementType::Service;
		element.params = params;
		element.arrays = arrays;
		return true;
	}

	// ���� ����� �� ����, ������ ������� �� ������
	// ��� �� ������, � xml ����� ����� ���������� ����� �������������
	element = {};
	return true;
}

bool Helper::Write(QByteArray& buffer, const File& fi, CubesLog::ILogManager* logManager)
{
	Writer writer(logManager);
	if (!writer.Write(buffer, fi))
		CFRC(false, LogError(logManager, HelperErrorCode::bufferWriteFailed, { {"", fi.fileName} }));
	return true;
}

bool Helper::Write(const QString& filename, const File& fi, CubesLog::ILogManager* logManager)
{
	Writer writer(logManager);
	if (!writer.Write(filename, fi))
		CFRC(false, LogError(logManager, HelperErrorCode::fileWriteFailed, { {"", fi.fileName} }));
	return true;
}
