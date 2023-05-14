#pragma once

#include <QDate>
#include <QSet>
#include <QSortFilterProxyModel>
#include "log_table_model.h"

class sort_filter_model : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    sort_filter_model(QObject* parent = 0);

public:
    void setFilter(QSet<message_type> filter);
    void addToFilter(message_type mt);
    void removeFromFilter(message_type mt);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;

private:
    QSet<message_type> filter_;
};