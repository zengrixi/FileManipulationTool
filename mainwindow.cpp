#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _type(File)
    , _currPath(QString())
    , _winflags(windowFlags())
{
    ui->setupUi(this);

    initStyleSheet();

    connect(ui->radioButton_file, &QRadioButton::toggled
        , this, &MainWindow::on_file_or_folder);
    connect(ui->radioButton_folder, &QRadioButton::toggled
        , this, &MainWindow::on_file_or_folder);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    // 窗口置顶
    switch ( arg1 )
    {
    case Qt::Unchecked:
        setWindowFlags(_winflags);
        show();
        break;
    case Qt::PartiallyChecked:
    case Qt::Checked:
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        show();
        break;
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    // 手动读取
    _currPath =
    QFileDialog::getExistingDirectory(
        this, "选择一个目录", "/home", QFileDialog::ShowDirsOnly);

    if ( _currPath.isEmpty() )
    {
        QMessageBox::critical(
            this, "提示", "什么都没有选择", QMessageBox::Ok);
    }
    else
    {
        ui->lineEdit->setText(_currPath);
        // 显示当前目录的文件/文件夹
        folderTraversal(_currPath);
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    // 打赏作者
}

void MainWindow::on_file_or_folder()
{
    // 选中文件或文件夹
    if ( ui->radioButton_folder->isChecked() )
    {
        _type = Folder;
    }
    else if ( ui->radioButton_file->isChecked() )
    {
        _type = File;
    }

    // 刷新列表显示
    folderTraversal(_currPath);
}

void MainWindow::initStyleSheet()
{
    // 初始化样式表
    ui->pushButton_4->setStyleSheet("background-color: rgba(0, 0, 0, 0)");// 实现一直透明
}

void MainWindow::folderTraversal(const QString &path)
{
    ui->listWidget->clear();

    QDir dir(path);
    if ( !dir.exists() )
    {
        return;
    }

    // 取到所有的文件名, 但是去掉.和..的文件夹
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

    // 文件夹优先
    dir.setSorting(QDir::DirsFirst);

    //转化成一个list
    QFileInfoList list = dir.entryInfoList();
    if ( list.size() < 1 )
    {
        return;
    }
    
    int i = 0;
    do
    {
        QFileInfo fileInfo = list.at(i);
        bool bisDir = fileInfo.isDir();
        if ( bisDir && _type == Folder )
        {
            ui->listWidget->addItem(fileInfo.fileName());
        }
        else if ( !bisDir && _type == File )
        {
            QString filename = fileInfo.fileName();
            QString suffix = ui->comboBox->currentText();
            QString ignore =  ui->comboBox_2->currentText();
            if ( suffix.contains("所有") )
            {
                // 忽略扩展名
                if ( ignore.contains("无") || !filename.contains(ignore) )
                {
                    ui->listWidget->addItem(filename);
                }
            }
            else
            {
                if ( filename.contains(suffix) )
                {
                    ui->listWidget->addItem(filename);
                }
            }
        }

        i++;
    } while ( i < list.size() );
}

void MainWindow::on_pushButton_3_clicked()
{
    // 创建文件
}

void MainWindow::on_comboBox_editTextChanged(const QString &arg1)
{
    // 检索扩展名
    folderTraversal(_currPath);
}

void MainWindow::on_comboBox_2_editTextChanged(const QString &arg1)
{
    // 忽略扩展名
    folderTraversal(_currPath);
}
