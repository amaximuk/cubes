#include <QComboBox>
#include <QPlainTextEdit>
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
#include "../parameters_compiler/parameters_compiler_helper.h"
#include "properties_item.h"
#include "properties_items_manager.h"
#include <src/array_window.h>

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

void PropertiesItemsManager::Create(const QString& unitId, const CubesUnitTypes::ParametersModel& pm, uint32_t& propertiesId)
{
	const QColor color = defaultColorFileIndex_ < defaultColorsFile_.size() ?
		defaultColorsFile_[defaultColorFileIndex_++] : QColor("White");

	CubesUnitTypes::UnitParameters unitParameters{};
	topManager_->GetUnitParameters(unitId, unitParameters);

	propertiesId = ++unique_number_;
	QSharedPointer<PropertiesItem> pi(new PropertiesItem(this, editor_, unitParameters, propertiesId, pm));

	//QString propertiesName = QString::fromStdString(unitParameters.fileInfo.info.id) + " #" + QString("%1").arg(propertiesId);
	QString propertiesName = pm.parameters[0].parameters[0].value.toString();

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

	items_.remove(propertiesId);
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

QList<uint32_t> PropertiesItemsManager::GetPropertyIdsByFileName(const QString& fileName, const QString& includeFileName)
{
	QList<uint32_t> result;
	for (const auto& item : items_)
	{
		if (item->GetFileName() == fileName && item->GetGroupName() == includeFileName)
			result.push_back(item->GetPropertiesId());
	}
	return result;
}

QList<CubesXml::Group> PropertiesItemsManager::GetXmlGroups(const QString& fileName, const QString& includeFileName)
{
	QMap<QString, CubesXml::Group> xmlGroups;
	for (const auto& item : items_)
	{
		if (item->GetFileName() == fileName && item->GetGroupName() == includeFileName)
		{
			CubesXml::Unit xmlUnit{};
			item->GetXml(xmlUnit);
			const auto group = item->GetUnitCategory().toLower();

			if (!xmlGroups.contains(group))
			{
				CubesXml::Group xmlGroup{};
				xmlGroup.path = group;
				xmlGroups[group] = xmlGroup;
			}
			xmlGroups[group].units.push_back(xmlUnit);
		}
	}

	QList<CubesXml::Group> result;
	for (const auto& xmlGroup : xmlGroups)
		result.push_back(xmlGroup);

	return result;
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
	uint32_t propertiesId = GetCurrentPropertiesId();
	SetFilePropertyExpanded(propertiesId, item->property(), false);
}

void PropertiesItemsManager::OnEditorExpanded(QtBrowserItem* item)
{
	uint32_t propertiesId = GetCurrentPropertiesId();
	SetFilePropertyExpanded(propertiesId, item->property(), true);
}

void PropertiesItemsManager::OnContextMenuRequested(const QPoint& pos)
{
	auto pe = editor_->GetPropertyEditor();
	if (pe->currentItem() == nullptr)
		return;
	if (pe->currentItem()->parent() == nullptr)
		return;



	auto propertiesId = GetCurrentPropertiesId();
	auto item = GetItem(propertiesId);
	if (item != nullptr)
	{
		auto pm = item->GetParameterModel(pe->currentItem()->property());
		auto ui = item->GetUnitParameters();


		parameters_compiler::file_info afi{};
		bool b = parameters_compiler::helper::extract_array_file_info(ui.fileInfo,
			pm->parameterInfoId.type.toStdString(), pm->parameterInfoId.name.toStdString(), afi);





		auto rename = [](QList<CubesUnitTypes::ParameterModel>& parameters, QString to_remove, auto&& rename) -> void {
			for (auto& parameter : parameters)
			{
				parameter.id = parameter.id.mid(to_remove.length() + 1);
				//parameter.id = "PARAMETERS/" + parameter.id.mid(to_remove.length() + 1);
				rename(parameter.parameters, to_remove, rename);
			}
		};




		item->RemoveSubProperties(pe->currentItem()->property());


		//{
		//	auto property = pe->currentItem()->property();

		//	//QMap<QString, const QtProperty*> idToProperty;
		//	//for (int i = property->subProperties().size(); i < count; ++i)
		//	//	property->addSubProperty(editor_->CreatePropertyForModel(pm->parameters[i], idToProperty));
		//	//for (const auto& kvp : idToProperty.toStdMap())
		//	//	RegisterProperty(kvp.second, kvp.first);

		//	auto collect = [](QtProperty* property, QList<QtProperty*>& list, auto&& collect) -> void {
		//		list.push_back(property);
		//		for (const auto& p : property->subProperties())
		//		{
		//			collect(p, list, collect);
		//			property->removeSubProperty(p);
		//		}
		//	};


		//	//QList<QtProperty*> toRemove;
		//	QList<QtProperty*> toUnregister;
		//	//const auto& subProperties = property->subProperties();
		//	//for (int i = count; i < subProperties.size(); ++i)
		//	//{
		//	//	collect(subProperties[i], toUnregister, collect);
		//	//	toRemove.push_back(subProperties[i]);
		//	//}
		//	collect(property, toUnregister, collect);

		//	//for (auto& p : toRemove)
		//	//	property->removeSubProperty(p);

		//	for (auto& p : toUnregister)
		//		UnregisterProperty(p);
		//}











		if (b)
		{
			qDebug() << pm->id;





			ArrayWindow* mv = new ArrayWindow();
			mv->setWindowModality(Qt::ApplicationModal);
			//mv->setAttribute(Qt::WA_DeleteOnClose, true);
			qDebug() << connect(mv, &ArrayWindow::BeforeClose, this, &PropertiesItemsManager::OnArrayWindowBeforeClose);
			mv->SetItemModel(afi, pm);
			mv->show();
			//mv->deleteLater();

		}
		else
			return;
	}






	

	//QString name = pe->currentItem()->property()->propertyName();
	//QString parentName = pe->currentItem()->parent()->property()->propertyName();
	//if (parentName == QString::fromLocal8Bit("Включаемые файлы"))
	//{
	//	QMenu contextMenu("Context menu");

	//	QAction action1(QString::fromLocal8Bit("Удалить %1").arg(name));
	//	connect(&action1, &QAction::triggered, this, &PropertiesItemsManager::OnDeleteInclude);
	//	contextMenu.addAction(&action1);

	//	contextMenu.exec(pe->mapToGlobal(pos));
	//}
}

void PropertiesItemsManager::OnCurrentItemChanged(QtBrowserItem* item)
{
	uint32_t propertiesId = GetCurrentPropertiesId();
	if (item != nullptr && items_.contains(propertiesId))
	{
		QString description = items_[propertiesId]->GetPropertyDescription(item->property());
		hint_->setPlainText(description);
	}
	else
		hint_->setPlainText("");
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
	qDebug() << connect(editor_, &PropertiesEditor::CurrentItemChanged, this, &PropertiesItemsManager::OnCurrentItemChanged);

	QWidget* propertiesPanelWidget = new QWidget;

	QWidget* hostsButtonsWidget = CreateSelectorWidget();

	QWidget* hintWidget = CreateHintWidget();

	QVBoxLayout* propertiesPaneLayout = new QVBoxLayout;
	propertiesPaneLayout->addWidget(hostsButtonsWidget, 0);
	propertiesPaneLayout->addWidget(editor_->GetPropertyEditor(), 1);
	propertiesPaneLayout->addWidget(hintWidget, 0);
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
	label->setText(QString::fromLocal8Bit("Юниты:"));
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

QWidget* PropertiesItemsManager::CreateHintWidget()
{
	QWidget* hintWidget = new QWidget;
	hint_ = new QPlainTextEdit;
	hint_->setFixedHeight(100);
	hint_->setReadOnly(true);
	QVBoxLayout* vBoxLayoutHint = new QVBoxLayout;
	vBoxLayoutHint->setMargin(0);
	vBoxLayoutHint->addWidget(hint_);
	vBoxLayoutHint->setContentsMargins(0, 0, 0, 0);
	hintWidget->setLayout(vBoxLayoutHint);
	return hintWidget;
}

void PropertiesItemsManager::SetFilePropertyExpanded(const uint32_t propertiesId, const QtProperty* property, bool is_expanded)
{
	if (items_.contains(propertiesId))
	{
		items_[propertiesId]->ExpandedChanged(property, is_expanded);
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

void PropertiesItemsManager::OnArrayWindowBeforeClose(const bool result)
{
	int a = 0;
}
