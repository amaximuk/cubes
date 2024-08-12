#pragma once

#include <QGraphicsItem>
#include "../unit/unit_types.h"

namespace CubesDiagram
{
    class DiagramItem : public QGraphicsItem
    {
    private:
        // TODO: Убрать обратно в private
        CubesUnit::PropertiesId propertiesId_;
        QImage pixmap_;
        QString name_;
        QString fileName_;
        QString includeName_;
        QColor color_;

    private:
        QFont font_;
        QFont groupFont_;
        QRect iconRect_;
        QRectF textRect_;
        QRectF includeTextRect_;
        QRectF boundingRect_;
        bool borderOnly_;

    public:
        DiagramItem(CubesUnit::PropertiesId propertiesId, QImage pixmap, QString name, QString fileName,
            QString includeName, QColor color, QGraphicsItem* parent = nullptr);
        DiagramItem(const DiagramItem& other);
        ~DiagramItem();

    public:
        // QGraphicsItem
        QRectF boundingRect() const override;
        QPainterPath shape() const override;
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
        QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

    public:
        CubesUnit::PropertiesId GetPropertiesId() { return propertiesId_; };
        QString GetName() { return name_; };
        QPointF GetLineAncorPosition() { return mapToScene(iconRect_.center()); }
        void SetIncludeName(QString includeName);
        void SetName(QString name);
        void SetColor(QColor color);
        void SetBorderOnly(bool borderOnly);
    };
}
