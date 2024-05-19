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
        virtual bool GetUnitsInFileList(CubesUnitTypes::FileId fileId, QStringList& unitNames) = 0;
        virtual bool GetUnitsInFileIncludeList(CubesUnitTypes::FileId fileId,
            CubesUnitTypes::IncludeId includeId, QStringList& unitNames) = 0;
        virtual bool GetUnitParameters(const QString& unitId, CubesUnitTypes::UnitParameters& unitParameters) = 0;

        virtual bool GetFileIncludeList(CubesUnitTypes::FileId fileId, CubesUnitTypes::IncludeIdNames& includeNames) = 0;
        virtual bool GetFileIncludeVariableList(CubesUnitTypes::FileId fileId,
            const CubesUnitTypes::IncludeId includeId, CubesUnitTypes::VariableIdVariables& variables) = 0;

        virtual bool CreatePropetiesItem(const QString& name, uint32_t& propertiesId) = 0;
        virtual bool GetPropetiesForDrawing(CubesUnitTypes::PropertiesId propertiesId, PropertiesForDrawing& pfd) = 0;
        virtual bool GetPropetiesUnitParameters(CubesUnitTypes::PropertiesId propertiesId,
            CubesUnitTypes::UnitParameters& unitParameters) = 0;
        virtual bool GetPropetiesUnitId(CubesUnitTypes::PropertiesId propertiesId, QString& unitId) = 0;

        virtual bool CreateDiagramItem(CubesUnitTypes::PropertiesId propertiesId) = 0;
        virtual bool EnshureVisible(CubesUnitTypes::PropertiesId propertiesId) = 0;

        virtual bool GetUnitsConnections(QMap<QString, QStringList>& connections) = 0;
        virtual bool GetDependsConnections(QMap<QString, QStringList>& connections) = 0;

        virtual bool GetAnalysisFiles(QVector<CubesAnalysis::File>& files) = 0;
        virtual bool GetAnalysisProperties(QVector<CubesAnalysis::Properties>& properties) = 0;
    };
}
