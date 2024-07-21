#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <regex>
#include <QTextStream>
#include "../properties/properties_item_types.h"
#include "../properties/properties_item.h"
#include "../unit/unit_types.h"
#include "../log/log_helper.h"
#include "xml_parser.h"
#include "xml_writer.h"
#include "xml_helper.h"

using namespace CubesXml;

#define CFRC(retcode, function) do { function; return retcode; } while(0)

bool Helper::Parse(QByteArray& byteArray, const QString& fileName, File& fi, CubesLog::ILogManager* logManager)
{
	CubesLog::LogHelper logHelper(logManager, CubesLog::SourceType::xmlHelper, GetHelperErrorDescriptions());
	Parser parser(logManager);
	if (!parser.Parse(byteArray, fileName))
		CFRC(false, logHelper.LogError(static_cast<CubesLog::BaseErrorCode>(HelperErrorCode::fileParseFailed),
			{ { "File name", fileName } }));
	fi = parser.GetFile(); // const& extends lifetime of object
	return true;
}

bool Helper::Parse(const QString& fileName, File& fi, CubesLog::ILogManager* logManager)
{
	CubesLog::LogHelper logHelper(logManager, CubesLog::SourceType::xmlHelper, GetHelperErrorDescriptions());
	Parser parser(logManager);
	if (!parser.Parse(fileName))
		CFRC(false, logHelper.LogError(static_cast<CubesLog::BaseErrorCode>(HelperErrorCode::fileParseFailed),
			{ { "File name", fileName } }));
	fi = parser.GetFile(); // const& extends lifetime of object
	return true;
}

bool Helper::GetElement(Unit& unit, const CubesUnit::ParameterModelId& id, Element& element, CubesLog::ILogManager* logManager)
{
	CubesLog::LogHelper logHelper(logManager, CubesLog::SourceType::xmlHelper, GetHelperErrorDescriptions());
	const static CubesUnit::ParameterModelIds ids;

	auto ss = id.split();
	if (ss.isEmpty())
		CFRC(false, logHelper.LogError(static_cast<CubesLog::BaseErrorCode>(HelperErrorCode::invalidArgument),
			CubesLog::DefaultDescription, "ParameterModelId is empty"));

	if (ss.front() != ids.parameters)
		CFRC(false, logHelper.LogError(static_cast<CubesLog::BaseErrorCode>(HelperErrorCode::invalidArgument),
			CubesLog::DefaultDescription, "Must be started with parameters"));

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
				CFRC(false, logHelper.LogError(static_cast<CubesLog::BaseErrorCode>(HelperErrorCode::unitParametersMalformed),
					CubesLog::DefaultDescription, "Array item not found", { {"Item", s.toString()} }));

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
						CFRC(false, logHelper.LogError(static_cast<CubesLog::BaseErrorCode>(HelperErrorCode::unitParametersMalformed),
							CubesLog::DefaultDescription, "Not last part of id must be array", { {"Item", s.toString()} }));

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
	CubesLog::LogHelper logHelper(logManager, CubesLog::SourceType::xmlHelper, GetHelperErrorDescriptions());
	Writer writer(logManager);
	if (!writer.Write(buffer, fi))
		CFRC(false, logHelper.LogError(static_cast<CubesLog::BaseErrorCode>(HelperErrorCode::bufferWriteFailed),
			{ {"", fi.fileName} }));
	return true;
}

bool Helper::Write(const QString& filename, const File& fi, CubesLog::ILogManager* logManager)
{
	CubesLog::LogHelper logHelper(logManager, CubesLog::SourceType::xmlHelper, GetHelperErrorDescriptions());
	Writer writer(logManager);
	if (!writer.Write(filename, fi))
		CFRC(false, logHelper.LogError(static_cast<CubesLog::BaseErrorCode>(HelperErrorCode::fileWriteFailed),
			{ {"", fi.fileName} }));
	return true;
}
