#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //generated = false;

    //以主界面为父窗口创建滚动条
    pScroll=new QScrollArea(this);

    //将包含所有UI信息的Widget放进滚动条区域
    pScroll->setWidgetResizable(true);
    pScroll->setWidget(ui->centralwidget);
    setCentralWidget(pScroll);

    ui->matrix_table->setRowCount(0);
    ui->matrix_table->setColumnCount(0);
    // 初始状态x个顶点
    int initial_vertex = 5;
    ui->vertex_num_spin->setValue(initial_vertex);
    updateTable(initial_vertex);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_MainWindow_iconSizeChanged(const QSize &iconSize)
{
    if(pScroll)
    {
        // 设置滚动条区域大小时，需要减去状态栏的高度
        // 此处53是标题栏与任务栏的高度，是centralWidget相对于主界面左上角坐标的y值
        int y_0 = ui->menubar->geometry().y() + ui->menubar->height();
        pScroll->setGeometry(0,y_0,width(),height()-ui->statusbar->height()-y_0);
        ui->centralwidget->setMinimumSize(width(),height()-ui->statusbar->height()-y_0);
    }
}

void MainWindow::adjust_table_size(QTableWidget *table, int max_w, int max_h)
{
    int w = table->horizontalHeader()->length() +
                    table->verticalHeader()->width() +
                    2 * table->frameWidth() + 20;

    int h = table->verticalHeader()->length() +
                     table->horizontalHeader()->height() +
                     2 * table->frameWidth() + 20;
    table->setFixedSize(qMin(w,max_w), qMin(h,max_h));
}

void MainWindow::updateTable(int count)
{
    int origin_count = ui->matrix_table->rowCount();
    //int b = ui->matrix_table->columnCount();
    //qDebug() << origin_count << b;

    // 改变表格大小
    ui->matrix_table->setRowCount(count);
    ui->matrix_table->setColumnCount(count);

    // 用顶点标签列表改变表头
    QStringList vertexLabels;
    for (int i = 0; i < count; ++i) {
        vertexLabels << QString("v%1").arg(i + 1);
    }

    ui->matrix_table->setHorizontalHeaderLabels(vertexLabels);
    ui->matrix_table->setVerticalHeaderLabels(vertexLabels);

    // 新增的单元格初始化为0
    for (int row = origin_count; row < count; ++row) {
        for (int col = 0; col < count; ++col) {
            // 检查单元格是否已存在，避免覆盖
            if (!ui->matrix_table->item(row, col)) {
                QTableWidgetItem* item = new QTableWidgetItem("1");
                item->setTextAlignment(Qt::AlignCenter);
                ui->matrix_table->setItem(row, col, item);
            }
        }
    }
    for (int col = origin_count; col < count; ++col) {
        for (int row = 0; row < count; ++row) {
            // 检查单元格是否已存在，避免覆盖
            if (!ui->matrix_table->item(row, col)) {
                QTableWidgetItem* item = new QTableWidgetItem("0");
                item->setTextAlignment(Qt::AlignCenter);
                ui->matrix_table->setItem(row, col, item);
            }
        }
    }

    // 改变窗口大小
    adjust_table_size(ui->matrix_table, 2068, 669);
}

void MainWindow::on_vertex_num_spin_valueChanged(int count)
{
    //generated = false;
    updateTable(count);
}

void MainWindow::generate_matrix_and_list()
{
    int size = ui->matrix_table->rowCount();
    adj_matrix.resize(size);
    for (int i = 0; i < size; ++i) {
        adj_matrix[i].resize(size);
    }
    adj_list.clear();
    adj_list.resize(size);

    // 读取邻接矩阵
    for(int i=0; i<size; ++i) {
        for(int j=0; j<size; ++j) {
            adj_matrix[i][j] = ui->matrix_table->item(i,j)->text().toInt();
        }
    }

    // 转换为邻接表
    for(int i=0; i<size; ++i) {
        for(int j=0; j<size; ++j) {
            if(adj_matrix[i][j] != 0) {
                adj_list[i].push_back(j);
            }
        }
    }

    //generated = true;
}
void MainWindow::on_generate_list_button_clicked()
{
    //if(!generated)
        generate_matrix_and_list();

    ui->list_display->clear();
    for(int i = 0; i < adj_list.size(); ++i) {
        QString line = QString::number(i) + ": ";
        for(int neighbor : adj_list[i]) {
            line += QString::number(neighbor) + " -> ";
        }

        // 移除末尾多余的 " -> "
        if(line.endsWith(" -> ")) {
            line.chop(4);
        }

        ui->list_display->append(line);
    }
}

void MainWindow::delay_ms(int msecs)
{
    if (msecs <= 0)
        return;

    QEventLoop loop;
    // 定时结束后退出事件循环
    QTimer::singleShot(msecs, &loop, &QEventLoop::quit);
    // 进入事件循环，等待定时结束
    loop.exec();
}

void MainWindow::set_ui_enabled(bool enabled)
{
    ui->DFS_non_recur_button->setEnabled(enabled);
    ui->DFS_recur_button->setEnabled(enabled);
    ui->BFS_button->setEnabled(enabled);
    ui->generate_list_button->setEnabled(enabled);
    ui->matrix_table->setEnabled(enabled);

    ui->vertex_num_spin->setEnabled(enabled);
    ui->start_vertex_edit->setEnabled(enabled);

    // ui->reset_button->setEnabled(true);
    // ui->help_button->setEnabled(true);

//    if (ui->cancel_button) {
//        ui->cancel_button->setEnabled(!enabled);
//    }
}

void MainWindow::push_to_stack(int value)
{
    QString log;
    ui->stack_display->insertItem(0, QString::number(value));

    // 短暂高亮后恢复颜色
    if (ui->stack_display->count() > 0) {
        ui->stack_display->item(0)->setBackground(Qt::lightGray);
        delay_ms(1600);
        if (ui->stack_display->count() > 0) {
            ui->stack_display->item(0)->setBackground(Qt::white);
        }
    }

    log = "入栈: " + QString::number(value) + "\n";
    ui->log_display->append(log);
    delay_ms(1600);
}

int MainWindow::pop_from_stack()
{
    QString log;

    if (ui->stack_display->count() == 0) {
        qWarning() << "栈为空，无法执行出栈操作";
        return -1;
    }

    QListWidgetItem* topItem = ui->stack_display->item(0);
    int value = topItem->text().toInt();

    // 高亮显示即将出栈的项目
    topItem->setBackground(Qt::yellow);

    // 短暂延迟后移除项目来模拟出栈动画
    delay_ms(1600);
    int row = ui->stack_display->row(topItem);
    if (row >= 0) {
        delete ui->stack_display->takeItem(row);
    }

    log = "出栈: " + QString::number(value) + "\n";
    ui->log_display->append(log);
    delay_ms(1600);

    return value;
}

void MainWindow::DFS(bool recursive)
{
    // 禁用按钮，防止重复点击
    set_ui_enabled(false);

    //if(!generated)
        generate_matrix_and_list();
    if(adj_list.empty()) {
        set_ui_enabled(true);
        return;
    }
    int start = ui->start_vertex_edit->value();
    int size = adj_list.size();
    if(start < 0 || start >= size) {
        set_ui_enabled(true);
        return;
    }
    visited = QVector<bool>(adj_list.size(), false);

    ui->stack_display->clear();
    ui->log_display->clear();
    QStack<int> stack;
    QString path;

    // 使用定时器逐步执行DFS，而不是一次性执行
    QTimer* dfsTimer = new QTimer(this);
    QEventLoop loop;

    // 连接定时器信号
    connect(dfsTimer, &QTimer::timeout, this, [&]() {
        // 以下是原循环代码
        if(stack.isEmpty()) {
            dfsTimer->stop();
            ui->log_display->append("遍历序列: [" + path + "]");
            loop.quit();
            dfsTimer->deleteLater();
            set_ui_enabled(true);
            return;
        }

        int v = stack.pop();
        pop_from_stack();
        path += QString::number(v) + " ";

        // 逆序入栈时，与递归顺序一致
        if(recursive)
            for(auto it = adj_list[v].rbegin(); it != adj_list[v].rend(); ++it) {
                if(!visited[*it]) {
                    visited[*it] = true;
                    stack.push(*it);
                    push_to_stack(*it);
                }
            }
        else// 正序入栈
            for(auto it = adj_list[v].begin(); it != adj_list[v].end(); ++it) {
                if(!visited[*it]) {
                    visited[*it] = true;
                    stack.push(*it);
                    push_to_stack(*it);
                }
            }
    });

    stack.push(start);
    visited[start] = true;
    push_to_stack(start);

    dfsTimer->start(3500);
    loop.exec();

}
void MainWindow::on_DFS_recur_button_clicked()
{
    // 由于需要循环来处理UI更新，所以无法真的递归调用函数，只能用逆序循环模拟递归遍历时栈的变化
    DFS(true);
}

void MainWindow::on_DFS_non_recur_button_clicked()
{
    DFS(false);
}

void MainWindow::enqueue(int value)
{
    QString log;

    // 在队列底部（队尾）添加新项
    ui->queue_display->addItem(QString::number(value));

    // 获取最后添加的项（队尾）
    int last_index = ui->queue_display->count() - 1;
    QListWidgetItem* last_item = ui->queue_display->item(last_index);

    // 短暂高亮新入队的项目
    if (last_item) {
        last_item->setBackground(Qt::lightGray);
        delay_ms(1600);
        last_item->setBackground(Qt::white);
    }

    log = "入队: " + QString::number(value) + "\n";
    ui->log_display->append(log);
    delay_ms(1600);
}

int MainWindow::dequeue()
{
    QString log;

    if(ui->queue_display->count() == 0) {
        //qWarning() << "队列为空，无法执行出队操作";
        return -1;
    }

    // 获取队首项
    QListWidgetItem* frontItem = ui->queue_display->item(0);
    int value = frontItem->text().toInt();

    // 高亮显示即将出队的项
    frontItem->setBackground(Qt::yellow);
    delay_ms(1600);

    // 移除队首项
    delete ui->queue_display->takeItem(0);

    log = "出队: " + QString::number(value) + "\n";
    ui->log_display->append(log);
    delay_ms(1600);

    return value;
}
void MainWindow::on_BFS_button_clicked()
{
    // 禁用按钮，防止重复点击
    set_ui_enabled(false);

    //if(!generated)
        generate_matrix_and_list();
    if(adj_list.empty()) {
        set_ui_enabled(true);
        return;
    }
    int start = ui->start_vertex_edit->value();
    int size = adj_list.size();
    if(start < 0 || start >= size) {
        set_ui_enabled(true);
        return;
    }
    visited = QVector<bool>(adj_list.size(), false);

    ui->queue_display->clear();
    ui->log_display->clear();
    //QQueue<int> queue;
    QString path;

    QTimer* bfsTimer = new QTimer(this);
    QEventLoop loop;

    connect(bfsTimer, &QTimer::timeout, this, [&]() {
        if(ui->queue_display->count() == 0) {
            bfsTimer->stop();
            ui->log_display->append("遍历序列: [" + path + "]");
            loop.quit();
            bfsTimer->deleteLater();
            set_ui_enabled(true);
            return;
        }

        int v = dequeue();
        path += QString::number(v) + " ";

        // 将未访问的邻居加入队列
        for(int neighbor : adj_list[v]) {
            if(!visited[neighbor]) {
                visited[neighbor] = true;
                enqueue(neighbor);
            }
        }
    });

    visited[start] = true;
    enqueue(start);

    bfsTimer->start(3500);
    loop.exec();
}
