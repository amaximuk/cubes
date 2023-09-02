#pragma once

#include "properties_items_manager_interface.h"

class QComboBox;
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
		QMap<uint32_t, QSharedPointer<PropertiesItem>> items_;
		//QList<QSharedPointer<properties_item>> items_;
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
		void Create(const CubesXml::Unit& xmlUnit, uint32_t& propertiesId);
		void Select(const uint32_t& propertiesId);
		void Remove(const uint32_t& propertiesId);
		QSharedPointer<PropertiesItem> GetItem(const uint32_t propertiesId);
		bool GetPropetiesForDrawing(const uint32_t propertiesId, PropertiesForDrawing& pfd);
		bool GetUnitParameters(const uint32_t propertiesId, CubesUnitTypes::UnitParameters& unitParameters);
		bool GetUnitId(const uint32_t propertiesId, QString& unitId);

		bool InformVariableChanged();
		//QColor GetFileColor(const QString& fileName);
		//QStringList GetFileIncludeNames(const QString& fileName);
		//QList<QPair<QString, QString>> GetFileIncludeVariables(const QString& fileName, const QString& includeName);
		void Clear();
		bool GetName(const uint32_t propertiesId, QString& name);

	signals:
		void BasePropertiesChanged(const uint32_t propertiesId, const QString& name, const QString& fileName, const QString& groupName);
		void PositionChanged(const uint32_t propertiesId, double posX, double posY, double posZ);
		void SelectedItemChanged(const uint32_t propertiesId);

	public:
		// IPropertiesItemsManagerBoss (для общения с PropertiesItem)
		void AfterNameChanged(PropertiesItem* item) override;
		void AfterFileNameChanged(PropertiesItem* item, QStringList& includeNames) override;
		void AfterIncludeNameChanged(PropertiesItem* item) override;
		//void AfterIncludeNameChanged(PropertiesItem* item, QList<QPair<QString, QString>>& variables) override;
		void AfterPositionChanged(PropertiesItem* item, double posX, double posY, double posZ) override;

		// IPropertiesItemsManagerWorker (для общения с TopManager)
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
		QString GetName(PropertiesItem* item);
	};
}