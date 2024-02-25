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
#include "file_item.h"
#include "file_items_manager.h"

using namespace CubesFile;

FileItemsManager::FileItemsManager(ITopManager* topManager)
{
	topManager_ = topManager;
	unique_number_ = 0;

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

QString FileItemsManager::GetCurrentFileName()
{
	if (selector_->count() > 0)
		return selector_->currentText();
	else
		return "";
}

void FileItemsManager::Create(const QString& filePath, QString& fileName, QString& platform, uint32_t& fileId)
{
	const QColor color = defaultColorFileIndex_ < defaultColorsFile_.size() ?
		defaultColorsFile_[defaultColorFileIndex_++] : QColor("White");

	fileId = ++unique_number_;

	//auto item = GetItem(fileName);
	if (fileName == ""/* || item != nullptr*/)
		fileName = QString::fromLocal8Bit("���� %1").arg(fileId);

	auto it = std::find(CubesUnitTypes::platform_names_.cbegin(), CubesUnitTypes::platform_names_.cend(), platform.toStdString());
	if (platform == "" || it == CubesUnitTypes::platform_names_.cend())
		platform = QString::fromStdString(CubesUnitTypes::platform_names_[0]);


	QSharedPointer<FileItem> fi(new FileItem(this, editor_, fileId));
	fi->SetName(fileName, true, fileName);
	fi->SetPath(filePath);
	fi->SetColor(color);

	items_[fileId] = fi;
	selector_->addItem(fileName, fileId);
	selector_->setCurrentIndex(selector_->count() - 1);

	emit FilesListChanged(GetFileNames());
}

void FileItemsManager::Create(const CubesXml::File& xmlFile, uint32_t& fileId)
{

}

void FileItemsManager::Select(const uint32_t& fileId)
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

void FileItemsManager::Remove(const uint32_t& fileId)
{
	int index = selector_->findData(fileId);
	if (index != -1)
		selector_->removeItem(index);

	items_.remove(fileId);
}

QSharedPointer<FileItem> FileItemsManager::GetItem(const uint32_t& fileId)
{
	if (items_.contains(fileId))
		return items_[fileId];
	return nullptr;
}

QStringList FileItemsManager::GetFileNames()
{
	QStringList fileNames;
	for (auto& file : items_)
		fileNames.push_back(file->GetName());
	return fileNames;
}

QColor FileItemsManager::GetFileColor(const QString& fileName)
{
	for (auto& fi : items_)
	{
		if (fi->GetName() == fileName)
			return fi->GetColor();
	}
	return QColor("Black");
}

void FileItemsManager::AddFileInclude(const QString& fileName, const QString& includeName, QList<QPair<QString, QString>> includeVariables)
{
	for (auto& fi : items_)
	{
		if (fi->GetName() == fileName)
		{
			fi->AddInclude(includeName, includeVariables);
			break;
		}
	}
}

QStringList FileItemsManager::GetFileIncludeNames(const QString& fileName, bool addEmptyValue)
{
	QStringList fileIncludeNames;
	if (addEmptyValue)
		fileIncludeNames.push_back("<not selected>");
	for (auto& fi : items_)
	{
		if (fi->GetName() == fileName)
		{
			fileIncludeNames.append(fi->GetIncludeNames());
			break;
		}
	}
	return fileIncludeNames;
}

QString FileItemsManager::GetFileIncludeName(const QString& fileName, const QString& filePath)
{
	for (auto& fi : items_)
	{
		if (fi->GetName() == fileName)
			return fi->GetIncludeName(filePath);
	}
	return "";
}

QList<QPair<QString, QString>> FileItemsManager::GetFileIncludeVariables(const QString& fileName, const QString& includeName)
{
	QList<QPair<QString, QString>> result;

	QStringList fileIncludeNames;
	for (auto& fi : items_)
	{
		if (fi->GetName() == fileName)
		{
			result = fi->GetIncludeVariables(includeName);
			break;
		}
	}

	return result;
}

void FileItemsManager::Clear()
{
	editor_->GetPropertyEditor()->clear();
	selector_->clear();
	items_.clear();
}

bool FileItemsManager::GetName(const uint32_t fileId, QString& name)
{
	auto pi = GetItem(fileId);
	if (pi == nullptr)
		return false;

	name = GetName(pi.get());
	return true;
}

File FileItemsManager::GetFile(const QString& fileName)
{
	File result{};

	for (auto& fi : items_)
	{
		if (fi->GetName() == fileName)
		{
			result = fi->GetFile();
			break;
		}
	}

	return result;
}

CubesXml::File FileItemsManager::GetXmlFile(const QString& fileName)
{
	CubesXml::File result{};

	for (auto& fi : items_)
	{
		if (fi->GetName() == fileName)
		{
			result = fi->GetXmlFile();
			break;
		}
	}

	return result;
}

void FileItemsManager::BeforeFileNameChanged(const QString& fileName, const QString& oldFileName, bool& cancel)
{
	int count = 0;
	for (const auto& i : items_)
	{
		if (i->GetName() == fileName)
			count++;
	}
	if (count > 0)
	{
		QMessageBox::critical(widget_, "Error", QString::fromLocal8Bit("��� ��� ������������. ��������� �� �����������!"));
		cancel = true;
	}
	else
		cancel = false;
}

void FileItemsManager::AfterFileNameChanged(const QString& fileName, const QString& oldFileName)
{
	// ��������������� � comboBox
	for (int i = 0; i < selector_->count(); ++i)
	{
		if (selector_->itemText(i) == oldFileName)
		{
			selector_->setItemText(i, fileName);
			break;
		}
	}

	// ��������������� ��� ���������� �����
	// �������� selected_ == oldFileName ���������, �� ������ ������ �������
	if (selected_ == oldFileName)
		selected_ = fileName;

	// ���������� � ��������������
	emit FileNameChanged(fileName, oldFileName);
}

void FileItemsManager::BeforeIncludeNameChanged(const QString& fileName, const QString& includeName, const QString& oldIncludeName, bool& cancel)
{
	// ������ �� ������
	cancel = false;
}

void FileItemsManager::AfterIncludeNameChanged(const QString& fileName, const QString& includeName, const QString& oldIncludeName)
{
	emit IncludeNameChanged(fileName, includeName, oldIncludeName);
}

void FileItemsManager::BeforeIncludesAdd(const QString& fileName, const QStringList& includeNames, bool& cancel)
{
	// ������ �� ������
	cancel = false;
}

void FileItemsManager::BeforeIncludesRemoved(const QString& fileName, const QStringList& includeNames, bool& cancel)
{
	QSet<QString> allUnitNames;
	for (const auto& includeName : includeNames)
	{
		QStringList unitNames;
		topManager_->GetUnitsInFileIncludeList(fileName, includeName, unitNames);
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

void FileItemsManager::AfterIncludesListChanged(const QString& fileName, const QStringList& includeNames)
{
	QStringList fileIncludeNames;
	fileIncludeNames.push_back("<not selected>");
	fileIncludeNames.append(includeNames);
	emit IncludesListChanged(fileName, fileIncludeNames);
}

void FileItemsManager::AfterVariableNameChanged(const QString& fileName, const QString& includeName, const QString& variableName, const QString& oldVariableName)
{
	emit VariableNameChanged(fileName, includeName, variableName, oldVariableName);
}

void FileItemsManager::AfterVariablesListChanged(const QString& fileName, const QString& includeName, const QList<QPair<QString, QString>>& variables)
{
	emit VariablesListChanged(fileName, includeName, variables);
}

void FileItemsManager::OnEditorCollapsed(QtBrowserItem* item)
{
	QString currentFileName = GetCurrentFileName();
	SetFilePropertyExpanded(currentFileName, item->property(), false);
}

void FileItemsManager::OnEditorExpanded(QtBrowserItem* item)
{
	QString currentFileName = GetCurrentFileName();
	SetFilePropertyExpanded(currentFileName, item->property(), true);
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
	QString currentFileName = GetCurrentFileName();
	Select(currentFileName);
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
	Create(QString::fromLocal8Bit("config.xml"), fileName, QString::fromStdString(CubesUnitTypes::platform_names_[0]), fileId);
}

void FileItemsManager::OnRemoveFileClicked()
{
	if (selected_.isEmpty())
	{
		QMessageBox::critical(widget_, "Error", QString::fromLocal8Bit("���� �� ������!"));
		return;
	}

	// ��������� ����������� ��������
	QStringList unitNames;
	topManager_->GetUnitsInFileList(selected_, unitNames);
	if (unitNames.count() > 0)
	{
		QString text = QString::fromLocal8Bit("��� ������������.\n�������� ����������!\n�����:\n");
		text.append(unitNames.join('\n'));
		QMessageBox::critical(widget_, "Error", text);
		return;
	}

	// ��������� �����, ����� �������� selected_ ���������
	QString selected = selected_;

	// ������� �� ���������, ������������� ���������� UnSelect
	selector_->removeItem(selector_->findText(selected_));

	// �������� ��� �����, ������ ���������� ������� ��� ��������
	QStringList fileNames;
	QSharedPointer<FileItem> toRemove;
	for (const auto& item : items_)
	{
		QString name = item->GetName();
		if (name == selected)
			toRemove = item;
		else
			fileNames.push_back(item->GetName());
	}

	// ������� �� ������
	items_.removeAll(toRemove);

	// ���� ��� ��� ���������
	if (items_.count() == 0)
		editor_->GetPropertyEditor()->clear();

	// ��������� �� ��������
	emit FilesListChanged(fileNames);
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

void FileItemsManager::SetFilePropertyExpanded(const QString& fileName, const QtProperty* property, bool is_expanded)
{
	QStringList fileIncludeNames;
	for (auto& fi : items_)
	{
		if (fi->GetName() == fileName)
			fi->ExpandedChanged(property, is_expanded);
	}
}

QString FileItemsManager::GetName(FileItem* item)
{
	return{};

	//QList<QPair<QString, QString>> variables;
	//topManager_->GetFileIncludeVariableList(item->GetFileName(), item->GetIncludeName(), variables);

	//QString name = item->GetName();
	//for (const auto& v : variables)
	//{
	//	QString replace = QString("@%1@").arg(v.first);
	//	name.replace(replace, v.second);
	//}

	//return name;
}
