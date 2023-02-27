#ifndef PROPERTIES_ITEM_H
#define PROPERTIES_ITEM_H

#include <QGraphicsItem>
#include <QObject>
#include <QPixmap>

class QtGroupPropertyManager;
class QtIntPropertyManager;
class QtDoublePropertyManager;
class QtStringPropertyManager;
class QtColorPropertyManager;
class QtFontPropertyManager;
class QtPointPropertyManager;
class QtSizePropertyManager;
class QtProperty;
class QtTreePropertyBrowser;

class properties_item : public QObject
{
    Q_OBJECT

private:
    MainWindow::UnitParameters unitParameters_;

    QtGroupPropertyManager *groupManager;
    QtIntPropertyManager *intManager;
    QtDoublePropertyManager *doubleManager;
    QtStringPropertyManager *stringManager;
    QtColorPropertyManager *colorManager;
    QtFontPropertyManager *fontManager;
    QtPointPropertyManager *pointManager;
    QtSizePropertyManager *sizeManager;

private:
    void CreatePropertyBrowser();

private slots:
    void valueChanged(QtProperty* property, int value);
    void valueChanged(QtProperty* property, double value);
    void valueChanged(QtProperty* property, const QString& value);
    void valueChanged(QtProperty* property, const QColor& value);
    void valueChanged(QtProperty* property, const QFont& value);
    void valueChanged(QtProperty* property, const QPoint& value);
    void valueChanged(QtProperty* property, const QSize& value);

public:
    properties_item(MainWindow::UnitParameters unitParameters, QObject* parent = nullptr);

public:
    QString getName() { return QString::fromStdString(unitParameters_.fiileInfo.info.id); };
    void ApplyToBrowser(QtTreePropertyBrowser* propertyEditor);
    QPixmap GetPixmap();
};

#endif // PROPERTIES_ITEM_H