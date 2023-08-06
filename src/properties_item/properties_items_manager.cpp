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
#include "properties_items_manager.h"

using namespace PropertiesItem;

properties_items_manager::properties_items_manager(top_manager_interface* top_manager)
{
	top_manager_ = top_manager;
	unique_number_ = 0;
	selected_ = 0;

	defaultColorFileIndex_ = 0;
	for (auto& c : defaultColorsFile_)
		c.setAlpha(0x20);

	widget_ = CreateEditorWidget();
}

properties_editor* properties_items_manager::GetEditor()
{
	return editor_;
}

QComboBox* properties_items_manager::GetSelector()
{
	return selector_;
}

QWidget* properties_items_manager::GetWidget()
{
	return widget_;
}

uint32_t properties_items_manager::GetCurrentPropertiesId()
{
	if (selector_->count() > 0)
		return selector_->itemData(selector_->currentIndex()).toUInt();
	else
		return 0;
}

void properties_items_manager::Create(const QString& unitId, uint32_t& propertiesId)
{
	const QColor color = defaultColorFileIndex_ < defaultColorsFile_.size() ?
		defaultColorsFile_[defaultColorFileIndex_++] : QColor("White");

	unit_types::UnitParameters unitParameters{};
	top_manager_->GetUnitParameters(unitId, unitParameters);

	propertiesId = ++unique_number_;
	QSharedPointer<properties_item> pi(new properties_item(this, editor_, unitParameters, propertiesId));

	QString propertiesName = QString::fromStdString(unitParameters.fileInfo.info.id);

	pi->SetName(propertiesName);
	items_[propertiesId] = pi;
	selector_->addItem(propertiesName, propertiesId);
	selector_->setCurrentIndex(selector_->count() - 1);
	//selector_->addItem(propertiesName);
	//selector_->setCurrentIndex(selector_->count() - 1);
	//selector_->setItemData(selector_->count() - 1, propertiesId);

	//emit FilesListChanged(GetFileNames());
}

void properties_items_manager::Select(const uint32_t& propertiesId)
{
	//QString currentFileName = GetCurrentFileName();
	if (selected_ != propertiesId)
	{
		if (selected_ != 0)
		{
			GetItem(selected_)->UnSelect();
			selected_ = 0;
			auto pe = editor_->GetPropertyEditor();
			pe->clear();
		}
		if (propertiesId != 0)
		{
			GetItem(propertiesId)->Select();
			selected_ = propertiesId;
		}

		int index = selector_->findData(propertiesId);
		if (index != -1)
			selector_->setCurrentIndex(index);
	}
}

QSharedPointer<properties_item> properties_items_manager::GetItem(const uint32_t propertiesId)
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

//QStringList GetFileNames()
//{
//	QStringList fileNames;
//	for (auto& file : items_)
//		fileNames.push_back(file->GetName());
//	return fileNames;
//}

QColor properties_items_manager::GetFileColor(const QString& fileName)
{
	for (auto& fi : items_)
	{
		//if (fi->GetName() == fileName)
		//	return fi->GetColor();
	}
	return QColor("Black");
}

QStringList properties_items_manager::GetFileIncludeNames(const QString& fileName)
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

QList<QPair<QString, QString>> properties_items_manager::GetFileIncludeVariables(const QString& fileName, const QString& includeName)
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

void properties_items_manager::Clear()
{
	editor_->GetPropertyEditor()->clear();
	selector_->clear();
	items_.clear();
}

void properties_items_manager::AfterNameChanged(properties_item* item)
{
	auto name = GetName(item);
	int index = selector_->findData(item->GetPropertiesId());
	if (index != -1)
		selector_->setItemText(index, name);

	auto fileName = item->GetFileName();
	auto groupName = item->GetGroupName();

	emit BasePropertiesChanged(item->GetPropertiesId(), name, fileName, groupName);
}

void properties_items_manager::AfterFileNameChanged(properties_item* item, QStringList& includeNames)
{
	auto name = GetName(item);
	int index = selector_->findData(item->GetPropertiesId());
	if (index != -1)
		selector_->setItemText(index, name);

	// Заполняем группы
	top_manager_->GetFileIncludeList(item->GetFileName(), includeNames);

	auto fileName = item->GetFileName();
	auto groupName = item->GetGroupName();

	emit BasePropertiesChanged(item->GetPropertiesId(), name, fileName, groupName);
}

void properties_items_manager::AfterIncludeNameChanged(properties_item* item, QList<QPair<QString, QString>>& variables)
{
	auto name = GetName(item);
	int index = selector_->findData(item->GetPropertiesId());
	if (index != -1)
		selector_->setItemText(index, name);

	// Заполняем переменные
	top_manager_->GetFileIncludeVariableList(item->GetFileName(), item->GetGroupName(), variables);

	auto fileName = item->GetFileName();
	auto groupName = item->GetGroupName();

	emit BasePropertiesChanged(item->GetPropertiesId(), name, fileName, groupName);
}

void properties_items_manager::OnEditorCollapsed(QtBrowserItem* item)
{
	QString currentFileName = GetCurrentPropertiesId();
	SetFilePropertyExpanded(currentFileName, item->property(), false);
}

void properties_items_manager::OnEditorExpanded(QtBrowserItem* item)
{
	QString currentFileName = GetCurrentPropertiesId();
	SetFilePropertyExpanded(currentFileName, item->property(), true);
}

void properties_items_manager::OnContextMenuRequested(const QPoint& pos)
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

void properties_items_manager::OnDeleteInclude(bool checked)
{

}

void properties_items_manager::OnSelectorIndexChanged(int index)
{
	uint32_t currentId = GetCurrentPropertiesId();
	Select(currentId);
	emit SelectedItemChanged(currentId);
}

void properties_items_manager::OnAddFileClicked()
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

void properties_items_manager::OnRemoveFileClicked()
{
	//if (selected_ == 0)
	//{
	//	QMessageBox::critical(widget_, "Error", QString::fromLocal8Bit("Файл не выбран!"));
	//	return;
	//}

	//// Проверяем возможность удаления
	//QStringList unitNames;
	//top_manager_->GetUnitsInFileList(selected_, unitNames);
	//if (unitNames.count() > 0)
	//{
	//	QString text = QString::fromLocal8Bit("Имя используется.\nУдаление невозможно!\nЮниты:\n");
	//	text.append(unitNames.join('\n'));
	//	QMessageBox::critical(widget_, "Error", text);
	//	return;
	//}

	//// Сохраняем копию, после удаления selected_ изменится
	//QString selected = selected_;

	//// Удаляем из селектора, автоматически происходит UnSelect
	//selector_->removeItem(selector_->findText(selected_));

	//// Получаем все имена, заодно запоминаем элемент для удаления
	//QStringList fileNames;
	//QSharedPointer<properties_item> toRemove;
	//for (const auto& item : items_)
	//{
	//	QString name = item->GetName();
	//	if (name == selected)
	//		toRemove = item;
	//	else
	//		fileNames.push_back(item->GetName());
	//}

	//// Удаляем из списка
	////items_.removeAll(toRemove);

	//// Если это был последний
	//if (items_.count() == 0)
	//	editor_->GetPropertyEditor()->clear();

	//// Сообщаяем об удалении
	//emit FilesListChanged(fileNames);
}

QWidget* properties_items_manager::CreateEditorWidget()
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

QWidget* properties_items_manager::CreateSelectorWidget()
{
	selector_ = new QComboBox();
	selector_->addItem("<not selected>", 0);
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

void properties_items_manager::SetFilePropertyExpanded(const QString& fileName, const QtProperty* property, bool is_expanded)
{
	QStringList fileIncludeNames;
	for (auto& fi : items_)
	{
		//if (fi->GetName() == fileName)
		//	fi->ExpandedChanged(property, is_expanded);
	}
}

QString properties_items_manager::GetName(properties_item* item)
{
	QList<QPair<QString, QString>> variables;
	top_manager_->GetFileIncludeVariableList(item->GetFileName(), item->GetGroupName(), variables);

	QString name = item->GetName();
	for (const auto& v : variables)
	{
		QString replace = QString("@%1@").arg(v.first);
		name.replace(replace, v.second);
	}

	return name;
}
