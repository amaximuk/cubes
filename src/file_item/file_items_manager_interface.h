#pragma once

#include <QString>
#include <QList>
#include <QList>

namespace FileItem
{
	class IFileItemsManager
	{
	public:
		// ��� �������� �������� �� ������
		virtual void BeforeFileNameChanged(const QString& fileName, const QString& oldFileName, bool& cancel) = 0;
		virtual void AfterFileNameChanged(const QString& fileName, const QString& oldFileName) = 0;

		//virtual void BeforeFileAdd(const QString& fileName, bool& cancel) = 0;
		//virtual void BeforeFileRemove(const QString& fileName, bool& cancel) = 0;
		//virtual void AfterFilesListChanged(const QString& fileName, const QStringList& fileNames) = 0;

		virtual void BeforeIncludeNameChanged(const QString& fileName, const QString& includeName, const QString& oldIncludeName, bool& cancel) = 0;
		virtual void AfterIncludeNameChanged(const QString& fileName, const QString& includeName, const QString& oldIncludeName) = 0;

		// ��� ����� �������� �������� �� �� ������, � ����� ������, ������� �������� ����� ��� �����
		virtual void BeforeIncludesAdd(const QString& fileName, const QStringList& includeNames, bool& cancel) = 0;
		virtual void BeforeIncludesRemoved(const QString& fileName, const QStringList& includeNames, bool& cancel) = 0;
		virtual void AfterIncludesListChanged(const QString& fileName, const QStringList& includeNames) = 0;

		virtual void AfterVariableChanged(const QString& fileName, const QString& includeName, const QList<QPair<QString, QString>>& variables) = 0;
	};
}
