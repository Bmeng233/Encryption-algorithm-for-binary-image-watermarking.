#include "mainwindow.h"
#include "prgAlgorithm.h"
#include "ui_mainwindow.h"
#include <QtCore>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>



//构造函数
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
ui(new Ui::MainWindow){
    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);    // 禁止最大化按钮
    setFixedSize(635, 140);                     // 禁止拖动窗口大小
    ui->setupUi(this);
    //自动填充
    ui->waterMarkinfo->setText("0100100101011111011011000110111101110110011001010101111101010101");
}

//析构
MainWindow::~MainWindow()
{
    delete ui;
}

//浏览按钮
void MainWindow::on_BrowseButton_clicked()
{
    auto file = QFileDialog::getOpenFileName(this, tr("请选择正确的8位bmp二值图像"));
    //将路径添加到文本框
    ui->lineEdit->setText(file);
}


//编码按钮
void MainWindow::on_EncodeButton_clicked()
{
    coreData code;
    watermark MK;
    //读入二进制密钥
        //数字输入校验
        QRegExp regx("[0-1]+$");
        QValidator *validator = new QRegExpValidator(regx, this );
        ui->waterMarkinfo->setValidator( validator );
        QString str1 = ui->waterMarkinfo->text();
        code = MK.byte_to_Array(str1);
		delete validator;
    //生成随机密钥
    wmLength = code.length();
    int width, height;
    uchar* BmpBuffer = MK.BmpReader(ui->lineEdit->displayText().toStdString().data(), width, height);
    if(BmpBuffer)
    {
        //获取边缘
        uchar* edge = MK.edgeExtract(BmpBuffer, width, height);
        //生成图像
        dst = MK.generateImg(BmpBuffer, edge, width*height, code);
        MK.BmpSaver("encode.bmp", dst, height, width);
        QMessageBox::warning(this, "", "水印编码成功！");
    }
}

//解码按钮
void MainWindow::on_DecodeButton_clicked()
{
    //coreData code;
    watermark MK;
    int width, height;
    uchar* reader = MK.BmpReader(ui->lineEdit->displayText().toStdString().data(), width, height);
    if(reader)
     {
        //QString str1 = ui->waterMarkinfo->text();
        //code = MK.byte_to_Array(str1);
        //wmLength = code.length();
        //解码
        coreData code = MK.decodeImg(reader, dst, width, height, wmLength);
        //打印二进制水印
        QMessageBox::warning(this, " ", "水印为：" + MK.array_to_byte(code));
    }
}
