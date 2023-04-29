#pragma once

#include <QAbstractTableModel>

QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;
QT_END_NAMESPACE

enum class message_type
{
    information,
    warning,
    error
};

inline message_type operator|(message_type lhs, message_type rhs)
{
    return static_cast<message_type>(
        static_cast<std::underlying_type<message_type>::type>(lhs) |
        static_cast<std::underlying_type<message_type>::type>(rhs));
}

inline uint qHash(message_type key, uint seed)
{
    return qHash(static_cast<uint>(key), seed);
}

struct log_message
{
    message_type type;
    QString description;
};

class log_table_model : public QAbstractTableModel
{
private:
    QVector<log_message> log_messages;

public:
    explicit log_table_model(QObject *parent = nullptr);

public:
    void addMessage(log_message message);

private:
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    //bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex()) override;
    //bool removeRows(int position, int rows, const QModelIndex& index = QModelIndex()) override;
};
