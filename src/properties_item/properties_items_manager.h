#pragma once

#include <QMap>
#include <QSet>
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
#include "../top_manager_interface.h"
#include "../unit_types.h"
#include "../xml_parser.h"
#include "properties_items_manager_interface.h"
#include "properties_item.h"

class properties_items_manager : public QObject, properties_items_manager_interface
{
	Q_OBJECT

private:
	QVector<QColor> defaultColorsFile_ = { QColor("Red"), QColor("Green"), QColor("Blue"), QColor("Orange"), QColor("Violet"), QColor("Yellow") };
	int defaultColorFileIndex_;

private:
	top_manager_interface* top_manager_;
	QPointer<QWidget> widget_;
	QPointer<properties_editor> editor_;
	QPointer<QComboBox> selector_;
	QMap<uint32_t, QSharedPointer<properties_item>> items_;
	//QList<QSharedPointer<properties_item>> items_;
	QString selected_;
	uint32_t unique_number_;

public:
	properties_items_manager(top_manager_interface* top_manager)
	{
		top_manager_ = top_manager;
		unique_number_ = 0;

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

	void Create(const QString& unitId, uint32_t& propertiesId)
	{
		const QColor color = defaultColorFileIndex_ < defaultColorsFile_.size() ?
			defaultColorsFile_[defaultColorFileIndex_++] : QColor("White");

		unit_types::UnitParameters unitParameters{};
		top_manager_->GetUnitParameters(unitId, unitParameters);

		propertiesId = unique_number_++;
		QSharedPointer<properties_item> pi(new properties_item(this, editor_, unitParameters, propertiesId));

		QString propertiesName = "XXXXXXXXXXXX";

		pi->SetName(propertiesName);
		items_[propertiesId] = pi;
		selector_->addItem(propertiesName);
		selector_->setCurrentIndex(selector_->count() - 1);

		emit FilesListChanged(GetFileNames());
	}

	void Select(const QString& propertiesName)
	{
		QString currentFileName = GetCurrentFileName();
		if (selected_ != propertiesName)
		{
			if (!selected_.isEmpty())
			{
				//GetItem(selected_)->UnSelect();
				selected_ = "";
			}
			if (!propertiesName.isEmpty())
			{
				//GetItem(fileName)->Select();
				selected_ = propertiesName;
			}
		}
	}

	QSharedPointer<properties_item> GetItem(const uint32_t propertiesId)
	{
		//for (auto& properties : items_)
		//{
		//	if (properties->GetName() == propertiesName)
		//		return properties;
		//}
		if (items_.contains(propertiesId))
			return items_[propertiesId];
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
			//if (fi->GetName() == fileName)
			//	return fi->GetColor();
		}
		return QColor("Black");
	}

	QStringList GetFileIncludeNames(const QString& fileName)
	{
		QStringList fileIncludeNames;
		fileIncludeNames.push_back("<not selected>");
		for (auto& fi : items_)
		{
			//if (fi->GetName() == fileName)
			//	fileIncludeNames.append(fi->GetIncludeNames());
		}
		return fileIncludeNames;
	}

	QList<QPair<QString, QString>> GetFileIncludeVariables(const QString& fileName, const QString& includeName)
	{
		QList<QPair<QString, QString>> result;

		QStringList fileIncludeNames;
		for (auto& fi : items_)
		{
			if (fi->GetName() == fileName)
			{
				//result = fi->GetIncludeVariables(includeName);
				//break;
			}
		}

		return result;
	}

	void Clear()
	{
		editor_->GetPropertyEditor()->clear();
		selector_->clear();
		items_.clear();
	}

signals:
	void FileNameChanged(const QString& fileName, const QString& oldFileName);
	void FilesListChanged(const QStringList& fileNames);
	void IncludeNameChanged(const QString& fileName, const QString& includeName, const QString& oldIncludeName);
	void IncludesListChanged(const QString& fileName, const QStringList& includeNames);
	void VariableChanged(const QString& fileName, const QString& includeName, const QList<QPair<QString, QString>>& variables);

public:
	// properties_items_manager_interface

	void BeforeFileNameChanged(const QString& fileName, const QString& oldFileName, bool& cancel) override
	{
		int count = 0;
		for (const auto& i : items_)
		{
			if (i->GetName() == fileName)
				count++;
		}
		if (count > 0)
		{
			QMessageBox::critical(widget_, "Error", QString::fromLocal8Bit("Имя уже используется. Дубликаты не допускаются!"));
			cancel = true;
		}
		else
			cancel = false;
	}

	void AfterFileNameChanged(const QString& fileName, const QString& oldFileName) override
	{
		// Переименовываем в comboBox
		for (int i = 0; i < selector_->count(); ++i)
		{
			if (selector_->itemText(i) == oldFileName)
			{
				selector_->setItemText(i, fileName);
				break;
			}
		}
		
		// Переименовываем имя выбранного файла
		// Проверка selected_ == oldFileName избыточна, на всякий случай оставлю
		if (selected_ == oldFileName)
			selected_ = fileName;

		// Уведомляем о переименовании
		emit FileNameChanged(fileName, oldFileName);
	}

	//void BeforeFileAdd(const QString& fileName, bool& cancel) override
	//{
	//	// Ничего не делаем
	//	cancel = false;
	//}

	//void BeforeFileRemove(const QString& fileName, bool& cancel) override
	//{
	//	QStringList unitNames;
	//	top_manager_->GetUnitsInFileList(fileName, unitNames);
	//	if (unitNames.count() > 0)
	//	{
	//		QString text = QString::fromLocal8Bit("Имя используется.\nУдаление невозможно!\nЮниты:\n");
	//		text.append(unitNames.join('\n'));
	//		QMessageBox::critical(widget_, "Error", text);
	//		cancel = true;
	//	}
	//	else
	//		cancel = false;
	//}

	//void AfterFilesListChanged(const QString& fileName, const QStringList& fileNames) override
	//{
	//	emit FilesListChanged(fileName, fileNames);
	//}

	void BeforeIncludeNameChanged(const QString& fileName, const QString& includeName, const QString& oldIncludeName, bool& cancel) override
	{
		// Ничего не делаем
		cancel = false;
	}

	void AfterIncludeNameChanged(const QString& fileName, const QString& includeName, const QString& oldIncludeName) override
	{
		emit IncludeNameChanged(fileName, includeName, oldIncludeName);
	}

	void BeforeIncludesAdd(const QString& fileName, const QStringList& includeNames, bool& cancel) override
	{
		// Ничего не делаем
		cancel = false;
	}

	void BeforeIncludesRemoved(const QString& fileName, const QStringList& includeNames, bool& cancel) override
	{
		QSet<QString> allUnitNames;
		for (const auto& includeName : includeNames)
		{
			QStringList unitNames;
			top_manager_->GetUnitsInFileIncludeList(fileName, includeName, unitNames);
			allUnitNames.unite(QSet<QString>(unitNames.begin(), unitNames.end()));
		}
		if (allUnitNames.count() > 0)
		{
			QString text = QString::fromLocal8Bit("Одно или несколько из удаляемых имен используется.\nУдаление невозможно!\nЮниты:\n");
			text.append(allUnitNames.values().join('\n'));
			QMessageBox::critical(widget_, "Error", text);
			cancel = true;
		}
		else
			cancel = false;
	}
	
	void AfterIncludesListChanged(const QString& fileName, const QStringList& includeNames) override
	{
		QStringList fileIncludeNames;
		fileIncludeNames.push_back("<not selected>");
		fileIncludeNames.append(includeNames);
		emit IncludesListChanged(fileName, fileIncludeNames);
	}

	void AfterVariableChanged(const QString& fileName, const QString& includeName, const QList<QPair<QString, QString>>& variables) override
	{
		emit VariableChanged(fileName, includeName, variables);
	}

	//void InformNameChanged(properties_item* fileItem, QString fileName, QString oldFileName) override
	//{
	//	int count = 0;
	//	for (const auto& i : items_)
	//	{
	//		if (i->GetName() == fileName)
	//			count++;
	//	}
	//	if (count > 1)
	//	{
	//		QMessageBox::critical(widget_, "Error", QString::fromLocal8Bit("Имя уже используется. Дубликаты не допускаются!"));
	//		fileItem->SetName(oldFileName, true, oldFileName);
	//	}
	//	else
	//	{
	//		for (int i = 0; i < selector_->count(); ++i)
	//		{
	//		    if (selector_->itemText(i) == oldFileName)
	//				selector_->setItemText(i, fileName);
	//		}
	//		emit NameChanged(fileName, oldFileName);
	//	}
	//}

	//void InformIncludeChanged(QString fileName, QStringList includeNames) override
	//{
	//	QStringList fileIncludeNames;
	//	fileIncludeNames.push_back("<not selected>");
	//	fileIncludeNames.append(includeNames);
	//	emit IncludeChanged(fileName, fileIncludeNames);
	//}

	//void InformIncludeNameChanged(QString fileName, QString includeName, QString oldIncludeName) override
	//{
	//	emit IncludeNameChanged(fileName, includeName, oldIncludeName);
	//}

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
			connect(&action1, &QAction::triggered, this, &properties_items_manager::OnDeleteInclude);
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

		uint32_t propertiesId{ 0 };
		Create(fileName, propertiesId);
		//Select(fileName);

		//for (auto& item : panes_[0].first->items())
		//{
		//	diagram_item* di = reinterpret_cast<diagram_item*>(item);
		//	QStringList fileNames = properties_items_manager_->GetFileNames();
		//	di->getProperties()->SetFileNames(fileNames);
		//}

		//if (panes_[0].first->selectedItems().size() > 0)
		//	reinterpret_cast<diagram_item*>(panes_[0].first->selectedItems()[0])->getProperties()->ApplyToBrowser(propertyEditor_);

		//panes_[0].first->invalidate();

	}

	void OnRemoveFileClicked()
	{
		if (selected_.isEmpty())
		{
			QMessageBox::critical(widget_, "Error", QString::fromLocal8Bit("Файл не выбран!"));
			return;
		}

		// Проверяем возможность удаления
		QStringList unitNames;
		top_manager_->GetUnitsInFileList(selected_, unitNames);
		if (unitNames.count() > 0)
		{
			QString text = QString::fromLocal8Bit("Имя используется.\nУдаление невозможно!\nЮниты:\n");
			text.append(unitNames.join('\n'));
			QMessageBox::critical(widget_, "Error", text);
			return;
		}

		// Сохраняем копию, после удаления selected_ изменится
		QString selected = selected_;

		// Удаляем из селектора, автоматически происходит UnSelect
		selector_->removeItem(selector_->findText(selected_));

		// Получаем все имена, заодно запоминаем элемент для удаления
		QStringList fileNames;
		QSharedPointer<properties_item> toRemove;
		for (const auto& item : items_)
		{
			QString name = item->GetName();
			if (name == selected)
				toRemove = item;
			else
				fileNames.push_back(item->GetName());
		}

		// Удаляем из списка
		//items_.removeAll(toRemove);

		// Если это был последний
		if (items_.count() == 0)
			editor_->GetPropertyEditor()->clear();

		// Сообщаяем об удалении
		emit FilesListChanged(fileNames);
	}

private:
	QWidget* CreateEditorWidget()
	{
		editor_ = new properties_editor();
		qDebug() << connect(editor_, &properties_editor::Collapsed, this, &properties_items_manager::OnEditorCollapsed);
		qDebug() << connect(editor_, &properties_editor::Expanded, this, &properties_items_manager::OnEditorExpanded);
		qDebug() << connect(editor_, &properties_editor::ContextMenuRequested, this, &properties_items_manager::OnContextMenuRequested);

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
		qDebug() << connect(selector_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &properties_items_manager::OnSelectorIndexChanged);

		QHBoxLayout* hBoxLayoutPropertyListButtons = new QHBoxLayout;
		hBoxLayoutPropertyListButtons->setMargin(0);
		hBoxLayoutPropertyListButtons->setContentsMargins(0, 0, 0, 0);

		QToolButton* toolButtonPropertyListAdd = new QToolButton;
		toolButtonPropertyListAdd->setFixedSize(24, 24);
		toolButtonPropertyListAdd->setIconSize(QSize(24, 24));
		toolButtonPropertyListAdd->setIcon(QIcon(":/images/plus.png"));
		toolButtonPropertyListAdd->setToolTip(QString::fromLocal8Bit("Добавить хост"));
		hBoxLayoutPropertyListButtons->addWidget(toolButtonPropertyListAdd);
		connect(toolButtonPropertyListAdd, &QToolButton::clicked, this, &properties_items_manager::OnAddFileClicked);

		QToolButton* toolButtonPropertyListRemove = new QToolButton;
		toolButtonPropertyListRemove->setFixedSize(24, 24);
		toolButtonPropertyListRemove->setIconSize(QSize(24, 24));
		toolButtonPropertyListRemove->setIcon(QIcon(":/images/minus.png"));
		toolButtonPropertyListRemove->setToolTip(QString::fromLocal8Bit("Удалить хост"));
		hBoxLayoutPropertyListButtons->addWidget(toolButtonPropertyListRemove);
		connect(toolButtonPropertyListRemove, &QToolButton::clicked, this, &properties_items_manager::OnRemoveFileClicked);

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
			//if (fi->GetName() == fileName)
			//	fi->ExpandedChanged(property, is_expanded);
		}
	}
};
