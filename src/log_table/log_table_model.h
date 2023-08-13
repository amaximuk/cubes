#pragma once

#include <QAbstractTableModel>

class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;

namespace CubeLog
{
    enum class MessageType
    {
        information,
        warning,
        error
    };

    inline MessageType operator|(MessageType lhs, MessageType rhs)
    {
        return static_cast<MessageType>(
            static_cast<std::underlying_type<MessageType>::type>(lhs) |
            static_cast<std::underlying_type<MessageType>::type>(rhs));
    }

    inline uint qHash(MessageType key, uint seed)
    {
        return ::qHash(static_cast<uint>(key), seed);
    }

    struct LogMessage
    {
        MessageType type;
        QString source;
        QString description;
    };

    class LogTableModel : public QAbstractTableModel
    {
    private:
        QVector<LogMessage> log_messages_;

    public:
        explicit LogTableModel(QObject* parent = nullptr);

    public:
        void AddMessage(const LogMessage& message);
        void Clear();

    private:
        int rowCount(const QModelIndex& parent) const override;
        int columnCount(const QModelIndex& parent) const override;
        QVariant data(const QModelIndex& index, int role) const override;
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    };
}
