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
		CubesUnitTypes::FileId selected_;
		CubesUnitTypes::FileId uniqueNumber_;

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
		void AddFileInclude(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId includeId,
			const CubesUnitTypes::VariableIdVariables& variables);
		QString GetFileName(const CubesUnitTypes::FileId fileId);
		CubesUnitTypes::IncludeIdNames GetFileIncludeNames(const CubesUnitTypes::FileId fileId, bool addEmptyValue);
		QString GetFileIncludeName(const CubesUnitTypes::FileId fileId, const QString& filePath);
		QString GetFileIncludeName(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId& fileIncludeId);
		QString GetFileIncludePath(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId& fileIncludeId);
		CubesUnitTypes::VariableIdVariables GetFileIncludeVariables(const CubesUnitTypes::FileId fileId,
			const CubesUnitTypes::IncludeId includeId);

		void Clear();
		bool GetName(const CubesUnitTypes::FileId fileId, QString& name);

		File GetFile(const CubesUnitTypes::FileId fileId);
		CubesXml::File GetXmlFile(const CubesUnitTypes::FileId fileId);

	signals:
		void FileNameChanged(const CubesUnitTypes::FileId fileId);
		void FilesListChanged(const CubesUnitTypes::FileIdNames& fileNames);
		void IncludeNameChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId includeId);
		void IncludesListChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeIdNames& includeNames);
		void VariableNameChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId includeId,
			const QString& variableName, const QString& oldVariableName);
		void VariablesListChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId includeId,
			const CubesUnitTypes::VariableIdVariables& variables);
		void ColorChanged(const CubesUnitTypes::FileId fileId, const QColor& color);

	public:
		// IFileItemsManagerBoss (��� ������� � FileItem)
		//void BeforeFileNameChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::FileId oldFileId, bool& cancel) override;
		void AfterFileNameChanged(const CubesUnitTypes::FileId fileId) override;
		//void BeforeIncludeNameChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeFileId includeId,
		//	const CubesUnitTypes::IncludeFileId oldIncludeId, bool& cancel) override;
		void AfterIncludeNameChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId includeId) override;
		void BeforeIncludesAdd(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeIdNames& includeNames,
			bool& cancel) override;
		void BeforeIncludesRemoved(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeIdNames& includeNames,
			bool& cancel) override;
		void AfterIncludesListChanged(const CubesUnitTypes::FileId fileId,
			const CubesUnitTypes::IncludeIdNames& includeNames) override;
		void AfterVariableNameChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId includeId,
			const QString& variableName, const QString& oldVariableName) override;
		void AfterVariablesListChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId includeId,
			const CubesUnitTypes::VariableIdVariables& variables) override;
		void AfterColorChanged(const CubesUnitTypes::FileId fileId, const QColor& color) override;

		// IFileItemsManagerWorker (��� ������� � TopManager)

	private:
		void OnEditorCollapsed(QtBrowserItem* item);
		void OnEditorExpanded(QtBrowserItem* item);
		void OnContextMenuRequested(const QPoint& pos);
		//void OnCurrentItemChanged(QtBrowserItem* item);
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
