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
	selected_ = 0;

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

uint32_t FileItemsManager::GetCurrentFileId()
{
	if (selector_->count() > 0)
		return selector_->itemData(selector_->currentIndex()).toUInt();
	else
		return 0;
}

QString FileItemsManager::GetCurrentFileName()
{
	if (selector_->count() > 0)
		return selector_->currentText();
	else
		return "";
}

uint32_t FileItemsManager::GetFileId(const QString& fileName)
{
	for (auto& fi : items_)
	{
		if (fi->GetName() == fileName)
			return fi->GetFileId();
	}
	return 0;
}

void FileItemsManager::Create(const QString& filePath, QString& fileName, QString& platform, uint32_t& fileId)
{
	const QColor color = defaultColorFileIndex_ < defaultColorsFile_.size() ?
		defaultColorsFile_[defaultColorFileIndex_++] : QColor("White");

	fileId = ++unique_number_;

	//auto item = GetItem(fileName);
	if (fileName == ""/* || item != nullptr*/)
		fileName = QString::fromLocal8Bit("Файл %1").arg(fileId);

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
	const QColor color = defaultColorFileIndex_ < defaultColorsFile_.size() ?
		defaultColorsFile_[defaultColorFileIndex_++] : QColor("White");

	fileId = ++unique_number_;

	auto fileName = xmlFile.name;
	if (fileName.isEmpty())
	{
		QFileInfo fi(xmlFile.fileName);
		fileName = fi.fileName();
	}
	auto platform = xmlFile.platform;
	auto filePath = xmlFile.fileName;

	auto it = std::find(CubesUnitTypes::platform_names_.cbegin(), CubesUnitTypes::platform_names_.cend(), platform.toStdString());
	if (platform == "" || it == CubesUnitTypes::platform_names_.cend())
		platform = QString::fromStdString(CubesUnitTypes::platform_names_[0]);


	QSharedPointer<FileItem> fi(new FileItem(this, editor_, xmlFile, fileId));
	//fi->SetName(fileName, true, fileName);
	//fi->SetPath(filePath);
	fi->SetColor(color);

	items_[fileId] = fi;
	selector_->addItem(fileName, fileId);
	selector_->setCurrentIndex(selector_->count() - 1);

	emit FilesListChanged(GetFileNames());
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
	auto it = items_.find(fileId);
	if (it != items_.end())
		return *it;
	else
		return nullptr;
}

QStringList FileItemsManager::GetFileNames()
{
	QStringList fileNames;
	for (auto& file : items_)
		fileNames.push_back(file->GetName());
	return fileNames;
}

QColor FileItemsManager::GetFileColor(const uint32_t& fileId)
{
	auto item = GetItem(fileId);
	QString fileName = item->GetName();

	for (auto& fi : items_)
	{
		if (fi->GetName() == fileName)
			return fi->GetColor();
	}
	return QColor("Black");
}

void FileItemsManager::AddFileInclude(const uint32_t& fileId, const QString& includeName, QList<QPair<QString, QString>> includeVariables)
{
	auto item = GetItem(fileId);
	QString fileName = item->GetName();

	for (auto& fi : items_)
	{
		if (fi->GetName() == fileName)
		{
			fi->AddInclude(includeName, includeVariables);
			break;
		}
	}
}

QStringList FileItemsManager::GetFileIncludeNames(const uint32_t& fileId, bool addEmptyValue)
{
	auto item = GetItem(fileId);
	QString fileName = item->GetName();

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

QString FileItemsManager::GetFileIncludeName(const uint32_t& fileId, const QString& filePath)
{
	auto item = GetItem(fileId);
	QString fileName = item->GetName();

	for (auto& fi : items_)
	{
		if (fi->GetName() == fileName)
			return fi->GetIncludeName(filePath);
	}
	return "";
}

QList<QPair<QString, QString>> FileItemsManager::GetFileIncludeVariables(const uint32_t& fileId, const QString& includeName)
{
	QList<QPair<QString, QString>> result;

	auto item = GetItem(fileId);
	QString fileName = item->GetName();

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
	auto fi = GetItem(fileId);
	if (fi == nullptr)
		return false;

	name = fi->GetName();
	return true;
}

File FileItemsManager::GetFile(const uint32_t& fileId)
{
	File result{};

	auto item = GetItem(fileId);
	QString fileName = item->GetName();

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

void FileItemsManager::BeforeFileNameChanged(const uint32_t fileId, const QString& oldFileName, bool& cancel)
{
	auto item = GetItem(fileId);
	QString fileName = item->GetName();

	int count = 0;
	for (const auto& i : items_)
	{
		if (i->GetName() == fileName)
			count++;
	}
	if (count > 0)
	{
		QMessageBox::critical(widget_, "Error", QString::fromLocal8Bit("Имя уже используется. Дубликаты не допускаются!"));
		cancel = true;
	}
	else
		cancel = false;
}

void FileItemsManager::AfterFileNameChanged(const uint32_t fileId, const QString& oldFileName)
{
	auto item = GetItem(fileId);
	QString fileName = item->GetName();

	// Переименовываем в comboBox
	int index = selector_->findData(fileId);
	selector_->setItemText(index, fileName);

	// Переименовываем имя выбранного файла
	selected_ = fileId;

	//// Переименовываем в comboBox
	//for (int i = 0; i < selector_->count(); ++i)
	//{
	//	if (selector_->itemText(i) == oldFileName)
	//	{
	//		selector_->setItemText(i, fileName);
	//		break;
	//	}
	//}

	//// Переименовываем имя выбранного файла
	//// Проверка selected_ == oldFileName избыточна, на всякий случай оставлю
	//if (selected_ == oldFileName)
	//	selected_ = fileName;

	// Уведомляем о переименовании
	emit FileNameChanged(fileName, oldFileName);
}

void FileItemsManager::BeforeIncludeNameChanged(const uint32_t fileId, const QString& includeName, const QString& oldIncludeName, bool& cancel)
{
	// Ничего не делаем
	cancel = false;
}

void FileItemsManager::AfterIncludeNameChanged(const uint32_t fileId, const QString& includeName, const QString& oldIncludeName)
{
	auto item = GetItem(fileId);
	QString fileName = item->GetName();

	emit IncludeNameChanged(fileName, includeName, oldIncludeName);
}

void FileItemsManager::BeforeIncludesAdd(const uint32_t fileId, const QStringList& includeNames, bool& cancel)
{
	// Ничего не делаем
	cancel = false;
}

void FileItemsManager::BeforeIncludesRemoved(const uint32_t fileId, const QStringList& includeNames, bool& cancel)
{
	auto item = GetItem(fileId);
	QString fileName = item->GetName();

	QSet<QString> allUnitNames;
	for (const auto& includeName : includeNames)
	{
		QStringList unitNames;
		topManager_->GetUnitsInFileIncludeList(fileName, includeName, unitNames);
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

void FileItemsManager::AfterIncludesListChanged(const uint32_t fileId, const QStringList& includeNames)
{
	auto item = GetItem(fileId);
	QString fileName = item->GetName();

	QStringList fileIncludeNames;
	fileIncludeNames.push_back("<not selected>");
	fileIncludeNames.append(includeNames);
	emit IncludesListChanged(fileName, fileIncludeNames);
}

void FileItemsManager::AfterVariableNameChanged(const uint32_t fileId, const QString& includeName, const QString& variableName, const QString& oldVariableName)
{
	auto item = GetItem(fileId);
	QString fileName = item->GetName();

	emit VariableNameChanged(fileName, includeName, variableName, oldVariableName);
}

void FileItemsManager::AfterVariablesListChanged(const uint32_t fileId, const QString& includeName, const QList<QPair<QString, QString>>& variables)
{
	auto item = GetItem(fileId);
	QString fileName = item->GetName();

	emit VariablesListChanged(fileName, includeName, variables);
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
	uint32_t fileId{ 0 };
	Create(QString::fromLocal8Bit("config.xml"), fileName, QString::fromStdString(CubesUnitTypes::platform_names_[0]), fileId);
}

void FileItemsManager::OnRemoveFileClicked()
{
	if (selected_ == 0)
	{
		QMessageBox::critical(widget_, "Error", QString::fromLocal8Bit("Файл не выбран!"));
		return;
	}

	auto fileId = GetCurrentFileId();
	auto item = GetItem(fileId);
	QString fileName = item->GetName();

	// Проверяем возможность удаления
	QStringList unitNames;
	topManager_->GetUnitsInFileList(fileName, unitNames);
	if (unitNames.count() > 0)
	{
		QString text = QString::fromLocal8Bit("Имя используется.\nУдаление невозможно!\nЮниты:\n");
		text.append(unitNames.join('\n'));
		QMessageBox::critical(widget_, "Error", text);
		return;
	}

	// Сохраняем копию, после удаления selected_ изменится
	QString selected = selected_;

	// Удаляем из селектора, автоматически происходит UnSelect
	selector_->removeItem(selector_->findData(selected_));

	// Получаем все имена, заодно запоминаем элемент для удаления
	QStringList fileNames;
	uint32_t toRemove;
	for (const auto& item : items_)
	{
		QString name = item->GetName();
		if (name == selected)
			toRemove = item->GetFileId();
		else
			fileNames.push_back(item->GetName());
	}

	// Удаляем из списка
	items_.remove(toRemove);

	// Если это был последний
	if (items_.count() == 0)
		editor_->GetPropertyEditor()->clear();

	// Сообщаяем об удалении
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

void FileItemsManager::SetPropertyExpanded(const uint32_t fileId, const QtProperty* property, bool is_expanded)
{
	auto it = items_.find(fileId);
	if (it != items_.end())
		(*it)->ExpandedChanged(property, is_expanded);
}
