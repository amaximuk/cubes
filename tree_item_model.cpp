#include <QMimeData>
#include <QDataStream>
#include "tree_item_model.h"

tree_item_model::tree_item_model(QObject *parent) : QStandardItemModel(parent)
//tree_item::tree_item(QObject *parent) : QAbstractItemModel(parent)
{

}

Qt::DropActions tree_item_model::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

QStringList tree_item_model::mimeTypes() const
{
    QStringList types;
    types << "application/x-dnditemdata";
    return types;
}

QMimeData *tree_item_model::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (QModelIndex index, indexes) {
        if (index.isValid()) {
            QString text = data(index, Qt::DisplayRole).toString();
            stream << text;
        }
    }

    mimeData->setData("application/x-dnditemdata", encodedData);
    return mimeData;
}
