#include <QComboBox>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QToolButton>
#include <QLabel>
#include "qttreepropertybrowser.h"
#include "../top_manager_interface.h"
#include "../unit_types.h"
#include "../xml_parser.h"
#include "properties_item.h"
#include "properties_items_manager.h"

using namespace CubesProperties;

PropertiesItemsManager::PropertiesItemsManager(ITopManager* topManager)
{
	topManager_ = topManager;
	unique_number_ = 0;
	selected_ = 0;

	defaultColorFileIndex_ = 0;
	for (auto& c : defaultColorsFile_)
		c.setAlpha(0x20);

	widget_ = CreateEditorWidget();
}

PropertiesEditor* PropertiesItemsManager::GetEditor()
{
	return editor_;
}

QComboBox* PropertiesItemsManager::GetSelector()
{
	return selector_;
}

QWidget* PropertiesItemsManager::GetWidget()
{
	return widget_;
}

uint32_t PropertiesItemsManager::GetCurrentPropertiesId()
{
	if (selector_->count() > 0)
		return selector_->itemData(selector_->currentIndex()).toUInt();
	else
		return 0;
}

void PropertiesItemsManager::Create(const QString& unitId, uint32_t& propertiesId)
{
	const QColor color = defaultColorFileIndex_ < defaultColorsFile_.size() ?
		defaultColorsFile_[defaultColorFileIndex_++] : QColor("White");

	CubesUnitTypes::UnitParameters unitParameters{};
	topManager_->GetUnitParameters(unitId, unitParameters);

	propertiesId = ++unique_number_;
	QSharedPointer<PropertiesItem> pi(new PropertiesItem(this, editor_, unitParameters, propertiesId));

	QString propertiesName = QString::fromStdString(unitParameters.fileInfo.info.id) + " #" + QString("%1").arg(propertiesId);

	pi->SetName(propertiesName);
	items_[propertiesId] = pi;
	selector_->addItem(propertiesName, propertiesId);
	selector_->setCurrentIndex(selector_->count() - 1);
	//selector_->addItem(propertiesName);
	//selector_->setCurrentIndex(selector_->count() - 1);
	//selector_->setItemData(selector_->count() - 1, propertiesId);

	//emit FilesListChanged(GetFileNames());
}

void PropertiesItemsManager::Create(const CubesXml::Unit& xmlUnit, uint32_t& propertiesId)
{
	const QColor color = defaultColorFileIndex_ < defaultColorsFile_.size() ?
		defaultColorsFile_[defaultColorFileIndex_++] : QColor("White");

	CubesUnitTypes::UnitParameters unitParameters{};
	topManager_->GetUnitParameters(xmlUnit.id, unitParameters);

	propertiesId = ++unique_number_;
	QSharedPointer<PropertiesItem> pi(new PropertiesItem(this, editor_, unitParameters, xmlUnit, propertiesId));

	QString propertiesName = QString::fromStdString(unitParameters.fileInfo.info.id) + " #" + QString("%1").arg(propertiesId);

	pi->SetName(propertiesName);
	items_[propertiesId] = pi;
	selector_->addItem(propertiesName, propertiesId);
	selector_->setCurrentIndex(selector_->count() - 1);
}

void PropertiesItemsManager::Select(const uint32_t& propertiesId)
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

void PropertiesItemsManager::Remove(const uint32_t& propertiesId)
{
	int index = selector_->findData(propertiesId);
	if (index != -1)
		selector_->removeItem(index);

	//for (int i = 1; i < propertiesItemsManager_->GetSelector()->count(); i++)
	//{
	//    auto pi = propertiesItemsManager_->GetItem(di->propertiesId_);
	//    if (propertiesItemsManager_->GetSelector()->itemText(i) == pi->GetName())
	//    {
	//        propertiesItemsManager_->GetSelector()->removeItem(i);
	//        break;
	//    }
	//}
}

QSharedPointer<PropertiesItem> PropertiesItemsManager::GetItem(const uint32_t propertiesId)
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

bool PropertiesItemsManager::GetPropetiesForDrawing(const uint32_t propertiesId, PropertiesForDrawing& pfd)
{
	auto pi = GetItem(propertiesId);
	if (pi == nullptr)
		return false;

	pfd.pixmap = pi->GetPixmap();
	pfd.name = GetName(pi.get());
	pfd.fileName = pi->GetFileName();
	pfd.groupName = pi->GetGroupName();
	pfd.color = {};
	return true;
}

bool PropertiesItemsManager::GetUnitParameters(const uint32_t propertiesId, CubesUnitTypes::UnitParameters& unitParameters)
{
	auto pi = GetItem(propertiesId);
	if (pi == nullptr)
		return false;

	unitParameters = pi->GetUnitParameters();
	return true;
}

bool PropertiesItemsManager::GetUnitId(const uint32_t propertiesId, QString& unitId)
{
	auto pi = GetItem(propertiesId);
	if (pi == nullptr)
		return false;

	unitId = pi->GetUnitId();
	return true;

}

bool PropertiesItemsManager::InformVariableChanged()
{
	for (auto& p : items_)
	{
		auto name = GetName(p.get());
		int index = selector_->findData(p->GetPropertiesId());
		if (index != -1)
			selector_->setItemText(index, name);

		auto fileName = p->GetFileName();
		auto groupName = p->GetGroupName();

		emit BasePropertiesChanged(p->GetPropertiesId(), name, fileName, groupName);
	}
	return true;
}

//QStringList GetFileNames()
//{
//	QStringList fileNames;
//	for (auto& file : items_)
//		fileNames.push_back(file->GetName());
//	return fileNames;
//}
//
//QColor PropertiesItemsManager::GetFileColor(const QString& fileName)
//{
//	for (auto& fi : items_)
//	{
//		//if (fi->GetName() == fileName)
//		//	return fi->GetColor();
//	}
//	return QColor("Black");
//}
//
//QStringList PropertiesItemsManager::GetFileIncludeNames(const QString& fileName)
//{
//	QStringList fileIncludeNames;
//	fileIncludeNames.push_back("<not selected>");
//	for (auto& fi : items_)
//	{
//		//if (fi->GetName() == fileName)
//		//	fileIncludeNames.append(fi->GetIncludeNames());
//	}
//	return fileIncludeNames;
//}
//
//QList<QPair<QString, QString>> PropertiesItemsManager::GetFileIncludeVariables(const QString& fileName, const QString& includeName)
//{
//	QList<QPair<QString, QString>> result;
//
//	QStringList fileIncludeNames;
//	for (auto& fi : items_)
//	{
//		if (fi->GetName() == fileName)
//		{
//			//result = fi->GetIncludeVariables(includeName);
//			//break;
//		}
//	}
//
//	return result;
//}

void PropertiesItemsManager::Clear()
{
	editor_->GetPropertyEditor()->clear();
	selector_->clear();
	items_.clear();
}

bool PropertiesItemsManager::GetName(const uint32_t propertiesId, QString& name)
{
	auto pi = GetItem(propertiesId);
	if (pi == nullptr)
		return false;

	name = GetName(pi.get());
	return true;
}

void PropertiesItemsManager::AfterNameChanged(PropertiesItem* item)
{
	auto name = GetName(item);
	int index = selector_->findData(item->GetPropertiesId());
	if (index != -1)
		selector_->setItemText(index, name);

	auto fileName = item->GetFileName();
	auto groupName = item->GetGroupName();

	emit BasePropertiesChanged(item->GetPropertiesId(), name, fileName, groupName);
}

void PropertiesItemsManager::AfterFileNameChanged(PropertiesItem* item, QStringList& includeNames)
{
	auto name = GetName(item);
	int index = selector_->findData(item->GetPropertiesId());
	if (index != -1)
		selector_->setItemText(index, name);

	// Заполняем группы
	topManager_->GetFileIncludeList(item->GetFileName(), includeNames);

	auto fileName = item->GetFileName();
	auto groupName = item->GetGroupName();

	emit BasePropertiesChanged(item->GetPropertiesId(), name, fileName, groupName);
}

void PropertiesItemsManager::AfterIncludeNameChanged(PropertiesItem* item)
{
	auto name = GetName(item);
	int index = selector_->findData(item->GetPropertiesId());
	if (index != -1)
		selector_->setItemText(index, name);

	// Заполняем переменные
	//QList<QPair<QString, QString>>& variables
	//topManager_->GetFileIncludeVariableList(item->GetFileName(), item->GetGroupName(), variables);

	auto fileName = item->GetFileName();
	auto groupName = item->GetGroupName();

	emit BasePropertiesChanged(item->GetPropertiesId(), name, fileName, groupName);
}

void PropertiesItemsManager::AfterPositionChanged(PropertiesItem* item, double posX, double posY, double posZ)
{
	emit PositionChanged(item->GetPropertiesId(), posX, posY, posZ);
}

void PropertiesItemsManager::AfterError(PropertiesItem* item, const QString& message)
{
	emit OnError(item->GetPropertiesId(), message);
}

void PropertiesItemsManager::OnEditorCollapsed(QtBrowserItem* item)
{
	QString currentFileName = GetCurrentPropertiesId();
	SetFilePropertyExpanded(currentFileName, item->property(), false);
}

void PropertiesItemsManager::OnEditorExpanded(QtBrowserItem* item)
{
	QString currentFileName = GetCurrentPropertiesId();
	SetFilePropertyExpanded(currentFileName, item->property(), true);
}

void PropertiesItemsManager::OnContextMenuRequested(const QPoint& pos)
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
		connect(&action1, &QAction::triggered, this, &PropertiesItemsManager::OnDeleteInclude);
		contextMenu.addAction(&action1);

		contextMenu.exec(pe->mapToGlobal(pos));
	}
}

void PropertiesItemsManager::OnDeleteInclude(bool checked)
{

}

void PropertiesItemsManager::OnSelectorIndexChanged(int index)
{
	uint32_t currentId = GetCurrentPropertiesId();
	Select(currentId);
	emit SelectedItemChanged(currentId);
}

void PropertiesItemsManager::OnAddFileClicked()
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

void PropertiesItemsManager::OnRemoveFileClicked()
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

QWidget* PropertiesItemsManager::CreateEditorWidget()
{
	editor_ = new PropertiesEditor();
	qDebug() << connect(editor_, &PropertiesEditor::Collapsed, this, &PropertiesItemsManager::OnEditorCollapsed);
	qDebug() << connect(editor_, &PropertiesEditor::Expanded, this, &PropertiesItemsManager::OnEditorExpanded);
	qDebug() << connect(editor_, &PropertiesEditor::ContextMenuRequested, this, &PropertiesItemsManager::OnContextMenuRequested);

	QWidget* propertiesPanelWidget = new QWidget;

	QWidget* hostsButtonsWidget = CreateSelectorWidget();

	QVBoxLayout* propertiesPaneLayout = new QVBoxLayout;
	propertiesPaneLayout->addWidget(hostsButtonsWidget);
	propertiesPaneLayout->addWidget(editor_->GetPropertyEditor());
	propertiesPaneLayout->setContentsMargins(0, 0, 0, 0);

	propertiesPanelWidget->setLayout(propertiesPaneLayout);

	return propertiesPanelWidget;
}

QWidget* PropertiesItemsManager::CreateSelectorWidget()
{
	selector_ = new QComboBox();
	selector_->addItem("<not selected>", 0);
	qDebug() << connect(selector_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PropertiesItemsManager::OnSelectorIndexChanged);

	QHBoxLayout* hBoxLayoutPropertyListButtons = new QHBoxLayout;
	hBoxLayoutPropertyListButtons->setMargin(0);
	hBoxLayoutPropertyListButtons->setContentsMargins(0, 0, 0, 0);

	QToolButton* toolButtonPropertyListAdd = new QToolButton;
	toolButtonPropertyListAdd->setFixedSize(24, 24);
	toolButtonPropertyListAdd->setIconSize(QSize(24, 24));
	toolButtonPropertyListAdd->setIcon(QIcon(":/images/plus.png"));
	toolButtonPropertyListAdd->setToolTip(QString::fromLocal8Bit("Добавить хост"));
	hBoxLayoutPropertyListButtons->addWidget(toolButtonPropertyListAdd);
	connect(toolButtonPropertyListAdd, &QToolButton::clicked, this, &PropertiesItemsManager::OnAddFileClicked);

	QToolButton* toolButtonPropertyListRemove = new QToolButton;
	toolButtonPropertyListRemove->setFixedSize(24, 24);
	toolButtonPropertyListRemove->setIconSize(QSize(24, 24));
	toolButtonPropertyListRemove->setIcon(QIcon(":/images/minus.png"));
	toolButtonPropertyListRemove->setToolTip(QString::fromLocal8Bit("Удалить хост"));
	hBoxLayoutPropertyListButtons->addWidget(toolButtonPropertyListRemove);
	connect(toolButtonPropertyListRemove, &QToolButton::clicked, this, &PropertiesItemsManager::OnRemoveFileClicked);

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

void PropertiesItemsManager::SetFilePropertyExpanded(const QString& fileName, const QtProperty* property, bool is_expanded)
{
	QStringList fileIncludeNames;
	for (auto& fi : items_)
	{
		//if (fi->GetName() == fileName)
		//	fi->ExpandedChanged(property, is_expanded);
	}
}

QString PropertiesItemsManager::GetName(PropertiesItem* item)
{
	QList<QPair<QString, QString>> variables;
	topManager_->GetFileIncludeVariableList(item->GetFileName(), item->GetGroupName(), variables);

	QString name = item->GetName();
	for (const auto& v : variables)
	{
		QString replace = QString("@%1@").arg(v.first);
		name.replace(replace, v.second);
	}

	return name;
}
