#pragma once

class file_item;

class file_items_manager_interface
{
public:
	virtual void InformIncludeNameChanged(file_item* fi) = 0;
	virtual void InformNameChanged(file_item* fi) = 0;
};
