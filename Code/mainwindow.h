#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "prgAlgorithm.h"

namespace Ui {
class MainWindow;
}
using coreData = QVector<bool>;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    //浏览按钮消息
    void on_BrowseButton_clicked();
    //编码按钮消息
    void on_EncodeButton_clicked();
    //解码按钮消息
    void on_DecodeButton_clicked();

private:
    Ui::MainWindow *ui;
    int wmLength;
    uchar* dst;
};

#endif // MAINWINDOW_H
