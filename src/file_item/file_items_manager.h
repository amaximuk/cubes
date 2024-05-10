#pragma once

#include <QColor>
#include "../unit_types.h"
#include "../analysis/analysis_types.h"
#include "file_items_manager_interface.h"

class QComboBox;
class ITopManager;
namespace CubesLog
{
	class ILogManager;
}

namespace CubesFile
{
	class FileItemsManager : public QObject, IFileItemsManager
	{
		Q_OBJECT

	private:
		QVector<QColor> defaultColorsFile_ = { QColor("Red"), QColor("Green"), QColor("Blue"),
			QColor("Orange"), QColor("Violet"), QColor("Yellow") };
		int defaultColorFileIndex_;

	private:
		ITopManager* topManager_;
		CubesLog::ILogManager* logManager_;
		QPointer<QWidget> widget_;
		QPointer<PropertiesEditor> editor_;
		QPointer<QComboBox> selector_;
		QMap<CubesUnitTypes::FileId, QSharedPointer<FileItem>> items_;
		CubesUnitTypes::FileId selected_;
		CubesUnitTypes::FileId uniqueNumber_;

	public:
		FileItemsManager(ITopManager* topManager, CubesLog::ILogManager* logManager);

	public:
		PropertiesEditor* GetEditor();
		QComboBox* GetSelector();
		QWidget* GetWidget();

		void Create(const QString& filePath, QString& fileName, QString& platform, CubesUnitTypes::FileId& fileId);
		void Create(const CubesXml::File& xmlFile, CubesUnitTypes::FileId& fileId);
		void Select(const CubesUnitTypes::FileId fileId);
		void Remove(const CubesUnitTypes::FileId fileId);
		void Clear();

		QSharedPointer<FileItem> GetItem(const CubesUnitTypes::FileId fileId);

		CubesUnitTypes::FileId GetCurrentFileId();
		CubesUnitTypes::FileId GetFileId(const QString& fileName);
		QString GetCurrentFileName();
		QString GetFileName(const CubesUnitTypes::FileId fileId);

		CubesUnitTypes::FileIdNames GetFileNames();
		QColor GetFileColor(const CubesUnitTypes::FileId fileId);

		void AddFileInclude(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId includeId,
			const CubesUnitTypes::VariableIdVariables& variables);

		bool GetFileIncludeNames(const CubesUnitTypes::FileId fileId, bool addEmptyValue, CubesUnitTypes::IncludeIdNames& includes);
		bool GetFileIncludeName(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId fileIncludeId, QString& includeName);
		bool GetFileIncludePath(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId fileIncludeId, QString& includePath);
		bool GetFileIncludeVariables(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId includeId,
			CubesUnitTypes::VariableIdVariables& variables);

		File GetFile(const CubesUnitTypes::FileId fileId);
		CubesXml::File GetXmlFile(const CubesUnitTypes::FileId fileId);
		bool GetAnalysisFiles(QVector<CubesAnalysis::File>& files);

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
		void PropertiesChanged();

	public:
		// IFileItemsManager (для общения с FileItem)
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
		void AfterPropertiesChanged() override;

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
