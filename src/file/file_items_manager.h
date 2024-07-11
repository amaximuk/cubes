#pragma once

#include <QColor>
#include "../unit/unit_types.h"
#include "../analysis/analysis_types.h"
#include "file_items_manager_interface.h"

namespace CubesTop { class ITopManager; }
namespace CubesLog { class ILogManager; }

class QComboBox;

namespace CubesFile
{
	class FileItemsManager : public QObject, IFileItemsManager
	{
		Q_OBJECT

	private:
		QVector<QColor> defaultColorsFile_ = { QColor("Red"), QColor("Green"), QColor("Blue"),
			QColor("Orange"), QColor("Violet"), QColor("Yellow") };
		enum class MessageId
		{
			unknown = 10000,
			create,
			remove,
			clear
		};
		int defaultColorFileIndex_;

	private:
		CubesTop::ITopManager* topManager_;
		CubesLog::ILogManager* logManager_;
		bool isMock_;
		QPointer<QWidget> widget_;
		QPointer<PropertiesEditor> editor_;
		QPointer<QComboBox> selector_;
		QMap<CubesUnitTypes::FileId, QSharedPointer<FileItem>> items_;
		CubesUnitTypes::FileId selected_;
		CubesUnitTypes::FileId uniqueNumber_;

	public:
		FileItemsManager(CubesTop::ITopManager* topManager, CubesLog::ILogManager* logManager, bool isMock = false);

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

		QMap<CubesUnitTypes::FileId, QSharedPointer<FileItem>> GetItems();
		File GetFile(const CubesUnitTypes::FileId fileId);
		CubesXml::File GetXmlFile(const CubesUnitTypes::FileId fileId);
		bool GetAnalysisFiles(QVector<CubesAnalysis::File>& files);
		bool GetParameterModels(CubesUnitTypes::FileIdParameterModels& models);
		bool GetParameterModelsRef(CubesUnitTypes::FileIdParameterModelsRef& models);

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
	//	void FileNameChanged(const CubesUnitTypes::FileId fileId);
	//	void FilesListChanged(const CubesUnitTypes::FileIdNames& fileNames);
	//	void IncludeNameChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId includeId);
	//	void IncludesListChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeIdNames& includeNames);
	//	void VariableNameChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId includeId,
	//		const QString& variableName, const QString& oldVariableName);
	//	void VariablesListChanged(const CubesUnitTypes::FileId fileId, const CubesUnitTypes::IncludeId includeId,
	//		const CubesUnitTypes::VariableIdVariables& variables);
	//	void ColorChanged(const CubesUnitTypes::FileId fileId, const QColor& color);
	//	void PropertiesChanged();

	public:
		// IFileItemsManager (��� ������� � FileItem)
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
