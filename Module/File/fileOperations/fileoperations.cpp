#include "fileoperations.h"

#include <QDir>
#include <QDebug>
#include <QEventLoop>
#include <QtConcurrent>

FileOperations::FileOperations(QObject *parent)
    : QObject(parent)
{

}

void FileOperations::createAFile(const QString &saveRout, const QString &fileNm)
{
    QDir tmpDir;
    QString curDir = tmpDir.currentPath();
    // 如果路径不存在则创建
    if (!tmpDir.exists(saveRout))
    {
        qDebug() << "不存在该路径";
        tmpDir.mkpath(saveRout);
    }

    QFile *tmpFile = new QFile();
    // 将程序执行路径设置到保存路径下
    tmpDir.setCurrent(saveRout);
    // 检查保存路径下是否存在要创建的文件
    if (tmpFile->exists(fileNm))
    {
        qDebug() << "文件存在";
        return;
    }

    // 在当前路径下创建文件
    tmpFile->setFileName(fileNm);
    if (!tmpFile->open(QIODevice::WriteOnly|QIODevice::Text))
    {
        qDebug()<<"打开失败";
    }
    tmpFile->close();

    //将程序当前路径设置为原来的路径
    tmpDir.setCurrent(curDir);
}

void FileOperations::fileRename(const QString &filePath, const QString &curFileNm, const QString &changedNm)
{
    auto curr = filePath + curFileNm;
    auto change = filePath + changedNm;

    QEventLoop eventLoop;
    QtConcurrent::run([&] ()
    {
        QFile file(curr);
        file.rename(change);
        file.close();
        eventLoop.quit();
    });

    eventLoop.exec();
}

void FileOperations::delFiles(const QString &fileDir)
{
    QFile file(fileDir);
    file.remove();
}
