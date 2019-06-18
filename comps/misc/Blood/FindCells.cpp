#include "stdafx.h"
#include "FindCells.h"
#include "math.h"
#include <stdio.h>
#include "measure5.h"
#include "alloc.h"
#include "docview5.h"
#include "\vt5\awin\misc_list.h"
#include "bytemorpho.h"
#include "params.h"
#include "object5.h"

_ainfo_base::arg CFindCellsInfo::s_pargs[] =
{
	{"Img",       szArgumentTypeImage, 0, true, true },
	{"Binary",    szArgumentTypeBinaryImage, 0, false, true },
	{"FindCells",    szArgumentTypeObjectList, 0, false, true },
	{"TestOnly",    szArgumentTypeInt, 0, false, true },
	{0, 0, 0, false, false },
};

static void bbb(int line)
{
	FILE *f=fopen("FindCells.log","a");
	fprintf(f,">> Line %i\n", line);
	fflush(f);
	fclose(f);
}

//#define BBB bbb(__LINE__);
#define BBB

static inline int round(double x)
{
	return int(floor(x+0.5));
}

IUnknown * FindComponentByName(IUnknown *punkThis, const IID iid, LPCTSTR szName)
{
	_bstr_t strName(szName);
	_bstr_t strCompName;

	if (CheckInterface(punkThis, iid))
	{
		strCompName = ::GetName(punkThis);
		if (strCompName == strName)
		{
			punkThis->AddRef();
			return punkThis;
		}
	}

	IUnknown *punk = 0;
	if (CheckInterface(punkThis, IID_ICompManager))
	{
		ICompManagerPtr	sptr(punkThis);
		int nCount = 0;
		sptr->GetCount(&nCount);
		for (int i = 0; i < nCount; i++)
		{
			sptr->GetComponentUnknownByIdx(i, &punk);

			if (punk)
			{
				if (CheckInterface(punk, iid))
				{
					strCompName = ::GetName(punk);
					if (strCompName == strName)
						return punk;
				}
				punk->Release();
				punk = 0;
			}
		}
	}

	if (CheckInterface(punkThis, IID_IRootedObject))
	{
		IRootedObjectPtr	sptrR(punkThis);
		IUnknown	*punkParent = 0;

		sptrR->GetParent(&punkParent);

		punk = FindComponentByName(punkParent, iid, strName);

		if (punkParent)
			punkParent->Release();
	}

	return punk;
}

void CalcObject(IUnknown* punkCalc, IUnknown* punkImage, bool bSetParams, ICompManager *pGroupManager )
{
	if( !pGroupManager )
	{
		IUnknown *punk = FindComponentByName(GetAppUnknown(), IID_IMeasureManager, szMeasurement);
		if( !punk )return;
		ICompManagerPtr	ptrManager( punk );
		punk->Release();
		pGroupManager = ptrManager;
	}

	IDataObjectListPtr ptrObjectList;

	if( bSetParams )
	{
		INamedDataObject2Ptr ptrObject(punkCalc);
		IUnknown* punkParent = 0;
		ptrObject->GetParent(&punkParent);
		ptrObjectList = punkParent;
		if( punkParent )punkParent->Release();
		if( ptrObjectList == 0 )return;
	}

	IUnknownPtr sptr2;
	if(punkImage==0)
	{
		IMeasureObjectPtr ptrObject(punkCalc);
		if(ptrObject!=0)
		{
			ptrObject->GetImage(&sptr2);
			punkImage=sptr2;
		}
	}

	int	nCount;
	pGroupManager->GetCount( &nCount );
	for( int idx = 0; idx < nCount; idx++ )
	{
		IUnknown	*punk = 0;
		pGroupManager->GetComponentUnknownByIdx( idx, &punk );
		IMeasParamGroupPtr	ptrG( punk );
		if( punk )punk->Release();
		if( ptrG == 0 )continue;

		if( bSetParams )
			ptrG->InitializeCalculation( ptrObjectList );
		ptrG->CalcValues( punkCalc, punkImage );
		if( bSetParams )
			ptrG->FinalizeCalculation();
	}

	if( bSetParams )
		ptrObjectList->UpdateObject( punkCalc );
}

void ConditionalCalcObject(IUnknown* punkCalc, IUnknown* punkImage, bool bSetParams, ICompManager *pGroupManager )
{
	bool bRecalc = ::GetValueInt(GetAppUnknown(), "\\measurement", "DynamicRecalc", 1) == 1;
	if(bRecalc) CalcObject(punkCalc, punkImage, bSetParams, pGroupManager );
}

/////////////////////////////////////////////////////////////////////////////
CFindCells::CFindCells()
{
	m_SmoothSize=5;
	m_EvenSize=127;
	m_FindAreaSize=64;
	m_NucleiTolerance=24;
	m_NucleiAddBlack=0;
	m_PointBufSize=0;
	nInserted=0;
	px=py=NULL;
	CheckBufSize();
}

#define del(par) {if(par) {delete(par); par=0;}}

/////////////////////////////////////////////////////////////////////////////
CFindCells::~CFindCells()
{
	if(px) delete px;
	if(py) delete py;
}

IUnknown *CFindCells::DoGetInterface( const IID &iid )
{
	if( iid == IID_ILongOperation )	return (ILongOperation*)this;
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	return CAction::DoGetInterface( iid );
}

void CFindCells::CheckBufSize()
//проверить размер буфера под точки, если близки к пределу - увеличить.
{
	if(nInserted>m_PointBufSize-16)
	{
		int newPointBufSize=max(m_PointBufSize*2, 16);
		int *px1=new int[newPointBufSize];
		int *py1=new int[newPointBufSize];
		if(px) CopyMemory(px1,px,sizeof(int)*m_PointBufSize);
		if(py) CopyMemory(py1,py,sizeof(int)*m_PointBufSize);
		delete px;
		delete py;
		px=px1; py=py1;
		m_PointBufSize=newPointBufSize;
	}
}

void CFindCells::Smoothing( BYTE *p_arg, BYTE *p_res , int cx , int cy, int Mask )
{
	if(Mask<=1)
	{
		CopyMemory(p_res, p_arg, cx*cy);
		return;
	}
	
	int m=Mask, mm=m*m, m1=(m-1)/2, m2=m-m1;
	smart_alloc(buf, int, cx+m);
	
	int *b=buf+m1;
	for(int ix=0; ix<cx; ix++, b++)
	{
		*b=0;
		for (int iy=0; iy<m; iy++)
		{
			if (iy-m1<0)
				*b+=p_arg[ix];
			else
				*b+=p_arg[ix+(iy-m1)*cx];
		}
		//??? b++;
	}
	for(ix=0; ix<m1; ix++) buf[ix]=buf[m1];
	for(ix=0; ix<m2; ix++) buf[m1+cx+ix]=buf[m1+cx-1];
	
	BYTE *p_r=p_res;
	for(int iy=0; iy<cy; iy++)
	{
		int sum=0;
		for (int ix=0; ix<m; ix++)      sum+=buf[ix];
		for (ix=0; ix<cx; ix++)
		{
			*p_r++ =sum/mm;
			int x1=ix-m1, x2=x1+m;
			if(x1<0) x1=0;
			if(x2>=cx) x2=cx-1;
			sum -= buf[x1+m1];
			sum += buf[x2+m1];
		}
		
		int y1=iy-m1, y2=y1+m;
		if(y1<0) y1=0;
		if(y2>=cy) y2=cy-1;
		BYTE *p_a1=p_arg+y1*cx;
		BYTE *p_a2=p_arg+y2*cx;
		int *b=buf+m1;
		for(ix=0; ix<cx; ix++, b++)
		{
			*b -= *p_a1++;
			*b += *p_a2++;
		}
		for(ix=0; ix<m1; ix++) buf[ix]=buf[m1];
		for(ix=0; ix<m2; ix++) buf[m1+cx+ix]=buf[m1+cx-1];
	}
}

#define INSERTPOINT     \
{ \
	if(y>=9 && y<size.cy-9 && x>=9 && x<size.cx-9) \
{ \
	CheckBufSize(); \
	int i = nInserted++; \
	px[i]=x; py[i]=y; \
	dst[y][x]=fillColor; \
	colorSum+=src[y][x]; \
} \
}


#define CHECK \
{ \
	if(dst[y][x]<fillColor)  \
{ \
	if(src[y][x]<=c && src[y][x]>c0-gradLimit) INSERTPOINT;\
} \
}


void CFindCells::ExpandDarkArea(color **src, byte **dst, CSize size, int tolerance)
{
	int x0=0, y0=0;
	for(int i=nDone; i<nInserted; i++)
	{
		x0+=px[i];
		y0+=py[i];
	}
	x0 /= max(nInserted-nDone,1);
	y0 /= max(nInserted-nDone,1);

	while(nInserted>nDone)
	{
		int cur = nDone++;
		int y=py[cur], x=px[cur];

		if((y-y0)*(y-y0)+(x-x0)*(x-x0)>m_MaxCellRadius*m_MaxCellRadius)
		{
			m_bCellTooBig=true;
			continue;
		}

		if(y>=9 && y<size.cy-9 && x>=9 && x<size.cx-9)
		{
			int cBack=int(colorSumBack/nBackCount);
			int cFore=int(colorSum/(nInserted-nInserted0+m_NucleiAddBlack));
			int c0=src[y][x];
			int c=cFore+(cBack-cFore)*tolerance/100;
			c=max(c, c0);
			y--; CHECK; y++;
			x++; CHECK; x--;
			y++; CHECK; y--;
			x--; CHECK;     x++;
			
			//изредка будем проверять окрестности, чтобы
			//присоединить и отдельные сегменты
			if(x%2==0 && y%2==0)
			{
				int R=m_MinNucleiSplitter;
				if(y>=R && y<size.cy-R && x>=R && x<size.cx-R)
				{
					int c=cFore+(cBack-cFore)*tolerance/100;
					c=max(c, c0);
					y+=R; x+=R/2; CHECK; y-=R; x-=R/2;
					y+=R; x-=R/2; CHECK; y-=R; x+=R/2;
					y-=R; x+=R/2; CHECK; y+=R; x-=R/2;
					y-=R; x-=R/2; CHECK; y+=R; x+=R/2;
					x+=R; CHECK; x-=R;
					x-=R; CHECK; x+=R;
				}
			}
		}
	}
}

#define INSERTPOINT2    \
{ \
	if(y>=9 && y<size.cy-9 && x>=9 && x<size.cx-9) \
{ \
	CheckBufSize(); \
	int i = nInserted++; \
	px[i]=x; py[i]=y; \
	dst[y][x]=fillColor; \
	colorSumR+=src[y][x]; \
	colorSumG+=src[y+size.cy][x]; \
	colorSumB+=src[y+size.cy*2][x]; \
} \
}

#define ADDPOINT2       \
{ \
	nInserted0--; \
	dst[y][x]=fillColor; \
	colorSumR+=src[y][x]; \
	colorSumG+=src[y+size.cy][x]; \
	colorSumB+=src[y+size.cy*2][x]; \
}


#define max3(a,b,c) max(max(a,b),c)

//обычный по 1 пикселу
#define CHECK2 \
{ \
	if(dst[y][x]<fillColor)  \
{ \
	color r=src[y][x], g=src[y+size.cy][x], b=src[y+size.cy*2][x]; \
	int d1=max3(abs(r-backR1), abs(g-backG1), abs(b-backB1)); \
	int d2=max3(abs(r-backR2), abs(g-backG2), abs(b-backB2)); \
	int d =max3(abs(r-foreR), abs(g-foreG), abs(b-foreB)); \
	int d0=abs(backG1-backG2); \
	d=d*3/4; \
	if(d<d1 && d<d2) INSERTPOINT2; \
} \
}


//сходство с эритроцитами - по квадрату
#define CHECK2S \
{ \
	if(dst[y][x]<fillColor)  \
{ \
	color r=src[y][x], g=src[y+size.cy][x], b=src[y+size.cy*2][x]; \
	int d =max3(abs(r-foreR), abs(g-foreG), abs(b-foreB)); \
	int d0=abs(backG1-backG2); \
	d=d*3/4; \
	int d1=max3(abs(r-backR1), abs(g-backG1), abs(b-backB1)); \
	int d2=65535; \
	const int w=2; \
	for(int i=-w; i<=w; i++) \
{ \
	for(int j=-w; j<=w; j++) \
{ \
	color r=src[y+i][x+j], g=src[y+i+size.cy][x+j], b=src[y+i+size.cy*2][x+j]; \
	int d2_ij=max3(abs(r-backR2), abs(g-backG2), abs(b-backB2)); \
	if(d2_ij<d2) d2=d2_ij; \
} \
} \
	if(d<d1 && d<d2) INSERTPOINT2; \
} \
}

//сходство с эритроцитам - по верт. линии
#define CHECK2V \
{ \
	if(dst[y][x]<fillColor)  \
        { \
		color r=src[y][x], g=src[y+size.cy][x], b=src[y+size.cy*2][x]; \
		int d =max3(abs(r-foreR), abs(g-foreG), abs(b-foreB)); \
		int d0=abs(backG1-backG2); \
		d=d*3/4; \
		int d1=max3(abs(r-backR1), abs(g-backG1), abs(b-backB1)); \
		int d2=65535; \
		const int w=2; \
		for(int i=-w; i<=w; i++) \
                { \
				for(int j=0; j<=0; j++) \
                        { \
						color r=src[y+i][x+j], g=src[y+i+size.cy][x+j], b=src[y+i+size.cy*2][x+j]; \
						int d2_ij=max3(abs(r-backR2), abs(g-backG2), abs(b-backB2)); \
						if(d2_ij<d2) d2=d2_ij; \
} \
} \
	if(d<d1 && d<d2) INSERTPOINT2; \
} \
}

//сходство с эритроцитам - по гор. линии
#define CHECK2H \
{ \
	if(dst[y][x]<fillColor)  \
        { \
		color r=src[y][x], g=src[y+size.cy][x], b=src[y+size.cy*2][x]; \
		int d =max3(abs(r-foreR), abs(g-foreG), abs(b-foreB)); \
		int d0=abs(backG1-backG2); \
		d=d*3/4; \
		int d1=max3(abs(r-backR1), abs(g-backG1), abs(b-backB1)); \
		int d2=65535; \
		const int w=2; \
		for(int i=-0; i<=0; i++) \
                { \
				for(int j=w; j<=w; j++) \
                        { \
						color r=src[y+i][x+j], g=src[y+i+size.cy][x+j], b=src[y+i+size.cy*2][x+j]; \
						int d2_ij=max3(abs(r-backR2), abs(g-backG2), abs(b-backB2)); \
						if(d2_ij<d2) d2=d2_ij; \
                        } \
                } \
                if(d<d1 && d<d2) INSERTPOINT2; \
        } \
}

#define CHECKADD2 \
{ \
	if(dst[y][x]==0)  \
        { \
		color r=src[y][x], g=src[y+size.cy][x], b=src[y+size.cy*2][x]; \
		int d1=max3(abs(r-backR1), abs(g-backG1), abs(b-backB1)); \
		int d2=max3(abs(r-backR2), abs(g-backG2), abs(b-backB2)); \
		int d=abs(backG1-backG2)/2; \
		if(d<d1 && d<d2) ADDPOINT2; \
        } \
}


//              fprintf(f,"(%u,%u,%u) - (%u,%u,%u) : %u\n", r,g,b, foreR, foreG, foreB, d<d1 && d<d2);

static RECT ClipRect={0,0,1024,1024};

static void Line(byte **dst, int x0, int y0, int x1, int y1, byte c)
{
	x0=min(max(x0,ClipRect.left),ClipRect.right-1);
	x1=min(max(x1,ClipRect.left),ClipRect.right-1);
	y0=min(max(y0,ClipRect.top),ClipRect.bottom-1);
	y1=min(max(y1,ClipRect.top),ClipRect.bottom-1);
	
	int L=max(abs(x1-x0),abs(y1-y0));
	if(L==0) L=1;
	int dx=(x1-x0)*65536/L, dy=(y1-y0)*65536/L;
	int x=x0*65536, y=y0*65536;
	for(int i=0; i<=L; i++)
	{
		dst[y/65536][x/65536]=c;
		y+=dy; x+=dx;
	}
}

static void CircleMin(int *src, int *dst, int dirs, int size)
{
	//smooth
	for(int i=0; i<dirs; i++)
	{
		int m=src[i];
		for(int j=-size; j<=size; j++)
		{
			int i1=(i+j+dirs)%dirs;
			if(src[i1]<m) m=src[i1];
		}
		dst[i]=m;
	}
}

static void CircleMax(int *src, int *dst, int dirs, int size)
{
	//smooth
	for(int i=0; i<dirs; i++)
	{
		int m=src[i];
		for(int j=-size; j<=size; j++)
		{
			int i1=(i+j+dirs)%dirs;
			if(src[i1]>m) m=src[i1];
		}
		dst[i]=m;
	}
}

static void CircleAvg(int *src, int *dst, int dirs, int size)
{
	//smooth
	for(int i=0; i<dirs; i++)
	{
		int s=0;
		for(int j=-size; j<=size; j++)
		{
			int i1=(i+j+dirs)%dirs;
			s+=src[i1];
		}
		dst[i]=s/(size*2+1);
	}
}

static void CircleMed(int *src, int *dst, int dirs, int size)
{
	//median filter
	smart_alloc(c,int,size*2+1);
	for(int i=0; i<dirs; i++)
	{
		for(int j=-size; j<size; j++)
		{
			int i1=(i+j+dirs)%dirs;
			c[j+size]=src[i1];
		}
		//самый дурацкий способ вычисления медианы - временно
		for(int k=0; k<=size*2; k++)
		{
			for(int k1=k+1; k1<=size*2; k1++)
				if(c[k1]>c[k])
				{
					int t=c[k1]; c[k1]=c[k]; c[k]=t;
				}
		}
		dst[i]=c[size];
	}
}

static void CircleClose2(int *src, int *dst, int dirs, int size)
{
	//убрать мусор...
	for(int i=0; i<dirs; i++)
	{
		dst[i]=src[i];
		for(int j=1; j<size; j++)
		{
			int i1=(i+j)%dirs;
			int i2=(i-j+dirs)%dirs;
			int c2=(src[i1]+src[i2])/2;
			dst[i]=max(dst[i],c2);
		}
	}
}

static void FillPoly(byte **dst, int *x, int*y, int n, byte color)
{
	smart_alloc(x0, int, ClipRect.bottom);
	for(int i=0; i<ClipRect.bottom; i++) x0[i]=-1;
	
	int lastDir=0;
	int x1=x[n-1], y1=y[n-1];
	for(i=0; i<n; i++)
	{
		int xx1=x1*65536, dxx;
		if(y1!=y[i]) dxx=(x[i]-x1)*65536/abs(y[i]-y1);
		while(y1!=y[i])
		{
			x1=xx1/65536;
			int dy= y[i]>y1 ? 1 : -1;
			if(x0[y1]!=-1)
			{
				int xx1=min(x1,x0[y1]);
				int xx2=max(x1,x0[y1]);
				for(int fillx=xx1; fillx<=xx2; fillx++)
					if(dst[y1][fillx]!=2) dst[y1][fillx] ^= color;
					x0[y1]=-1;
			}
			else x0[y1]=x1;
			if(dy!=lastDir && lastDir!=0)
			{
				if(x0[y1]==-1) x0[y1]=x1;
				else x0[y1]=-1;
			}
			lastDir=dy;
			y1+=dy;
			xx1+=dxx;
		}
		x1=x[i];
	}
}

void CFindCells::SelectByRadius(color **src, byte **dst, CSize size, int tolerance, RECT *dstRect)
{
	//FILE *f=fopen("select.log","a");
	int x0=0, y0=0;
	for(int i=nDone; i<nInserted; i++)
	{
		x0+=px[i];
		y0+=py[i];
	}
	x0 /= nInserted-nDone;
	y0 /= nInserted-nDone;

	m_ptNucleusCenter = _point(x0,y0);
	
	int cBack=int(colorSumBack/nBackCount);
	int cFore=int(colorSum/(nInserted-nInserted0));
	int foreR=int(colorSumR/(nInserted-nInserted0));
	int foreG=int(colorSumG/(nInserted-nInserted0));
	int foreB=int(colorSumB/(nInserted-nInserted0));
	
	int dirs=256;
	//double PI=3.1415926535;
	smart_alloc(cs,int,dirs);
	smart_alloc(sn,int,dirs);
	
	for(i=0; i<dirs; i++)
	{
		cs[i]=int(cos(i*2*PI/dirs)*65536);
		sn[i]=int(sin(i*2*PI/dirs)*65536);
	}
	
	smart_alloc(rad,int,dirs); //радиусы клетки
	for(i=0; i<dirs; i++) rad[i]=40;
	smart_alloc(rad2,int,dirs);
	
	//debug - for edit&continue
	CircleMin(rad, rad2, dirs, 3);
	CircleMax(rad, rad2, dirs, 3);
	CircleMed(rad, rad2, dirs, 3);
	CircleAvg(rad, rad2, dirs, 3);
	
	int LineColor=3;
	for(int count=0; count<20; count++)
	{
		LineColor=3;
		CircleMax(rad, rad2, dirs, 4);
		CircleMin(rad2, rad, dirs, 4);
		CircleMin(rad, rad2, dirs, 16);
		CircleMax(rad2, rad, dirs, 16);
		
		int dfb1=max3(abs(foreR-backR1), abs(foreG-backG1), abs(foreB-backB1));
		int dfb2=max3(abs(foreR-backR2), abs(foreG-backG2), abs(foreB-backB2));
		int dfb=min(dfb1,dfb2);
		
		for(i=0; i<dirs; i++)
		{
			rad[i]-=8;
			int badCount=0;
			int radGood=rad[i];
			while(badCount<4)
			{
				int x=x0+rad[i]*cs[i]/65536, y=y0+rad[i]*sn[i]/65536;
				if(rad[i]>=m_MaxCellRadius) m_bCellTooBig=true;
				if(x>0 && y>0 && x<size.cx-1 && y<size.cy-1 && rad[i]<m_MaxCellRadius)
				{
					if(dst[y][x]==2) badCount=0;
					else
					{
						color r=src[y][x], g=src[y+size.cy][x], b=src[y+size.cy*2][x];
						int d1=max3(abs(r-backR1), abs(g-backG1), abs(b-backB1));
						int d2=max3(abs(r-backR2), abs(g-backG2), abs(b-backB2));
						int dr=(g-backG1)*(backR2-backR1)/(backG2-backG1);
						int db=(g-backG1)*(backB2-backB1)/(backG2-backG1);
						int d3=max(abs(dr),abs(db));
						int d =max3(abs(r-foreR), abs(g-foreG), abs(b-foreB));
						int d0 = abs(backG2-backG1);
						d1=min(d1,d2);
						//if(g<=backG1 && g>=backG2) d1=min(d1,d3);
						d1=d1*100/tolerance;
						if(d<d1 || d0<d1) badCount--;
						if(d>d1 && d0>d1) badCount++;
						//if(d>d2) badCount=0;
					}
				}
				else badCount=1000;
				//if (dst[y][x]==0) dst[y][x]=1;
				rad[i]++;
				if(badCount<0) badCount=0;
				if(badCount<=0) radGood=rad[i];
			}
			rad[i]=radGood;
		}
	}
	
	CircleMax(rad, rad2, dirs, 4);
	CircleMin(rad2, rad, dirs, 4);
	CircleMin(rad, rad2, dirs, 8);
	CircleMax(rad2, rad, dirs, 8);
	CircleAvg(rad, rad2, dirs, 2);
	CircleAvg(rad2, rad, dirs, 2);
	
	/*
	{
	LineColor=6;
	int x2=x0+rad[dirs-1]*cs[dirs-1]/65536;
	int y2=y0+rad[dirs-1]*sn[dirs-1]/65536;
	for(i=0; i<dirs; i++)
	{
	int x=x0+rad[i]*cs[i]/65536, y=y0+rad[i]*sn[i]/65536;
	Line(dst, x,y, x2,y2, LineColor - (i&1));
	x2=x; y2=y;
	}
	}
	*/
	
	//построим минимальный прямоугольник, содержащий клетку
	{
		for(i=0; i<dirs; i++)
		{
			int x=x0+rad[i]*cs[i]/65536, y=y0+rad[i]*sn[i]/65536;
			if(x<dstRect->left) dstRect->left=x;
			if(y<dstRect->top) dstRect->top=y;
			if(x>dstRect->right) dstRect->right=x;
			if(y>dstRect->bottom) dstRect->bottom=y;
		}
		dstRect->right++;
		dstRect->bottom++;
	}
	
	//а теперь заполним многоугольник
	{
		smart_alloc(x,int,dirs);
		smart_alloc(y,int,dirs);
		for(int i=0; i<dirs; i++)
		{
			x[i]=x0+rad[i]*cs[i]/65536;
			y[i]=y0+rad[i]*sn[i]/65536;
			if(x[i]<ClipRect.left) x[i]=ClipRect.left;
			if(y[i]<ClipRect.top) y[i]=ClipRect.top;
			if(x[i]>=ClipRect.right) x[i]=ClipRect.right-1;
			if(y[i]>=ClipRect.bottom) y[i]=ClipRect.bottom-1;
		}
		FillPoly(dst, x, y, dirs, 1);
	}
	
	//fclose(f);
}

void CFindCells::SelectByRadius2(color **src, byte **dst, CSize size, int tolerance, RECT *dstRect)
//то же самое, что SelectByRadius, но не по лучам, а 
{
	int x0=0, y0=0;
	for(int i=nDone; i<nInserted; i++)
	{
		x0+=px[i];
		y0+=py[i];
	}
	x0 /= nInserted-nDone;
	y0 /= nInserted-nDone;
	
	int cBack=int(colorSumBack/nBackCount);
	int cFore=int(colorSum/(nInserted-nInserted0));
	int foreR=int(colorSumR/(nInserted-nInserted0));
	int foreG=int(colorSumG/(nInserted-nInserted0));
	int foreB=int(colorSumB/(nInserted-nInserted0));
	
	int dirs=256;
	//double PI=3.1415926535;
	smart_alloc(cs,int,dirs);
	smart_alloc(sn,int,dirs);
	
	for(i=0; i<dirs; i++)
	{
		cs[i]=int(cos(i*2*PI/dirs)*65536);
		sn[i]=int(sin(i*2*PI/dirs)*65536);
	}
	
	smart_alloc(rad,int,dirs); //радиусы клетки
	for(i=0; i<dirs; i++) rad[i]=40;
	smart_alloc(rad2,int,dirs);
	
	//debug - for edit&continue
	CircleMin(rad, rad2, dirs, 3);
	CircleMax(rad, rad2, dirs, 3);
	CircleMed(rad, rad2, dirs, 3);
	CircleAvg(rad, rad2, dirs, 3);
	
#define ip ((i+1)%dirs)
#define im ((i+dirs-1)%dirs)

	for(int count=0; count<200; count++)
	{
		for(i=0; i<dirs; i++)
			rad2[i]=(rad[ip]+rad[im])/2;
		
		int dfb1=max3(abs(foreR-backR1), abs(foreG-backG1), abs(foreB-backB1));
		int dfb2=max3(abs(foreR-backR2), abs(foreG-backG2), abs(foreB-backB2));
		int dfb=min(dfb1,dfb2);
		
		for(i=0; i<dirs; i++)
		{
			int x=x0+rad[i]*cs[i]/65536, y=y0+rad[i]*sn[i]/65536;
			if(x>0 && y>0 && x<size.cx-1 && y<size.cy-1)
			{
				if(dst[y][x]==2 || rad[i]<rad2[i]-1) rad[i]++;
				else if (rad[i]<rad2[i]+2)
				{
					color r=src[y][x], g=src[y+size.cy][x], b=src[y+size.cy*2][x];
					int d1=max3(abs(r-backR1), abs(g-backG1), abs(b-backB1));
					int d2=max3(abs(r-backR2), abs(g-backG2), abs(b-backB2));
					int dr=(g-backG1)*(backR2-backR1)/(backG2-backG1);
					int db=(g-backG1)*(backB2-backB1)/(backG2-backG1);
					int d3=max(abs(dr),abs(db));
					int d =max3(abs(r-foreR), abs(g-foreG), abs(b-foreB));
					int d0 = abs(backG2-backG1);
					d1=min(d1,d2);
					d1=d1*100/tolerance;
					if(d<d1 || d0<d1) rad[i]++;
				}
			}
		}
	}
	
	//построим минимальный прямоугольник, содержащий клетку
	{
		for(i=0; i<dirs; i++)
		{
			int x=x0+rad[i]*cs[i]/65536, y=y0+rad[i]*sn[i]/65536;
			if(x<dstRect->left) dstRect->left=x;
			if(y<dstRect->top) dstRect->top=y;
			if(x>dstRect->right) dstRect->right=x;
			if(y>dstRect->bottom) dstRect->bottom=y;
		}
		dstRect->right++;
		dstRect->bottom++;
	}
	
	//а теперь заполним многоугольник
	{
		smart_alloc(x,int,dirs);
		smart_alloc(y,int,dirs);
		for(int i=0; i<dirs; i++)
		{
			x[i]=x0+rad[i]*cs[i]/65536;
			y[i]=y0+rad[i]*sn[i]/65536;
			if(x[i]<ClipRect.left) x[i]=ClipRect.left;
			if(y[i]<ClipRect.top) y[i]=ClipRect.top;
			if(x[i]>=ClipRect.right) x[i]=ClipRect.right-1;
			if(y[i]>=ClipRect.bottom) y[i]=ClipRect.bottom-1;
		}
		FillPoly(dst, x, y, dirs, 1);
	}
	
	//fclose(f);
}

void CFindCells::ExpandSelectedArea(color **src, byte **dst, CSize size, int tolerance)
{
	//FILE *f=fopen("select.log","a");
	int x0=0, y0=0;
	for(int i=nDone; i<nInserted; i++)
	{
		x0+=px[i];
		y0+=py[i];
	}
	x0 /= nInserted-nDone;
	y0 /= nInserted-nDone;
	
	while(nInserted>nDone)
	{
		int cur = nDone++;
		int y=py[cur], x=px[cur];
		if((y-y0)*(y-y0)+(x-x0)*(x-x0)>m_MaxCellRadius*m_MaxCellRadius)
			continue;
		
		if(y>=5 && y<size.cy-5 && x>=5 && x<size.cx-5)
		{
			int cBack=int(colorSumBack/nBackCount);
			int cFore=int(colorSum/(nInserted-nInserted0));
			int foreR=int(colorSumR/(nInserted-nInserted0));
			int foreG=int(colorSumG/(nInserted-nInserted0));
			int foreB=int(colorSumB/(nInserted-nInserted0));
			
			int c=cFore+(cBack-cFore)*tolerance/100;
			int c0=src[y][x];
			c=max(c, c0);
			
			y--; CHECK2H; y++;
			x++; CHECK2V; x--;
			y++; CHECK2V; y--;
			x--; CHECK2H; x++;
			//fprintf(f,"%10u %10u   %5u %5u\n", nInserted, nDone, x,y);
			//fflush(f);
		}
	}
	//fclose(f);
}

bool CFindCells::SelectCell(color **src, byte **dst, int x0, int y0, CSize size, RECT *dstRect)
{
	// 26.07.2006 build 108
    // Для ловли прилегающих к краю клеток - если опорная точка на самом краю, отодвинемся.
	// По хорошему бы надо искать рядом темную, но и так сойдет - все равно эта клетка убьется,
	// как лежащая на краю, и Струй сдвинется так, чтобы она попала в центр.
	x0 = max(9, min(size.cx-10, x0));
	y0 = max(9, min(size.cy-10, y0));

	dstRect->left=x0;
	dstRect->top=y0;
	dstRect->right=x0;
	dstRect->bottom=y0;
	
	nInserted=0; //сколько точек добавлено в список
	nInserted0=0; //сколько точек добавлено в список
	nDone=0; //сколько точек обработано
	colorSum=0; //сумма цветов по добавленным точкам
	colorSumBack=0; //сумма цветов по добавленным точкам
	nBackCount=0;
	m_bCellTooBig=false;
	
	ClipRect.left=0;
	ClipRect.top=0;
	ClipRect.right=size.cx;
	ClipRect.bottom=size.cy;

	BBB

	//откалибруем фон (приблизительно)
	int xx0=min(max(x0,m_EvenSize),size.cx-1-m_EvenSize)-m_EvenSize;
	int yy0=min(max(y0,m_EvenSize),size.cy-1-m_EvenSize)-m_EvenSize;
	for(int i=0; i<1000; i++)
	{
		int xx,yy;
		xx=xx0+rand()%(m_EvenSize*2); yy=yy0+rand()%(m_EvenSize*2);
		int c1=src[yy][xx];
		xx=xx0+rand()%(m_EvenSize*2); yy=yy0+rand()%(m_EvenSize*2);
		int c2=src[yy][xx];
		if(c2>c1) c1=c2;
		xx=xx0+rand()%(m_EvenSize*2); yy=yy0+rand()%(m_EvenSize*2);
		c2=src[yy][xx];
		if(c2>c1) c1=c2;
		colorSumBack+=c1;
		nBackCount++;
	}

	BBB

	int x=x0, y=y0;
	src+=size.cy;
	INSERTPOINT; x++;
	INSERTPOINT; y++;
	INSERTPOINT; x--;
	INSERTPOINT;
	src-=size.cy;
	
	gradLimit=50*256;
	fillColor=2;
	ExpandDarkArea(src+size.cy, dst, size, m_NucleiTolerance);
	if(nInserted<m_MinNucleiArea)
	{
		for(int i=0; i<nInserted; i++)
		{
			dst[py[i]][px[i]]=0;
		}
		return false;
	}

	if(m_TestOnly)
	{ // надо только проверить, и ядро вроде засекли
        return true;
	}

	BBB

	int x_min=px[0], y_min=py[0], x_max=px[0], y_max=py[0];
	{
		for(int i=0; i<nInserted; i++)
		{
			if(px[i]<x_min) x_min=px[i];
			if(px[i]>x_max) x_max=px[i];
			if(py[i]<y_min) y_min=py[i];
			if(py[i]>y_max) y_max=py[i];
		}
	}
	y_max++; x_max++;
	
	y_min-=5; x_min-=5; //раздвинем прямоугольник на 5 пикселов
	y_max+=5; x_max+=5;
	
	if(x_min<0) x_min=0;
	if(y_min<0) y_min=0;
	if(x_max>size.cx) x_max=size.cx;
	if(y_max>size.cy) y_max=size.cy;
	
	smart_alloc(dst2, byte*, y_max-y_min);
	{for(int y=y_min; y<y_max; y++)
		dst2[y-y_min]=dst[y]+x_min;}
	
	DilateColor(dst2, x_max-x_min, y_max-y_min, m_NucleiCloseSize, 2,2,0);
	DilateColor(dst2, x_max-x_min, y_max-y_min, m_NucleiCloseSize, 0,0,2);
	DilateColor(dst2, x_max-x_min, y_max-y_min, m_NucleiCloseSize, 0,0,2);
	DilateColor(dst2, x_max-x_min, y_max-y_min, m_NucleiCloseSize, 2,2,0);

	BBB

	::DeleteSmallSegments(dst2, x_max-x_min, y_max-y_min, 12);

	BBB

	nDone=0;
	int cBack=int(colorSumBack/nBackCount);
	int cFore=int(colorSum/nInserted);
	int c=(cFore+cBack)/2;
	nInserted0=nInserted;//-int(sqrt(nInserted)*3)-1;
	//colorSum=c*(nInserted-nInserted0);
	gradLimit=50*256;
	colorSumR=0;
	colorSumG=0;
	colorSumB=0;
	
	int n=nInserted;
	fillColor=1;
	for(i=0; i<n; i++)
	{
		int x=px[i], y=py[i];
		y-=2; CHECKADD2; y+=2;
		x+=2; CHECKADD2; x-=2;
		y+=2; CHECKADD2; y-=2;
		x-=2; CHECKADD2; x+=2;
	}

	BBB

	nInserted0=nInserted;
	colorSumR=0;
	colorSumG=0;
	colorSumB=0;
	fillColor=1;
	for(i=0; i<n; i++)
	{
		int x=px[i], y=py[i];
		y-=8; CHECKADD2; y+=8;
		x+=8; CHECKADD2; x-=8;
		y+=8; CHECKADD2; y-=8;
		x-=8; CHECKADD2; x+=8;
	}
	
	//уберем за собой мусор
	for(y=0; y<size.cy; y++)
	{
		for(int x=0; x<size.cx; x++)
		{
			if(dst[y][x]==1) dst[y][x]=0;
		}
	}

	BBB

	int stability=100;
	nInserted0=nInserted-(nInserted-nInserted0)*stability;
	colorSumR*=stability;
	colorSumG*=stability;
	colorSumB*=stability;
	
	fillColor=1;
	int foreR=int(colorSumR/(nInserted-nInserted0));
	int foreG=int(colorSumG/(nInserted-nInserted0));
	int foreB=int(colorSumB/(nInserted-nInserted0));
	
	for(i=0; i<n; i++)
	{
		int x=px[i], y=py[i];
		y-=5; CHECK2; y+=5;
		x+=5; CHECK2; x-=5;
		y+=5; CHECK2; y-=5;
		x-=5; CHECK2; x+=5;
	}
	
	//уберем за собой мусор
	for(y=0; y<size.cy; y++)
	{
		for(int x=0; x<size.cx; x++)
		{
			if(dst[y][x]==1) dst[y][x]=0;
		}
	}

	BBB

	fillColor=1;
	SelectByRadius(src, dst, size, 100-m_CitoplasmaTolerance, dstRect);
	//ExpandSelectedArea(src, dst, size, m_CitoplasmaTolerance);
	//DeleteSmallSegments(dst, size);
	//DilateColor(dst, size.cx, size.cy, m_NucleiCloseSize, 1,1,0);
	//DilateColor(dst, size.cx, size.cy, m_NucleiCloseSize, 0,0,1);

	BBB

	return(true);
}

#define INSPOINT        \
{ \
	CheckBufSize(); \
	int i = nInserted++; \
	px[i]=x; py[i]=y; \
}

void CFindCells::DeleteSmallSegments(byte **dst, CSize size)
{
	int minX=size.cx, maxX=-1, minY=size.cy, maxY=-1;
	int x0,y0;
	for(int i=0; i<nInserted; i++)
	{
		if(px[i]<minX) {minX=px[i]; y0=py[i];}
		if(px[i]>maxX) maxX=px[i];
		if(py[i]<minY) minY=py[i];
		if(py[i]>maxY) maxY=py[i];
	}
	x0=minX-1;
	int x=x0, y=y0;
	int dx=0, dy=-1;
	int q=0;
	nInserted0=nInserted;
	do      {
		INSPOINT;
		int qq=0;
		while(dst[y+dy][x+dx]>=fillColor)
		{
			int t=dx; dx=dy; dy=-t;
			qq++;
			if(qq>4)
			{
				q=65536;
				break;
			}
		}
		x+=dx; y+=dy;
		int t=dx; dx=-dy; dy=t;
		q++; if (q>32768 ) break;
	} while(x!=x0 || y!=y0);
	if(q>32768) return;
	
	smart_alloc(lx0, int, size.cy);
	for(y=0; y<size.cy; y++) lx0[y]=-1;
	
	int lastY=py[nInserted0];
	for(i=nInserted0+1; i<nInserted; i++)
	{
		if(py[i]==lastY) continue;
		
		
	}
}


void CFindCells::LoadIni()
{
	char section[]="\\FindCells";
	
	long nCounter = GetValueInt(GetAppUnknown(), "\\Binary", "IndexedColorsCount", 1);
	
	m_SmoothSize=GetValueInt(GetAppUnknown(),section, "SmoothSize", 5);
	m_EvenSize=GetValueInt(GetAppUnknown(),section, "EvenSize", 127);
	m_FindAreaSize=GetValueInt(GetAppUnknown(),section, "FindAreaSize", 64);
	m_FindAreaSize=max(12, m_FindAreaSize);
	m_FindThreshold = GetValueInt(GetAppUnknown(),section, "FindThreshold",60);
	m_bShowCross = GetValueInt(GetAppUnknown(),section, "ShowCross",0)!=0;

	m_NucleiTolerance=GetValueInt(GetAppUnknown(),section, "NucleiTolerance", 20);
	m_NucleiAddBlack=GetValueInt(GetAppUnknown(),section, "NucleiAddBlack", 0);
	m_CitoplasmaTolerance=GetValueInt(GetAppUnknown(),section, "CitoplasmaTolerance", 25);
	if(m_CitoplasmaTolerance<1) m_CitoplasmaTolerance=1;
	if(m_CitoplasmaTolerance>99) m_CitoplasmaTolerance=99;
	m_MinNucleiArea=GetValueInt(GetAppUnknown(),section, "MinNucleiArea", 100);
	m_MinNucleiSplitter=GetValueInt(GetAppUnknown(),section, "MinNucleiSplitter",10);
	m_NucleiCloseSize=GetValueInt(GetAppUnknown(),section, "NucleiCloseSize",5);
	if(m_NucleiCloseSize%2 == 0) m_NucleiCloseSize = m_NucleiCloseSize-1;
	m_NucleiCloseSize = max(1,m_NucleiCloseSize);
	SetValue(GetAppUnknown(),section, "NucleiCloseSize",long(m_NucleiCloseSize));

	m_MaxCellRadius=GetValueInt(GetAppUnknown(),section, "MaxCellRadius",150);
 	m_CellCircleRadius=GetValueInt(GetAppUnknown(),section, "CellCircleRadius",80);
 	m_FramePosX=GetValueInt(GetAppUnknown(),section, "FramePosX",0);
 	m_FramePosY=GetValueInt(GetAppUnknown(),section, "FramePosY",0);
	m_MinCellDistance=GetValueInt(GetAppUnknown(),section, "MinCellDistance",100);
}

const int NC=256;

void CFindCells::CalcHist(color **src, int cx, int cy, int *hist)
//в hist должно быть NC свободных ячеек
{
	ZeroMemory(hist, NC*sizeof(int));
	for(int y=0; y<cy; y++)
	{
		color *pSrc=src[y];
		for(int x=0; x<cx; x++)
		{
			hist[(*pSrc++)/(65536/NC)]++;
		}
	}
}

void CFindCells::CalcBack2(int *hist, color &back1, color &back2, color &threshold)
{
	double sm=0.5; //размажем гистограмму
	smart_alloc(h, double, NC);
	h[0]=0;
	for     (int i=1; i<NC; i++) h[i]=hist[i]*(1-sm)+h[i-1]*sm;
	for     (i=NC-2; i>=0; i--) h[i]=h[i]*(1-sm)+h[i+1]*sm;
	
	back1=0; back2=0;
	for     (i=0; i<NC; i++) if(h[i]>h[back1]) back1=i;
	for     (i=0; i<NC; i++) if(h[i]*abs(i-back1)>h[back2]*abs(back2-back1)) back2=i;
	
	if (back1<back2)
	{ color temp=back1; back1=back2; back2=temp; }
	
	threshold=back2;
	for(i=back2; i<back1; i++)
	{
		if(h[i]<h[threshold]) threshold=i;
	}
	threshold=(back1+back2*3)/4;
	
	back1*=(65536/NC); back2*=(65536/NC);
	threshold*=(65536/NC);
}

#define col(x) color(min(max((x),0),65535))

void CFindCells::Calibrate(color **src, byte **bin, int cx, int cy)
{
	//--------- Калибровка ---------
	const int NC=256;
	smart_alloc(hist_r, int, NC);
	smart_alloc(hist_g, int, NC);
	smart_alloc(hist_b, int, NC);
	
	//CalcHist(src+cy*0, cx, cy, hist_r);
	CalcHist(src+cy*1, cx, cy, hist_g);
	//CalcHist(src+cy*2, cx, cy, hist_b);
	
	//CalcBack2(hist_r, backR1, backR2);
	//CalcBack2(hist_g, backG1, backG2);
	//CalcBack2(hist_b, backB1, backB2);
	//return;
	
	color th12;
	color back1, back2;
	CalcBack2(hist_g, back1, back2, th12);
	
	int th=(back1+back2)/2;
	int th0=th-abs(back2-back1);
	if(th0<0) th0=0;
	int th1=th+abs(back2-back1);
	if(th1>65535) th1=65535;
	
	INT64 RSum1=0, GSum1=0, BSum1=0, count1=0;
	INT64 RSum2=0, GSum2=0, BSum2=0, count2=0;
	
	for(int y=0; y<cy; y++)
	{
		color *pSrc0=src[y];
		color *pSrc1=src[y+cy];
		color *pSrc2=src[y+cy*2];
		byte *pBin=bin[y];
		for(int ix = 0; ix<cx; ix++, pBin++)
		{
			color R=(*pSrc0++);
			color G=(*pSrc1++);
			color B=(*pSrc2++);
			if(G<th0)
			{
				*pBin=2;
			}
			else if (G<=th12)
			{
				count2++;
				RSum2+=R;
				GSum2+=G;
				BSum2+=B;
				*pBin=1;
			}
			else if(G<th1)
			{
				count1++;
				RSum1+=R;
				GSum1+=G;
				BSum1+=B;
				*pBin=0;
			}
			else
			{
				*pBin=0;
			}
		}
	}
	
	if(count1==0) count1=1;
	if(count2==0) count2=1;
	
	backR1=int(RSum1/count1); backR2=int(RSum2/count2);
	backG1=int(GSum1/count1); backG2=int(GSum2/count2);
	backB1=int(BSum1/count1); backB2=int(BSum2/count2);

	char section[]="\\FindCells\\Result";
	SetValue(GetAppUnknown(),section, "SampleDensity", double(count2)/(count1+count2));

	SetValue(GetAppUnknown(),section, "Back1", long(backG1/256+0.5));
	SetValue(GetAppUnknown(),section, "Back2", long(backG2/256+0.5));

	SetValueColor(GetAppUnknown(),section, "BackColor1", RGB(backR1/256,backG1/256,backB1/256));
	SetValueColor(GetAppUnknown(),section, "BackColor2", RGB(backR2/256,backG2/256,backB2/256));


	return;
	
	smart_alloc(RSum, INT64, NC); smart_RSum.zero();
	smart_alloc(GSum, INT64, NC); smart_GSum.zero();
	smart_alloc(BSum, INT64, NC); smart_BSum.zero();
	smart_alloc(count, int, NC); smart_count.zero();
	
	for(y=0; y<cy; y++)
	{
		color *pSrc0=src[y];
		color *pSrc1=src[y+cy];
		color *pSrc2=src[y+cy*2];
		for(int ix = 0; ix<cx; ix++)
		{
			color R=(*pSrc0++);
			color G=(*pSrc1++);
			color B=(*pSrc2++);
			int c=G/256;
			count[c]++;
			RSum[c]+=R;
			GSum[c]+=G;
			BSum[c]+=B;
		}
	}
	
	const int threshold=cx*cy/100*5;  //отбросим 5% самых темных точек
	int c_threshold=0;
	int s=0;
	for(int c=0; c<NC; c++)
	{
		s+=count[c];
		if(s<threshold)
		{
			count[c]=0;
			RSum[c]=0;
			GSum[c]=0;
			BSum[c]=0;
			c_threshold=c;
		}
	}
	
	//построим по оставшимся точкам прямую r(g), g(g), b(g)
	double r0=0, g0=0, b0=0;
	double r1=0, g1=0, b1=0;
	double c0=0, c1=0;
	s=0;
	for(c=0; c<NC; c++)
	{
		r0+=RSum[c]; r1+=RSum[c]*c;
		g0+=GSum[c]; g1+=GSum[c]*c;
		b0+=BSum[c]; b1+=BSum[c]*c;
		c0+=c*count[c]; c1+=c*c*count[c];
		s+=count[c];
	}
	r0/=s; g0/=s; b0/=s; c0/=s;
	r1/=s; g1/=s; b1/=s; c1/=s;
	
	r1=(r1-c0*r0)/(c1-c0*c0);
	g1=(g1-c0*g0)/(c1-c0*c0);
	b1=(b1-c0*b0)/(c1-c0*c0);
	
	c=back1/256;
	backR1=col(r0+(c-c0)*r1);
	backG1=col(g0+(c-c0)*g1);
	backB1=col(b0+(c-c0)*b1);
	
	c=back2/256;
	backR2=int(r0+(c-c0)*r1);
	backG2=int(g0+(c-c0)*g1);
	backB2=int(b0+(c-c0)*b1);
	
	//CalcBack2(hist_r, backR1, backR2);
	//CalcBack2(hist_g, backG1, backG2);
	//CalcBack2(hist_b, backB1, backB2);
	
	//--------- Калибровка end------
}

//extract object name from Unknown
static _bstr_t GetObjectName( IUnknown *punk )
{
	INamedObject2Ptr	sptr( punk );
	if (sptr == 0) return "";

	BSTR	bstr = 0;
	if (FAILED(sptr->GetName(&bstr)) || !bstr)
		return "";

	_bstr_t	bstrRes(bstr);
	::SysFreeString(bstr);

	return bstrRes;
}

IUnknown *CFindCells::GetContextObject(_bstr_t bstrName, _bstr_t bstrType)
{ //Возвращает с AddRef
	IDocumentSitePtr sptrDoc(m_ptrTarget);
	if (sptrDoc == 0) return 0;

	// get active view from document
	IUnknown * punkActiveView = 0;
	sptrDoc->GetActiveView(&punkActiveView);
	IDataContext2Ptr ptrC = punkActiveView;
	if (punkActiveView) punkActiveView->Release();
	if (ptrC == 0) return 0;
	
	if( bstrName==_bstr_t("") )
	{
		IUnknown *punkImage = 0;
		ptrC->GetActiveObject( bstrType, &punkImage );
		//if( punkImage )
			//punkImage->Release();

		return punkImage;
	}

	ptrC = m_ptrTarget; // теперь пойдем по документу

	long lPos = 0;
	ptrC->GetFirstObjectPos( bstrType, &lPos );
	
	while( lPos )
	{
		IUnknown *punkImage = 0;
		ptrC->GetNextObject( bstrType, &lPos, &punkImage );

		if( punkImage )
		{
			if( GetObjectName( punkImage ) == bstrName )
			{
				//if( punkImage )
					//punkImage->Release();
				return punkImage;
			}

			punkImage->Release();
		}
	}
	return 0;
}


class CELL
{
public:
	CELL()
	{
		img=0; ptC=ptNC=_point(0,0);
	};
	~CELL()
	{
		img=0; ptC=ptNC=_point(0,0);
	};
	IImage3Ptr img;
	_point ptC; // center
	_point ptNC; // nucleus center
};

HRESULT CFindCells::DoInvoke()
{
	if (m_ptrTarget==0) return S_FALSE;
	IDocumentPtr doc(m_ptrTarget);
	if(doc==0) return S_FALSE;

	_bstr_t bstrImage = GetArgString("Img");
	_bstr_t bstrBinary = GetArgString("Binary");
	_bstr_t bstrObjectList=GetArgString("FindCells");
	m_TestOnly=GetArgLong("TestOnly")!=0;

	//IImage3Ptr      pArg( GetDataArgument() );
	IUnknown *punk2 = GetContextObject(bstrImage, szTypeImage);
	IImage3Ptr pArg(punk2);
	if (punk2!=0) punk2->Release();


	//IBinaryImagePtr pBin = GetDataResult("Binary");
	IUnknown *punk4=::CreateTypedObject(_bstr_t(szTypeBinaryImage)); // создаем, но не включаем в контекст
	IBinaryImagePtr pBin = punk4;
	if (punk4!=0) punk4->Release();
	
	//INamedDataObject2Ptr ptrObjectList = GetDataResult("FindCells");
	IUnknown *punk = GetContextObject(bstrObjectList, szTypeObjectList);
	INamedDataObject2Ptr ptrObjectList (punk);
	if (punk!=0) punk->Release();

	if( pArg==NULL || pBin==0)
		return S_FALSE;	

	if (ptrObjectList==0)
	{
		IUnknown *punk3=::CreateTypedObject(_bstr_t(szTypeObjectList));
		ptrObjectList=punk3;
		if (punk3!=0) punk3->Release();
		if(ptrObjectList!=0)
		{
			//::SetValue( doc, 0, 0/*path*/, _variant_t((IUnknown*)ptrObjectList));
			SetToDocData(doc, ptrObjectList);
			if( bstrObjectList != _bstr_t("")  )
			{
				INamedObject2Ptr	sptr( ptrObjectList );
				if (sptr != 0) sptr->SetName( bstrObjectList );
			}
			GUID guidNewBase;
			INamedDataObject2Ptr ptrArgObj(pArg);
			if(ptrArgObj!=0)
			ptrArgObj->GetBaseKey(&guidNewBase);
			ptrObjectList->SetBaseKey(&guidNewBase);
		}
	}

	if( pArg==NULL || ptrObjectList==NULL || pBin==0)
		return S_FALSE;	
	
	BBB
	//TDilate=0;
	
	BBB
	
	LoadIni();
	int nPaneCount;

	BBB
	
	SIZE size;
	pArg->GetSize((int*)&size.cx,(int*)&size.cy);
	POINT ptOffset;
	pArg->GetOffset(&ptOffset);
	if(size.cx<100 || size.cy<100) return S_FALSE;
	
	nPaneCount = ::GetImagePaneCount( pArg );
	if(nPaneCount<3) return S_FALSE;

	StartNotification( size.cy, nPaneCount );
	
	int T=-(int)GetTickCount();
	
	int T1=-(int)GetTickCount();
	
	pBin->CreateNew(size.cx, size.cy);
	pBin->SetOffset( ptOffset, TRUE);
	
	smart_alloc(srcRows, color *, size.cy*nPaneCount);
	for(int nPane=0;nPane<nPaneCount;nPane++ )
	{
		for(int y=0; y<size.cy; y++)
		{
			pArg->GetRow(y, nPane, &srcRows[nPane*size.cy+y]);
		}
	}
	
	smart_alloc(binRows, byte *, size.cy);
	for(int y=0; y<size.cy; y++)
	{
		pBin->GetRow(&binRows[y], y, false);
	}
	
	int TCalibr=-(int)GetTickCount();
	Calibrate(srcRows, binRows, size.cx, size.cy);
	TCalibr+=GetTickCount();
	
	int wx=size.cx/2, wy=size.cy/2, ww=wx*wy;
	smart_alloc(grayImg, byte, wx*wy);
	
	//перевод Arg в Grayscale с одновременным уменьшением в 2 раза
	for(int yGray=0; yGray<wy; yGray++)
	{
		int y=yGray*2;
		color* pSrc1_0 = srcRows[y+size.cy];
		color* pSrc1_1 = srcRows[y+size.cy+1];
		
		byte *pDst=grayImg+wx*yGray;
		for(int xGray = 0; xGray < wx; xGray++)
		{
			int s =
				int(*pSrc1_0++) + int(*pSrc1_0++) +
				int(*pSrc1_1++) + int(*pSrc1_1++);
			*pDst++=s/(4*256);
		}
	}

	BBB

	smart_alloc(grayImg2, byte, wx*wy);
	Smoothing(grayImg, grayImg2, wx, wy, m_SmoothSize); //сгладим

	BBB

	Smoothing(grayImg2, grayImg, wx, wy, m_EvenSize); //а теперь прогоним "контурную резкость".
	{ //вычитание
		byte *p1=grayImg;
		byte *p2=grayImg2;
		for(int i=0; i<ww; i++, p1++, p2++)
		{
			int s=(int(*p2)-int(*p1))+128;
			if(s<0) s=0;
			if(s>255) s=255;
			*p2=s;
		}
	}

	BBB

	int TFind=-(int)GetTickCount();
	
#define maxNNuclei 20
	//Теперь начинаем искать локальные минимумы.
	int sqr_x=wx/(m_FindAreaSize/3);
	int sqr_y=wy/(m_FindAreaSize/3);
	
	smart_alloc(min_coord_x, int, sqr_x*sqr_y);
	smart_alloc(min_coord_y, int, sqr_x*sqr_y);
	smart_alloc(min_v, int, sqr_x*sqr_y);
	{for(int i=0; i<sqr_x*sqr_y; i++) min_v[i]=256;}

	BBB

	{ //найдем минимумы по квадратикам
		for(int y=0; y<wy; y++)
		{
			int iy=y*sqr_y/wy;
			byte *pSrc=grayImg2+wx*y;
			for(int x = 0; x<wx; x++)
			{
				int ix=x*sqr_x/wx;
				int i=iy*sqr_x+ix;
				int v=*pSrc++;
				if(v<min_v[i])
				{
					min_v[i]=v;
					min_coord_x[i]=x;
					min_coord_y[i]=y;
				}
			}
		}
	}

	BBB

	int minX[maxNNuclei], minY[maxNNuclei], minV[maxNNuclei];
	
	for(int i=0; i<maxNNuclei; i++)
	{
		int mv=256, mx, my;
		for(int iy=0; iy<sqr_y; iy++)
		{
			for(int ix = 0; ix<sqr_x; ix++)
			{
				int j=iy*sqr_x+ix;
				int v=min_v[j];
				if(v<mv)
				{
					mv=v;
					mx=ix;
					my=iy;
				}
			}
		}
		minV[i]=mv;
		int j=my*sqr_x+mx;
		minX[i]=min_coord_x[j];
		minY[i]=min_coord_y[j];
		
		if(mx>sqr_x-2) mx=sqr_x-2;
		if(my>sqr_y-2) my=sqr_y-2;
		if(mx<1) mx=1;
		if(my<1) my=1;
		
		for(iy=my-1; iy<=my+1; iy++)
		{
			for(int ix=mx-1; ix<=mx+1; ix++)
			{
				int j=iy*sqr_x+ix;
				min_v[j]=255;
			}
		}
		
		j=my*sqr_x+mx;
		min_v[j]=256; min_v[j+1]=256;
		min_v[j+sqr_x]=256; min_v[j+sqr_x+1]=256;
	}

	BBB
		
	/*
	for(i=0; i<maxNNuclei; i++)
	{
	int mv=256;
	for(int y=0; y<wy; y++)
	{
	byte *pSrc=grayImg2+wx*y;
	for(int x = 0; x<wx; x++)
	{
	register byte v=*pSrc++;
	if(v<mv)
	{
	mv=v;
	minX[i]=x;
	minY[i]=y;
	}
	}
	}
	minV[i]=mv;
	int x0=max(0,minX[i]-m_FindAreaSize);
	int y0=max(0,minY[i]-m_FindAreaSize);
	int x1=min(wx,minX[i]+m_FindAreaSize+1);
	int y1=min(wy,minY[i]+m_FindAreaSize+1);
	for(y=y0; y<y1; y++)
	{
	byte *pSrc=grayImg2+wx*y+x0;
	for(int x = x0; x<x1; x++)
	{
	*pSrc++=255;
	}
	}
	}
	*/
	
	for(i=0; i<maxNNuclei; i++)
	{
		minX[i]*=2;
		minY[i]*=2;
	}

	if(m_bShowCross) //debug ability 
	{
		for(i=0; i<maxNNuclei; i++)
		{
			{
				int x=minX[i],y=minY[i];
				int y0=max(y-5,0);
		        int y1=min(y+5,size.cy-1);
				for(y=y0; y<=y1; y++)
				{
					srcRows[y+0*size.cy][x]=65535-i*65535/(maxNNuclei-1);
					srcRows[y+1*size.cy][x]=0;
					srcRows[y+2*size.cy][x]=i*65535/(maxNNuclei-1);
				}
			}
			{
				int x=minX[i],y=minY[i];
				int x0=max(x-5,0);
				int x1=min(x+5,size.cx-1);
				for(x=x0; x<=x1; x++)
				{
					srcRows[y+0*size.cy][x]=65535-i*65535/(maxNNuclei-1);
					srcRows[y+1*size.cy][x]=0;
					srcRows[y+2*size.cy][x]=i*65535/(maxNNuclei-1);
				}
			}
		}
	}

	//посчитаем, сколько из них нам интересны
	int nNuclei=0;
	/*
	int maxDV = ( minV[maxNNuclei-1] - minV[0] ) * m_FindTolerance / 100;
	//параметр - на случай, если вообще нет интересных точек
	
	for(i=1; i<maxNNuclei; i++)
	{
		double v0 = double(minV[maxNNuclei-1]) - minV[i];
		double v1 = double(minV[maxNNuclei-1]) - minV[i-1];
		double dv = sqrt(max(0,v1*v1-v0*v0));
		//double dv = minV[i]-minV[i-1];

		if(dv>=maxDV)
		{
			maxDV=dv;
			nNuclei=i;
		}
	}
	*/

	for(i=0; i<maxNNuclei; i++)
	{
		if(minV[i]<m_FindThreshold) nNuclei=i+1;
	}

	BBB

	TFind+=GetTickCount();
	
	T1+=GetTickCount();

	{
		char section[]="\\FindCells\\Result";
		SetValue(GetAppUnknown(),section, "KilledLeft", long(0));
		SetValue(GetAppUnknown(),section, "KilledRight", long(0));
		SetValue(GetAppUnknown(),section, "KilledTop", long(0));
		SetValue(GetAppUnknown(),section, "KilledBottom", long(0));
		SetValue(GetAppUnknown(),section, "KilledBig", long(0));
		SetValue(GetAppUnknown(),section, "KilledDupe", long(0));
		SetValue(GetAppUnknown(),section, "CellFound", long(0));
	}

	long nOld = 0;
	ptrObjectList->GetChildsCount(&nOld);

	_list_t <CELL> cells;

	for(i=0; i<nNuclei; i++)
	{
		if(minV[i]>255)  continue;
		smart_alloc(selection, byte, size.cx*size.cy);
		smart_selection.zero();
		smart_alloc(sel, byte*, size.cy);
		{for(int y=0; y<size.cy; y++)
			sel[y]=selection+size.cx*y;}

		RECT rect;
		BBB;

        bool isCell=
		SelectCell(srcRows, sel, minX[i], minY[i], size, &rect);

		if(isCell && m_TestOnly)
		{
			char section[]="\\FindCells\\Result";
			SetValue(GetAppUnknown(),section, "CellFound", long(1));
			break; // дальше не искать
		}

		int center_x=(rect.left+rect.right+1)/2;
		int center_y=(rect.top+rect.bottom+1)/2;
		rect.left=min(rect.left,center_x-m_CellCircleRadius);
		rect.right=max(rect.right,center_x+m_CellCircleRadius);
		rect.top=min(rect.top,center_y-m_CellCircleRadius);
		rect.bottom=max(rect.bottom,center_y+m_CellCircleRadius);

		rect.left=max(min(rect.left,size.cx),0);
		rect.right=max(min(rect.right,size.cx),0);
		rect.top=max(min(rect.top,size.cy),0);
		rect.bottom=max(min(rect.bottom,size.cy),0);

		if(isCell)
		{ // Проверка коллизий - не упирается ли клетка в край, не пересекается ли с другими...
			if(rect.left<=10)
			{
				char section[]="\\FindCells\\Result";
				SetValue(GetAppUnknown(),section, "KilledLeft", long(1));
				isCell=false;
			}

			if(rect.right>size.cx-10)
			{
				char section[]="\\FindCells\\Result";
				SetValue(GetAppUnknown(),section, "KilledRight", long(1));
				isCell=false;
			}

			if(rect.top<=10)
			{
				char section[]="\\FindCells\\Result";
				SetValue(GetAppUnknown(),section, "KilledTop", long(1));
				isCell=false;
			}

			if(rect.bottom>size.cy-10)
			{
				char section[]="\\FindCells\\Result";
				SetValue(GetAppUnknown(),section, "KilledBottom", long(1));
				isCell=false;
			}

			if(m_bCellTooBig)
			{
				char section[]="\\FindCells\\Result";
				SetValue(GetAppUnknown(),section, "KilledBig", long(1));
				isCell=false;
			}

			// сопоставление со старым Object List
			long pos;
			ptrObjectList->GetFirstChildPosition((long*)&pos);
			for(int i=0; i<nOld && pos!=0; i++)
			{ // сравним координаты нового объекта с координатами старых
				IUnknown* punk1 = 0;
				ptrObjectList->GetNextChild((long*)&pos, &punk1);
				IMeasureObjectPtr ptrObj = ICalcObjectPtr(punk1);
				if(punk1!=0) punk1->Release();
				if(ptrObj!=0) //ну мало ли объекты какие-нибудь не такие...
				{
					double fOldFramePosX=0, fOldFramePosY=0;
					ICalcObjectPtr ptrCalcOld(ptrObj);
					if(ptrCalcOld!=0)
					{
						ptrCalcOld->GetValue(KEY_X_OFFSET, &fOldFramePosX);
						ptrCalcOld->GetValue(KEY_Y_OFFSET, &fOldFramePosY);
					}

					IUnknown* punk2 = 0;
					ptrObj->GetImage(&punk2);
					IImage3Ptr ptrImg(punk2);
					if(punk2) punk2->Release();
					if(ptrImg!=0)
					{
						_size size(0,0);
						ptrImg->GetSize((int*)&size.cx,(int*)&size.cy);
						_point ptOffset(0,0);
						ptrImg->GetOffset(&ptOffset);
						int xOld = ptOffset.x + size.cx/2;
						int yOld = ptOffset.y + size.cy/2;
						if(_hypot(
							xOld+fOldFramePosX-center_x-m_FramePosX,
							yOld+fOldFramePosY-center_y-m_FramePosY)
							< m_MinCellDistance)
						{
							char section[]="\\FindCells\\Result";
							SetValue(GetAppUnknown(),section, "KilledDupe", long(1));
							isCell=false;
						}
					}
				}
			}

			//убиение совсем уж близколежащих клеток (<m_MinNucleiSplitter)
			long lPos2 = cells.head();
			while(lPos2) // пройдемся по всем клеткам, уже включенным в список
			{
				CELL cell = cells.get(lPos2);
				int lPos2Old=lPos2;
				lPos2 = cells.next(lPos2);
				_point ptC2 = cell.ptC;
				_point ptNC2 = cell.ptNC;

				//убиение совсем уж близколежащих клеток (<m_MinNucleiSplitter)
				// чтобы не пытаться их резать
				// (сопоставляем не клетки, а ядра - чтобы если у двух клеток слилась только цитоплазма, не грохнуть одну из них)
				if(_hypot(ptNC2.x - m_ptNucleusCenter.x, ptNC2.y - m_ptNucleusCenter.y) < m_MinNucleiSplitter)
				{ // ничего не помечаем в shell.data - просто скипаем фигню
					isCell=false;
				}
			}
		}

	BBB

		if(isCell && !m_bCellTooBig)
		{
			char section[]="\\FindCells\\Result";
			SetValue(GetAppUnknown(),section, "CellFound", long(1));
			{
				//-------------------- image
				_bstr_t s2(szTypeImage);
				IUnknown        *punk2 = ::CreateTypedObject(s2);
				IImage3Ptr      ptrI(punk2);
				punk2->Release();
				INamedDataObject2Ptr ptrImg(ptrI);
				
				ptrI->CreateVirtual( rect );
				ptrI->InitRowMasks();
				ptrImg->SetParent( pArg,  0); //Приаттачить
				ptrImg->StoreData(sdfCopyParentData);  //Сделать невиртуальным

				_point ofs1(0,0);
				ptrI->GetOffset(&ofs1);
				// ofs1.x += m_FramePosX; // вместо этого сохраним смещение отдельно
				// ofs1.y += m_FramePosY;
				ptrI->SetOffset(ofs1, FALSE);

				{// сохранить цвет фона в уголке image
					color *pR=0, *pG=0, *pB=0;
					ptrI->GetRow(0, 0, &pR);
					ptrI->GetRow(0, 1, &pG);
					ptrI->GetRow(0, 2, &pB);
					if(pR!=0) pR[0]=pR[1]=backR1;
					if(pG!=0) pG[0]=pG[1]=backG1;
					if(pB!=0) pB[0]=pB[1]=backB1;
				}
				
				//-------------------- object
				_bstr_t s(szTypeObject);
				IUnknown        *punk = ::CreateTypedObject(s);
				IMeasureObjectPtr       ptrO(punk );
				punk->Release();
				
				INamedDataObject2Ptr ptrObject(ptrO);
				ptrO->SetImage( ptrI );
				ptrObject->SetParent(ptrObjectList, 0);
				SetToDocData( doc, ptrObject );

				ICalcObjectPtr	ptrCalc(ptrO);
				if(ptrCalc!=0)
				{ // сохраним смещение в const-параметрах
					ptrCalc->SetValue(KEY_X_OFFSET, m_FramePosX);
					ptrCalc->SetValue(KEY_Y_OFFSET, m_FramePosY);
				}

				CELL cell;
				cell.img = ptrI;
				cell.ptC = _point(center_x,center_y);
				cell.ptNC = m_ptNucleusCenter;
				cells.add_tail(cell);

				int w,h;
				ptrI->GetSize(&w,&h);

	BBB

				color selColors[]=
				{
					0,0,0,   0,0,65535,   0,65535,0,   0,65535,65535,
						65535,0,0,   65535,0,65535,   65535,65535,0,   65535,65535,65535
				};
				//0=back, 1=citoplasma, 2=nucleus
				for(int y=rect.top; y<rect.bottom; y++)
				{
					byte *p;
					ptrI->GetRowMask( y-rect.top, &p );
					for(int x=rect.left; x<rect.right; x++,p++)
					{
						if(sel[y][x]>0)
							*p=256-sel[y][x];
						/*else if (1/*
							(y-enter_y)*(y-center_y)+
							(x-center_x)*(x-center_x)<
							m_CellCircleRadius*m_CellCircleRadius
							)*/
						else if (
							abs(y-center_y)<m_CellCircleRadius &&
							abs(x-center_x)<m_CellCircleRadius)
							*p=253;
						else *p=0;
						/*
						srcRows[0*size.cy+y][x]=selColors[sel[y][x]*3+0];
						srcRows[1*size.cy+y][x]=selColors[sel[y][x]*3+1];
						srcRows[2*size.cy+y][x]=selColors[sel[y][x]*3+2];
						*/
					}
				}

	BBB

				//изобразим контур - зальем его фоном
				if(0) for(y=rect.top+2; y<rect.bottom-2; y++)
				{
					byte *p,*pP,*pM, *pP2, *pM2;
					ptrI->GetRowMask( y-rect.top, &p );
					ptrI->GetRowMask( y-rect.top-1, &pM );
					ptrI->GetRowMask( y-rect.top+1, &pP );
					ptrI->GetRowMask( y-rect.top-2, &pM2 );
					ptrI->GetRowMask( y-rect.top+2, &pP2 );
					p+=2; pM+=2; pP+=2; pM2+=2; pP2+=2;
					for(int x=rect.left+2; x<rect.right-2; x++,p++,pM++,pP++, pM2++, pP2++)
					{
						if(*p==253)
						{
							if (*pM>253 || *pP>253 || p[-1]>253 || p[1]>253 ||
								*pM2>253 || *pP2>253 || p[-2]>253 || p[2]>253)
							{
								*p=0;
							}
						}
					}
				}

				if(ofs1.x>=0 && ofs1.x<30000 && ofs1.y>=0 && ofs1.y<30000)
				{ // контура создаем, только если не убежали далеко за край

					int nContourKind = GetValueInt(GetAppUnknown(),"\\FindCells", "ContourKind",0);

					if(1==nContourKind)
					{
						ptrI->FreeContours();
						Contour *cNewCont = new Contour;

						cNewCont->nAllocatedSize = cNewCont->nContourSize = 4;
						cNewCont->lReserved = 0;
						cNewCont->lFlags = cfExternal;
						cNewCont->ppoints = new ContourPoint[ 4 ];
						cNewCont->ppoints[0].x = short(ofs1.x);
						cNewCont->ppoints[0].y = short(ofs1.y);
						cNewCont->ppoints[1].x = short(ofs1.x + w);
						cNewCont->ppoints[1].y = short(ofs1.y);
						cNewCont->ppoints[2].x = short(ofs1.x + w);
						cNewCont->ppoints[2].y = short(ofs1.y + h);
						cNewCont->ppoints[3].x = short(ofs1.x);
						cNewCont->ppoints[3].y = short(ofs1.y + h);
						ptrI->AddContour( cNewCont );
					}
					else if(2==nContourKind)
					{
						ptrI->FreeContours();

						double r = max(w,h)/2;
						r = max(r,10);
						double x0 = ofs1.x+w/2;
						double y0 = ofs1.y+h/2;

						int n = round(r*6/40); // количество звездочек
						n = max(n,3);

						for(i=0; i<n; i++)
						{
							double x1 = x0 + sin(i*2*PI/n)*r;
							double y1 = y0 - cos(i*2*PI/n)*r;
							Contour *cNewCont = new Contour;
							cNewCont->nAllocatedSize = cNewCont->nContourSize = 10;
							cNewCont->lReserved = 0;
							cNewCont->lFlags = cfExternal;
							cNewCont->ppoints = new ContourPoint[ 10 ];
							for(int j=0; j<10; j++)
							{
								double rr = (j&1) ? 3 : 8;
								cNewCont->ppoints[j].x = round(x1 + sin(j*2*PI/10)*rr);
								cNewCont->ppoints[j].y = round(y1 - cos(j*2*PI/10)*rr);
							}
							ptrI->AddContour( cNewCont );
						}
					}
					else
					{
						ptrI->InitContours();
					}
				}
				
				//залить маски ?и контура
				
				//ptrO->SetImage( ptrI );

				::ConditionalCalcObject(ptrO, 0, true, 0 );
			}
			
			for(int y=0; y<size.cy; y++)
			{
				byte *pSel = sel[y];
			}
			for(int j=i+1; j<nNuclei; j++)
			{
				if(sel[minY[j]][minX[j]]==2)
					minV[j]=65535; //ядро уже захвачено
			}
		}
		else minV[i]=65535;
	}

	BBB;

	// "деление" клеток - если клетки лежат слишком близко (пересекаются), то
	// отбросим у каждой клетки куски, которые ближе ко второй.
	long lPos1=cells.head();
	while(lPos1)
	{
		CELL cell = cells.get(lPos1);
		int lPos1Old=lPos1;
		lPos1 = cells.next(lPos1);
		IImage3Ptr img1 = cell.img;
		_point ptC1 = cell.ptC;
		_point ptNC1 = cell.ptNC;

		long lPos2 = cells.head();
		while(lPos2)
		{
			CELL cell = cells.get(lPos2);
			int lPos2Old=lPos2;
			lPos2 = cells.next(lPos2);
			IImage3Ptr img2 = cell.img;
			_point ptC2 = cell.ptC;
			_point ptNC2 = cell.ptNC;
			if(lPos2Old==lPos1Old) continue;

			if(_hypot(ptC2.x - ptC1.x, ptC2.y - ptC1.y) < m_MinCellDistance)
			{
				_size size(0,0);
				img1->GetSize((int*)&size.cx,(int*)&size.cy);
				_point ptOffset(0,0);
				img1->GetOffset(&ptOffset);
				
				for(int nPane=0;nPane<nPaneCount;nPane++ )
				{
					int c0=backG1;
					if(nPane==0) c0=backR1;
					else if(nPane==2) c0=backB1;
					for(int y=0; y<size.cy; y++)
					{
						color *pRow;
						img1->GetRow(y, nPane, &pRow);
						for(int x=0; x<size.cx; x++)
						{
							double r1 = _hypot(ptNC1.x - x - ptOffset.x, ptNC1.y - y - ptOffset.y);
							double r2 = _hypot(ptNC2.x - x - ptOffset.x, ptNC2.y - y - ptOffset.y);
							double d = (r2-r1)/20+1;
							d=max(d,0.2);
							if(d<1)
								pRow[x] = int(pRow[x]*d + c0*(1-d));
						}
					}
				}
				for(int y=0; y<size.cy; y++)
				{
					byte *pMask;
					img1->GetRowMask(y, &pMask);
					for(int x=0; x<size.cx; x++)
					{
						double r1 = _hypot(ptNC1.x - x - ptOffset.x, ptNC1.y - y - ptOffset.y);
						double r2 = _hypot(ptNC2.x - x - ptOffset.x, ptNC2.y - y - ptOffset.y);
						if(r2<r1)
						{
							if (
								abs(y+ptOffset.y - ptC1.y)<m_CellCircleRadius &&
								abs(x+ptOffset.x - ptC1.x)<m_CellCircleRadius)
								pMask[x]=253;
							else pMask[x]=0;
						}
					}
				}

			}
		}
	}

	FinishNotification();

	BBB

	T+=GetTickCount();
	
	_bstr_t s("");
	char s1[200];
	sprintf(s1,"Total=%d  ",T);
	s=s+s1;
	sprintf(s1,"Подготовка=%d  ",T1);
	s=s+s1;
	sprintf(s1,"(в т.ч. калибровка=%d, поиск ядер=%d)  ", TCalibr, TFind);
	s=s+s1;
	//sprintf(s1,"dilate=%d  ",TDilate);
	//s=s+s1;
	
	//MessageBox(0, s, "Time: ", MB_OK);
	BBB
	
	return S_OK;
}

