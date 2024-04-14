#pragma once

#include <QAbstractTableModel>
#include "log_table_interface.h"

class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;

namespace CubesLog
{
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
