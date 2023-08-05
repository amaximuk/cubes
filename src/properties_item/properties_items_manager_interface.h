#pragma once

#include <QString>

class properties_item;

class properties_items_manager_interface
{
public:
	// “ут значени€ мен€ютс€ по одному
	//virtual void BeforeFileNameChanged(const QString& fileName, const QString& oldFileName, bool& cancel) = 0;
	//virtual void AfterFileNameChanged(const uint32_t propertiesId, const QString& fileName, QStringList& includeNames) = 0;

	virtual void AfterNameChanged(properties_item* item) = 0;
	virtual void AfterFileNameChanged(properties_item* item, QStringList& includeNames) = 0;
	virtual void AfterIncludeNameChanged(properties_item* item, QList<QPair<QString, QString>>& variables) = 0;
	
	
	//virtual void BeforeFileGroupChanged(const QString& fileName, const QString& oldFileName, bool& cancel) = 0;
	//virtual void AfterFileGroupChanged(const uint32_t propertiesId, const QString& fileGroup, QList<QPair<QString, QString>>& variables) = 0;
	//virtual void BeforeFileAdd(const QString& fileName, bool& cancel) = 0;
	//virtual void BeforeFileRemove(const QString& fileName, bool& cancel) = 0;
	//virtual void AfterFilesListChanged(const QString& fileName, const QStringList& fileNames) = 0;
	//virtual void BeforeNameChanged(const uint32_t propertiesId, const QString& name, const QString& oldName, bool& cancel) = 0;
	//virtual void AfterNameChanged(const uint32_t propertiesId, const QString& name, const QString& oldName) = 0;

	//virtual void BeforeIncludeNameChanged(const QString& fileName, const QString& includeName, const QString& oldIncludeName, bool& cancel) = 0;
	//virtual void AfterIncludeNameChanged(const QString& fileName, const QString& includeName, const QString& oldIncludeName) = 0;

	//// “ут могут мен€тьс€ значени€ не по одному, а сразу список, поэтому передаем сразу все имена
	//virtual void BeforeIncludesAdd(const QString& fileName, const QStringList& includeNames, bool& cancel) = 0;
	//virtual void BeforeIncludesRemoved(const QString& fileName, const QStringList& includeNames, bool& cancel) = 0;
	//virtual void AfterIncludesListChanged(const QString& fileName, const QStringList& includeNames) = 0;
	//virtual void AfterVariableChanged(const QString& fileName, const QString& includeName, const QList<QPair<QString, QString>>& variables) = 0;
};
