#pragma once
#include "qtexpropertymanager.h"

QT_BEGIN_NAMESPACE

class QtExLineEditFactoryPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtExLineEditFactory : public QtAbstractEditorFactory<QtExStringPropertyManager>
{
    Q_OBJECT
public:
    QtExLineEditFactory(QObject *parent = 0);
    ~QtExLineEditFactory();
protected:
    void connectPropertyManager(QtExStringPropertyManager *manager);
    QWidget *createEditor(QtExStringPropertyManager *manager, QtProperty *property,
                QWidget *parent);
    void disconnectPropertyManager(QtExStringPropertyManager *manager);
private:
    QtExLineEditFactoryPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtExLineEditFactory)
    Q_DISABLE_COPY(QtExLineEditFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, const QString &))
    Q_PRIVATE_SLOT(d_func(), void slotRegExpChanged(QtProperty *, const QRegExp &))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(const QString &))
    Q_PRIVATE_SLOT(d_func(), void slotEditingFinished())
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

QT_END_NAMESPACE
