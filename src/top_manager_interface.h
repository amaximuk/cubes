#pragma once

#include "unit_types.h"
#include "xml_parser.h"

struct PropertiesForDrawing
{
    QPixmap pixmap;
    QString name;
    QString fileName;
    QString groupName;
    QColor color;
};

class ITopManager
{
public:
	virtual void GetUnitsInFileList(const QString& fileName, QStringList& unitNames) = 0;
	virtual void GetUnitsInFileIncludeList(const QString& fileName, const QString& includeName, QStringList& unitNames) = 0;
	virtual void GetUnitParameters(const QString& unitId, CubesUnitTypes::UnitParameters& unitParameters) = 0;

    virtual void GetFileIncludeList(const QString& fileName, QStringList& includeNames) = 0;
    virtual void GetFileIncludeVariableList(const QString& fileName, const QString& includeName, QList<QPair<QString, QString>>& variables) = 0;

    virtual bool CreatePropetiesItem(const QString& name, uint32_t& propertiesId) = 0;
    virtual bool GetPropeties(const uint32_t propertiesId, PropertiesForDrawing& pfd) = 0;

    virtual QString GetNewUnitName(const QString& baseName) = 0;
    virtual QMap<QString, QStringList> GetUnitsConnections() = 0;
    virtual QMap<QString, QStringList> GetDependsConnections() = 0;
};
