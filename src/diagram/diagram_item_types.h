#pragma once

#include <QString>
#include <QColor>
#include <QImage>
#include "../unit/unit_types.h"
#include "../log/log_types.h"

namespace CubesDiagram
{
    constexpr int GridSize = 16;

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
        QSizeF size;
        QString text;

        PropertiesForDrawing()
        {
            itemType = ItemType::Unit;
        }

        PropertiesForDrawing(const PropertiesForDrawing& other)
        {
            pixmap = QImage(other.pixmap);
            name = other.name;
            fileName = other.fileName;
            includeName = other.includeName;
            color = QColor(other.color);
            itemType = other.itemType;
            size = other.size;
            text = other.text;
        }

        PropertiesForDrawing& operator=(const PropertiesForDrawing& other)
        {
            pixmap = QImage(other.pixmap);
            name = other.name;
            fileName = other.fileName;
            includeName = other.includeName;
            color = QColor(other.color);
            itemType = other.itemType;
            size = other.size;
            text = other.text;
            return *this;
        }
    };
}