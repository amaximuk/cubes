#pragma once

#include <QString>
#include <QPixmap>
#include <QColor>

namespace CubesTop
{
    struct PropertiesForDrawing
    {
        QPixmap pixmap;
        QString name;
        QString fileName;
        QString includeName;
        QColor color;
    };
}
