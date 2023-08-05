#pragma once

#include <QString>
#include <QPixmap>
#include <QColor>
#include "unit_types.h"
#include "xml_parser.h"

struct properties_for_drawing
{
    QPixmap pixmap;
    QString name;
    QString fileName;
    QString groupName;
    QColor color;
};

class top_manager_interface
{
public:
	virtual void GetUnitsInFileList(const QString& fileName, QStringList& unitNames) = 0;
	virtual void GetUnitsInFileIncludeList(const QString& fileName, const QString& includeName, QStringList& unitNames) = 0;
	virtual void GetUnitParameters(const QString& unitId, unit_types::UnitParameters& unitParameters) = 0;

    virtual void GetFileIncludeList(const QString& fileName, QStringList& includeNames) = 0;
    virtual void GetFileIncludeVariableList(const QString& fileName, const QString& includeName, QList<QPair<QString, QString>>& variables) = 0;
};
