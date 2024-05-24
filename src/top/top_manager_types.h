#pragma once

#include <QString>
#include <QImage>
#include <QColor>

namespace CubesTop
{
    struct PropertiesForDrawing
    {
        QImage pixmap;
        QString name;
        QString fileName;
        QString includeName;
        QColor color;
    };
}
