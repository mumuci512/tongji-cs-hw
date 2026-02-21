#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)  // 通过初始化列表来初始化指针，相当于ui = new Ui::widget
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

