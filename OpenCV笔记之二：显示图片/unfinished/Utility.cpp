#include "Utility.h"


/// <summary>
/// 将彩色图像分解为R、G、B、A单通道的图像
/// </summary>
/// <param name="Src">需要处理的源图像的数据结构。</param>
/// <param name="Blue">保存Blue通道图像的数据结构。</param>
/// <param name="Green">保存Green通道图像的数据结构。</param>
/// <param name="Red">保存Red通道图像的数据结构。</param>
/// <param name="Alpha">保存Alpha通道图像的数据结构。</param>
/// <remarks>采用了8位并行处理，速度大约能提高20%。</remarks>
void SplitRGBA(TMatrix *Src, TMatrix *&Blue, TMatrix *&Green, TMatrix *&Red, TMatrix *&Alpha)
{

	Blue = CreateMatrix(Src->Width, Src->Height, Src->Depth, 1, true);
	Green = CreateMatrix(Src->Width, Src->Height, Src->Depth, 1, true);
	Red = CreateMatrix(Src->Width, Src->Height, Src->Depth, 1, true);
	if (Src->Channel == 4)	Alpha = CreateMatrix(Src->Width, Src->Height, Src->Depth, 1, true);

	int X, Y, Block, Width = Src->Width, Height = Src->Height;
	unsigned char *LinePS, *LinePB, *LinePG, *LinePR, *LinePA;
	const int BlockSize = 8;						
	Block = Width / BlockSize;						//	8路并行,再多开几路，速度并没有增加了

	if (Src->Channel == 3)
	{
		for (Y = 0; Y < Height; Y++)
		{
			LinePS = Src->Data + Y * Src->WidthStep;
			LinePB = Blue->Data + Y * Blue->WidthStep;
			LinePG = Green->Data + Y * Green->WidthStep;
			LinePR = Red->Data + Y * Red->WidthStep;
			for (X = 0; X < Block * BlockSize; X += BlockSize)			//	如果把LinePB全写在一起，速度反而慢一些
			{
				LinePB[0] = LinePS[0];		LinePG[0] = LinePS[1];		LinePR[0] = LinePS[2];			
				LinePB[1] = LinePS[3];		LinePG[1] = LinePS[4];		LinePR[1] = LinePS[5];
				LinePB[2] = LinePS[6];		LinePG[2] = LinePS[7];		LinePR[2] = LinePS[8];
				LinePB[3] = LinePS[9];		LinePG[3] = LinePS[10];		LinePR[3] = LinePS[11];
				LinePB[4] = LinePS[12];		LinePG[4] = LinePS[13];		LinePR[4] = LinePS[14];
				LinePB[5] = LinePS[15];		LinePG[5] = LinePS[16];		LinePR[5] = LinePS[17];
				LinePB[6] = LinePS[18];		LinePG[6] = LinePS[19];		LinePR[6] = LinePS[20];
				LinePB[7] = LinePS[21];		LinePG[7] = LinePS[22];		LinePR[7] = LinePS[23];	
				LinePB += 8;				LinePG += 8;				LinePR += 8;				LinePS += 24;
			}
			for(; X < Width; X++)
			{
				LinePB[0] = LinePS[0];		LinePG[0] = LinePS[1];		LinePR[0] = LinePS[2];
				LinePB++;					LinePG++;					LinePR++;					LinePS += 3;
			}
		}
	}
	else if (Src->Channel == 4)
	{
		for (Y = 0; Y < Height; Y++)
		{
			LinePS = Src->Data + Y * Src->WidthStep;
			LinePB = Blue->Data + Y * Blue->WidthStep;
			LinePG = Green->Data + Y * Green->WidthStep;
			LinePR = Red->Data + Y * Red->WidthStep;
			LinePA = Alpha->Data + Y * Alpha->WidthStep;
			for (X = 0; X < Block * BlockSize; X += BlockSize)
			{
				LinePB[0] = LinePS[0];		LinePG[0] = LinePS[1];		LinePR[0] = LinePS[2];		LinePA[0] = LinePS[3];
				LinePB[1] = LinePS[4];		LinePG[1] = LinePS[5];		LinePR[1] = LinePS[6];		LinePA[1] = LinePS[7];
				LinePB[2] = LinePS[8];		LinePG[2] = LinePS[9];		LinePR[2] = LinePS[10];		LinePA[2] = LinePS[11];
				LinePB[3] = LinePS[12];		LinePG[3] = LinePS[13];		LinePR[3] = LinePS[14];		LinePA[3] = LinePS[15];
				LinePB[4] = LinePS[16];		LinePG[4] = LinePS[17];		LinePR[4] = LinePS[18];		LinePA[4] = LinePS[19];
				LinePB[5] = LinePS[20];		LinePG[5] = LinePS[21];		LinePR[5] = LinePS[22];		LinePA[5] = LinePS[23];
				LinePB[6] = LinePS[24];		LinePG[6] = LinePS[25];		LinePR[6] = LinePS[26];		LinePA[6] = LinePS[27];
				LinePB[7] = LinePS[28];		LinePG[7] = LinePS[29];		LinePR[7] = LinePS[30];		LinePA[7] = LinePS[31];
				LinePB += 8;				LinePG += 8;				LinePR += 8;				LinePA += 8;				LinePS += 32;
			}
			for(; X < Width; X++)
			{
				LinePB[0] = LinePS[0];		LinePG[0] = LinePS[1];		LinePR[0] = LinePS[2];		LinePA[0] = LinePS[3];
				LinePB++;					LinePG++;					LinePR++;					LinePA++;					LinePS += 4;
			}
		}
	}
}

/// <summary>
/// 将R、G、B、A单通道的图像合并为彩色的图像。
/// </summary>
/// <param name="Dest">合并处理后的图像的数据结构。</param>
/// <param name="Blue">Blue通道图像的数据结构。</param>
/// <param name="Green">Green通道图像的数据结构。</param>
/// <param name="Red">Red通道图像的数据结构。</param>
/// <param name="Alpha">Alpha通道图像的数据结构。</param>
/// <remarks>采用了8位并行处理，速度大约能提高20%。</remarks>
void CombineRGBA(TMatrix *Dest, TMatrix *Blue, TMatrix *Green, TMatrix *Red, TMatrix *Alpha)
{
	int X, Y, Block, Width = Dest->Width, Height = Dest->Height;
	unsigned char *LinePD, *LinePB, *LinePG, *LinePR, *LinePA;
	const int BlockSize = 8;						
	Block = Width / BlockSize;						//	8路并行,再多开几路，速度并没有增加了

	if (Dest->Channel == 3)
	{
		for (Y = 0; Y < Height; Y++)
		{
			LinePD = Dest->Data + Y * Dest->WidthStep;
			LinePB = Blue->Data + Y * Blue->WidthStep;
			LinePG = Green->Data + Y * Green->WidthStep;
			LinePR = Red->Data + Y * Red->WidthStep;
			for (X = 0; X < Block * BlockSize; X += BlockSize)				//	如果把LinePB全写在一起，速度区别不大
			{
				LinePD[0] = LinePB[0];		LinePD[1] = LinePG[0];		LinePD[2] = LinePR[0];
				LinePD[3] = LinePB[1];		LinePD[4] = LinePG[1];		LinePD[5] = LinePR[1];
				LinePD[6] = LinePB[2];		LinePD[7] = LinePG[2];		LinePD[8] = LinePR[2];
				LinePD[9] = LinePB[3];		LinePD[10] = LinePG[3];		LinePD[11] = LinePR[3];
				LinePD[12] = LinePB[4];		LinePD[13] = LinePG[4];		LinePD[14] = LinePR[4];
				LinePD[15] = LinePB[5];		LinePD[16] = LinePG[5];		LinePD[17] = LinePR[5];	
				LinePD[18] = LinePB[6];		LinePD[19] = LinePG[6];		LinePD[20] = LinePR[6];	
				LinePD[21] = LinePB[7];		LinePD[22] = LinePG[7];		LinePD[23] = LinePR[7];	
				LinePB += 8;				LinePG += 8;				LinePR += 8;				LinePD += 24;
			}
			for(; X < Width; X++)
			{
				LinePD[0] = LinePB[0];		LinePD[1] = LinePG[0];		LinePD[2] = LinePR[0];		
				LinePB++;					LinePG++;					LinePR++;					LinePD += 3;
			}
		}
	}
	else if (Dest->Channel == 4)
	{
		for (Y = 0; Y < Height; Y++)
		{
			LinePD = Dest->Data + Y * Dest->WidthStep;
			LinePB = Blue->Data + Y * Blue->WidthStep;
			LinePG = Green->Data + Y * Green->WidthStep;
			LinePR = Red->Data + Y * Red->WidthStep;
			LinePA = Alpha->Data + Y * Alpha->WidthStep;
			for (X = 0; X < Block * BlockSize; X += BlockSize)
			{
				LinePD[0] = LinePB[0];		LinePD[1] = LinePG[0];		LinePD[2] = LinePR[0];		LinePD[3] = LinePA[0];
				LinePD[4] = LinePB[1];		LinePD[5] = LinePG[1];		LinePD[6] = LinePR[1];		LinePD[7] = LinePA[1];
				LinePD[8] = LinePB[2];		LinePD[9] = LinePG[2];		LinePD[10] = LinePR[2];		LinePD[11] = LinePA[2];
				LinePD[12] = LinePB[3];		LinePD[13] = LinePG[3];		LinePD[14] = LinePR[3];		LinePD[15] = LinePA[3];
				LinePD[16] = LinePB[4];		LinePD[17] = LinePG[4];		LinePD[18] = LinePR[4];		LinePD[19] = LinePA[4];
				LinePD[20] = LinePB[5];		LinePD[21] = LinePG[5];		LinePD[22] = LinePR[5];		LinePD[23] = LinePA[5];
				LinePD[24] = LinePB[6];		LinePD[25] = LinePG[6];		LinePD[26] = LinePR[6];		LinePD[27] = LinePA[6];
				LinePD[28] = LinePB[7];		LinePD[29] = LinePG[7];		LinePD[30] = LinePR[7];		LinePD[31] = LinePA[7];
				LinePB += 8;				LinePG += 8;				LinePR += 8;				LinePA += 8;				LinePD += 32;
			}
			for(; X < Width; X++)
			{
				LinePD[0] = LinePB[0];		LinePD[1] = LinePG[0];		LinePD[2] = LinePR[0];		LinePD[3] = LinePA[0];
				LinePB++;					LinePG++;					LinePD++;					LinePA++;					LinePD += 4;
				X++;
			}
		}
	}
}
/// <summary>
/// 按照指定的边缘模式计算扩展后各坐标的有理值
/// </summary>
/// <param name="Length">原始序列的长度。</param>
/// <param name="Left">左侧需要扩展的长度。</param>
/// <param name="Right">右侧需要扩展的长度。</param>
/// <param name="Edge">边缘扩展的模式。</param>
/// <returns>返回扩展后对应的坐标。</returns>
int *GetExpandPos(int Length, int Left, int Right, EdgeMode Edge)
{
	if (Left < 0 || Length < 0 || Right < 0) return NULL;
	int *Pos = (int *)AllocMemory((Left + Length + Right) * sizeof(int), false);
	if (Pos == NULL) return NULL;

	int X, PosX;
	for (X = -Left; X < Length + Right; X++)
	{
		if (X < 0)
		{
			if (Edge == EdgeMode::Tile)							//重复边缘像素
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
			if (Edge == EdgeMode::Tile)
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
/// 按指定的数据扩展现有的矩阵数据。
/// </summary>
/// <param name="Src">原始的矩阵数据 。</param>
/// <param name="Left">左侧需要扩展的长度。</param>
/// <param name="Top">上部需要扩展的长度。</param>
/// <param name="Right">右侧需要扩展的长度。</param>
/// <param name="Bottom">下部需要扩展的长度。</param>
/// <param name="Edge">边缘扩展的模式。</param>
/// <returns>返回扩展后矩阵对象，支持任意深度。</returns>
TMatrix *GetExpandMatrix(TMatrix *Src, int Left, int Top, int Right, int Bottom, EdgeMode Edge)
{
	if (Src == NULL || Src->Data == NULL) return NULL;
	if (Left < 0 || Right < 0 || Top < 0 || Bottom < 0) return NULL;

	int X, Y, SrcW, SrcH, DstW, DstH, ElementSize; 
	SrcW = Src->Width;				SrcH = Src->Height;
	DstW = SrcW + Left + Right;		DstH = SrcH + Top + Bottom;
	ElementSize	= Src->Channel * GetElementSize(Src->Depth);

	TMatrix *Dest = CreateMatrix(DstW, DstH, Src->Depth, Src->Channel, Src->RowAligned);
	if (Dest != NULL)
	{
		int *RowPos = GetExpandPos(SrcW, Left, Right, Edge);																//	获取坐标偏移量
		int *ColPos = GetExpandPos(SrcH, Top, Bottom, Edge);
		for (Y = 0; Y < SrcH; Y++)			
		{
			unsigned char *LinePS = Src->Data + Y * Src->WidthStep;															//	统一使用unsigned char有利于计算字节的复制数量
			unsigned char *LinePD = Dest->Data + (Y + Top) * Dest->WidthStep;
			for (X = 0; X < Left; X++)
				memcpy(LinePD + X * ElementSize, LinePS + RowPos[X] * ElementSize, ElementSize);							//	左侧的像素
			memcpy(LinePD + Left * ElementSize, LinePS, SrcW * ElementSize);												//	中间的像素											
			for (X = Left + SrcW; X < Left + SrcW + Right; X++)
				memcpy(LinePD + X * ElementSize, LinePS + RowPos[X] * ElementSize, ElementSize);							//	右侧的像素
		}
		for (Y = 0; Y < Top; Y++)
			memcpy(Dest->Data + Y * Dest->WidthStep, Dest->Data + (Top + ColPos[Y]) * Dest->WidthStep, Dest->WidthStep);	//	行行直接拷贝

		for (Y = Top + SrcH; Y < Top + SrcH + Bottom; Y++)
			memcpy(Dest->Data + Y * Dest->WidthStep, Dest->Data + (Top + ColPos[Y]) * Dest->WidthStep, Dest->WidthStep);

		FreeMemory(RowPos);
		FreeMemory(ColPos);
	}
	return Dest;
}

unsigned char ClampToByte(int Value)
{
	return ((Value | ((signed int)(255 - Value) >> 31) ) & ~((signed int)Value >> 31));
}