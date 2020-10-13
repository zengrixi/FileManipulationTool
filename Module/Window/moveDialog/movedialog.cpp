#include "movedialog.h"

#include <QMouseEvent>
#include <QPainter>

constexpr int WIDTH = 4;
constexpr int HEIGHT = 4;

MoveDialog::MoveDialog(QWidget *parent)
    : MoveDialog(true, parent)
{

}

MoveDialog::MoveDialog(bool transparent, QWidget *parent)
    : QDialog(parent)
    , _moveOption(false)
    , _showShadow(true)
    , _leftButtonPress(false)
{
    // 删除标题栏
    setWindowFlags( Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, transparent);
}

void MoveDialog::paintEvent(QPaintEvent *event)
{
    QDialog::paintEvent(event);

    if (_showShadow)            //显示阴影
    {
        QPainter painter(this);

        painter.drawPixmap(0, 0, WIDTH, HEIGHT, QPixmap(":/res/lb_left_top"));
        painter.drawPixmap(width() - WIDTH, 0, WIDTH, HEIGHT, QPixmap(":/res/lb_right_top"));
        painter.drawPixmap(0, height() - HEIGHT, WIDTH, HEIGHT, QPixmap(":/res/lb_left_bottom"));
        painter.drawPixmap(width() - WIDTH, height() - HEIGHT, WIDTH, HEIGHT, QPixmap(":/res/lb_right_bottom"));

        painter.drawPixmap(0, WIDTH, HEIGHT, height() - 2*WIDTH,
                           QPixmap(":/res/lb_left").scaled(WIDTH, height() - 2*HEIGHT));
        painter.drawPixmap(width() - WIDTH, WIDTH, HEIGHT, height() - 2*HEIGHT,
                           QPixmap(":/res/lb_right").scaled(WIDTH, height() - 2*HEIGHT));
        painter.drawPixmap(HEIGHT, 0, width() - 2*WIDTH, HEIGHT,
                           QPixmap(":/res/lb_top").scaled(width() - 2*WIDTH, HEIGHT));
        painter.drawPixmap(WIDTH, height() - HEIGHT, width() - 2*WIDTH, HEIGHT,
                           QPixmap(":/res/lb_bottom").scaled(width() - 2*WIDTH, HEIGHT));
    }
}

void MoveDialog::mousePressEvent(QMouseEvent *event)
{
    QDialog::mousePressEvent(event);
    if (event->button() == Qt::LeftButton && !_moveOption)            //按左键
    {
        _leftButtonPress = true;
    }
    _pressAt = event->globalPos();
}

void MoveDialog::mouseMoveEvent(QMouseEvent *event)
{
    QDialog::mouseMoveEvent(event);
    if (!_leftButtonPress )            //不按左键
    {
        event->ignore();
        return;
    }

    const int xpos = event->globalX() - _pressAt.x();
    const int ypos = event->globalY() - _pressAt.y();
    _pressAt = event->globalPos();
    move(x() + xpos, y() + ypos);
}

void MoveDialog::mouseReleaseEvent(QMouseEvent *event)
{
    QDialog::mouseReleaseEvent(event);
    _pressAt = event->globalPos();
    _leftButtonPress = false;
}
