#include "qtpropertymanager.h"
#include "qtexpropertymanager.h"
#include "qtpropertybrowserutils_p.h"

QT_BEGIN_NAMESPACE

template <class Value, class PrivateData>
static Value getData(const QMap<const QtProperty*, PrivateData>& propertyMap,
    Value PrivateData::* data,
    const QtProperty* property, const Value& defaultValue = Value())
{
    typedef QMap<const QtProperty*, PrivateData> PropertyToData;
    typedef typename PropertyToData::const_iterator PropertyToDataConstIterator;
    const PropertyToDataConstIterator it = propertyMap.constFind(property);
    if (it == propertyMap.constEnd())
        return defaultValue;
    return it.value().*data;
}

template <class Value, class PrivateData>
static Value getValue(const QMap<const QtProperty*, PrivateData>& propertyMap,
    const QtProperty* property, const Value& defaultValue = Value())
{
    return getData<Value>(propertyMap, &PrivateData::val, property, defaultValue);
}

// QtExStringPropertyManager

class QtExStringPropertyManagerPrivate
{
    QtExStringPropertyManager* q_ptr;
    Q_DECLARE_PUBLIC(QtExStringPropertyManager)
public:

    struct Data
    {
        Data() : regExp(QString(QLatin1Char('*')), Qt::CaseSensitive, QRegExp::Wildcard)
        {
        }
        QString val;
        QString oldVal;
        QRegExp regExp;
    };

    typedef QMap<const QtProperty*, Data> PropertyValueMap;
    QMap<const QtProperty*, Data> m_values;
};

/*!
    \class QtStringPropertyManager

    \brief The QtStringPropertyManager provides and manages QString properties.

    A string property's value can be retrieved using the value()
    function, and set using the setValue() slot.

    The current value can be checked against a regular expression. To
    set the regular expression use the setRegExp() slot, use the
    regExp() function to retrieve the currently set expression.

    In addition, QtStringPropertyManager provides the valueChanged() signal
    which is emitted whenever a property created by this manager
    changes, and the regExpChanged() signal which is emitted whenever
    such a property changes its currently set regular expression.

    \sa QtAbstractPropertyManager, QtLineEditFactory
*/

/*!
    \fn void QtStringPropertyManager::valueChanged(QtProperty *property, const QString &value)

    This signal is emitted whenever a property created by this manager
    changes its value, passing a pointer to the \a property and the
    new \a value as parameters.

    \sa setValue()
*/

/*!
    \fn void QtStringPropertyManager::regExpChanged(QtProperty *property, const QRegExp &regExp)

    This signal is emitted whenever a property created by this manager
    changes its currenlty set regular expression, passing a pointer to
    the \a property and the new \a regExp as parameters.

    \sa setRegExp()
*/

/*!
    Creates a manager with the given \a parent.
*/
QtExStringPropertyManager::QtExStringPropertyManager(QObject *parent)
    : QtAbstractPropertyManager(parent)
{
    d_ptr = new QtExStringPropertyManagerPrivate;
    d_ptr->q_ptr = this;
}

/*!
    Destroys this manager, and all the properties it has created.
*/
QtExStringPropertyManager::~QtExStringPropertyManager()
{
    clear();
    delete d_ptr;
}

/*!
    Returns the given \a property's value.

    If the given property is not managed by this manager, this
    function returns an empty string.

    \sa setValue()
*/
QString QtExStringPropertyManager::value(const QtProperty *property) const
{
    return getValue<QString>(d_ptr->m_values, property);
}

/*!
    Returns the given \a property's currently set regular expression.

    If the given \a property is not managed by this manager, this
    function returns an empty expression.

    \sa setRegExp()
*/
QRegExp QtExStringPropertyManager::regExp(const QtProperty *property) const
{
    return getData<QRegExp>(d_ptr->m_values, &QtExStringPropertyManagerPrivate::Data::regExp, property, QRegExp());
}

/*!
    \reimp
*/
QString QtExStringPropertyManager::valueText(const QtProperty *property) const
{
    const QtExStringPropertyManagerPrivate::PropertyValueMap::const_iterator it = d_ptr->m_values.constFind(property);
    if (it == d_ptr->m_values.constEnd())
        return QString();
    return it.value().val;
}

/*!
    \fn void QtStringPropertyManager::setValue(QtProperty *property, const QString &value)

    Sets the value of the given \a property to \a value.

    If the specified \a value doesn't match the given \a property's
    regular expression, this function does nothing.

    \sa value(), setRegExp(), valueChanged()
*/
void QtExStringPropertyManager::setValue(QtProperty *property, const QString &val)
{
    const QtExStringPropertyManagerPrivate::PropertyValueMap::iterator it = d_ptr->m_values.find(property);
    if (it == d_ptr->m_values.end())
        return;

    QtExStringPropertyManagerPrivate::Data data = it.value();

    if (data.val == val)
        return;

    if (data.regExp.isValid() && !data.regExp.exactMatch(val))
        return;

    data.val = val;

    it.value() = data;

    emit propertyChanged(property);
    emit valueChanged(property, data.val);
}

void QtExStringPropertyManager::setOldValue(QtProperty *property, const QString &val)
{
    const QtExStringPropertyManagerPrivate::PropertyValueMap::iterator it = d_ptr->m_values.find(property);
    if (it == d_ptr->m_values.end())
        return;

    QtExStringPropertyManagerPrivate::Data data = it.value();

    data.oldVal = val;

    it.value() = data;
}

void QtExStringPropertyManager::editingFinished(QtProperty *property)
{
    const QtExStringPropertyManagerPrivate::PropertyValueMap::iterator it = d_ptr->m_values.find(property);
    if (it == d_ptr->m_values.end())
        return;

    QtExStringPropertyManagerPrivate::Data data = it.value();

    if (data.val == data.oldVal)
        return;

	QString oldVal = data.oldVal;
    data.oldVal = data.val;
	
    it.value() = data;

    emit editingFinished(property, data.val, oldVal);
}

/*!
    Sets the regular expression of the given \a property to \a regExp.

    \sa regExp(), setValue(), regExpChanged()
*/
void QtExStringPropertyManager::setRegExp(QtProperty *property, const QRegExp &regExp)
{
    const QtExStringPropertyManagerPrivate::PropertyValueMap::iterator it = d_ptr->m_values.find(property);
    if (it == d_ptr->m_values.end())
        return;

    QtExStringPropertyManagerPrivate::Data data = it.value() ;

    if (data.regExp == regExp)
        return;

    data.regExp = regExp;

    it.value() = data;

    emit regExpChanged(property, data.regExp);
}

/*!
    \reimp
*/
void QtExStringPropertyManager::initializeProperty(QtProperty *property)
{
    d_ptr->m_values[property] = QtExStringPropertyManagerPrivate::Data();
}

/*!
    \reimp
*/
void QtExStringPropertyManager::uninitializeProperty(QtProperty *property)
{
    d_ptr->m_values.remove(property);
}

QT_END_NAMESPACE

#include "moc_qtexpropertymanager.cpp"
//#include "qtexpropertymanager.moc"
