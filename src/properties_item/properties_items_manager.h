#pragma once

#include "properties_items_manager_interface.h"

class QComboBox;
class QPlainTextEdit;
class ITopManager;

namespace CubesProperties
{
	class PropertiesItem;

	class PropertiesItemsManager : public QObject, IPropertiesItemsManagerBoss, IPropertiesItemsManagerWorker
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
		QPointer<QPlainTextEdit> hint_;
		QMap<uint32_t, QSharedPointer<PropertiesItem>> items_;
		uint32_t selected_;
		uint32_t unique_number_;

	public:
		PropertiesItemsManager(ITopManager* topManager);

	public:
		PropertiesEditor* GetEditor();
		QComboBox* GetSelector();
		QWidget* GetWidget();
		uint32_t GetCurrentPropertiesId();
		void Create(const QString& unitId, uint32_t& propertiesId);
		void Create(const QString& unitId, const CubesUnitTypes::ParametersModel& pm, uint32_t& propertiesId);
		void Create(const CubesXml::Unit& xmlUnit, uint32_t& propertiesId);
		void Select(const uint32_t& propertiesId);
		void Remove(const uint32_t& propertiesId);
		QSharedPointer<PropertiesItem> GetItem(const uint32_t propertiesId);
		bool GetPropetiesForDrawing(const uint32_t propertiesId, PropertiesForDrawing& pfd);
		bool GetUnitParameters(const uint32_t propertiesId, CubesUnitTypes::UnitParameters& unitParameters);
		bool GetUnitId(const uint32_t propertiesId, QString& unitId);

		bool InformVariableChanged();
		void Clear();
		bool GetName(const uint32_t propertiesId, QString& name);

		QList<uint32_t> GetPropertyIdsByFileName(const QString& fileName, const QString& includeFileName = "<not selected>");
		QList<CubesXml::Group> GetXmlGroups(const QString& fileName, const QString& includeFileName = "<not selected>");

	signals:
		void BasePropertiesChanged(const uint32_t propertiesId, const QString& name, const QString& fileName, const QString& groupName);
		void PositionChanged(const uint32_t propertiesId, double posX, double posY, double posZ);
		void SelectedItemChanged(const uint32_t propertiesId);
		void OnError(const uint32_t propertiesId, const QString& message);

	public:
		// IPropertiesItemsManagerBoss (для общения с PropertiesItem)
		void AfterNameChanged(const uint32_t propertiesId) override;
		void AfterFileNameChanged(const uint32_t propertiesId, QStringList& includeNames) override;
		void AfterIncludeNameChanged(const uint32_t propertiesId) override;
		//void AfterIncludeNameChanged(const uint32_t propertiesId, QList<QPair<QString, QString>>& variables) override;
		void AfterPositionChanged(const uint32_t propertiesId, double posX, double posY, double posZ) override;
		void AfterError(const uint32_t propertiesId, const QString& message) override;

		// IPropertiesItemsManagerWorker (для общения с TopManager)
	private:
		void OnEditorCollapsed(QtBrowserItem* item);
		void OnEditorExpanded(QtBrowserItem* item);
		void OnContextMenuRequested(const QPoint& pos);
		void OnCurrentItemChanged(QtBrowserItem* item);
		void OnDeleteInclude(bool checked = false);
		void OnSelectorIndexChanged(int index);
		void OnAddFileClicked();
		void OnRemoveFileClicked();

	private:
		QWidget* CreateEditorWidget();
		QWidget* CreateSelectorWidget();
		QWidget* CreateHintWidget();
		void SetFilePropertyExpanded(const uint32_t propertiesId, const QtProperty* property, bool is_expanded);
		QString GetName(const uint32_t propertiesId);

		void OnArrayWindowBeforeClose(const bool result);
	};
}