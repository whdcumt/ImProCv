#include "Utility.h"

/// <summary>
/// 实现图像方框模糊效果，O(1)复杂度，最新整理时间 2015.12.16
/// </summary>
/// <param name="Src">需要处理的源图像的数据结构。</param>
/// <param name="Dest">保存处理后的图像的数据结构。</param>
/// <param name="Radius">方框模糊的半径，有效范围[1,1000]。</param>
/// <param name="EdgeBehavior">边缘处数据的处理方法，0表示重复边缘像素，1使用镜像的方式对边缘像素求均值。</param>
/// <remarks> 1: 能处理8位灰度和24位图像。</remarks>
/// <remarks> 2: Src和Dest可以相同，在相同时速度会稍慢。</remarks>
/// <remarks> 3: 开发者laviewpbt ,QQ - 33184777。</remarks>
void __stdcall BoxBlur(TMatrix *Src, TMatrix *Dest, int Radius, EdgeMode Edge)
{
	int X, Y, Z, Width, Height, Channel, Index;  //X,Y,Z自变量,Width图像宽度，Height为图形高度，Channel图像的通道数
	int Value, ValueB, ValueG, ValueR;     //每个通道的灰度值
	int *RowPos, *ColPos, *ColSum, *Diff; 

	Width = Src->Width, Height = Src->Height, Channel = Src->Channel;
	int Size = 2 * Radius + 1,  Amount = Size * Size, HalfAmount = Amount / 2;

	RowPos = GetExpandPos(Width, Radius, Radius, Edge); //扩展行之后的数值
	ColPos = GetExpandPos(Height, Radius, Radius, Edge);//扩展列之后的数值
	ColSum = (int *)AllocMemory(Width * Channel * sizeof(int), true); //开辟行内存地址
	Diff   = (int *)AllocMemory((Width - 1) * Channel * sizeof(int), true);//开辟行内存地址-1
	unsigned char *RowData = (unsigned char *)AllocMemory((Width + 2 * Radius) * Channel, true);//开辟行内存地址
	TMatrix *Sum = CreateMatrix(Width, Height, DEPTH_32S, Channel, true);

	for (Y = 0; Y < Height; Y++)					//	水平方向的耗时比垂直方向上的大
	{
		unsigned char *LinePS = Src->Data + Y * Src->WidthStep;
		int *LinePD			  = (int *)(Sum->Data + Y * Sum->WidthStep);

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

		unsigned char *AddPos = RowData + Size * Channel;
		unsigned char *SubPos = RowData;
					
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
		int *LinePS = (int *)(Sum->Data + ColPos[Y] * Sum->WidthStep);
		for(X = 0; X < Width * Channel; X++)	ColSum[X] += LinePS[X];
	}

	for (Y = 0; Y < Height; Y++)
	{
		unsigned char* LinePD	= Dest->Data + Y * Dest->WidthStep;	
		int *AddPos				= (int*)(Sum->Data + ColPos[Y + Size - 1] * Sum->WidthStep);
		int *SubPos				= (int*)(Sum->Data + ColPos[Y] * Sum->WidthStep);

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
	FreeMatrix(Sum);
	FreeMemory(ColSum);
	FreeMemory(RowData);
}

void __stdcall BoxBlurSSE(TMatrix *Src, TMatrix *Dest, int Radius, EdgeMode Edge)
{

	int X, Y, Z, Width, Height, Channel, Size, Index;
	int Value, ValueB, ValueG, ValueR;
	int *RowPos, *ColPos, *ColSum, *Diff;

	Width = Src->Width, Height = Src->Height, Channel = Src->Channel, Size = 2 * Radius + 1;
	float Scale = 1.0 / (Size * Size);

	int Amount = Size * Size;

	RowPos = GetExpandPos(Width, Radius, Radius, Edge);
	ColPos = GetExpandPos(Height, Radius, Radius, Edge);
	ColSum = (int *)AllocMemory(Width * Channel * sizeof(int), true);
	Diff   = (int *)AllocMemory((Width - 1) * Channel * sizeof(int), true);
	unsigned char *RowData = (unsigned char *)AllocMemory((Width + 2 * Radius) * Channel, true);
	TMatrix *Sum = CreateMatrix(Width, Height, DEPTH_32S, Channel, true);

	for (Y = 0; Y < Height; Y++)					//	水平方向的耗时比垂直方向上的大
	{
		unsigned char *LinePS = Src->Data + Y * Src->WidthStep;
		int *LinePD			  = (int *)(Sum->Data + Y * Sum->WidthStep);

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

		unsigned char *AddPos = RowData + Size * Channel;
		unsigned char *SubPos = RowData;
		X = 0;					//	注意这个赋值在下面的循环外部，这可以避免当Width<8时第二个for循环循环变量未初始化			
		__m128i Zero = _mm_setzero_si128();
		for (; X <= (Width - 1) * Channel - 8; X += 8)
		{
			__m128i Add = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i const *)(AddPos + X)), Zero);		
			__m128i Sub = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i const *)(SubPos + X)), Zero);		
			_mm_store_si128((__m128i *)(Diff + X + 0), _mm_sub_epi32(_mm_unpacklo_epi16(Add, Zero), _mm_unpacklo_epi16(Sub, Zero)));		//	由于采用了_aligned_malloc函数分配内存，可是使用_mm_store_si128
			_mm_store_si128((__m128i *)(Diff + X + 4), _mm_sub_epi32(_mm_unpackhi_epi16(Add, Zero), _mm_unpackhi_epi16(Sub, Zero)));
		}
		for(; X < (Width - 1) * Channel; X++)
			Diff[X] = AddPos[X] - SubPos[X];

		//	第一个点要特殊处理
		if (Channel == 1)
		{
			for(Z = 0, Value = 0; Z < Size; Z++)	Value += RowData[Z];
			LinePD[0] = Value;

			for(X = 1; X < Width; X++)
			{
				Value += Diff[X - 1];
				LinePD[X] = Value;
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

			for(X = 1; X < Width; X++)
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
		X = 0;
		int *LinePS = (int *)(Sum->Data + ColPos[Y] * Sum->WidthStep);
		for( ; X <= Width * Channel - 4; X += 4)
		{
			__m128i SumP = _mm_load_si128((const __m128i*)(ColSum + X));
			__m128i SrcP = _mm_loadu_si128((const __m128i*)(LinePS + X));
			_mm_store_si128((__m128i *)(ColSum + X), _mm_add_epi32(SumP, SrcP));
		}

		for( ; X < Width * Channel; X++)	ColSum[X] += LinePS[X];
	}

	for (Y = 0; Y < Height; Y++)
	{
		unsigned char* LinePD	= Dest->Data + Y * Dest->WidthStep;	
		int *AddPos				= (int*)(Sum->Data + ColPos[Y + Size - 1] * Sum->WidthStep);
		int *SubPos				= (int*)(Sum->Data + ColPos[Y] * Sum->WidthStep);
			
		X = 0;
		const __m128 Inv = _mm_set1_ps(Scale);
		for( ; X <= Width * Channel - 8; X += 8 )
		{
			__m128i Sub1 = _mm_loadu_si128((const __m128i*)(SubPos + X + 0));
			__m128i Sub2 = _mm_loadu_si128((const __m128i*)(SubPos + X + 4));

			__m128i Add1 = _mm_loadu_si128((const __m128i*)(AddPos + X + 0));
			__m128i Add2 = _mm_loadu_si128((const __m128i*)(AddPos + X + 4));
			__m128i Col1 = _mm_load_si128((const __m128i*)(ColSum + X + 0));
			__m128i Col2 = _mm_load_si128((const __m128i*)(ColSum + X + 4));

			__m128i Sum1 = _mm_add_epi32(Col1, Add1);
			__m128i Sum2 = _mm_add_epi32(Col2, Add2);

			__m128i Dest1 = _mm_cvtps_epi32(_mm_mul_ps(Inv, _mm_cvtepi32_ps(Sum1)));
			__m128i Dest2 = _mm_cvtps_epi32(_mm_mul_ps(Inv, _mm_cvtepi32_ps(Sum2)));

			Dest1 = _mm_packs_epi32(Dest1, Dest2);
			_mm_storel_epi64((__m128i *)(LinePD + X), _mm_packus_epi16(Dest1, Dest1));

			_mm_store_si128((__m128i *)(ColSum + X + 0), _mm_sub_epi32(Sum1, Sub1));
			_mm_store_si128((__m128i *)(ColSum + X + 4), _mm_sub_epi32(Sum2, Sub2));
		}
		for( ; X < Width * Channel; X++)
		{
			Value = ColSum[X] + AddPos[X];
			LinePD[X] = Value * Scale;
			ColSum[X] = Value - SubPos[X];
		}
	}

	FreeMemory(RowPos);
	FreeMemory(ColPos);
	FreeMemory(Diff);
	FreeMatrix(Sum);
	FreeMemory(ColSum);
	FreeMemory(RowData);

}


