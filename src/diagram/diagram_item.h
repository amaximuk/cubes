#pragma once

#include <QGraphicsItem>
#include <QObject>
#include <QPointer>
#include <QFont>

//#include "../properties_item/properties_item.h"
//#include "../unit_types.h"


class QPixmap;
class QGraphicsSceneContextMenuEvent;
class QMenu;
class QPolygonF;
class QGraphicsScene;

namespace CubeDiagram
{
    class DiagramItem : public QGraphicsItem
    {
    public:
        uint32_t propertiesId_;
        QPixmap pixmap_;
        QString name_;
        QString fileName_;
        QString groupName_;
        QColor color_;
        //QSharedPointer<properties_item> properties_;
    private:
        QFont font_;
        QFont groupFont_;
        QRect iconRect_;
        QRectF textRect_;
        QRectF groupTextRect_;
        QRectF boundingRect_;
        bool borderOnly_;

    public:
        DiagramItem(uint32_t propertiesId, QPixmap pixmap, QString name, QString fileName, QString groupName, QColor color, QGraphicsItem* parent = nullptr);
        DiagramItem(const DiagramItem& other);
        ~DiagramItem();

    public:
        // QGraphicsItem
        QRectF boundingRect() const override;
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
        QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

    public:
        //QSharedPointer<properties_item> GetProperties() { return properties_; };
        QPointF GetLineAncorPosition() { return mapToScene(iconRect_.center()); }
        void InformPositionXChanged(double x);
        void InformPositionYChanged(double y);
        void InformPositionZChanged(double z);
        void InformFileChanged();
        void InformGroupChanged();
        void InformNameChanged(QString name, QString oldName);
        void InformDependencyChanged();
        void SetBorderOnly(bool borderOnly);
        //void SetGroupName(QString name) { groupName_ = name; }
        //QString GetGroupName() { return groupName_; }
    };
}
