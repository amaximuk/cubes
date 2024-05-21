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
//using namespace CubesXml::Helper;

#define ELRC(retcode, message) do {\
		std::stringstream ss;\
		ss << message;\
		if (logManager != nullptr)\
		{\
			CubesLog::LogMessage lm{};\
			lm.type = CubesLog::MessageType::error;\
			lm.code = CubesLog::CreateCode(CubesLog::MessageType::error, CubesLog::SourceType::xmlHelper, 1000);\
			lm.source = CubesLog::SourceType::xmlHelper;\
			lm.description = QString::fromStdString(ss.str());\
			lm.tag = CubesUnitTypes::InvalidUniversalId;\
			logManager->AddMessage(lm);\
		}\
		std::cout << ss.str() << std::endl; return retcode;\
	} while(0)

bool Helper::Parse(QByteArray& byteArray, const QString& fileName, File& fi, CubesLog::ILogManager* logManager)
{
	Parser parser(logManager);
	if (!parser.Parse(byteArray, fileName))
		ELRC(false, "Parsing failed (" << fileName.toStdString() << ")");
	fi = parser.GetFile(); // const& extends lifetime of object
	return true;
}

bool Helper::Parse(const QString& fileName, File& fi, CubesLog::ILogManager* logManager)
{
	Parser parser(logManager);
	if (!parser.Parse(fileName))
		ELRC(false, "Parsing failed (" << fileName.toStdString() << ")");
	fi = parser.GetFile(); // const& extends lifetime of object
	return true;
}

bool Helper::GetElement(Unit& unit, const CubesUnitTypes::ParameterModelId& id, Element& element, CubesLog::ILogManager* logManager)
{
	const static CubesUnitTypes::ParameterModelIds ids;

	auto ss = id.split();
	if (ss.isEmpty())
		ELRC(false, "ParameterModelId is empty");
	if (ss.front() != ids.parameters)
		ELRC(false, "Must be started with parameters");
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
				ELRC(false, "Array item not found");

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
						ELRC(false, "Not last part of id must be array");

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

/*
int Helper::GetItemsCount(Unit& unit, const CubesUnitTypes::ParameterModelId& id, CubesLog::ILogManager* logManager)
{
	const static CubesUnitTypes::ParameterModelIds ids;

	auto ss = id.split();
	if (ss.size() < 2)
		ELRC(-1, "ParameterModelId too short");
	if (ss.front() != ids.parameters)
		ELRC(-1, "Must be started with parameters");
	ss.pop_front();

	bool inside_array = false;
	Array* array = nullptr;
	QList<Param>* params = &unit.params;
	QList<Array>* arrays = &unit.arrays;
	while (ss.size() > 0)
	{
		const auto& s = ss.front();
		if (inside_array)
		{
			auto index = ids.ItemIndex(s);
			if (index != -1 && array->items.size() > index)
			{
				params = &array->items[index].params;
				arrays = &array->items[index].arrays;
			}
			else
				ELRC(-1, "Part of id not found");

			array = nullptr;
			inside_array = false;
		}
		else
		{
			for (auto& p : *params)
			{
				if (s == p.name)
				{
					ELRC(-1, "Is not array");
				}
			}

			for (auto& a : *arrays)
			{
				if (s == a.name)
				{
					array = &a;
					inside_array = true;
					break;
				}
			}
		}
		ss.pop_front();
	}

	if (array == nullptr)
		ELRC(-1, "Not found");

	return array->items.size();
}

Param* Helper::GetParam(Unit& unit, const CubesUnitTypes::ParameterModelId& id, CubesLog::ILogManager* logManager)
{
	const static CubesUnitTypes::ParameterModelIds ids;

	auto ss = id.split();
	if (ss.size() < 2)
		ELRC(nullptr, "ParameterModelId too short");
	if (ss.front() != ids.parameters)
		ELRC(nullptr, "Must be started with parameters");
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
				params = &array->items[index].params;
				arrays = &array->items[index].arrays;
			}
			else
				ELRC(nullptr, "Array item not found");

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
						ELRC(nullptr, "Not last part of id must be array");

					// ����� ������� ��������
					return &p;
				}
			}

			// ��������� ���������� ����� ������� � ��������� ������ ���� � id
			for (auto& a : *arrays)
			{
				if (s == a.name)
				{
					// ����� ������, ���� � ����
					array = &a;
					inside_array = true;
					break;
				}
			}
		}
		ss.pop_front();
	}
	return nullptr;
}

Item* Helper::GetItem(Unit& unit, const CubesUnitTypes::ParameterModelId& id, QString& type, CubesLog::ILogManager* logManager)
{
	const static CubesUnitTypes::ParameterModelIds ids;

	auto ss = id.split();
	if (ss.size() < 2)
		ELRC(nullptr, "ParameterModelId too short");
	if (ss.front() != ids.parameters)
		ELRC(nullptr, "Must be started with parameters");
	ss.pop_front();

	// � ������� �� GetParam ���� ������� ������� � ��� ���

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
					type = array_type;
					return &array->items[index];
				}

				// ���� ������ ��������
				params = &array->items[index].params;
				arrays = &array->items[index].arrays;
			}
			else
				ELRC(nullptr, "Array item not found");

			array = nullptr;
			inside_array = false;
		}
		else
		{
			// ��������� �� ����� ���� ���������� � ������ ��������, ������ � ������
			// ����������� �������� ������ ����� �������
			// ��������� �� ���� ������� �������, ��������� �� ���������

			// ��������� ���������� ����� ������� � ��������� ������ ���� � id
			for (auto& a : *arrays)
			{
				if (s == a.name)
				{
					// ����� ������, ���� � ����
					array = &a;
					inside_array = true;
					array_type = a.type;
					break;
				}
			}
		}
		ss.pop_front();
	}
	return nullptr;
}
*/

bool Helper::Write(QByteArray& buffer, const File& fi, CubesLog::ILogManager* logManager)
{
	Writer writer(logManager);
	if (!writer.Write(buffer, fi))
		ELRC(false, "Write failed (" << fi.fileName.toStdString() << ")");
	return true;
}

bool Helper::Write(const QString& filename, const File& fi, CubesLog::ILogManager* logManager)
{
	Writer writer(logManager);
	if (!writer.Write(filename, fi))
		ELRC(false, "Write failed (" << fi.fileName.toStdString() << ")");
	return true;
}
