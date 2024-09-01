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

    enum class HorizontalAlignment
    {
        Left,
        Center,
        Right
    };

    enum class VerticalAlignment
    {
        Top,
        Center,
        Bottom
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
        bool showBorder;
        HorizontalAlignment horizontalAlignment;
        VerticalAlignment verticalAlignment;

        PropertiesForDrawing()
        {
            itemType = ItemType::Unit;
            showBorder = true;
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
            showBorder = other.showBorder;
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
            showBorder = other.showBorder;
            return *this;
        }
    };
}