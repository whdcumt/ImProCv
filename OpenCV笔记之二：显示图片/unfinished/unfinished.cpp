// unfinished.cpp : �������̨Ӧ�ó������ڵ㡣
/// <summary>
///  ʵ��ͼ�񷽿�ģ��Ч����O(1)���Ӷȣ���������ʱ�� 2016.9.20��
/// </summary>
#include "stdafx.h"
#include "stdlib.h"
#include <malloc.h>
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;
/// <summary>
/// �����ڴ溯��,��32�ֽڶ��롣
/// </summary>
/// <param name="Size">��Ҫʹ�õ��ڴ��С�����ֽ�Ϊ��λ����</param>
/// <param name="ZeroMemory">�ڴ������Ƿ�������㴦�� ��</param>
/// <returns>�����������ڴ��ָ�룬ʧ���򷵻�NULL��</returns>
void *AllocMemory(unsigned int Size, bool ZeroMemory)	//	https://technet.microsoft.com/zh-cn/library/8z34s9c6
{
	void *Ptr = _aligned_malloc(Size, 32);				//	����SSE,AVX�ȸ߼����������󣬷�����ʼ��ַʹ��32�ֽڶ��롣��ʵ_mm_malloc�����������
	if (Ptr != NULL && ZeroMemory == true)
		memset(Ptr, 0, Size);
	return Ptr;
}
/// <summary>
/// �ͷ��ڴ档
/// </summary>
/// <param name="Ptr">�ڴ����ݵĵ�ַ��</param>
void FreeMemory(void *Ptr)
{
	if (Ptr != NULL) _aligned_free(Ptr);		//	_mm_free���Ǹú���
}
/// <summary>
/// ����ָ���ı�Եģʽ������չ������������ֵ
/// </summary>
/// <param name="Length">ԭʼ���еĳ��ȡ�</param>
/// <param name="Left">�����Ҫ��չ�ĳ��ȡ�</param>
/// <param name="Right">�Ҳ���Ҫ��չ�ĳ��ȡ�</param>
/// <param name="Edge">��Ե��չ��ģʽ��</param>
/// <returns>������չ���Ӧ�����ꡣ</returns>
int *GetExpandPos(int Length, int Left, int Right, int Edge)
{
	if (Left < 0 || Length < 0 || Right < 0) return NULL;
	int *Pos = (int *)AllocMemory((Left + Length + Right) * sizeof(int), false);
	if (Pos == NULL) return NULL;

	int X, PosX;
	for (X = -Left; X < Length + Right; X++)
	{
		if (X < 0)
		{
			if (Edge == 0)							//�ظ���Ե����
				Pos[X + Left] = 0;
			else
			{
				PosX = -X;
				while (PosX >= Length) PosX -= Length;			// ����������
				Pos[X + Left] = PosX;
			}
		}
		else if (X >= Length)
		{
			if (Edge == 0)
				Pos[X + Left] = Length - 1;
			else
			{
				PosX = Length - (X - Length + 2);
				while (PosX < 0) PosX += Length;
				Pos[X + Left] = PosX;
			}
		}
		else
		{
			Pos[X + Left] = X;
		}
	}
	return Pos;
}
/// <summary>
/// ʵ��ͼ�񷽿�ģ��Ч����O(1)���Ӷȣ���������ʱ�� 2016.9.20
/// </summary>
/// <param name="Src">��Ҫ�����Դͼ������ݽṹ��</param>
/// <param name="Dest">���洦����ͼ������ݽṹ��</param>
/// <param name="Radius">����ģ���İ뾶����Ч��Χ[1,1000]��</param>
/// <param name="EdgeBehavior">��Ե�����ݵĴ�������0��ʾ�ظ���Ե���أ�1ʹ�þ���ķ�ʽ�Ա�Ե�������ֵ��</param>
/// <remarks> 1: �ܴ���8λ�ҶȺ�24λͼ��</remarks>
/// <remarks> 2: Src��Dest������ͬ������ͬʱ�ٶȻ�������</remarks>
/// <remarks> 3: ������laviewpbt ,QQ - 33184777��</remarks>
void  BoxBlur(unsigned char *Src, unsigned char *Dest, int Width,int Height,int Channel,unsigned char Radius, int Edge)
{
	int X, Y, Z, Index;  //X,Y,Z�Ա���,Widthͼ���ȣ�HeightΪͼ�θ߶ȣ�Channelͼ���ͨ����
	int Value, ValueB, ValueG, ValueR;     //ÿ��ͨ���ĻҶ�ֵ
	int *RowPos, *ColPos;
	int  *ColSum, *Diff; 
	//Width = Src->Width, Height = Src->Height, Channel = Src->Channel;
	int Size = 2 * Radius + 1,  Amount = Size * Size, HalfAmount = Amount / 2;
	RowPos = GetExpandPos(Width, Radius, Radius, Edge); //��չ��֮�����ֵ
	ColPos = GetExpandPos(Height, Radius, Radius, Edge);//��չ��֮�����ֵ
	ColSum = (int *)AllocMemory(Width * Channel * sizeof(int), true); //�������ڴ��ַ
	Diff   = (int *)AllocMemory((Width - 1) * Channel * sizeof(int), true);//�������ڴ��ַ-1
	unsigned char *RowData = (unsigned char *)AllocMemory((Width + 2 * Radius) * Channel, true);//�������ڴ��ַ
	unsigned char *Data=(unsigned char *)AllocMemory(Height *Width * Channel*sizeof(int), false);//	����True�ڴ�ѭ���ﻻ���ܶ�
	for (Y = 0; Y < Height; Y++)					//	ˮƽ����ĺ�ʱ�ȴ�ֱ�����ϵĴ�
	{
		unsigned char *LinePS = Src + Y *  Width*Channel*sizeof(unsigned char);
		int *LinePD= (int*)(Data + Y * Width*Channel*sizeof(int));

		//	����һ�����ݼ���Ե���ֲ��ֵ���ʱ�Ļ�������
		if (Channel == 1)
		{
			for (X = 0; X < Radius; X++)				
				RowData[X] = LinePS[RowPos[X]];						
			memcpy(RowData + Radius, LinePS, Width);																						
			for (X = Radius + Width; X < Radius + Width + Radius; X++)	
				RowData[X] = LinePS[RowPos[X]];								
		}
		else if (Channel == 3)
		{
			for (X = 0; X < Radius; X++)
			{
				Index = RowPos[X] * 3;
				RowData[X * 3] = LinePS[Index];		
				RowData[X * 3 + 1] = LinePS[Index + 1];		
				RowData[X * 3 + 2] = LinePS[Index + 2];		
			}
			memcpy(RowData + Radius * 3, LinePS, Width * 3);																						
			for (X = Radius + Width; X < Radius + Width + Radius; X++)	
			{
				Index = RowPos[X] * 3;
				RowData[X * 3 + 0] = LinePS[Index + 0];		
				RowData[X * 3 + 1] = LinePS[Index + 1];		
				RowData[X * 3 + 2] = LinePS[Index + 2];		
			}
		}

		unsigned char *AddPos = RowData + Size * Channel;
		unsigned char *SubPos = RowData;
					
		for(X = 0; X < (Width - 1) * Channel; X++)
			Diff[X] = AddPos[X] - SubPos[X];

		//	��һ����Ҫ���⴦��
		if (Channel == 1)
		{
			for(Z = 0, Value = 0; Z < Size; Z++)	Value += RowData[Z];
			LinePD[0] = Value;

			for(X = 1; X < Width; X ++)
			{
				Value += Diff[X - 1];	LinePD[X] = Value;				//	����·�����ٶ�������ߺܶ�
			}
		}
		else if (Channel == 3)
		{
			for(Z = 0, ValueB = ValueG = ValueR = 0; Z < Size; Z++)
			{
				ValueB += RowData[Z * 3 + 0];
				ValueG += RowData[Z * 3 + 1];
				ValueR += RowData[Z * 3 + 2];
			}
			LinePD[0] = ValueB;	LinePD[1] = ValueG;	LinePD[2] = ValueR;

			for(X = 1; X < Width; X ++)
			{
				Index = X * 3;	
				ValueB += Diff[Index - 3];		LinePD[Index + 0] = ValueB;
				ValueG += Diff[Index - 2];		LinePD[Index + 1] = ValueG;
				ValueR += Diff[Index - 1];		LinePD[Index + 2] = ValueR;
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////
		
	for (Y = 0; Y < Size - 1; Y++)			//	ע��û�����һ��Ŷ						//	����ĺ�ʱֻռ������15%����
	{
		int *LinePS = (int*)(Data + ColPos[Y] * Width*Channel*sizeof(int));
		for(X = 0; X < Width * Channel; X++)	ColSum[X] += LinePS[X];
	}

	for (Y = 0; Y < Height; Y++)
	{
		unsigned char* LinePD	= Dest + Y * Width*Channel*sizeof(unsigned char);	
		int *AddPos	= (int*)(Data + ColPos[Y + Size - 1] * Width*Channel*sizeof(int));
		int *SubPos = (int*)(Data + ColPos[Y] * Width*Channel*sizeof(int));

		for(X = 0; X < Width * Channel; X++)
		{
			Value = ColSum[X] + AddPos[X];
			LinePD[X] = (Value + HalfAmount) / Amount;					//		+  HalfAmount ��Ҫ��Ϊ����������
			ColSum[X] = Value - SubPos[X];
		}
	}
	FreeMemory(RowPos);
	FreeMemory(ColPos);
	FreeMemory(Diff);
	FreeMemory(Data);
	FreeMemory(ColSum);
	FreeMemory(RowData);
}


int main(int argc, char* argv[])
{
	const char*imagename="E:\\InputName.bmp";  //�˴���Ҫ����Ҫ��ʾͼƬ�ļ���ʵ���ļ�λ�ø���
	//���ļ��ж���ͼ��
	Mat img=imread(imagename);
	Mat img1=imread(imagename);
	Mat Src,Dest;
	cvtColor(img,Src,CV_BGR2GRAY);
	cvtColor(img1,Dest,CV_BGR2GRAY);
	//imshow("img",img);
	//imshow("img1",img1);
	 imshow("Src",Src);
	//imshow("Dest",Dest);
	//Mat dst;
	//�����ȡͼ��ʧ��
	//if(img.empty())
	//{
	//	fprintf(stderr,"Can not load image%s\n",imagename);
	//	return -1;
	//}
	//Sobel(src,dst,src.depth(),1,1);
	//��ʾͼ��
	//imshow("Sobel",dst); 
	//�˺����ȴ����������������������
	BoxBlur(Src.data,Dest.data,Src.cols,Src.rows,1,3,1); /*(1)ɾ��__stdcall(2)TMatrix�޸�Ϊunsigned char(2)����Width,Height,Channels */
	//BoxBlur(img.data,img1.data,img.cols,img.rows,3,3,1); /*(1)ɾ��__stdcall(2)TMatrix�޸�Ϊunsigned char(2)����Width,Height,Channels */
	imshow("��ֵģ��",Dest);
	waitKey();
	while(1);
	return 0;
}
