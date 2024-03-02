#pragma once

#include <QString>
#include <QRegExp>
#include "../unit_types.h"

namespace CubesProperties
{
    //struct ParameterModelIds
    //{
    //    CubesUnitTypes::ParameterModelId baseGroupName;
    //    CubesUnitTypes::ParameterModelId parametersGroupName;
    //    CubesUnitTypes::ParameterModelId editorGroupName;
    //    CubesUnitTypes::ParameterModelId itemGroupName;
    //    CubesUnitTypes::ParameterModelId dependsParameterName;
    //    CubesUnitTypes::ParameterModelId optionalParameterName;

    //    static const ParameterModelIds& Defaults()
    //    {
    //        static const ParameterModelIds ids {
    //            QString::fromLocal8Bit("$BASE"),
    //            QString::fromLocal8Bit("$PARAMETERS"),
    //            QString::fromLocal8Bit("$EDITOR"),
    //            QString::fromLocal8Bit("$ITEM"),
    //            QString::fromLocal8Bit("$DEPENDS"),
    //            QString::fromLocal8Bit("$OPTIONAL"),
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