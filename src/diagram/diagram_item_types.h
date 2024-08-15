#pragma once

#include <QString>
#include <QColor>
#include <QImage>
#include "../unit/unit_types.h"
#include "../log/log_types.h"

namespace CubesDiagram
{
	enum class ItemType
	{
		Unit,
		Text,
		Group
	};

    struct PropertiesForDrawing
    {
        QImage pixmap;
        QString name;
        QString fileName;
        QString includeName;
        QColor color;
        ItemType itemType;
        QSize size;
        QString text;
    };
}