#include "StdAfx.h"

enum RETVAL
{
	RET_OK,
	RET_ERR_OUTOFMEMORY,
	RET_ERR_NULLREFERENCE,
	RET_ERR_ARGUMENTOUTOFRANGE,
	RET_ERR_PARAMISMATCH,
	RET_ERR_DIVIDEBYZERO,
	RET_ERR_NOTSUPPORTED,
	RET_ERR_UNKNOWN
};

enum DEPTH
{
	DEPTH_8U, 				//	unsigned char
	DEPTH_8S, 				//	char
	DEPTH_16S,				//	short
	DEPTH_32S,				//  int
	DEPTH_32F,				//	float
	DEPTH_64F				//	double
};

enum EdgeMode
{
	Tile = 0,				//	重复边缘像素
	Smear = 1				//	镜像数据
};

struct TMatrix
{
	int Width;				//	矩阵的宽度
	int Height;				//	矩阵的高度
	int Channel;			//	矩阵通道数
	int Depth;				//	矩阵元素的类型
	int WidthStep;			//	矩阵一行元素的占用的字节数
	int RowAligned;			//	是否使用四字节对齐
	unsigned char *Data;	//	矩阵的数据
};

struct Complex
{
	float Real;
	float Imag;
};
union Cv32suf
{
	int i;
	unsigned u;
	float f;
};

TMatrix *CreateMatrix(int Width, int Height, int Depth, int Channel, int RowAligned);		//	创建数据矩阵
TMatrix *CloneMatrix(TMatrix *Src);															//	克隆数据矩阵
void FreeMatrix(TMatrix *Matrix);															//	释放数据矩阵
void *AllocMemory(unsigned int Size, bool ZeroMemory = false);								//	分配内存，32字节对齐
void FreeMemory(void *Ptr);																	//	释放内存
int GetElementSize(int Depth);																//	获取元素大小