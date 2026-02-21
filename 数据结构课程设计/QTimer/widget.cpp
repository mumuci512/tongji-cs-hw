#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    timer = new QTimer;

    pixId = 2;

    QImage img;
    img.load("D:\\hw_data-structure-design\\projects\\QTimer\\1.png");
    ui->label->setPixmap(QPixmap::fromImage(img));

    // 定时器时间到，发出timeout信号
    connect(timer, &QTimer::timeout,this, &Widget::timeoutSlot);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_startButton_clicked()
{
    timer->start(1000);
}

void Widget::timeoutSlot()
{
    QString path("D:\\hw_data-structure-design\\projects\\QTimer\\");
    path += QString::number(pixId);
    path += ".png";

    QImage img;
    img.load(path);
    ui->label->setPixmap(QPixmap::fromImage(img));

    pixId ++;

    if(pixId == 6)
        pixId = 1;
}

void Widget::on_endButton_clicked()
{
    timer->stop();
}

void Widget::on_singleButton_clicked()
{
    QTimer::singleShot(1000,this,SLOT(timeoutSlot()));
}
