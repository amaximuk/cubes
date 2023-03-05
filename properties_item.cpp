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
#include "parameters_compiler_helper.h"

properties_item::properties_item(unit_types::UnitParameters unitParameters, diagram_item* diagramItem, QObject* parent):
    QObject(parent)
{
    unitParameters_ = unitParameters;
    diagramItem_ = diagramItem;
    parametersModel_ = {};
    editorModel_ = {};

    CreateParametersModel();
    CreateEditorModel();
    CreatePropertyBrowser();
}

void properties_item::CreateEditorModel()
{
    unit_types::ParameterModel pmx;
    pmx.id = QString::fromLocal8Bit("_POSITION_X");
    pmx.name = QString::fromLocal8Bit("Позиция X");
    pmx.editorSettings.type = unit_types::EditorType::SpinDouble;
    pmx.editorSettings.SpinDoubleMin = -10000;
    pmx.editorSettings.SpinDoubleMax = 10000;
    pmx.editorSettings.SpinDoubleSingleStep = 20;
    editorModel_.parameters.push_back(pmx);

    unit_types::ParameterModel pmy;
    pmy.id = QString::fromLocal8Bit("_POSITION_Y");
    pmy.name = QString::fromLocal8Bit("Позиция Y");
    pmy.editorSettings.type = unit_types::EditorType::SpinDouble;
    pmy.editorSettings.SpinDoubleMin = -10000;
    pmy.editorSettings.SpinDoubleMax = 10000;
    pmy.editorSettings.SpinDoubleSingleStep = 20;
    editorModel_.parameters.push_back(pmy);

    unit_types::ParameterModel pmz;
    pmz.id = QString::fromLocal8Bit("_POSITION_Z");
    pmz.name = QString::fromLocal8Bit("Позиция Z");
    pmz.editorSettings.type = unit_types::EditorType::SpinDouble;
    pmz.editorSettings.SpinDoubleMin = -10000;
    pmz.editorSettings.SpinDoubleMax = 10000;
    editorModel_.parameters.push_back(pmz);
}

void properties_item::CreateParametersModel()
{
    for (const auto& pi : unitParameters_.fiileInfo.parameters)
    {
        QRegularExpression re(R"wwww(^array<(?<value>.*)>)wwww");
        QRegularExpressionMatch match = re.match(QString::fromStdString(pi.type));
        QString type = QString::fromStdString(pi.type);
        bool is_array = false;
        if (match.hasMatch())
        {
            is_array = true;
            type = match.captured("value");
        }

        unit_types::ParameterModel pm;
        pm.id = QString::fromStdString(pi.name);
        if (is_array)
        {
            pm.is_array = true;
            if (pi.restrictions.set_count.size() > 0)
            {
                pm.editorSettings.type = unit_types::EditorType::ComboBox;
                for (const auto& s : pi.restrictions.set_count)
                    pm.editorSettings.ComboBoxValues.push_back(QString::fromStdString(s));
            }
            else
            {
                pm.editorSettings.type = unit_types::EditorType::SpinInterger;

                if (pi.restrictions.min_count != "")
                    pm.editorSettings.SpinIntergerMin = std::stoi(pi.restrictions.min_count);
                else
                    pm.editorSettings.SpinIntergerMin = 0;
                
                if (pi.restrictions.max_count != "")
                    pm.editorSettings.SpinIntergerMax = std::stoi(pi.restrictions.max_count);
                else
                    pm.editorSettings.SpinIntergerMax = 1000; // !!! todo: make a define for a const
            }
        }
        else
        {
            // "unit", "path", "string", "double", "int", "bool", "float", "int8_t", "int16_t", "int32_t", "int64_t", "uint8_t", "uint16_t", "uint32_t", "uint64_t"

            if (type == "unit")
            {
                pm.editorSettings.type = unit_types::EditorType::ComboBox;

                unit_types::ParameterModel pm_depends;
                pm_depends.id = pm.id + "_DEPENDS";
                pm_depends.editorSettings.type = unit_types::EditorType::CheckBox;
                pm.parameters.push_back(pm_depends);
            }
            else if (type == "path" || type == "string")
            {
                pm.editorSettings.type = unit_types::EditorType::String;
            }
            else if (type == "bool")
            {
                pm.editorSettings.type = unit_types::EditorType::CheckBox;
            }
            else if (type == "int" || type == "int8_t" || type == "int16_t" || type == "int32_t" || type == "int64_t" || type == "uint8_t" || type == "uint16_t" || type == "uint32_t" || type == "uint64_t")
            {
                pm.editorSettings.type = unit_types::EditorType::SpinInterger;

                if (pi.restrictions.min != "")
                    pm.editorSettings.SpinIntergerMin = std::stoi(pi.restrictions.min);
                else
                    pm.editorSettings.SpinIntergerMin = unit_types::GetMinForIntegralType(type);

                if (pi.restrictions.max != "")
                    pm.editorSettings.SpinIntergerMax = std::stoi(pi.restrictions.max);
                else
                    pm.editorSettings.SpinIntergerMax = unit_types::GetMaxForIntegralType(type);
            }
            else if (type == "double" || type == "float")
            {
                pm.editorSettings.type = unit_types::EditorType::SpinDouble;

                if (pi.restrictions.min != "")
                    pm.editorSettings.SpinDoubleMin = std::stod(pi.restrictions.min);
                else
                    pm.editorSettings.SpinDoubleMin = unit_types::GetMinForFloatingPointType(type);

                if (pi.restrictions.max != "")
                    pm.editorSettings.SpinDoubleMax = std::stod(pi.restrictions.max);
                else
                    pm.editorSettings.SpinDoubleMax = unit_types::GetMinForFloatingPointType(type);
            }
            else
            {
                // enum user type
                const auto pti = parameters_compiler::helper::get_type_info(unitParameters_.fiileInfo, type.toStdString());
                if (pti->type == "enum")
                {
                    pm.editorSettings.type = unit_types::EditorType::ComboBox;
                    for (const auto v : pti->values)
                        pm.editorSettings.ComboBoxValues.push_back(QString::fromStdString(v.first));
                }
                else assert(false);
            }
        }
        parametersModel_.parameters.push_back(pm);
    }
}

QtProperty* properties_item::GetPropertyForModel(unit_types::ParameterModel& model)
{
    // String, SpinInterger, SpinDouble, ComboBox, CheckBox

    QtProperty* pr = nullptr;
    if (model.editorSettings.type == unit_types::EditorType::String)
    {
        pr = stringManager->addProperty(model.id);
        stringManager->setRegExp(pr, QRegExp("-?\\d{1,3}"));
    }
    else if (model.editorSettings.type == unit_types::EditorType::SpinInterger)
    {
        pr = intManager->addProperty(model.id);
        intManager->blockSignals(true);
        intManager->setRange(pr, model.editorSettings.SpinIntergerMin, model.editorSettings.SpinIntergerMax);
        intManager->blockSignals(false);
    }
    else if (model.editorSettings.type == unit_types::EditorType::SpinDouble)
    {
        pr = doubleManager->addProperty(model.id);
        doubleManager->setRange(pr, model.editorSettings.SpinDoubleMin, model.editorSettings.SpinDoubleMax);
        doubleManager->setSingleStep(pr, model.editorSettings.SpinDoubleSingleStep);
    }
    else if (model.editorSettings.type == unit_types::EditorType::ComboBox)
    {
        pr = enumManager->addProperty(model.id);
        enumManager->blockSignals(true);
        enumManager->setEnumNames(pr, model.editorSettings.ComboBoxValues);
        enumManager->blockSignals(false);
    }
    else if (model.editorSettings.type == unit_types::EditorType::CheckBox)
        pr = boolManager->addProperty(model.id);
    else assert(false);

    RegisterProperty(pr, model.id);

    for (auto& sp : model.parameters)
    {
        pr->addSubProperty(GetPropertyForModel(sp));
    }

    //if (model.type == unit_types::ParameterType::SimpleParameter)
    //{
    //}
    //else if (model.type == unit_types::ParameterType::UnitParameter)
    //{
    //}
    //else if (model.type == unit_types::ParameterType::ArrayParameter)
    //{

    //}
    //else assert(false);
    return pr;
}

void properties_item::CreatePropertyBrowser()
{
    groupManager = new QtGroupPropertyManager(this);
    intManager = new QtIntPropertyManager(this);
    doubleManager = new QtDoublePropertyManager(this);
    stringManager = new QtStringPropertyManager(this);
    enumManager = new QtEnumPropertyManager(this);
    colorManager = new QtColorPropertyManager(this);
    fontManager = new QtFontPropertyManager(this);
    pointManager = new QtPointPropertyManager(this);
    sizeManager = new QtSizePropertyManager(this);
    boolManager = new QtBoolPropertyManager(this);

    qDebug() << connect(intManager, SIGNAL(valueChanged(QtProperty*, int)), this, SLOT(valueChanged(QtProperty*, int)));
    qDebug() << connect(doubleManager, SIGNAL(valueChanged(QtProperty*, double)), this, SLOT(valueChanged(QtProperty*, double)));
    qDebug() << connect(stringManager, SIGNAL(valueChanged(QtProperty*, const QString&)), this, SLOT(valueChanged(QtProperty*, const QString&)));
    qDebug() << connect(enumManager, SIGNAL(valueChanged(QtProperty*, int)), this, SLOT(valueChanged(QtProperty*, int)));
    qDebug() << connect(colorManager, SIGNAL(valueChanged(QtProperty*, const QColor&)), this, SLOT(valueChanged(QtProperty*, const QColor&)));
    qDebug() << connect(fontManager, SIGNAL(valueChanged(QtProperty*, const QFont&)), this, SLOT(valueChanged(QtProperty*, const QFont&)));
    qDebug() << connect(pointManager, SIGNAL(valueChanged(QtProperty*, const QPoint&)), this, SLOT(valueChanged(QtProperty*, const QPoint&)));
    qDebug() << connect(sizeManager, SIGNAL(valueChanged(QtProperty*, const QSize&)), this, SLOT(valueChanged(QtProperty*, const QSize&)));
    qDebug() << connect(boolManager, SIGNAL(valueChanged(QtProperty*, bool)), this, SLOT(valueChanged(QtProperty*, bool)));
}


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
    propertyEditor->setFactoryForManager(enumManager, comboBoxFactory);
    propertyEditor->setFactoryForManager(colorManager->subIntPropertyManager(), spinBoxFactory);
    propertyEditor->setFactoryForManager(fontManager->subIntPropertyManager(), spinBoxFactory);
    propertyEditor->setFactoryForManager(fontManager->subBoolPropertyManager(), checkBoxFactory);
    propertyEditor->setFactoryForManager(fontManager->subEnumPropertyManager(), comboBoxFactory);
    propertyEditor->setFactoryForManager(pointManager->subIntPropertyManager(), spinBoxFactory);
    propertyEditor->setFactoryForManager(sizeManager->subIntPropertyManager(), spinBoxFactory);
    propertyEditor->setFactoryForManager(boolManager, checkBoxFactory);


    propertyEditor->setResizeMode(QtTreePropertyBrowser::ResizeMode::Interactive);
    propertyEditor->setSplitterPosition(250);
    //propertyEditor->setResizeMode(QtTreePropertyBrowser::ResizeMode::ResizeToContents);
    propertyEditor->clear();

    QtProperty* mainGroup = groupManager->addProperty(QString::fromStdString(unitParameters_.fiileInfo.info.id));
    
    QtProperty* propertiesGroup = groupManager->addProperty(QString::fromLocal8Bit("Свойства"));
    mainGroup->addSubProperty(propertiesGroup);

    for (auto& pm : parametersModel_.parameters)
        propertiesGroup->addSubProperty(GetPropertyForModel(pm));
    
    QtProperty* editorGroup = groupManager->addProperty(QString::fromLocal8Bit("Редактор"));
    mainGroup->addSubProperty(editorGroup);

    for (auto& pm : editorModel_.parameters)
        editorGroup->addSubProperty(GetPropertyForModel(pm));

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
    doubleManager->setValue(GetProperty("_POSITION_X"), point.x());
    doubleManager->setValue(GetProperty("_POSITION_Y"), point.y());
}

void properties_item::ZOrderChanged(double value)
{
    doubleManager->setValue(GetProperty("_POSITION_Z"), value);
}

QString properties_item::GetPropertyDescription(QtProperty* property)
{
    QString id = GetPropertyId(property);
    return id;
}

void properties_item::valueChanged(QtProperty* property, int value)
{
    qDebug() << "valueChanged value = " << value;
    //if (!idToProperty.contains("Channels"))
    //    return;

    //QtProperty* channelsGroup = idToProperty["Channels"];

    auto pm = GetParameterModel(property);
    if (pm->is_array)
    {
        int count = std::stoi(property->valueText().toStdString());

        while (count > property->subProperties().size())
        {
            unit_types::ParameterModel pm_new;
            pm_new.id = QString::fromLocal8Bit("Item %1").arg(property->subProperties().size());
            pm_new.editorSettings.type = unit_types::EditorType::String;
            pm->parameters.push_back(pm_new);
            property->addSubProperty(GetPropertyForModel(pm_new));
        }

        while (count < property->subProperties().size())
        {
            QtProperty* removeProperty = property->subProperties()[property->subProperties().size() - 1];
            UnregisterProperty(removeProperty);
            pm->parameters.pop_back();
            property->removeSubProperty(removeProperty);
        }
    }
}

void properties_item::valueChanged(QtProperty* property, double value)
{
    QString id = GetPropertyId(property);

    int gridSize = 20;
    if (id == "_POSITION_X")
    {
        qDebug() << "valueChanged X value = " << value;
        qreal xV = round(value / gridSize) * gridSize;
        if (xV != value)
            doubleManager->setValue(property, xV);
        diagramItem_->InformPositionXChanged(xV);
    }
    else if (id == "_POSITION_Y")
    {
        qDebug() << "valueChanged Y value = " << value;
        qreal yV = round(value / gridSize) * gridSize;
        if (yV != value)
            doubleManager->setValue(property, yV);
        diagramItem_->InformPositionYChanged(yV);
    }
    else if (id == "_POSITION_Z")
    {
        qDebug() << "valueChanged Z value = " << value;
        diagramItem_->InformPositionZChanged(value);
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

void properties_item::valueChanged(QtProperty* property, bool value)
{

}

void properties_item::RegisterProperty(QtProperty* property, const QString& id)
{
    propertyToId[property] = id;
    idToProperty[id] = property;
    idToExpanded[id] = false;
    //QtBrowserItem *item = propertyEditor->addProperty(property);
    //if (idToExpanded.contains(id))
    //    propertyEditor->setExpanded(item, idToExpanded[id]);
}

void properties_item::UnregisterProperty(const QString& id)
{
    propertyToId.remove(idToProperty[id]);
    idToProperty.remove(id);
    idToExpanded.remove(id);
    //QtBrowserItem *item = propertyEditor->addProperty(property);
    //if (idToExpanded.contains(id))
    //    propertyEditor->setExpanded(item, idToExpanded[id]);
}

void properties_item::UnregisterProperty(QtProperty* property)
{
    idToProperty.remove(propertyToId[property]);
    idToExpanded.remove(propertyToId[property]);
    propertyToId.remove(property);
    //QtBrowserItem *item = propertyEditor->addProperty(property);
    //if (idToExpanded.contains(id))
    //    propertyEditor->setExpanded(item, idToExpanded[id]);
}

QtProperty* properties_item::GetProperty(const QString& id)
{
    if (idToProperty.contains(id))
        return idToProperty[id];
    else
        return nullptr;
}

QString properties_item::GetPropertyId(QtProperty* property)
{
    if (propertyToId.contains(property))
        return propertyToId[property];
    else
        return QString();
}

unit_types::ParameterModel* properties_item::GetParameterModel(QtProperty* property)
{
    unit_types::ParameterModel* pm = nullptr;

    QString id = GetPropertyId(property);
    if (id == "")
        return pm;

    {
        QStringList sl = id.split("/");
        auto ql = &parametersModel_.parameters;
        while (sl.size() > 0)
        {
            for (auto& x : *ql)
            {
                if (x.id == sl[0])
                {
                    pm = &x;
                    ql = &x.parameters;
                    sl.pop_front();
                    break;
                }
            }
        }
    }

    if (pm != nullptr)
        return pm;

    {
        QStringList sl = id.split("/");
        auto ql = &editorModel_.parameters;
        while (sl.size() > 0)
        {
            for (auto x : *ql)
            {
                if (x.id == sl[0])
                {
                    pm = &x;
                    ql = &x.parameters;
                    sl.pop_front();
                    break;
                }
            }
        }
    }

    return pm;
}

bool properties_item::GetExpanded(QtProperty* property)
{
    return false;
}

void properties_item::updateExpandState(QtTreePropertyBrowser* propertyEditor)
{
    QList<QtBrowserItem*> list = propertyEditor->topLevelItems();
    QListIterator<QtBrowserItem*> it(list);
    while (it.hasNext()) {
        QtBrowserItem* item = it.next();
        QtProperty* prop = item->property();
        idToExpanded[propertyToId[prop]] = propertyEditor->isExpanded(item);
    }
}

void properties_item::applyExpandState(QtTreePropertyBrowser* propertyEditor)
{
    QList<QtBrowserItem*> list = propertyEditor->topLevelItems();
    QListIterator<QtBrowserItem*> it(list);
    while (it.hasNext())
    {
        QtBrowserItem* item = it.next();
        QListIterator<QtBrowserItem*> it2 = item->children();
        while (it2.hasNext())
        {
            QtBrowserItem* item2 = it2.next();
            QListIterator<QtBrowserItem*> it3 = item2->children();
            while (it3.hasNext())
            {
                QtBrowserItem* item3 = it3.next();
                QtProperty* prop = item3->property();
                //propertyEditor->setExpanded(item, idToExpanded[propertyToId[prop]]);
                propertyEditor->setExpanded(item3, false);

            }
        }
        
    }
}
