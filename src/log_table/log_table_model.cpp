#include <QMimeData>
#include <QDataStream>
#include <QBrush>
#include <QIcon>
#include "log_table_model.h"

using namespace CubesLog;

LogTableModel::LogTableModel(QObject *parent) : QAbstractTableModel(parent)
{
}

void LogTableModel::AddMessage(const LogMessage& message)
{
    beginInsertRows(QModelIndex(), log_messages_.size(), log_messages_.size());
    log_messages_.push_back(message);
    endInsertRows();

    //insertRows(log_messages.size(), 1);
    QModelIndex topLeft = createIndex(log_messages_.size() - 1, 0);
    QModelIndex bottomRight = createIndex(log_messages_.size() - 1, 1);

    // emit a signal to make the view reread identified data
    emit dataChanged(topLeft, bottomRight, { Qt::DisplayRole });

    //log_messages[log_messages.size() - 1] = message;
}

void LogTableModel::Clear()
{
    if (log_messages_.size() > 0)
    {
        beginRemoveRows(QModelIndex(), 0, log_messages_.size() - 1);
        log_messages_.clear();
        endRemoveRows();
    }
}

int LogTableModel::rowCount(const QModelIndex& parent) const
{
    return log_messages_.size();
}

int LogTableModel::columnCount(const QModelIndex& parent) const
{
    return 3;
}

QVariant LogTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= log_messages_.size())
        return QVariant();

    if (role == Qt::DecorationRole)
    {
        if (index.column() == 0)
        {
            if (log_messages_[index.row()].type == MessageType::information)
                return QIcon(":/images/information.png");
            else if (log_messages_[index.row()].type == MessageType::warning)
                return QIcon(":/images/warning.png");
            else if (log_messages_[index.row()].type == MessageType::error)
                return QIcon(":/images/error.png");
        }
    }
    else if (role == Qt::DisplayRole)
    {
        if (index.column() == 1)
            return QString("%1 (%2)").arg(log_messages_[index.row()].source).arg(log_messages_[index.row()].tag);
        else if (index.column() == 2)
            return log_messages_[index.row()].description;
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
            if (log_messages_[index.row()].type == MessageType::information)
                return static_cast<uint32_t>(MessageType::information);
            else if (log_messages_[index.row()].type == MessageType::warning)
                return static_cast<uint32_t>(MessageType::warning);
            else if (log_messages_[index.row()].type == MessageType::error)
                return static_cast<uint32_t>(MessageType::error);
        }
        else if (index.column() == 1)
            return log_messages_[index.row()].source;
        else if (index.column() == 2)
            return log_messages_[index.row()].description;
    }
    return QVariant();
}

QVariant LogTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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
