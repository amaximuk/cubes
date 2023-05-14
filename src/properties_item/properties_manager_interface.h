#pragma once

#include <QString>

class properties_manager_interface
{
public:
	// “ут значени€ мен€ютс€ по одному
	virtual void BeforeFileNameChanged(const QString& fileName, const QString& oldFileName, bool& cancel) = 0;
	virtual void AfterFileNameChanged(const QString& fileName, const QString& oldFileName) = 0;

	//virtual void BeforeFileAdd(const QString& fileName, bool& cancel) = 0;
	//virtual void BeforeFileRemove(const QString& fileName, bool& cancel) = 0;
	//virtual void AfterFilesListChanged(const QString& fileName, const QStringList& fileNames) = 0;

	virtual void BeforeIncludeNameChanged(const QString& fileName, const QString& includeName, const QString& oldIncludeName, bool& cancel) = 0;
	virtual void AfterIncludeNameChanged(const QString& fileName, const QString& includeName, const QString& oldIncludeName) = 0;

	// “ут могут мен€тьс€ значени€ не по одному, а сразу список, поэтому передаем сразу все имена
	virtual void BeforeIncludesAdd(const QString& fileName, const QStringList& includeNames, bool& cancel) = 0;
	virtual void BeforeIncludesRemoved(const QString& fileName, const QStringList& includeNames, bool& cancel) = 0;
	virtual void AfterIncludesListChanged(const QString& fileName, const QStringList& includeNames) = 0;
};
