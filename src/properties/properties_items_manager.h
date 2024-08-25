#pragma once

#include "../top/top_manager_interface.h"
#include "properties_items_manager_interface.h"

namespace CubesTop { class ITopManager; }
namespace CubesLog { class ILogManager; }
namespace CubesLog { class LogHelper; }
namespace CubesProperties { class PropertiesItem; }

class QComboBox;
class QPlainTextEdit;

namespace CubesProperties
{
	class PropertiesItemsManager : public QObject, IPropertiesItemsManager
	{
		Q_OBJECT

	private:
		CubesTop::ITopManager* topManager_;
		CubesLog::ILogManager* logManager_;
		const bool isArray_;
		QPointer<QWidget> widget_;
		QPointer<PropertiesEditor> editor_;
		QPointer<QComboBox> selector_;
		QPointer<QPlainTextEdit> hint_;
		QMap<CubesUnit::PropertiesId, QSharedPointer<PropertiesItem>> items_;
		CubesUnit::PropertiesId selected_;
		CubesUnit::PropertiesId uniqueNumber_;

		// Log
		QSharedPointer<CubesLog::LogHelper> logHelper_;

	public:
		PropertiesItemsManager(CubesTop::ITopManager* topManager, CubesLog::ILogManager* logManager, bool isArray, bool isMock = false);

	public:
		PropertiesEditor* GetEditor();
		QComboBox* GetSelector();
		QWidget* GetWidget();
		uint32_t GetCurrentPropertiesId();
		void Create(const QString& unitId, CubesUnit::PropertiesId& propertiesId);
		void Create(const QString& unitId, CubesUnit::ParameterModelPtrs parameterModelPtrs, CubesUnit::PropertiesId& propertiesId);
		void Create(const CubesXml::Unit& xmlUnit, CubesUnit::PropertiesId& propertiesId);
		void Select(CubesUnit::PropertiesId propertiesId);
		void Remove(CubesUnit::PropertiesId propertiesId);
		QSharedPointer<PropertiesItem> GetItem(CubesUnit::PropertiesId propertiesId);
		bool GetUnitsInFileList(CubesUnit::FileId fileId, QStringList& unitNames);
		bool GetUnitsInFileIncludeList(CubesUnit::FileId fileId,
			CubesUnit::IncludeId includeId, QStringList& unitNames);

		bool GetUnitParameters(CubesUnit::PropertiesId propertiesId, CubesUnit::UnitParametersPtr& unitParametersPtr);
		bool GetUnitId(CubesUnit::PropertiesId propertiesId, QString& unitId);
		bool GetXmlUnit(CubesUnit::PropertiesId propertiesId, CubesXml::Unit& xmlUnit);

		// TODO: переделать на PropertiesId
		bool GetUnitsConnections(QMap<QString, QStringList>& connections);
		bool GetDependsConnections(QMap<QString, QStringList>& connections);
		bool SortUnitsBoost();
		bool SortUnitsRectangular(bool check);

		bool InformVariableChanged();
		bool InformFileNameChanged(CubesUnit::FileId fileId, const QString& fileName);
		bool InformFileListChanged(const CubesUnit::FileIdNames& fileNames);
		bool InformIncludeNameChanged(CubesUnit::FileId fileId, CubesUnit::IncludeId includeId,
			const QString& includeName);
		bool InformIncludesListChanged(CubesUnit::FileId fileId, const CubesUnit::IncludeIdNames& includeNames);
		bool InformFileColorChanged(CubesUnit::FileId fileId);

		void Clear();
		bool GetName(CubesUnit::PropertiesId propertiesId, QString& name);

		QList<uint32_t> GetPropertyIds();
		QList<uint32_t> GetPropertyIdsByFileName(const QString& fileName, const QString& includeName = "<not selected>");
		QList<CubesXml::Group> GetXmlGroups(CubesUnit::FileId fileId,
			CubesUnit::IncludeId includeId = CubesUnit::InvalidIncludeId);
		bool GetAnalysisProperties(QVector<CubesAnalysis::Properties>& properties);

		CubesUnit::PropertiesIdParameterModelPtrs GetPropertiesIdParameterModelPtrs();
		
		//bool GetUnitParameters(CubesUnit::PropertiesIdUnitParameters& unitParameters);

	private:
		BasePropertiesChangedDelegate basePropertiesChangedDelegate_;
		PositionChangedDelegate positionChangedDelegate_;
		SizeChangedDelegate sizeChangedDelegate_;
		SelectedItemChangedDelegate selectedItemChangedDelegate_;
		ErrorDelegate errorDelegate_;
		ConnectionChangedDelegate connectionChangedDelegate_;
		PropertiesChangedDelegate propertiesChangedDelegate_;

	public:
		void SetBasePropertiesChangedDelegate(BasePropertiesChangedDelegate basePropertiesChangedDelegate) { basePropertiesChangedDelegate_ = basePropertiesChangedDelegate; };
		void SetPositionChangedDelegate(PositionChangedDelegate positionChangedDelegate) { positionChangedDelegate_ = positionChangedDelegate; };
		void SetSizeChangedDelegate(SizeChangedDelegate sizeChangedDelegate) { sizeChangedDelegate_ = sizeChangedDelegate; };
		void SetSelectedItemChangedDelegate(SelectedItemChangedDelegate selectedItemChangedDelegate) { selectedItemChangedDelegate_ = selectedItemChangedDelegate; };
		void SetErrorDelegate(ErrorDelegate errorDelegate) { errorDelegate_ = errorDelegate; };
		void SetConnectionChangedDelegate(ConnectionChangedDelegate connectionChangedDelegate) { connectionChangedDelegate_ = connectionChangedDelegate; };
		void SetPropertiesChangedDelegate(PropertiesChangedDelegate propertiesChangedDelegate) { propertiesChangedDelegate_ = propertiesChangedDelegate; };

	//signals:
	//	void BasePropertiesChanged(const CubesUnit::PropertiesId propertiesId, const QString& name,
	//		const CubesUnit::FileId fileId, const CubesUnit::IncludeId includeId);
	//	void PositionChanged(const CubesUnit::PropertiesId propertiesId, double posX, double posY, double posZ);
	//	void SelectedItemChanged(const CubesUnit::PropertiesId propertiesId);
	//	void Error(const CubesUnit::PropertiesId propertiesId, const QString& message);
	//	void ConnectionChanged(const CubesUnit::PropertiesId propertiesId);
	//	void PropertiesChanged();

	public:
		// IPropertiesItemsManager (для общения с PropertiesItem)
		void AfterNameChanged(const CubesUnit::PropertiesId propertiesId) override;
		void AfterFileNameChanged(const CubesUnit::PropertiesId propertiesId, CubesUnit::IncludeIdNames& includeNames) override;
		void AfterIncludeNameChanged(const CubesUnit::PropertiesId propertiesId) override;
		void AfterPositionChanged(const CubesUnit::PropertiesId propertiesId, double posX, double posY, double posZ) override;
		void AfterSizeChanged(CubesUnit::PropertiesId propertiesId, QSizeF size) override;
		void AfterConnectionChanged(const CubesUnit::PropertiesId propertiesId) override;
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
		void SetPropertyExpanded(const CubesUnit::PropertiesId propertiesId, const QtProperty* property, bool is_expanded);
		QString GetName(const CubesUnit::PropertiesId propertiesId);

		void OnArrayWindowBeforeClose(const bool result, CubesUnit::ParameterModelPtr parameterModelPtr,
			QSharedPointer<CubesProperties::PropertiesItem> pi);
	};
}