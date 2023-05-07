#pragma once

class file_item;

class file_items_manager_interface
{
public:
	virtual void InformNameChanged(QString fileName, QString oldFileName) = 0;
	virtual void InformIncludeChanged(QString fileName, QStringList includeNames) = 0;
	virtual void InformIncludeNameChanged(QString fileName, QString includeName, QString oldIncludeName) = 0;
};
