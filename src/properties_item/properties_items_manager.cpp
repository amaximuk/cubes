#include <QComboBox>
#include <QPlainTextEdit>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QToolButton>
#include <QLabel>
#include "qttreepropertybrowser.h"
#include "parameters.h"
#include "../top_manager_interface.h"
#include "../unit_types.h"
#include "../xml/xml_parser.h"
#include "properties_item.h"
#include "properties_items_manager.h"
#include <src/array_window.h>

using namespace CubesProperties;

PropertiesItemsManager::PropertiesItemsManager(ITopManager* topManager, CubesLog::ILogManager* logManager, bool isArray):
	isArray_(isArray)
{
	topManager_ = topManager;
	logManager_ = logManager;
	selected_ = CubesUnitTypes::InvalidPropertiesId;
	uniqueNumber_ = CubesUnitTypes::InvalidPropertiesId;

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

void PropertiesItemsManager::Create(const QString& unitId, CubesUnitTypes::PropertiesId& propertiesId)
{
	const QColor color = defaultColorFileIndex_ < defaultColorsFile_.size() ?
		defaultColorsFile_[defaultColorFileIndex_++] : QColor("White");

	CubesUnitTypes::UnitParameters unitParameters{};
	topManager_->GetUnitParameters(unitId, unitParameters);

	propertiesId = ++uniqueNumber_;
	QSharedPointer<PropertiesItem> pi(new PropertiesItem(this, editor_, unitParameters, isArray_, propertiesId));

	//QString propertiesName = QString::fromStdString(unitParameters.fileInfo.info.id) + " #" + QString("%1").arg(propertiesId);
	//pi->SetName(propertiesName);

	items_[propertiesId] = pi;
	selector_->addItem(pi->GetName(), propertiesId);
	selector_->setCurrentIndex(selector_->count() - 1);

	if (logManager_ != nullptr)
	{
		logManager_->AddMessage({ CubesLog::MessageType::information, propertiesId, "Properties Manager",
			QString("Item created, id = %1, name = %2").arg(propertiesId).arg(pi->GetName()) });
	}
}

void PropertiesItemsManager::Create(const QString& unitId, const CubesUnitTypes::ParametersModel& pm,
	CubesUnitTypes::PropertiesId& propertiesId)
{
	const QColor color = defaultColorFileIndex_ < defaultColorsFile_.size() ?
		defaultColorsFile_[defaultColorFileIndex_++] : QColor("White");

	CubesUnitTypes::UnitParameters unitParameters{};
	topManager_->GetUnitParameters(unitId, unitParameters);

	propertiesId = ++uniqueNumber_;
	QSharedPointer<PropertiesItem> pi(new PropertiesItem(this, editor_, unitParameters, propertiesId, pm));

	QString propertiesName = pm.parameters[0].parameters[0].value.toString();

	pi->SetName(propertiesName);
	items_[propertiesId] = pi;
	selector_->addItem(propertiesName, propertiesId);
	selector_->setCurrentIndex(selector_->count() - 1);

	if (logManager_ != nullptr)
	{
		logManager_->AddMessage({ CubesLog::MessageType::information, propertiesId, "Properties Manager",
			   QString("Item created, id = %1, name = %2").arg(propertiesId).arg(pi->GetName()) });
	}
}

void PropertiesItemsManager::Create(const CubesXml::Unit& xmlUnit, CubesUnitTypes::PropertiesId& propertiesId)
{
	const QColor color = defaultColorFileIndex_ < defaultColorsFile_.size() ?
		defaultColorsFile_[defaultColorFileIndex_++] : QColor("White");

	CubesUnitTypes::UnitParameters unitParameters{};
	topManager_->GetUnitParameters(xmlUnit.id, unitParameters);

	propertiesId = ++uniqueNumber_;
	QSharedPointer<PropertiesItem> pi(new PropertiesItem(this, editor_, unitParameters, xmlUnit, isArray_, propertiesId));

	//QString propertiesName = QString::fromStdString(unitParameters.fileInfo.info.id) + " #" + QString("%1").arg(propertiesId);
	//pi->SetName(propertiesName);

	items_[propertiesId] = pi;
	//	auto name = GetName(propertiesId);

	selector_->addItem(pi->GetName(), propertiesId);
	selector_->setCurrentIndex(selector_->count() - 1);

	if (logManager_ != nullptr)
	{
		logManager_->AddMessage({ CubesLog::MessageType::information, propertiesId, "Properties Manager",
		   QString("Item created, id = %1, name = %2").arg(propertiesId).arg(pi->GetName()) });
	}
}

void PropertiesItemsManager::Select(const CubesUnitTypes::PropertiesId propertiesId)
{
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

void PropertiesItemsManager::Remove(const CubesUnitTypes::PropertiesId propertiesId)
{
	int index = selector_->findData(propertiesId);
	if (index != -1)
		selector_->removeItem(index);

	items_.remove(propertiesId);

	if (logManager_ != nullptr)
	{
		logManager_->AddMessage({ CubesLog::MessageType::information, propertiesId, "Properties Manager",
		   QString("Item removed, id = %1").arg(propertiesId) });
	}
}

QSharedPointer<PropertiesItem> PropertiesItemsManager::GetItem(const CubesUnitTypes::PropertiesId propertiesId)
{
	auto it = items_.find(propertiesId);
	if (it != items_.end())
		return it.value();
	else
		return nullptr;
}

bool PropertiesItemsManager::GetUnitsInFileList(const CubesUnitTypes::FileId& fileId, QStringList& unitNames)
{
	for (auto& item : items_)
	{
		if (item->GetFileId() == fileId)
			unitNames.push_back(item->GetName());
	}

	return true;
}

bool PropertiesItemsManager::GetUnitsInFileIncludeList(const CubesUnitTypes::FileId& fileId,
	const CubesUnitTypes::IncludeId includeId, QStringList& unitNames)
{
	for (auto& item : items_)
	{
		if (item->GetFileId() == fileId && item->GetIncludeId() == includeId)
			unitNames.push_back(item->GetName());
	}

	return true;
}

bool PropertiesItemsManager::GetPropetiesForDrawing(const CubesUnitTypes::PropertiesId propertiesId, PropertiesForDrawing& pfd)
{
	auto pi = GetItem(propertiesId);
	if (pi == nullptr)
		return false;

	pfd.pixmap = pi->GetPixmap();
	pfd.name = GetName(propertiesId);
	pfd.fileName = pi->GetFileName();
	pfd.includeName = pi->GetIncludeName();
	pfd.color = {};
	return true;
}

bool PropertiesItemsManager::GetUnitParameters(const CubesUnitTypes::PropertiesId propertiesId, CubesUnitTypes::UnitParameters& unitParameters)
{
	auto pi = GetItem(propertiesId);
	if (pi == nullptr)
		return false;

	unitParameters = pi->GetUnitParameters();
	return true;
}

bool PropertiesItemsManager::GetUnitId(const CubesUnitTypes::PropertiesId propertiesId, QString& unitId)
{
	auto pi = GetItem(propertiesId);
	if (pi == nullptr)
		return false;

	unitId = pi->GetUnitId();
	return true;

}

bool PropertiesItemsManager::GetUnitsConnections(QMap<QString, QStringList>& connections)
{
	for (auto& item : items_)
	{
		QString name;
		if (!GetName(item->GetPropertiesId(), name))
			return false;

		QStringList conn = item->GetConnectedNames();
		if (conn.size() > 0)
			connections[name].append(conn);
	}

	return true;
}

bool PropertiesItemsManager::GetDependsConnections(QMap<QString, QStringList>& connections)
{
	for (auto& item : items_)
	{
		QString name;
		if (!GetName(item->GetPropertiesId(), name))
			return false;

		QStringList conn = item->GetDependentNames();
		if (conn.size() > 0)
			connections[name].append(conn);
	}

	return true;
}

bool PropertiesItemsManager::InformVariableChanged()
{
	for (auto& item : items_)
	{
		auto name = GetName(item->GetPropertiesId());
		int index = selector_->findData(item->GetPropertiesId());
		if (index != -1)
			selector_->setItemText(index, name);

		auto fileName = item->GetFileName();
		auto includeName = item->GetIncludeName();

		// TODO: ����� �� ��� ���? Inform �������� �� �����?
		emit BasePropertiesChanged(item->GetPropertiesId(), name, fileName, includeName);
	}

	return true;
}

bool PropertiesItemsManager::InformFileNameChanged(const CubesUnitTypes::FileId& fileId, const QString& fileName)
{
	for (auto& item : items_)
	{
		if (item->GetFileId() == fileId)
			item->SetFileIdName(fileId, fileName);
	}

	return true;
}

bool PropertiesItemsManager::InformFileListChanged(const CubesUnitTypes::FileIdNames& fileNames)
{
	for (auto& item : items_)
		item->SetFileIdNames(fileNames);

	return true;
}

bool PropertiesItemsManager::InformIncludeNameChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId& includeId,
	const QString& includeName)
{
	for (auto& item : items_)
	{
		if (item->GetFileId() == fileId && item->GetIncludeId() == includeId)
			item->SetIncludeIdName(includeId, includeName);
	}

	return true;
}

bool PropertiesItemsManager::InformIncludesListChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeIdNames& includeNames)
{
	for (auto& item : items_)
	{
		if (item->GetFileId() == fileId)
			item->SetIncludeIdNames(includeNames);
	}

	return true;
}

bool PropertiesItemsManager::InformFileColorChanged(const CubesUnitTypes::FileId& fileId, const QColor& color)
{
	for (auto& item : items_)
	{
		if (item->GetFileId() == fileId)
			item->color_ = color;
	}

	return true;
	for (auto& item : scene_->items())
	{
		CubeDiagram::DiagramItem* di = reinterpret_cast<CubeDiagram::DiagramItem*>(item);

		auto pi = propertiesItemsManager_->GetItem(di->GetPropertiesId());
		if (pi->GetFileId() == fileId)
			di->color_ = color;
	}

}

void PropertiesItemsManager::Clear()
{
	editor_->GetPropertyEditor()->clear();
	selector_->clear();
	selector_->addItem("<not selected>", 0);
	items_.clear();

	uniqueNumber_ = 0;
	selected_ = 0;

	defaultColorFileIndex_ = 0;

	if (logManager_ != nullptr)
	{
		logManager_->AddMessage({ CubesLog::MessageType::information, 0, "Properties Manager",
		   QString("All items removed") });
	}
}

bool PropertiesItemsManager::GetName(const CubesUnitTypes::PropertiesId propertiesId, QString& name)
{
	name = GetName(propertiesId);
	return true;
}

QList<uint32_t> PropertiesItemsManager::GetPropertyIds()
{
	return items_.keys();
}

QList<uint32_t> PropertiesItemsManager::GetPropertyIdsByFileName(const QString& fileName, const QString& includeName)
{
	QList<uint32_t> result;
	for (const auto& item : items_)
	{
		if (item->GetFileName() == fileName && item->GetIncludeName() == includeName)
			result.push_back(item->GetPropertiesId());
	}
	return result;
}

QList<CubesXml::Group> PropertiesItemsManager::GetXmlGroups(const CubesUnitTypes::FileId fileId,
	const CubesUnitTypes::IncludeId& includeId)
{
	QMap<QString, CubesXml::Group> xmlGroups;
	for (const auto& item : items_)
	{
		if (item->GetFileId() == fileId && item->GetIncludeId() == includeId)
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

bool PropertiesItemsManager::GetAnalysisProperties(QVector<CubesAnalysis::Properties>& properties)
{
	properties.clear();

	for (const auto& item : items_)
	{
		auto itemProperties = item->GetAnalysisProperties();
		itemProperties.propertiesId = item->GetPropertiesId();
		itemProperties.unitId = item->GetUnitId();
		itemProperties.name = GetName(item->GetPropertiesId());
		properties.push_back(itemProperties);
	}

	return true;
}

void PropertiesItemsManager::AfterNameChanged(const CubesUnitTypes::PropertiesId propertiesId)
{
	QString name;
	GetName(propertiesId, name);
	//	auto name = GetName(propertiesId);
	int index = selector_->findData(propertiesId);
	if (index != -1)
		selector_->setItemText(index, name);

	auto item = GetItem(propertiesId);
	auto fileName = item->GetFileName();
	auto includeName = item->GetIncludeName();

	emit BasePropertiesChanged(propertiesId, name, fileName, includeName);
}

void PropertiesItemsManager::AfterFileNameChanged(const CubesUnitTypes::PropertiesId propertiesId,
	CubesUnitTypes::IncludeIdNames& includeNames)
{
	QString name;
	GetName(propertiesId, name);
	//auto name = GetName(propertiesId);
	int index = selector_->findData(propertiesId);
	if (index != -1)
		selector_->setItemText(index, name);

	auto item = GetItem(propertiesId);

	// ��������� ������
	topManager_->GetFileIncludeList(item->GetFileId(), includeNames);

	auto fileName = item->GetFileName();
	auto includeName = item->GetIncludeName();

	emit BasePropertiesChanged(propertiesId, name, fileName, includeName);
}

void PropertiesItemsManager::AfterIncludeNameChanged(const CubesUnitTypes::PropertiesId propertiesId)
{
	QString name;
	GetName(propertiesId, name);
	//	auto name = GetName(propertiesId);
	int index = selector_->findData(propertiesId);
	if (index != -1)
		selector_->setItemText(index, name);

	auto item = GetItem(propertiesId);
	auto fileName = item->GetFileName();
	auto includeName = item->GetIncludeName();

	emit BasePropertiesChanged(propertiesId, name, fileName, includeName);
}

void PropertiesItemsManager::AfterPositionChanged(const CubesUnitTypes::PropertiesId propertiesId, double posX, double posY, double posZ)
{
	emit PositionChanged(propertiesId, posX, posY, posZ);
}

void PropertiesItemsManager::AfterError(const CubesUnitTypes::PropertiesId propertiesId, const QString& message)
{
	if (logManager_ != nullptr)
	{
		logManager_->AddMessage({ CubesLog::MessageType::error, propertiesId, "Properties Manager", message });
	}
	//emit Error(propertiesId, message);
}

void PropertiesItemsManager::AfterConnectionChanged(const CubesUnitTypes::PropertiesId propertiesId)
{
	emit ConnectionChanged(propertiesId);
}

void PropertiesItemsManager::AfterPropertiesChanged()
{
	emit PropertiesChanged();
}

void PropertiesItemsManager::OnEditorCollapsed(QtBrowserItem* item)
{
	uint32_t propertiesId = GetCurrentPropertiesId();
	SetPropertyExpanded(propertiesId, item->property(), false);
}

void PropertiesItemsManager::OnEditorExpanded(QtBrowserItem* item)
{
	uint32_t propertiesId = GetCurrentPropertiesId();
	SetPropertyExpanded(propertiesId, item->property(), true);
}

void PropertiesItemsManager::OnContextMenuRequested(const QPoint& pos)
{
	// $PARAMETERS/DEVICES/$ITEM_0/$PARAMETERS/STATES

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

		// ���������,��� �������� ������ ����������������� ���� ������, �� ������������
		auto pi = parameters::helper::parameter::get_parameter_info(ui.fileInfo,
			pm->parameterInfoId.type.toStdString(), pm->parameterInfoId.name.toStdString());
		if (pi == nullptr)
			return;
		bool is_array = parameters::helper::common::get_is_array_type(pi->type);
		bool is_inner_type = parameters::helper::common::get_is_inner_type(pi->type);
		if (!is_array || is_inner_type)
			return;


		auto pmCopy = *pm;

		parameters::file_info afi{};
		bool b = parameters::helper::common::extract_array_file_info(ui.fileInfo,
			pm->parameterInfoId.type.toStdString(), pm->parameterInfoId.name.toStdString(), afi);

		//auto rename = [](QList<CubesUnitTypes::ParameterModel>& parameters, QString to_remove, auto&& rename) -> void {
		//	for (auto& parameter : parameters)
		//	{
		//		parameter.id = parameter.id.mid(to_remove.length() + 1);
		//		rename(parameter.parameters, to_remove, rename);
		//	}
		//};

		// ������� ��� �������� ������� �� ������ ����������
		//item->RemoveSubProperties(pe->currentItem()->property());
		item->RemoveItems(pm->id);

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
			qDebug() << pm->id.toString();

			ArrayWindow* mv = new ArrayWindow();
			mv->setWindowModality(Qt::ApplicationModal);
			//mv->setAttribute(Qt::WA_DeleteOnClose, true);
			qDebug() << connect(mv, &ArrayWindow::BeforeClose, this, &PropertiesItemsManager::OnArrayWindowBeforeClose);
			mv->SetItemModel(afi, pmCopy, pi->restrictions, item);
			mv->show();
			//mv->deleteLater();
		}
		else
			return;
	}

	//QString name = pe->currentItem()->property()->propertyName();
	//QString parentName = pe->currentItem()->parent()->property()->propertyName();
	//if (parentName == QString::fromLocal8Bit("���������� �����"))
	//{
	//	QMenu contextMenu("Context menu");

	//	QAction action1(QString::fromLocal8Bit("������� %1").arg(name));
	//	connect(&action1, &QAction::triggered, this, &PropertiesItemsManager::OnDeleteInclude);
	//	contextMenu.addAction(&action1);

	//	contextMenu.exec(pe->mapToGlobal(pos));
	//}
}

void PropertiesItemsManager::OnCurrentItemChanged(QtBrowserItem* item)
{
	CubesUnitTypes::PropertiesId propertiesId = GetCurrentPropertiesId();
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
	uint32_t currentPropertiesId = GetCurrentPropertiesId();
	Select(currentPropertiesId);
	emit SelectedItemChanged(currentPropertiesId);
}

void PropertiesItemsManager::OnAddUnitClicked()
{
	uint32_t currentPropertiesId = GetCurrentPropertiesId();
	
	if (currentPropertiesId != 0)
	{
		QString unitId;
		if (!GetUnitId(currentPropertiesId, unitId))
			return;

		uint32_t propertiesId{ 0 };
		Create(unitId, propertiesId);

		PropertiesForDrawing pfd{};
		if (!GetPropetiesForDrawing(propertiesId, pfd))
			return;

		const auto item = GetItem(currentPropertiesId);
		

		QPointF pos = item->GetPosition();
		pos += { 40, 20 };
		topManager_->CreateDiagramItem(propertiesId, pfd, pos);
	}
	else
	{
		CubesUnitTypes::UnitParameters unitParameters;
		topManager_->GetUnitParameters("", unitParameters);

		const auto unitId = QString::fromStdString(unitParameters.fileInfo.info.id);

		uint32_t propertiesId{ 0 };
		Create(unitId, propertiesId);

		PropertiesForDrawing pfd{};
		if (!GetPropetiesForDrawing(propertiesId, pfd))
			return;

		QPointF pos{ 0, 0 };
		topManager_->CreateDiagramItem(propertiesId, pfd, pos);
	}

	//topManager_->CreateDiagramItem();
	//Create(const QString & unitId, bool isArrayUnit, uint32_t & propertiesId);

	//bool ok;
	//QString fileName = QInputDialog::getText(widget_, QString::fromLocal8Bit("���������� �����"), QString::fromLocal8Bit("��� �����:"), QLineEdit::Normal, "", &ok);
	//if (!ok || fileName.isEmpty())
	//	return;

	//int count = 0;
	//for (const auto& i : items_)
	//{
	//	if (i->GetName() == fileName)
	//		count++;
	//}
	//if (count > 0)
	//{
	//	QMessageBox::critical(widget_, "Error", QString::fromLocal8Bit("��� ��� ������������. ��������� �� �����������!"));
	//	return;
	//}

	//QColor fileColor = defaultColorFileIndex_ < defaultColorsFile_.size() ?
	//	defaultColorsFile_[defaultColorFileIndex_++] : QColor("White");

	//uint32_t propertiesId{ 0 };
	//Create(fileName, propertiesId);
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

void PropertiesItemsManager::OnAimUnitClicked()
{
	uint32_t currentPropertiesId = GetCurrentPropertiesId();

	if (currentPropertiesId != 0)
	{
		topManager_->EnshureVisible(currentPropertiesId);
	}

	//if (selected_ == 0)
	//{
	//	QMessageBox::critical(widget_, "Error", QString::fromLocal8Bit("���� �� ������!"));
	//	return;
	//}

	//// ��������� ����������� ��������
	//QStringList unitNames;
	//top_manager_->GetUnitsInFileList(selected_, unitNames);
	//if (unitNames.count() > 0)
	//{
	//	QString text = QString::fromLocal8Bit("��� ������������.\n�������� ����������!\n�����:\n");
	//	text.append(unitNames.join('\n'));
	//	QMessageBox::critical(widget_, "Error", text);
	//	return;
	//}

	//// ��������� �����, ����� �������� selected_ ���������
	//QString selected = selected_;

	//// ������� �� ���������, ������������� ���������� UnSelect
	//selector_->removeItem(selector_->findText(selected_));

	//// �������� ��� �����, ������ ���������� ������� ��� ��������
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

	//// ������� �� ������
	////items_.removeAll(toRemove);

	//// ���� ��� ��� ���������
	//if (items_.count() == 0)
	//	editor_->GetPropertyEditor()->clear();

	//// ��������� �� ��������
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

	if (isArray_)
	{
		QToolButton* toolButtonPropertyListAdd = new QToolButton;
		toolButtonPropertyListAdd->setFixedSize(24, 24);
		toolButtonPropertyListAdd->setIconSize(QSize(24, 24));
		toolButtonPropertyListAdd->setIcon(QIcon(":/images/plus.png"));
		toolButtonPropertyListAdd->setToolTip(QString::fromLocal8Bit("�������� ����"));
		hBoxLayoutPropertyListButtons->addWidget(toolButtonPropertyListAdd);
		connect(toolButtonPropertyListAdd, &QToolButton::clicked, this, &PropertiesItemsManager::OnAddUnitClicked);
	}

	QToolButton* toolButtonPropertyListRemove = new QToolButton;
	toolButtonPropertyListRemove->setFixedSize(24, 24);
	toolButtonPropertyListRemove->setIconSize(QSize(24, 24));
	toolButtonPropertyListRemove->setIcon(QIcon(":/images/aim.png"));
	toolButtonPropertyListRemove->setToolTip(QString::fromLocal8Bit("�������� ����"));
	hBoxLayoutPropertyListButtons->addWidget(toolButtonPropertyListRemove);
	connect(toolButtonPropertyListRemove, &QToolButton::clicked, this, &PropertiesItemsManager::OnAimUnitClicked);

	//QToolButton* toolButtonPropertyListRemove = new QToolButton;
	//toolButtonPropertyListRemove->setFixedSize(24, 24);
	//toolButtonPropertyListRemove->setIconSize(QSize(24, 24));
	//toolButtonPropertyListRemove->setIcon(QIcon(":/images/minus.png"));
	//toolButtonPropertyListRemove->setToolTip(QString::fromLocal8Bit("������� ����"));
	//hBoxLayoutPropertyListButtons->addWidget(toolButtonPropertyListRemove);
	//connect(toolButtonPropertyListRemove, &QToolButton::clicked, this, &PropertiesItemsManager::OnRemoveUnitClicked);

	QWidget* buttonsWidget = new QWidget;
	buttonsWidget->setLayout(hBoxLayoutPropertyListButtons);

	QLabel* label = new QLabel;
	label->setText(QString::fromLocal8Bit("�����:"));
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

void PropertiesItemsManager::SetPropertyExpanded(const CubesUnitTypes::PropertiesId propertiesId, const QtProperty* property, bool is_expanded)
{
	auto it = items_.find(propertiesId);
	if (it != items_.end())
		it.value()->ExpandedChanged(property, is_expanded);
}

QString PropertiesItemsManager::GetName(const CubesUnitTypes::PropertiesId propertiesId)
{
	auto item = GetItem(propertiesId);

	CubesUnitTypes::VariableIdVariables variables;
	topManager_->GetFileIncludeVariableList(item->GetFileId(), item->GetIncludeId(), variables);

	QString name = item->GetName();
	for (const auto& v : variables)
	{
		QString replace = QString("@%1@").arg(v.first);
		name.replace(replace, v.second);
	}

	return name;
}

void PropertiesItemsManager::OnArrayWindowBeforeClose(const bool result, CubesUnitTypes::ParameterModel pm,
	QSharedPointer<CubesProperties::PropertiesItem> pi)
{
	pi->AddItems(pm);
}
