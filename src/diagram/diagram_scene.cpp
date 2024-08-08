#include <QGraphicsSceneMouseEvent>
#include <QGuiApplication>
#include <QCursor>
#include <QKeyEvent>
#include <QPainter>
#include <QDebug>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include "../log/log_helper.h"
#include "../xml/xml_writer.h"
#include "../xml/xml_parser.h"
#include "diagram_item.h"
#include "diagram_scene.h"

using namespace CubesDiagram;

DiagramScene::DiagramScene(CubesTop::ITopManager* topManager, CubesLog::ILogManager* logManager, QObject *parent) :
    QGraphicsScene(parent)
{
    topManager_ = topManager;
    logManager_ = logManager;
    isItemMoving_ = false;
    movingItem_ = nullptr;
    selectedWithCtrl_ = false;
}

void DiagramScene::InformBasePropertiesChanged(CubesUnit::PropertiesId propertiesId, const QString& name,
    const QString& fileName, const QString& includeName, const QColor& color)
{
    for (auto& item : items())
    {
        CubesDiagram::DiagramItem* di = reinterpret_cast<CubesDiagram::DiagramItem*>(item);
        if (di->propertiesId_ == propertiesId)
        {
            di->name_ = name;
            di->fileName_ = fileName;
            di->includeName_ = includeName;
            di->color_ = color;
            di->InformNameChanged(name, "");
            di->InformIncludeChanged();
            di->InformColorChanged(color);
        }
    }
}

void DiagramScene::InformItemPositionChanged(DiagramItem* item)
{
    emit ItemPositionChanged(item);
}

void DiagramScene::InformItemCreated(DiagramItem* item)
{
    emit AfterItemCreated(item);
}

void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    bool ctrl = (event->modifiers() == Qt::ControlModifier);
    //if (!ctrl)
    {
        // TODO: Ошибка: Если выделять за границу прямоугольника itemAt возвращает 0, но item выделен!!!
        movingItem_ = itemAt(event->scenePos(), QTransform());
        qDebug() << "moving item: " << movingItem_ << ", pos: " << event->scenePos();
        if (movingItem_ != nullptr)
        {
            isItemMoving_ = true;

            // Если сразу тащим, без предварительного выделения, selectedItems() пустой
            // Если при этом были выделены другие item, то selectedItems() надо очистить
            if (!selectedItems().contains(movingItem_))
            {
                if (!ctrl)
                {
                    clearSelection();
                }
                else
                {
                    selectedWithCtrl_ = true;
                }
                movingItem_->setSelected(true);
            }

            for (auto& item : selectedItems())
            {
                DiagramItem* di = new DiagramItem(*reinterpret_cast<DiagramItem*>(item));
                DiagramItem* olddi = reinterpret_cast<DiagramItem*>(item);

                di->name_ = olddi->name_;
                dragItems_.push_back({ di, olddi });
                di->SetBorderOnly(true);
                addItem(di);
            }

            //bool ctrl = (event->modifiers() == Qt::ControlModifier);
            bool shift = (event->modifiers() == Qt::ShiftModifier);

            if (shift)
            {
                QGuiApplication::setOverrideCursor(Qt::DragCopyCursor);
                for (auto& pair : dragItems_)
                {
                    pair.second->name_ = "<new item>";
                    pair.second->InformNameChanged("<new item>", "");
                    pair.first->SetBorderOnly(false);
                }
            }
            else
                QGuiApplication::setOverrideCursor(Qt::ArrowCursor);

            QPointF p = movingItem_->mapFromParent(event->scenePos());
            startPosition_ = event->scenePos() - p;

        }
    }
    // Always remember to call parents mousePressEvent
    QGraphicsScene::mousePressEvent(event);
}

void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mouseMoveEvent(event);

    selectedWithCtrl_ = false;

    if (isItemMoving_)
        QGraphicsScene::invalidate(sceneRect(), QGraphicsScene::BackgroundLayer);
}

void DiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    isItemMoving_ = false;

    //bool ctrl = (event->modifiers() == Qt::ControlModifier);
    bool shift = (event->modifiers() == Qt::ShiftModifier);

    if (movingItem_ != nullptr)
    {
        const int gridSize = 20;
        QPointF delta = startPosition_ - movingItem_->pos();
        if (shift && (std::abs(delta.x()) >= gridSize || std::abs(delta.y()) >= gridSize))
        {
            for (auto& item : selectedItems())
            {
                item->setPos(item->pos() + delta);
            }
            clearSelection();


            for (auto& pair : dragItems_)
            {
                QString unitId;
                if (!topManager_->GetPropetiesUnitId(pair.first->propertiesId_, unitId))
                {
                    qDebug() << "ERROR GetPropetiesUnitParameters: " << pair.first->propertiesId_;
                }

                CubesUnit::PropertiesId propertiesId{ CubesUnit::InvalidPropertiesId };
                if (!topManager_->CreatePropetiesItem(unitId, propertiesId))
                {
                    qDebug() << "ERROR CreatePropetiesItem: " << unitId;
                }

                CubesTop::PropertiesForDrawing pfd{};
                if (!topManager_->GetPropetiesForDrawing(propertiesId, pfd))
                {
                    qDebug() << "ERROR GetPropetiesForDrawing: " << propertiesId;
                }

                pair.second->name_ = pair.first->name_;
                pair.second->InformNameChanged(pair.first->name_, "");




                pair.second->setPos(pair.first->pos());
                InformItemPositionChanged(pair.second);




                pair.first->name_ = pfd.name;
                pair.first->InformNameChanged(pfd.name, "");
                pair.first->propertiesId_ = propertiesId;


                pair.first->SetBorderOnly(false);
                InformItemCreated(pair.first);

                QPointF position = pair.first->pos() - delta;

                qreal xV = round(position.x() / gridSize) * gridSize;
                qreal yV = round(position.y() / gridSize) * gridSize;
                position = QPoint(xV, yV);

                addItem(pair.first);
                pair.first->setPos(position);
                InformItemPositionChanged(pair.first);

                pair.first->setSelected(true);
            }
            dragItems_.clear();

            QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
        }
        else
        {
            for (auto& pair : dragItems_)
            {
                pair.second->name_ = pair.first->name_;
                pair.second->InformNameChanged(pair.first->name_, "");
                delete pair.first;
            }
            dragItems_.clear();
            QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
        }

        //if (event->modifiers() == Qt::ControlModifier)
        if (selectedWithCtrl_)
        {
            movingItem_->setSelected(false);
            selectedWithCtrl_ = false;
        }
        movingItem_ = nullptr;
    }

    // !!!!!!!!!!!!!!!!!!!!!!!!!!
    //if (event->modifiers() != Qt::ControlModifier)
    QGraphicsScene::mouseReleaseEvent(event);

    QGraphicsScene::invalidate(sceneRect(), QGraphicsScene::BackgroundLayer);
}

void DiagramScene::keyPressEvent(QKeyEvent *keyEvent)
{
    if (isItemMoving_)
    {
        //bool ctrl = (keyEvent->modifiers() == Qt::ControlModifier);
        bool shift = (keyEvent->modifiers() == Qt::ShiftModifier);
        if (shift)
        {
            qDebug() << "press" << dragItems_.size();
            QGuiApplication::setOverrideCursor(Qt::DragCopyCursor);
            for (auto& pair : dragItems_)
            {
                pair.second->name_ = "<new item>";
                pair.second->InformNameChanged("<new item>", "");
                pair.first->SetBorderOnly(false);
            }
        }
        else
        {
            qDebug() << "release " << dragItems_.size();
            QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
            for (auto& pair : dragItems_)
            {
                pair.second->name_ = pair.first->name_;
                pair.second->InformNameChanged(pair.first->name_, "");
                pair.first->SetBorderOnly(true);
            }
        }
    }
    else
    {
        if (keyEvent->key() == Qt::Key_Delete)
        {
            for (auto item : selectedItems())
            {
                DiagramItem* di = reinterpret_cast<DiagramItem*>(item);
                emit BeforeItemDeleted(di);
                delete item;
            }
            invalidate();
        }
    }

    QGraphicsScene::keyPressEvent(keyEvent);
}

void DiagramScene::keyReleaseEvent(QKeyEvent *keyEvent)
{
    if (isItemMoving_)
    {
        //bool ctrl = (keyEvent->modifiers() == Qt::ControlModifier);
        bool ctrl = (keyEvent->modifiers() == Qt::ShiftModifier);
        if (ctrl)
        {
            QGuiApplication::setOverrideCursor(Qt::DragCopyCursor);
            for (auto& pair : dragItems_)
            {
                pair.second->name_ = "<new item>";
                pair.second->InformNameChanged("<new item>", "");
                pair.first->SetBorderOnly(false);
            }
        }
        else
        {
            QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
            for (auto& pair : dragItems_)
            {
                pair.second->name_ = pair.first->name_;
                pair.second->InformNameChanged(pair.first->name_, "");
                pair.first->SetBorderOnly(true);
            }
        }
    }

    bool ctrl = (keyEvent->modifiers() == Qt::ControlModifier);
    if (ctrl && keyEvent->key() == Qt::Key_C)
    {
        qDebug() << "CTRL-C";

        std::vector<CubesXml::Unit> xmlUnits;
        for (auto& item : selectedItems())
        {
            DiagramItem* di = reinterpret_cast<DiagramItem*>(item);
            const auto propertiesId = di->GetPropertiesId();
            CubesXml::Unit xmlUnit{};
            topManager_->GetPropetiesXmlUnit(propertiesId, xmlUnit);
            xmlUnits.push_back(xmlUnit);
        }

        CubesXml::Writer writer(logManager_);
        QByteArray byteArray;
        writer.WriteUnits(byteArray, xmlUnits);

        QClipboard* clipboard = QApplication::clipboard();
        QMimeData* mimeData = new QMimeData;
        mimeData->setData("application/x-cubes-units+xml", byteArray);

        QString as1251 = QString::fromLocal8Bit(byteArray);
        mimeData->setData("text/plain", as1251.toUtf8());
        clipboard->setMimeData(mimeData);
    }
    else if (ctrl && keyEvent->key() == Qt::Key_V)
    {
        qDebug() << "CTRL-V";

        const QClipboard* clipboard = QApplication::clipboard();
        const QMimeData* mimeData = clipboard->mimeData();

        if (mimeData->hasFormat("application/x-cubes-units+xml"))
        {
            QByteArray byteArray = mimeData->data("application/x-cubes-units+xml");
            CubesXml::Parser parser(logManager_);
            
            std::vector<CubesXml::Unit> xmlUnits;
            parser.ParseUnits(byteArray, xmlUnits);

            QList<CubesXml::Unit> units;
            for (const auto& unit : xmlUnits)
                units.push_back(unit);

            topManager_->AddUnits(units);
        }
    }

    QGraphicsScene::keyPressEvent(keyEvent);
}

void DiagramScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    DrawConnections(painter, rect);
    QGraphicsScene::drawBackground(painter, rect);
}

DiagramItem* DiagramScene::GetDiagramItem(QString name)
{
    for (const auto& item : items())
    {
        DiagramItem* di = reinterpret_cast<DiagramItem*>(item);
        if (name == di->name_)
        {
            return di;
        }
    }
    return nullptr;
}

void DiagramScene::DrawConnections(QPainter* painter, const QRectF& rect)
{
    {
        QMap<QString, QStringList> connections;
        if (!topManager_->GetUnitsConnections(connections))
            return;
        painter->setPen(QPen(QBrush(QColor(0xFF, 0, 0, 0x20), Qt::SolidPattern), 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->setRenderHint(QPainter::Antialiasing);
        for (const auto& kvp : connections.toStdMap())
        {
            auto di1 = GetDiagramItem(kvp.first);
            for (const auto& item : kvp.second)
            {
                auto di2 = GetDiagramItem(item);
                // Может быть указано имя несуществующего юнита
                if (di2 != nullptr)
                    painter->drawLine(di1->GetLineAncorPosition(), di2->GetLineAncorPosition());
            }
        }
    }

    {
        QMap<QString, QStringList> connections;
        if (!topManager_->GetDependsConnections(connections))
            return;
        painter->setPen(QPen(QBrush(QColor(0, 0, 0, 0x80), Qt::SolidPattern), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->setRenderHint(QPainter::Antialiasing);
        for (const auto& kvp : connections.toStdMap())
        {
            auto di1 = GetDiagramItem(kvp.first);
            for (const auto& item : kvp.second)
            {
                auto di2 = GetDiagramItem(item);
                // Может быть указано имя несуществующего юнита
                if (di2 != nullptr)
                    painter->drawLine(di1->GetLineAncorPosition(), di2->GetLineAncorPosition());
            }
        }
    }
}

void DiagramScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    auto item = itemAt(event->scenePos(), QTransform());
    if (item != nullptr)
    {
        DiagramItem* di = reinterpret_cast<DiagramItem*>(item);
    }
}
