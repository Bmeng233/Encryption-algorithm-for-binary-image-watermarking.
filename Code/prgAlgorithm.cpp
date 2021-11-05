#include "prgAlgorithm.h"
#include "lib.h"

//水印具体算法
//读取Bmp文件
uchar* watermark::BmpReader(const char *bmpName, int& bmpWidth, int& bmpHeight)
{
    //以二进制打开
    FILE *fp = fopen(bmpName, "rb");
    //空指针则返回文件错误
    if(fp == Q_NULLPTR)
    {
        QMessageBox::warning(Q_NULLPTR, "错误", "请选择文件!");
        return Q_NULLPTR;
    }
    // 跳过无用文件头信息
    fseek(fp, sizeof(BITMAPFILEHEADER), 0);
    // 读取文件头有用信息
    BITMAPINFOHEADER* head = new BITMAPINFOHEADER;
    fread(head, sizeof(BITMAPINFOHEADER), 1, fp);
    //读取宽高
    bmpWidth = head->biWidth;
    bmpHeight = head->biHeight;
    int bitCount = head->biBitCount;
    //判断图片编码位数，本算法只能处理8位bmp文件
    if(bitCount == 8)
    {
        //专用于存储RGB颜色的数据类型
        rgb = new RGBQUAD[256];
        fread(rgb, sizeof(RGBQUAD), 256, fp);

        uchar* allPixel = new uchar[ bmpWidth * bmpHeight ];
        fread(allPixel, sizeof(uchar), bmpWidth * bmpHeight, fp);
        fclose(fp);

        uchar* reader = new uchar[bmpWidth * bmpHeight];
        for(int i = 0; i < bmpHeight; i++)
        {
            for(int j = 0; j<bmpWidth; j++)
            {
                if(allPixel[(bmpHeight- i - 1)*bmpWidth + j] != 255 && allPixel[(bmpHeight- i - 1)*bmpWidth + j] != 0)
                {
                    QMessageBox::warning(Q_NULLPTR, "错误", "该bmp图像非二值图像");
                    return Q_NULLPTR;
                }
                reader[i*bmpWidth + j] = allPixel[(bmpHeight- i - 1)*bmpWidth + j];
            }
        }
        return reader;
    }
    else
    {
        QMessageBox::warning(Q_NULLPTR, "错误", "只能处理8bitbmp图像");
        return Q_NULLPTR;
    }
}
//存储bmp文件
bool watermark::BmpSaver(const char* filename, uchar* buffer, const int height, const int width)
{
    uchar* data = new uchar[height*width];
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j<width; j++)
        {
            data[i*width + j] = buffer[(height- i - 1)*width + j];
        }
    }
    //写入bmp头信息
    int colorTableSize = 1024;
    BITMAPFILEHEADER fileHeader;
    fileHeader.bfType = 0x4D42;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTableSize + height*width;
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTableSize;

    BITMAPINFOHEADER bitmapHeader = { 0 };
    bitmapHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapHeader.biHeight = height;
    bitmapHeader.biWidth = width;
    bitmapHeader.biPlanes = 1;
    bitmapHeader.biBitCount = 8;
    bitmapHeader.biSizeImage = height*width;
    bitmapHeader.biCompression = 0;

    FILE *fp = fopen(filename, "wb");
    if(fp == Q_NULLPTR)
    {
        QMessageBox::warning(Q_NULLPTR, "错误", "保存错误！");
        return false;
    }
    else
    {
        fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
        fwrite(&bitmapHeader, sizeof(BITMAPINFOHEADER), 1, fp);
        fwrite(rgb, sizeof(RGBQUAD), 256, fp);
        fwrite(data, height*width, 1, fp);
        fclose(fp);
        return true;
    }
}


//核心算法
//提取图像边缘
uchar* watermark::edgeExtract(uchar* buffer, const int width, const int height)
{
    uchar* UP = differential(moveImg(buffer, width, height, 0, 1), buffer, width*height);
    uchar* DOWN = differential(moveImg(buffer, width, height, 0, -1), buffer, width*height);
    uchar* LEFT = differential(moveImg(buffer, width, height, 1, 0), buffer, width*height);
    uchar* RIGHT = differential(moveImg(buffer, width, height, -1, 0), buffer, width*height);
    uchar* imgMap = new uchar[height*width];
    for(int i = 0; i<height*width; i++)
    {
        LEFT[i] = LEFT[i]/255;
        RIGHT[i] = RIGHT[i]/255;
        UP[i] = UP[i]/255;
        DOWN[i] = DOWN[i]/255;

        int lr = LEFT[i] + RIGHT[i];
        int tb = UP[i] + DOWN[i];
        int b = lr + tb;

        imgMap[i] = 0;
        if((b == 1) ||(b == 2 && lr != 2 && tb != 2))
        {
            imgMap[i] = 1;
        }
    }

    uchar* res = new uchar[height*width];
    for(int i = 0; i<height; i++)
    {
        for(int j = 0; j<width; j++)
        {
            res[i*width + j] = imgMap[i*width + j] * 255;
            if(imgMap[i*width + j])
            {
                int sum1 = 0, sum2 = 0;
                for(int a = -1; a<2; a++)
                {
                    if(a + i <0 || a+ i >= height)
                    {
                        continue;
                    }
                    for(int b = -1; b<2; b++)
                    {
                        if(b +j <0 || b+j>=width)
                        {
                            continue;
                        }
                        sum1 += buffer[(a + i) *width + (b + j)]/255;
                        sum2 += imgMap[(a + i) *width + (b + j)];
                    }
                }
                if(sum1 == sum2)
                {
                    res[i*width + j] = 0;
                }
            }
        }
    }

    return res;
}

uchar* watermark::moveImg(uchar* buffer, const int width, const int height, int x_off, int y_off)
{
    uchar* res = new uchar[width*height];
    for(int i = 0; i<height; i++)
    {
        for(int j = 0;j<width; j++)
        {
            if(i-x_off < 0 || j-y_off < 0)
            {
                res[i*width + j] = 0;
            }
            else
            {
                res[i*width + j] = buffer[(i - x_off)*width + (j - y_off)];
            }
        }
    }

    return res;
}

uchar* watermark::differential(uchar* buffer1, uchar* buffer2, const int size)
{
    uchar* res = new uchar[size];
    for(int i = 0; i<size; i++)
    {
        if(buffer1[i] > buffer2[i])
            res[i] = buffer1[i] - buffer2[i];
    }
    return res;
}



//生成图像
uchar* watermark::generateImg(uchar* buffer, uchar* edge, const int size, coreData code)
{
    uchar* res = new uchar[size];
    for(int i = 0; i<size; i++)
    {
        res[i] = buffer[i];
    }
    int count = 0;
    for(int i = 0; i<size; i++)
    {
        if(edge[i]==255)
        {
            res[i] = 255*code[count++];
            if(count == code.length())
            {
                return res;
            }
        }
    }
    return Q_NULLPTR;
}

//提取图像数据信息
coreData watermark::decodeImg(uchar* buffer, uchar* dst, const int width, const int height, const int length)
{
    uchar* edge = edgeExtract(buffer, width, height);
    coreData res;
    for(int i = 0; i<width*height; i++)
    {
        if(edge[i] == 255)
        {
            res.append(dst[i]);
            if(length == res.length())
            {
                return res;
            }
        }
    }
    return coreData();
}

coreData watermark::byte_to_Array(QString &number)
{
    coreData res;
    for(auto byte : number)
    {
        if(byte == '1')
        {
            res.append(true);
        }
        else if(byte == '0')
        {
            res.append(false);
        }
        else
        {
            QMessageBox::warning(nullptr, "错误", "请输入二进制格式水印");
            break;
        }
    }
    return res;
}

QString watermark::array_to_byte(coreData &array)
{
    QString res;
    for(auto ele:array)
    {
        if(ele)
        {
            res.append('1');
        }
        else
        {
            res.append('0');
        }
    }
    return res;
}
