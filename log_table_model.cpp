#include <QMimeData>
#include <QDataStream>
#include <QBrush>
#include <QIcon>
#include "log_table_model.h"

log_table_model::log_table_model(QObject *parent) : QAbstractTableModel(parent)
{
    //this->setHeaderData(0, Qt::Horizontal, QObject::tr("Annual Pay"), Qt::DisplayRole);
    //this->setHeaderData(1, Qt::Horizontal, QObject::tr("First Name"), Qt::DisplayRole);
    //this->setHeaderData(2, Qt::Horizontal, QObject::tr("Last Name"), Qt::DisplayRole);

}
int log_table_model::rowCount(const QModelIndex& parent) const
{
    return 20; // сделаем фиксированно 5 строк в таблице
    //если вы станете использовать скажем QList, то пишите return list.size();
}
int log_table_model::columnCount(const QModelIndex& parent) const
{
    return 4; // количество колонок сделаем также фиксированным
}

QVariant log_table_model::data(const QModelIndex& index, int role) const
{
    if (index.column() == 0 && role == Qt::DecorationRole)
    {
        return QIcon(":/images/cubes.png");
    }
    if (role == Qt::DisplayRole) {
        QString unswer = QString("row = ") + QString::number(index.row()) + "  col = " + QString::number(index.column());
        // строкой выше мы формируем ответ. QString::number преобразует число в текст
        return QVariant(unswer);
    }
    return QVariant();
}

QVariant log_table_model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
            return QString("Header #%1").arg(section);
        //else
        //    return {};
    }

    //if (role == Qt::FontRole)
    //{
    //    QFont serifFont("Times", 10, QFont::Bold, true);
    //    return serifFont;
    //}

    //if (role == Qt::TextAlignmentRole)
    //{
    //    return Qt::AlignRight;
    //}

    //if (role == Qt::BackgroundRole)
    //{
    //    return QBrush(Qt::blue);
    //}

    //if (role == Qt::ForegroundRole)
    //{
    //    return QBrush(Qt::red);
    //}

    return QAbstractTableModel::headerData(section, orientation, role);
}



#include <QtWidgets>

//! [0]
MySortFilterProxyModel::MySortFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
}
//! [0]

//! [1]
void MySortFilterProxyModel::setFilterMinimumDate(QDate date)
{
    minDate = date;
    invalidateFilter();
}
//! [1]

//! [2]
void MySortFilterProxyModel::setFilterMaximumDate(QDate date)
{
    maxDate = date;
    invalidateFilter();
}
//! [2]

//! [3]
bool MySortFilterProxyModel::filterAcceptsRow(int sourceRow,
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
//! [3]

//! [4] //! [5]
bool MySortFilterProxyModel::lessThan(const QModelIndex& left,
    const QModelIndex& right) const
{

    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);
    //! [4]

    
    if (leftData.toString() < rightData.toString())
        return false;
    
    return true;

    //! [6]
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
//! [5] //! [6]

//! [7]
bool MySortFilterProxyModel::dateInRange(QDate date) const
{
    return (!minDate.isValid() || date > minDate)
        && (!maxDate.isValid() || date < maxDate);
}
//! [7]