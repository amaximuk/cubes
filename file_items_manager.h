#pragma once

#include <QList>
#include <QSharedPointer>
#include <QString>
#include <QColor>
#include "file_items_manager_api.h"
#include "file_item.h"

class file_items_manager : public QObject, file_items_manager_interface
{
Q_OBJECT

private:
	QList<QSharedPointer<file_item>> files_;
	QString selectedFile_;

public:
	file_items_manager()
	{
	}

public:
	QSharedPointer<file_item> Create(const QString& fileName, const QColor& color)
	{
		QSharedPointer<file_item> fi(new file_item(this));
		fi->SetName(fileName);
		fi->SetColor(color);
		files_.push_back(fi);
		UpdateFileNameRegExp();
		return fi;
	}

	QSharedPointer<file_item> GetItem(const QString& fileName)
	{
		for (auto& file : files_)
		{
			if (file->GetName() == fileName)
				return file;
		}
		return nullptr;
	}

	QStringList GetFileNames()
	{
		QStringList fileNames;
		for (auto& file : files_)
			fileNames.push_back(file->GetName());
		return fileNames;
	}

	QColor GetFileColor(const QString& fileName)
	{
		for (auto& fi : files_)
		{
			if (fi->GetName() == fileName)
				return fi->GetColor();
		}
		return QColor("Black");
	}

	QStringList GetFileIncludeNames(const QString& fileName)
	{
		QStringList fileIncludeNames;
		fileIncludeNames.push_back("<not selected>");
		for (auto& fi : files_)
		{
			if (fi->GetName() == fileName)
				fileIncludeNames.append(fi->GetIncludeNames());
		}
		return fileIncludeNames;
	}

	void SetFilePropertyExpanded(const QString& fileName, const QtProperty* property, bool is_expanded)
	{
		QStringList fileIncludeNames;
		for (auto& fi : files_)
		{
			if (fi->GetName() == fileName)
				fi->ExpandedChanged(property, is_expanded);
		}
	}

	void ApplyFileToBrowser(const QString& fileName, QtTreePropertyBrowser* propertyEditor)
	{
		for (auto& fi : files_)
		{
			if (fi->GetName() == fileName)
				fi->ApplyToBrowser(propertyEditor);
		}
	}

	void Clear()
	{
		files_.clear();
	}

signals:
	void NameChanged(QString fileName, QString oldFileName);
	void IncludeChanged(QString fileName, QStringList includeNames);
	void IncludeNameChanged(QString fileName, QString includeName, QString oldIncludeName);

public:
	void InformNameChanged(QString fileName, QString oldFileName) override
	{
		UpdateFileNameRegExp();
		emit NameChanged(fileName, oldFileName);
	}

	void InformIncludeChanged(QString fileName, QStringList includeNames) override
	{
		QStringList fileIncludeNames;
		fileIncludeNames.push_back("<not selected>");
		fileIncludeNames.append(includeNames);
		emit IncludeChanged(fileName, fileIncludeNames);
	}

	void InformIncludeNameChanged(QString fileName, QString includeName, QString oldIncludeName) override
	{
		emit IncludeNameChanged(fileName, includeName, oldIncludeName);
	}

private:
	void UpdateFileNameRegExp()
	{
		for (int i = 0; i < files_.size(); ++i)
		{
			if (files_.size() > 1)
			{
				QString regexp("^(?!");
				bool insert = false;
				for (int j = 0; j < files_.size(); ++j)
				{
					if (i != j)
					{
						if (insert)
							regexp += "$|";
						else
							insert = true;
						regexp += files_[j]->GetName();
					}
				}
				regexp += "$)(.+)";
				files_[i]->SetNameRegExp(regexp);
			}
			else
				files_[i]->SetNameRegExp("*");
		}
	}
};
