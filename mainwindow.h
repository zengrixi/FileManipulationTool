#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    typedef enum
    {
        File = 0x01, Folder = 0x02       
    }Type;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_checkBox_stateChanged(int arg1);

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_file_or_folder();

    void on_pushButton_3_clicked();

    void on_comboBox_editTextChanged(const QString &arg1);

    void on_comboBox_2_editTextChanged(const QString &arg1);

private:
    void initStyleSheet();
    void folderTraversal(const QString & path);

private:
    Ui::MainWindow *ui;

    Type _type;
    QString _currPath;
    Qt::WindowFlags _winflags;
};

#endif // MAINWINDOW_H
