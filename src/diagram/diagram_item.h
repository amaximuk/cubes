#pragma once

#include <QGraphicsItem>
#include <QObject>
#include <QPointer>
#include <QFont>

#include "../properties_item/properties_item.h"
#include "../unit_types.h"


QT_BEGIN_NAMESPACE
class QPixmap;
class QGraphicsSceneContextMenuEvent;
class QMenu;
class QPolygonF;
class QGraphicsScene;
QT_END_NAMESPACE

class diagram_item : public QGraphicsItem
{
private:
    QPixmap pixmap_;
    QSharedPointer<properties_item> properties_;
    QFont font_;
    QFont groupFont_;
    QRect iconRect_;
    QRectF textRect_;
    QRectF groupTextRect_;
    QRectF boundingRect_;
    bool borderOnly_;
    QString groupName_;

public:
    diagram_item(unit_types::UnitParameters unitParameters, QGraphicsItem *parent = nullptr);
    diagram_item(const diagram_item& other);
    ~diagram_item();

public:
    // QGraphicsItem
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    //QString getName() { return properties_->GetName(); };
    //QList<QString> getConnectedNames() { return properties_->GetConnectedNames(); }
    //QList<QString> getDependentNames() { return properties_->GetDependentNames(); }
    //QString getInstanceName() { return properties_->GetInstanceName(); }

public:
    QSharedPointer<properties_item> GetProperties() { return properties_; };
    QPointF GetLineAncorPosition() { return mapToScene(iconRect_.center()); }
    void InformPositionXChanged(double x);
    void InformPositionYChanged(double y);
    void InformPositionZChanged(double z);
    void InformFileChanged();
    void InformGroupChanged();
    void InformNameChanged(QString name, QString oldName);
    void InformDependencyChanged();
    void SetBorderOnly(bool borderOnly);
    void SetGroupName(QString name) { groupName_ = name; }
    QString GetGroupName() { return groupName_; }
};
