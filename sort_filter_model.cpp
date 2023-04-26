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

void sort_filter_model::setFilterMinimumDate(QDate date)
{
    minDate = date;
    invalidateFilter();
}

void sort_filter_model::setFilterMaximumDate(QDate date)
{
    maxDate = date;
    invalidateFilter();
}

bool sort_filter_model::filterAcceptsRow(int sourceRow,
    const QModelIndex& sourceParent) const
{

    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    QModelIndex index1 = sourceModel()->index(sourceRow, 1, sourceParent);
    QModelIndex index2 = sourceModel()->index(sourceRow, 2, sourceParent);
    QString s1 = sourceModel()->data(index0).toString();
    QString s2 = sourceModel()->data(index1).toString();
    QString s3 = sourceModel()->data(index2).toString();
    if (s1 == "row = 1  col = 0")
        return false;
    return true;
    return (sourceModel()->data(index0).toString().contains(filterRegExp())
        || sourceModel()->data(index1).toString().contains(filterRegExp()))
        && dateInRange(sourceModel()->data(index2).toDate());
}

bool sort_filter_model::lessThan(const QModelIndex& left,
    const QModelIndex& right) const
{

    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);
    
    if (leftData.toString() < rightData.toString())
        return false;
    
    return true;

    if (leftData.userType() == QMetaType::QDateTime) {
        return leftData.toDateTime() < rightData.toDateTime();
    }
    else {
        static const QRegularExpression emailPattern("[\\w\\.]*@[\\w\\.]*");

        QString leftString = leftData.toString();
        if (left.column() == 1) {
            const QRegularExpressionMatch match = emailPattern.match(leftString);
            if (match.hasMatch())
                leftString = match.captured(0);
        }
        QString rightString = rightData.toString();
        if (right.column() == 1) {
            const QRegularExpressionMatch match = emailPattern.match(rightString);
            if (match.hasMatch())
                rightString = match.captured(0);
        }

        return QString::localeAwareCompare(leftString, rightString) < 0;
    }
}

bool sort_filter_model::dateInRange(QDate date) const
{
    return (!minDate.isValid() || date > minDate)
        && (!maxDate.isValid() || date < maxDate);
}
