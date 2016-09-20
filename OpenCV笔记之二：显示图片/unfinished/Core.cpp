#include "stdAfx.h"
#include "Utility.h"


/// <summary>
/// 根据矩阵元素的类型来获取一个元素实际占用的字节数
/// </summary>
/// <param name="Depth">矩阵元素的类型。</param>
/// <remarks> 只作为内部使用。</remarks>
int GetElementSize(int Depth)
{
	int Size;
	switch (Depth)
	{
	case DEPTH_8U:
		Size = sizeof(unsigned char);
		break;
	case DEPTH_8S:
		Size = sizeof(char);
		break;
	case DEPTH_16S:
		Size = sizeof(short);
		break;
	case DEPTH_32S:
		Size = sizeof(int);
		break;
	case DEPTH_32F:
		Size = sizeof(float);
		break;
	case DEPTH_64F:
		Size = sizeof(double);
		break;
	default:
		Size = 0;
		break;
	}
	return Size;
}

/// <summary>
/// 分配内存函数,以32字节对齐。
/// </summary>
/// <param name="Size">需要使用的内存大小（以字节为单位）。</param>
/// <param name="ZeroMemory">内存数据是否进行清零处理 。</param>
/// <returns>返回所分配内存的指针，失败则返回NULL。</returns>
void *AllocMemory(unsigned int Size, bool ZeroMemory)	//	https://technet.microsoft.com/zh-cn/library/8z34s9c6
{
	void *Ptr = _aligned_malloc(Size, 32);					//	考虑SSE,AVX等高级函数的需求，分配起始地址使用32字节对齐。其实_mm_malloc就是这个函数
	if (Ptr != NULL && ZeroMemory == true)
		memset(Ptr, 0, Size);
	return Ptr;
}

/// <summary>
/// 释放内存。
/// </summary>
/// <param name="Ptr">内存数据的地址。</param>
void FreeMemory(void *Ptr)
{
	if (Ptr != NULL) _aligned_free(Ptr);		//	_mm_free就是该函数
}

/// <summary>
/// 创建新的矩阵数据。
/// </summary>
/// <param name="Width">矩阵的宽度。</param>
/// <param name="Height">矩阵的高度。</param>
/// <param name="Depth">矩阵的元素类型。</param>
/// <param name="Channel">矩阵的通道数。</param>
/// <param name="RowAligned">每行数据量是否需要四字节对齐。</param>
/// <returns>返回创建的矩阵数据，失败则返回NULL。</returns>
TMatrix *CreateMatrix(int Width, int Height, int Depth, int Channel, int RowAligned)
{
	if (Width < 1 || Height < 1) return NULL;
	if (Depth < DEPTH_8U && Depth > DEPTH_64F) return NULL;

	TMatrix * Matrix = (TMatrix *)AllocMemory(sizeof(TMatrix), false);		//	不相信这个分配会不成功
	Matrix->Width = Width;
	Matrix->Height = Height;
	Matrix->Depth = Depth;
	Matrix->Channel = Channel;
	Matrix->RowAligned = RowAligned;
	if (RowAligned)
		Matrix->WidthStep = WidthBytes(Width * Channel * GetElementSize(Depth));
	else
		Matrix->WidthStep = Width * Channel * GetElementSize(Depth);
	Matrix->Data = (unsigned char *)AllocMemory(Matrix->Height * Matrix->WidthStep, false);		//	用了True在大循环里换慢很多
	if (Matrix->Data == NULL) 
	{
		FreeMemory(Matrix);
		return NULL;
	}
	return Matrix;
}

/// <summary>
/// 释放创建的矩阵。
/// </summary>
/// <param name="Matrix">需要释放的矩阵对象。</param>
/// <returns>返回值为0表示成功，其他表示失败。</returns>
void FreeMatrix(TMatrix *Matrix)
{
	if (Matrix == NULL) return;
	if (Matrix->Data == NULL) 
		FreeMemory(Matrix);
	else
	{
		FreeMemory(Matrix->Data);			//	注意释放的顺序
		FreeMemory(Matrix);
	}
}

/// <summary>
/// 克隆现有的矩阵。
/// </summary>
/// <param name="Src">被克隆的矩阵对象。</param>
/// <param name="Dest">克隆后的矩阵对象。</param>
TMatrix *Clone (TMatrix *Src)
{
	if (Src == NULL || Src->Data == NULL) return NULL;
	TMatrix *Dest = CreateMatrix(Src->Width, Src->Height, Src->Depth, Src->Channel, Src->RowAligned);
	if (Dest != NULL)	
	{
		memcpy(Dest->Data, Src->Data, Src->Height * Src->WidthStep);
		return Dest;
	}
	return NULL;	
}

