#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setWindowTitle("calculator");

    this->setMaximumSize(400,580);
    this->setMinimumSize(400,580);

    QFont font("Times New Roman",10);
    ui->mainLineEdit->setFont(font);

    QIcon delIcon("D:\\hw_data-structure-design\\projects\\calculator\\del.jpg");
    ui->delButton->setIcon(delIcon);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_oneButton_clicked()
{
    expression += "1";
    ui->mainLineEdit->setText(expression);
}
