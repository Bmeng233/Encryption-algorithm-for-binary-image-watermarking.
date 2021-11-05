#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("二值图像水印处理器-By高圣昌");
    w.show();

    return a.exec();
}
