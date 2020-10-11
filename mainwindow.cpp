#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _type(Type::File)
    , _currPath(QString())
    , _winflags(windowFlags())
    , _fileOperations(new FileOperations(this))
{
    ui->setupUi(this);

    initStyleSheet();

    connect(ui->radioButton_file, &QRadioButton::toggled
        , this, &MainWindow::selFileOrFolder);
    connect(ui->radioButton_folder, &QRadioButton::toggled
        , this, &MainWindow::selFileOrFolder);

    // 初始化为繁体转简体
    _simplifiedAndTradFunctions = Chardet::CharConversion::convertTradCharactersToSimplifiedCharacters;
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
        showFilesUnderPath(_currPath);
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    // 打赏作者
}

// 选中文件或者文件夹
void MainWindow::selFileOrFolder()
{
    if ( ui->radioButton_folder->isChecked() )
    {
        _type = Type::Folder;
    }
    else if ( ui->radioButton_file->isChecked() )
    {
        _type = Type::File;
    }

    // 刷新列表显示
    showFilesUnderPath(_currPath);
}

void MainWindow::initStyleSheet()
{
    // 初始化样式表
    ui->pushButton_4->setStyleSheet("background-color: rgba(0, 0, 0, 0)");// 实现一直透明
}

// 显示路径下的文件
void MainWindow::showFilesUnderPath(const QString &path)
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
        if ( bisDir && _type == Type::Folder )
        {
            ui->listWidget->addItem(fileInfo.fileName());
        }
        else if ( !bisDir && _type == Type::File )
        {
            QString filename = fileInfo.fileName();
            QString suffix = ui->comboBox->currentText();
            QString ignore = ui->comboBox_2->currentText();
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
                // 只显示扩展名
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
    QString filePath = ui->lineEdit->text();
    QString fileNm = ui->lineEdit_2->text();
    QString sfx = ui->lineEdit_3->text();
    if ( !sfx.isEmpty() )
    {
        fileNm + "." + ui->lineEdit_3->text();
    }
    _fileOperations->createAFile(filePath, fileNm);
}

void MainWindow::on_comboBox_editTextChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    // 检索扩展名
    showFilesUnderPath(_currPath);
}

void MainWindow::on_comboBox_2_editTextChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    // 忽略扩展名
    showFilesUnderPath(_currPath);
}

void MainWindow::on_pushButton_6_clicked()
{
    // 简繁转换
    if (ui->radioButton_5->isChecked())
    {
        // 繁体转简体
        _simplifiedAndTradFunctions = Chardet::CharConversion::convertTradCharactersToSimplifiedCharacters;
    }
    else if (ui->radioButton_6->isChecked())
    {
        // 简体转繁体
        _simplifiedAndTradFunctions = Chardet::CharConversion::convertSimplifiedCharactersToTradCharacters;
    }

    auto filePath = ui->lineEdit->text();
    auto i = 0;
    while (i < ui->listWidget->count())
    {
        auto fileNm = ui->listWidget->item(i)->text();
        auto nmAftChg = _simplifiedAndTradFunctions(fileNm);
        _fileOperations->fileRename(filePath, fileNm, nmAftChg);
        i++;
    }
    showFilesUnderPath(_currPath);
}
