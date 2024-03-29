#pragma once

#include <QGraphicsItem>

namespace CubeDiagram
{
    class DiagramItem : public QGraphicsItem
    {
    public:
        // TODO: Убрать обратно в private
        uint32_t propertiesId_;
        QPixmap pixmap_;
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
        DiagramItem(uint32_t propertiesId, QPixmap pixmap, QString name, QString fileName,
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
        QPointF GetLineAncorPosition() { return mapToScene(iconRect_.center()); }
        void InformIncludeChanged();
        void InformNameChanged(QString name, QString oldName);
        void InformColorChanged(QColor color);
        void SetBorderOnly(bool borderOnly);
        uint32_t GetPropertiesId() { return propertiesId_; };
    };
}
