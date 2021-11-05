#ifndef PRGALGORITHM_H
#define PRGALGORITHM_H

#include "lib.h"

class watermark
{
public:
    //数组转换为字节
    QString array_to_byte(coreData &array);
    //字节转换为数组
    coreData byte_to_Array(QString &number);
    //解码图片
    coreData decodeImg(uchar* buffer, uchar* dst, const int width, const int height, const int length);
    //寻找边缘
    uchar* edgeExtract(uchar* buffer, const int width, const int height);
    //读取图片
    uchar* BmpReader(const char *bmpName, int& bmpWidth, int& bmpHeight);
    //保存图片
    bool BmpSaver(const char* filename, uchar* buffer, const int height, const int width);
    //算法处理
    uchar* differential(uchar* buffer1, uchar* buffer2, const int size);
    uchar* moveImg(uchar* buffer, const int width, const int height, int x_off, int y_off);
    //生成图像
    uchar* generateImg(uchar* buffer, uchar* edge, const int size, coreData code);
private:
    BITMAPINFOHEADER BMIH;
    BITMAPFILEHEADER BMFH;
    int biWidth;
    int biHeight;
    int biBitCount;
    unsigned char *allPixel;
    RGBQUAD *rgb;
protected:

};


#endif // PRGALGORITHM_H
