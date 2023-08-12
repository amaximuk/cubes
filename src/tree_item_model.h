#pragma once

#include <QAbstractItemModel>
#include <QStandardItemModel>

class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;

class TreeItemModel : public QStandardItemModel
{
public:
    explicit TreeItemModel(QObject *parent = nullptr);
protected:
    Qt::DropActions supportedDropActions() const override;
    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
};
