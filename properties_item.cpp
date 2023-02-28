#include <QPainter>
#include <QDebug>
#include <QRegularExpression>

#include "diagram_scene.h"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "main_window.h"
#include "base64.h"
#include "diagram_item.h"
#include "properties_item.h"

properties_item::properties_item(unit_types::UnitParameters unitParameters, diagram_item* diagramItem, QObject* parent):
    QObject(parent)
{
    unitParameters_ = unitParameters;
    diagramItem_ = diagramItem;
    CreatePropertyBrowser();
}

void properties_item::CreatePropertyBrowser()
{
    groupManager = new QtGroupPropertyManager(this);
    intManager = new QtIntPropertyManager(this);
    doubleManager = new QtDoublePropertyManager(this);
    stringManager = new QtStringPropertyManager(this);
    colorManager = new QtColorPropertyManager(this);
    fontManager = new QtFontPropertyManager(this);
    pointManager = new QtPointPropertyManager(this);
    sizeManager = new QtSizePropertyManager(this);

    qDebug() << connect(intManager, SIGNAL(valueChanged(QtProperty*, int)), this, SLOT(valueChanged(QtProperty*, int)));
    qDebug() << connect(doubleManager, SIGNAL(valueChanged(QtProperty*, double)), this, SLOT(valueChanged(QtProperty*, double)));
    qDebug() << connect(stringManager, SIGNAL(valueChanged(QtProperty*, const QString&)), this, SLOT(valueChanged(QtProperty*, const QString&)));
    qDebug() << connect(colorManager, SIGNAL(valueChanged(QtProperty*, const QColor&)), this, SLOT(valueChanged(QtProperty*, const QColor&)));
    qDebug() << connect(fontManager, SIGNAL(valueChanged(QtProperty*, const QFont&)), this, SLOT(valueChanged(QtProperty*, const QFont&)));
    qDebug() << connect(pointManager, SIGNAL(valueChanged(QtProperty*, const QPoint&)), this, SLOT(valueChanged(QtProperty*, const QPoint&)));
    qDebug() << connect(sizeManager, SIGNAL(valueChanged(QtProperty*, const QSize&)), this, SLOT(valueChanged(QtProperty*, const QSize&)));
}
QtProperty* positionXProperty;
QtProperty* positionYProperty;

void properties_item::ApplyToBrowser(QtTreePropertyBrowser* propertyEditor)
{
    QtSpinBoxFactory* intSpinBoxFactory = new QtSpinBoxFactory(this);
    QtDoubleSpinBoxFactory* doubleSpinBoxFactory = new QtDoubleSpinBoxFactory(this);
    QtCheckBoxFactory* checkBoxFactory = new QtCheckBoxFactory(this);
    QtSpinBoxFactory* spinBoxFactory = new QtSpinBoxFactory(this);
    QtLineEditFactory* lineEditFactory = new QtLineEditFactory(this);
    QtEnumEditorFactory* comboBoxFactory = new QtEnumEditorFactory(this);

    propertyEditor->setFactoryForManager(intManager, intSpinBoxFactory);
    propertyEditor->setFactoryForManager(doubleManager, doubleSpinBoxFactory);
    propertyEditor->setFactoryForManager(stringManager, lineEditFactory);
    propertyEditor->setFactoryForManager(colorManager->subIntPropertyManager(), spinBoxFactory);
    propertyEditor->setFactoryForManager(fontManager->subIntPropertyManager(), spinBoxFactory);
    propertyEditor->setFactoryForManager(fontManager->subBoolPropertyManager(), checkBoxFactory);
    propertyEditor->setFactoryForManager(fontManager->subEnumPropertyManager(), comboBoxFactory);
    propertyEditor->setFactoryForManager(pointManager->subIntPropertyManager(), spinBoxFactory);
    propertyEditor->setFactoryForManager(sizeManager->subIntPropertyManager(), spinBoxFactory);


    propertyEditor->setResizeMode(QtTreePropertyBrowser::ResizeMode::Interactive);
    propertyEditor->setSplitterPosition(250);
    //propertyEditor->setResizeMode(QtTreePropertyBrowser::ResizeMode::ResizeToContents);
    propertyEditor->clear();

    QtProperty* mainGroup = groupManager->addProperty(QString::fromStdString(unitParameters_.fiileInfo.info.id));
    
    QtProperty* propertiesGroup = groupManager->addProperty(QString::fromLocal8Bit("Свойства"));
    mainGroup->addSubProperty(propertiesGroup);

    
    for (const auto& pi : unitParameters_.fiileInfo.parameters)
    {
        QRegularExpression re(R"wwww(^array<(?<value>.*)>)wwww");
        QRegularExpressionMatch match = re.match(QString::fromStdString(pi.type));
        if (match.hasMatch())
        {
            QString value = match.captured("value");

            QtProperty* gr = groupManager->addProperty(QString::fromStdString(pi.display_name));
            propertiesGroup->addSubProperty(gr);

            QtProperty* pr = intManager->addProperty(QString::fromLocal8Bit("Количество"));
            gr->addSubProperty(pr);
        }
        else
        {
            QtProperty* pr = stringManager->addProperty(QString::fromStdString(pi.display_name));
            propertiesGroup->addSubProperty(pr);
            //addProperty(channelsGroup, QLatin1String("Channels"));
        }
    }

    QtProperty* editorGroup = groupManager->addProperty(QString::fromLocal8Bit("Редактор"));
    mainGroup->addSubProperty(editorGroup);


    positionXProperty = doubleManager->addProperty("Position X");
    doubleManager->setRange(positionXProperty, -10000, 10000);
    //doubleManager->setValue(positionXProperty, di->scenePos().x());
    //addProperty(positionXProperty, QLatin1String("Position X"));
    editorGroup->addSubProperty(positionXProperty);

    positionYProperty = doubleManager->addProperty("Position Y");
    doubleManager->setRange(positionYProperty, -10000, 10000);
    //doubleManager->setValue(positionYProperty, di->scenePos().y());
    //addProperty(positionYProperty, QLatin1String("Position Y"));
    editorGroup->addSubProperty(positionYProperty);

    
    propertyEditor->addProperty(mainGroup);
}

QPixmap properties_item::GetPixmap()
{
    QPixmap px;
    bool loaded = false;
    if (unitParameters_.fiileInfo.info.pictogram != "")
    {
        std::string s = base64_decode(unitParameters_.fiileInfo.info.pictogram);
        QByteArray ba(s.c_str(), static_cast<int>(s.size()));
        try
        {
            loaded = px.loadFromData(ba);
        }
        catch (...)
        {
            loaded = false;
        }
    }
    if (!loaded)
        px.load(":/images/ice.png");
    return px;
}

void properties_item::PositionChanged(QPointF point)
{
    doubleManager->setValue(positionXProperty, point.x());
    doubleManager->setValue(positionYProperty, point.y());
}

void properties_item::valueChanged(QtProperty* property, int value)
{
    qDebug() << "valueChanged value = " << value;
    //if (!idToProperty.contains("Channels"))
    //    return;

    //QtProperty* channelsGroup = idToProperty["Channels"];

    //while (value > channelsGroup->subProperties().size() - 1)
    //{
    //    QtProperty* channelsCountProperty = intManager->addProperty(QString::fromLocal8Bit("Item %1").arg(channelsGroup->subProperties().size()));
    //    intManager->setRange(channelsCountProperty, 0, 10000);
    //    intManager->setValue(channelsCountProperty, 0);
    //    addProperty(channelsCountProperty, QString::fromLocal8Bit("Item %1").arg(channelsGroup->subProperties().size()));
    //    channelsGroup->addSubProperty(channelsCountProperty);
    //}

    //while (value < channelsGroup->subProperties().size() - 1)
    //{
    //    QtProperty* channelsCountProperty = channelsGroup->subProperties()[channelsGroup->subProperties().size() - 1];
    //    //removeProperty(channelsCountProperty, QString::fromLocal8Bit("Item %1").arg(channelsGroup->subProperties().size()));

    //    channelsGroup->removeSubProperty(channelsCountProperty);
    //}


}

void properties_item::valueChanged(QtProperty* property, double value)
{
    if (property == positionXProperty)
    {
        qDebug() << "valueChanged X value = " << value;
        diagramItem_->InformPositionXChanged(value);
    }
    else if (property == positionYProperty)
    {
        qDebug() << "valueChanged Y value = " << value;
        diagramItem_->InformPositionYChanged(value);
    }

    //if (!propertyToId.contains(property))
    //    return;

    ////if (!currentItem)
    ////    return;

    //if (scene_->selectedItems().count() > 0 && !scene_->isItemMoving())
    //{
    //    //diagram_item* gi = qobject_cast<diagram_item*>(sp_scene_->selectedItems()[0]);
    //    diagram_item* gi = (diagram_item*)(scene_->selectedItems()[0]);
    //    qDebug() << gi->getName();

    //    QString id = propertyToId[property];
    //    if (id == "Position X")
    //        gi->setX(value);
    //    else if (id == "Position Y")
    //        gi->setY(value);
    //}
}

void properties_item::valueChanged(QtProperty* property, const QString& value)
{
    //    if (!propertyToId.contains(property))
    //        return;

    //    if (!currentItem)
    //        return;

    //    QString id = propertyToId[property];
    //    if (id == QLatin1String("text")) {
    //        if (currentItem->rtti() == QtCanvasItem::Rtti_Text) {
    //            QtCanvasText *i = (QtCanvasText *)currentItem;
    //            i->setText(value);
    //        }
    //    }
    //    canvas->update();
}

void properties_item::valueChanged(QtProperty* property, const QColor& value)
{
    //    if (!propertyToId.contains(property))
    //        return;

    //    if (!currentItem)
    //        return;

    //    QString id = propertyToId[property];
    //    if (id == QLatin1String("color")) {
    //        if (currentItem->rtti() == QtCanvasItem::Rtti_Text) {
    //            QtCanvasText *i = (QtCanvasText *)currentItem;
    //            i->setColor(value);
    //        }
    //    } else if (id == QLatin1String("brush")) {
    //        if (currentItem->rtti() == QtCanvasItem::Rtti_Rectangle ||
    //                currentItem->rtti() == QtCanvasItem::Rtti_Ellipse) {
    //            QtCanvasPolygonalItem *i = (QtCanvasPolygonalItem *)currentItem;
    //            QBrush b = i->brush();
    //            b.setColor(value);
    //            i->setBrush(b);
    //        }
    //    } else if (id == QLatin1String("pen")) {
    //        if (currentItem->rtti() == QtCanvasItem::Rtti_Rectangle ||
    //                currentItem->rtti() == QtCanvasItem::Rtti_Line) {
    //            QtCanvasPolygonalItem *i = (QtCanvasPolygonalItem *)currentItem;
    //            QPen p = i->pen();
    //            p.setColor(value);
    //            i->setPen(p);
    //        }
    //    }
    //    canvas->update();
}

void properties_item::valueChanged(QtProperty* property, const QFont& value)
{
    //    if (!propertyToId.contains(property))
    //        return;

    //    if (!currentItem)
    //        return;

    //    QString id = propertyToId[property];
    //    if (id == QLatin1String("font")) {
    //        if (currentItem->rtti() == QtCanvasItem::Rtti_Text) {
    //            QtCanvasText *i = (QtCanvasText *)currentItem;
    //            i->setFont(value);
    //        }
    //    }
    //    canvas->update();
}

void properties_item::valueChanged(QtProperty* property, const QPoint& value)
{
    //    if (!propertyToId.contains(property))
    //        return;

    //    if (!currentItem)
    //        return;

    //    QString id = propertyToId[property];
    //    if (currentItem->rtti() == QtCanvasItem::Rtti_Line) {
    //        QtCanvasLine *i = (QtCanvasLine *)currentItem;
    //        if (id == QLatin1String("endpoint")) {
    //            i->setPoints(i->startPoint().x(), i->startPoint().y(), value.x(), value.y());
    //        }
    //    }
    //    canvas->update();
}

void properties_item::valueChanged(QtProperty* property, const QSize& value)
{
    //    if (!propertyToId.contains(property))
    //        return;

    //    if (!currentItem)
    //        return;

    //    QString id = propertyToId[property];
    //    if (id == QLatin1String("size")) {
    //        if (currentItem->rtti() == QtCanvasItem::Rtti_Rectangle) {
    //            QtCanvasRectangle *i = (QtCanvasRectangle *)currentItem;
    //            i->setSize(value.width(), value.height());
    //        } else if (currentItem->rtti() == QtCanvasItem::Rtti_Ellipse) {
    //            QtCanvasEllipse *i = (QtCanvasEllipse *)currentItem;
    //            i->setSize(value.width(), value.height());
    //        }
    //    }
    //    canvas->update();
}
