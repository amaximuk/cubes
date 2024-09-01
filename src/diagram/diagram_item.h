#pragma once

#include <QGraphicsItem>
#include "../unit/unit_types.h"
#include "diagram_item_types.h"

namespace CubesDiagram
{
    class DiagramItem : public QGraphicsItem
    {
    private:
        CubesUnit::PropertiesId propertiesId_;
        CubesDiagram::PropertiesForDrawing pfd_;

    private:
        QFont font_;
        QFont groupFont_;
        QRect iconRect_;
        QRectF textRect_;
        QRectF includeTextRect_;
        QRectF boundingRect_;
        bool borderOnly_;

    public:
        DiagramItem(CubesUnit::PropertiesId propertiesId, CubesDiagram::PropertiesForDrawing pfd, QGraphicsItem* parent = nullptr);
        DiagramItem(const DiagramItem& other);
        ~DiagramItem();

    public:
        // QGraphicsItem
        QRectF boundingRect() const override;
        QPainterPath shape() const override;
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
        QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
        void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
        void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
        void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
        void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
        void keyPressEvent(QKeyEvent* keyEvent) override;

    public:
        CubesUnit::PropertiesId GetPropertiesId() { return propertiesId_; };
        QString GetName() { return pfd_.name; };
        QPointF GetLineAncorPosition() { return mapToScene(iconRect_.center()); }
        QSizeF GetSize() { return iconRect_.size(); };
        void SetIncludeName(QString includeName);
        void SetName(QString name);
        void SetColor(QColor color);
        void SetBorderOnly(bool borderOnly);
        void SetSize(QSizeF size);
        void SetText(QString text, bool showBorder, HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment);

        bool IsResizing();

    private:
        void UpdateGeometry();
    };
}
