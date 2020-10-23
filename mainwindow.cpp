#include "mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QtConcurrent>

#include "app.h"
#include "strmanipulation.h"
#include "ui_mainwindow.h"

#if defined(Q_OS_WIN32)
#    include "windows.h"
#endif

// 正则表达式定义
const static QString g_regularExpressionToExtractFileNm("^.*(?=\\.)");
const static QString g_regularExpressionToExtractSfxNm("\\.[^\\.]\\w*$");

const static auto extractFileNm = [](const QString &fileNm) -> QString {
    auto regular = QRegExp(g_regularExpressionToExtractFileNm);
    regular.indexIn(fileNm);
    return regular.cap(0);
};

const static auto extractTheSfx = [](const QString &fileNm) -> QString {
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
    , _fileOperations(new FileOperations(this)) {
    ui->setupUi(this);

    initializeTheForm();
    initializeTheStyleShet();

    connect(ui->radioButton_file, &QRadioButton::toggled, this, &MainWindow::selFileOrFolder);
    connect(ui->radioButton_folder, &QRadioButton::toggled, this, &MainWindow::selFileOrFolder);

    // 初始化为繁体转简体
    _simplifiedAndTradFunctions = Chardet::CharConversion::convertTradCharactersToSimplifiedCharacters;

    // 绑定添加按钮事件
    connect(ui->pushButton_8, &QPushButton::clicked, this, &MainWindow::addCharactersToTheFileNm);
    connect(ui->pushButton_9, &QPushButton::clicked, this, &MainWindow::addCharactersToTheFileNm);
    connect(ui->pushButton_delAllPre, &QPushButton::clicked, this, &MainWindow::delBtnTriggered);
    connect(ui->pushButton_delAllFollowing, &QPushButton::clicked, this, &MainWindow::delBtnTriggered);

    // 列表更新事件
    connect(ui->listWidget, &FileListWidget::updLst, this, &MainWindow::showFilesUnderPath);
    connect(ui->comboBox, &QComboBox::editTextChanged, this, &MainWindow::showFilesUnderPath);
    connect(ui->comboBox_2, &QComboBox::editTextChanged, this, &MainWindow::showFilesUnderPath);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_checkBox_stateChanged(int arg1) {
#if defined(Q_OS_WIN32)
    RECT rect;
    auto hwnd = ::GetForegroundWindow();
    GetWindowRect(hwnd, &rect);
#endif
    switch (arg1) {
    case Qt::Unchecked: //取消窗口置顶
#if defined(Q_OS_WIN)
        SetWindowPos(hwnd,
                     HWND_NOTOPMOST,
                     rect.left,
                     rect.top,
                     abs(rect.right - rect.left),
                     abs(rect.bottom - rect.top),
                     SWP_SHOWWINDOW);
#else
        setWindowFlags(_winflags);
        show();
#endif
        break;
    case Qt::PartiallyChecked:
    case Qt::Checked: //窗口置顶
#if defined(Q_OS_WIN)
        SetWindowPos(hwnd,
                     HWND_TOPMOST,
                     rect.left,
                     rect.top,
                     abs(rect.right - rect.left),
                     abs(rect.bottom - rect.top),
                     SWP_SHOWWINDOW);
#else
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        show();
#endif
        break;
    }
}

void MainWindow::on_pushButton_2_clicked() {
    // 手动读取
    _currPath = QFileDialog::getExistingDirectory(this, "选择一个目录", "/home", QFileDialog::ShowDirsOnly);

    if (_currPath.isEmpty()) {
        QMessageBox::critical(this, "提示", "什么都没有选择", QMessageBox::Ok);
    } else {
        ui->currPath->setText(_currPath);
        // 显示当前目录的文件/文件夹
        showFilesUnderPath();
    }
}

void MainWindow::on_pushButton_4_clicked() {
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
void MainWindow::selFileOrFolder() {
    if (ui->radioButton_folder->isChecked()) {
        _type = Type::Folder;
    } else if (ui->radioButton_file->isChecked()) {
        _type = Type::File;
    }

    // 刷新列表显示
    showFilesUnderPath();
}

void MainWindow::initializeTheStyleShet() //初始化样式表
{
    QFile f(":qdarkstyle/style.qss");

    if (!f.exists()) {
        qDebug("无法设置样式表，找不到文件");
    } else {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }

    //    ui->pushButton_4->setStyleSheet("background-color: rgba(0, 0, 0, 0)");// 实现一直透明
}

// 显示路径下的文件
void MainWindow::showFilesUnderPath() {
    ui->listWidget->clear();

    QEventLoop eventLoop;
    QtConcurrent::run([&] {
        recursivelyTraverseFiles(_currPath);
        eventLoop.exit();
    });
    eventLoop.exec();
}

void MainWindow::recursivelyTraverseFiles(const QString &path) //递归遍历文件
{
    QDir dir(path);
    if (!dir.exists()) {
        qDebug() << "未找到路径";
        return;
    }
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot); //设置过滤器
    dir.setSorting(QDir::DirsFirst); //设置排序

    auto recursive = ui->checkBox_readSubdirectory->isChecked(); //是否递归
    for (auto fileInfo : dir.entryInfoList()) {
        auto fileNm = fileInfo.fileName();
        auto bisDir = fileInfo.isDir();
        auto filePath = fileInfo.filePath();
        if (bisDir) {
            if (_type == Type::Folder) {
                auto item = new QListWidgetItem(fileNm);
                item->setToolTip(filePath);
                ui->listWidget->addItem(item);
            }
            if (recursive) recursivelyTraverseFiles(filePath); //递归遍历文件
        } else {
            if (_type != Type::File) continue;

            auto suffix = ui->comboBox->currentText();
            auto ignore = ui->comboBox_2->currentText();
            auto isSuffix = extractTheSfx(fileNm);
            if (suffix.contains("所有")) {
                // 忽略扩展名
                if (ignore.contains("无") || isSuffix != ignore) {
                    auto item = new QListWidgetItem(fileNm);
                    item->setToolTip(filePath);
                    ui->listWidget->addItem(item);
                }
            } else {
                // 只显示某种类型的文件
                if (isSuffix == suffix) {
                    auto item = new QListWidgetItem(fileNm);
                    item->setToolTip(filePath);
                    ui->listWidget->addItem(item);
                }
            }
        }
    };
}

void MainWindow::on_pushButton_3_clicked() {
    // 创建文件
    QString filePath = _currPath;
    QString fileNm = ui->lineEdit_2->text();
    QString sfx = ui->lineEdit_3->text();
    if (!sfx.isEmpty()) { fileNm + "." + ui->lineEdit_3->text(); }
    _fileOperations->createAFile(filePath, fileNm);
}

void MainWindow::on_pushButton_6_clicked() {
    // 简繁转换
    if (ui->radioButton_5->isChecked()) {
        _simplifiedAndTradFunctions = Chardet::CharConversion::convertTradCharactersToSimplifiedCharacters;
    } else if (ui->radioButton_6->isChecked()) {
        _simplifiedAndTradFunctions = Chardet::CharConversion::convertSimplifiedCharactersToTradCharacters;
    }

    traverseTheFileLstAndProcess([&](const QString &fileNm) -> QString { return _simplifiedAndTradFunctions(fileNm); });
}

void MainWindow::on_pushButton_7_clicked() {
    // 给文件名加序列号
    auto count = ui->listWidget->count();
    auto brkChar = ui->brkChar->text();
    traverseTheFileLstAndProcess([&](const QString &fileNm) -> QString {
        auto serlNum = QString::number(count--);
        return serlNum + brkChar + fileNm;
    });
}

// 将字符添加到文件
void MainWindow::addCharactersToTheFileNm() {
    auto btn = qobject_cast<QPushButton *>(sender());
    auto charactersToBeAdded = ui->charactersToAdd->text();
    traverseTheFileLstAndProcess([&](const QString &fileNm) -> QString {
        auto nmAftChg = QString();
        // 头部添加或者尾部添加
        if (btn == ui->pushButton_8) nmAftChg = charactersToBeAdded + fileNm;
        else if (btn == ui->pushButton_9)
            nmAftChg = fileNm + charactersToBeAdded;
        return nmAftChg;
    });
}

void MainWindow::initializeTheForm() //初始化窗体
{ }

void MainWindow::initializeTheCtl() //初始化控件
{
    setWindowTitle(App::AppName); //设置窗口标题
}

template<typename T>
void MainWindow::traverseTheFileLstAndProcess(const T &func) {
    auto i = 0;
    while (i < ui->listWidget->count()) {
        auto fileNm = ui->listWidget->item(i)->text();
        auto filePath = ui->listWidget->item(i)->toolTip();
        //去除尾部的文件名
        auto index = filePath.lastIndexOf("/");
        filePath.remove(index + 1, fileNm.size());
        //分离文件名和后缀
        auto frstNm = extractFileNm(fileNm);
        auto suffix = extractTheSfx(fileNm);
        auto nmAftChg = func(frstNm) + suffix;
        _fileOperations->fileRename(filePath, fileNm, nmAftChg);
        i++;
    }
    showFilesUnderPath();
}

void MainWindow::on_replaceAll_clicked() {
    // 全部替换
    auto befStr = ui->lineEdit_bef->text();
    auto postStr = ui->lineEdit_rear->text();
    auto freq = ui->lineEdit_freq->text().toInt();
    auto left = ui->radioButton_left->isChecked();
    auto right = ui->radioButton_right->isChecked();
    auto excludeNumbers = ui->radioButton_aftTheNum->isChecked();

    if (befStr == postStr) return;

    traverseTheFileLstAndProcess([&](const QString &fileNm) -> QString {
        // 取文件名和后缀名
        auto frstNm = fileNm;
        auto serlNum = QString();

        // 是否排除开头的数字
        if (excludeNumbers) {
            auto regular2 = QRegExp("^[0-9]*");
            regular2.indexIn(fileNm);
            serlNum = regular2.cap(0);
            frstNm.remove(0, serlNum.size());
        }

        // 替换字符串
        if (left) {
            int i = 0, j = 0;
            while ((j = frstNm.indexOf(befStr, j)) != -1) {
                if (++i > freq && freq > 0) break;
                frstNm.replace(j, befStr.size(), postStr);
            }
        } else if (right) {
            int i = 0, j = 0;
            auto index = -1;
            while ((j = frstNm.lastIndexOf(befStr, index)) != -1) {
                if (++i > freq && freq > 0) break;
                frstNm.replace(j, befStr.size(), postStr);
                index = j - frstNm.size();
            }
        }

        return serlNum + frstNm;
    });
}

void MainWindow::delBtnTriggered() {
    //删除按钮触发
    auto btn = qobject_cast<QPushButton *>(sender());
    auto delAllPre = (btn == ui->pushButton_delAllPre);
    auto delAllFollowing = (btn == ui->pushButton_delAllFollowing);
    auto inclItself = ui->checkBox_2->isChecked();
    auto deletedContent = ui->lineEdit->text();
    auto frstAppearance = ui->radioButton_frstAppearance->isChecked();
    auto lastAppeared = ui->radioButton_lastAppeared->isChecked();
    traverseTheFileLstAndProcess([&](const QString &fileNm) -> QString {
        // 取文件名和后缀名
        auto frstNm = fileNm;
        auto leftShftLen = inclItself ? deletedContent.size() : 0;
        auto rightShftLen = inclItself ? 0 : deletedContent.size();
        auto index = frstAppearance ? frstNm.indexOf(deletedContent)
                                    : (lastAppeared ? frstNm.lastIndexOf(deletedContent) : -1); //首次出现还是最后出现
        if (index != -1) {
            auto moveSubscriptLeft = index + leftShftLen;
            auto moveSubscriptRight = index + rightShftLen;
            delAllPre ? frstNm.remove(0, moveSubscriptLeft)
                      : delAllFollowing ? frstNm.remove(moveSubscriptRight, frstNm.count() - moveSubscriptRight)
                                        : QString(); //删除前面还是删除后面
        }
        return frstNm;
    });
}

void MainWindow::on_pushButton_10_clicked() {
    //顺序删除
    auto whatToDel = ui->lineEdit_6->text();
    auto count = ui->lineEdit_7->text().toInt();
    auto frLeftToRight = ui->radioButton_7->isChecked();
    traverseTheFileLstAndProcess([&](const QString &fileNm) -> QString {
        auto frstNm = fileNm;
        auto index = 0;
        auto freq = count;
        freq == 0 ? --freq : freq; //一开始为0则不计入循环判断
        while (0 != freq-- &&
               -1 != (index = StrManipulation::srchStrForwardOrBackward(frstNm, whatToDel, frLeftToRight))) {
            frstNm.remove(index, whatToDel.size());
        }
        return frstNm;
    });
}

void MainWindow::on_pushButton_5_clicked() {
    //删除中间的内容
    auto startStr = ui->lineEdit_4->text();
    auto endStr = ui->lineEdit_5->text();
    auto frstAppearanceS = ui->radioButton->isChecked();
    auto lastAppearedE = ui->radioButton_4->isChecked();
    traverseTheFileLstAndProcess([&](const QString &fileNm) -> QString {
        auto frstNm = fileNm;
        auto frontIdx = StrManipulation::srchStrForwardOrBackward(frstNm, startStr, frstAppearanceS);
        auto postIdx = StrManipulation::srchStrForwardOrBackward(frstNm, endStr, lastAppearedE);
        if (frontIdx < 0 || postIdx < 0 || postIdx < frontIdx) return fileNm;
        frstNm.remove(frontIdx, postIdx - frontIdx - 1);
        return frstNm;
    });
}
