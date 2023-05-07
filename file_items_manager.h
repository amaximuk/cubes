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
	QList<QSharedPointer<file_item>> file_items_;

public:
	file_items_manager()
	{
	}

public:
	QSharedPointer<file_item> Create(QString name, QColor color)
	{
		QSharedPointer<file_item> fi(new file_item(this));
		fi->SetName(name);
		fi->SetColor(color);
		file_items_.push_back(fi);
		UpdateFileNameRegExp();
		return fi;
	}

	QStringList GetFileNames()
	{
		QStringList fileNames;
		for (auto& file : file_items_)
			fileNames.push_back(file->GetName());
		return fileNames;
	}

	QColor GetFileColor(const QString& fileName)
	{
		for (auto& fi : file_items_)
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
		for (auto& fi : file_items_)
		{
			if (fi->GetName() == fileName)
				fileIncludeNames.append(fi->GetIncludeNames());
		}
		return fileIncludeNames;
	}

	void SetFilePropertyExpanded(const QString& fileName, const QtProperty* property, bool is_expanded)
	{
		QStringList fileIncludeNames;
		for (auto& fi : file_items_)
		{
			if (fi->GetName() == fileName)
				fi->ExpandedChanged(property, is_expanded);
		}
	}

	void ApplyFileToBrowser(const QString& fileName, QtTreePropertyBrowser* propertyEditor)
	{
		for (auto& fi : file_items_)
		{
			if (fi->GetName() == fileName)
				fi->ApplyToBrowser(propertyEditor);
		}
	}

	void Clear()
	{
		file_items_.clear();
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
		for (auto& fi : file_items_)
		{
			if (file_items_.size() > 1)
			{
				QString regexp("^(?!");
				bool insert = false;
				for (int i = 0; i < file_items_.size(); ++i)
				{
					if (fi->GetName() != file_items_[i]->GetName())
					{
						if (insert)
							regexp += "$|";
						else
							insert = true;
						regexp += file_items_[i]->GetName();
					}
				}
				regexp += "$)(.+)";
				fi->SetNameRegExp(regexp);
			}
			else
				fi->SetNameRegExp("*");
		}
	}
};
