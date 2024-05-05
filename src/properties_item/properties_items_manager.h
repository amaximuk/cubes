#pragma once

#include "../top_manager_types.h"
#include "properties_items_manager_interface.h"

class QComboBox;
class QPlainTextEdit;
class ITopManager;
namespace CubesLog
{
	class ILogManager;
}

namespace CubesProperties
{
	class PropertiesItem;

	class PropertiesItemsManager : public QObject, IPropertiesItemsManager
	{
		Q_OBJECT

	private:
		QVector<QColor> defaultColorsFile_ = { QColor("Red"), QColor("Green"), QColor("Blue"), QColor("Orange"),
			QColor("Violet"), QColor("Yellow") };
		int defaultColorFileIndex_;

	private:
		ITopManager* topManager_;
		CubesLog::ILogManager* logManager_;
		const bool isArray_;
		QPointer<QWidget> widget_;
		QPointer<PropertiesEditor> editor_;
		QPointer<QComboBox> selector_;
		QPointer<QPlainTextEdit> hint_;
		QMap<CubesUnitTypes::PropertiesId, QSharedPointer<PropertiesItem>> items_;
		CubesUnitTypes::PropertiesId selected_;
		CubesUnitTypes::PropertiesId uniqueNumber_;

	public:
		PropertiesItemsManager(ITopManager* topManager, CubesLog::ILogManager* logManager, bool isArray);

	public:
		PropertiesEditor* GetEditor();
		QComboBox* GetSelector();
		QWidget* GetWidget();
		uint32_t GetCurrentPropertiesId();
		void Create(const QString& unitId, CubesUnitTypes::PropertiesId& propertiesId);
		void Create(const QString& unitId, const CubesUnitTypes::ParametersModel& pm, CubesUnitTypes::PropertiesId& propertiesId);
		void Create(const CubesXml::Unit& xmlUnit, CubesUnitTypes::PropertiesId& propertiesId);
		void Select(const CubesUnitTypes::PropertiesId propertiesId);
		void Remove(const CubesUnitTypes::PropertiesId propertiesId);
		QSharedPointer<PropertiesItem> GetItem(const CubesUnitTypes::PropertiesId propertiesId);
		bool GetUnitsInFileList(const CubesUnitTypes::FileId& fileId, QStringList& unitNames);
		bool GetUnitsInFileIncludeList(const CubesUnitTypes::FileId& fileId,
			const CubesUnitTypes::IncludeId includeId, QStringList& unitNames);

		//bool GetPropetiesForDrawing(const CubesUnitTypes::PropertiesId propertiesId, PropertiesForDrawing& pfd);
		bool GetUnitParameters(const CubesUnitTypes::PropertiesId propertiesId, CubesUnitTypes::UnitParameters& unitParameters);
		bool GetUnitId(const CubesUnitTypes::PropertiesId propertiesId, QString& unitId);

		// TODO: переделать на PropertiesId
		bool GetUnitsConnections(QMap<QString, QStringList>& connections);
		bool GetDependsConnections(QMap<QString, QStringList>& connections);

		bool InformVariableChanged();
		bool InformFileNameChanged(const CubesUnitTypes::FileId& fileId, const QString& fileName);
		bool InformFileListChanged(const CubesUnitTypes::FileIdNames& fileNames);
		bool InformIncludeNameChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeId& includeId,
			const QString& includeName);
		bool InformIncludesListChanged(const CubesUnitTypes::FileId& fileId, const CubesUnitTypes::IncludeIdNames& includeNames);
		//bool InformFileColorChanged(const CubesUnitTypes::FileId& fileId, const QColor& color);

		void Clear();
		bool GetName(const CubesUnitTypes::PropertiesId propertiesId, QString& name);

		QList<uint32_t> GetPropertyIds();
		QList<uint32_t> GetPropertyIdsByFileName(const QString& fileName, const QString& includeName = "<not selected>");
		QList<CubesXml::Group> GetXmlGroups(const CubesUnitTypes::FileId fileId,
			const CubesUnitTypes::IncludeId& includeId = CubesUnitTypes::InvalidIncludeId);
		bool GetAnalysisProperties(QVector<CubesAnalysis::Properties>& properties);

	signals:
		void BasePropertiesChanged(const CubesUnitTypes::PropertiesId propertiesId, const QString& name, const QString& fileName,
			const QString& includeName);
		void PositionChanged(const CubesUnitTypes::PropertiesId propertiesId, double posX, double posY, double posZ);
		void SelectedItemChanged(const CubesUnitTypes::PropertiesId propertiesId);
		void Error(const CubesUnitTypes::PropertiesId propertiesId, const QString& message);
		void ConnectionChanged(const CubesUnitTypes::PropertiesId propertiesId);
		void PropertiesChanged();

	public:
		// IPropertiesItemsManager (для общения с PropertiesItem)
		void AfterNameChanged(const CubesUnitTypes::PropertiesId propertiesId) override;
		void AfterFileNameChanged(const CubesUnitTypes::PropertiesId propertiesId, CubesUnitTypes::IncludeIdNames& includeNames) override;
		void AfterIncludeNameChanged(const CubesUnitTypes::PropertiesId propertiesId) override;
		void AfterPositionChanged(const CubesUnitTypes::PropertiesId propertiesId, double posX, double posY, double posZ) override;
		void AfterError(const CubesUnitTypes::PropertiesId propertiesId, const QString& message) override;
		void AfterConnectionChanged(const CubesUnitTypes::PropertiesId propertiesId) override;
		void AfterPropertiesChanged() override;

	private:
		void OnEditorCollapsed(QtBrowserItem* item);
		void OnEditorExpanded(QtBrowserItem* item);
		void OnContextMenuRequested(const QPoint& pos);
		void OnCurrentItemChanged(QtBrowserItem* item);
		void OnDeleteInclude(bool checked = false);
		void OnSelectorIndexChanged(int index);
		void OnAddUnitClicked();
		void OnAimUnitClicked();

	private:
		QWidget* CreateEditorWidget();
		QWidget* CreateSelectorWidget();
		QWidget* CreateHintWidget();
		void SetPropertyExpanded(const CubesUnitTypes::PropertiesId propertiesId, const QtProperty* property, bool is_expanded);
		QString GetName(const CubesUnitTypes::PropertiesId propertiesId);

		void OnArrayWindowBeforeClose(const bool result, CubesUnitTypes::ParameterModel pm,
			QSharedPointer<CubesProperties::PropertiesItem> pi);
	};
}