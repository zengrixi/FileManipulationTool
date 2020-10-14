#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

// 正则表达式定义
const static QString g_regularExpressionToExtractFileNm("^.*(?=\\.)");
const static QString g_regularExpressionToExtractSfxNm("\\.[^\\.]\\w*$");

const static auto extractFileNm = [] (const QString &fileNm)->QString {
    auto regular = QRegExp(g_regularExpressionToExtractFileNm);
    regular.indexIn(fileNm);
    return regular.cap(0);
};

const static auto extractTheSfx = [] (const QString &fileNm)->QString {
    auto regular2 = QRegExp(g_regularExpressionToExtractSfxNm);
    regular2.indexIn(fileNm);
    return regular2.cap(0);
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _type(Type::File)
    , _currPath(QString())
    , _winflags(windowFlags())
    , _fileOperations(new FileOperations(this))
{
    ui->setupUi(this);

    initializeTheForm();
    initializeTheStyleShet();

    connect(ui->radioButton_file, &QRadioButton::toggled
        , this, &MainWindow::selFileOrFolder);
    connect(ui->radioButton_folder, &QRadioButton::toggled
        , this, &MainWindow::selFileOrFolder);

    // 初始化为繁体转简体
    _simplifiedAndTradFunctions = Chardet::CharConversion::convertTradCharactersToSimplifiedCharacters;

    // 绑定添加按钮事件
    connect(ui->pushButton_8, &QPushButton::clicked, this, &MainWindow::addCharactersToTheFileNm);
    connect(ui->pushButton_9, &QPushButton::clicked, this, &MainWindow::addCharactersToTheFileNm);
    connect(ui->pushButton_delAllPre, &QPushButton::clicked, this, &MainWindow::delBtnTriggered);
    connect(ui->pushButton_delAllFollowing, &QPushButton::clicked, this, &MainWindow::delBtnTriggered);

    // 列表更新事件
    connect(ui->comboBox, &QComboBox::editTextChanged, this, &MainWindow::showFilesUnderPath);
    connect(ui->comboBox_2, &QComboBox::editTextChanged, this, &MainWindow::showFilesUnderPath);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    // 窗口置顶
    switch (arg1)
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

    if (_currPath.isEmpty())
    {
        QMessageBox::critical(
            this, "提示", "什么都没有选择", QMessageBox::Ok);
    }
    else
    {
        ui->currPath->setText(_currPath);
        // 显示当前目录的文件/文件夹
        showFilesUnderPath();
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    // 打赏作者
    QEventLoop eventLoop;
    PictureBannerWidget rewardQrCdDispForm;
    //打赏窗体初始化
    rewardQrCdDispForm.addPage(QPixmap(":/image/alipayPaymentCd"));
    rewardQrCdDispForm.addPage(QPixmap(":/image/wechatPaymentCd"));
    rewardQrCdDispForm.addPage(QPixmap(":/image/qqCollectionCd"));
    rewardQrCdDispForm.setWindowTitle("感谢打赏٩(๑❛ᴗ❛๑)۶");
    rewardQrCdDispForm.setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    rewardQrCdDispForm.setAttribute(Qt::WA_ShowModal);
    rewardQrCdDispForm.show();
    connect(&rewardQrCdDispForm, &PictureBannerWidget::close, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();
}

// 选中文件或者文件夹
void MainWindow::selFileOrFolder()
{
    if (ui->radioButton_folder->isChecked())
    {
        _type = Type::Folder;
    }
    else if (ui->radioButton_file->isChecked())
    {
        _type = Type::File;
    }

    // 刷新列表显示
    showFilesUnderPath();
}

void MainWindow::initializeTheStyleShet()           //初始化样式表
{
    QFile f(":qdarkstyle/style.qss");

    if (!f.exists())
    {
        printf("Unable to set stylesheet, file not found\n");
    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }

//    ui->pushButton_4->setStyleSheet("background-color: rgba(0, 0, 0, 0)");// 实现一直透明
}

// 显示路径下的文件
void MainWindow::showFilesUnderPath()
{
    ui->listWidget->clear();

    QDir dir(_currPath);
    if (!dir.exists())
    {
        return;
    }

    // 取到所有的文件名, 但是去掉.和..的文件夹
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

    // 文件夹优先
    dir.setSorting(QDir::DirsFirst);

    //转化成一个list
    QFileInfoList list = dir.entryInfoList();
    if (list.size() < 1)
    {
        return;
    }
    
    int i = 0;
    do
    {
        QFileInfo fileInfo = list.at(i);
        bool bisDir = fileInfo.isDir();
        if (bisDir && _type == Type::Folder)
        {
            ui->listWidget->addItem(fileInfo.fileName());
        }
        else if (!bisDir && _type == Type::File)
        {
            QString filename = fileInfo.fileName();
            QString suffix = ui->comboBox->currentText();
            QString ignore = ui->comboBox_2->currentText();
            if (suffix.contains("所有"))
            {
                // 忽略扩展名
                if (ignore.contains("无") || !filename.contains(ignore))
                {
                    ui->listWidget->addItem(filename);
                }
            }
            else
            {
                // 只显示扩展名
                if (filename.contains(suffix))
                {
                    ui->listWidget->addItem(filename);
                }
            }
        }

        i++;
    } while (i < list.size());
}

void MainWindow::on_pushButton_3_clicked()
{
    // 创建文件
    QString filePath = _currPath;
    QString fileNm = ui->lineEdit_2->text();
    QString sfx = ui->lineEdit_3->text();
    if (!sfx.isEmpty())
    {
        fileNm + "." + ui->lineEdit_3->text();
    }
    _fileOperations->createAFile(filePath, fileNm);
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

    traverseTheFileLstAndProcess([=] (const QString &fileNm)->QString {
        return _simplifiedAndTradFunctions(fileNm);
    });
}

void MainWindow::on_pushButton_7_clicked()
{
    // 给文件名加序列号
    auto count = ui->listWidget->count();
    auto brkChar = ui->brkChar->text();
    traverseTheFileLstAndProcess([&] (const QString &fileNm)->QString {
        auto serlNum = QString::number(count--);
        return serlNum + brkChar + fileNm;
    });
}

// 将字符添加到文件
void MainWindow::addCharactersToTheFileNm()
{
    auto btn = qobject_cast<QPushButton *>(sender());
    auto charactersToBeAdded = ui->charactersToAdd->text();
    traverseTheFileLstAndProcess([&] (const QString &fileNm)->QString {
        auto nmAftChg = QString();
        // 头部添加或者尾部添加
        if (btn == ui->pushButton_8)
            nmAftChg = charactersToBeAdded + fileNm;
        else if (btn == ui->pushButton_9)
        {
            // 判断是否有后缀, 如果有后缀则在后缀前添加
            auto frstNm = extractFileNm(fileNm);
            auto suffix = extractTheSfx(fileNm);
            nmAftChg = frstNm + charactersToBeAdded + suffix;
        }

        return nmAftChg;
    });
}

void MainWindow::initializeTheForm()            //初始化表格
{

}

void MainWindow::initializeTheCtl()         //初始化控件
{

}

template<typename T>
void MainWindow::traverseTheFileLstAndProcess(const T &func)
{
    auto nmAftChg = QString();
    auto i = 0;
    while (i < ui->listWidget->count())
    {
        auto fileNm = ui->listWidget->item(i)->text();
        // 头部添加或者尾部添加
        nmAftChg = func(fileNm);
        _fileOperations->fileRename(_currPath, fileNm, nmAftChg);
        i++;
    }
    showFilesUnderPath();
}

void MainWindow::on_replaceAll_clicked()
{
    // 全部替换
    auto befStr = ui->lineEdit_bef->text();
    auto postStr = ui->lineEdit_rear->text();
    auto freq = ui->lineEdit_freq->text().toInt();
    auto left = ui->radioButton_left->isChecked();
    auto right = ui->radioButton_right->isChecked();
    auto excludeNumbers = ui->radioButton_aftTheNum->isChecked();

    if (befStr == postStr) return;

    traverseTheFileLstAndProcess([&] (const QString &fileNm)->QString {
        // 取文件名和后缀名
        auto frstNm = extractFileNm(fileNm);
        auto suffix = extractTheSfx(fileNm);
        auto serlNum = QString();

        // 是否排除开头的数字
        if (excludeNumbers)
        {
            auto regular2 = QRegExp("^[0-9]*");
            regular2.indexIn(fileNm);
            serlNum = regular2.cap(0);
            frstNm.remove(0, serlNum.size());
        }

        // 替换字符串
        if (left)
        {
            int i = 0, j = 0;
            while ((j = frstNm.indexOf(befStr, j)) != -1)
            {
                if (++i > freq && freq > 0) break;
                frstNm.replace(j, befStr.size(), postStr);
            }
        }
        else if (right)
        {
            int i = 0, j = 0;
            auto index = -1;
            while ((j = frstNm.lastIndexOf(befStr, index)) != -1)
            {
                if (++i > freq && freq > 0) break;
                frstNm.replace(j, befStr.size(), postStr);
                index = j - frstNm.size();
            }
        }

        return serlNum + frstNm + suffix;
    });
}

void MainWindow::delBtnTriggered()
{
    //删除按钮触发
    auto btn = qobject_cast<QPushButton *>(sender());
    auto delAllPre = (btn == ui->pushButton_delAllPre);
    auto delAllFollowing = (btn == ui->pushButton_delAllFollowing);
    auto inclItself = ui->checkBox_2->isChecked();
    auto deletedContent = ui->lineEdit->text();
    auto frstAppearance = ui->radioButton_frstAppearance->isChecked();
    auto lastAppeared = ui->radioButton_lastAppeared->isChecked();
    traverseTheFileLstAndProcess([&] (const QString &fileNm)->QString {
        // 取文件名和后缀名
        auto frstNm = extractFileNm(fileNm);
        auto suffix = extractTheSfx(fileNm);
        auto leftShftLen = inclItself ? deletedContent.size() : 0;
        auto rightShftLen = inclItself ? 0 : deletedContent.size();
        auto index = frstAppearance ? frstNm.indexOf(deletedContent) :
                     (lastAppeared ? frstNm.lastIndexOf(deletedContent) : -1);           //首次出现还是最后出现
        if (index != -1)
        {
            auto moveSubscriptLeft = index + leftShftLen;
            auto moveSubscriptRight = index + rightShftLen;
            delAllPre ? frstNm.remove(0, moveSubscriptLeft) :
            delAllFollowing ? frstNm.remove(moveSubscriptRight, frstNm.count() - moveSubscriptRight) : QString();         //删除前面还是删除后面
        }
        return frstNm + suffix;
    });
}
