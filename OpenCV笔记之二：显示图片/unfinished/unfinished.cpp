// unfinished.cpp : �������̨Ӧ�ó������ڵ㡣
/// <summary>
/// ʵ��ͼ��OpenCV�Ļ�����ͼ��Ķ�ȡ���ҶȻ��ͱ�Ե��⣨sobel�㷨��,��������ʱ�� 2016.7.6��
/// </summary>
/// <remarks> �ĳ�����ʵ����򵥵�OpenCV�Ĳ�����������Ϊ�������̵Ļ���</remarks>
/// <remarks> ���潫½������OpenCV��ص�һЩ������Ϊ����</remarks>
/// <remarks> �㷨ʵ�ֵ�Ч�����ļ�������ʾ</remarks>

#include "stdafx.h"
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;
int main(int argc, char* argv[])
{
	const char*imagename="E:\\7681024.jpg";  //�˴���Ҫ����Ҫ��ʾͼƬ�ļ���ʵ���ļ�λ�ø���
	//���ļ��ж���ͼ��
	Mat img=imread(imagename);
	Mat src;
	cvtColor(img,src,CV_BGR2GRAY);
	imshow("GRAY",src);
	Mat dst;
	//�����ȡͼ��ʧ��
	if(img.empty())
	{
		fprintf(stderr,"Can not load image%s\n",imagename);
		return -1;
	}
	Sobel(src,dst,src.depth(),1,1);
	//��ʾͼ��
	imshow("Sobel",dst); 
	//�˺����ȴ����������������������
	waitKey();
	while(1);
	return 0;
}

 