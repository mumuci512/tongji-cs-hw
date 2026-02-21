#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    QPixmap pix("D:\\hw_data-structure-design\\projects\\objectTimer\\1.png");
    ui->label->setPixmap(pix);

    pixId = 2;
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_startButton_clicked()
{
    // 开启定时器，返回定时器编号
    myTimerId = this->startTimer(1000);     // Widget继承于QWidget，QWidget继承于QObject，startTimer是QObject的函数，所以用this
}

void Widget::timerEvent(QTimerEvent *event)
{
    if (event->timerId() != myTimerId)      // 只有当前事件的timerId是on_startButton_clicked的timerId时才执行
        return;

    QString path("D:\\hw_data-structure-design\\projects\\objectTimer\\");
    path += QString::number(pixId);
    path += ".png";

    QPixmap pix(path);
    ui->label->setPixmap(pix);

    pixId ++;

    if(pixId == 6)
        pixId = 1;

}

void Widget::on_endButton_clicked()
{
    this->killTimer(myTimerId);
}
