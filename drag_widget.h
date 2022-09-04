#ifndef DRAG_WIDGET_H
#define DRAG_WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDropEvent;
QT_END_NAMESPACE

class drag_widget : public QWidget
{
    Q_OBJECT

public:
    explicit drag_widget(QPixmap pixmap, QString name, QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:

};

#endif // DRAG_WIDGET_H
