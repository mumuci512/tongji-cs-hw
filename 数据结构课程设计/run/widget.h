#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; } // Widget类属于ui命名空间
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT // 涉及到信号与槽的 类 都需要加上Q_OBJECT宏定义

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
