#include "StdAfx.h"
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include "DemoData.h"
#include "Image.h"

BYTE baData0[] =
{
	104,8,94,44,4,164,123,67,
	178,250,150,216,125,58,47,61,
	254,235,198,195,208,138,237,128,
	68,116,161,79,85,6,218,23,
	222,2,97,81,10,52,226,196,
	12,86,66,147,17,50,50,122,
	115,6,5,85,48,44,190,226,
	154,250,110,199,80,232,189,215
};

BYTE baData1[] =
{
	108,240,198,145,254,59,40,233,
	186,249,89,76,237,145,62,222,
	182,250,249,6,114,104,3,44,
	120,44,122,193,7,213,188,128,
	124,156,52,244,200,255,116,59,
	34,171,132,9,249,159,122,240,
	45,143,75,222,126,134,225,231,
	69,208,110,68,98,23,255,156
};

BYTE baData2[] =
{
	117,204,84,131,81,201,239,65,
	173,52,107,212,29,166,15,56,
	65,140,163,229,60,206,130,154,
	104,175,241,89,73,245,175,178,
	191,73,206,69,127,224,192,131,
	195,91,181,180,65,236,37,62,
	90,187,168,36,149,136,14,205,
	75,145,172,7,164,182,244,75
};

BYTE baData3[] =
{
	157,153,170,155,204,220,178,73,
	170,197,172,129,213,13,187,230,
	50,150,132,153,46,164,146,9,
	74,73,249,13,214,33,58,48,
	252,64,103,75,1,105,136,90,
	200,128,60,136,152,66,164,140,
	30,52,124,56,182,217,139,79,
	230,40,62,152,43,61,144,48
};


BYTE DataCnv::GetSeed1(int x, int y)
{
	int xx[4],yy[4];
	xx[0] = x&0x7; yy[0] = y&0x7;
	xx[1] = (x/8)&0x7; yy[1] = (y/8)&0x7;
	xx[2] = (x/64)&0x7; yy[2] = (y/64)&0x7;
	xx[3] = (x/512)&0x7; yy[3] = (y/512)&0x7;
	BYTE b0,b1,b2,b3;
	b0 = baData0[yy[0]*8+xx[0]];
	b1 = baData1[yy[1]*8+xx[1]];
	b2 = baData2[yy[2]*8+xx[2]];
	b3 = baData3[yy[3]*8+xx[3]];
	BYTE b = (b0^b1)+(b2^b3);
	return b;
}

BYTE DataCnv::GetSeed2(BYTE *pbData, int x, int y)
{
	int xx[4],yy[4];
	xx[0] = x&0x7; yy[0] = y&0x7;
	xx[1] = (x/8)&0x7; yy[1] = (y/8)&0x7;
	xx[2] = (x/64)&0x7; yy[2] = (y/64)&0x7;
	xx[3] = (x/512)&0x7; yy[3] = (y/512)&0x7;
	BYTE *pbData0 = pbData;
	BYTE *pbData1 = pbData+64;
	BYTE *pbData2 = pbData+128;
	BYTE *pbData3 = pbData+192;
	BYTE b0,b1,b2,b3;
	b0 = pbData0[yy[0]*8+xx[0]];
	b1 = pbData1[yy[1]*8+xx[1]];
	b2 = pbData2[yy[2]*8+xx[2]];
	b3 = pbData3[yy[3]*8+xx[3]];
	BYTE b = (b0^b1)+(b2^b3);
	return b;
}

BYTE DataCnv::GetSeed3(BYTE *pbData, int x, int y)
{
	BYTE b0 = GetSeed2(pbData,x,y);
	BYTE b1 = GetSeed1(x,y);
	return b0^b1;
}

void DataCnv::InitData(BYTE *pbData)
{
	srand((unsigned)time(NULL));
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			for (int k = 0; k < 8; k++)
			{
				BYTE ch = (BYTE)rand();
				pbData[i*64+j*8+k] = ch;
			}
		}
	}
}

void DataCnv::InitData4(BYTE *pbData)
{
	DataCnv::InitData(pbData);
	DataCnv::InitData(pbData+256);
	DataCnv::InitData(pbData+512);
	DataCnv::InitData(pbData+768);
}

template<class COLORREPR> void DoConvertImage(CImage<COLORREPR> &Dst, CImage<COLORREPR> &Src, BYTE *pbData)
{
	Dst.Create(Src.GetCX(), Src.GetCY(), Src.IsGrayScale()?CImageBase::GrayScale:CImageBase::TrueColor);
	for (int y = 0; y < Src.GetCY(); y++)
	{
		BYTE *pSrc = (BYTE *)Src[y];
		BYTE *pDst = (BYTE *)Dst[y];
		int nSize = Src.GetCols()*sizeof(COLORREPR);
		for (int x = 0; x < nSize; x++)
		{
			BYTE bSeed = DataCnv::GetSeed3(pbData,x,y);
			pDst[x] = pSrc[x]^bSeed;
		}
	}
}


CImageBase *DataCnv::ConvertImage(CImageBase *pimgSrc, BYTE *pbData)
{
	if (pimgSrc->Is16BitColor())
	{
		C16Image *pimgDst = new C16Image;
		DoConvertImage(*pimgDst,*(C16Image *)pimgSrc,pbData);
		return pimgDst;
	}
	else
	{
		C8Image *pimgDst = new C8Image;
		DoConvertImage(*pimgDst,*(C8Image *)pimgSrc,pbData);
		return pimgDst;
	}
}
