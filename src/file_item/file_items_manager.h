#pragma once

#include "../unit_types.h"
#include "file_items_manager_interface.h"

class QComboBox;
class ITopManager;

namespace CubesFile
{
	class FileItemsManager : public QObject, IFileItemsManagerBoss
	{
		Q_OBJECT

	private:
		QVector<QColor> defaultColorsFile_ = { QColor("Red"), QColor("Green"), QColor("Blue"),
			QColor("Orange"), QColor("Violet"), QColor("Yellow") };
		int defaultColorFileIndex_;

	private:
		ITopManager* topManager_;
		QPointer<QWidget> widget_;
		QPointer<PropertiesEditor> editor_;
		QPointer<QComboBox> selector_;
		QMap<CubesUnitTypes::FileId, QSharedPointer<FileItem>> items_;
		uint32_t selected_;
		uint32_t uniqueNumber_;

	public:
		FileItemsManager(ITopManager* topManager);

	public:
		PropertiesEditor* GetEditor();
		QComboBox* GetSelector();
		QWidget* GetWidget();
		uint32_t GetCurrentFileId();
		QString GetCurrentFileName();
		uint32_t GetFileId(const QString& fileName);
		void Create(const QString& filePath, QString& fileName, QString& platform, uint32_t& fileId);
		void Create(const CubesXml::File& xmlFile, uint32_t& fileId);
		void Select(const CubesUnitTypes::FileId fileId);
		void Remove(const CubesUnitTypes::FileId fileId);
		QSharedPointer<FileItem> GetItem(const CubesUnitTypes::FileId fileId);
		CubesUnitTypes::FileIdNames GetFileNames();
		QColor GetFileColor(const CubesUnitTypes::FileId fileId);
		void AddFileInclude(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeFileId& includeFileId,
			QList<QPair<QString, QString>> includeVariables);
		QString GetFileName(const CubesUnitTypes::FileId fileId);
		CubesUnitTypes::IncludeFileIdNames GetFileIncludeNames(const CubesUnitTypes::FileId fileId, bool addEmptyValue);
		QString GetFileIncludeName(const CubesUnitTypes::FileId fileId, const QString& filePath);
		QString GetFileIncludeName(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeFileId& fileIncludeId);
		QList<QPair<QString, QString>> GetFileIncludeVariables(const CubesUnitTypes::FileId fileId,
			const CubesUnitTypes::IncludeFileId& includeFileId);

		void Clear();
		bool GetName(const CubesUnitTypes::FileId fileId, QString& name);

		File GetFile(const CubesUnitTypes::FileId fileId);
		CubesXml::File GetXmlFile(const QString& fileName);

	signals:
		void FileNameChanged(const CubesUnitTypes::FileId fileId);
		void FilesListChanged(const CubesUnitTypes::FileIdNames& fileNames);
		void IncludeNameChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeFileId includeId);
		void IncludesListChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeFileIdNames& includeNames);
		void VariableNameChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeFileId includeId,
			const QString& variableName, const QString& oldVariableName);
		void VariablesListChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeFileId includeId,
			const QList<QPair<QString, QString>>& variables);
		void ColorChanged(const CubesUnitTypes::FileId fileId, const QColor& color);

	public:
		// IFileItemsManagerBoss (для общения с FileItem)
		//void BeforeFileNameChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::FileId oldFileId, bool& cancel) override;
		void AfterFileNameChanged(const CubesUnitTypes::FileId fileId) override;
		//void BeforeIncludeNameChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeFileId includeId,
		//	const CubesUnitTypes::IncludeFileId oldIncludeId, bool& cancel) override;
		void AfterIncludeNameChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeFileId includeId) override;
		void BeforeIncludesAdd(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeFileIdNames& includeNames,
			bool& cancel) override;
		void BeforeIncludesRemoved(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeFileIdNames& includeNames,
			bool& cancel) override;
		void AfterIncludesListChanged(const CubesUnitTypes::FileId fileId,
			const CubesUnitTypes::IncludeFileIdNames& includeNames) override;
		void AfterVariableNameChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeFileId& includeFileId,
			const QString& variableName, const QString& oldVariableName) override;
		void AfterVariablesListChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeFileId& includeFileId,
			const QList<QPair<QString, QString>>& variables) override;
		void AfterColorChanged(const CubesUnitTypes::FileId fileId, const QColor& color) override;

		// IFileItemsManagerWorker (для общения с TopManager)

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
		void SetPropertyExpanded(const CubesUnitTypes::FileId fileId, const QtProperty* property, bool is_expanded);
	};
}
