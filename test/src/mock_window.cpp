#include "parameters/yaml_parser.h"
#include "file/file_item.h"
#include "file/file_items_manager.h"
#include "log/log_table_model.h"
#include "log/sort_filter_model.h"
#include "parameters/base64.h"
#include "properties/properties_item.h"
#include "properties/properties_items_manager.h"
#include "analysis/analysis_manager.h"
#include "tree/tree_item_model.h"
#include "xml/xml_parser.h"
#include "xml/xml_writer.h"
#include "xml/xml_helper.h"
#include "mock_window.h"

MockWindow::MockWindow()
    : MockWindow("")
{
}

MockWindow::MockWindow(const QString& parametersPath)
    : TopManager(false, true, parametersPath)
{
    modified_ = false;
    uniqueNumber_ = 0;
}

MockWindow::~MockWindow()
{
}

// ILogManager
void MockWindow::AddMessage(const CubesLog::Message& m)
{
    TopManager::AddMessage(m);

    messages_.push_back(m);
}

// Units
bool MockWindow::SortUnitsBoost()
{
    if (!TopManager::SortUnitsBoost())
        return false;
   
    return true;
}

bool MockWindow::SortUnitsRectangular(bool check)
{
    if (!TopManager::SortUnitsRectangular(check))
        return false;

    return true;
}

// Files
bool MockWindow::NewFile()
{
    if (!TopManager::NewFile())
        return false;

    return true;
}

bool MockWindow::SaveFile(const QString& path)
{
    if (!TopManager::SaveFile(path))
        return false;

    return true;
}

bool MockWindow::SaveFolder(const QString& path)
{
    if (!TopManager::SaveFolder(path))
        return false;

    return true;
}

bool MockWindow::OpenFile(const QString& path)
{
    if (!TopManager::OpenFile(path))
        return false;

    return true;
}

bool MockWindow::OpenFolder(const QString& path)
{
    if (!TopManager::OpenFolder(path))
        return false;

    return true;
}

bool MockWindow::ImportXml(const QString& path)
{
    if (!TopManager::ImportXml(path))
        return false;

    return true;
}

QVector<CubesLog::Message> MockWindow::GetMessages()
{
    return messages_;
}

void MockWindow::ClearMessages()
{
    messages_.clear();
}
