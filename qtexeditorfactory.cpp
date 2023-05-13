#include "qteditorfactory.h"
#include "qtexeditorfactory.h"
#include "qtpropertybrowserutils_p.h"
#include <QAbstractItemView>
#include <QLineEdit>

//Q_NAMESPACE

// ---------- EditorFactoryPrivate :
// Base class for editor factory private classes. Manages mapping of properties to editors and vice versa.

template <class Editor>
class EditorFactoryPrivate
{
public:

    typedef QList<Editor*> EditorList;
    typedef QMap<QtProperty*, EditorList> PropertyToEditorListMap;
    typedef QMap<Editor*, QtProperty*> EditorToPropertyMap;

    Editor* createEditor(QtProperty* property, QWidget* parent);
    void initializeEditor(QtProperty* property, Editor* e);
    void slotEditorDestroyed(QObject* object);

    PropertyToEditorListMap  m_createdEditors;
    EditorToPropertyMap m_editorToProperty;
};

template <class Editor>
Editor* EditorFactoryPrivate<Editor>::createEditor(QtProperty* property, QWidget* parent)
{
    Editor* editor = new Editor(parent);
    initializeEditor(property, editor);
    return editor;
}

template <class Editor>
void EditorFactoryPrivate<Editor>::initializeEditor(QtProperty* property, Editor* editor)
{
    typename PropertyToEditorListMap::iterator it = m_createdEditors.find(property);
    if (it == m_createdEditors.end())
        it = m_createdEditors.insert(property, EditorList());
    it.value().append(editor);
    m_editorToProperty.insert(editor, property);
}

template <class Editor>
void EditorFactoryPrivate<Editor>::slotEditorDestroyed(QObject* object)
{
    const typename EditorToPropertyMap::iterator ecend = m_editorToProperty.end();
    for (typename EditorToPropertyMap::iterator itEditor = m_editorToProperty.begin(); itEditor != ecend; ++itEditor) {
        if (itEditor.key() == object) {
            Editor* editor = itEditor.key();
            QtProperty* property = itEditor.value();
            const typename PropertyToEditorListMap::iterator pit = m_createdEditors.find(property);
            if (pit != m_createdEditors.end()) {
                pit.value().removeAll(editor);
                if (pit.value().empty())
                    m_createdEditors.erase(pit);
            }
            m_editorToProperty.erase(itEditor);
            return;
        }
    }
}


// QtExLineEditFactory

class QtExLineEditFactoryPrivate : public EditorFactoryPrivate<QLineEdit>
{
    QtExLineEditFactory *q_ptr;
    Q_DECLARE_PUBLIC(QtExLineEditFactory)
public:

    void slotPropertyChanged(QtProperty *property, const QString &value);
    void slotRegExpChanged(QtProperty *property, const QRegExp &regExp);
    void slotSetValue(const QString &value);
    void slotEditingFinished();
};

void QtExLineEditFactoryPrivate::slotPropertyChanged(QtProperty *property,
                const QString &value)
{
    if (!m_createdEditors.contains(property))
        return;

    QListIterator<QLineEdit *> itEditor( m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QLineEdit *editor = itEditor.next();
        if (editor->text() != value)
            editor->setText(value);
    }
}

void QtExLineEditFactoryPrivate::slotRegExpChanged(QtProperty *property,
            const QRegExp &regExp)
{
    if (!m_createdEditors.contains(property))
        return;

    QtExStringPropertyManager *manager = q_ptr->propertyManager(property);
    if (!manager)
        return;

    QListIterator<QLineEdit *> itEditor(m_createdEditors[property]);
    while (itEditor.hasNext()) {
        QLineEdit *editor = itEditor.next();
        editor->blockSignals(true);
        const QValidator *oldValidator = editor->validator();
        QValidator *newValidator = 0;
        if (regExp.isValid()) {
            newValidator = new QRegExpValidator(regExp, editor);
        }
        editor->setValidator(newValidator);
        if (oldValidator)
            delete oldValidator;
        editor->blockSignals(false);
    }
}

void QtExLineEditFactoryPrivate::slotSetValue(const QString &value)
{
    QObject *object = q_ptr->sender();
    const QMap<QLineEdit *, QtProperty *>::ConstIterator ecend = m_editorToProperty.constEnd();
    for (QMap<QLineEdit *, QtProperty *>::ConstIterator itEditor = m_editorToProperty.constBegin(); itEditor != ecend; ++itEditor)
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtExStringPropertyManager *manager = q_ptr->propertyManager(property);
            if (!manager)
                return;
            manager->setValue(property, value);
            return;
        }
}

void QtExLineEditFactoryPrivate::slotEditingFinished()
{
    QObject *object = q_ptr->sender();
    const QMap<QLineEdit *, QtProperty *>::ConstIterator ecend = m_editorToProperty.constEnd();
    for (QMap<QLineEdit *, QtProperty *>::ConstIterator itEditor = m_editorToProperty.constBegin(); itEditor != ecend; ++itEditor)
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtExStringPropertyManager *manager = q_ptr->propertyManager(property);
            if (!manager)
                return;
			if (!itEditor.key()->isModified())
				return;
			itEditor.key()->setModified(false);
            manager->editingFinished(property);
            return;
        }
}

/*!
    \class QtLineEditFactory

    \brief The QtLineEditFactory class provides QLineEdit widgets for
    properties created by QtStringPropertyManager objects.

    \sa QtAbstractEditorFactory, QtStringPropertyManager
*/

/*!
    Creates a factory with the given \a parent.
*/
QtExLineEditFactory::QtExLineEditFactory(QObject *parent)
    : QtAbstractEditorFactory<QtExStringPropertyManager>(parent)
{
    d_ptr = new QtExLineEditFactoryPrivate();
    d_ptr->q_ptr = this;

}

/*!
    Destroys this factory, and all the widgets it has created.
*/
QtExLineEditFactory::~QtExLineEditFactory()
{
    qDeleteAll(d_ptr->m_editorToProperty.keys());
    delete d_ptr;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtExLineEditFactory::connectPropertyManager(QtExStringPropertyManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty *, const QString &)),
                this, SLOT(slotPropertyChanged(QtProperty *, const QString &)));
    connect(manager, SIGNAL(regExpChanged(QtProperty *, const QRegExp &)),
                this, SLOT(slotRegExpChanged(QtProperty *, const QRegExp &)));
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
QWidget *QtExLineEditFactory::createEditor(QtExStringPropertyManager *manager,
        QtProperty *property, QWidget *parent)
{

    QLineEdit *editor = d_ptr->createEditor(property, parent);
    QRegExp regExp = manager->regExp(property);
    if (regExp.isValid()) {
        QValidator *validator = new QRegExpValidator(regExp, editor);
        editor->setValidator(validator);
    }
    editor->setText(manager->value(property));

    connect(editor, SIGNAL(textEdited(const QString &)),
                this, SLOT(slotSetValue(const QString &)));
    connect(editor, SIGNAL(editingFinished()),
                this, SLOT(slotEditingFinished()));
    connect(editor, SIGNAL(destroyed(QObject *)),
                this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtExLineEditFactory::disconnectPropertyManager(QtExStringPropertyManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, const QString &)),
                this, SLOT(slotPropertyChanged(QtProperty *, const QString &)));
    disconnect(manager, SIGNAL(regExpChanged(QtProperty *, const QRegExp &)),
                this, SLOT(slotRegExpChanged(QtProperty *, const QRegExp &)));
}

#include "moc_qtexeditorfactory.cpp"
//#include "qtexeditorfactory.moc"
