#pragma once

#include <QString>
#include <QRegExp>
#include "../unit_types.h"

namespace CubesProperties
{
    struct ParameterModelIds
    {
        CubesUnitTypes::ParameterModelId baseGroupName;
        CubesUnitTypes::ParameterModelId parametersGroupName;
        CubesUnitTypes::ParameterModelId editorGroupName;
        CubesUnitTypes::ParameterModelId itemGroupName;
        CubesUnitTypes::ParameterModelId dependsParameterName;
        CubesUnitTypes::ParameterModelId optionalParameterName;

        static const ParameterModelIds& Defaults()
        {
            static const ParameterModelIds ids {
                QString::fromLocal8Bit("$BASE"),
                QString::fromLocal8Bit("$PARAMETERS"),
                QString::fromLocal8Bit("$EDITOR"),
                QString::fromLocal8Bit("$ITEM"),
                QString::fromLocal8Bit("$DEPENDS"),
                QString::fromLocal8Bit("$OPTIONAL"),
            };
            return ids;
        }
    };
}