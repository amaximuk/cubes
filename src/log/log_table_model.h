#pragma once

#include <QAbstractTableModel>
#include "log_manager_interface.h"

class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;

namespace CubesLog
{
    class LogTableModel : public QAbstractTableModel
    {
    private:
        QVector<Message> log_messages_;

    public:
        explicit LogTableModel(QObject* parent = nullptr);

    public:
        void AddMessage(const Message& message);
        bool GetMessage(int row, Message& message);
        void Clear();

    private:
        int rowCount(const QModelIndex& parent) const override;
        int columnCount(const QModelIndex& parent) const override;
        QVariant data(const QModelIndex& index, int role) const override;
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    };
}
