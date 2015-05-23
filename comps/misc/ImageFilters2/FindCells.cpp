#include "stdafx.h"
#include "FindCells.h"
#include "math.h"
#include <stdio.h>
#include "measure5.h"
#include "alloc.h"
#include "FilterNucleus.h"

_ainfo_base::arg CFindCellsInfo::s_pargs[] = 
{       
	{"Img",       szArgumentTypeImage, 0, true, true },
	{"FindCells",    szArgumentTypeObjectList, 0, false, true },
	{"Binary",    szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

/////////////////////////////////////////////////////////////////////////////
CFindCells::CFindCells()
{
	m_SmoothSize=5;
	m_EvenSize=127;
	m_FindAreaSize=32;
	m_NucleiTolerance=24;
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
	}
	for(ix=0; ix<m1; ix++) buf[ix]=buf[m1];
	for(ix=0; ix<m2; ix++) buf[m1+cx+ix]=buf[m1+cx-1];

	BYTE *p_r=p_res;
	for(int iy=0; iy<cy; iy++)
	{
		int sum=0;
		for (int ix=0; ix<m; ix++)	sum+=buf[ix];
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

static int TDilate=0;

void CFindCells::DilateColor(BYTE **sel, int cx, int cy, int mask, byte cFore, byte cForeNew, byte cBack)
//обработка маски - дилатация
{
	TDilate-=GetTickCount();

	int m=mask, mm=m*m, m1=(m-1)/2, m2=m-m1;
	smart_alloc(count, int, (cx+1)*(cy+1));
	smart_alloc(countRows, int*, cy+1);
	for(int y=0; y<=cy; y++) countRows[y] = count+(cx+1)*y;

	for(int x=0; x<=cx; x++) count[x]=0; //1-я строка =0

	for(y=1; y<=cy; y++)
	{
		countRows[y][0]=0;
		for(int x=1; x<=cx; x++) 
		{
			countRows[y][x]=
				countRows[y-1][x]+countRows[y][x-1]-
				countRows[y-1][x-1];
			if (sel[y-1][x-1]==cFore) 
				countRows[y][x]++;
		}
	}

	smart_alloc(y0, int, cy);
	smart_alloc(y1, int, cy);
	for(y=0; y<cy; y++)
	{
		y0[y]=max(y-m1,0);
		y1[y]=min(y+m2,cy);
	}

	smart_alloc(x0, int, cx);
	smart_alloc(x1, int, cx);
	for(x=0; x<cx; x++)
	{
		x0[x]=max(x-m1,0);
		x1[x]=min(x+m2,cx);
	}

	for(y=0; y<cy; y++)
	{
		int *c0=countRows[y0[y]];
		int *c1=countRows[y1[y]];
		for(int x=0; x<cx; x++) 
		{
			int xx0=x0[x], xx1=x1[x];
			if(sel[y][x]==cBack && c1[xx1]-c1[xx0]-c0[xx1]+c0[xx0]>2)
				sel[y][x]=cForeNew;
		}
	}
	TDilate+=GetTickCount();
}

#define INSERTPOINT	\
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
	while(nInserted>nDone)
	{
		int cur = nDone++;
		int y=py[cur], x=px[cur];

		if(y>=9 && y<size.cy-9 && x>=9 && x<size.cx-9)
		{
			int cBack=int(colorSumBack/nBackCount);
			int cFore=int(colorSum/(nInserted-nInserted0));
			int c0=src[y][x];
			int c=cFore+(cBack-cFore)*tolerance/100;
			c=max(c, c0);
			y--; CHECK; y++;
			x++; CHECK; x--;
			y++; CHECK; y--;
			x--; CHECK;	x++;

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

#define INSERTPOINT2	\
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

#define ADDPOINT2	\
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


//		fprintf(f,"(%u,%u,%u) - (%u,%u,%u) : %u\n", r,g,b, foreR, foreG, foreB, d<d1 && d<d2); 

static RECT ClipRect={0,0,1024,1024};

static Line(byte **dst, int x0, int y0, int x1, int y1, byte c)
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

static CircleMin(int *src, int *dst, int dirs, int size)
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

static CircleMax(int *src, int *dst, int dirs, int size)
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

static CircleAvg(int *src, int *dst, int dirs, int size)
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

static CircleMed(int *src, int *dst, int dirs, int size)
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

static FillPoly(byte **dst, int *x, int*y, int n, byte color)
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
				if(x>0 && y>0 && x<size.cx-1 && y<size.cy-1)
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

	ClipRect.left=0;
	ClipRect.top=0;
	ClipRect.right=size.cx;
	ClipRect.bottom=size.cy;

	//откалибруем фон (приблизительно)
	int xx0=max(min(x0,m_EvenSize),size.cx-1-m_EvenSize)-m_EvenSize;
	int yy0=max(min(y0,m_EvenSize),size.cy-1-m_EvenSize)-m_EvenSize;
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
		return(false);
	}

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

	//DeleteSmallSegments(dst, size);
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

	fillColor=1;
	SelectByRadius(src, dst, size, m_CitoplasmaTolerance, dstRect);
	//ExpandSelectedArea(src, dst, size, m_CitoplasmaTolerance);
	//DeleteSmallSegments(dst, size);
	//DilateColor(dst, size.cx, size.cy, m_NucleiCloseSize, 1,1,0);
	//DilateColor(dst, size.cx, size.cy, m_NucleiCloseSize, 0,0,1);

	return(true);
}

#define INSPOINT	\
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
	do	{
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
	char section[]="FindCells";

	long nCounter = GetValueInt(GetAppUnknown(), "\\Binary", "IndexedColorsCount", 1);

	m_SmoothSize=GetValueInt(GetAppUnknown(),section, "SmoothSize", 5);
	m_EvenSize=GetValueInt(GetAppUnknown(),section, "EvenSize", 127);
	m_FindAreaSize=GetValueInt(GetAppUnknown(),section, "FindAreaSize", 32);
	m_NucleiTolerance=GetValueInt(GetAppUnknown(),section, "NucleiTolerance", 20);
	m_CitoplasmaTolerance=GetValueInt(GetAppUnknown(),section, "CitoplasmaTolerance", 75);
	if(m_CitoplasmaTolerance<1) m_CitoplasmaTolerance=1;
	m_MinNucleiArea=GetValueInt(GetAppUnknown(),section, "MinNucleiArea", 100);
	m_MinNucleiSplitter=GetValueInt(GetAppUnknown(),section, "MinNucleiSplitter",10);
	m_NucleiCloseSize=GetValueInt(GetAppUnknown(),section, "NucleiCloseSize",5);
	m_MaxCellRadius=GetValueInt(GetAppUnknown(),section, "MaxCellRadius",150);
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
	for	(int i=1; i<NC; i++) h[i]=hist[i]*(1-sm)+h[i-1]*sm;
	for	(i=NC-2; i>=0; i--) h[i]=h[i]*(1-sm)+h[i+1]*sm;

	back1=0; back2=0;
	for	(i=0; i<NC; i++) if(h[i]>h[back1]) back1=i;
	for	(i=0; i<NC; i++) if(h[i]*abs(i-back1)>h[back2]*abs(back2-back1)) back2=i;

	if (back1<back2)
	{ color temp=back1; back1=back2; back2=temp; }

	threshold=back2;
	for(i=back2; i<back1; i++)
	{
		if(h[i]<h[threshold]) threshold=i;
	}
	
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
	if(th1>65535) th0=65535;

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
				count1++;
				RSum1+=R;
				GSum1+=G;
				BSum1+=B;
				*pBin=1;
			}
			else if(G<th1)
			{
				count2++;
				RSum2+=R;
				GSum2+=G;
				BSum2+=B;
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


bool CFindCells::InvokeFilter()
{
	IImage3Ptr	pArg( GetDataArgument() );
	//IUnknown	*punkR = GetDataResult();
	//MessageBox(0, (char *)(punkR), "punkR", MB_OK);
	INamedDataObject2Ptr ptrObjectList( GetDataResult("FindCells") );
	IBinaryImagePtr pBin = GetDataResult("Binary");

	if( pArg==NULL || ptrObjectList==NULL || pBin==NULL )
		return false;


	TDilate=0;


	LoadIni();
	int nPaneCount = ::GetImagePaneCount( pArg );

	SIZE size;
	pArg->GetSize((int*)&size.cx,(int*)&size.cy);
	POINT ptOffset;
	pArg->GetOffset(&ptOffset);

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
				(*pSrc1_0++) + (*pSrc1_0++) +
				(*pSrc1_1++) + (*pSrc1_1++);
			*pDst++=s/(4*256);
		}
	}

	smart_alloc(grayImg2, byte, wx*wy); 
	Smoothing(grayImg, grayImg2, wx, wy, m_SmoothSize); //сгладим

	Smoothing(grayImg2, grayImg, wx, wy, m_EvenSize); //а теперь прогоним "контурную резкость".
	{ //вычитание
		byte *p1=grayImg;	
		byte *p2=grayImg2;
		for(int i=0; i<ww; i++, p1++, p2++)
		{
			int s=(*p2-*p1)+128;
			if(s<0) s=0;
			if(s>255) s=255;
			*p2=s;
		}
	}

	int TFind=-(int)GetTickCount();

	#define maxNNuclei 20
	//Теперь начинаем искать локальные минимумы.
	int sqr_x=wx/(m_FindAreaSize/3);
	int sqr_y=wy/(m_FindAreaSize/3);

	smart_alloc(min_coord_x, int, sqr_x*sqr_y);
	smart_alloc(min_coord_y, int, sqr_x*sqr_y);
	smart_alloc(min_v, int, sqr_x*sqr_y);
	{for(int i=0; i<sqr_x*sqr_y; i++) min_v[i]=256;}

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
	
	//посчитаем, сколько из них нам интересны
	int nNuclei=0;
	int maxDV=3; //параметр - на случай, если вообще нет интересных точек
	
	for(i=1; i<maxNNuclei; i++) 
	{
		if(minV[i]-minV[i-1]>maxDV-1)
		{
			maxDV=minV[i]-minV[i-1];
			nNuclei=i;
		}
	}

	TFind+=GetTickCount();

	T1+=GetTickCount();

	for(i=0; i<nNuclei; i++)
	{
		if(minV[i]>255)  continue;
		smart_alloc(selection, byte, size.cx*size.cy);
		smart_selection.zero();
		smart_alloc(sel, byte*, size.cy);
		{for(int y=0; y<size.cy; y++) 
			sel[y]=selection+size.cx*y;}

		RECT rect;
		bool isCell=
			SelectCell(srcRows, sel, minX[i], minY[i], size, &rect);
		if(isCell)
		{
			{
				//-------------------- image
				_bstr_t s2(szTypeImage);
				IUnknown	*punk2 = ::CreateTypedObject(s2);
				IImage3Ptr	ptrI(punk2);
				punk2->Release();
				INamedDataObject2Ptr ptrImg(ptrI);

				ptrI->CreateVirtual( rect );
				ptrI->InitRowMasks();
				DWORD dwFlags = 0;
				ptrImg->GetObjectFlags( &dwFlags );
				ptrImg->SetParent( pArg,  dwFlags);

				//-------------------- object
				_bstr_t s(szTypeObject);
				IUnknown	*punk = ::CreateTypedObject(s);
				IMeasureObjectPtr	ptrO(punk );
				punk->Release();

				INamedDataObject2Ptr ptrObject(ptrO);
				ptrO->SetImage( ptrI );
				ptrObject->GetObjectFlags( &dwFlags );
				ptrObject->SetParent(ptrObjectList, dwFlags);
				

				int w,h;
				ptrI->GetSize(&w,&h);

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
						else *p=0;
						/*
						srcRows[0*size.cy+y][x]=selColors[sel[y][x]*3+0];
						srcRows[1*size.cy+y][x]=selColors[sel[y][x]*3+1];
						srcRows[2*size.cy+y][x]=selColors[sel[y][x]*3+2];
						*/
					}
				}
				//ptrI->InitContours();

				//залить маски ?и контура

				//ptrO->SetImage( ptrI );
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

	FinishNotification();

	T+=GetTickCount();

	_bstr_t s("");
	char s1[200];
	sprintf(s1,"Total=%d  ",T);
	s=s+s1;
	sprintf(s1,"Подготовка=%d  ",T1);
	s=s+s1;
	sprintf(s1,"(в т.ч. калибровка=%d, поиск ядер=%d)  ", TCalibr, TFind);
	s=s+s1;
	sprintf(s1,"dilate=%d  ",TDilate);
	s=s+s1;

	MessageBox(0, s, "Time: ", MB_OK);

	return true;
}

