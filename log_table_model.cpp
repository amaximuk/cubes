#include <QMimeData>
#include <QDataStream>
#include <QBrush>
#include <QIcon>
#include "log_table_model.h"

log_table_model::log_table_model(QObject *parent) : QAbstractTableModel(parent)
{
}

void log_table_model::addMessage(const log_message& message)
{
    beginInsertRows(QModelIndex(), log_messages.size(), log_messages.size());
    log_messages.push_back(message);
    endInsertRows();

    //insertRows(log_messages.size(), 1);
    QModelIndex topLeft = createIndex(log_messages.size() - 1, 0);
    QModelIndex bottomRight = createIndex(log_messages.size() - 1, 1);

    // emit a signal to make the view reread identified data
    emit dataChanged(topLeft, bottomRight, { Qt::DisplayRole });

    //log_messages[log_messages.size() - 1] = message;
}

void log_table_model::clear()
{
    if (log_messages.size() > 0)
    {
        beginRemoveRows(QModelIndex(), 0, log_messages.size() - 1);
        log_messages.clear();
        endRemoveRows();
    }
}

int log_table_model::rowCount(const QModelIndex& parent) const
{
    return log_messages.size();
}

int log_table_model::columnCount(const QModelIndex& parent) const
{
    return 3;
}

QVariant log_table_model::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= log_messages.size())
        return QVariant();

    if (role == Qt::DecorationRole)
    {
        if (index.column() == 0)
        {
            if (log_messages[index.row()].type == message_type::information)
                return QIcon(":/images/information.png");
            else if (log_messages[index.row()].type == message_type::warning)
                return QIcon(":/images/warning.png");
            else if (log_messages[index.row()].type == message_type::error)
                return QIcon(":/images/error.png");
        }
    }
    else if (role == Qt::DisplayRole)
    {
        if (index.column() == 1)
            return log_messages[index.row()].source;
        else if (index.column() == 2)
            return log_messages[index.row()].description;
    }
    else if (role == Qt::UserRole)
    {
        if (index.column() == 0)
        {
            //if (log_messages[index.row()].type == message_type::information)
            //    return QString::fromLocal8Bit("0-information");
            //else if (log_messages[index.row()].type == message_type::warning)
            //    return QString::fromLocal8Bit("1-warning");
            //else if (log_messages[index.row()].type == message_type::error)
            //    return QString::fromLocal8Bit("2-error");
            if (log_messages[index.row()].type == message_type::information)
                return static_cast<uint32_t>(message_type::information);
            else if (log_messages[index.row()].type == message_type::warning)
                return static_cast<uint32_t>(message_type::warning);
            else if (log_messages[index.row()].type == message_type::error)
                return static_cast<uint32_t>(message_type::error);
        }
        else if (index.column() == 1)
            return log_messages[index.row()].source;
        else if (index.column() == 2)
            return log_messages[index.row()].description;
    }
    return QVariant();
}

QVariant log_table_model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            if (section == 0)
                return "";
            else if (section == 1)
                return QString::fromLocal8Bit("Источник");
            else if (section == 2)
                return QString::fromLocal8Bit("Описание");
        }
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

//bool log_table_model::insertRows(int position, int rows, const QModelIndex& parent)
//{
//    beginInsertRows(QModelIndex(), position, position + rows - 1);
//
//    for (int row = 0; row < rows; ++row) {
//        log_messages.insert(position, {});
//    }
//
//    endInsertRows();
//    return true;
//}
//
//bool log_table_model::removeRows(int position, int rows, const QModelIndex& parent)
//{
//    beginRemoveRows(QModelIndex(), position, position + rows - 1);
//
//    for (int row = 0; row < rows; ++row) {
//        log_messages.removeAt(position);
//    }
//
//    endRemoveRows();
//    return true;
//}
