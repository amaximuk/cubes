#include <QMimeData>
#include <QDataStream>
#include <QBrush>
#include <QIcon>
#include "log_table_model.h"

log_table_model::log_table_model(QObject *parent) : QAbstractTableModel(parent)
{
}

void log_table_model::addMessage(log_message message)
{
    insertRows(log_messages.size(), 1);
    QModelIndex topLeft = createIndex(log_messages.size() - 1, 0);
    QModelIndex bottomRight = createIndex(log_messages.size() - 1, 1);

    // emit a signal to make the view reread identified data
    emit dataChanged(topLeft, bottomRight, { Qt::DisplayRole });

    log_messages[log_messages.size() - 1] = message;
    //log_messages.push_back(message);
}

int log_table_model::rowCount(const QModelIndex& parent) const
{
    return log_messages.size();
}

int log_table_model::columnCount(const QModelIndex& parent) const
{
    return 2;
}

QVariant log_table_model::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= log_messages.size())
        return QVariant();

    if (index.column() == 1 && role == Qt::DecorationRole)
    {
        return QIcon(":/images/cubes.png");
    }
    if (role == Qt::DisplayRole)
    {
        if (index.column() == 0)
            return QVariant((int)log_messages[index.row()].type);
        else if (index.column() == 1)
             return log_messages[index.row()].description;
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

bool log_table_model::insertRows(int position, int rows, const QModelIndex& parent)
{
    beginInsertRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row) {
        log_messages.insert(position, {});
    }

    endInsertRows();
    return true;
}

bool log_table_model::removeRows(int position, int rows, const QModelIndex& parent)
{
    beginRemoveRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row) {
        log_messages.removeAt(position);
    }

    endRemoveRows();
    return true;
}
