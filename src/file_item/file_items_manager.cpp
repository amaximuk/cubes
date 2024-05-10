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
#include "../top_manager_interface.h"
#include "../unit_types.h"
#include "../log_table/log_table_interface.h"
#include "file_item.h"
#include "file_items_manager.h"

using namespace CubesFile;

FileItemsManager::FileItemsManager(ITopManager* topManager, CubesLog::ILogManager* logManager)
{
	topManager_ = topManager;
	logManager_ = logManager;
	selected_ = CubesUnitTypes::InvalidFileId;
	uniqueNumber_ = CubesUnitTypes::InvalidFileId;

	defaultColorFileIndex_ = 0;
	for (auto& c : defaultColorsFile_)
		c.setAlpha(0x20);

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

	//auto item = GetItem(fileName);
	if (fileName == ""/* || item != nullptr*/)
		fileName = QString::fromLocal8Bit("���� %1").arg(fileId);

	auto it = std::find(CubesUnitTypes::platform_names_.cbegin(), CubesUnitTypes::platform_names_.cend(), platform.toStdString());
	if (platform == "" || it == CubesUnitTypes::platform_names_.cend())
		platform = QString::fromStdString(CubesUnitTypes::platform_names_[0]);


	QSharedPointer<FileItem> fi(new FileItem(this, editor_, fileId));
	fi->SetName(fileName, true, fileName);
	fi->SetPath(QString("config_%1.xml").arg(fileId));
	fi->SetColor(color);

	items_[fileId] = fi;
	selector_->addItem(fileName, fileId);
	selector_->setCurrentIndex(selector_->count() - 1);

	emit FilesListChanged(GetFileNames());

	logManager_->AddMessage({ CubesLog::MessageType::information, fileId, "Files Manager",
		QString("Item created, id = %1, name = %2").arg(fileId).arg(fileName) });
}

void FileItemsManager::Create(const CubesXml::File& xmlFile, CubesUnitTypes::FileId& fileId)
{
	fileId = ++uniqueNumber_;

	//auto fileName = xmlFile.name;
	//if (fileName.isEmpty())
	//{
	//	QFileInfo fi(xmlFile.fileName);
	//	fileName = fi.fileName();
	//}
	//auto platform = xmlFile.platform;
	//const auto& filePath = xmlFile.fileName;

	//auto it = std::find(CubesUnitTypes::platform_names_.cbegin(), CubesUnitTypes::platform_names_.cend(), platform.toStdString());
	//if (platform == "" || it == CubesUnitTypes::platform_names_.cend())
	//	platform = QString::fromStdString(CubesUnitTypes::platform_names_[0]);


	QSharedPointer<FileItem> fi(new FileItem(this, editor_, xmlFile, fileId));
	//fi->SetName(fileName, true, fileName);
	//fi->SetPath(filePath);
	//if (/*xmlFile == nullptr || */ xmlFile.color.isEmpty())
	//{
	//	const QColor color = defaultColorFileIndex_ < defaultColorsFile_.size() ?
	//		defaultColorsFile_[defaultColorFileIndex_++] : QColor("White");
	//	fi->SetColor(color);
	//}
	//else
	//{
	//	fi->SetColor();
	//}


	items_[fileId] = fi;
	selector_->addItem(fi->GetName(), fileId);
	selector_->setCurrentIndex(selector_->count() - 1);

	emit FilesListChanged(GetFileNames());

	logManager_->AddMessage({ CubesLog::MessageType::information, fileId, "Files Manager",
		QString("Item created, id = %1, name = %2").arg(fileId).arg(xmlFile.name) });
}

void FileItemsManager::Select(const CubesUnitTypes::FileId fileId)
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

void FileItemsManager::Remove(const CubesUnitTypes::FileId fileId)
{
	int index = selector_->findData(fileId);
	if (index != -1)
		selector_->removeItem(index);

	items_.remove(fileId);

	logManager_->AddMessage({ CubesLog::MessageType::information, fileId, "Files Manager",
		QString("Item removed, id = %1").arg(fileId) });
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

	return QColor("Black");
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

//QString FileItemsManager::GetFileIncludeName(const CubesUnitTypes::FileId fileId, const QString& filePath)
//{
//	const auto it = items_.find(fileId);
//	if (it != items_.end())
//		return (*it)->GetIncludeName(filePath);
//
//	return "";
//}

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
	editor_->GetPropertyEditor()->clear();
	selector_->clear();
	items_.clear();

	logManager_->AddMessage({ CubesLog::MessageType::information, 0, "Files Manager",
		QString("All items removed") });
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

//
//void FileItemsManager::BeforeFileNameChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::FileId oldFileId, bool& cancel)
//{
//	//auto item = GetItem(fileId);
//	//QString fileName = item->GetName();
//
//	//int count = 0;
//	//for (const auto& i : items_)
//	//{
//	//	if (i->GetName() == fileName)
//	//		count++;
//	//}
//	//if (count > 0)
//	//{
//	//	QMessageBox::critical(widget_, "Error", QString::fromLocal8Bit("��� ��� ������������. ��������� �� �����������!"));
//	//	cancel = true;
//	//}
//	//else
//	//	cancel = false;
//}

void FileItemsManager::AfterFileNameChanged(const CubesUnitTypes::FileId fileId)
{
	auto item = GetItem(fileId);
	QString fileName = item->GetName();

	// ��������������� � comboBox
	int index = selector_->findData(fileId);
	selector_->setItemText(index, fileName);

	// ��������������� ��� ���������� �����
	selected_ = fileId;

	//// ��������������� � comboBox
	//for (int i = 0; i < selector_->count(); ++i)
	//{
	//	if (selector_->itemText(i) == oldFileName)
	//	{
	//		selector_->setItemText(i, fileName);
	//		break;
	//	}
	//}

	//// ��������������� ��� ���������� �����
	//// �������� selected_ == oldFileName ���������, �� ������ ������ �������
	//if (selected_ == oldFileName)
	//	selected_ = fileName;

	// ���������� � ��������������
	emit FileNameChanged(fileId);
}

//void FileItemsManager::BeforeIncludeNameChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::FileId includeId,
//	const CubesUnitTypes::FileId oldIncludeId, bool& cancel)
//{
//	// ������ �� ������
//	cancel = false;
//}

void FileItemsManager::AfterIncludeNameChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::FileId includeId)
{
	emit IncludeNameChanged(fileId, includeId);
}

void FileItemsManager::BeforeIncludesAdd(const CubesUnitTypes::FileId fileId,
	const CubesUnitTypes::IncludeIdNames& includeNames, bool& cancel)
{
	// ������ �� ������
	cancel = false;
}

void FileItemsManager::BeforeIncludesRemoved(const CubesUnitTypes::FileId fileId,
	const CubesUnitTypes::IncludeIdNames& includeNames, bool& cancel)
{
	//auto item = GetItem(fileId);
	//QString fileName = item->GetName();

	QSet<QString> allUnitNames;
	for (const auto& includeId : includeNames.keys())
	{
		QStringList unitNames;
		topManager_->GetUnitsInFileIncludeList(fileId, includeId, unitNames);
		allUnitNames.unite(QSet<QString>(unitNames.begin(), unitNames.end()));
	}
	if (allUnitNames.count() > 0)
	{
		QString text = QString::fromLocal8Bit("���� ��� ��������� �� ��������� ���� ������������.\n�������� ����������!\n�����:\n");
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
	emit IncludesListChanged(fileId, includes);
}

void FileItemsManager::AfterVariableNameChanged(const CubesUnitTypes::FileId fileId,
	const CubesUnitTypes::IncludeId includeId, const QString& variableName, const QString& oldVariableName)
{
	emit VariableNameChanged(fileId, includeId, variableName, oldVariableName);
}

void FileItemsManager::AfterVariablesListChanged(const CubesUnitTypes::FileId fileId,
	const CubesUnitTypes::IncludeId includeId, const CubesUnitTypes::VariableIdVariables& variables)
{
	emit VariablesListChanged(fileId, includeId, variables);
}

void FileItemsManager::AfterColorChanged(const CubesUnitTypes::FileId fileId, const QColor& color)
{
	emit ColorChanged(fileId, color);
}

void FileItemsManager::AfterPropertiesChanged()
{
	emit PropertiesChanged();
}

void FileItemsManager::OnEditorCollapsed(QtBrowserItem* item)
{
	uint32_t propertiesId = GetCurrentFileId();
	SetPropertyExpanded(propertiesId, item->property(), false);
}

void FileItemsManager::OnEditorExpanded(QtBrowserItem* item)
{
	uint32_t propertiesId = GetCurrentFileId();
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
	if (parentName == QString::fromLocal8Bit("���������� �����"))
	{
		QMenu contextMenu("Context menu");

		QAction action1(QString::fromLocal8Bit("������� %1").arg(name));
		connect(&action1, &QAction::triggered, this, &FileItemsManager::OnDeleteInclude);
		contextMenu.addAction(&action1);

		contextMenu.exec(pe->mapToGlobal(pos));
	}
}

//void FileItemsManager::OnCurrentItemChanged(QtBrowserItem* item)
//{
//	CubesUnitTypes::FileId FileId = GetCurrentFileId();
//	if (item != nullptr && items_.contains(FileId))
//	{
//		QString description = items_[FileId]->GetPropertyDescription(item->property());
//		hint_->setPlainText(description);
//	}
//	else
//		hint_->setPlainText("");
//}
//
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
	QString fileName = QInputDialog::getText(widget_, QString::fromLocal8Bit("���������� �����"), QString::fromLocal8Bit("��� �����:"), QLineEdit::Normal, "", &ok);
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
		QMessageBox::critical(widget_, "Error", QString::fromLocal8Bit("��� ��� ������������. ��������� �� �����������!"));
		return;
	}

	QColor fileColor = defaultColorFileIndex_ < defaultColorsFile_.size() ?
		defaultColorsFile_[defaultColorFileIndex_++] : QColor("White");
	uint32_t fileId{ 0 };
	Create(QString::fromLocal8Bit("config_%1.xml"), fileName, QString::fromStdString(CubesUnitTypes::platform_names_[0]), fileId);
}

void FileItemsManager::OnRemoveFileClicked()
{
	if (selected_ == 0)
	{
		QMessageBox::critical(widget_, "Error", QString::fromLocal8Bit("���� �� ������!"));
		return;
	}

	auto fileId = GetCurrentFileId();
	//auto item = GetItem(fileId);
	//QString fileName = item->GetName();

	// ��������� ����������� ��������
	QStringList unitNames;
	topManager_->GetUnitsInFileList(fileId, unitNames);
	if (unitNames.count() > 0)
	{
		QString text = QString::fromLocal8Bit("��� ������������.\n�������� ����������!\n�����:\n");
		text.append(unitNames.join('\n'));
		QMessageBox::critical(widget_, "Error", text);
		return;
	}

	// ��������� �����, ����� �������� selected_ ���������
	uint32_t selected = selected_;

	// ������� �� ���������, ������������� ���������� UnSelect
	selector_->removeItem(selector_->findData(selected_));

	// ������� �� ������
	items_.remove(selected);

	// ���� ��� ��� ���������
	if (items_.count() == 0)
		editor_->GetPropertyEditor()->clear();

	// �������� ��� �����
	CubesUnitTypes::FileIdNames fileNames;
	for (const auto& item : items_)
		fileNames[item->GetFileId()] = item->GetName();

	// ��������� �� ��������
	emit FilesListChanged(fileNames);

	logManager_->AddMessage({ CubesLog::MessageType::information, fileId, "Files Manager",
		QString("Item removed, id = %1").arg(fileId) });
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
	toolButtonPropertyListAdd->setToolTip(QString::fromLocal8Bit("�������� ����"));
	hBoxLayoutPropertyListButtons->addWidget(toolButtonPropertyListAdd);
	connect(toolButtonPropertyListAdd, &QToolButton::clicked, this, &FileItemsManager::OnAddFileClicked);

	QToolButton* toolButtonPropertyListRemove = new QToolButton;
	toolButtonPropertyListRemove->setFixedSize(24, 24);
	toolButtonPropertyListRemove->setIconSize(QSize(24, 24));
	toolButtonPropertyListRemove->setIcon(QIcon(":/images/minus.png"));
	toolButtonPropertyListRemove->setToolTip(QString::fromLocal8Bit("������� ����"));
	hBoxLayoutPropertyListButtons->addWidget(toolButtonPropertyListRemove);
	connect(toolButtonPropertyListRemove, &QToolButton::clicked, this, &FileItemsManager::OnRemoveFileClicked);

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

void FileItemsManager::SetPropertyExpanded(const CubesUnitTypes::FileId fileId, const QtProperty* property, bool is_expanded)
{
	auto it = items_.find(fileId);
	if (it != items_.end())
		(*it)->ExpandedChanged(property, is_expanded);
}
