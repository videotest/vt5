#include "stdafx.h"
#include "actions.h"
#include "nameconsts.h"
#include "image5.h"
#include "data5.h"
#include "misc_utils.h"
#include <stdio.h>
#include "alloc.h"
#include "\vt5\awin\misc_ptr.h"
#include <math.h>
#include <stdexcept>

#include "MorphInfo.h"
#include "DivFunc.h"
#ifdef _DEBUG
#include "tchar.h"
#include "\vt5\awin\profiler.h"
#include "actions.h"
#endif

_ainfo_base::arg	CBinCleanV4Info::s_pargs[] = 
{
	{"BinImage",szArgumentTypeBinaryImage, 0, true, true },
	{"CleanV4",szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

_ainfo_base::arg	CBinMedianInfo::s_pargs[] = 
{
	{"MaskSize",szArgumentTypeInt, "3", true, false },
	{"BinImage",szArgumentTypeBinaryImage, 0, true, true },
	{"BinMedian",szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

_ainfo_base::arg	CBinErodeInfo::s_pargs[] = 
{
	{"MaskSize",szArgumentTypeInt, "3", true, false },
	{"BinImage",szArgumentTypeBinaryImage, 0, true, true },
	{"BinErode",szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

_ainfo_base::arg	CBinDilateInfo::s_pargs[] = 
{
	{"MaskSize",szArgumentTypeInt, "3", true, false },
	{"BinImage",szArgumentTypeBinaryImage, 0, true, true },
	{"BinDilate",szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

_ainfo_base::arg	CBinOpenInfo::s_pargs[] = 
{
	{"MaskSize",szArgumentTypeInt, "3", true, false },
	{"BinImage",szArgumentTypeBinaryImage, 0, true, true },
	{"BinErode",szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

_ainfo_base::arg	CBinCloseInfo::s_pargs[] = 
{
	{"MaskSize",szArgumentTypeInt, "3", true, false },
	{"BinImage",szArgumentTypeBinaryImage, 0, true, true },
	{"BinErode",szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

_ainfo_base::arg	CBinNotInfo::s_pargs[] = 
{
	{"BinSource1",szArgumentTypeBinaryImage, 0, true, true },
	{"BinNot",szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

_ainfo_base::arg	CBinOrInfo::s_pargs[] = 
{
	{"BinSource1",szArgumentTypeBinaryImage, 0, true, true },
	{"BinSource2",szArgumentTypeBinaryImage, 0, true, true },
	{"BinOr",szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

_ainfo_base::arg	CBinAndInfo::s_pargs[] = 
{
	{"BinSource1",szArgumentTypeBinaryImage, 0, true, true },
	{"BinSource2",szArgumentTypeBinaryImage, 0, true, true },
	{"BinAnd",szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

_ainfo_base::arg	CBinXorInfo::s_pargs[] = 
{
	{"BinSource1",szArgumentTypeBinaryImage, 0, true, true },
	{"BinSource2",szArgumentTypeBinaryImage, 0, true, true },
	{"BinXor",szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

_ainfo_base::arg	CBinCustomLogicInfo::s_pargs[] = 
{
	{"Combinations",szArgumentTypeInt, "11", true, false }, // битовая маска
	{"BinSource1",szArgumentTypeBinaryImage, 0, true, true },
	{"BinSource2",szArgumentTypeBinaryImage, 0, true, true },
	{"CustomLogic",szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

_ainfo_base::arg	CBinReconstructForeInfo::s_pargs[] = 
{
	{"BinSource",szArgumentTypeBinaryImage, 0, true, true },
	{"BinSeed",szArgumentTypeBinaryImage, 0, true, true },
	{"BinReconstructed",szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

_ainfo_base::arg	CBinReconstructBackInfo::s_pargs[] = 
{
	{"BinSource",szArgumentTypeBinaryImage, 0, true, true },
	{"BinSeed",szArgumentTypeBinaryImage, 0, true, true },
	{"BinReconstructed",szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

_ainfo_base::arg	CBinDeleteSmallObjectsInfo::s_pargs[] = 
{
	{"MinSize",szArgumentTypeInt, "5", true, false }, // минимальный линейный размер области, которую не трогаем
	{"MinSize2",szArgumentTypeInt, "-1", true, false }, // второй размер - для фильтрации по форме
	{"FormThreshold",szArgumentTypeDouble, "0.3", true, false }, // граница для фильтрации по форме
	{"FillValue",szArgumentTypeInt, "0", true, false },
	{"BinSource",szArgumentTypeBinaryImage, 0, true, true },
	{"BinResult",szArgumentTypeBinaryImage, 0, false, true },
	{"Flags",szArgumentTypeInt, "0", true, false }, // битовые флаги: бит0 - удалять большие (0-мелочь), бит1 - удалять длинные (0 - круглые)
	{0, 0, 0, false, false },
};

_ainfo_base::arg	CBinMoveInfo::s_pargs[] = 
{
	{"dX",szArgumentTypeInt, "0", true, false },
	{"dY",szArgumentTypeInt, "0", true, false },
	{0, 0, 0, false, false },
};

// MorphDivision
_ainfo_base::arg	CBinMorphDivisionInfo::s_pargs[] = 
{
	{"BinImage", szArgumentTypeBinaryImage, 0, true, true },
	{"BinMorphDivision", szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

// MorphDivision2
_ainfo_base::arg	CBinMorphDivision2Info::s_pargs[] = 
{
	{"BinImage", szArgumentTypeBinaryImage, 0, true, true },
	{"BinMorphDivision2", szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

// SmoothContour
_ainfo_base::arg	CSmoothContourInfo::s_pargs[] = 
{	{"MaskSize",szArgumentTypeInt, "3", true, false },
	{"BinImage", szArgumentTypeBinaryImage, 0, true, true },
	{"SmoothContour", szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

static double sqr(double x)
{ return x*x; }

////////////////////////////////////////////////////////////////////////////////////////////////////
//CBinCleanV4
////////////////////////////////////////////////////////////////////////////////////////////////////
CBinCleanV4::CBinCleanV4()
{
}

bool CBinCleanV4::InvokeFilter()
{
	IBinaryImagePtr pSrc = GetDataArgument();
	IBinaryImagePtr pDst = GetDataResult();

	if( pSrc==NULL || pDst==NULL) return false;

	_point offset;
	pSrc->GetOffset(&offset);
	int cx,cy;
	pSrc->GetSizes(&cx,&cy);

	if(pDst->CreateNew(cx, cy)!=S_OK) return false;
	pDst->SetOffset( offset, TRUE);

	StartNotification(cy);

	smart_alloc(src, byte*, cy);
	smart_alloc(dst, byte*, cy);
	if(dst==0 || src==0) return false;

	for(int y=0; y<cy; y++)
	{
		pSrc->GetRow(&src[y], y, false);
		pDst->GetRow(&dst[y], y, false);
		memcpy(dst[y], src[y], cx);
	}

	BYTE	*p1, *p2, *p3;
	int		c, a, g, v;   

	//clean first and last cols and rows
	for( int x = 0; x<cx; x++ )
	{
		if(src[0][x] && !src[1][x] )
			dst[0][x] = 0;
		if(src[cy-1][x] && !src[cy-2][x] )
			dst[cy-1][x] = 0;
	}

	for( int y = 0; y<cy; y++ )
	{
		if(src[y][0] && !src[y][1] )
			dst[y][0] = 0;
		if(src[y][cx-1] && !src[y][cx-2] )
			dst[y][cx-1] = 0;
	}

	if( cx>=3 && cy>=3 )
	{ //clean other
		for( y = 1; y < cy-1; y++ )
		{
			p1 = dst[y-1];
			p2 = dst[y];
			p3 = dst[y+1];

			do
			{
				c = 0;
				for( x = 1; x < cx-1; x++ )
				{
					a = p2[x];
					v = (int)p1[x]   + (int)p3[x];
					g = (int)p2[x-1] + (int)p2[x+1];

					if(( v + g ) != 510 )
					{
						if (( v + g ) > 510 )
							p2[x]=0xff;
						else
							p2[x]=0;
					}
					else
					{
						if (!v||!g)
							p2[x]=0;
					}

					if( a != p2[x] )
						c=1;
				}
			}while( c );
		}

		for( y = cy-2; y > 1; y--)
		{
			p1 = dst[y-1];
			p2 = dst[y];
			p3 = dst[y+1];

			do
			{
				c=0;
				for(x = 1; x < cx-1; x++)
				{
					a = p2[x];
					v = (p1[x] ? 1 : 0)   + (p3[x] ? 1 : 0);
					g = (p2[x-1] ? 1 : 0) + (p2[x+1] ? 1 : 0);

					if(( v + g ) != 2 )
					{
						if (( v + g ) > 2 )
							p2[x]=0xff;
						else
							p2[x]=0;
					}
					else
					{
						if (!v||!g)
							p2[x]=0;
					}

					if( a != p2[x] )
						c=1;
				}
			}while( c );
		}
	}


	FinishNotification();

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//CBinMedian
////////////////////////////////////////////////////////////////////////////////////////////////////
CBinMedian::CBinMedian()
{
}

static void median3x3(byte **src, byte **dst, int cx, int cy)
{
	//медиана 3x3. Допустимо совпадение src и dst.
	//медиана 3x3 - чтобы шеф порадовался за ровные края
	int wx=cx+2, wy=cy+2;
	smart_alloc(buf,byte,wx*wy); smart_buf.zero(); //на 1 пиксел шире
	for(int y=0; y<cy; y++)
	{
		byte *p=src[y];
		byte *m=buf+(y+1)*wx+1;
		for(int x=0; x<cx; x++)
		{
			if(*p)
			{
				m[-wx-1]++; m[-wx]++; m[-wx+1]++;
				m[-1]++; m[0]++; m[1]++;
				m[wx-1]++; m[wx]++; m[wx+1]++;
			}
			p++; m++;
		}
	}
	for(int y=0; y<cy; y++)
	{
		byte *p=dst[y];
		byte *m=buf+(y+1)*wx+1;
		for(int x=0; x<cx; x++)
		{
			*p = (*m>=5) ? 255 : 0;
			p++; m++;
		}
	}
}

/*
static void median5(byte **src, byte **dst, int cx, int cy)
{
	//медиана по 5-пиксельному шаблону (крест). Допустимо совпадение src и dst.
	//чтобы шеф порадовался за отсутствие однопиксельной фигни :-)
	int wx=cx+2, wy=cy+2;
	smart_alloc(buf,byte,wx*wy); smart_buf.zero(); //на 1 пиксел шире
	for(int y=0; y<cy; y++)
	{
		byte *p=src[y];
		byte *m=buf+(y+1)*wx+1;
		for(int x=0; x<cx; x++)
		{
			if(*p)
			{
				m[-wx]++;
				m[-1]++; m[0]++; m[1]++;
				m[wx]++;
			}
			p++; m++;
		}
	}
	for(y=0; y<cy; y++)
	{
		byte *p=dst[y];
		byte *m=buf+(y+1)*wx+1;
		for(int x=0; x<cx; x++)
		{
			*p = (*m>=3) ? 255 : 0;
			p++; m++;
		}
	}
}
*/

static void median3x3_fi(byte **src, byte **dst, int cx, int cy)
{
	//обновляем изображение по ходу дела. Нет изотропности,
	//зато две соседних однопиксельных фигни не дадут одну.
	if(dst!=src)
	{
		for(int y=0; y<cy; y++) CopyMemory(dst[y], src[y], cx);
	}
	for(int y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			register int m=0;
			if(y>0)
			{
				if(x>0 && dst[y-1][x-1]) m++;
				if(dst[y-1][x]) m++;
				if(x<cx-1 && dst[y-1][x+1]) m++;
			}
			{
				if(x>0 && dst[y][x-1]) m++;
				if(dst[y][x]) m++;
				if(x<cx-1 && dst[y][x+1]) m++;
			}
			if(y<cy-1)
			{
				if(x>0 && dst[y+1][x-1]) m++;
				if(dst[y+1][x]) m++;
				if(x<cx-1 && dst[y+1][x+1]) m++;
			}
			dst[y][x] = (m>=5) ? 255 : 0;
		}
	}
}

static void median5(byte **src, byte **dst, int cx, int cy)
{
	//медиана по 5-пиксельному шаблону (крест). Допустимо совпадение src и dst.
	//чтобы шеф порадовался за отсутствие однопиксельной фигни :-)
	//обновляем изображение по ходу дела. Нет изотропности,
	//зато две соседних однопиксельных фигни не дадут одну.
	if(dst!=src)
	{
		for(int y=0; y<cy; y++) CopyMemory(dst[y], src[y], cx);
	}
	for(int y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			register int m=0;
			if(dst[y][x]) m++;
			if(x>0 && dst[y][x-1]) m++;
			if(x<cx-1 && dst[y][x+1]) m++;
			if(y>0 && dst[y-1][x]) m++;
			if(y<cy-1 && dst[y+1][x]) m++;
			dst[y][x] = (m>=3) ? 255 : 0;
		}
	}
}

static void median(byte **src, byte **dst, int cx, int cy, int m)
{
	//медиана m*m. Допустимо совпадение src и dst.

	if(m<=1)
	{
		for(int y=0; y<cy; y++) CopyMemory(dst[y], src[y], cx);
		return;
	}

	if(m==2)
	{
		median5(src, dst, cx, cy);
		return;
	}

	if(m==3)
	{
		median3x3_fi(src, dst, cx, cy);
		return;
	}

	if(m==4)
	{
		median3x3_fi(src, dst, cx, cy);
		median5(dst, dst, cx, cy);
		return;
	}

	bool bIsEven=(m&1)==0; //для четных - делаем на 1 меньше, а потом по кресту 5pix
	if(bIsEven) m--;

	int wx=cx+1, wy=cy+1;
	smart_alloc(buf,int,wx*wy); smart_buf.zero(); //можно обойтись гораздо меньшим буфером

	for(int x=0; x<wx; x++) buf[x]=0;
	for(int y=0; y<cy; y++)
	{
		int *m=buf+(y+1)*wx;
		*m=0; m++;
		byte *p=src[y];
		for(int x=0; x<cx; x++)
		{
			*m=m[-wx]+m[-1]-m[-wx-1];
			if(*p) (*m)++;
			p++; m++;
		}
	}

	int mm=m*m, m1=(m-1)/2, m2=m-m1, mm2=mm/2;
	//m1 точек налево или вверх от текущей, m2 - направо или вниз
	smart_alloc(x0tbl, int, cx);
	smart_alloc(x1tbl, int, cx);
	smart_alloc(y0tbl, int, cy);
	smart_alloc(y1tbl, int, cy);

	for(int x=0; x<cx; x++) x0tbl[x]=max(x-m1,0);
	for(int x=0; x<cx; x++) x1tbl[x]=min(x+m2,cx-1);
	for(int y=0; y<cy; y++) y0tbl[y]=max(y-m1,0);
	for(int y=0; y<cy; y++) y1tbl[y]=min(y+m2,cy-1);

	for(int y=0; y<cy; y++)
	{
		int y0=y0tbl[y], y1=y1tbl[y];
		int *c0=buf+y0*wx, *c1=buf+y1*wx;
		for(int x=0; x<cx; x++)
		{
			int x0=x0tbl[x], x1=x1tbl[x];
			int s=c1[x1]+c0[x0]-c1[x0]-c0[x1];
			dst[y][x] = (s>mm2)?(255):(0);
		}
	}

	if(bIsEven) median3x3_fi(dst, dst, cx, cy);
}

bool CBinMedian::InvokeFilter()
{
	IBinaryImagePtr pSrc = GetDataArgument();
	IBinaryImagePtr pDst = GetDataResult();

	int nMaskSize = GetArgLong( "MaskSize" );	
	nMaskSize = max(2, min(15, nMaskSize));

	if( pSrc==NULL || pDst==NULL) return false;

	_point offset;
	pSrc->GetOffset(&offset);
	int cx,cy;
	pSrc->GetSizes(&cx,&cy);

	if(pDst->CreateNew(cx, cy)!=S_OK) return false;
	pDst->SetOffset( offset, TRUE);

	StartNotification(cy);
	smart_alloc(src, byte*, cy);
	smart_alloc(dst, byte*, cy);
	if(dst==0 || src==0) return false;

	for(int y=0; y<cy; y++)
	{
		pSrc->GetRow(&src[y], y, false);
		pDst->GetRow(&dst[y], y, false);
	}
	FinishNotification();

	//median3(src, dst, cx, cy);
	median(src, dst, cx, cy,nMaskSize);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//CBinErode
////////////////////////////////////////////////////////////////////////////////////////////////////
CBinErode::CBinErode()
{
}

//bool CBinErode::InvokeFilter()
//{
//#ifdef _DEBUG
//	UINT maskSize = GetArgLong( "MaskSize" );
//	char buffer[14];
//	sprintf(buffer, "mask = %d\n", maskSize);
//	OutputDebugStr(buffer);
//	__time_test__ t("CBinErode::InvokeFilter -");
//#endif
//
//	if(!_InitMorpho())
//		return false;
//
//	StartNotification(GetHeight());
//	_DoMorphology(true);
//
//	_DeInitMorpho();
//
//	return true;
//}
//

using namespace _kir;
bool CBinErode::InvokeFilter()
{
	UINT maskSize = GetArgLong( "MaskSize" );
#ifdef _DEBUG
	char buffer[14];
	sprintf(buffer, "mask = %d\n", maskSize);
	//OutputDebugStr(buffer);
	__time_test__ t("CBinErode::InvokeFilter -");
#endif


	IBinaryImagePtr	ptrDest = GetDataResult();
	IBinaryImagePtr	ptrSrc = GetDataArgument();
	int cx,cy;
	ptrSrc->GetSizes(&cx, &cy);
	if(FAILED(ptrDest->CreateNew(cx, cy))) return false;
	



	CRoundMask mask;

	mask.Init(ptrSrc, ptrDest, maskSize);



	int pos = 0;
	
	StartNotification(cy,1,100);

	do{
		if(mask.Stamp())
		{
			pos++;
			Notify(pos);	
		}
	
	}while(mask.Shift());

	POINT p;
	ptrSrc->GetOffset(&p);
	ptrDest->SetOffset(p,0);

	
	FinishNotification();
	return true;
}



CRoundMask::CRoundMask()
{
	cx =0,cy=0;		
	maskSize =0;
	byte** rows =0;
	x =0,y=0;
	source = NULL,dest = NULL;
	notify = false;
}
bool CRoundMask::Init(IBinaryImage* source, IBinaryImage* dest, UINT mask)
{
	
	this->source = source;
	this->dest = dest;
	source->GetSizes(&cx,&cy);
	this->maskSize = mask/2;
	if(this->maskSize==0) this->maskSize =1;// kir - mask with size 1 must work as one with size 3 (previous version feature)
	if(maskSize>cx || maskSize>cy) return false;
	int rowRange = 2*maskSize +1;
	rows = new byte* [rowRange];
	for(int i=0;i<=maskSize;i++)//init half + 1 rows because mask have symmetry
	{
		rows[i] = new byte[rowRange];
	}
	int k =2;
	for(i, k;i<rowRange; i++,k+=2)//init others
	{
		rows[i] = rows[i-k];
	}

	float dist,fx,fy;
	for(int j=0;j<maskSize+1;j++)//init mask
	{
		byte* tRow1;
		tRow1 = rows[maskSize-j];

		tRow1+=maskSize;

		for(int i=0; i<=maskSize; i++)
		{
			fx = i, fy = j;
			dist = sqrt(fx*fx+fy*fy)-.1;
			tRow1[i]=tRow1[-i]= (dist<maskSize)? -1:0;
			
		}		
	}

	byte* t;

	for(k=0;k<rowRange;k++)
	{
	
		dest->GetRow(&t,k,false);
		destRows.Add(t);

		source->GetRow(&t,k,false);
		srcRows.Add(t);

		
		
	}
	destRows.Encycle();
	srcRows.Encycle();
	
	return true;

}
byte CRoundMask::CheckPoint(int i,int j)
{
	int x =i,y = j;


	return 0xff;
}
bool CRoundMask::Shift()
{

	x++;
	if(x>=cx) //row ended
	{
		notify = true;
		x=0;
		y++;
		if(y>=cy) 
			return false; //image ended
		if(y<=maskSize) return true;
		destRows.SetCurrent();
		srcRows.SetCurrent();		

		int k=y+maskSize; //adding next row to rowset in exchange with current head
		//renew current rowset
		if(k>=cy) //reached last row
		{

			destRows.SetData(0);
			srcRows.SetData(0);
		}
		else
		{
			byte *b;
			source->GetRow(&b,k,false);
			srcRows.SetData(b);
			dest->GetRow(&b,k,false);
			destRows.SetData(b);
		}
		destRows.ShiftHead();
		srcRows.ShiftHead();


	}

	return true;
}
bool CRoundMask::Stamp(byte mark)
{
	int cur = min(y,maskSize);
	srcRows.SetCurrent(cur);
	bool yChanged = false;
	if(notify) {notify = false; yChanged = true;}
	
	
	byte* b = srcRows.GetData();
	if(!b[x]) //empty byte - skipping
		return yChanged;
	
	srcRows.SetCurrent();
	destRows.SetCurrent(cur);
	

	int left = max(x-maskSize,0),right = min(x + maskSize+1,cx),top = max(y - maskSize,0), bottom = min(y+maskSize+1,cy);
	bool needClear =false;
	int dx = max(maskSize-x,0),dy = max(maskSize-y,0);
	for(int j = top; j< bottom ;j++)
	{
		for(int i =left ; i<right ; i++)
		{
			byte b;
		
			b = rows[j-top+dy][i-left+dx];
			if(b)
			{
				if(srcRows.GetData()[i]==0) {needClear=true; break;}
			}
		}
	if(needClear) break;
	srcRows.Shift();
	}
	if(!needClear) 
	{
		byte* row = destRows.GetData();
		row[x] = 0xff;
	}
	return yChanged;
}

CRoundMask::~CRoundMask()
{
	

	for(int i=0;i<=maskSize;i++)
	{
		delete[] rows[i];
	}
	delete[] rows;

}



////////////////////////////////////////////////////////////////////////////////////////////////////
//CBinDilate
////////////////////////////////////////////////////////////////////////////////////////////////////
CBinDilate::CBinDilate()
{
}

bool CBinDilate::InvokeFilter()
{
	if(!_InitMorpho())
		return false;

	StartNotification(GetHeight());
	_DoMorphology(false);

	_DeInitMorpho();

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//CBinOpen
////////////////////////////////////////////////////////////////////////////////////////////////////
CBinOpen::CBinOpen()
{
}

bool CBinOpen::InvokeFilter()
{
	if(!_InitMorpho2())
		return false;

	StartNotification(GetHeight(), 2);
	_DoMorphology(true,m_pbinIntr,m_pbinSrc);
	NextNotificationStage();
	m_pbinIntr->ClearBorders();
	_DoMorphology(false,m_pbinDst,m_pbinIntr);

	_DeInitMorpho();

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//CBinClose
////////////////////////////////////////////////////////////////////////////////////////////////////
CBinClose::CBinClose()
{
}

bool CBinClose::InvokeFilter()
{
	if(!_InitMorpho2())
		return false;

	StartNotification(GetHeight(), 2);
	_DoMorphology(false,m_pbinIntr,m_pbinSrc);
	NextNotificationStage();
	_DoMorphology(true,m_pbinDst,m_pbinIntr);
	
	_DeInitMorpho();

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//CBinCustomLogic
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CBinCustomLogic::InvokeFilter()
{
	IBinaryImagePtr	binDst = GetDataResult();
	IBinaryImagePtr	binSrc1 = GetDataArgument("BinSource1");
	IBinaryImagePtr	binSrc2 = GetDataArgument("BinSource2");
	if(binDst == 0 || binSrc1 == 0 || binSrc2 == 0)
		return false;

	int nCombinations = GetArgLong( "Combinations" );

	int cx, cy, cx1, cy1, cx2, cy2;
	binSrc1->GetSizes(&cx1, &cy1);
	binSrc2->GetSizes(&cx2, &cy2);
	if(cx2!=cx1 || cy2!=cy1) return false; // не будем работать с разным размером
	cx = min(cx1, cx2);
	cy = min(cy1, cy2);
	if( binDst->CreateNew(cx, cy) != S_OK )
		return false;

	StartNotification(cy);

	byte lut[4];
	lut[0] = (nCombinations & 1) ? 255 : 0;
	lut[1] = (nCombinations & 2) ? 255 : 0;
	lut[2] = (nCombinations & 4) ? 255 : 0;
	lut[3] = (nCombinations & 8) ? 255 : 0;

	for(int y=0; y<cy; y++)
	{
		byte *dst,*src1,*src2;
		binDst->GetRow(&dst, y, FALSE);
		binSrc1->GetRow(&src1, y, FALSE);
		binSrc2->GetRow(&src2, y, FALSE);
		for(long x = 0; x<cx; x++)
		{
			int h = (src1[x]>=128 ? 1 : 0) | (src2[x]>=128 ? 2 : 0);
			dst[x] = lut[h];
		}
		Notify(y);
	}
	FinishNotification();

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//CBinReconstructFore
////////////////////////////////////////////////////////////////////////////////////////////////////
#define ADD_XY { dst[y][x]=255; seedpos[nSeed++] = (y<<16) + x; }
#define CHECK_XY  {if(dst[y][x]==1) ADD_XY;}

bool CBinReconstructFore::InvokeFilter()
{
	IBinaryImagePtr	binDst = GetDataResult();
	IBinaryImagePtr	binSrc = GetDataArgument("BinSource");
	IBinaryImagePtr	binSeed = GetDataArgument("BinSeed");
	if(binDst == 0 || binSrc == 0 || binSeed == 0)
		return false;

	int cx, cy, cx1, cy1, cx2, cy2;
	binSrc->GetSizes(&cx1, &cy1);
	binSeed->GetSizes(&cx2, &cy2);
	cx = min(cx1, cx2);
	cy = min(cy1, cy2);
	if( binDst->CreateNew(cx, cy) != S_OK )
		return false;

	smart_alloc(src, byte*, cy);
	smart_alloc(seed, byte*, cy);
	smart_alloc(dst, byte*, cy);

	StartNotification(cy,2);

	for(int y=0; y<cy; y++)
	{
		binDst->GetRow(dst+y, y, FALSE);
		binSrc->GetRow(src+y, y, FALSE);
		binSeed->GetRow(seed+y, y, FALSE);
	}

	smart_alloc(seedpos,int,cx*cy);
	int nSeed=0;

	for(long y = 0; y < cy; y++)
	{
		for(long x = 0; x<cx; x++)
		{
			dst[y][x] = src[y][x] ? 1 : 0;
			if(seed[y][x]) ADD_XY;
		}
		Notify(y);
	}
	NextNotificationStage();

	int nSeed1=0;
	while(nSeed1<nSeed)
	{
        int y = seedpos[nSeed1] >> 16;
        int x = seedpos[nSeed1] & 65535;
		y--; if(y>=0) CHECK_XY; y++;
		y++; if(y<cy) CHECK_XY; y--;
		x--; if(x>=0) CHECK_XY; x++;
		x++; if(x<cx) CHECK_XY; x--;
		nSeed1++;
	}

	for(long y = 0; y < cy; y++)
	{
		for(long x = 0; x<cx; x++)
		{
			if(dst[y][x]==1) dst[y][x]=0;
		}
		Notify(y);
	}

	FinishNotification();

	return true;
}
#undef ADD_XY
#undef CHECK_XY

////////////////////////////////////////////////////////////////////////////////////////////////////
//CBinReconstructBack
////////////////////////////////////////////////////////////////////////////////////////////////////
#define ADD_XY { dst[y][x]=0; seedpos[nSeed++] = (y<<16) + x; }
#define CHECK_XY  {if(dst[y][x]==1) ADD_XY;}

bool CBinReconstructBack::InvokeFilter()
{
	IBinaryImagePtr	binDst = GetDataResult();
	IBinaryImagePtr	binSrc = GetDataArgument("BinSource");
	IBinaryImagePtr	binSeed = GetDataArgument("BinSeed");
	if(binDst == 0 || binSrc == 0 || binSeed == 0)
		return false;

	int cx, cy, cx1, cy1, cx2, cy2;
	binSrc->GetSizes(&cx1, &cy1);
	binSeed->GetSizes(&cx2, &cy2);
	cx = min(cx1, cx2);
	cy = min(cy1, cy2);
	if( binDst->CreateNew(cx, cy) != S_OK )
		return false;

	smart_alloc(src, byte*, cy);
	smart_alloc(seed, byte*, cy);
	smart_alloc(dst, byte*, cy);

	StartNotification(cy,2);

	for(int y=0; y<cy; y++)
	{
		binDst->GetRow(dst+y, y, FALSE);
		binSrc->GetRow(src+y, y, FALSE);
		binSeed->GetRow(seed+y, y, FALSE);
	}

	smart_alloc(seedpos,int,cx*cy);
	int nSeed=0;

	for(long y = 0; y < cy; y++)
	{
		for(long x = 0; x<cx; x++)
		{
			dst[y][x] = src[y][x] ? 255 : 1;
			if(!seed[y][x]) ADD_XY;
		}
		Notify(y);
	}
	NextNotificationStage();

	int nSeed1=0;
	while(nSeed1<nSeed)
	{
        int y = seedpos[nSeed1] >> 16;
        int x = seedpos[nSeed1] & 65535;
		y--; if(y>=0) CHECK_XY; y++;
		y++; if(y<cy) CHECK_XY; y--;
		x--; if(x>=0) CHECK_XY; x++;
		x++; if(x<cx) CHECK_XY; x--;
		nSeed1++;
	}

	for(long y = 0; y < cy; y++)
	{
		for(long x = 0; x<cx; x++)
		{
			if(dst[y][x]==1) dst[y][x]=255;
		}
		Notify(y);
	}

	FinishNotification();

	return true;
}
#undef ADD_XY
#undef CHECK_XY

////////////////////////////////////////////////////////////////////////////////////////////////////
//CBinDeleteSmallObjects
////////////////////////////////////////////////////////////////////////////////////////////////////
void CBinDeleteSmallObjects::FillSmallSegments(byte **buf, int cx, int cy, byte fill_color)
{
	if(cx<3 || cy<3) return; //на такой фигне делать нечего...

	//smart_alloc(ind, byte, 256);
	_ptr_t<int> smart_ind(256); // _ptr_t - из awin, он умеет реаллокейтить
	int *ind = smart_ind.ptr();

	int cur_ind=0;
	for(int i=cur_ind; i<smart_ind.size(); i++) ind[i]=i;
	//индексы для объединения нескольких областей в одну
	
	smart_alloc(cnt_buf, int, cx*cy);
	smart_cnt_buf.zero();
	
	smart_alloc(cnt, int*, cy);
	for(int y=0; y<cy; y++) cnt[y]=cnt_buf+y*cx;
	
	cnt[0][0]=cur_ind=0;
	for(int x=1; x<cx; x++)
	{
		if(buf[0][x]!=buf[0][x-1])
		{
			cur_ind++;
			if(cur_ind>=smart_ind.size())
			{
				smart_ind.alloc(smart_ind.size()*2);
				ind = smart_ind.ptr();
				for(int i=cur_ind; i<smart_ind.size(); i++) ind[i]=i;
			}
		}
		cnt[0][x]=cur_ind;
	} //проиндексировали первую строку
	
	for(int y=1; y<cy; y++)
	{
		for(int x=0; x<cx; )
		{ //идем слева направо по строке
			int i=-1;
			int xx=x;
			for(; x<cx && buf[y][x]==buf[y][xx]; x++)
			{	//если значения совпадают - продолжим текущую область
				//(если уже выбрана область правее - то ее)
				if(buf[y][x]==buf[y-1][x] && cnt[y-1][x]>i) i=cnt[y-1][x];
			}				
			if(i<0)
			{ //попали в новую область
				cur_ind++;
				if(cur_ind>=smart_ind.size())
				{
					smart_ind.alloc(smart_ind.size()*2);
					ind = smart_ind.ptr();
					for(int i=cur_ind; i<smart_ind.size(); i++) ind[i]=i;
				}
				i=cur_ind;
			}
			for(; xx<x; xx++) cnt[y][xx]=i;
		}
	}
	
	for(int y=1; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			if(cnt[y-1][x]<cnt[y][x] && buf[y-1][x]==buf[y][x])
				ind[cnt[y-1][x]]=ind[cnt[y][x]];
		}
	}
	
	for(int i=cur_ind; i>=0; i--)
		ind[i]=ind[ind[i]];
	
	smart_alloc(area, int, cur_ind+1);
	smart_area.zero();
	
	smart_alloc(min_x, int, cur_ind+1);
	smart_alloc(max_x, int, cur_ind+1);
	smart_alloc(min_y, int, cur_ind+1);
	smart_alloc(max_y, int, cur_ind+1);
	for(int i=cur_ind; i>=0; i--)
	{
		min_x[i]=cx; max_x[i]=0;
		min_y[i]=cy; max_y[i]=0;
	}
	
	for(int y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			int c=ind[cnt[y][x]];
			area[c]++;
			if(x<min_x[c]) min_x[c]=x;
			if(x>max_x[c]) max_x[c]=x;
			if(y<min_y[c]) min_y[c]=y;
			if(y>max_y[c]) max_y[c]=y;
		}
	}
	
	smart_alloc(del_it, BOOL, cur_ind+1);
	for(int i=cur_ind; i>=0; i--)
	{
		int nSizeLinear = max( max_x[i]-min_x[i], max_y[i]-min_y[i] );
		int nSize = (m_nFlags & 4) ? area[i] : nSizeLinear;
		if( nSize <= m_nMinSize )
		{ // 0..MinSize
			del_it[i] = !(m_nFlags & 1);
		}
		else if( nSize <= m_nMinSize2 )
		{ // MinSize..MinSize2
			double fForm = double(area[i]) / sqr(nSizeLinear+1);
			if(fForm > m_fFormThreshold)
				del_it[i] = !(m_nFlags & 2);
			else
				del_it[i] = (m_nFlags & 2);
		}
		else
		{ // MinSize2 и выше
			del_it[i] = (m_nFlags & 1);
		}
	}

	//приграничные области не трогаем
	/*
	{for (y=0;y<cy;y++)
	{
		{int c=ind[cnt[y][0]]; del_it[c]=0;}
		{int c=ind[cnt[y][cx-1]]; del_it[c]=0;}
	}}
	{for (x=0;x<cx;x++)
	{
		{int c=ind[cnt[0][x]]; del_it[c]=0;}
		{int c=ind[cnt[cy-1][x]]; del_it[c]=0;}
	}}
	*/
	
	for(int y=0; y<cy; y++)
	{
		for(int x=0; x<cx; x++)
		{
			int c=ind[cnt[y][x]];
			if(del_it[c]) buf[y][x]=fill_color;
		}
	}
}

bool CBinDeleteSmallObjects::InvokeFilter()
{
	IBinaryImagePtr	binDst = GetDataResult();
	IBinaryImagePtr	binSrc = GetDataArgument("BinSource");
	if(binDst == 0 || binSrc == 0)
		return false;

	m_nMinSize = GetArgLong( "MinSize" );	
	m_nMinSize2 = GetArgLong( "MinSize2" );	
	m_fFormThreshold = GetArgDouble( "FormThreshold" );	
	m_nFillValue = GetArgLong( "FillValue" );
	m_nFlags = GetArgLong( "Flags" );

	int cx, cy;
	binSrc->GetSizes(&cx, &cy);
	if( binDst->CreateNew(cx, cy) != S_OK )
		return false;

	// A.M. fix, BT4414 (BinDeleteSmallObjects used in script BinMeas).
	POINT ptOffset;
	binSrc->GetOffset(&ptOffset);
	binDst->SetOffset(ptOffset, FALSE);

	smart_alloc(src, byte*, cy);
	smart_alloc(dst, byte*, cy);

	StartNotification(cy,1);

	for(int y=0; y<cy; y++)
	{
		binDst->GetRow(dst+y, y, FALSE);
		binSrc->GetRow(src+y, y, FALSE);
	}

	for(long y = 0; y < cy; y++)
	{
		for(long x = 0; x<cx; x++)
		{
			dst[y][x] = src[y][x];
		}
		Notify(y);
	}
	NextNotificationStage();

	FillSmallSegments(dst, cx,cy, m_nFillValue);

	FinishNotification();
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//CBinMove
////////////////////////////////////////////////////////////////////////////////////////////////////
CBinMove::CBinMove() : m_ptOrigOffset(0 ,0), 
					   m_ptNewOffset(0, 0),
					   m_rectActiveImage(0, 0, 0, 0)
{
	m_image = 0;
}

CBinMove::~CBinMove()
{
}

IUnknown* CBinMove::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return CInteractiveAction::DoGetInterface( iid );
}

bool CBinMove::Initialize()
{
	if( !CInteractiveAction::Initialize() )
		return false;

	IDataContextPtr		sptrContext( m_ptrTarget );
	IUnknown* punkObj = 0;
	sptrContext->GetActiveObject(_bstr_t(szTypeBinaryImage), &punkObj);

	m_image = punkObj;

	if(punkObj)
		punkObj->Release();

	if(m_image != 0)
	{
		int nCX = 0, nCY = 0;
		POINT pt = {0, 0};
		m_image->GetSizes(&nCX, &nCY);
		m_image->GetOffset(&pt);

		m_rectActiveImage.set(pt.x, pt.y, pt.x + nCX, pt.y + nCY);
	}
	
	return m_image != 0;
}

void CBinMove::DeInit()
{
}

bool CBinMove::Finalize()
{
	if( !CInteractiveAction::Finalize() )
		return false;

	return true;
}

bool CBinMove::DoStartTracking( _point point )
{
	m_ptOrigOffset = point;
	return true;
}

bool CBinMove::DoTrack( _point point )
{
	MoveTo(point.delta(m_ptOrigOffset));
	m_ptOrigOffset = point;
	return true;
}


bool CBinMove::DoFinishTracking( _point point )
{
	m_ptNewOffset = point;
	Finalize();
	return true;
}

bool CBinMove::DoLButtonDblClick( _point point )
{
	_point pt(0, 0);
	m_image->GetOffset(&pt);
	pt.x = -pt.x;
	pt.y = -pt.y;
	MoveTo(pt);

	return true;
}

void CBinMove::MoveTo( _point point )
{
	IScrollZoomSitePtr		sptrSite( m_ptrTarget );
	if(sptrSite != 0)
	{
		::InvalidateRect(m_hwnd, &_window( m_rectActiveImage, sptrSite), false);
		_point pt(0, 0);
		m_image->GetOffset(&pt);
		m_rectActiveImage.set_offset(point.offset(pt));
		m_image->SetOffset(m_rectActiveImage.top_left(), TRUE);
		::InvalidateRect(m_hwnd, &_window(m_rectActiveImage, sptrSite), false);
	}
}

HRESULT CBinMove::DoInvoke()
{
	_point ptOffset = m_ptNewOffset.delta(m_ptOrigOffset);

	//trace("Begin (%d, %d); End (%d, %d)\n", m_ptOrigOffset.x, m_ptOrigOffset.y, m_ptNewOffset.x, m_ptNewOffset.y);
	
	MoveTo(ptOffset);

	return S_OK;
}

bool CBinMove::DoLButtonDown( _point point )
{
	StartTracking( point );
	return true;
}

bool CBinMove::DoChar( int nChar, bool bKeyDown )
{
	if(CInteractiveAction::DoChar(nChar, bKeyDown))
		return true;

	if(m_image == 0)return false;

	if(nChar == VK_UP || nChar == VK_DOWN|| nChar == VK_LEFT || nChar == VK_RIGHT)
	{
		_point	point(0, 0);

		int	nDelta = 1;
		if( GetAsyncKeyState( VK_SHIFT ) & 0x8000 )
			nDelta = 20;

		if( nChar == VK_UP )point.y -= nDelta;
		if( nChar == VK_DOWN )point.y += nDelta;
		if( nChar == VK_LEFT )point.x -= nDelta;
		if( nChar == VK_RIGHT )point.x += nDelta;

		MoveTo( point );
	}

	return false;
}

HRESULT CBinMove::Paint( HDC hDC, RECT rectDraw, 
						  IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, 
						  BYTE *pdibCache )
{
	IScrollZoomSitePtr	ptrScrollZoom(punkTarget);
	
	::SelectObject(hDC, ::GetStockObject(NULL_BRUSH));
	::SelectObject(hDC, ::GetStockObject(WHITE_PEN));

	_rect	rect1 = ::_window(m_rectActiveImage, ptrScrollZoom);
	::Rectangle(hDC, RECT_COORDS_ARG(rect1));

	return S_OK;
}

HRESULT CBinMove::GetRect( RECT *prect )
{
	if(prect)
	{
		*prect = m_rectActiveImage;
		return S_OK;
	}

	return E_INVALIDARG;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//CBinMorphDivision
////////////////////////////////////////////////////////////////////////////////////////////////////
CBinMorphDivision::CBinMorphDivision()
{
}

//// vt5
////void CDivMorphFunc::DoWork( void **pArguments, void *pResult )
//// end vt5
//bool CBinMorphDivision::InvokeFilter()
//{
//	// vt5
//	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
//
//	//CBINArgument	*pinList = (CBINArgument	*)pArguments[0];
//	//CBINArgument	*poutList = (CBINArgument	*)pResult;
//	//StartNotification(pinList->m_lpbi->biHeight);
//
//	IBinaryImagePtr ptrBinSrc = GetDataArgument();
//	IBinaryImagePtr	ptrBinDst = GetDataResult();
//	if ((ptrBinSrc == NULL) || (ptrBinDst == NULL)) return false;
//	
//	StartNotification(2);
//	// end vt5
//
//	try{
//		{ 
//			WORD bFill=20,BF;
//			WORD  * hptr8, *_hptr8, * xhptr8;
//			HGLOBAL hDIBits8,_hDIBits8, XhDIBits8;
//			int iDilCount=0;
//			LONG dwd,x,y;
//			BOOL bError=FALSE;
//			// vt5
//			//CString cStr;
//			// vt5
//			
//			// vt5
//			int iImageWidth, iImageHeight;
//			ptrBinSrc->GetSizes(&iImageWidth, &iImageHeight);
//			ptrBinDst->CreateNew(iImageWidth, iImageHeight);
//			// end vt5
//
//			// vt5
//			//DWORD bpl8;
//			// округляем ширину до кратного 4-м (в большую сторону)
//			DWORD bpl8 = (iImageWidth+3)&~3;
//			// end vt5
//			
//			// vt5
//			//int	iMemSize = ((pinList->m_lpbi->biWidth+3)/4*4)*pinList->m_lpbi->biHeight+1024+sizeof(BITMAPINFOHEADER);
//			int	iMemSize = ((iImageWidth+3)/4*4)*iImageHeight+1024+sizeof(BITMAPINFOHEADER);
//			// end vt5
//
//			BITMAPINFOHEADER *lpbmih = (BITMAPINFOHEADER *)AllocMem( iMemSize );
//			// vt5
//			//poutList->m_lpbi = lpbmih;
//			//memcpy( poutList->m_lpbi, pinList->m_lpbi, iMemSize );
//			lpbmih->biHeight = iImageHeight;
//			lpbmih->biWidth = iImageWidth;
//			// end vt5
//
//			BYTE	*pimage = (BYTE*)(lpbmih+1)+1024;
//
//			// vt5
//			// Копируем содержимое исходного изображения в массив байт
//			BYTE *row;
//			int i, j;
//			for (i = 0; i < iImageHeight; i++)
//			{
//				ptrBinSrc->GetRow(&row, i, false);
//				for (j = 0; j < iImageWidth; j++)
//					pimage[i*bpl8+j] = row[j];
//			}
//			// end vt5
//
//			CMorphInfo info( lpbmih );
//						
//			int Percentage=0;
//			ChangePercent(0);
//			bpl8=(lpbmih->biWidth+3)&~3;
//			if (!ChangePercent(5))
//			{
//    			bError = TRUE;
//    			goto finish;  
//			}
//    
//					 hDIBits8=GlobalAlloc(GHND,lpbmih->biHeight*bpl8*2);
//					 if (!hDIBits8) 
//					{
//		    			bError = TRUE;
//		    			goto finish;  
//					}
//					 hptr8=(WORD *)GlobalLock(hDIBits8);
//					if (!hptr8) 
//					{
//						bError = TRUE;
//						goto finish;  
//					}
//					 _hDIBits8=GlobalAlloc(GHND,lpbmih->biHeight*bpl8*2);
//					 if (!_hDIBits8) 
//					{
//						bError = TRUE;
//						goto finish;  
//					}
//					 _hptr8=(WORD *)GlobalLock(_hDIBits8);
//					 if (!_hptr8) 
//					{
//						bError = TRUE;
//						goto finish;  
//					}
//					 
//					 XhDIBits8=GlobalAlloc(GHND,lpbmih->biHeight*bpl8*2);
//					 if (!XhDIBits8) 
//					{
//						bError = TRUE;
//						goto finish;  
//					}
//					 xhptr8=(WORD *)GlobalLock(XhDIBits8);
//					 if (!xhptr8) 
//					{
//						bError = TRUE;
//						goto finish;  
//					}
//					 
//					// vt5
//					for (dwd=0;dwd<(LONG)(lpbmih->biHeight*bpl8);dwd++)
//						hptr8[dwd]=(pimage[dwd]>127) ? 255:0;
//					
//					for (dwd=0;dwd<lpbmih->biHeight*((lpbmih->biWidth+3)&~3);dwd++)
//					{
//		     		hptr8[dwd]&=1;
//		     		xhptr8[dwd]=hptr8[dwd];
//					}
//					//for (dwd=0;dwd<(LONG)(lpbmih->biHeight*bpl8);dwd++)
//					//hptr8[dwd]=pimage[dwd];
//					//
//					//for (dwd=0;dwd<lpbmih->biHeight*((lpbmih->biWidth+3)&~3);dwd++)
//					//{
//		   //  		hptr8[dwd]&=1;
//		   //  		xhptr8[dwd]=hptr8[dwd];
//					//}
//					 // end vt5
//
//		//  EROSION
//					if (!ChangePercent(15))
//					{
//						bError = TRUE;
//						goto finish;  
//					}
//					 while (Erosion (hptr8,(int)lpbmih->biWidth,(int)lpbmih->biHeight,bFill++));
//					if (!ChangePercent(25))
//					{
//						bError = TRUE;
//						goto finish;  
//					}
//		// FIND ERODANTS			
//					   Erodants(hptr8,_hptr8,(int)lpbmih->biWidth,(int)lpbmih->biHeight);
//						if (!ChangePercent(30))
//						{
//							bError = TRUE;
//							goto finish;  
//						}
//		// CLEAN IMAGE			   
//					   CleanImageForward (_hptr8,hptr8,(int)lpbmih->biWidth,(int)lpbmih->biHeight);
//					   CleanImageBack (_hptr8,hptr8,(int)lpbmih->biWidth,(int)lpbmih->biHeight);
//						if (!ChangePercent(35))
//						{
//							bError = TRUE;
//							goto finish;  
//						}
//		//	Thrash out
//					   for (dwd=0;dwd<(LONG)(lpbmih->biHeight*bpl8);dwd++)
//					   {
//					   if (_hptr8[dwd]==255) _hptr8[dwd]=0;
//					   hptr8[dwd]=(_hptr8[dwd]==0 ? 0 : 255);
//					   }
//						if (!ChangePercent(45))
//						{
//							bError = TRUE;
//							goto finish;  
//						}
//		//	Clear around
//					   dwd=((lpbmih->biWidth+3)&~3);
//					   for (y=0;y<3;y++)
//					   for (x=0;x<lpbmih->biWidth;x++)
//						{
//						_hptr8[y*dwd+x]=0;
//						_hptr8[x+(lpbmih->biHeight-3+y)*dwd]=0;
//						}
//					   for (x=0;x<3;x++)
//					   for (y=0;y<lpbmih->biHeight;y++)
//						{
//						_hptr8[y*dwd+x]=0;
//						_hptr8[y*dwd+lpbmih->biWidth-3+x]=0;
//						}
//					   memcpy(hptr8,_hptr8,lpbmih->biHeight*dwd*2);
//		//	Back step contour
//
//					   DilationX(hptr8,_hptr8,xhptr8,(int)lpbmih->biWidth,(int)lpbmih->biHeight);
//					   memcpy(hptr8,_hptr8,lpbmih->biHeight*dwd*2);
//					   bFill--;			   
//					   BF=bFill=bFill*3-bFill/3;
//						if (!ChangePercent(50))
//						{
//							bError = TRUE;
//							goto finish;  
//						}
//					   
//			do
//			{
//		// DILATION
//				while (Dilation(hptr8,xhptr8,(int)lpbmih->biWidth,(int)lpbmih->biHeight,bFill)&&bFill)
//				{
//						bFill--;
//						Percentage=50*(BF-bFill)/BF+50;
//						iDilCount++;
//						if (!ChangePercent(Percentage))
//						{
//							bError = TRUE;
//							goto finish;  
//						}
//				}
//				bFill--;
//			}
//			while ((iDilCount==0)&&(bFill));
//
//		   for (dwd=0;dwd<lpbmih->biHeight*((lpbmih->biWidth+3)&~3);dwd++)
//		   hptr8[dwd]=(hptr8[dwd]==0 ? 0 : 255);
//   
//		   dwd=((lpbmih->biWidth+3)&~3);
//   
//			for (x=0;x<lpbmih->biWidth;x++)
//			{
//				if (xhptr8[x]!=0)
//		 			if (hptr8[x+dwd]!=0)
//		   				hptr8[x]=255;
//				if (xhptr8[x+(lpbmih->biHeight-1)*dwd]!=0)
//					if (hptr8[x+(lpbmih->biHeight-2)*dwd]!=0)
//						hptr8[x+(lpbmih->biHeight-1)*dwd]=255;
//			}
//			for (y=0;y<lpbmih->biHeight;y++)
//			{
//				if (xhptr8[y*dwd]!=0)
//				 if (hptr8[y*dwd+1]!=0) hptr8[y*dwd]=255;
//				if (xhptr8[y*dwd+lpbmih->biWidth-1]!=0)
//				 if (hptr8[y*dwd+lpbmih->biWidth-2]!=0)
//				  hptr8[y*dwd+lpbmih->biWidth-1]=255;
//			}
//			for (dwd=0;dwd<(LONG)(lpbmih->biHeight*bpl8);dwd++)
//				pimage[dwd]=LOBYTE(hptr8[dwd]);
//
//			Erose( info );
//			Deaglom( info );
//			Deaglom( info );
//			Clean( info );
//finish:;
//			ChangePercent(100);
//			// vt5
//			Notify(1);
//			// Перегоняем содержимое массива в результирующее изображение
//			for (i = 0; i < iImageHeight; i++)
//			{
//				ptrBinDst->GetRow(&row, i, false);
//				for (j = 0; j < iImageWidth; j++)
//					row[j] = pimage[i*bpl8+j];
//			}
//			// end vt5
//			
//			GlobalUnlock(XhDIBits8);
//			GlobalUnlock(_hDIBits8);
//			GlobalFree(XhDIBits8);
//			GlobalFree(_hDIBits8);
//			GlobalUnlock(hDIBits8);
//			GlobalFree(hDIBits8);
//		}
//	}
//	// vt5
//	//catch( CException *pe )
//	//{
//	//	pe->ReportError();
//	//	pe->Delete();
//	//}
//	// end vt5
//	catch( ... )
//	{
//	}
//	// vt5
//	//EndNotification();
//	FinishNotification();
//	// end vt5
//}

#if 1
bool CBinMorphDivision::InvokeFilter()
{

	IBinaryImagePtr ptrBinSrc = GetDataArgument();
	IBinaryImagePtr	ptrBinDst = GetDataResult();
	if ((ptrBinSrc == NULL) || (ptrBinDst == NULL)) return false;

	StartNotification(200);

	try
	{
		WORD bFill=20,BF;
		int iDilCount = 0;
		LONG dwd, x, y;
		BOOL bError = FALSE;
		
		int iImageWidth, iImageHeight;
		ptrBinSrc->GetSizes(&iImageWidth, &iImageHeight);
		ptrBinDst->CreateNew(iImageWidth, iImageHeight);

		// округляем ширину до кратного 4-м (в большую сторону)
		DWORD bpl8 = (iImageWidth+3)&~3;
		// Создаем массив для работы с изображением
		smart_alloc(pimage,byte,iImageHeight*bpl8);
		
		// Копируем содержимое исходного изображения в массив байт
		byte *row;
		int i, j;
		for (i = 0; i < iImageHeight; i++)
		{
			ptrBinSrc->GetRow(&row, i, false);
			for (j = 0; j < iImageWidth; j++)
				pimage[i*bpl8+j] = row[j];
		}

		// Создаем копию исходного изображения
		smart_alloc(srcImage,byte,iImageHeight*bpl8);
		for (i = 0; i < iImageHeight; i++)
		{
			ptrBinSrc->GetRow(&row, i, false);
			for (j = 0; j < iImageWidth; j++)
				srcImage[i*bpl8+j] = row[j];
		}

		int Percentage = 0;
		Notify(5);
		
		smart_alloc(hptr8,WORD,iImageHeight*bpl8);
		memset(hptr8, 0, iImageHeight*bpl8*2);
		smart_alloc(_hptr8,WORD,iImageHeight*bpl8);
		memset(_hptr8, 0, iImageHeight*bpl8*2);
		smart_alloc(xhptr8,WORD,iImageHeight*bpl8);
		memset(xhptr8, 0, iImageHeight*bpl8*2);
					
		for (dwd = 0; dwd < (LONG)(iImageHeight*bpl8); dwd++)
			hptr8[dwd]= (pimage[dwd] > 127) ? 255 : 0;
		
		for (dwd = 0; dwd < iImageHeight*bpl8; dwd++)
		{
			hptr8[dwd] &= 1;
			xhptr8[dwd] = hptr8[dwd];
		}

		//  EROSION
		Notify(15);
		while (Erosion(hptr8, (int)iImageWidth, (int)iImageHeight, bFill++));
		Notify(25);

		// FIND ERODANTS			
		Erodants(hptr8, _hptr8, (int)iImageWidth, (int)iImageHeight);
		Notify(30);
		
		// CLEAN IMAGE			   
		CleanImageForward(_hptr8, hptr8, (int)iImageWidth, (int)iImageHeight);
		CleanImageBack(_hptr8, hptr8, (int)iImageWidth, (int)iImageHeight);
		Notify(35);
		
		//	Thrash out
		for (dwd = 0; dwd<(LONG)(iImageHeight*bpl8); dwd++)
		{
			if (_hptr8[dwd] == 255) 
				_hptr8[dwd] = 0;
			hptr8[dwd] = (_hptr8[dwd]==0 ? 0 : 255);
		}
		Notify(45);
		
		//	Clear around
		dwd = bpl8;
		for (y = 0; y < 3; y++)
			for (x = 0; x < iImageWidth;x++)
			{
				_hptr8[y*dwd+x] = 0;
				_hptr8[x+(iImageHeight-3+y)*dwd] = 0;
			}
		for (x = 0; x < 3; x++)
			for (y = 0; y < iImageHeight; y++)
			{
				_hptr8[y*dwd+x] = 0;
				_hptr8[y*dwd+iImageWidth-3+x] = 0;
			}
		memcpy(hptr8, _hptr8, iImageHeight*dwd*2);

		//	Back step contour
		DilationX(hptr8, _hptr8, xhptr8, (int)iImageWidth, (int)iImageHeight);
		memcpy(hptr8, _hptr8, iImageHeight*dwd*2);
		bFill--;			   
		BF = bFill = bFill*3-bFill/3;
		Notify(50);
					
		do
		{
		// DILATION
			while (Dilation(hptr8, xhptr8, (int)iImageWidth, (int)iImageHeight, bFill) && bFill)
			{
					bFill--;
					Percentage = 50*(BF-bFill)/BF+50;
					iDilCount++;
					Notify(Percentage);
			}
			bFill--;
		} 
		while ((iDilCount == 0) && (bFill));

		for (dwd = 0; dwd < iImageHeight*bpl8; dwd++)
		hptr8[dwd] = (hptr8[dwd] == 0 ? 0 : 255);
	
		dwd = bpl8;
		for (x = 0; x < iImageWidth; x++)
		{
			if (xhptr8[x] != 0)
		 		if (hptr8[x+dwd] != 0)
		   			hptr8[x] = 255;
			if (xhptr8[x+(iImageHeight-1)*dwd] != 0)
				if (hptr8[x+(iImageHeight-2)*dwd] != 0)
					hptr8[x+(iImageHeight-1)*dwd] = 255;
		}
		for (y = 0; y< iImageHeight; y++)
		{
			if (xhptr8[y*dwd] != 0)
				if (hptr8[y*dwd+1] != 0) 
					hptr8[y*dwd] = 255;
			if (xhptr8[y*dwd+iImageWidth-1] != 0)
				if (hptr8[y*dwd+iImageWidth-2] != 0)
					hptr8[y*dwd+iImageWidth-1] = 255;
		}
		for (dwd = 0; dwd<(LONG)(iImageHeight*bpl8); dwd++)
			pimage[dwd] = LOBYTE(hptr8[dwd]);
		
		
		bool bIsIsolated = false;
		// Удаляем точки объекта, вокруг которых нет других точек объекта
		// 0 0 0    0 0 0
		// 0 1 0 -> 0 0 0
		// 0 0 0    0 0 0
		for ( i = 0; i < iImageHeight; i++ )
			for ( j = 0; j < iImageWidth; j++)
			{
				if ( pimage[i*bpl8+j] == 255 )
				{
					bIsIsolated = true;
					if ( (i > 0) && (j > 0) && (pimage[(i-1)*bpl8+(j-1)] == 255) )
						bIsIsolated = false;
					if ( (i > 0) && (pimage[(i-1)*bpl8+j] == 255) )
						bIsIsolated = false;
					if ( (i > 0) && (j < iImageWidth) && (pimage[(i-1)*bpl8+(j+1)] == 255) )
						bIsIsolated = false;

					if ( (j > 0) && (pimage[i*bpl8+(j-1)] == 255) )
						bIsIsolated = false;
					if ( (j < iImageWidth) && (pimage[i*bpl8+(j+1)] == 255) )
						bIsIsolated = false;

					if ( (i < iImageHeight) && (j > 0) && (pimage[(i+1)*bpl8+(j-1)] == 255) )
						bIsIsolated = false;
					if ( (i < iImageHeight) && (pimage[(i+1)*bpl8+j] == 255) )
						bIsIsolated = false;
					if ( (i < iImageHeight) && (j < iImageWidth) && (pimage[(i+1)*bpl8+(j+1)] == 255) )
						bIsIsolated = false;

					if ( bIsIsolated )
					{
						pimage[i*bpl8+j] = 0;
					}
				}
			}
		
		bool bIsDilated = false;
		while ( !bIsDilated )
		{
			bIsDilated = true;
			for ( i = 0; i < iImageHeight; i++ )
				for ( j = 0; j < iImageWidth; j++ )
				{
					if ( (pimage[i*bpl8+j] == 255) )
					//if ( (pimage[i*bpl8+j] == 0) && (srcImage[i*bpl8+j] > 127) )
					{
						if ( (i > 0) && (j > 0) && (pimage[(i-1)*bpl8+(j-1)] == 0) )
							if ( (i < iImageHeight) && (j < iImageWidth) && (pimage[(i+1)*bpl8+(j+1)] == 0) )
							{
								pimage[i*bpl8+j] = 0;
								bIsDilated = false;
							}
						if ( (i > 0) && (pimage[(i-1)*bpl8+j] == 0) )
							if ( (i < iImageHeight) && (pimage[(i+1)*bpl8+j] == 0) )
							{
								pimage[i*bpl8+j] = 0;
								bIsDilated = false;
							}
						if ( (i > 0) && (j < iImageWidth) && (pimage[(i-1)*bpl8+(j+1)] == 0) )
							if ( (i < iImageHeight) && (j > 0) && (pimage[(i+1)*bpl8+(j-1)] == 0) )
							{
								pimage[i*bpl8+j] = 0;
								bIsDilated = false;
							}
						if ( (j > 0) && (pimage[i*bpl8+(j-1)] == 0) )
							if ( (j < iImageWidth) && (pimage[i*bpl8+(j+1)] == 0) )
							{
								pimage[i*bpl8+j] = 0;
								bIsDilated = false;
							}
					}
				}
		}

		int nCount = 0;
		bIsDilated = false;
		// Заполняем точки фона, у которых среди соседей не более одной точки фона
		// ? 1 ?    ? 1 ?
		// 0 0 1 -> 0 1 1
		// ? 1 ?    ? 1 ?
		while ( !bIsDilated )
		{
			bIsDilated = true;
			for ( i = 0; i < iImageHeight; i++ )
				for ( j = 0; j < iImageWidth; j++ )
				{
					if ( (pimage[i*bpl8+j] == 0) )
					//if ( (pimage[i*bpl8+j] == 0) && (srcImage[i*bpl8+j] > 127) )
					{
						nCount = 0;
						if ( (i > 0) && (pimage[(i-1)*bpl8+j] == 0) )
							nCount++;

						if ( (j > 0) && (pimage[i*bpl8+(j-1)] == 0) )
							nCount++;
						if ( (j < iImageWidth) && (pimage[i*bpl8+(j+1)] == 0) )
							nCount++;

						if ( (i < iImageHeight) && (pimage[(i+1)*bpl8+j] == 0) )
							nCount++;

						if ( nCount < 2 )
						{
							pimage[i*bpl8+j] = 255;
							bIsDilated = false;
						}
					}
				}
		}

		// Перегоняем содержимое массива в результирующее изображение
		for (i = 0; i < iImageHeight; i++)
		{
			ptrBinDst->GetRow(&row, i, false);
			for (j = 0; j < iImageWidth; j++)
				row[j] = pimage[i*bpl8+j];
		}
		
	}
	catch(...)
	{
	}
	
	FinishNotification();
	
	return true;
}
#else
static void MakeDebugImage(IUnknown *punkTarget, int iImageWidth, int iImageHeight,
	WORD *hptr8, DWORD bpl8, const char *pName)
{
	IUnknownPtr punkTest(::CreateTypedObject(_bstr_t("Image")), false);
	IImage4Ptr sptrImgTest(punkTest);
	sptrImgTest->CreateImage(iImageWidth, iImageHeight, _bstr_t("Gray"), 1);
	for (int i = 0; i < iImageHeight; i++)
	{
		color *row;
		sptrImgTest->GetRow(i, 0, &row);
		for (int j = 0; j < iImageWidth; j++)
			row[j] = hptr8[i*bpl8+j]<<8;
	}
	::SetName(punkTest, _bstr_t(pName));
	_variant_t	var( (IUnknown*)punkTest );
	::SetValue( punkTarget, 0, 0, var );
}

bool CBinMorphDivision::InvokeFilter()
{

	IBinaryImagePtr ptrBinSrc = GetDataArgument();
	IBinaryImagePtr	ptrBinDst = GetDataResult();
	if ((ptrBinSrc == NULL) || (ptrBinDst == NULL)) return false;

	StartNotification(200);

	try
	{
		WORD bFill=20,BF;
		int iDilCount = 0;
		LONG dwd, x, y;
		BOOL bError = FALSE;
		
		int iImageWidthSrc, iImageHeightSrc;
		ptrBinSrc->GetSizes(&iImageWidthSrc, &iImageHeightSrc);
		ptrBinDst->CreateNew(iImageWidthSrc, iImageHeightSrc);

		// Создаем большее изображение со смещением - иначе не работают функции в DivFunc.cpp.
		int iImageWidth = iImageWidthSrc+4, iImageHeight = iImageHeightSrc+4;
		int iOffset = 2;

		// округляем ширину до кратного 4-м (в большую сторону)
		DWORD bpl8 = (iImageWidth+3)&~3;
		// Создаем массив для работы с изображением
		smart_alloc(pimage,byte,iImageHeight*bpl8);
		memset(pimage,0,iImageHeight*bpl8);
		
		// Копируем содержимое исходного изображения в массив байт
		byte *row;
		int i, j;
		for (i = 0; i < iImageHeightSrc; i++)
		{
			ptrBinSrc->GetRow(&row, i, false);
			for (j = 0; j < iImageWidthSrc; j++)
				pimage[(i+iOffset)*bpl8+j+iOffset] = row[j];
		}

		// Создаем копию исходного изображения
		smart_alloc(srcImage,byte,iImageHeight*bpl8);
		for (i = 0; i < iImageHeight; i++)
		{
			ptrBinSrc->GetRow(&row, i, false);
			for (j = 0; j < iImageWidth; j++)
				srcImage[i*bpl8+j] = row[j];
		}

		int Percentage = 0;
		Notify(5);
		
		smart_alloc(hptr8,WORD,iImageHeight*bpl8);
		memset(hptr8, 0, iImageHeight*bpl8*2);
		smart_alloc(_hptr8,WORD,iImageHeight*bpl8);
		memset(_hptr8, 0, iImageHeight*bpl8*2);
		smart_alloc(xhptr8,WORD,iImageHeight*bpl8);
		memset(xhptr8, 0, iImageHeight*bpl8*2);
					
		for (dwd = 0; dwd < (LONG)(iImageHeight*bpl8); dwd++)
			hptr8[dwd]= (pimage[dwd] > 127) ? 255 : 0;
		
		for (dwd = 0; dwd < iImageHeight*bpl8; dwd++)
		{
			hptr8[dwd] &= 1;
			xhptr8[dwd] = hptr8[dwd];
		}

		//  EROSION
		Notify(15);
		while (Erosion(hptr8, (int)iImageWidth, (int)iImageHeight, bFill++));
		Notify(25);

//		MakeDebugImage(m_ptrTarget, iImageWidth, iImageHeight,
//			hptr8, bpl8, "Erosion");


		// FIND ERODANTS			
		Erodants(hptr8, _hptr8, (int)iImageWidth, (int)iImageHeight);
		Notify(30);

//		MakeDebugImage(m_ptrTarget, iImageWidth, iImageHeight,
//			_hptr8, bpl8, "Erodants");


		// CLEAN IMAGE			   
		CleanImageForward(_hptr8, hptr8, (int)iImageWidth, (int)iImageHeight);
		CleanImageBack(_hptr8, hptr8, (int)iImageWidth, (int)iImageHeight);
		Notify(35);
		
		//	Thrash out
		for (dwd = 0; dwd<(LONG)(iImageHeight*bpl8); dwd++)
		{
			if (_hptr8[dwd] == 255) 
				_hptr8[dwd] = 0;
			hptr8[dwd] = (_hptr8[dwd]==0 ? 0 : 255);
		}
		Notify(45);

//		MakeDebugImage(m_ptrTarget, iImageWidth, iImageHeight,
//			hptr8, bpl8, "Cleaned Erodants");


		//	Clear around
		dwd = bpl8;
/*		for (y = 0; y < 3; y++)
			for (x = 0; x < iImageWidth;x++)
			{
				_hptr8[y*dwd+x] = 0;
				_hptr8[x+(iImageHeight-3+y)*dwd] = 0;
			}
		for (x = 0; x < 3; x++)
			for (y = 0; y < iImageHeight; y++)
			{
				_hptr8[y*dwd+x] = 0;
				_hptr8[y*dwd+iImageWidth-3+x] = 0;
			}*/
		memcpy(hptr8, _hptr8, iImageHeight*dwd*2);

		//	Back step contour
		DilationX(hptr8, _hptr8, xhptr8, (int)iImageWidth, (int)iImageHeight);
		memcpy(hptr8, _hptr8, iImageHeight*dwd*2);
		bFill--;			   
		BF = bFill = CalcDilateNum(bFill);//bFill*3-bFill/3;
		Notify(50);

//		MakeDebugImage(m_ptrTarget, iImageWidth, iImageHeight,
//			hptr8, bpl8, "DilationX");

		do
		{
		// DILATION
			while (bFill && Dilation(hptr8, xhptr8, (int)iImageWidth, (int)iImageHeight, bFill))
			{
					bFill--;
					Percentage = 50*(BF-bFill)/BF+50;
					iDilCount++;
					Notify(Percentage);
			}
			if (bFill > 0)
				bFill--;
		} 
		while ((iDilCount == 0) && (bFill));

//		MakeDebugImage(m_ptrTarget, iImageWidth, iImageHeight,
//			hptr8, bpl8, "Dilation");


		for (dwd = 0; dwd < iImageHeight*bpl8; dwd++)
		hptr8[dwd] = (hptr8[dwd] == 0 ? 0 : 255);
	
		dwd = bpl8;
		for (x = 0; x < iImageWidth; x++)
		{
			if (xhptr8[x] != 0)
		 		if (hptr8[x+dwd] != 0)
		   			hptr8[x] = 255;
			if (xhptr8[x+(iImageHeight-1)*dwd] != 0)
				if (hptr8[x+(iImageHeight-2)*dwd] != 0)
					hptr8[x+(iImageHeight-1)*dwd] = 255;
		}
		for (y = 0; y< iImageHeight; y++)
		{
			if (xhptr8[y*dwd] != 0)
				if (hptr8[y*dwd+1] != 0) 
					hptr8[y*dwd] = 255;
			if (xhptr8[y*dwd+iImageWidth-1] != 0)
				if (hptr8[y*dwd+iImageWidth-2] != 0)
					hptr8[y*dwd+iImageWidth-1] = 255;
		}
		for (dwd = 0; dwd<(LONG)(iImageHeight*bpl8); dwd++)
			pimage[dwd] = LOBYTE(hptr8[dwd]);
		
		// Перегоняем содержимое массива в результирующее изображение
		for (i = 0; i < iImageHeightSrc; i++)
		{
			ptrBinDst->GetRow(&row, i, false);
			for (j = 0; j < iImageWidthSrc; j++)
				row[j] = pimage[(i+iOffset)*bpl8+j+iOffset];
		}
		
	}
	catch(...)
	{
	}
	
	FinishNotification();
	
	return true;
}
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
//CBinMorphDivision2
////////////////////////////////////////////////////////////////////////////////////////////////////
using namespace _kir;
CBinMorphDivision2::CBinMorphDivision2()
{
}

CWatershed::CWatershed()
{
	disp = 2;//min dispersion of object centers
	levelMap.reserve(1000);
	imgDist = 0;
	src = 0;
	srcCopy=0;
	currLevel = 0;
	init = false;
	currPosition=0;
	currentLabel = 0;
	currentDist = 0;
	pixNumber=0;
	e_mask = true;
	
	
}
CWatershed::~CWatershed()
{
	delete[]imgDist ;
	delete[]src;
	delete[] srcCopy;
	delete[] sortedSrc;
}


void CWatershed::Init(IBinaryImage* bin)
{
	if(init) return;
	
	bin->GetSizes(&cx, &cy);
	src = new byte[cx*cy];
	srcCopy = new short[cx*cy];
	if(src && srcCopy)
	{
		byte* temp = src;
		short* tempCopy = srcCopy;
		for(int j=0;j<cy;j++)
		{
			byte* b;
			bin->GetRow(&b, j, false);
			for(int i =0;i<cx;i++)
			{
				tempCopy[i]=temp[i]=b[i];
			}
			temp+=cx;
			tempCopy+=cx;
		}
	}

	init =true;
	sortedSrc = new byte*[cx*cy];
	imgDist = new short[cx*cy];
	memset(imgDist,0,sizeof(short)*cx*cy);
}

void CWatershed::Erode()
{
	UINT count;
	util.reserve(cx*cy/10);
	hMin = 1;
	while(count = erode())
	{
		currLevel++;
		levelMap.push_back(count);
        /* levelMap will contains the quantity of pixels in each gray level
		   forex.: levelMap[N] will contain a number of pixels with graylevel N+1 or
		   - other speak - the number of pixels affected by the N-th step of erosion */
		pixNumber+=count;
	}
	hMax = currLevel;
	util.reserve(0);//clear unnesessary memory
}
void CWatershed::AddLevel(byte* b, short* copy, int& count)
{
	sortedSrc[currPosition] = b;
	currPosition++;
	count++;
	util.push_back(copy);

}

void CWatershed::CopyRow(byte* dest, int y)
{
	for(int i=0;i<cx;i++)
	{
        dest[i] = srcCopy[i+y*cx];
	}
}


int CWatershed::erode()
{
	
	int count = 0;
	for(int j=0;j<cy;j++)
		for(int i =0;i<cx;i++)
		{
			int ind = i+cx*j;
			if(srcCopy[ind]==0) continue;
			else 
			{
				if(j-1>=0)			
				{
					if(i-1>=0)
					if(e_mask && srcCopy[i-1+cx*(j-1)]==0){ AddLevel(&src[ind],&srcCopy[ind],count); continue; }
					if(srcCopy[i+cx*(j-1)]==0){AddLevel(&src[ind],&srcCopy[ind],count); continue;}
					if(i+1<cx) 
						if(e_mask && srcCopy[i+1+cx*(j-1)]==0){AddLevel(&src[ind],&srcCopy[ind],count); continue;}				
				}

				if(i-1>=0) if(srcCopy[ind-1]==0){AddLevel(&src[ind],&srcCopy[ind],count); continue;}
				if(i+1<cx) if(srcCopy[ind+1]==0){AddLevel(&src[ind],&srcCopy[ind],count); continue;}	

				if(j+1<cy)
				{
					if(i-1>=0) if(e_mask && srcCopy[i-1+cx*(j+1)]==0){AddLevel(&src[ind],&srcCopy[ind],count); continue;}
					if(srcCopy[i+cx*(j+1)]==0){AddLevel(&src[ind],&srcCopy[ind],count); continue;}
					if(i+1<cx) if(e_mask && srcCopy[i+1+cx*(j+1)]==0){AddLevel(&src[ind],&srcCopy[ind],count); continue;}	
				}
			
			}
			

		}
		int s = util.size();
		for(int i =0;i<s;i++) *(util[i])=0;
		util.clear();
		e_mask = !e_mask;
		return count;//return number of affected pixels
}


short& CWatershed::im_out(byte* p)
{
	return (short&)*(srcCopy+(p-src));
}
short& CWatershed::im_dist(/*in*/byte* p)
{
	return *(imgDist+(p-src));
}

bool CWatershed::pix_in_area(byte*& b, byte* p,int i, int j)
{
	int x,y;
	x = (p-src)%cx+i;
	y = (p-src)/cx+j;

	if(x<0 || y<0 || x>=cx || y>=cy) return false;
	b = p+i+cx*j;
	return true;
}

short* CWatershed::out_neighbour(short* p, int i, int j)
{
int x,y;
	x = (p-srcCopy)%(cx*sizeof(short))+i;
	y = (p-srcCopy)/(cx*sizeof(short))+j;

	if(x<0 || y<0 || x>=cx || y>=cy) return false;
	return p+i+cx*j;
}
void CWatershed::FindWatershed()
{
	//ptr to end of sortedSrc:
	byte** level =  sortedSrc+pixNumber;
	//we no need more the srcCopy data so will use it as output image:
	memset(srcCopy,INIT,cx*cy*sizeof(short));
	for(int h = hMax;h>=hMin;h--)//from centers to borders
	{

		UINT n = levelMap[h-1];//number of pixels in level h
		UINT nm = n;
		level-=n; //ptr to pixels at level h
		int k=0;
		byte* p;
		while(n)//for all pixels with level h
		{	
			p=level[k];
			im_out(p) = MASK;//mark co-pixel in output image with MASK
			bool pushed = false;
			for(int j =-1;j<=1;j++)//investigating neighbours of the current pixel 
			{
				for(int i = -1; i<=1;i++)
				{
					if(j==0 && i==0) continue;
					byte* b;
					if(pix_in_area(b, p, i,j))
					{
						if(im_out(b)>0 || im_out(b)==WSHED )//if p is near already labeled point b
						{
							im_dist(p) = 1;
							fifo.push(p);//add to points of interest, will be analyzed later
							pushed=true;
							break;
						}
					}
				}
				if(pushed) break;
			}
			n--;
			k++;//next pixel of current gray level
		}
		currentDist=1;
		fifo.push(FICT_PIX);//NULL pixel for sign the edge of levels
		while(1)
		{
			p = fifo.front();
			fifo.pop();

			if(p==FICT_PIX)
			{
				if(fifo.empty()) break;
				else
				{
					fifo.push(FICT_PIX);
					currentDist++;
					p = fifo.front();
					fifo.pop();
				}
			}//if(p==FICT_PIX)


			for(int j =-1;j<=1;j++)
			{
				for(int i = -1; i<=1; i++)//for every pix in Ng(p)
				{
					if(j==0 && i==0) continue;
					byte* b;
					if(pix_in_area(b, p, i,j))
					{
						if(im_dist(b)<currentDist && (im_out(b)>0 || im_out(b)==WSHED))//i.e. b belongs to an already labeled bassin or wshed
						{																//
							if(im_out(b)>0)
							{
								if(im_out(p)==MASK || im_out(p)==WSHED)
								{
									im_out(p)=im_out(b);
								}
								else if(im_out(p)!=im_out(b)) im_out(p)=WSHED;

							}
							else if(im_out(p)==MASK)im_out(p)=WSHED;
						}
						else if(im_out(b)==MASK && im_dist(b)==0)
						{
							im_dist(b) = currentDist+1;
							fifo.push(b);
						}
					}
				}

			}
		}

			//now check if new minima has been discovered
			k=0;
			while(nm)//for all pixels with level h
			{	
				p=level[k];
				im_dist(p)=0;
				if(im_out(p)==MASK)//not marked - minima
				{
					currentLabel+=1;
					fifo.push(p);
					im_out(p)=currentLabel;
					while(!fifo.empty())
					{
						byte* pp = fifo.front();
						fifo.pop();
						//for(int j =-1;j<=1;j++)
						//{
						//	for(int i = -1; i<=1;i++)

						for(int j =-disp;j<=disp;j++)
						{
							for(int i = -disp; i<=disp;i++)
							{
								if(j==0 && i==0) continue;
								byte* b;
								if(pix_in_area(b, pp, i,j))
								{
									if(im_out(b)==MASK)//
									{
										fifo.push(b);
										im_out(b)=currentLabel;
									}
								}
							}				
						}//for(int j
					}//while(!fifo.empty())
				}
				nm--;
				k++;
			}

	}
	NormalizeOutput();
}
void CWatershed::NormalizeOutput()
{
	int n = cx*cy;
	util.reserve(cx*cy/10);
	
	for(UINT k=0;k<pixNumber;k++)
	{
		
		byte* p = sortedSrc[k];
		if(im_out(p)==0) continue;
		bool pushed = false;
		for(int j =-1;j<=1;j++)//investigating neighbours of the current pixel 
		{
			for(int i = -1; i<=1;i++)
			{
				if(j==0 && i==0) continue;
				byte* b;
				if(pix_in_area(b,p,i,j))
				{
					if(im_out(b)<im_out(p) && im_out(b)>0) 
					{
						im_out(p)=0;
					}

				}
			}
		if(pushed) break;
		}
	}
	for(int k=0;k<n;k++)
	{
		srcCopy[k] = (srcCopy[k]==-1 || srcCopy[k]==0 || srcCopy[k]==-2)?0:255;
	}

	n = util.size();
	while(n)
	{
		*(util[n-1])=0;
		n--;
	}
}



bool CBinMorphDivision2::InvokeFilter()
{
#ifdef _DEBUG
	__time_test__ t("CBinMorphDivision2 time test -");
#endif
	IBinaryImagePtr ptrBinSrc = GetDataArgument();
	IBinaryImagePtr	ptrBinDst = GetDataResult();
	if ((ptrBinSrc == NULL) || (ptrBinDst == NULL)) return false;


	int iImageWidth, iImageHeight;
	ptrBinSrc->GetSizes(&iImageWidth, &iImageHeight);
	ptrBinDst->CreateNew(iImageWidth, iImageHeight);
	StartNotification(iImageHeight);
	Notify(iImageHeight/8);


	CWatershed ws;
	ws.Init(ptrBinSrc);
	Notify(iImageHeight/4);
	ws.Erode();
	Notify(iImageHeight/3);
	ws.FindWatershed();
	Notify(iImageHeight/7*6);


	byte* row;
	for(int i =0;i<iImageHeight;i++)
	{
		ptrBinDst->GetRow(&row, i, false);
		ws.CopyRow(row,i);
		Notify(i);
	}
	FinishNotification();
	return true;
}
//////////////////
//	class CContour
//	namespace _kir
//////////////////
CContour::CContour()
{
	marker = 256;
	cx=cy=0;
	contours.reserve(256);
	currentContour=0;
	maskSize=0;
	image=0;
	imageOut =0;
	gauss=0;
	area=0;
#ifdef _DEBUG
	init = false;
#endif 
}

CContour::~CContour()
{
	int n =contours.size();
	for(int i=0;i<n;i++)
	{
		delete contours[i];
		delete cOut[i];
	}
	delete[] image;
	delete[] gauss;
	delete[] area;
	delete[] imageOut;
}
void CContour::Gauss()
{
	int n =contours.size();
	cOut.reserve(n);
	for(int i=0;i<n;i++)
	{
		Contour* c = contours[i];
		Contour* c_out = new Contour;
		cOut.push_back(c_out);
		if(!c) continue;
		int s = c->size();
		if(s<maskSize) 
		{
			c->SetCurrent();
			for(int i=0;i<s;i++)
			{
			short* p =  c->GetData();
			short* taget = imOut(p);
			*taget = 255;
			c_out->Add(taget);
			c->Shift();
			}
			c_out->Encycle();
			continue;
		}

		for(int k =0;k<s;k++)
		{
			int mask = maskSize/2;
			float* f = gauss+mask;
			float dx=0,dy =0;
			POINT p0;
			p0.x = (c->GetData()-image)%cx;
			p0.y = (c->GetData()-image)/cx;
			for(int m =1; m<= mask;m++)
			{
				POINT p1,p2;

				short* b1 = c->GetNthFromCurrent(m);
				p1.x = (b1-image)%cx;
				p1.y = (b1-image)/cx;
				short* b2 = c->GetNthFromCurrent(-m);
				p2.x = (b2-image)%cx;
				p2.y = (b2-image)/cx;
				dx+=2*f[m]*((p1.x + p2.x)/2. - (float)p0.x);
				dy+=2*f[m]*((p1.y + p2.y)/2. - (float)p0.y);
			}

			dx += dx>0 ? 0.5: -0.5;
			dy += dy>0 ? 0.5: -0.5;
			int x = dx;
			int y = dy;



			//x = p0.x + x;
			//y = p0.y + y;
			short* out = imageOut + p0.x + x + (p0.y + y)*cx;
			//if(!(x==0 && y==0)) 
			//{
			//	shift_and_fill(x,y, c->GetData());
			//}
			*out = *(c->GetData());
			c_out->Add(out);
			c->Shift();	
		}
		c_out->Encycle();
		redraw(c_out);
	}


	normalize();
}

void CContour::shift_and_fill(int x, int y, short* point)
{
	
	short mark;	
	int sx = (x<0? -1:1)*(x==0?0:1) ;
	int sy = (y<0? -1:1 )*(y==0?0:1);

	short* m = point +sx + cx*sy;
	if( m >= image && m<image+cx*cy)
	{
	 mark = *m==0? 255:0;
	}
	else mark=0;

	y=abs(y);
	x=abs(x);
	for(int j = 0; j<=y; j++)
		for(int i = 0; i<=x; i++)
		{
			/*if(j!=y && i!=x)
			{*/
			short* temp = imageOut+ (point-image) + i*sx + j*cx*sy;
			*temp = mark;
			/*}*/
		}

}
void CContour::redraw(Contour* c)
{
	int n = c->size();

	c->SetCurrent();
	short mark = *(c->GetData());
	while(n)
	{
		short* p1, *p2;
		p1=c->GetData();
		p2 = c->GetNthFromCurrent(1);

		int dx = (p2-imageOut)%cx-(p1-imageOut)%cx;
		int dy = (p2-imageOut)/cx-(p1-imageOut)/cx;
		while(dx || dy)
		{
			if(dx)
			{
	
				*(p1+dx + dy*cx)=mark;
				if(dx>0)dx--;
				else dx++;
			
			}

			if(dy)
			{

				*(p1+dx + dy*cx)=mark;
				if(dy>0)dy--;
				else dy++;
			
			}
		}
		c->Shift();
		n--;
	}
}

void CContour::repair(Contour* c)
{
	
	
	
	

}
void CContour::prefill()
{
	int nn = cOut.size();

	for(int i=0;i<nn;i++)
	{
		Contour* c = cOut[i];
		int m = c->size();
		int n =m;
		c->SetCurrent();
		while(n>0)
		{
			n--;
			while(c->size()>1 && c->Next()==c->GetData())
			{
				c->Shift();
				c->Delete();
				c->Encycle();
				n--;
			}
			c->Encycle();
			c->Shift();

		}

		while(m)
		{
			m--;
			short* left,* right,* curr;
			c->Encycle();
			curr = c->GetData();
			left = c->Previous();
			right = c->Next();
			int y0,yl,yr;
			y0 = (curr-imageOut)/cx;
			yl = (left-imageOut)/cx;
			yr = (right-imageOut)/cx;
			if((yl<=y0 && yr<=y0) || (yl>=y0 && yr>=y0))//max/min
			{
				if(yl==y0)
				*curr =255;
			}
			c->Shift();
		}
	}
}
void CContour::Fill()
{

	for(int j=0;j<cy;j++)
	{
		int i;

		if(*(imageOut+j*cx)==0)
		{
			fill_contour(1,imageOut+j*cx);
		}

		i=cx-1;

		if(*(imageOut+i+j*cx)==0)
		{
			fill_contour(1,imageOut+i+j*cx);
		}

	}

	for(int i =0;i<cx;i++)
	{
		int j;

		if(*(imageOut+i)==0)
		{
			fill_contour(1,imageOut+i);
		}

		j=cy-1;
		if(*(imageOut+i+j*cx)==0)
		{
			fill_contour(1,imageOut+i+j*cx);
		}
	}
	//after previous operations lefts only inner '0'-marked points
	//mark 'em
	for(j=0;j<cy;j++)
	{
		for(i=0;i<cx;i++)
		{
			if(*(imageOut+i+j*cx)==0)
			{

				if(i-1>=0) 
				{

					/*if(*(imageOut+i+j*cx-1)==255) {fill_contour(1,imageOut+i+j*cx);continue;} */

					fill_contour(*(imageOut+i+j*cx-1),imageOut+i+j*cx);

				}
			}
		}
	}


}



int CContour::fill_row(short* row, int begin, short fill)
{
	short mark = row[begin];

	for (int i =begin+1;i<cx;i++)
	{
		if(row[i]==mark)
		{
			for(int j=begin+1;j<i;j++)
			{
				if(row[j]<255) row[j]=fill;
			}
			return i;
		}
		else if(row[i]>255)  i = fill_row(row, i, (~fill) & 0xff);
		
	}
	return i;
}
short* CContour::imOut(short *p)//find corresponding pixel in imageOut
{
	return imageOut+(p-image);
}
short* CContour::im(short *p)//find corresponding pixel in image
{
	return image+(p-imageOut);
}

void CContour::GetRow(short*& row, int n)
{
	row = imageOut + cx*n;
	/*short mark;
	if(row[0]==1) mark=0;
	else mark=255;
	

	for(int i=0;i<cx-1;i++)
	{
		if()
		short t = row[i];
		if(t!=255)	row[i]=mark;
		if(t!=row[i+1])
		{	

			if(row[i+1]==1) mark=0;
			else if(t==255 && row[i+1]>255) mark=255;
			else mark = ~mark & 0xff;
		}		
	}
	row[cx-1]=mark;*/
}


bool CContour::Init(IBinaryImage* binImage, int maskSize)
{
float e = 2.73;

#ifdef _DEBUG
	init = true;
#endif 
	this->maskSize = (maskSize/2)*2+1;
	gauss = new float[this->maskSize];
	float *f =  gauss + maskSize/2; 
	for(int i=0;i<=maskSize/2;i++)
	{
		f[-i] = f[i] = 1./(i+1)/(i+1);
	}
	area = new POINT[this->maskSize];
	binImage->GetSizes(&cx,&cy);
	image = new short[cx*cy];
	imageOut = new short[cx*cy];
	if(!image) return false;
	for(int j=0;j<cy;j++)
	{
		byte* row;
		binImage->GetRow(&row, j, false);
		short* dest = image+cx*j;
		//short* destOut = imageOut + cx*j;
		for(int i=0;i<cx;i++)
		{

			/*destOut[i] = */dest[i] = row[i];
		}
	}
	memset(imageOut,0,cx*cy*sizeof(short));

	return true;
}
bool CContour::isPointInArea(short* p, short* areaP)
{
	return p==areaP-1 || p==areaP+1 || p==areaP-cx || p==areaP+cx || 
			p==areaP-cx-1 || p==areaP+cx-1 || p==areaP-cx+1 || p==areaP+cx+1 ;
}

void CContour::turn(int& i,int& j)
{
	int x=i,y=j;
	i = -y;
	j = x;
}
/*check_point return true if
					1) point connected to the current point - always
					2) point(i,j) belong to contour (has at least one empty neighbour) and has common with prePoint empty neighbours
					3) point not empty
					*/
bool CContour::check_point(int i,int j,short* prePoint)
{
	if(i<0 || i>=cx || j<0 || j>=cy) return false;
	short *temp = image+i+j*cx;
	short* neigh;

	if (*temp == 0) return false;//empty
	if(j-1>=0)
	{
		if(i-1>=0) 
		{
			neigh = temp-cx-1;
			if(*neigh ==0 && isPointInArea(neigh, prePoint))  return true;
		}
		{
			neigh = temp-cx;
			if(*neigh ==0 && isPointInArea(neigh, prePoint))  return true;
		}
		if(i+1<cx) 
		{
			neigh = temp-cx+1;
			if(*neigh ==0 && isPointInArea(neigh, prePoint))  return true;
		}
	}
	if(i-1>=0)
	{
		neigh = temp-1;
		if(*neigh ==0 && isPointInArea(neigh, prePoint))  return true;
	}
	if(i+1<cx) 
	{
		neigh = temp+1;
		if(*neigh ==0 && isPointInArea(neigh, prePoint))  return true;
	}
	if(j+1<cy)
	{
		if(i-1>=0) 
		{
			neigh = temp+cx-1;
			if(*neigh ==0 && isPointInArea(neigh, prePoint))  return true;
		}
		neigh = temp+cx;
		if(*neigh ==0 && isPointInArea(neigh, prePoint))  return true;
		if(i+1<cx) 
		{
			neigh = temp+cx+1;
			if(*neigh ==0 && isPointInArea(neigh, prePoint))  return true;
		}
	}



	//if both prePoint and temp are border points return true too
	if(isBorder(prePoint) && isBorder(temp)) return true;
	return false;
}

bool CContour::isBorder(short* p)
{
	int x=(p-image)%cx;
	int y=(p-image)/cx;
	return isBorder(x,y);
}
bool CContour::follow_contour(short* begin)
{
	short* point =0;
	short* prePoint = begin;
	bool end = false;
	int x,y;
	x = (begin-image)%cx;
	y = (begin-image)/cx;
	int i=1,j=0;//begin with left to right

	currentContour = new Contour;
	currentContour->Add(begin);
	marker++;
	contours.push_back(currentContour);
	while(!end)
	{	
	int i0=i=-i,j0=j=-j;		

		bool added = false;
		while(!added)
		{
			turn(i,j);
			if(check_point(x+i,y+j, prePoint))
			{
				short* temp = image+ x +i + cx*(y+j);
				if(prePoint==begin && *temp == marker) goto s;//if this way not already marked


				*temp = marker;

				x+=i;
				y+=j;

				prePoint = temp;
				currentContour->Add(prePoint);
				added=true;
				break;

			}
s:
			if(i==i0 && j==j0) {end=true; break;}//no more ways founded -> contour encycled
		}

	
	}
	
	//encycle contour
	if(currentContour->size()>1) currentContour->DeleteTail();
	currentContour->Encycle();
	return true;
	
}


void CContour::Build()
{
	CHECK_INIT
	for(int j=0;j<cy;j++)
	{
		short* row = image+j*cx;
		for(int i=0;i<cx;i++)
		{
			if(row[i]!=0 && row[i]<=255)
			{
				if (check_point(i,j,row+i))	follow_contour(row+i);
			}
		}

	}
	normalize();
	
}
bool CContour::zero(short* pOut)
{
	short *p = im(pOut);
	if(*p==0) return true;
	//int right =(p-image)%cx+1 ,bottom=(p-image)/cx+1;
	//if(right<cx)
	//	if(*(p+1)==0 && *(pOut+1)==0) return true;
	//if(bottom<cy)
	//	if(*(p+cx)==0 && *(pOut+cx)==0)return true;
	return false;

}
CContour::Contour* CContour::find_contour(short mark)
{
	if(mark<257) return 0;
	if(mark-257>=contours.size()) return 0;
	return contours[mark-257];
}

void CContour::fill_contour(short mark, short* begin)//working with imageOut!!
{
	queue<short*> points;
	points.push(begin);

	short* m = im(begin);
	if(*m==0) mark=1;


	//if(mark==255) {if(*m==0) mark=1;}
	//else if(mark!=1)
	//{
	//	/**/

	//currentContour = find_contour(mark);
	//if(currentContour==0) 
	//	return;
	//int n =currentContour->size();
	//while(n)
	//{
	//	
	//	int x,y,x1,y1,i,j;
	//	x = (currentContour->GetData()-image)%cx;
	//	y = (currentContour->GetData()-image)/cx;
	//	x1 = (currentContour->Next()-image)%cx;
	//	y1 = (currentContour->Next()-image)/cx;
	//	i = x1-x;
	//	j = y1-y;
	//	turn(i,j);
	//	if(x+i>=cx || y+j>=cy) continue;
	//	short* s = image + x+i +(y+j)*cx;
	//	if(*s ==0){mark=1; break;}
	//	if(*s==255) break;
	//	currentContour->Shift();
	//	n--;
	//}
	//}
	
	*begin = mark;
	short* p;
	short* low = imageOut, *high = imageOut+cx*cy;
	while(!points.empty())
	{
		 p = points.front();
		 points.pop();
		 int x,y;
		 x = (p-imageOut)%cx;
		 y = (p-imageOut)/cx;
		 if(y-1>=0)
		 {
			 if(*(p-cx)==0) {
				 points.push(p-cx);
				 *(p-cx)=mark;
			 }
			
		 }
		 if(y+1 < cy)
		 {
			if(*(p+cx)==0) {
				 points.push(p+cx);
				 *(p+cx)=mark;
			 }
		 }
		 if(x-1>=0)
		 {
			if(*(p-1)==0) {
				 points.push(p-1);
				 *(p-1)=mark;
			 }
		 }
		 if(x+1<cx)
		 {
			if(*(p+1)==0) {
				 points.push(p+1);
				 *(p+1)=mark;
			 }
		 }
	}	

}
// recursively remove contours point which are dead ended 
void _kir::CContour::normalize(void)
{
	int n = contours.size();
	for(int i=0;i<n;i++)
	{
		Contour* c = contours[i];
		c->SetCurrent();
		if(c->size()<=8)
		{
			continue;
		}
		int m = 0;
		while(1)
		{
			c->Shift();
			m++;
			short* p = 0;
			while(c->size()>0 && c->Next()==c->Previous())
			{
				p=c->GetData();
				c->Delete();
				c->Shift();
				*p=0;
			}
			if(c->Next()==c->GetData())
			{
				c->Shift();
				c->Delete();
			}
				
			if(m>=c->size()) break;
		}
	}
}

bool CSmoothContour::InvokeFilter()
{
	IBinaryImagePtr bin = GetDataArgument();
	IBinaryImagePtr binOut = GetDataResult();
	int nMaskSize = GetArgLong( "MaskSize" );

	int cx,cy;
	bin->GetSizes(&cx,&cy);
	binOut->CreateNew(cx,cy);
	CContour c;

	c.Init(bin,nMaskSize);
	c.Build();
	StartNotification(cy);
	c.Gauss();
	c.Fill();

	for(int j=0;j<cy;j++)
	{	

		byte* outRow;
		short *srcRow;
		binOut->GetRow(&outRow,j,false);

		c.GetRow(srcRow,j);
		for(int i=0;i<cx;i++)
		{
			outRow[i] = srcRow[i]>1?255:0;
		}
		Notify(j);	
	}
	
	return true;
}




