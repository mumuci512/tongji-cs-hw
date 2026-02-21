#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

//#define TIMEOUT     1000

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    virtual void timerEvent(QTimerEvent *event); // 从虚基类继承该函数并实现
    ~Widget();

private slots:
    void on_startButton_clicked();

    void on_endButton_clicked();

private:
    Ui::Widget *ui;
    int myTimerId;
    int pixId;
};
#endif // WIDGET_H
