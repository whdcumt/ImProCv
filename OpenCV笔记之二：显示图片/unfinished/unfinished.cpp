// unfinished.cpp : �������̨Ӧ�ó������ڵ㡣
/// <summary>
///  ʵ��ͼ�񷽿�ģ��Ч����O(1)���Ӷȣ���������ʱ�� 2016.9.20��
/// </summary>
#include "stdafx.h"
#include "stdlib.h"
#include <malloc.h>
#include<string.h>
/// <summary>
/// �����ڴ溯��,��32�ֽڶ��롣
/// </summary>
/// <param name="Size">��Ҫʹ�õ��ڴ��С�����ֽ�Ϊ��λ����</param>
/// <param name="ZeroMemory">�ڴ������Ƿ�������㴦�� ��</param>
/// <returns>�����������ڴ��ָ�룬ʧ���򷵻�NULL��</returns>
extern "C" void *AllocMemory(unsigned int Size, bool ZeroMemory)	//	https://technet.microsoft.com/zh-cn/library/8z34s9c6
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
extern "C" void FreeMemory(void *Ptr)
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
extern "C" void  BoxBlur(double *Src, double *Dest, int Width,int Height,int Channel,unsigned char Radius, int Edge)
{
	int X, Y, Z, Index;  //X,Y,Z�Ա���,Widthͼ���ȣ�HeightΪͼ�θ߶ȣ�Channelͼ���ͨ����
	double Value, ValueB, ValueG, ValueR;     //ÿ��ͨ���ĻҶ�ֵ
	int *RowPos, *ColPos;
	double  *ColSum, *Diff; 
	//Width = Src->Width, Height = Src->Height, Channel = Src->Channel;
	int Size = 2 * Radius + 1,  Amount = Size * Size, HalfAmount = Amount / 2;
	RowPos = GetExpandPos(Width, Radius, Radius, Edge); //��չ��֮�����ֵ
	ColPos = GetExpandPos(Height, Radius, Radius, Edge);//��չ��֮�����ֵ
	ColSum = (double *)AllocMemory(Width * Channel * sizeof(double), true); //�������ڴ��ַ
	Diff   = (double *)AllocMemory((Width - 1) * Channel * sizeof(double), true);//�������ڴ��ַ-1
	double *RowData = (double*)AllocMemory((Width + 2 * Radius) * Channel, true);//�������ڴ��ַ
	double *Data=(double *)AllocMemory(Height *Width * Channel*sizeof(double), false);//	����True�ڴ�ѭ���ﻻ���ܶ�
	for (Y = 0; Y < Height; Y++)					//	ˮƽ����ĺ�ʱ�ȴ�ֱ�����ϵĴ�
	{
		double *LinePS = Src + Y *  Width*Channel*sizeof(double);
		double *LinePD= (double*)(Data + Y * Width*Channel*sizeof(double));

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

		double *AddPos = RowData + Size * Channel;
		double *SubPos = RowData;
					
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
	return 0;
}
