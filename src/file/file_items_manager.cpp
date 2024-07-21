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
#include "../log/log_helper.h"
#include "file_item.h"
#include "file_items_manager.h"

using namespace CubesFile;

FileItemsManager::FileItemsManager(CubesTop::ITopManager* topManager, CubesLog::ILogManager* logManager, bool isMock)
{
	topManager_ = topManager;
	logManager_ = logManager;
	isMock_ = isMock;
	selected_ = CubesUnit::InvalidFileId;
	uniqueNumber_ = CubesUnit::InvalidFileId;

	defaultColorFileIndex_ = 0;
	for (auto& c : defaultColorsFile_)
		c.setAlpha(0x20);

	logHelper_.reset(new CubesLog::LogHelper(logManager_, CubesLog::SourceType::fileManager,
		GetFileManagerErrorDescriptions()));

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

CubesUnit::FileId FileItemsManager::GetCurrentFileId()
{
	if (selector_->count() > 0)
		return selector_->itemData(selector_->currentIndex()).toUInt();
	else
		return CubesUnit::InvalidFileId;
}

QString FileItemsManager::GetCurrentFileName()
{
	if (selector_->count() > 0)
		return selector_->currentText();
	else
		return "";
}

CubesUnit::FileId FileItemsManager::GetFileId(const QString& fileName)
{
	for (auto& fi : items_)
	{
		if (fi->GetName() == fileName)
			return fi->GetFileId();
	}
	return CubesUnit::InvalidFileId;
}

void FileItemsManager::Create(const QString& filePath, QString& fileName, QString& platform, CubesUnit::FileId& fileId)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! filePath unused!!!
	const QColor color = defaultColorFileIndex_ < defaultColorsFile_.size() ?
		defaultColorsFile_[defaultColorFileIndex_++] : QColor("White");

	fileId = ++uniqueNumber_;

	if (fileName == "")
		fileName = QString::fromLocal8Bit("���� %1").arg(fileId);

	auto it = std::find(CubesUnit::platform_names_.cbegin(), CubesUnit::platform_names_.cend(), platform.toStdString());
	if (platform == "" || it == CubesUnit::platform_names_.cend())
		platform = QString::fromStdString(CubesUnit::platform_names_[0]);


	QSharedPointer<FileItem> fi(new FileItem(this, editor_, fileId));
	fi->SetName(fileName, true, fileName);
	fi->SetPath(QString("config_%1.xml").arg(fileId));
	fi->SetColor(color);

	items_[fileId] = fi;
	selector_->addItem(fileName, fileId);
	selector_->setCurrentIndex(selector_->count() - 1);

	if (filesListChangedDelegate_)
		filesListChangedDelegate_(GetFileNames());

	logHelper_->LogInformation(static_cast<CubesLog::BaseErrorCode>(FileManagerErrorCode::itemCreated),
		{ {QString::fromLocal8Bit("���"), fileName} }, fileId);
}

void FileItemsManager::Create(const CubesXml::File& xmlFile, CubesUnit::FileId& fileId)
{
	fileId = ++uniqueNumber_;

	QSharedPointer<FileItem> fi(new FileItem(this, editor_, xmlFile, fileId));

	items_[fileId] = fi;
	if (selector_ != nullptr)
	{
		selector_->addItem(fi->GetName(), fileId);
		selector_->setCurrentIndex(selector_->count() - 1);
	}

	if (filesListChangedDelegate_)
		filesListChangedDelegate_(GetFileNames());

	logHelper_->LogInformation(static_cast<CubesLog::BaseErrorCode>(FileManagerErrorCode::itemCreated),
		{ {QString::fromLocal8Bit("���"), fi->GetName()} }, fileId);
}

void FileItemsManager::Select(const CubesUnit::FileId fileId)
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

void FileItemsManager::Remove(const CubesUnit::FileId fileId)
{
	// �������� ��� �����
	QString name = GetFileName(fileId);

	int index = selector_->findData(fileId);
	if (index != -1)
		selector_->removeItem(index);

	items_.remove(fileId);

	logHelper_->LogInformation(static_cast<CubesLog::BaseErrorCode>(FileManagerErrorCode::itemRemoved),
		{ {QString::fromLocal8Bit("���"), name} }, fileId);
}

QSharedPointer<FileItem> FileItemsManager::GetItem(const CubesUnit::FileId fileId)
{
	auto it = items_.find(fileId);
	if (it != items_.end())
		return *it;
	else
		return nullptr;
}

CubesUnit::FileIdNames FileItemsManager::GetFileNames()
{
	CubesUnit::FileIdNames fileNames;
	for (auto& file : items_)
		fileNames[file->GetFileId()] = file->GetName();
	return fileNames;
}

QColor FileItemsManager::GetFileColor(const CubesUnit::FileId fileId)
{
	const auto it = items_.find(fileId);
	if (it != items_.end())
		return (*it)->GetColor();

	return Qt::white;
}

void FileItemsManager::AddFileInclude(const CubesUnit::FileId fileId, const CubesUnit::IncludeId includeId,
	const CubesUnit::VariableIdVariables& variables)
{
	const auto it = items_.find(fileId);
	if (it != items_.end())
		(*it)->AddInclude(includeId, variables);
}

QString FileItemsManager::GetFileName(const CubesUnit::FileId fileId)
{
	const auto it = items_.find(fileId);
	if (it != items_.end())
		return (*it)->GetName();

	return "";
}

bool FileItemsManager::GetFileIncludeNames(const CubesUnit::FileId fileId, bool addEmptyValue,
	CubesUnit::IncludeIdNames& includes)
{
	if (addEmptyValue)
		includes[CubesUnit::InvalidIncludeId] = "<not selected>";

	const auto it = items_.find(fileId);
	if (it != items_.end())
		includes.insert((*it)->GetIncludes());

	return true;
}

bool FileItemsManager::GetFileIncludeName(const CubesUnit::FileId fileId,
	const CubesUnit::IncludeId fileIncludeId, QString& includeName)
{
	const auto it = items_.find(fileId);
	if (it == items_.end())
		return false;

	includeName = (*it)->GetIncludeName(fileIncludeId);

	return true;
}

bool FileItemsManager::GetFileIncludePath(const CubesUnit::FileId fileId,
	const CubesUnit::IncludeId fileIncludeId, QString& includePath)
{
	const auto it = items_.find(fileId);
	if (it == items_.end())
		return false;
	
	includePath = (*it)->GetIncludePath(fileIncludeId);

	return true;
}

bool FileItemsManager::GetFileIncludeVariables(const CubesUnit::FileId fileId, const CubesUnit::IncludeId includeId,
	CubesUnit::VariableIdVariables& variables)
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

	logHelper_->LogInformation(static_cast<CubesLog::BaseErrorCode>(FileManagerErrorCode::allItemsRemoved));
}

QMap<CubesUnit::FileId, QSharedPointer<FileItem>> FileItemsManager::GetItems()
{
	return items_;
}

File FileItemsManager::GetFile(const CubesUnit::FileId fileId)
{
	File result{};

	const auto it = items_.find(fileId);
	if (it != items_.end())
		result = (*it)->GetFile();

	return result;
}

CubesXml::File FileItemsManager::GetXmlFile(const CubesUnit::FileId fileId)
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

bool FileItemsManager::GetParameterModels(CubesUnit::FileIdParameterModels& models)
{
	for (const auto& item : items_)
	{
		const auto id = item->GetFileId();
		const auto model = item->GetParameterModels();
		models[id] = model;
	}

	return true;
}

bool FileItemsManager::GetParameterModelsRef(CubesUnit::FileIdParameterModelsRef& models)
{
	for (const auto& item : items_)
	{
		const auto id = item->GetFileId();
		const auto& model = item->GetParameterModelsRef();
		models[id] = model;
	}

	return true;
}
void FileItemsManager::AfterFileNameChanged(const CubesUnit::FileId fileId)
{
	auto item = GetItem(fileId);
	QString fileName = item->GetName();

	// ��������������� � comboBox
	int index = selector_->findData(fileId);
	selector_->setItemText(index, fileName);

	// ��������������� ��� ���������� �����
	selected_ = fileId;

	// ���������� � ��������������
	if (fileNameChangedDelegate_)
		fileNameChangedDelegate_(fileId);
}

void FileItemsManager::AfterIncludeNameChanged(const CubesUnit::FileId fileId, const CubesUnit::FileId includeId)
{
	if (includeNameChangedDelegate_)
		includeNameChangedDelegate_(fileId, includeId);
}

void FileItemsManager::BeforeIncludesAdd(const CubesUnit::FileId fileId,
	const CubesUnit::IncludeIdNames& includeNames, bool& cancel)
{
	// ������ �� ������
	cancel = false;
}

void FileItemsManager::BeforeIncludesRemoved(const CubesUnit::FileId fileId,
	const CubesUnit::IncludeIdNames& includeNames, bool& cancel)
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
		QString text = QString::fromLocal8Bit("���� ��� ��������� �� ��������� ���� ������������.\n�������� ����������!\n�����:\n");
		text.append(allUnitNames.values().join('\n'));
		QMessageBox::critical(widget_, "Error", text);
		cancel = true;
	}
	else
		cancel = false;
}

void FileItemsManager::AfterIncludesListChanged(const CubesUnit::FileId fileId,
	const CubesUnit::IncludeIdNames& includeNames)
{
	CubesUnit::IncludeIdNames includes;
	includes[CubesUnit::InvalidIncludeId] = "<not selected>";
	includes.insert(includeNames);
	if (includesListChangedDelegate_)
		includesListChangedDelegate_(fileId, includes);
}

void FileItemsManager::AfterVariableNameChanged(const CubesUnit::FileId fileId,
	const CubesUnit::IncludeId includeId, const QString& variableName, const QString& oldVariableName)
{
	if (variableNameChangedDelegate_)
		variableNameChangedDelegate_(fileId, includeId, variableName, oldVariableName);
}

void FileItemsManager::AfterVariablesListChanged(const CubesUnit::FileId fileId,
	const CubesUnit::IncludeId includeId, const CubesUnit::VariableIdVariables& variables)
{
	if (variablesListChangedDelegate_)
		variablesListChangedDelegate_(fileId, includeId, variables);
}

void FileItemsManager::AfterColorChanged(const CubesUnit::FileId fileId, const QColor& color)
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
	CubesUnit::PropertiesId propertiesId = GetCurrentFileId();
	SetPropertyExpanded(propertiesId, item->property(), false);
}

void FileItemsManager::OnEditorExpanded(QtBrowserItem* item)
{
	CubesUnit::PropertiesId propertiesId = GetCurrentFileId();
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
	CubesUnit::FileId fileId{ CubesUnit::InvalidFileId };
	Create(QString::fromLocal8Bit("config_%1.xml"), fileName, QString::fromStdString(CubesUnit::platform_names_[0]), fileId);
}

void FileItemsManager::OnRemoveFileClicked()
{
	if (selected_ == 0)
	{
		QMessageBox::critical(widget_, "Error", QString::fromLocal8Bit("���� �� ������!"));
		return;
	}

	auto fileId = GetCurrentFileId();

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

	// �������� ��� �����
	QString name = GetFileName(selected_);

	// ������� �� ���������, ������������� ���������� UnSelect
	selector_->removeItem(selector_->findData(selected_));

	// ������� �� ������
	items_.remove(selected);

	// ���� ��� ��� ���������
	if (items_.count() == 0)
		editor_->GetPropertyEditor()->clear();

	// �������� ��� �����
	CubesUnit::FileIdNames fileNames;
	for (const auto& item : items_)
		fileNames[item->GetFileId()] = item->GetName();

	// ��������� �� ��������
	if (filesListChangedDelegate_)
		filesListChangedDelegate_(fileNames);

	logHelper_->LogInformation(static_cast<CubesLog::BaseErrorCode>(FileManagerErrorCode::itemRemoved),
		{ {QString::fromLocal8Bit("���"), name} }, fileId);
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

void FileItemsManager::SetPropertyExpanded(const CubesUnit::FileId fileId, const QtProperty* property, bool is_expanded)
{
	auto it = items_.find(fileId);
	if (it != items_.end())
		(*it)->ExpandedChanged(property, is_expanded);
}
