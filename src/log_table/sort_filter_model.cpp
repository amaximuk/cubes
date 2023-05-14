#include <QtWidgets>
#include <QMimeData>
#include <QDataStream>
#include <QBrush>
#include <QIcon>
#include "sort_filter_model.h"

sort_filter_model::sort_filter_model(QObject* parent)
    : QSortFilterProxyModel(parent)
{
}

void sort_filter_model::setFilter(QSet<message_type> filter)
{
    filter_ = filter;
    invalidateFilter();
}

void sort_filter_model::addToFilter(message_type mt)
{
    filter_.insert(mt);
    invalidateFilter();
}

void sort_filter_model::removeFromFilter(message_type mt)
{
    filter_.remove(mt);
    invalidateFilter();
}

bool sort_filter_model::filterAcceptsRow(int sourceRow,
    const QModelIndex& sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    message_type t = static_cast<message_type>(sourceModel()->data(index, Qt::UserRole).toUInt());
    if (filter_.contains(t))
        return true;
    else
        return false;
}

bool sort_filter_model::lessThan(const QModelIndex& left,
    const QModelIndex& right) const
{
    if (left.column() == 0)
    {
        //message_type leftMt = static_cast<message_type>(sourceModel()->data(left, Qt::UserRole).toUInt());
        //message_type rightMt = static_cast<message_type>(sourceModel()->data(right, Qt::UserRole).toUInt());
        uint32_t leftMt = sourceModel()->data(left, Qt::UserRole).toUInt();
        uint32_t rightMt = sourceModel()->data(right, Qt::UserRole).toUInt();

        if (leftMt < rightMt)
            return true;
        else
            return false;
    }
    else
    {
        QVariant leftData = sourceModel()->data(left);
        QVariant rightData = sourceModel()->data(right);
        //QString leftString = leftData.toString();
        //QString rightString = rightData.toString();

        //qDebug() << left.row() << " : " << leftString << " --- " << right.row() << " : " <<
        //    rightString << " --- " << (leftData.toString() < rightData.toString());

        if (leftData.toString() < rightData.toString())
            return true;
        else
            return false;
    }
    return false;
}
