#pragma once

#include "properties_items_manager_interface.h"

class QComboBox;
class ITopManager;

namespace CubesProperties
{
	class PropertiesItem;

	class PropertiesItemsManager : public QObject, IPropertiesItemsManager
	{
		Q_OBJECT

	private:
		QVector<QColor> defaultColorsFile_ = { QColor("Red"), QColor("Green"), QColor("Blue"), QColor("Orange"), QColor("Violet"), QColor("Yellow") };
		int defaultColorFileIndex_;

	private:
		ITopManager* top_manager_;
		QPointer<QWidget> widget_;
		QPointer<PropertiesEditor> editor_;
		QPointer<QComboBox> selector_;
		QMap<uint32_t, QSharedPointer<PropertiesItem>> items_;
		//QList<QSharedPointer<properties_item>> items_;
		uint32_t selected_;
		uint32_t unique_number_;

	public:
		PropertiesItemsManager(ITopManager* top_manager);

	public:
		PropertiesEditor* GetEditor();
		QComboBox* GetSelector();
		QWidget* GetWidget();
		uint32_t GetCurrentPropertiesId();
		void Create(const QString& unitId, uint32_t& propertiesId);
		void Select(const uint32_t& propertiesId);
		QSharedPointer<PropertiesItem> GetItem(const uint32_t propertiesId);
		QColor GetFileColor(const QString& fileName);
		QStringList GetFileIncludeNames(const QString& fileName);
		QList<QPair<QString, QString>> GetFileIncludeVariables(const QString& fileName, const QString& includeName);
		void Clear();

	signals:
		void BasePropertiesChanged(const uint32_t propertiesId, const QString& name, const QString& fileName, const QString& groupName);
		void SelectedItemChanged(const uint32_t propertiesId);

	public:
		// properties_items_manager_interface
		void AfterNameChanged(PropertiesItem* item) override;
		void AfterFileNameChanged(PropertiesItem* item, QStringList& includeNames) override;
		void AfterIncludeNameChanged(PropertiesItem* item, QList<QPair<QString, QString>>& variables);

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