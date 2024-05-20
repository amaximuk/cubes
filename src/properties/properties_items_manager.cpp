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
#include "../top/top_manager_interface.h"
#include "../unit/unit_types.h"
#include "../xml/xml_parser.h"
#include "../windows/array_window.h"
#include "../utils/graph.h"
#include "properties_item.h"
#include "properties_items_manager.h"

using namespace CubesProperties;

PropertiesItemsManager::PropertiesItemsManager(CubesTop::ITopManager* topManager,
	CubesLog::ILogManager* logManager, bool isArray, bool isMock):
	isArray_(isArray)
{
	topManager_ = topManager;
	logManager_ = logManager;
	selected_ = CubesUnitTypes::InvalidPropertiesId;
	uniqueNumber_ = CubesUnitTypes::InvalidPropertiesId;

	defaultColorFileIndex_ = 0;
	for (auto& c : defaultColorsFile_)
		c.setAlpha(0x20);

	if (!isMock)
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

	items_[propertiesId] = pi;
	selector_->addItem(pi->GetName(), propertiesId);
	selector_->setCurrentIndex(selector_->count() - 1);

	if (logManager_ != nullptr)
	{
		CubesLog::LogMessage lm{};
		lm.type = CubesLog::MessageType::information;
		lm.source = CubesLog::SourceType::propertiesManager;
		lm.description = QString("Item created, id = %1, name = %2").arg(propertiesId).arg(pi->GetName());
		lm.tag = propertiesId;
		logManager_->AddMessage(lm);
	}
}

void PropertiesItemsManager::Create(const QString& unitId, const CubesUnitTypes::ParameterModels& pm,
	CubesUnitTypes::PropertiesId& propertiesId)
{
	const QColor color = defaultColorFileIndex_ < defaultColorsFile_.size() ?
		defaultColorsFile_[defaultColorFileIndex_++] : QColor("White");

	CubesUnitTypes::UnitParameters unitParameters{};
	topManager_->GetUnitParameters(unitId, unitParameters);

	propertiesId = ++uniqueNumber_;
	QSharedPointer<PropertiesItem> pi(new PropertiesItem(this, editor_, unitParameters, propertiesId, pm));

	QString propertiesName = pm[0].parameters[0].value.toString();

	pi->SetName(propertiesName);
	items_[propertiesId] = pi;
	selector_->addItem(propertiesName, propertiesId);
	selector_->setCurrentIndex(selector_->count() - 1);

	if (logManager_ != nullptr)
	{
		CubesLog::LogMessage lm{};
		lm.type = CubesLog::MessageType::information;
		lm.source = CubesLog::SourceType::propertiesManager;
		lm.description = QString("Item created, id = %1, name = %2").arg(propertiesId).arg(pi->GetName());
		lm.tag = propertiesId;
		logManager_->AddMessage(lm);
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

	items_[propertiesId] = pi;

	selector_->addItem(pi->GetName(), propertiesId);
	selector_->setCurrentIndex(selector_->count() - 1);

	if (logManager_ != nullptr)
	{
		CubesLog::LogMessage lm{};
		lm.type = CubesLog::MessageType::information;
		lm.source = CubesLog::SourceType::propertiesManager;
		lm.description = QString("Item created, id = %1, name = %2").arg(propertiesId).arg(pi->GetName());
		lm.tag = propertiesId;
		logManager_->AddMessage(lm);
	}

	topManager_->CreateDiagramItem(propertiesId);
}

void PropertiesItemsManager::Select(CubesUnitTypes::PropertiesId propertiesId)
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

void PropertiesItemsManager::Remove(CubesUnitTypes::PropertiesId propertiesId)
{
	int index = selector_->findData(propertiesId);
	if (index != -1)
		selector_->removeItem(index);

	items_.remove(propertiesId);

	if (logManager_ != nullptr)
	{
		CubesLog::LogMessage lm{};
		lm.type = CubesLog::MessageType::information;
		lm.source = CubesLog::SourceType::propertiesManager;
		lm.description = QString("Item removed, id = %1").arg(propertiesId);
		lm.tag = propertiesId;
		logManager_->AddMessage(lm);
	}
}

QSharedPointer<PropertiesItem> PropertiesItemsManager::GetItem(CubesUnitTypes::PropertiesId propertiesId)
{
	auto it = items_.find(propertiesId);
	if (it != items_.end())
		return it.value();
	else
		return nullptr;
}

bool PropertiesItemsManager::GetUnitsInFileList(CubesUnitTypes::FileId fileId, QStringList& unitNames)
{
	for (auto& item : items_)
	{
		if (item->GetFileId() == fileId)
			unitNames.push_back(item->GetName());
	}

	return true;
}

bool PropertiesItemsManager::GetUnitsInFileIncludeList(CubesUnitTypes::FileId fileId,
	const CubesUnitTypes::IncludeId includeId, QStringList& unitNames)
{
	for (auto& item : items_)
	{
		if (item->GetFileId() == fileId && item->GetIncludeId() == includeId)
			unitNames.push_back(item->GetName());
	}

	return true;
}

bool PropertiesItemsManager::GetUnitParameters(CubesUnitTypes::PropertiesId propertiesId, CubesUnitTypes::UnitParameters& unitParameters)
{
	auto pi = GetItem(propertiesId);
	if (pi == nullptr)
		return false;

	unitParameters = pi->GetUnitParameters();
	return true;
}

bool PropertiesItemsManager::GetUnitId(CubesUnitTypes::PropertiesId propertiesId, QString& unitId)
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

bool PropertiesItemsManager::SortUnitsBoost()
{
	if (items_.empty())
		return true;

	// Prepare sort
	int nextIndex = 0;
	QMap<QString, int> nameToIndex;
	QMap<int, QString> indexToName;
	QMap<QString, QSet<QString>> connectedNames;
	for (auto& item : items_)
	{
		QString name;
		if (!GetName(item->GetPropertiesId(), name))
			return false;

		if (!nameToIndex.contains(name))
		{
			nameToIndex[name] = nextIndex;
			indexToName[nextIndex] = name;
			nextIndex++;
		}

		auto connected = item->GetConnectedNames();
		connectedNames[name].unite(QSet<QString>(connected.begin(), connected.end()));
	}

	// Sort
	std::vector<std::pair<int, int>> edges;

	for (const auto& kvp : connectedNames.toStdMap())
	{
		for (const auto& se : kvp.second)
		{
			if (nameToIndex.contains(kvp.first) && nameToIndex.contains(se))
				edges.push_back({ nameToIndex[kvp.first], nameToIndex[se] });
		}
	}

	std::vector<std::pair<int, int>> coordinates;
	if (!CubesGraph::RearrangeGraph(nameToIndex.size(), edges, coordinates))
		return false;

	// Update positions
	for (auto& item : items_)
	{
		QString name;
		if (!GetName(item->GetPropertiesId(), name))
			return false;

		int i = nameToIndex[name];

		int gridSize = 20;
		QPointF position(80 + coordinates[i].first * 80, 80 + coordinates[i].second * 80);

		item->SetPosition(position);
		double z = item->GetZOrder();
		AfterPositionChanged(item->GetPropertiesId(), position.x(), position.y(), z);
	}

	return true;
}

bool PropertiesItemsManager::SortUnitsRectangular(bool check)
{
	if (items_.empty())
		return true;

	bool sort = true;
	if (check)
	{
		int count = 0;
		for (auto& item : items_)
		{
			QPointF p = item->GetPosition();
			if (qFuzzyIsNull(p.x()) && qFuzzyIsNull(p.y()))
				++count;
		}

		// Все нулевые, распределяем по сетке
		if (count != items_.size())
			sort = false;
	}

	if (sort)
	{
		int size = items_.size();
		int rows = std::sqrt(items_.size());
		int columns = (items_.size() + rows - 1) / rows;

		int c = 0;
		int r = 0;
		for (auto& item : items_)
		{
			QPoint position(c * 200, r * 80);

			item->SetPosition(position);
			double z = item->GetZOrder();
			AfterPositionChanged(item->GetPropertiesId(), position.x(), position.y(), z);

			if (++c == columns) { ++r; c = 0; };
		}
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

		auto fileId = item->GetFileId();
		auto includeId = item->GetIncludeId();

		if (basePropertiesChangedDelegate_)
			basePropertiesChangedDelegate_(item->GetPropertiesId(), name, fileId, includeId);
	}

	return true;
}

bool PropertiesItemsManager::InformFileNameChanged(CubesUnitTypes::FileId fileId, const QString& fileName)
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

bool PropertiesItemsManager::InformIncludeNameChanged(CubesUnitTypes::FileId fileId, CubesUnitTypes::IncludeId includeId,
	const QString& includeName)
{
	for (auto& item : items_)
	{
		if (item->GetFileId() == fileId && item->GetIncludeId() == includeId)
			item->SetIncludeIdName(includeId, includeName);
	}

	return true;
}

bool PropertiesItemsManager::InformIncludesListChanged(CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeIdNames& includeNames)
{
	for (auto& item : items_)
	{
		if (item->GetFileId() == fileId)
			item->SetIncludeIdNames(includeNames);
	}

	return true;
}

bool PropertiesItemsManager::InformFileColorChanged(CubesUnitTypes::FileId fileId)
{
	for (auto& item : items_)
	{
		auto name = GetName(item->GetPropertiesId());
		auto fileId = item->GetFileId();
		auto includeId = item->GetIncludeId();

		if (basePropertiesChangedDelegate_)
			basePropertiesChangedDelegate_(item->GetPropertiesId(), name, fileId, includeId);
	}

	return true;
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
		CubesLog::LogMessage lm{};
		lm.type = CubesLog::MessageType::information;
		lm.source = CubesLog::SourceType::propertiesManager;
		lm.description = QString("All items removed");
		lm.tag = CubesUnitTypes::InvalidUniversalId;
		logManager_->AddMessage(lm);
	}
}

bool PropertiesItemsManager::GetName(CubesUnitTypes::PropertiesId propertiesId, QString& name)
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
	CubesUnitTypes::IncludeId includeId)
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

bool PropertiesItemsManager::GetParameterModels(CubesUnitTypes::PropertiesIdParameterModels& models)
{
	for (const auto& item : items_)
	{
		const auto id = item->GetPropertiesId();
		const auto model = item->GetParameterModels();
		models[id] = model;
	}

	return true;
}

bool PropertiesItemsManager::GetUnitParameters(CubesUnitTypes::PropertiesIdUnitParameters& unitParameters)
{
	for (const auto& item : items_)
	{
		const auto id = item->GetFileId();
		const auto parameters = item->GetUnitParameters();
		unitParameters[id] = parameters;
	}

	return true;
}

void PropertiesItemsManager::AfterNameChanged(CubesUnitTypes::PropertiesId propertiesId)
{
	QString name;
	GetName(propertiesId, name);

	int index = selector_->findData(propertiesId);
	if (index != -1)
		selector_->setItemText(index, name);

	auto item = GetItem(propertiesId);
	auto fileId = item->GetFileId();
	auto includeId = item->GetIncludeId();

	if (basePropertiesChangedDelegate_)
		basePropertiesChangedDelegate_(item->GetPropertiesId(), name, fileId, includeId);
}

void PropertiesItemsManager::AfterFileNameChanged(CubesUnitTypes::PropertiesId propertiesId,
	CubesUnitTypes::IncludeIdNames& includeNames)
{
	QString name;
	GetName(propertiesId, name);

	int index = selector_->findData(propertiesId);
	if (index != -1)
		selector_->setItemText(index, name);

	auto item = GetItem(propertiesId);

	// Заполняем группы
	topManager_->GetFileIncludeList(item->GetFileId(), includeNames);

	auto fileId = item->GetFileId();
	auto includeId = item->GetIncludeId();

	if (basePropertiesChangedDelegate_)
		basePropertiesChangedDelegate_(item->GetPropertiesId(), name, fileId, includeId);
}

void PropertiesItemsManager::AfterIncludeNameChanged(CubesUnitTypes::PropertiesId propertiesId)
{
	QString name;
	GetName(propertiesId, name);

	int index = selector_->findData(propertiesId);
	if (index != -1)
		selector_->setItemText(index, name);

	auto item = GetItem(propertiesId);
	auto fileId = item->GetFileId();
	auto includeId = item->GetIncludeId();

	if (basePropertiesChangedDelegate_)
		basePropertiesChangedDelegate_(item->GetPropertiesId(), name, fileId, includeId);
}

void PropertiesItemsManager::AfterPositionChanged(CubesUnitTypes::PropertiesId propertiesId, double posX, double posY, double posZ)
{
	if (positionChangedDelegate_)
		positionChangedDelegate_(propertiesId, posX, posY, posZ);
}

void PropertiesItemsManager::AfterError(CubesUnitTypes::PropertiesId propertiesId, const QString& message)
{
	if (logManager_ != nullptr)
	{
		CubesLog::LogMessage lm{};
		lm.type = CubesLog::MessageType::error;
		lm.source = CubesLog::SourceType::propertiesManager;
		lm.description = message;
		lm.tag = propertiesId;
		logManager_->AddMessage(lm);
	}
}

void PropertiesItemsManager::AfterConnectionChanged(CubesUnitTypes::PropertiesId propertiesId)
{
	if (connectionChangedDelegate_)
		connectionChangedDelegate_(propertiesId);
}

void PropertiesItemsManager::AfterPropertiesChanged()
{
	if (propertiesChangedDelegate_)
		propertiesChangedDelegate_();
}

void PropertiesItemsManager::OnEditorCollapsed(QtBrowserItem* item)
{
	CubesUnitTypes::PropertiesId propertiesId = GetCurrentPropertiesId();
	SetPropertyExpanded(propertiesId, item->property(), false);
}

void PropertiesItemsManager::OnEditorExpanded(QtBrowserItem* item)
{
	CubesUnitTypes::PropertiesId propertiesId = GetCurrentPropertiesId();
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

		// Проверяем,что параметр массив пользовательского типа данных, не перечислений
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

		// Удаляем все элементы массива из панели параметров
		item->RemoveItems(pm->id);

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
	CubesUnitTypes::PropertiesId currentPropertiesId = GetCurrentPropertiesId();
	Select(currentPropertiesId);
	if (selectedItemChangedDelegate_)
		selectedItemChangedDelegate_(currentPropertiesId);
}

void PropertiesItemsManager::OnAddUnitClicked()
{
	CubesUnitTypes::PropertiesId currentPropertiesId = GetCurrentPropertiesId();
	
	if (currentPropertiesId != 0)
	{
		QString unitId;
		if (!GetUnitId(currentPropertiesId, unitId))
			return;

		CubesUnitTypes::PropertiesId propertiesId{ CubesUnitTypes::InvalidPropertiesId };
		Create(unitId, propertiesId);

		auto pi = GetItem(propertiesId);
		const auto cpi = GetItem(currentPropertiesId);
		QPointF pos = cpi->GetPosition();
		pos += { 40, 20 };
		pi->SetPosition(pos);

		topManager_->CreateDiagramItem(propertiesId);
		//CubesTop::PropertiesForDrawing pfd{};

		//auto pi = GetItem(propertiesId);
		//if (pi == nullptr)
		//	return;

		//pfd.pixmap = pi->GetPixmap();
		//QString name;
		//if (!GetName(propertiesId, name))
		//	return;
		//pfd.name = name;
		//pfd.fileName = pi->GetFileName();
		//pfd.includeName = pi->GetIncludeName();
		//pfd.color = Qt::white;

		//const auto item = GetItem(currentPropertiesId);

		//QPointF pos = item->GetPosition();
		//pos += { 40, 20 };
		//topManager_->CreateDiagramItem(propertiesId, pfd, pos);
	}
	else
	{
		CubesUnitTypes::UnitParameters unitParameters;
		topManager_->GetUnitParameters("", unitParameters);

		const auto unitId = QString::fromStdString(unitParameters.fileInfo.info.id);

		CubesUnitTypes::PropertiesId propertiesId{ CubesUnitTypes::InvalidPropertiesId };
		Create(unitId, propertiesId);

		topManager_->CreateDiagramItem(propertiesId);

		//CubesTop::PropertiesForDrawing pfd{};

		//auto pi = GetItem(propertiesId);
		//if (pi == nullptr)
		//	return;

		//pfd.pixmap = pi->GetPixmap();
		//QString name;
		//if (!GetName(propertiesId, name))
		//	return;
		//pfd.name = name;
		//pfd.fileName = pi->GetFileName();
		//pfd.includeName = pi->GetIncludeName();
		//pfd.color = Qt::white;

		//QPointF pos{ 0, 0 };
		//topManager_->CreateDiagramItem(propertiesId, pfd, pos);
	}
}

void PropertiesItemsManager::OnAimUnitClicked()
{
	CubesUnitTypes::PropertiesId currentPropertiesId = GetCurrentPropertiesId();

	if (currentPropertiesId != 0)
	{
		topManager_->EnshureVisible(currentPropertiesId);
	}
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
		toolButtonPropertyListAdd->setToolTip(QString::fromLocal8Bit("Добавить юнит"));
		hBoxLayoutPropertyListButtons->addWidget(toolButtonPropertyListAdd);
		connect(toolButtonPropertyListAdd, &QToolButton::clicked, this, &PropertiesItemsManager::OnAddUnitClicked);
	}

	QToolButton* toolButtonPropertyListRemove = new QToolButton;
	toolButtonPropertyListRemove->setFixedSize(24, 24);
	toolButtonPropertyListRemove->setIconSize(QSize(24, 24));
	toolButtonPropertyListRemove->setIcon(QIcon(":/images/aim.png"));
	toolButtonPropertyListRemove->setToolTip(QString::fromLocal8Bit("Показать юнит"));
	hBoxLayoutPropertyListButtons->addWidget(toolButtonPropertyListRemove);
	connect(toolButtonPropertyListRemove, &QToolButton::clicked, this, &PropertiesItemsManager::OnAimUnitClicked);

	//QToolButton* toolButtonPropertyListRemove = new QToolButton;
	//toolButtonPropertyListRemove->setFixedSize(24, 24);
	//toolButtonPropertyListRemove->setIconSize(QSize(24, 24));
	//toolButtonPropertyListRemove->setIcon(QIcon(":/images/minus.png"));
	//toolButtonPropertyListRemove->setToolTip(QString::fromLocal8Bit("Удалить юнит"));
	//hBoxLayoutPropertyListButtons->addWidget(toolButtonPropertyListRemove);
	//connect(toolButtonPropertyListRemove, &QToolButton::clicked, this, &PropertiesItemsManager::OnRemoveUnitClicked);

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

void PropertiesItemsManager::SetPropertyExpanded(CubesUnitTypes::PropertiesId propertiesId, const QtProperty* property, bool is_expanded)
{
	auto it = items_.find(propertiesId);
	if (it != items_.end())
		it.value()->ExpandedChanged(property, is_expanded);
}

QString PropertiesItemsManager::GetName(CubesUnitTypes::PropertiesId propertiesId)
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
