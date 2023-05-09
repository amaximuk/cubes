#pragma once

#include <QList>
#include <QSharedPointer>
#include <QString>
#include <QColor>
#include <QComboBox>
#include <QAction>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QToolButton>
#include <QLabel>
#include "file_items_manager_api.h"
#include "file_item.h"

class file_items_manager : public QObject, file_items_manager_interface
{
	Q_OBJECT

private:
	QVector<QColor> defaultColorsFile_ = { QColor("Red"), QColor("Green"), QColor("Blue"), QColor("Orange"), QColor("Violet"), QColor("Yellow") };
	int defaultColorFileIndex_;

private:
	QPointer<QWidget> widget_;
	QPointer<properties_editor> editor_;
	QPointer<QComboBox> selector_;
	QList<QSharedPointer<file_item>> items_;
	QString selected_;

public:
	file_items_manager()
	{
		defaultColorFileIndex_ = 0;
		for (auto& c : defaultColorsFile_)
			c.setAlpha(0x20);

		widget_ = CreateEditorWidget();
	}

public:
	properties_editor* GetEditor()
	{
		return editor_;
	}

	QComboBox* GetSelector()
	{
		return selector_;
	}

	QWidget* GetWidget()
	{
		return widget_;
	}

	QString GetCurrentFileName()
	{
		if (selector_->count() > 0)
			return selector_->currentText();
		else
			return "";
	}

	void Create(const QString& fileName)
	{
		const QColor color = defaultColorFileIndex_ < defaultColorsFile_.size() ?
			defaultColorsFile_[defaultColorFileIndex_++] : QColor("White");

		QSharedPointer<file_item> fi(new file_item(this, editor_));
		fi->SetName(fileName);
		fi->SetColor(color);
		items_.push_back(fi);
		selector_->addItem(fileName);
		selector_->setCurrentIndex(selector_->count() - 1);
	}

	void Select(const QString& fileName)
	{
		QString currentFileName = GetCurrentFileName();
		if (selected_ != "" && selected_ != fileName)
			GetItem(selected_)->UnSelect();
		if (selected_ != fileName)
		{
			GetItem(fileName)->Select();
			selected_ = fileName;
		}
	}

	QSharedPointer<file_item> GetItem(const QString& fileName)
	{
		for (auto& file : items_)
		{
			if (file->GetName() == fileName)
				return file;
		}
		return nullptr;
	}

	QStringList GetFileNames()
	{
		QStringList fileNames;
		for (auto& file : items_)
			fileNames.push_back(file->GetName());
		return fileNames;
	}

	QColor GetFileColor(const QString& fileName)
	{
		for (auto& fi : items_)
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
		for (auto& fi : items_)
		{
			if (fi->GetName() == fileName)
				fileIncludeNames.append(fi->GetIncludeNames());
		}
		return fileIncludeNames;
	}

	void Clear()
	{
		editor_->GetPropertyEditor()->clear();
		selector_->clear();
		items_.clear();
	}

signals:
	void NameChanged(QString fileName, QString oldFileName);
	void IncludeChanged(QString fileName, QStringList includeNames);
	void IncludeNameChanged(QString fileName, QString includeName, QString oldIncludeName);

public:
	void InformNameChanged(file_item* fileItem, QString fileName, QString oldFileName) override
	{
		int count = 0;
		for (const auto& i : items_)
		{
			if (i->GetName() == fileName)
				count++;
		}
		if (count > 1)
		{
			QMessageBox::critical(widget_, "Error", QString::fromLocal8Bit("Имя уже используется. Дубликаты не допускаются!"));
			fileItem->SetName(oldFileName, true, oldFileName);
		}
		else
		{
			for (int i = 0; i < selector_->count(); ++i)
			{
			    if (selector_->itemText(i) == oldFileName)
					selector_->setItemText(i, fileName);
			}
			emit NameChanged(fileName, oldFileName);
		}
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
	void OnEditorCollapsed(QtBrowserItem* item)
	{
		QString currentFileName = GetCurrentFileName();
		SetFilePropertyExpanded(currentFileName, item->property(), false);
	}

	void OnEditorExpanded(QtBrowserItem* item)
	{
		QString currentFileName = GetCurrentFileName();
		SetFilePropertyExpanded(currentFileName, item->property(), true);
	}

	void OnContextMenuRequested(const QPoint& pos)
	{
		auto pe = editor_->GetPropertyEditor();
		if (pe->currentItem() == nullptr)
			return;
		if (pe->currentItem()->parent() == nullptr)
			return;

		QString name = pe->currentItem()->property()->propertyName();
		QString parentName = pe->currentItem()->parent()->property()->propertyName();
		if (parentName == QString::fromLocal8Bit("Включаемые файлы"))
		{
			QMenu contextMenu("Context menu");

			QAction action1(QString::fromLocal8Bit("Удалить %1").arg(name));
			connect(&action1, &QAction::triggered, this, &file_items_manager::OnDeleteInclude);
			contextMenu.addAction(&action1);

			contextMenu.exec(pe->mapToGlobal(pos));
		}
	}

	void OnDeleteInclude(bool checked = false)
	{

	}

	void OnSelectorIndexChanged(int index)
	{
		QString currentFileName = GetCurrentFileName();
		if (currentFileName != "")
			Select(currentFileName);
	}

	void OnAddFileClicked()
	{
		bool ok;
		QString fileName = QInputDialog::getText(widget_, QString::fromLocal8Bit("Добавление файла"), QString::fromLocal8Bit("Имя файла:"), QLineEdit::Normal, "", &ok);
		if (!ok || fileName.isEmpty())
			return;

		int count = 0;
		for (const auto& i : items_)
		{
			if (i->GetName() == fileName)
				count++;
		}
		if (count > 0)
		{
			QMessageBox::critical(widget_, "Error", QString::fromLocal8Bit("Имя уже используется. Дубликаты не допускаются!"));
			return;
		}

		QColor fileColor = defaultColorFileIndex_ < defaultColorsFile_.size() ?
			defaultColorsFile_[defaultColorFileIndex_++] : QColor("White");
		Create(fileName);
		Select(fileName);

		//for (auto& item : panes_[0].first->items())
		//{
		//	diagram_item* di = reinterpret_cast<diagram_item*>(item);
		//	QStringList fileNames = file_items_manager_->GetFileNames();
		//	di->getProperties()->SetFileNames(fileNames);
		//}

		//if (panes_[0].first->selectedItems().size() > 0)
		//	reinterpret_cast<diagram_item*>(panes_[0].first->selectedItems()[0])->getProperties()->ApplyToBrowser(propertyEditor_);

		//panes_[0].first->invalidate();

	}

	void OnRemoveFileClicked()
	{
		//QMessageBox::StandardButton resBtn = QMessageBox::question(widget_, "parameters_composer",
		//	QString::fromLocal8Bit("Вы действительно хотите выйти?\nВсе несохраненные изменения будут потеряны!"), QMessageBox::No | QMessageBox::Yes);
		//if (resBtn == QMessageBox::Yes)
		//	QApplication::quit();
	}

private:
	QWidget* CreateEditorWidget()
	{
		editor_ = new properties_editor();
		qDebug() << connect(editor_, &properties_editor::Collapsed, this, &file_items_manager::OnEditorCollapsed);
		qDebug() << connect(editor_, &properties_editor::Expanded, this, &file_items_manager::OnEditorExpanded);
		qDebug() << connect(editor_, &properties_editor::ContextMenuRequested, this, &file_items_manager::OnContextMenuRequested);

		QWidget* propertiesPanelWidget = new QWidget;

		QWidget* hostsButtonsWidget = CreateSelectorWidget();

		QVBoxLayout* propertiesPaneLayout = new QVBoxLayout;
		propertiesPaneLayout->addWidget(hostsButtonsWidget);
		propertiesPaneLayout->addWidget(editor_->GetPropertyEditor());
		propertiesPaneLayout->setContentsMargins(0, 0, 0, 0);

		propertiesPanelWidget->setLayout(propertiesPaneLayout);

		return propertiesPanelWidget;
	}

	QWidget* CreateSelectorWidget()
	{
		selector_ = new QComboBox();
		qDebug() << connect(selector_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &file_items_manager::OnSelectorIndexChanged);

		QHBoxLayout* hBoxLayoutPropertyListButtons = new QHBoxLayout;
		hBoxLayoutPropertyListButtons->setMargin(0);
		hBoxLayoutPropertyListButtons->setContentsMargins(0, 0, 0, 0);

		QToolButton* toolButtonPropertyListAdd = new QToolButton;
		toolButtonPropertyListAdd->setFixedSize(24, 24);
		toolButtonPropertyListAdd->setIconSize(QSize(24, 24));
		toolButtonPropertyListAdd->setIcon(QIcon(":/images/plus.png"));
		toolButtonPropertyListAdd->setToolTip(QString::fromLocal8Bit("Добавить хост"));
		hBoxLayoutPropertyListButtons->addWidget(toolButtonPropertyListAdd);
		connect(toolButtonPropertyListAdd, &QToolButton::clicked, this, &file_items_manager::OnAddFileClicked);

		QToolButton* toolButtonPropertyListRemove = new QToolButton;
		toolButtonPropertyListRemove->setFixedSize(24, 24);
		toolButtonPropertyListRemove->setIconSize(QSize(24, 24));
		toolButtonPropertyListRemove->setIcon(QIcon(":/images/minus.png"));
		toolButtonPropertyListRemove->setToolTip(QString::fromLocal8Bit("Удалить хост"));
		hBoxLayoutPropertyListButtons->addWidget(toolButtonPropertyListRemove);
		connect(toolButtonPropertyListRemove, &QToolButton::clicked, this, &file_items_manager::OnRemoveFileClicked);

		QWidget* buttonsWidget = new QWidget;
		buttonsWidget->setLayout(hBoxLayoutPropertyListButtons);

		QLabel* label = new QLabel;
		label->setText(QString::fromLocal8Bit("Файлы:"));
		//label->setStyleSheet("font-weight: bold; font-size: 14px");

		QHBoxLayout* headerLayout = new QHBoxLayout;
		headerLayout->addWidget(label, 0);
		headerLayout->addWidget(selector_, 1);
		headerLayout->addWidget(buttonsWidget, 0);
		headerLayout->setContentsMargins(0, 0, 0, 0);

		QWidget* mainWidget = new QWidget;
		mainWidget->setLayout(headerLayout);
		return mainWidget;
	}

	void SetFilePropertyExpanded(const QString& fileName, const QtProperty* property, bool is_expanded)
	{
		QStringList fileIncludeNames;
		for (auto& fi : items_)
		{
			if (fi->GetName() == fileName)
				fi->ExpandedChanged(property, is_expanded);
		}
	}
};
