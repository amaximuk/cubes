#pragma once

#include <QDate>
#include <QSortFilterProxyModel>
#include "log_table_model.h"

class sort_filter_model : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    sort_filter_model(QObject* parent = 0);

    QDate filterMinimumDate() const { return minDate; }
    void setFilterMinimumDate(QDate date);

    QDate filterMaximumDate() const { return maxDate; }
    void setFilterMaximumDate(QDate date);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;

private:
    bool dateInRange(QDate date) const;

    QDate minDate;
    QDate maxDate;
};