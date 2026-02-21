#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv); // 必须写的
    MainWindow w;
    w.show();
    return a.exec(); // 程序永远不会退出（死循环）
}

/*
 * 控件（一种对象）：分为 标签QLable、行编辑框QLineEdit、文本编辑框、按钮QPushButton
 * 布局（一种对象）：QHBoxLayout水平布局 QVBoxLayout垂直布局 网格布局
 * 窗口（一种对象）：QWidget
 */
