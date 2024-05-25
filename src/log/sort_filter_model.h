#pragma once

#include <QSortFilterProxyModel>
#include "log_manager_interface.h"
#include "log_table_model.h"

namespace CubesLog
{
    class SortFilterModel : public QSortFilterProxyModel
    {
        Q_OBJECT

    private:
        QSet<MessageType> filter_;

    public:
        SortFilterModel(QObject* parent = 0);

    public:
        void SetFilter(QSet<MessageType> filter);
        void AddToFilter(MessageType mt);
        void RemoveFromFilter(MessageType mt);

    protected:
        bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
        bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;
    };
}
