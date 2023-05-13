#pragma once

#include "qtpropertybrowser.h"

#if QT_VERSION >= 0x040400
QT_BEGIN_NAMESPACE
#endif

class QtExStringPropertyManagerPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtExStringPropertyManager : public QtAbstractPropertyManager
{
    Q_OBJECT
public:
    QtExStringPropertyManager(QObject *parent = 0);
    ~QtExStringPropertyManager();

    QString value(const QtProperty *property) const;
    QRegExp regExp(const QtProperty *property) const;

public Q_SLOTS:
    void setOldValue(QtProperty *property, const QString &val);
    void setValue(QtProperty *property, const QString &val);
    void editingFinished(QtProperty *property);
    void setRegExp(QtProperty *property, const QRegExp &regExp);
Q_SIGNALS:
    void valueChanged(QtProperty *property, const QString &val);
    void editingFinished(QtProperty *property, const QString &val, const QString &oldVal);
    void regExpChanged(QtProperty *property, const QRegExp &regExp);
protected:
    QString valueText(const QtProperty *property) const;
    virtual void initializeProperty(QtProperty *property);
    virtual void uninitializeProperty(QtProperty *property);
private:
    QtExStringPropertyManagerPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtExStringPropertyManager)
    Q_DISABLE_COPY(QtExStringPropertyManager)
};

#if QT_VERSION >= 0x040400
QT_END_NAMESPACE
#endif
