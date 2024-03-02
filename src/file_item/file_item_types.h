#pragma once

#include <QString>

namespace CubesFile
{
    struct ParameterModelIds
    {
        QString baseGroupName;
        QString parametersGroupName;
        QString editorGroupName;
        QString itemGroupName;
        QString dependsParameterName;
        QString optionalParameterName;
        QString includesGroupName;
        QString variablesGroupName;

        static const ParameterModelIds& Defaults()
        {
            static const ParameterModelIds ids {
                QString::fromLocal8Bit("$BASE"),
                QString::fromLocal8Bit("$PARAMETERS"),
                QString::fromLocal8Bit("$EDITOR"),
                QString::fromLocal8Bit("$ITEM"),
                QString::fromLocal8Bit("$DEPENDS"),
                QString::fromLocal8Bit("$OPTIONAL"),
                QString::fromLocal8Bit("$INCLUDES"),
                QString::fromLocal8Bit("$VARIABLES")
            };
            return ids;
        }
    };
}