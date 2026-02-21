#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QVector>
#include <QScrollArea>
#include <QTableWidget>
#include <QStack>
#include <QQueue>
#include <QEventLoop>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void updateTable(int count);
    ~MainWindow();

private slots:
    void on_vertex_num_spin_valueChanged(int arg1);

    void on_generate_list_button_clicked();

    void on_MainWindow_iconSizeChanged(const QSize &iconSize);

    void on_DFS_recur_button_clicked();

    void on_DFS_non_recur_button_clicked();

    void on_BFS_button_clicked();

private:
    Ui::MainWindow *ui;
    QVector<QVector<int>> adj_matrix;
    QVector<QVector<int>> adj_list;
    QScrollArea *pScroll;
    QVector<bool> visited;
    //bool generated; // 标记是否已经生成matrix和list，没有在matrix_table的内容更新的时候改为false所以没有启用

    void adjust_table_size(QTableWidget* table, int max_w, int max_h);
    void generate_matrix_and_list();
    void push_to_stack(int value);
    int pop_from_stack();
    void delay_ms(int msecs);
    void set_ui_enabled(bool enabled);
    void DFS(bool recursive);
    void enqueue(int value);
    int dequeue();
};
#endif // MAINWINDOW_H
