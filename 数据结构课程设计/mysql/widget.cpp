#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 连接数据库
    db = QSqlDatabase::addDatabase(QMYSQL);
}

Widget::~Widget()
{
    delete ui;
}

