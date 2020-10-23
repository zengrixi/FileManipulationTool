#include "filelistwidget.h"

#include <QMenu>

FileListWidget::FileListWidget(QWidget *parent)
    : QListWidget(parent), _fileOperations(new FileOperations()), _currentlyClickedItm(nullptr) { }

FileListWidget::~FileListWidget() {
    _fileOperations->deleteLater();
}

void FileListWidget::contextMenuEvent(QContextMenuEvent *event) //上下文菜单事件
{
    Q_UNUSED(event)

    QMenu popMenu;
    QAction del("删除");

    //关联菜单点击事件
    connect(&del, &QAction::triggered, this, &FileListWidget::delTheFile);

    _currentlyClickedItm = itemAt(mapFromGlobal(QCursor::pos()));
    if (_currentlyClickedItm != nullptr) { popMenu.addAction(&del); }
    popMenu.exec(QCursor::pos()); //菜单出现的位置为当前鼠标的位置
}

void FileListWidget::delTheFile() //删除文件
{
    if (_currentlyClickedItm != nullptr) {
        auto filePath = _currentlyClickedItm->toolTip();
        _currentlyClickedItm = nullptr;
        _fileOperations->delFiles(filePath);
        emit updLst(); //更新文件列表
    }
}
