#pragma once

#include <QString>
#include <QPixmap>
#include <QColor>

struct PropertiesForDrawing
{
    QPixmap pixmap;
    QString name;
    QString fileName;
    QString includeName;
    QColor color;
};
