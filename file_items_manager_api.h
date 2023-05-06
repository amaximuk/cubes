#pragma once

class file_item;

class file_items_manager_interface
{
public:
	virtual void InformNameChanged(file_item* fi, QString oldName) = 0;
	virtual void InformIncludeChanged(file_item* fi) = 0;
};
