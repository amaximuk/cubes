#include <QMimeData>
#include <QDataStream>
#include "tree_item_model.h"

TreeItemModel::TreeItemModel(QObject *parent) : QStandardItemModel(parent)
//tree_item::tree_item(QObject *parent) : QAbstractItemModel(parent)
{

}

Qt::DropActions TreeItemModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

QStringList TreeItemModel::mimeTypes() const
{
    QStringList types;
    types << "application/x-dnditemdata";
    return types;
}

QMimeData *TreeItemModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (QModelIndex index, indexes) {
        if (index.isValid()) {
            QString text = data(index, Qt::DisplayRole).toString();
            QPoint point = data(index, Qt::UserRole + 1).toPoint();
            stream << text << point;
        }
    }

    mimeData->setData("application/x-dnditemdata", encodedData);
    return mimeData;
}
