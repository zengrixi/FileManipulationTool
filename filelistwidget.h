#ifndef FILELISTWIDGET_H
#define FILELISTWIDGET_H

#include <QListWidget>

#include "fileoperations.h"

class FileListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit FileListWidget(QWidget *parent = nullptr);
    virtual ~FileListWidget();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);

signals:
    void updLst();            //update list

public slots:
    void delTheFile();

private:
    FileOperations *_fileOperations;
    QListWidgetItem *_currentlyClickedItm;
};

#endif // FILELISTWIDGET_H
