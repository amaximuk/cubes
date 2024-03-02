#pragma once

#include <QString>
#include "../unit_types.h"

namespace CubesFile
{
    //struct ParameterModelIds
    //{
    //    CubesUnitTypes::ParameterModelId baseGroupName;
    //    CubesUnitTypes::ParameterModelId parametersGroupName;
    //    CubesUnitTypes::ParameterModelId editorGroupName;
    //    CubesUnitTypes::ParameterModelId itemGroupName;
    //    CubesUnitTypes::ParameterModelId dependsParameterName;
    //    CubesUnitTypes::ParameterModelId optionalParameterName;
    //    CubesUnitTypes::ParameterModelId includesGroupName;
    //    CubesUnitTypes::ParameterModelId variablesGroupName;

    //    static const ParameterModelIds& Defaults()
    //    {
    //        static const ParameterModelIds ids {
    //            QString::fromLocal8Bit("$BASE"),
    //            QString::fromLocal8Bit("$PARAMETERS"),
    //            QString::fromLocal8Bit("$EDITOR"),
    //            QString::fromLocal8Bit("$ITEM"),
    //            QString::fromLocal8Bit("$DEPENDS"),
    //            QString::fromLocal8Bit("$OPTIONAL"),
    //            QString::fromLocal8Bit("$INCLUDES"),
    //            QString::fromLocal8Bit("$VARIABLES")
    //        };
    //        return ids;
    //    }

    //    static const CubesUnitTypes::ParameterModelId GetDefaultItemId(int n)
    //    {
    //        auto itemGroupName = ParameterModelIds::Defaults().itemGroupName;
    //        return QString("%1_%2").arg(itemGroupName.toString()).arg(n);
    //    }
    //};
}