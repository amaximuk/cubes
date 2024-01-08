#pragma once

#include "file_items_manager_interface.h"

class QComboBox;
class ITopManager;

namespace CubesFile
{
	class FileItemsManager : public QObject, IFileItemsManager
	{
		Q_OBJECT

	private:
		QVector<QColor> defaultColorsFile_ = { QColor("Red"), QColor("Green"), QColor("Blue"), QColor("Orange"), QColor("Violet"), QColor("Yellow") };
		int defaultColorFileIndex_;

	private:
		ITopManager* topManager_;
		QPointer<QWidget> widget_;
		QPointer<PropertiesEditor> editor_;
		QPointer<QComboBox> selector_;
		QList<QSharedPointer<FileItem>> items_;
		QString selected_;
		uint32_t unique_number_;

	public:
		FileItemsManager(ITopManager* topManager);

	public:
		PropertiesEditor* GetEditor();
		QComboBox* GetSelector();
		QWidget* GetWidget();
		QString GetCurrentFileName();
		void Create(const QString& filePath, QString& fileName, QString& platform);
		void Select(const QString& fileName);
		QSharedPointer<FileItem> GetItem(const QString& fileName);
		QStringList GetFileNames();
		QColor GetFileColor(const QString& fileName);
		void AddFileInclude(const QString& fileName, const QString& includeName, QList<QPair<QString, QString>> includeVariables);
		QStringList GetFileIncludeNames(const QString& fileName, bool addEmptyValue);
		QString GetFileIncludeName(const QString& fileName, const QString& filePath);
		QList<QPair<QString, QString>> GetFileIncludeVariables(const QString& fileName, const QString& includeName);
		void Clear();
		File GetFile(const QString& fileName);
		CubesXml::File GetXmlFile(const QString& fileName);

	signals:
		void FileNameChanged(const QString& fileName, const QString& oldFileName);
		void FilesListChanged(const QStringList& fileNames);
		void IncludeNameChanged(const QString& fileName, const QString& includeName, const QString& oldIncludeName);
		void IncludesListChanged(const QString& fileName, const QStringList& includeNames);
		void VariableNameChanged(const QString& fileName, const QString& includeName, const QString& variableName, const QString& oldVariableName);
		void VariablesListChanged(const QString& fileName, const QString& includeName, const QList<QPair<QString, QString>>& variables);

	public:
		void BeforeFileNameChanged(const QString& fileName, const QString& oldFileName, bool& cancel) override;
		void AfterFileNameChanged(const QString& fileName, const QString& oldFileName) override;
		void BeforeIncludeNameChanged(const QString& fileName, const QString& includeName, const QString& oldIncludeName, bool& cancel) override;
		void AfterIncludeNameChanged(const QString& fileName, const QString& includeName, const QString& oldIncludeName) override;
		void BeforeIncludesAdd(const QString& fileName, const QStringList& includeNames, bool& cancel) override;
		void BeforeIncludesRemoved(const QString& fileName, const QStringList& includeNames, bool& cancel) override;
		void AfterIncludesListChanged(const QString& fileName, const QStringList& includeNames) override;
		void AfterVariableNameChanged(const QString& fileName, const QString& includeName, const QString& variableName, const QString& oldVariableName) override;
		void AfterVariablesListChanged(const QString& fileName, const QString& includeName, const QList<QPair<QString, QString>>& variables) override;

	private:
		void OnEditorCollapsed(QtBrowserItem* item);
		void OnEditorExpanded(QtBrowserItem* item);
		void OnContextMenuRequested(const QPoint& pos);
		void OnDeleteInclude(bool checked = false);
		void OnSelectorIndexChanged(int index);
		void OnAddFileClicked();
		void OnRemoveFileClicked();

	private:
		QWidget* CreateEditorWidget();
		QWidget* CreateSelectorWidget();
		void SetFilePropertyExpanded(const QString& fileName, const QtProperty* property, bool is_expanded);
	};
}
