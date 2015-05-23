#include "stdafx.h"
#include "cleanframe.h"
#include "stdafx.h"
#include "CleanFrame.h"
#include "misc_utils.h"
#include <math.h>
#include "alloc.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "ImagePane.h"
#include "PointList.h"

#define MaxColor color(-1)

_ainfo_base::arg        CCleanFrameInfo::s_pargs[] = 
{       
	{"Mask",      szArgumentTypeInt, "7", true, false },
	{"Level",      szArgumentTypeInt, "8192", true, false },
	{"CurrentFrame",       szArgumentTypeImage, 0, true, true },
	{"PrevFrame",       szArgumentTypeImage, 0, true, true },
	{"Clear",    szArgumentTypeImage, 0, false, true },	
	//{"LowFreq",    szArgumentTypeImage, 0, false, true },	
	//{"HighFreq",    szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false },
};

/////////////////////////////////////////////////////////////////////////////
static void dilate(color **srcRows, color **dstRows, color **tmpRows, int cx, int cy, int mask)
{
#define CMP max
#include "dilate.inl"
#undef CMP
}

static void erode(color **srcRows, color **dstRows, color **tmpRows, int cx, int cy, int mask)
{
#define CMP min
#include "dilate.inl"
#undef CMP
}

static void smooth(color **srcRows, color **dstRows, int cx, int cy, int mask)
{
	int m = mask;
	int m1 = (mask-1)/2;
	int m2 = mask-m1;
	int mm = m*m;
	int *buf=new int[cx+m];

	int *b=buf+m1;
	for(int ix=0; ix<cx; ix++, b++) 
	{
		*b=0;
		for (int iy=0; iy<m; iy++)
		{
			if (iy-m1<0) 
				*b+=srcRows[0][ix];
			else
				*b+=srcRows[iy-m1][ix];
		}
	}
	for(ix=0; ix<m1; ix++) buf[ix]=buf[m1];
	for(ix=0; ix<m2; ix++) buf[m1+cx+ix]=buf[m1+cx-1];

	for(int iy=0; iy<cy; iy++)
	{
		int sum=0;
		for (int ix=0; ix<m; ix++)	sum+=buf[ix];
		for (ix=0; ix<cx; ix++)
		{
			dstRows[iy][ix] =sum/mm;
			int x1=ix-m1, x2=x1+m;
			if(x1<0) x1=0;
			if(x2>=cx) x2=cx-1;
			sum -= buf[x1+m1];
			sum += buf[x2+m1];
		}

		int y1=iy-m1, y2=y1+m;
		if(y1<0) y1=0;
		if(y2>=cy) y2=cy-1;
		int *b=buf+m1;
		for(ix=0; ix<cx; ix++, b++)
		{
			*b -= srcRows[y1][ix]++;
			*b += srcRows[y2][ix]++;
		}
		for(ix=0; ix<m1; ix++) buf[ix]=buf[m1];
		for(ix=0; ix<m2; ix++) buf[m1+cx+ix]=buf[m1+cx-1];
	}
	delete buf;
}

static bool CreateCompatible(IUnknownPtr ptrDst, IUnknownPtr ptrSrc)
{
	ICompatibleObjectPtr ptrCO( ptrDst );
	if( ptrCO == NULL ) return false;
	if( S_OK != ptrCO->CreateCompatibleObject( ptrSrc, NULL, 0 ) )
		return false;
	return true;
}

#define ALLOC_ROWS(name, cx,cy) \
	smart_alloc(name##Rows, color *, cy); \
	smart_alloc(name##Buf, color, cx*cy); \
	{ for(int y=0; y<cy; y++) name##Rows[y] = name##Buf+cx*y; }


/////////////////////////////////////////////////////////////////////////////
bool CCleanFrame::InvokeFilter()
{
	IImage3Ptr	ptrSrc( GetDataArgument( "CurrentFrame" ) );
	IImage3Ptr	ptrSrc2( GetDataArgument( "PrevFrame" ) );
	IImage3Ptr	ptrDst( GetDataResult( "Clear" ) );
	//IImage3Ptr	ptrLow( GetDataResult( "LowFreq" ) );
	//IImage3Ptr	ptrHigh( GetDataResult( "HighFreq" ) );
	
	if(ptrSrc == NULL || ptrDst == NULL )
		return false;

	long nMask = GetArgLong("Mask");
	nMask = max(3,nMask) | 1; // нечетное число
	long nLevel = GetArgLong("Level");

	if( !CreateCompatible(ptrDst, ptrSrc) ) return false;
	//if( !CreateCompatible(ptrLow, ptrSrc) ) return false;
	//if( !CreateCompatible(ptrHigh, ptrSrc) ) return false;

	int nPaneCount = ::GetImagePaneCount( ptrSrc );
	int cx,cy;
	ptrSrc->GetSize(&cx,&cy);

	StartNotification(cy,nPaneCount);

	smart_alloc(srcRows, color *, cy);
	smart_alloc(src2Rows, color *, cy);
	smart_alloc(dstRows, color *, cy);
	ALLOC_ROWS(low, cx,cy);
	ALLOC_ROWS(high, cx,cy);
	ALLOC_ROWS(low2, cx,cy);
	ALLOC_ROWS(high2, cx,cy);
	ALLOC_ROWS(highAvg, cx,cy);

	for( int nPane=0;nPane<nPaneCount;nPane++ )
	{
		for(int y=0; y<cy; y++)
		{
			ptrSrc->GetRow(y, nPane, &srcRows[y]);
			ptrSrc2->GetRow(y, nPane, &src2Rows[y]);
			ptrDst->GetRow(y, nPane, &dstRows[y]);
		}

		smooth( srcRows, lowRows, cx, cy, nMask);
		for(int y=0; y<cy; y++)
			for(int x=0; x<cx; x++)
				highRows[y][x] = min(65535,max(0,32768+srcRows[y][x]-lowRows[y][x]));

		smooth( src2Rows, low2Rows, cx, cy, nMask);
		for(int y=0; y<cy; y++)
			for(int x=0; x<cx; x++)
				high2Rows[y][x] = min(65535,max(0,32768+src2Rows[y][x]-low2Rows[y][x]));

		double s=0, s2=0, snew=0; // уровни ВЧ в текущей картинке и в прошлом кадре

		// для начала попробуем просто чистку ВЧ
		if(0)
		for(int y=0; y<cy; y++)
		{
			for(int x=0; x<cx; x++)
			{
				int h = highRows[y][x] - 32768;
				if(h<-nLevel) h += nLevel;
				else if(h>nLevel) h -=nLevel;
				else h=0;
				dstRows[y][x] = min(65535,max(0, lowRows[y][x] + h));
			}
		}

		// там, где low близки - можно осреднять high
		if(0)
		for(int y=0; y<cy; y++)
		{
			for(int x=0; x<cx; x++)
			{
				int d = int(lowRows[y][x]) - low2Rows[y][x];
				double f = fabs(double(d) / nLevel);
				f = min(1.0, f)/2;
				int h = highRows[y][x] - 32768;
				int h2 = high2Rows[y][x] - 32768;
				h = h2*f + h*(1-f);
				dstRows[y][x] = min(65535,max(0, lowRows[y][x] + h));
				s += (highRows[y][x]-32768.0) * (highRows[y][x]-32768.0);
				s2 += (high2Rows[y][x]-32768.0) * (high2Rows[y][x]-32768.0);
				snew += double(h) * h;
			}
		}
		s /= cx*cy;
		s2 /= cx*cy;
		snew /= cx*cy;

		// там, где low близки - ослабляем high
		if(0)
		for(int y=0; y<cy; y++)
		{
			for(int x=0; x<cx; x++)
			{
				int d = int(lowRows[y][x]) - low2Rows[y][x];
				double f = fabs(double(d) / nLevel);
				f = min(1.0, f);
				int h = highRows[y][x] - 32768;
				int h2 = high2Rows[y][x] - 32768;
				h = h*f;
				dstRows[y][x] = min(65535,max(0, lowRows[y][x] + h));
				s += (highRows[y][x]-32768.0) * (highRows[y][x]-32768.0);
				s2 += (high2Rows[y][x]-32768.0) * (high2Rows[y][x]-32768.0);
				snew += double(h) * h;
			}
		}
		s /= cx*cy;
		s2 /= cx*cy;
		snew /= cx*cy;

		// там, где low близки - можно осреднять high
		// + чистка
		if(0)
		for(int y=0; y<cy; y++)
		{
			for(int x=0; x<cx; x++)
			{
				int d = int(lowRows[y][x]) - low2Rows[y][x];
				double f = fabs(double(d) / 512);
				f = min(1.0, f)/2;
				int h = highRows[y][x] - 32768;
				if(h<-nLevel) h += nLevel;
				else if(h>nLevel) h -=nLevel;
				else h=0;
				int h2 = high2Rows[y][x] - 32768;
				h = h2*f + h*(1-f);
				//h *= 1.2;
				dstRows[y][x] = min(65535,max(0, lowRows[y][x] + h));
			}
		}

		// там, где low близки - можно осреднять high... для осреднения по множеству кадров в качестве второго аргумента берем Y1 вместо X1
		if(0)
		for(int y=0; y<cy; y++)
		{
			for(int x=0; x<cx; x++)
			{
				int d = int(lowRows[y][x]) - low2Rows[y][x];
				double f = fabs(double(d) / nLevel);
				f = min(1.0, f)*0.95;
				int h = highRows[y][x] - 32768;
				int h2 = high2Rows[y][x] - 32768;
				h = h2*f + h*(1-f);
				dstRows[y][x] = min(65535,max(0, lowRows[y][x] + h));
				s += (highRows[y][x]-32768.0) * (highRows[y][x]-32768.0);
				s2 += (high2Rows[y][x]-32768.0) * (high2Rows[y][x]-32768.0);
				snew += double(h) * h;
			}
		}
		s /= cx*cy;
		s2 /= cx*cy;
		snew /= cx*cy;

		// там, где low близки - осредняем high
		// полученный осредненный чистим "по яркости" по квадрату 9*9
		if(0)
		{
			for(int y=0; y<cy; y++)
			{
				for(int x=0; x<cx; x++)
				{
					int d = int(lowRows[y][x]) - low2Rows[y][x];
					double f = fabs(double(d) / nLevel);
					f = min(1.0, f)/2;
					int h = highRows[y][x] - 32768;
					int h2 = high2Rows[y][x] - 32768;
					h = h2*f + h*(1-f);
					highAvgRows[y][x] = 32768+h;
					dstRows[y][x] = min(65535,max(0, lowRows[y][x] + h));
				}
			}

			const int w = 7;
			for(int y=w; y<cy-w; y++)
			{
				for(int x=w; x<cx-w; x++)
				{
					int h = highAvgRows[y][x];
					int s0 = 1, s1 = h;
					int h0 = h - 20*256;
					int h1 = h + 20*256;
					for(int yy=-w; yy<=w; yy++)
					{
						for(int xx=-w; xx<=w; xx++)
						{
							int c = highAvgRows[y+yy][x+xx];
							if( h0<=c && c<=h1 )
							{
								s0 ++;
								s1 += c;
							}
						}
					}
					s1 /= s0;
					dstRows[y][x] = min(65535,max(0, lowRows[y][x]+s1-32768));
				}
			}
		}

		// TODO:
		// Фильтр: осреднять по значениям с близкими low и high - причем с нескольких кадров
		// Т.е. берем в текущей точке low, смотрим в окрестности low,
		// если близко - смотрим high, если и high близко - берем в осреднение.
		// затем повторяем ту же процедуру для прошлого кадра

		int w = 1;
		for(int y=0; y<cy; y++)
		{
			for(int x=0; x<cx; x++)
			{
				int hi = highRows[y][x];
				int lo = lowRows[y][x];
				int s0 = 0;
				double s1 = 0;
				w = 1;
				for(int y1=y-w; y1<=y+w; y1++)
				{
					if(y1<0 || y1>=cy) continue;
					for(int x1=x-w; x1<=x+w; x1++)
					{
						if(x1<0 || x1>=cx) continue;
						int lo2 = lowRows[y1][x1];
						if( abs(lo2-lo)>nLevel*2 ) continue;
						int hi2 = highRows[y1][x1];
						//if( abs(hi2-hi)>nLevel*4 ) continue;

						s0 ++;
						s1 += hi2;
					}
				}
				s0*=3; s1*=3;
				w = 2;
				for(int y1=y-w; y1<=y+w; y1++)
				{
					if(y1<0 || y1>=cy) continue;
					for(int x1=x-w; x1<=x+w; x1++)
					{
						if(x1<0 || x1>=cx) continue;
						int lo2 = low2Rows[y1][x1];
						if( abs(lo2-lo)>nLevel ) continue;
						int hi2 = high2Rows[y1][x1];
						//if( abs(hi2-hi)>nLevel*4 ) continue;

						s0 ++;
						s1 += hi2;
					}
				}
				s1 /= s0;
				dstRows[y][x] = min(65535,max(0, lowRows[y][x]+s1-32768));
			}
		}

		NextNotificationStage();
	}
	FinishNotification();

	return true;
}
