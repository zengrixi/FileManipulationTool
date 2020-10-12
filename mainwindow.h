#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "fileoperations.h"
#include "charconversion.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum class Type
    {
        File = 0x01, Folder = 0x02       
    };

    enum class AddedLocation
    {
        Head = 0x01, Tail = 0x02
    };

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_checkBox_stateChanged(int arg1);

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void selFileOrFolder();

    void on_pushButton_3_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void addCharactersToTheFileNm();

private:
    void initStyleSheet();
    void showFilesUnderPath();
    template <typename T>
    void traverseTheFileLstAndProcess(T const &func);


private:
    Ui::MainWindow *ui;

    Type _type;
    QString _currPath;
    Qt::WindowFlags _winflags;
    FileOperations *_fileOperations;
    Chardet::pFunSimplifiedAndTradConversion _simplifiedAndTradFunctions;
};

#endif // MAINWINDOW_H
