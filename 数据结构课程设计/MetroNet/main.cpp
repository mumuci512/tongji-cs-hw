#include "mainwindow.h"

#include <QApplication>
#include <QFont>
#include <QTextCodec>   // 假如Qt是版本6则不需要

QSqlDatabase db;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setFont(QFont("Microsoft Yahei", 9));

    // 设置文字编码防止乱码，假如Qt是版本6则不需要
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

    MainWindow w;
    w.show();
    return a.exec();
}
