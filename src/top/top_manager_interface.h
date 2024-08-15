#pragma once

#include "../unit/unit_types.h"
#include "../xml/xml_types.h"
#include "../analysis/analysis_types.h"
#include "top_manager_types.h"

namespace CubesTop
{
    class ITopManager
    {
    public:
        virtual ~ITopManager() = default;

    public:
        virtual bool GetUnitsInFileList(CubesUnit::FileId fileId, QStringList& unitNames) = 0;
        virtual bool GetUnitsInFileIncludeList(CubesUnit::FileId fileId,
            CubesUnit::IncludeId includeId, QStringList& unitNames) = 0;
        virtual bool GetUnitParametersPtr(const QString& unitId, CubesUnit::UnitParametersPtr& unitParametersPtr) = 0;

        virtual bool GetFileIncludeList(CubesUnit::FileId fileId, CubesUnit::IncludeIdNames& includeNames) = 0;
        virtual bool GetFileIncludeVariableList(CubesUnit::FileId fileId,
            const CubesUnit::IncludeId includeId, CubesUnit::VariableIdVariables& variables) = 0;

        virtual bool CreatePropetiesItem(const QString& name, uint32_t& propertiesId) = 0;
        virtual bool GetPropetiesForDrawing(CubesUnit::PropertiesId propertiesId, CubesDiagram::PropertiesForDrawing& pfd) = 0;
        virtual bool GetPropetiesUnitParameters(CubesUnit::PropertiesId propertiesId,
            CubesUnit::UnitParametersPtr& unitParametersPtr) = 0;
        virtual bool GetPropetiesUnitId(CubesUnit::PropertiesId propertiesId, QString& unitId) = 0;
        virtual bool GetPropetiesXmlUnit(CubesUnit::PropertiesId propertiesId, CubesXml::Unit& xmlUnit) = 0;

        virtual bool CreateDiagramItem(CubesUnit::PropertiesId propertiesId) = 0;
        virtual bool EnshureVisible(CubesUnit::PropertiesId propertiesId) = 0;

        virtual bool GetUnitsConnections(QMap<QString, QStringList>& connections) = 0;
        virtual bool GetDependsConnections(QMap<QString, QStringList>& connections) = 0;

        virtual bool GetAnalysisFiles(QVector<CubesAnalysis::File>& files) = 0;
        virtual bool GetAnalysisProperties(QVector<CubesAnalysis::Properties>& properties) = 0;

        virtual bool AddUnits(const QList<CubesXml::Unit>& units, QList<CubesUnit::PropertiesId>& addedPropertiesIds) = 0;
        virtual bool UnitsContextMenuRequested(QPoint globalPosition, QList<CubesUnit::PropertiesId>& propertiesIds) = 0;
        virtual bool GetVisibleSceneRect(QRectF& rect) = 0;
    };
}
