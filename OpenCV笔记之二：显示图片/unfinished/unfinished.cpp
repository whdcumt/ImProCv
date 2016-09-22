// unfinished.cpp : 定义控制台应用程序的入口点。
/// <summary>
///  实现图像方框模糊效果，O(1)复杂度，最新整理时间 2016.9.20。
/// </summary>
#include "stdafx.h"
#include "stdlib.h"
#include <malloc.h>
#include<string.h>
/// <summary>
/// 分配内存函数,以32字节对齐。
/// </summary>
/// <param name="Size">需要使用的内存大小（以字节为单位）。</param>
/// <param name="ZeroMemory">内存数据是否进行清零处理 。</param>
/// <returns>返回所分配内存的指针，失败则返回NULL。</returns>
extern "C" void *AllocMemory(unsigned int Size, bool ZeroMemory)	//	https://technet.microsoft.com/zh-cn/library/8z34s9c6
{
	void *Ptr = _aligned_malloc(Size, 32);				//	考虑SSE,AVX等高级函数的需求，分配起始地址使用32字节对齐。其实_mm_malloc就是这个函数
	if (Ptr != NULL && ZeroMemory == true)
		memset(Ptr, 0, Size);
	return Ptr;
}
/// <summary>
/// 释放内存。
/// </summary>
/// <param name="Ptr">内存数据的地址。</param>
extern "C" void FreeMemory(void *Ptr)
{
	if (Ptr != NULL) _aligned_free(Ptr);		//	_mm_free就是该函数
}
/// <summary>
/// 按照指定的边缘模式计算扩展后各坐标的有理值
/// </summary>
/// <param name="Length">原始序列的长度。</param>
/// <param name="Left">左侧需要扩展的长度。</param>
/// <param name="Right">右侧需要扩展的长度。</param>
/// <param name="Edge">边缘扩展的模式。</param>
/// <returns>返回扩展后对应的坐标。</returns>
extern "C" int *GetExpandPos(int Length, int Left, int Right, int Edge)
{
	if (Left < 0 || Length < 0 || Right < 0) return NULL;
	int *Pos = (int *)AllocMemory((Left + Length + Right) * sizeof(int), false);
	if (Pos == NULL) return NULL;

	int X, PosX;
	for (X = -Left; X < Length + Right; X++)
	{
		if (X < 0)
		{
			if (Edge == 0)							//重复边缘像素
				Pos[X + Left] = 0;
			else
			{
				PosX = -X;
				while (PosX >= Length) PosX -= Length;			// 做镜像数据
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
/// 实现图像方框模糊效果，O(1)复杂度，最新整理时间 2016.9.20
/// </summary>
/// <param name="Src">需要处理的源图像的数据结构。</param>
/// <param name="Dest">保存处理后的图像的数据结构。</param>
/// <param name="Radius">方框模糊的半径，有效范围[1,1000]。</param>
/// <param name="EdgeBehavior">边缘处数据的处理方法，0表示重复边缘像素，1使用镜像的方式对边缘像素求均值。</param>
/// <remarks> 1: 能处理8位灰度和24位图像。</remarks>
/// <remarks> 2: Src和Dest可以相同，在相同时速度会稍慢。</remarks>
/// <remarks> 3: 开发者laviewpbt ,QQ - 33184777。</remarks>
extern "C" void  BoxBlur(double *Src, double *Dest, int Width,int Height,int Channel,unsigned char Radius, int Edge)
{
	int X, Y, Z, Index;  //X,Y,Z自变量,Width图像宽度，Height为图形高度，Channel图像的通道数
	double Value, ValueB, ValueG, ValueR;     //每个通道的灰度值
	int *RowPos, *ColPos;
	double  *ColSum, *Diff; 
	//Width = Src->Width, Height = Src->Height, Channel = Src->Channel;
	int Size = 2 * Radius + 1,  Amount = Size * Size, HalfAmount = Amount / 2;
	RowPos = GetExpandPos(Width, Radius, Radius, Edge); //扩展行之后的数值
	ColPos = GetExpandPos(Height, Radius, Radius, Edge);//扩展列之后的数值
	ColSum = (double *)AllocMemory(Width * Channel * sizeof(double), true); //开辟行内存地址
	Diff   = (double *)AllocMemory((Width - 1) * Channel * sizeof(double), true);//开辟行内存地址-1
	double *RowData = (double*)AllocMemory((Width + 2 * Radius) * Channel, true);//开辟行内存地址
	double *Data=(double *)AllocMemory(Height *Width * Channel*sizeof(double), false);//	用了True在大循环里换慢很多
	for (Y = 0; Y < Height; Y++)					//	水平方向的耗时比垂直方向上的大
	{
		double *LinePS = Src + Y *  Width*Channel*sizeof(double);
		double *LinePD= (double*)(Data + Y * Width*Channel*sizeof(double));

		//	拷贝一行数据及边缘部分部分到临时的缓冲区中
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

		double *AddPos = RowData + Size * Channel;
		double *SubPos = RowData;
					
		for(X = 0; X < (Width - 1) * Channel; X++)
			Diff[X] = AddPos[X] - SubPos[X];

		//	第一个点要特殊处理
		if (Channel == 1)
		{
			for(Z = 0, Value = 0; Z < Size; Z++)	Value += RowData[Z];
			LinePD[0] = Value;

			for(X = 1; X < Width; X ++)
			{
				Value += Diff[X - 1];	LinePD[X] = Value;				//	分四路并行速度又能提高很多
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
		
	for (Y = 0; Y < Size - 1; Y++)			//	注意没有最后一项哦						//	这里的耗时只占整个的15%左右
	{
		double *LinePS = (double*)(Data + ColPos[Y] * Width*Channel*sizeof(double));
		for(X = 0; X < Width * Channel; X++)	ColSum[X] += LinePS[X];
	}

	for (Y = 0; Y < Height; Y++)
	{
		double* LinePD	= Dest + Y * Width*Channel*sizeof(double);	
		double *AddPos	= (double*)(Data + ColPos[Y + Size - 1] * Width*Channel*sizeof(double));
		double *SubPos = (double*)(Data + ColPos[Y] * Width*Channel*sizeof(double));

		for(X = 0; X < Width * Channel; X++)
		{
			Value = ColSum[X] + AddPos[X];
			LinePD[X] = (Value + HalfAmount) / Amount;					//		+  HalfAmount 主要是为了四舍五入
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
	return 0;
}
