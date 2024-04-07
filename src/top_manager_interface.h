#pragma once

#include "unit_types.h"
#include "xml/xml_parser.h"

struct PropertiesForDrawing
{
    QPixmap pixmap;
    QString name;
    QString fileName;
    QString includeName;
    QColor color;
};

class ITopManager
{
public:
	virtual void GetUnitsInFileList(const CubesUnitTypes::FileId& fileId, QStringList& unitNames) = 0;
	virtual void GetUnitsInFileIncludeList(const CubesUnitTypes::FileId& fileId,
        const CubesUnitTypes::IncludeId includeId, QStringList& unitNames) = 0;
	virtual void GetUnitParameters(const QString& unitId, CubesUnitTypes::UnitParameters& unitParameters) = 0;

    virtual void GetFileIncludeList(const CubesUnitTypes::FileId& fileId, CubesUnitTypes::IncludeIdNames& includeNames) = 0;
    virtual void GetFileIncludeVariableList(const CubesUnitTypes::FileId& fileId,
        const CubesUnitTypes::IncludeId includeId, CubesUnitTypes::VariableIdVariables& variables) = 0;

    virtual bool CreatePropetiesItem(const QString& name, uint32_t& propertiesId) = 0;
    virtual bool GetPropetiesForDrawing(const uint32_t propertiesId, PropertiesForDrawing& pfd) = 0;
    virtual bool GetPropetiesUnitParameters(const uint32_t propertiesId, CubesUnitTypes::UnitParameters& unitParameters) = 0;
    virtual bool GetPropetiesUnitId(const uint32_t propertiesId, QString& unitId) = 0;

    virtual bool CreateDiagramItem(uint32_t propertiesId, const PropertiesForDrawing& pfd, QPointF pos) = 0;
    virtual void EnshureVisible(uint32_t propertiesId) = 0;

    virtual QString GetNewUnitName(const QString& baseName) = 0;
    virtual QMap<QString, QStringList> GetUnitsConnections() = 0;
    virtual QMap<QString, QStringList> GetDependsConnections() = 0;
};
