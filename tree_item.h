#ifndef TREE_ITEM_H
#define TREE_ITEM_H

#include <QAbstractItemModel>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;
QT_END_NAMESPACE


class tree_item : public QStandardItemModel
//class tree_item : public QAbstractItemModel
{
public:
    explicit tree_item(QObject *parent = nullptr);
protected:
    Qt::DropActions supportedDropActions() const override;
    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
};

#endif // TREE_ITEM_H
