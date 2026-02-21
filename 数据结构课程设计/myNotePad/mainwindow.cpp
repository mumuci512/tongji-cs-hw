#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "form.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionNew,&QAction::triggered,this,&MainWindow::ActionNewSlot);
    connect(ui->actionOpen,&QAction::triggered,this,&MainWindow::ActionOpenSlot);
    connect(ui->actionSave,&QAction::triggered,this,&MainWindow::ActionSaveSlot);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ActionNewSlot()
{
    ui->textEdit->clear();
    this->setWindowTitle("新建文本文档.txt");
}

void MainWindow::ActionOpenSlot()
{
    QString fileName = QFileDialog::getOpenFileName(this,
          tr("Open Image"), QCoreApplication::applicationFilePath(), tr("*.cpp"));
    if (fileName.isEmpty())
    {
        //QMessageBox::warning(this,"警告","请选择一个文件");
    }
    else
    {
        //qDebug() << fileName;
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);
        QByteArray ba = file.readAll();
        ui->textEdit->setText(QString(ba));
        file.close();
    }
}

void MainWindow::ActionSaveSlot()
{
    QString fileName = QFileDialog::getOpenFileName(this,
          tr("Open Image"), QCoreApplication::applicationFilePath(), tr("*.cpp"));
    if (fileName.isEmpty())
    {
        //QMessageBox::warning(this,"警告","请选择一个文件");
    }
    else
    {
        //qDebug() << fileName;
        QFile file(fileName);
        file.open(QIODevice::WriteOnly);

        QByteArray ba;
        ba.append(ui->textEdit->toPlainText());
        file.write(ba);
        file.close();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *k)
{
    if(k->modifiers() == Qt::ControlModifier && k->key() == Qt::Key_S)  // crtl+s被按下
    {
//        this->hide();   //隐藏当前widget，也可以直接关闭（会导致当前类的临时对象等都释放）

//        // 堆空间创建新对象（用指针）。因为若直接创建，那创建的是临时变量，本函数结束即释放
//        Form* f = new Form; // 可以从旧窗口传参数进入新窗口，eg:Form* f = new Form(socket);socket是传入的参数，需要对应改变Form类的构造函数
//        f->show();

        ActionSaveSlot();
    }
}

void MainWindow::mousePressEvent(QMouseEvent *m)
{
    QPoint pt = m->pos();   //获取光标位置
    qDebug << pt;

    if(m->button() == Qt::LeftButton)
    {
        qDebug << "左键被按下";
    }
}
