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
#include "../xml/xml_writer.h"
#include "../windows/array_window.h"
#include "../utils/graph.h"
#include "../log/log_helper.h"
#include "properties_item_types.h"
#include "properties_item.h"
#include "properties_items_manager.h"

using namespace CubesProperties;

PropertiesItemsManager::PropertiesItemsManager(CubesTop::ITopManager* topManager,
	CubesLog::ILogManager* logManager, bool isArray, bool isMock):
	isArray_(isArray)
{
	topManager_ = topManager;
	logManager_ = logManager;
	selected_ = CubesUnit::InvalidPropertiesId;
	uniqueNumber_ = CubesUnit::InvalidPropertiesId;

	logHelper_.reset(new CubesLog::LogHelper(logManager_, CubesLog::SourceType::propertiesManager,
		GetPropertiesManagerErrorDescriptions()));

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

void PropertiesItemsManager::Create(const QString& unitId, CubesUnit::PropertiesId& propertiesId)
{
	CubesUnit::UnitParametersPtr unitParametersPtr{};
	topManager_->GetUnitParametersPtr(unitId, unitParametersPtr);

	propertiesId = ++uniqueNumber_;
	QSharedPointer<PropertiesItem> pi(new PropertiesItem(this, logManager_, editor_, unitParametersPtr, isArray_, propertiesId));

	items_[propertiesId] = pi;
	selector_->addItem(pi->GetName(), propertiesId);
	selector_->setCurrentIndex(selector_->count() - 1);

	logHelper_->LogInformation(static_cast<CubesLog::BaseErrorCode>(PropertiesManagerErrorCode::itemCreated),
		{ {QString::fromLocal8Bit("Имя"), pi->GetName()} }, propertiesId);
}

void PropertiesItemsManager::Create(const QString& unitId, CubesUnit::ParameterModelPtrs parameterModelPtrs,
	CubesUnit::PropertiesId& propertiesId)
{
	CubesUnit::UnitParametersPtr unitParametersPtr{};
	topManager_->GetUnitParametersPtr(unitId, unitParametersPtr);

	propertiesId = ++uniqueNumber_;
	QSharedPointer<PropertiesItem> pi(new PropertiesItem(this, logManager_, editor_, unitParametersPtr, propertiesId, parameterModelPtrs));

	QString propertiesName = parameterModelPtrs[0]->parameters[0]->value.toString();

	pi->SetName(propertiesName);
	items_[propertiesId] = pi;
	selector_->addItem(propertiesName, propertiesId);
	selector_->setCurrentIndex(selector_->count() - 1);

	logHelper_->LogInformation(static_cast<CubesLog::BaseErrorCode>(PropertiesManagerErrorCode::itemCreated),
		{ {QString::fromLocal8Bit("Имя"), pi->GetName()} }, propertiesId);
}

void PropertiesItemsManager::Create(const CubesXml::Unit& xmlUnit, CubesUnit::PropertiesId& propertiesId)
{
	CubesUnit::UnitParametersPtr unitParametersPtr{};
	topManager_->GetUnitParametersPtr(xmlUnit.id, unitParametersPtr);

	propertiesId = ++uniqueNumber_;
	QSharedPointer<PropertiesItem> pi(new PropertiesItem(this, logManager_, editor_, unitParametersPtr, xmlUnit, isArray_, propertiesId));

	items_[propertiesId] = pi;

	if (selector_ != nullptr)
	{
		selector_->addItem(pi->GetName(), propertiesId);
		selector_->setCurrentIndex(selector_->count() - 1);
	}

	logHelper_->LogInformation(static_cast<CubesLog::BaseErrorCode>(PropertiesManagerErrorCode::itemCreated),
		{ {QString::fromLocal8Bit("Имя"), pi->GetName()} }, propertiesId);

	topManager_->CreateDiagramItem(propertiesId);
}

void PropertiesItemsManager::Select(CubesUnit::PropertiesId propertiesId)
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

void PropertiesItemsManager::Remove(CubesUnit::PropertiesId propertiesId)
{
	// Получаем имя файла
	QString name = GetName(propertiesId);

	int index = selector_->findData(propertiesId);
	if (index != -1)
		selector_->removeItem(index);

	items_.remove(propertiesId);

	logHelper_->LogInformation(static_cast<CubesLog::BaseErrorCode>(PropertiesManagerErrorCode::itemRemoved),
		{ {QString::fromLocal8Bit("Имя"), name} }, propertiesId);
}

QSharedPointer<PropertiesItem> PropertiesItemsManager::GetItem(CubesUnit::PropertiesId propertiesId)
{
	auto it = items_.find(propertiesId);
	if (it != items_.end())
		return it.value();
	else
		return nullptr;
}

bool PropertiesItemsManager::GetUnitsInFileList(CubesUnit::FileId fileId, QStringList& unitNames)
{
	for (auto& item : items_)
	{
		if (item->GetFileId() == fileId)
			unitNames.push_back(item->GetName());
	}

	return true;
}

bool PropertiesItemsManager::GetUnitsInFileIncludeList(CubesUnit::FileId fileId,
	const CubesUnit::IncludeId includeId, QStringList& unitNames)
{
	for (auto& item : items_)
	{
		if (item->GetFileId() == fileId && item->GetIncludeId() == includeId)
			unitNames.push_back(item->GetName());
	}

	return true;
}

bool PropertiesItemsManager::GetUnitParameters(CubesUnit::PropertiesId propertiesId, CubesUnit::UnitParametersPtr& unitParametersPtr)
{
	auto pi = GetItem(propertiesId);
	if (pi == nullptr)
		return false;

	unitParametersPtr = pi->GetUnitParametersPtr();
	return true;
}

bool PropertiesItemsManager::GetUnitId(CubesUnit::PropertiesId propertiesId, QString& unitId)
{
	auto pi = GetItem(propertiesId);
	if (pi == nullptr)
		return false;

	unitId = pi->GetUnitId();
	return true;
}

bool PropertiesItemsManager::GetXmlUnit(CubesUnit::PropertiesId propertiesId, CubesXml::Unit& xmlUnit)
{
	auto pi = GetItem(propertiesId);
	if (pi == nullptr)
		return false;

	//CubesXml::Unit xmlUnit{};
	//pi->GetXml(xmlUnit);

	//CubesXml::Writer writer(logManager_);
	//writer.Write(byteArray, xmlUnit);

	pi->GetXml(xmlUnit);

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

		int gridSize = CubesDiagram::GridSize;
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

bool PropertiesItemsManager::InformFileNameChanged(CubesUnit::FileId fileId, const QString& fileName)
{
	for (auto& item : items_)
	{
		if (item->GetFileId() == fileId)
			item->SetFileIdName(fileId, fileName);
	}

	return true;
}

bool PropertiesItemsManager::InformFileListChanged(const CubesUnit::FileIdNames& fileNames)
{
	for (auto& item : items_)
		item->SetFileIdNames(fileNames);

	return true;
}

bool PropertiesItemsManager::InformIncludeNameChanged(CubesUnit::FileId fileId, CubesUnit::IncludeId includeId,
	const QString& includeName)
{
	for (auto& item : items_)
	{
		if (item->GetFileId() == fileId && item->GetIncludeId() == includeId)
			item->SetIncludeIdName(includeId, includeName);
	}

	return true;
}

bool PropertiesItemsManager::InformIncludesListChanged(CubesUnit::FileId fileId, const CubesUnit::IncludeIdNames& includeNames)
{
	for (auto& item : items_)
	{
		if (item->GetFileId() == fileId)
			item->SetIncludeIdNames(includeNames);
	}

	return true;
}

bool PropertiesItemsManager::InformFileColorChanged(CubesUnit::FileId fileId)
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
	if (editor_ != nullptr)
		editor_->GetPropertyEditor()->clear();
	if (selector_ != nullptr)
	{
		selector_->clear();
		selector_->addItem("<not selected>", 0);
	}
	items_.clear();

	uniqueNumber_ = 0;
	selected_ = 0;

	logHelper_->LogInformation(static_cast<CubesLog::BaseErrorCode>(PropertiesManagerErrorCode::allItemsRemoved));
}

bool PropertiesItemsManager::GetName(CubesUnit::PropertiesId propertiesId, QString& name)
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

QList<CubesXml::Group> PropertiesItemsManager::GetXmlGroups(const CubesUnit::FileId fileId,
	CubesUnit::IncludeId includeId)
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

CubesUnit::PropertiesIdParameterModelPtrs PropertiesItemsManager::GetPropertiesIdParameterModelPtrs()
{
	CubesUnit::PropertiesIdParameterModelPtrs result;

	for (const auto& item : items_)
	{
		const auto id = item->GetPropertiesId();
		const auto model = item->GetParameterModelPtrs();
		result[id] = model;
	}

	return result;
}

//bool PropertiesItemsManager::GetUnitParameters(CubesUnit::PropertiesIdUnitParameters& unitParameters)
//{
//	for (const auto& item : items_)
//	{
//		const auto id = item->GetFileId();
//		const auto parameters = item->GetUnitParameters();
//		unitParameters[id] = parameters;
//	}
//
//	return true;
//}

void PropertiesItemsManager::AfterNameChanged(CubesUnit::PropertiesId propertiesId)
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

void PropertiesItemsManager::AfterFileNameChanged(CubesUnit::PropertiesId propertiesId,
	CubesUnit::IncludeIdNames& includeNames)
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

void PropertiesItemsManager::AfterIncludeNameChanged(CubesUnit::PropertiesId propertiesId)
{
	QString name;
	GetName(propertiesId, name);

	if (selector_ != nullptr)
	{
		int index = selector_->findData(propertiesId);
		if (index != -1)
			selector_->setItemText(index, name);
	}

	auto item = GetItem(propertiesId);
	auto fileId = item->GetFileId();
	auto includeId = item->GetIncludeId();

	if (basePropertiesChangedDelegate_)
		basePropertiesChangedDelegate_(item->GetPropertiesId(), name, fileId, includeId);
}

void PropertiesItemsManager::AfterPositionChanged(CubesUnit::PropertiesId propertiesId, double posX, double posY, double posZ)
{
	if (positionChangedDelegate_)
		positionChangedDelegate_(propertiesId, posX, posY, posZ);
}

void PropertiesItemsManager::AfterSizeChanged(CubesUnit::PropertiesId propertiesId, QSizeF size)
{
	qDebug() << "PropertiesItemsManager::AfterSizeChanged : " << size;
	if (sizeChangedDelegate_)
		sizeChangedDelegate_(propertiesId, size);
}

void PropertiesItemsManager::AfterConnectionChanged(CubesUnit::PropertiesId propertiesId)
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
	CubesUnit::PropertiesId propertiesId = GetCurrentPropertiesId();
	SetPropertyExpanded(propertiesId, item->property(), false);
}

void PropertiesItemsManager::OnEditorExpanded(QtBrowserItem* item)
{
	CubesUnit::PropertiesId propertiesId = GetCurrentPropertiesId();
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
		auto pm = item->GetParameterModelPtr(pe->currentItem()->property());
		auto ui = item->GetUnitParametersPtr();

		// Проверяем,что параметр массив пользовательского типа данных, не перечислений
		auto pi = parameters::helper::parameter::get_parameter_info(ui->fileInfo,
			pm->parameterInfoId.type.toStdString(), pm->parameterInfoId.name.toStdString());
		if (pi == nullptr)
			return;
		bool is_array = parameters::helper::common::get_is_array_type(pi->type);
		bool is_inner_type = parameters::helper::common::get_is_inner_type(pi->type);
		if (!is_array || is_inner_type)
			return;


		CubesUnit::ParameterModelPtr pmCopy = CubesUnit::CreateParameterModelPtr(pm->Clone());

		parameters::file_info afi{};
		bool b = parameters::helper::common::extract_array_file_info(ui->fileInfo,
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
	CubesUnit::PropertiesId propertiesId = GetCurrentPropertiesId();
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
	CubesUnit::PropertiesId currentPropertiesId = GetCurrentPropertiesId();
	Select(currentPropertiesId);
	if (selectedItemChangedDelegate_)
		selectedItemChangedDelegate_(currentPropertiesId);
}

void PropertiesItemsManager::OnAddUnitClicked()
{
	CubesUnit::PropertiesId currentPropertiesId = GetCurrentPropertiesId();
	
	if (currentPropertiesId != 0)
	{
		QString unitId;
		if (!GetUnitId(currentPropertiesId, unitId))
			return;

		CubesUnit::PropertiesId propertiesId{ CubesUnit::InvalidPropertiesId };
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
		CubesUnit::UnitParametersPtr unitParametersPtr;
		topManager_->GetUnitParametersPtr("", unitParametersPtr);

		const auto unitId = QString::fromStdString(unitParametersPtr->fileInfo.info.id);

		CubesUnit::PropertiesId propertiesId{ CubesUnit::InvalidPropertiesId };
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
	CubesUnit::PropertiesId currentPropertiesId = GetCurrentPropertiesId();

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

void PropertiesItemsManager::SetPropertyExpanded(CubesUnit::PropertiesId propertiesId, const QtProperty* property, bool is_expanded)
{
	auto it = items_.find(propertiesId);
	if (it != items_.end())
		it.value()->ExpandedChanged(property, is_expanded);
}

QString PropertiesItemsManager::GetName(CubesUnit::PropertiesId propertiesId)
{
	auto item = GetItem(propertiesId);

	CubesUnit::VariableIdVariables variables;
	topManager_->GetFileIncludeVariableList(item->GetFileId(), item->GetIncludeId(), variables);

	QString name = item->GetName();
	for (const auto& v : variables)
	{
		QString replace = QString("@%1@").arg(v.first);
		name.replace(replace, v.second);
	}

	return name;
}

void PropertiesItemsManager::OnArrayWindowBeforeClose(const bool result, CubesUnit::ParameterModelPtr parameterModelPtr,
	QSharedPointer<CubesProperties::PropertiesItem> pi)
{
	pi->AddItems(parameterModelPtr);
}
