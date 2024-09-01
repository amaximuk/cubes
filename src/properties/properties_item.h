#pragma once

#include <QObject>
#include <QString>
#include "properties_items_manager_interface.h"
#include "../unit/unit_types.h"
#include "../analysis/analysis_types.h"
#include "../xml/xml_parser.h"
#include "../property_browser/properties_editor.h"
#include "../diagram/diagram_item_types.h"

namespace CubesLog { class ILogManager; }
namespace CubesLog { class LogHelper; }

namespace CubesProperties
{
    class PropertiesItem : public QObject
    {
        Q_OBJECT

    private:
        // Params
        IPropertiesItemsManager* propertiesItemsManager_;
        CubesLog::ILogManager* logManager_;
        QPointer<PropertiesEditor> editor_;
        CubesUnit::PropertiesId propertiesId_;
        CubesUnit::UnitParametersPtr unitParametersPtr_;

        // ћодель параметров
        CubesUnit::ParameterModelPtrs parameterModelPtrs_;

        // —войства верхнего уровн€
        QList<QtProperty*> topLevelProperties_;

        // —оответствие между свойствами и id модели
        QMap<const QtProperty*, CubesUnit::ParameterModelId> propertyToId_;
        QMap<CubesUnit::ParameterModelId, const QtProperty*> idToProperty_;

        // ѕри добавлении свойства приходит событие, что оно развернуто, надо его игнорировать
        bool ignoreEvents_;

        // «начени€ имен параметров
        CubesUnit::ParameterModelIds ids_;

        // Log
        QSharedPointer<CubesLog::LogHelper> logHelper_;

    public:
        PropertiesItem(IPropertiesItemsManager* propertiesItemsManager, CubesLog::ILogManager* logManager, PropertiesEditor* editor,
            CubesUnit::UnitParametersPtr unitParametersPtr, bool isArrayUnit, CubesUnit::PropertiesId propertiesId);
        PropertiesItem(IPropertiesItemsManager* propertiesItemsManager, CubesLog::ILogManager* logManager, PropertiesEditor* editor,
            CubesUnit::UnitParametersPtr unitParametersPtr, CubesUnit::PropertiesId propertiesId, CubesUnit::ParameterModelPtrs parameterModelPtrs);
        PropertiesItem(IPropertiesItemsManager* propertiesItemsManager, CubesLog::ILogManager* logManager, PropertiesEditor* editor,
            CubesUnit::UnitParametersPtr unitParametersPtr, const CubesXml::Unit& xmlUnit, bool isArrayUnit, CubesUnit::PropertiesId propertiesId);

    public:
        uint32_t GetPropertiesId() const;

        void Select();
        void UnSelect();
        void ExpandedChanged(const QtProperty* property, bool is_expanded);







        void SetName(QString name);
        QString GetName();

        QImage GetPixmap();

        void SetPosition(QPointF point);
        void SetSize(QSizeF size);
        QPointF GetPosition();
        void SetZOrder(double value);
        double GetZOrder();
        QSizeF GetSize();


        void SetFileIdNames(CubesUnit::FileIdNames fileNames);
        void SetFileIdName(CubesUnit::FileId fileId, QString fileName);
        CubesUnit::FileId GetFileId();
        QString GetFileName();

        void SetIncludeIdNames(CubesUnit::IncludeIdNames includeNames);
        void SetIncludeIdName(CubesUnit::IncludeId includeId, QString includeName);
        CubesUnit::IncludeId GetIncludeId();
        QString GetIncludeName();

        // TODO: Ќадо от них избавитьс€
        CubesUnit::UnitParametersPtr GetUnitParametersPtr() { return unitParametersPtr_; };
        QString GetUnitId() { return QString::fromStdString(unitParametersPtr_->fileInfo.info.id); };
        QString GetUnitCategory() { return QString::fromStdString(unitParametersPtr_->fileInfo.info.category); };
        CubesDiagram::ItemType GetItemType();

        QString GetPropertyDescription(QtProperty* property);
        QList<QString> GetConnectedNames();
        QList<QString> GetDependentNames();

        QString GetText();
        bool GetShowBorder();
        CubesDiagram::HorizontalAlignment GetHorizontalAlignment();
        CubesDiagram::VerticalAlignment GetVerticalAlignment();


        void GetXml(CubesXml::Unit& xmlUnit);
        void RemoveItems(const CubesUnit::ParameterModelId& id);
        void AddItems(CubesUnit::ParameterModelPtr model);
        CubesAnalysis::Properties GetAnalysisProperties();
        CubesUnit::ParameterModelPtrs GetParameterModelPtrs();

    private:
        // TODO: move to private
        bool GetXmlParam(CubesUnit::ParameterModelPtr pm, CubesXml::Param& param);
        bool GetXmlArrray(CubesUnit::ParameterModelPtr pm, CubesXml::Array& array);

    private slots:
        void ValueChanged(QtProperty* property, const QVariant& value);
        void StringEditingFinished(QtProperty* property, const QString& value, const QString& oldValue);

    private:
        void CreateParametersModel(const CubesXml::Unit* xmlUnit, bool isArrayUnit);
        void CreateProperties();
        void CreateParameterModel(const CubesUnit::ParameterInfoId& parameterInfoId,
            const CubesUnit::ParameterModelId& parentModelId, const CubesXml::Unit* xmlUnit,
            CubesUnit::ParameterModelPtr& model);
        void FillParameterModel(const CubesXml::Unit* xmlUnit, CubesUnit::ParameterModelPtr model, bool isItem);
        void FillArrayModel(const CubesXml::Unit* xmlUnit, CubesUnit::ParameterModelPtr model);
        void UpdateArrayModel(const CubesXml::Unit* xmlUnit, CubesUnit::ParameterModelPtr model);

        void GetConnectedNamesInternal(CubesUnit::ParameterModelPtr model, QList<QString>& list);
        void GetDependentNamesInternal(CubesUnit::ParameterModelPtr model, QList<QString>& list);
        void GetAnalysisPropertiesInternal(CubesUnit::ParameterModelPtr model, QVector<CubesAnalysis::UnitProperty>& list);

        void RegisterProperty(const QtProperty* property, const CubesUnit::ParameterModelId& id);
        void UnregisterProperty(const CubesUnit::ParameterModelId& id);
        void UnregisterProperty(const QtProperty* property);
        QtProperty* GetProperty(const CubesUnit::ParameterModelId& id);
        CubesUnit::ParameterModelId GetPropertyId(const QtProperty* property);
        //CubesUnit::ParameterModel* GetParameterModel(const CubesUnit::ParameterModelId& id);
        //CubesUnit::ParameterModel* GetParameterModel(const QtProperty* property);
        CubesUnit::UnitParameters* GetPropertyUnitParameters(const QtProperty* property);

        void ApplyExpandState();
        void ApplyExpandState(QtBrowserItem* index);

    public:
        CubesUnit::ParameterModelPtr GetParameterModelPtr(const QtProperty* property);

    private:
        bool CheckParametersMatching(const CubesXml::Unit* xmlUnit, const QString& type, const CubesUnit::ParameterModelId& id);
    };
}