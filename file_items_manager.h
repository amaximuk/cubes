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
		Update();
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
	void ItemNameChanged(QString itemName, QString oldName);
	void ItemChanged(QString itemName);

public:
	void InformNameChanged(file_item* fi, QString oldName) override
	{
		Update();
		emit ItemNameChanged(fi->GetName(), oldName);
	}

	void InformIncludeChanged(file_item* fi) override
	{
		emit ItemChanged(fi->GetName());
	}

private:
	void Update()
	{
		if (file_items_.size() > 1)
		{
			for (auto& fi : file_items_)
			{
				//"(?!ignoreme|ignoreme2|ignoremeN)(.+)"
				QString regexp("(?!");
				bool insert = false;
				for (int i = 0; i < file_items_.size(); ++i)
				{
					if (fi->GetName() != file_items_[i]->GetName())
					{
						if (insert)
							regexp += "|";
						else
							insert = true;
						regexp += file_items_[i]->GetName();
					}
				}
				regexp += ")(.+)";
				fi->SetNameRegExp(regexp);
			}
		}

	}
};
