#pragma once

#include <QColor>
#include "../unit/unit_types.h"
#include "../analysis/analysis_types.h"
#include "file_items_manager_interface.h"

namespace CubesTop { class ITopManager; }
namespace CubesLog { class ILogManager; }
namespace CubesLog { class LogHelper; }

class QComboBox;

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
		CubesTop::ITopManager* topManager_;
		CubesLog::ILogManager* logManager_;
		bool isMock_;
		QPointer<QWidget> widget_;
		QPointer<PropertiesEditor> editor_;
		QPointer<QComboBox> selector_;
		QMap<CubesUnit::FileId, QSharedPointer<FileItem>> items_;
		CubesUnit::FileId selected_;
		CubesUnit::FileId uniqueNumber_;

		// Log
		QSharedPointer<CubesLog::LogHelper> logHelper_;

	public:
		FileItemsManager(CubesTop::ITopManager* topManager, CubesLog::ILogManager* logManager, bool isMock = false);

	public:
		PropertiesEditor* GetEditor();
		QComboBox* GetSelector();
		QWidget* GetWidget();

		void Create(const QString& filePath, QString& fileName, QString& platform, CubesUnit::FileId& fileId);
		void Create(const CubesXml::File& xmlFile, CubesUnit::FileId& fileId);
		void Select(const CubesUnit::FileId fileId);
		void Remove(const CubesUnit::FileId fileId);
		void Clear();

		QSharedPointer<FileItem> GetItem(const CubesUnit::FileId fileId);

		CubesUnit::FileId GetCurrentFileId();
		CubesUnit::FileId GetFileId(const QString& fileName);
		QString GetCurrentFileName();
		QString GetFileName(const CubesUnit::FileId fileId);

		CubesUnit::FileIdNames GetFileNames();
		QColor GetFileColor(const CubesUnit::FileId fileId);

		void AddFileInclude(const CubesUnit::FileId fileId, const CubesUnit::IncludeId includeId,
			const CubesUnit::VariableIdVariables& variables);

		bool GetFileIncludeNames(const CubesUnit::FileId fileId, bool addEmptyValue, CubesUnit::IncludeIdNames& includes);
		bool GetFileIncludeName(const CubesUnit::FileId fileId, const CubesUnit::IncludeId fileIncludeId, QString& includeName);
		bool GetFileIncludePath(const CubesUnit::FileId fileId, const CubesUnit::IncludeId fileIncludeId, QString& includePath);
		bool GetFileIncludeVariables(const CubesUnit::FileId fileId, const CubesUnit::IncludeId includeId,
			CubesUnit::VariableIdVariables& variables);

		QMap<CubesUnit::FileId, QSharedPointer<FileItem>> GetItems();
		File GetFile(const CubesUnit::FileId fileId);
		CubesXml::File GetXmlFile(const CubesUnit::FileId fileId);
		bool GetAnalysisFiles(QVector<CubesAnalysis::File>& files);
		bool GetParameterModels(CubesUnit::FileIdParameterModels& models);
		bool GetParameterModelsRef(CubesUnit::FileIdParameterModelsRef& models);

	private:
		FileNameChangedDelegate fileNameChangedDelegate_;
		FilesListChangedDelegate filesListChangedDelegate_;
		IncludeNameChangedDelegate includeNameChangedDelegate_;
		IncludesListChangedDelegate includesListChangedDelegate_;
		VariableNameChangedDelegate variableNameChangedDelegate_;
		VariablesListChangedDelegate variablesListChangedDelegate_;
		ColorChangedDelegate colorChangedDelegate_;
		PropertiesChangedDelegate propertiesChangedDelegate_;

	public:
		void SetFileNameChangedDelegate(FileNameChangedDelegate fileNameChangedDelegate) { fileNameChangedDelegate_ = fileNameChangedDelegate; };
		void SetFilesListChangedDelegate(FilesListChangedDelegate filesListChangedDelegate) { filesListChangedDelegate_ = filesListChangedDelegate; };
		void SetIncludeNameChangedDelegate(IncludeNameChangedDelegate includeNameChangedDelegate) { includeNameChangedDelegate_ = includeNameChangedDelegate; };
		void SetIncludesListChangedDelegate(IncludesListChangedDelegate includesListChangedDelegate) { includesListChangedDelegate_ = includesListChangedDelegate; };
		void SetVariableNameChangedDelegate(VariableNameChangedDelegate variableNameChangedDelegate) { variableNameChangedDelegate_ = variableNameChangedDelegate; };
		void SetVariablesListChangedDelegate(VariablesListChangedDelegate variablesListChangedDelegate) { variablesListChangedDelegate_ = variablesListChangedDelegate; };
		void SetColorChangedDelegate(ColorChangedDelegate colorChangedDelegate) { colorChangedDelegate_ = colorChangedDelegate; };
		void SetPropertiesChangedDelegate(PropertiesChangedDelegate propertiesChangedDelegate) { propertiesChangedDelegate_ = propertiesChangedDelegate; };

	//signals:
	//	void FileNameChanged(const CubesUnit::FileId fileId);
	//	void FilesListChanged(const CubesUnit::FileIdNames& fileNames);
	//	void IncludeNameChanged(const CubesUnit::FileId fileId, const CubesUnit::IncludeId includeId);
	//	void IncludesListChanged(const CubesUnit::FileId fileId, const CubesUnit::IncludeIdNames& includeNames);
	//	void VariableNameChanged(const CubesUnit::FileId fileId, const CubesUnit::IncludeId includeId,
	//		const QString& variableName, const QString& oldVariableName);
	//	void VariablesListChanged(const CubesUnit::FileId fileId, const CubesUnit::IncludeId includeId,
	//		const CubesUnit::VariableIdVariables& variables);
	//	void ColorChanged(const CubesUnit::FileId fileId, const QColor& color);
	//	void PropertiesChanged();

	public:
		// IFileItemsManager (для общения с FileItem)
		//void BeforeFileNameChanged(const CubesUnit::FileId fileId, const CubesUnit::FileId oldFileId, bool& cancel) override;
		void AfterFileNameChanged(const CubesUnit::FileId fileId) override;
		//void BeforeIncludeNameChanged(const CubesUnit::FileId fileId, const CubesUnit::IncludeFileId includeId,
		//	const CubesUnit::IncludeFileId oldIncludeId, bool& cancel) override;
		void AfterIncludeNameChanged(const CubesUnit::FileId fileId, const CubesUnit::IncludeId includeId) override;
		void BeforeIncludesAdd(const CubesUnit::FileId fileId, const CubesUnit::IncludeIdNames& includeNames,
			bool& cancel) override;
		void BeforeIncludesRemoved(const CubesUnit::FileId fileId, const CubesUnit::IncludeIdNames& includeNames,
			bool& cancel) override;
		void AfterIncludesListChanged(const CubesUnit::FileId fileId,
			const CubesUnit::IncludeIdNames& includeNames) override;
		void AfterVariableNameChanged(const CubesUnit::FileId fileId, const CubesUnit::IncludeId includeId,
			const QString& variableName, const QString& oldVariableName) override;
		void AfterVariablesListChanged(const CubesUnit::FileId fileId, const CubesUnit::IncludeId includeId,
			const CubesUnit::VariableIdVariables& variables) override;
		void AfterColorChanged(const CubesUnit::FileId fileId, const QColor& color) override;
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
		void SetPropertyExpanded(const CubesUnit::FileId fileId, const QtProperty* property, bool is_expanded);
	};
}
