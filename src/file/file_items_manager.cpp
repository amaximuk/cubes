#include <QComboBox>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QFileInfo>
#include <QDebug>
#include "qttreepropertybrowser.h"
#include "../top/top_manager_interface.h"
#include "../unit/unit_types.h"
#include "../log/log_manager_interface.h"
#include "file_item.h"
#include "file_items_manager.h"

using namespace CubesFile;

FileItemsManager::FileItemsManager(CubesTop::ITopManager* topManager, CubesLog::ILogManager* logManager, bool isMock)
{
	topManager_ = topManager;
	logManager_ = logManager;
	isMock_ = isMock;
	selected_ = CubesUnitTypes::InvalidFileId;
	uniqueNumber_ = CubesUnitTypes::InvalidFileId;

	defaultColorFileIndex_ = 0;
	for (auto& c : defaultColorsFile_)
		c.setAlpha(0x20);

	if (!isMock)
		widget_ = CreateEditorWidget();
}

PropertiesEditor* FileItemsManager::GetEditor()
{
	return editor_;
}

QComboBox* FileItemsManager::GetSelector()
{
	return selector_;
}

QWidget* FileItemsManager::GetWidget()
{
	return widget_;
}

CubesUnitTypes::FileId FileItemsManager::GetCurrentFileId()
{
	if (selector_->count() > 0)
		return selector_->itemData(selector_->currentIndex()).toUInt();
	else
		return CubesUnitTypes::InvalidFileId;
}

QString FileItemsManager::GetCurrentFileName()
{
	if (selector_->count() > 0)
		return selector_->currentText();
	else
		return "";
}

CubesUnitTypes::FileId FileItemsManager::GetFileId(const QString& fileName)
{
	for (auto& fi : items_)
	{
		if (fi->GetName() == fileName)
			return fi->GetFileId();
	}
	return CubesUnitTypes::InvalidFileId;
}

void FileItemsManager::Create(const QString& filePath, QString& fileName, QString& platform, CubesUnitTypes::FileId& fileId)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! filePath unused!!!
	const QColor color = defaultColorFileIndex_ < defaultColorsFile_.size() ?
		defaultColorsFile_[defaultColorFileIndex_++] : QColor("White");

	fileId = ++uniqueNumber_;

	if (fileName == "")
		fileName = QString::fromLocal8Bit("Файл %1").arg(fileId);

	auto it = std::find(CubesUnitTypes::platform_names_.cbegin(), CubesUnitTypes::platform_names_.cend(), platform.toStdString());
	if (platform == "" || it == CubesUnitTypes::platform_names_.cend())
		platform = QString::fromStdString(CubesUnitTypes::platform_names_[0]);


	QSharedPointer<FileItem> fi(new FileItem(this, editor_, fileId, isMock_));
	fi->SetName(fileName, true, fileName);
	fi->SetPath(QString("config_%1.xml").arg(fileId));
	fi->SetColor(color);

	items_[fileId] = fi;
	selector_->addItem(fileName, fileId);
	selector_->setCurrentIndex(selector_->count() - 1);

	if (filesListChangedDelegate_)
		filesListChangedDelegate_(GetFileNames());

	if (logManager_ != nullptr)
	{
		CubesLog::Message lm{};
		lm.type = CubesLog::MessageType::information;
		lm.code = CubesLog::CreateCode(CubesLog::MessageType::information, CubesLog::SourceType::fileManager,
			static_cast<uint32_t>(MessageId::create));
		lm.source = CubesLog::SourceType::fileManager;
		lm.description = QString("Item created, id = %1, name = %2").arg(fileId).arg(fileName);
		lm.tag = fileId;
		logManager_->AddMessage(lm);
	}
}

void FileItemsManager::Create(const CubesXml::File& xmlFile, CubesUnitTypes::FileId& fileId)
{
	fileId = ++uniqueNumber_;

	QSharedPointer<FileItem> fi(new FileItem(this, editor_, xmlFile, fileId, isMock_));

	items_[fileId] = fi;
	if (selector_ != nullptr)
	{
		selector_->addItem(fi->GetName(), fileId);
		selector_->setCurrentIndex(selector_->count() - 1);
	}

	if (filesListChangedDelegate_)
		filesListChangedDelegate_(GetFileNames());

	if (logManager_ != nullptr)
	{
		CubesLog::Message lm{};
		lm.type = CubesLog::MessageType::information;
		lm.code = CubesLog::CreateCode(CubesLog::MessageType::information, CubesLog::SourceType::fileManager,
			static_cast<uint32_t>(MessageId::create));
		lm.source = CubesLog::SourceType::fileManager;
		lm.description = QString("Item created, id = %1, name = %2").arg(fileId).arg(xmlFile.name);
		lm.tag = fileId;
		logManager_->AddMessage(lm);
	}
}

void FileItemsManager::Select(const CubesUnitTypes::FileId fileId)
{
	if (selector_ != nullptr)
	{
		if (selected_ != fileId)
		{
			if (selected_ != 0)
			{
				GetItem(selected_)->UnSelect();
				selected_ = 0;
				auto pe = editor_->GetPropertyEditor();
				pe->clear();
			}
			if (fileId != 0)
			{
				GetItem(fileId)->Select();
				selected_ = fileId;
			}

			int index = selector_->findData(fileId);
			if (index != -1)
				selector_->setCurrentIndex(index);
		}
	}
}

void FileItemsManager::Remove(const CubesUnitTypes::FileId fileId)
{
	int index = selector_->findData(fileId);
	if (index != -1)
		selector_->removeItem(index);

	items_.remove(fileId);

	if (logManager_ != nullptr)
	{
		CubesLog::Message lm{};
		lm.type = CubesLog::MessageType::information;
		lm.code = CubesLog::CreateCode(CubesLog::MessageType::information, CubesLog::SourceType::fileManager,
			static_cast<uint32_t>(MessageId::remove));
		lm.source = CubesLog::SourceType::fileManager;
		lm.description = QString("Item removed, id = %1").arg(fileId);
		lm.tag = fileId;
		logManager_->AddMessage(lm);
	}
}

QSharedPointer<FileItem> FileItemsManager::GetItem(const CubesUnitTypes::FileId fileId)
{
	auto it = items_.find(fileId);
	if (it != items_.end())
		return *it;
	else
		return nullptr;
}

CubesUnitTypes::FileIdNames FileItemsManager::GetFileNames()
{
	CubesUnitTypes::FileIdNames fileNames;
	for (auto& file : items_)
		fileNames[file->GetFileId()] = file->GetName();
	return fileNames;
}

QColor FileItemsManager::GetFileColor(const CubesUnitTypes::FileId fileId)
{
	const auto it = items_.find(fileId);
	if (it != items_.end())
		return (*it)->GetColor();

	return Qt::white;
}

void FileItemsManager::AddFileInclude(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId includeId,
	const CubesUnitTypes::VariableIdVariables& variables)
{
	const auto it = items_.find(fileId);
	if (it != items_.end())
		(*it)->AddInclude(includeId, variables);
}

QString FileItemsManager::GetFileName(const CubesUnitTypes::FileId fileId)
{
	const auto it = items_.find(fileId);
	if (it != items_.end())
		return (*it)->GetName();

	return "";
}

bool FileItemsManager::GetFileIncludeNames(const CubesUnitTypes::FileId fileId, bool addEmptyValue,
	CubesUnitTypes::IncludeIdNames& includes)
{
	if (addEmptyValue)
		includes[CubesUnitTypes::InvalidIncludeId] = "<not selected>";

	const auto it = items_.find(fileId);
	if (it != items_.end())
		includes.insert((*it)->GetIncludes());

	return true;
}

bool FileItemsManager::GetFileIncludeName(const CubesUnitTypes::FileId fileId,
	const CubesUnitTypes::IncludeId fileIncludeId, QString& includeName)
{
	const auto it = items_.find(fileId);
	if (it == items_.end())
		return false;

	includeName = (*it)->GetIncludeName(fileIncludeId);

	return true;
}

bool FileItemsManager::GetFileIncludePath(const CubesUnitTypes::FileId fileId,
	const CubesUnitTypes::IncludeId fileIncludeId, QString& includePath)
{
	const auto it = items_.find(fileId);
	if (it == items_.end())
		return false;
	
	includePath = (*it)->GetIncludePath(fileIncludeId);

	return true;
}

bool FileItemsManager::GetFileIncludeVariables(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId includeId,
	CubesUnitTypes::VariableIdVariables& variables)
{
	for (auto& fi : items_)
	{
		if (fi->GetFileId() == fileId)
			return fi->GetIncludeVariables(includeId, variables);
	}

	return false;
}

void FileItemsManager::Clear()
{
	if (editor_ != nullptr)
		editor_->GetPropertyEditor()->clear();
	if (selector_ != nullptr)
		selector_->clear();
	items_.clear();

	if (logManager_ != nullptr)
	{
		CubesLog::Message lm{};
		lm.type = CubesLog::MessageType::information;
		lm.code = CubesLog::CreateCode(CubesLog::MessageType::information, CubesLog::SourceType::fileManager,
			static_cast<uint32_t>(MessageId::clear));
		lm.source = CubesLog::SourceType::fileManager;
		lm.description = QString("All items removed");
		lm.tag = CubesUnitTypes::InvalidUniversalId;
		logManager_->AddMessage(lm);
	}
}

QMap<CubesUnitTypes::FileId, QSharedPointer<FileItem>> FileItemsManager::GetItems()
{
	return items_;
}

File FileItemsManager::GetFile(const CubesUnitTypes::FileId fileId)
{
	File result{};

	const auto it = items_.find(fileId);
	if (it != items_.end())
		result = (*it)->GetFile();

	return result;
}

CubesXml::File FileItemsManager::GetXmlFile(const CubesUnitTypes::FileId fileId)
{
	CubesXml::File result{};

	const auto it = items_.find(fileId);
	if (it != items_.end())
		result = (*it)->GetXmlFile();

	return result;
}

bool FileItemsManager::GetAnalysisFiles(QVector<CubesAnalysis::File>& files)
{
	files.clear();

	for (const auto& item : items_)
	{
		const auto itemFiles = item->GetAnalysisFiles();
		for(const auto& file : itemFiles)
			files.push_back(file);
	}

	return true;
}

bool FileItemsManager::GetParameterModels(CubesUnitTypes::PropertiesIdParameterModels& models)
{
	for (const auto& item : items_)
	{
		const auto id = item->GetFileId();
		const auto model = item->GetParameterModels();
		models[id] = model;
	}

	return true;
}

void FileItemsManager::AfterFileNameChanged(const CubesUnitTypes::FileId fileId)
{
	auto item = GetItem(fileId);
	QString fileName = item->GetName();

	// Переименовываем в comboBox
	int index = selector_->findData(fileId);
	selector_->setItemText(index, fileName);

	// Переименовываем имя выбранного файла
	selected_ = fileId;

	// Уведомляем о переименовании
	if (fileNameChangedDelegate_)
		fileNameChangedDelegate_(fileId);
}

void FileItemsManager::AfterIncludeNameChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::FileId includeId)
{
	if (includeNameChangedDelegate_)
		includeNameChangedDelegate_(fileId, includeId);
}

void FileItemsManager::BeforeIncludesAdd(const CubesUnitTypes::FileId fileId,
	const CubesUnitTypes::IncludeIdNames& includeNames, bool& cancel)
{
	// Ничего не делаем
	cancel = false;
}

void FileItemsManager::BeforeIncludesRemoved(const CubesUnitTypes::FileId fileId,
	const CubesUnitTypes::IncludeIdNames& includeNames, bool& cancel)
{
	QSet<QString> allUnitNames;
	for (const auto& includeId : includeNames.keys())
	{
		QStringList unitNames;
		topManager_->GetUnitsInFileIncludeList(fileId, includeId, unitNames);
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

void FileItemsManager::AfterIncludesListChanged(const CubesUnitTypes::FileId fileId,
	const CubesUnitTypes::IncludeIdNames& includeNames)
{
	CubesUnitTypes::IncludeIdNames includes;
	includes[CubesUnitTypes::InvalidIncludeId] = "<not selected>";
	includes.insert(includeNames);
	if (includesListChangedDelegate_)
		includesListChangedDelegate_(fileId, includes);
}

void FileItemsManager::AfterVariableNameChanged(const CubesUnitTypes::FileId fileId,
	const CubesUnitTypes::IncludeId includeId, const QString& variableName, const QString& oldVariableName)
{
	if (variableNameChangedDelegate_)
		variableNameChangedDelegate_(fileId, includeId, variableName, oldVariableName);
}

void FileItemsManager::AfterVariablesListChanged(const CubesUnitTypes::FileId fileId,
	const CubesUnitTypes::IncludeId includeId, const CubesUnitTypes::VariableIdVariables& variables)
{
	if (variablesListChangedDelegate_)
		variablesListChangedDelegate_(fileId, includeId, variables);
}

void FileItemsManager::AfterColorChanged(const CubesUnitTypes::FileId fileId, const QColor& color)
{
	if (colorChangedDelegate_)
		colorChangedDelegate_(fileId, color);
}

void FileItemsManager::AfterPropertiesChanged()
{
	if (propertiesChangedDelegate_)
		propertiesChangedDelegate_();
}

void FileItemsManager::OnEditorCollapsed(QtBrowserItem* item)
{
	CubesUnitTypes::PropertiesId propertiesId = GetCurrentFileId();
	SetPropertyExpanded(propertiesId, item->property(), false);
}

void FileItemsManager::OnEditorExpanded(QtBrowserItem* item)
{
	CubesUnitTypes::PropertiesId propertiesId = GetCurrentFileId();
	SetPropertyExpanded(propertiesId, item->property(), true);
}

void FileItemsManager::OnContextMenuRequested(const QPoint& pos)
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
		connect(&action1, &QAction::triggered, this, &FileItemsManager::OnDeleteInclude);
		contextMenu.addAction(&action1);

		contextMenu.exec(pe->mapToGlobal(pos));
	}
}

void FileItemsManager::OnDeleteInclude(bool checked)
{

}

void FileItemsManager::OnSelectorIndexChanged(int index)
{
	uint32_t currentId = GetCurrentFileId();
	Select(currentId);
}

void FileItemsManager::OnAddFileClicked()
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
	CubesUnitTypes::FileId fileId{ CubesUnitTypes::InvalidFileId };
	Create(QString::fromLocal8Bit("config_%1.xml"), fileName, QString::fromStdString(CubesUnitTypes::platform_names_[0]), fileId);
}

void FileItemsManager::OnRemoveFileClicked()
{
	if (selected_ == 0)
	{
		QMessageBox::critical(widget_, "Error", QString::fromLocal8Bit("Файл не выбран!"));
		return;
	}

	auto fileId = GetCurrentFileId();

	// Проверяем возможность удаления
	QStringList unitNames;
	topManager_->GetUnitsInFileList(fileId, unitNames);
	if (unitNames.count() > 0)
	{
		QString text = QString::fromLocal8Bit("Имя используется.\nУдаление невозможно!\nЮниты:\n");
		text.append(unitNames.join('\n'));
		QMessageBox::critical(widget_, "Error", text);
		return;
	}

	// Сохраняем копию, после удаления selected_ изменится
	uint32_t selected = selected_;

	// Удаляем из селектора, автоматически происходит UnSelect
	selector_->removeItem(selector_->findData(selected_));

	// Удаляем из списка
	items_.remove(selected);

	// Если это был последний
	if (items_.count() == 0)
		editor_->GetPropertyEditor()->clear();

	// Получаем все имена
	CubesUnitTypes::FileIdNames fileNames;
	for (const auto& item : items_)
		fileNames[item->GetFileId()] = item->GetName();

	// Сообщаяем об удалении
	if (filesListChangedDelegate_)
		filesListChangedDelegate_(fileNames);

	if (logManager_ != nullptr)
	{
		CubesLog::Message lm{};
		lm.type = CubesLog::MessageType::information;
		lm.code = CubesLog::CreateCode(CubesLog::MessageType::information, CubesLog::SourceType::fileManager,
			static_cast<uint32_t>(MessageId::remove));
		lm.source = CubesLog::SourceType::fileManager;
		lm.description = QString("Item removed, id = %1").arg(fileId);
		lm.tag = fileId;
		logManager_->AddMessage(lm);
	}
}

QWidget* FileItemsManager::CreateEditorWidget()
{
	editor_ = new PropertiesEditor();
	qDebug() << connect(editor_, &PropertiesEditor::Collapsed, this, &FileItemsManager::OnEditorCollapsed);
	qDebug() << connect(editor_, &PropertiesEditor::Expanded, this, &FileItemsManager::OnEditorExpanded);
	qDebug() << connect(editor_, &PropertiesEditor::ContextMenuRequested, this, &FileItemsManager::OnContextMenuRequested);

	QWidget* propertiesPanelWidget = new QWidget;

	QWidget* hostsButtonsWidget = CreateSelectorWidget();

	QVBoxLayout* propertiesPaneLayout = new QVBoxLayout;
	propertiesPaneLayout->addWidget(hostsButtonsWidget);
	propertiesPaneLayout->addWidget(editor_->GetPropertyEditor());
	propertiesPaneLayout->setContentsMargins(0, 0, 0, 0);

	propertiesPanelWidget->setLayout(propertiesPaneLayout);

	return propertiesPanelWidget;
}

QWidget* FileItemsManager::CreateSelectorWidget()
{
	selector_ = new QComboBox();
	qDebug() << connect(selector_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &FileItemsManager::OnSelectorIndexChanged);

	QHBoxLayout* hBoxLayoutPropertyListButtons = new QHBoxLayout;
	hBoxLayoutPropertyListButtons->setMargin(0);
	hBoxLayoutPropertyListButtons->setContentsMargins(0, 0, 0, 0);

	QToolButton* toolButtonPropertyListAdd = new QToolButton;
	toolButtonPropertyListAdd->setFixedSize(24, 24);
	toolButtonPropertyListAdd->setIconSize(QSize(24, 24));
	toolButtonPropertyListAdd->setIcon(QIcon(":/images/plus.png"));
	toolButtonPropertyListAdd->setToolTip(QString::fromLocal8Bit("Добавить хост"));
	hBoxLayoutPropertyListButtons->addWidget(toolButtonPropertyListAdd);
	connect(toolButtonPropertyListAdd, &QToolButton::clicked, this, &FileItemsManager::OnAddFileClicked);

	QToolButton* toolButtonPropertyListRemove = new QToolButton;
	toolButtonPropertyListRemove->setFixedSize(24, 24);
	toolButtonPropertyListRemove->setIconSize(QSize(24, 24));
	toolButtonPropertyListRemove->setIcon(QIcon(":/images/minus.png"));
	toolButtonPropertyListRemove->setToolTip(QString::fromLocal8Bit("Удалить хост"));
	hBoxLayoutPropertyListButtons->addWidget(toolButtonPropertyListRemove);
	connect(toolButtonPropertyListRemove, &QToolButton::clicked, this, &FileItemsManager::OnRemoveFileClicked);

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

void FileItemsManager::SetPropertyExpanded(const CubesUnitTypes::FileId fileId, const QtProperty* property, bool is_expanded)
{
	auto it = items_.find(fileId);
	if (it != items_.end())
		(*it)->ExpandedChanged(property, is_expanded);
}
