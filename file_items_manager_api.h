#pragma once

class file_item;

enum class Operation
{
	Add,
	Remove
};

class file_items_manager_interface
{
public:
	virtual void BeforeFileNameChanged(const QString& fileName, const QString& oldFileName, bool& cancel) = 0;
	virtual void AfterFileNameChanged(const QString& fileName, const QString& oldFileName) = 0;
	virtual void BeforeFilesListChanged(const QString& fileName, Operation operation, bool& cancel) = 0;
	virtual void AfterFilesListChanged(const QString& fileName, Operation operation, const QStringList& fileNames) = 0;
	virtual void BeforeIncludeNameChanged(const QString& fileName, const QString& includeName,
		const QString& oldIncludeName, bool& cancel) = 0;
	virtual void AfterIncludeNameChanged(const QString& fileName, const QString& includeName,
		const QString& oldIncludeName) = 0;
	virtual void BeforeIncludesListChanged(const QString& fileName, const QString& includeName,
		Operation operation, bool& cancel) = 0;
	virtual void AfterIncludesListChanged(const QString& fileName, const QString& includeName,
		Operation operation, const QStringList& includeNames) = 0;
};
