// unfinished.cpp : 定义控制台应用程序的入口点。
/// <summary>
/// 实现图像OpenCV的基本的图像的读取、灰度化和边缘检测（sobel算法）,最新整理时间 2016.7.6。
/// </summary>
/// <remarks> 改程序是实现最简单的OpenCV的操作，可以作为其他工程的基础</remarks>
/// <remarks> 后面将陆续增加OpenCV相关的一些程序，作为积累</remarks>
/// <remarks> 算法实现的效果如文件夹中所示</remarks>

#include "stdafx.h"
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;
int main(int argc, char* argv[])
{
	const char*imagename="E:\\7681024.jpg";  //此处需要根据要显示图片文件的实际文件位置更改
	//从文件中读入图像
	Mat img=imread(imagename);
	Mat src;
	cvtColor(img,src,CV_BGR2GRAY);
	imshow("GRAY",src);
	Mat dst;
	//如果读取图像失败
	if(img.empty())
	{
		fprintf(stderr,"Can not load image%s\n",imagename);
		return -1;
	}
	Sobel(src,dst,src.depth(),1,1);
	//显示图像
	imshow("Sobel",dst); 
	//此函数等待按键，按键盘任意键返回
	waitKey();
	while(1);
	return 0;
}

 