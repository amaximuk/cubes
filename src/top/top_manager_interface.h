#pragma once

#include "../unit_types.h"
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
        virtual bool GetUnitsInFileList(const CubesUnitTypes::FileId& fileId, QStringList& unitNames) = 0;
        virtual bool GetUnitsInFileIncludeList(const CubesUnitTypes::FileId& fileId,
            const CubesUnitTypes::IncludeId includeId, QStringList& unitNames) = 0;
        virtual bool GetUnitParameters(const QString& unitId, CubesUnitTypes::UnitParameters& unitParameters) = 0;

        virtual bool GetFileIncludeList(const CubesUnitTypes::FileId& fileId, CubesUnitTypes::IncludeIdNames& includeNames) = 0;
        virtual bool GetFileIncludeVariableList(const CubesUnitTypes::FileId& fileId,
            const CubesUnitTypes::IncludeId includeId, CubesUnitTypes::VariableIdVariables& variables) = 0;

        virtual bool CreatePropetiesItem(const QString& name, uint32_t& propertiesId) = 0;
        virtual bool GetPropetiesForDrawing(const uint32_t propertiesId, PropertiesForDrawing& pfd) = 0;
        virtual bool GetPropetiesUnitParameters(const uint32_t propertiesId, CubesUnitTypes::UnitParameters& unitParameters) = 0;
        virtual bool GetPropetiesUnitId(const uint32_t propertiesId, QString& unitId) = 0;

        virtual bool CreateDiagramItem(uint32_t propertiesId, const PropertiesForDrawing& pfd, QPointF pos) = 0;
        virtual bool EnshureVisible(uint32_t propertiesId) = 0;

        virtual bool GetUnitsConnections(QMap<QString, QStringList>& connections) = 0;
        virtual bool GetDependsConnections(QMap<QString, QStringList>& connections) = 0;

        virtual bool GetAnalysisFiles(QVector<CubesAnalysis::File>& files) = 0;
        virtual bool GetAnalysisProperties(QVector<CubesAnalysis::Properties>& properties) = 0;
    };
}
