#pragma once

#include <QStandardItemModel>

class TreeItemModel : public QStandardItemModel
{
public:
    explicit TreeItemModel(QObject *parent = nullptr);
protected:
    Qt::DropActions supportedDropActions() const override;
    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
};
