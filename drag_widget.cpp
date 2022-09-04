#include <QtWidgets>
#include "drag_widget.h"

drag_widget::drag_widget(QPixmap pixmap, QString name, QWidget *parent):
    QWidget(parent),
    pixmap_(pixmap),
    name_(name)
{
    QLayout *lwd = new QVBoxLayout(this);
    QLabel *icn = new QLabel();
    icn->setPixmap(pixmap);
    icn->move(10, 10);
    icn->show();
    icn->setAttribute(Qt::WA_DeleteOnClose);
    QLabel *text = new QLabel();
    text->setText(name);
    lwd->addWidget(icn);
    lwd->addWidget(text);
}

void drag_widget::mousePressEvent(QMouseEvent *event)
{
    QLabel *child = static_cast<QLabel*>(childAt(event->pos()));
    if (!child)
        return;

    QPixmap pixmap = child->pixmap(Qt::ReturnByValue);
    if (pixmap.isNull())
        return;

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << pixmap << QPoint(event->pos() - child->pos()) << name_;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-dnditemdata", itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);
    drag->setHotSpot(event->pos() - child->pos());

    QPixmap tempPixmap = pixmap;
    QPainter painter;
    painter.begin(&tempPixmap);
    painter.fillRect(pixmap.rect(), QColor(127, 127, 127, 127));
    painter.end();
    child->setPixmap(tempPixmap);

    drag->exec(Qt::CopyAction);
    child->setPixmap(pixmap);
}
