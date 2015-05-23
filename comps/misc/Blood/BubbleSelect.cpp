#include "stdafx.h"
#include "BubbleSelect.h"
#include "math.h"
#include <stdio.h>
#include "measure5.h"
#include "alloc.h"

#define szBinSrc "BinSrc"
#define szX0 "x0"
#define szY0 "y0"
#define szBinResult "BubbleSelect"

_ainfo_base::arg CBubbleSelectInfo::s_pargs[] = 
{       
	{szBinSrc,       szArgumentTypeBinaryImage, 0, true, true },
	{szX0,		szArgumentTypeInt, "0", true, false },
	{szY0,		szArgumentTypeInt, "0", true, false },
	{szBinResult,    szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

/////////////////////////////////////////////////////////////////////////////
CBubbleSelect::CBubbleSelect()
{
}

#define del(par) {if(par) {delete(par); par=0;}}

/////////////////////////////////////////////////////////////////////////////
CBubbleSelect::~CBubbleSelect()
{
}

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

static int CircleMaskErode(byte *mask, int n, int size)
{
	smart_alloc(count,int,n+size+1);
	count[0]=0;
	for(int i=1; i<n+size+1; i++)
	{
		count[i] = mask[i%n] ? count[i-1]+1 : count[i-1];
	}
	int s1=size/2;
	for(i=0; i<n; i++)
	{
		int j=i-s1;
		if(j<0) j+=n;
		int c=count[j+size]-count[j];
		mask[i] = c<size ? 0 : 255;
	}
}

static int CircleMaskDilate(byte *mask, int n, int size)
{
	smart_alloc(count,int,n+size+1);
	count[0]=0;
	for(int i=1; i<n+size+1; i++)
	{
		count[i] = mask[i%n] ? count[i-1]+1 : count[i-1];
	}
	int s1=size/2;
	for(i=0; i<n; i++)
	{
		int j=i-s1;
		if(j<0) j+=n;
		int c=count[j+size]-count[j];
		mask[i] = c==0 ? 0 : 255;
	}
}

static void CircleMaskOpen(byte *mask, int n, int size)
{
	int count=0;

	for(int i=n-size; i<n; i++)
	{
		if(mask[i])	count++;
		else count=0;
	}

	for(i=0; i<n; i++)
	{
		if(mask[i])	count++;
		else
		{
			if(count<size)
			{
				int j=i;
				while(count--)
				{
					j--; if(j<0) j+=n;
					mask[j]=0;
				}
			}
			count=0;
		}
	}
}


static void ExpandBubble(byte **src, double *x, double *y, int n)
{
	smart_alloc(x1,double,n);
	smart_alloc(y1,double,n);
	smart_alloc(mask,byte,n);

	for(int i=0; i<n; i++)
	{
		int ip=(i+1)%n;
		int im=(i+n-1)%n;
		double nx=y[ip]-y[im];
		double ny=x[im]-x[ip];
		double nlen=sqrt(nx*nx+ny*ny);
		nx/=nlen; ny/=nlen;
		x1[i]=x[i]+nx*2;
		y1[i]=y[i]+ny*2;

		x1[i]=(x1[i]*2+x[ip]+x[im])/4;
		y1[i]=(y1[i]*2+y[ip]+y[im])/4;

		//подстрахуемся
		if(x1[i]<ClipRect.left) x1[i]=ClipRect.left;
		if(x1[i]>=ClipRect.right) x1[i]=ClipRect.right-1;
		if(y1[i]<ClipRect.top) y1[i]=ClipRect.top;
		if(y1[i]>=ClipRect.bottom) y1[i]=ClipRect.bottom-1;

		int xi=int(x1[i]), yi=int(y1[i]);
		mask[i]=src[yi][xi];
	}	

	//CircleMaskErode(mask, n, 25);
	//CircleMaskDilate(mask, n, 25);
	CircleMaskOpen(mask, n, 35);

	for(i=0; i<n; i++)
	{
		if(mask[i])
		{
			x[i]=x1[i];
			y[i]=y1[i];
		}
	}

	for(i=0; i<n; i++)
	{ //подстрахуемся
		if(x[i]<ClipRect.left) x[i]=ClipRect.left;
		if(x[i]>=ClipRect.right) x[i]=ClipRect.right-1;
		if(y[i]<ClipRect.top) y[i]=ClipRect.top;
		if(y[i]>=ClipRect.bottom) y[i]=ClipRect.bottom-1;
	}
}

static int SmoothBubble(byte **src, double *x, double *y, int n)
{
	smart_alloc(x1,double,n);
	smart_alloc(y1,double,n);

	for(int i=0; i<n; i++)
	{
		int ip=(i+1)%n;
		int im=(i+n-1)%n;
		x1[i]=(x[ip]+x[i]*2+x[im])/4;
		y1[i]=(y[ip]+y[i]*2+y[im])/4;
	}	
	for(i=0; i<n; i++)
	{
		x[i]=x1[i];
		y[i]=y1[i];
	}
}

static int LimitCurvature(byte **src, double *x, double *y, int n)
{
	smart_alloc(x1,double,n);
	smart_alloc(y1,double,n);

	for(int i=0; i<n; i++)
	{
		int ip=(i+1)%n;
		int im=(i+n-1)%n;
		double x0=(x[ip]+x[im])/2;
		double y0=(y[ip]+y[im])/2;
		double L=_hypot(x[ip]-x[im], y[ip]-y[im]);
		double h=_hypot(x[i]-x0, y[i]-y0);
		if(h>L*PI/16)
		{
			double c=L*PI/16/h;
			x1[i]=x0+(x[i]-x0)*c;
			y1[i]=y0+(y[i]-y0)*c;
		}
		else
		{
			x1[i]=x[i];
			y1[i]=y[i];
		}
	}	
	for(i=0; i<n; i++)
	{
		x[i]=x1[i];
		y[i]=y1[i];
	}
}

static void RegridBubble(double *x, double *y, int n)
{
	smart_alloc(len,double,n+2);
	smart_alloc(x1,double,n);
	smart_alloc(y1,double,n);

	len[0]=0;
	for(int i=0; i<n; i++) 
		len[i+1]=len[i]+_hypot(x[i+1]-x[i], y[i+1]-y[i]);
	len[n]=len[n-1]+_hypot(x[0]-x[n-1], y[0]-y[n-1]);
	len[n+1]=len[n]+_hypot(x[1]-x[0], y[1]-y[0]); //на всякий случай
	
	double c=n/len[n];
	for(i=0; i<=n+1; i++) 
		len[i] *= c;

	int j=0;
	for(i=0; i<n; i++) 
	{
		while(i>len[j+1]) j++;
		// len[j]<=i<=len[j+1]
		int j0=j%n, j1=(j+1)%n;
		double c=(i-len[j])/(len[j+1]-len[j]);
		x1[i]=x[j0]*(1-c)+x[j1]*c;
		y1[i]=y[j0]*(1-c)+y[j1]*c;
	}

	for(i=0; i<n; i++)
	{
		x[i]=x1[i];
		y[i]=y1[i];
	}
}

static int CorrectBubble(double *x, double *y, int n)
{
	smart_alloc(x1,double,n);
	smart_alloc(y1,double,n);

	for(int i=0; i<n; i++) 
	{
		int ip=(i+1)%n;
		int im=(i+n-1)%n;
		double L=_hypot(x[ip]-x[im],y[ip]-y[im]);
		double L1=_hypot(x[i]-x[im],y[i]-y[im]);
		double L2=_hypot(x[ip]-x[i],y[ip]-y[i]);
		double c=(L2-L1)/(L*2)/2;
		x1[i]=x[i]+(x[ip]-x[im])*c;
		y1[i]=y[i]+(y[ip]-y[im])*c;
	}

	for(i=0; i<n; i++)
	{
		x[i]=x1[i];
		y[i]=y1[i];
	}

	for(i=0; i<n; i++)
	{ //подстрахуемся
		if(x[i]<ClipRect.left) x[i]=ClipRect.left;
		if(x[i]>=ClipRect.right) x[i]=ClipRect.right-1;
		if(y[i]<ClipRect.top) y[i]=ClipRect.top;
		if(y[i]>=ClipRect.bottom) y[i]=ClipRect.bottom-1;
	}
}


bool CBubbleSelect::InvokeFilter()
{
	IBinaryImagePtr pBinSrc = GetDataArgument(szBinSrc);
	IBinaryImagePtr pBinDst = GetDataResult(szBinResult);

	if( pBinSrc==NULL || pBinDst==NULL)
		return false;

	SIZE size;
	pBinSrc->GetSizes((int*)&size.cx,(int*)&size.cy);
	POINT ptOffset;
	pBinSrc->GetOffset(&ptOffset);

	StartNotification( size.cy, 1 );

	pBinDst->CreateNew(size.cx, size.cy);
	pBinDst->SetOffset( ptOffset, TRUE);

	ClipRect.left=0;
	ClipRect.top=0;
	ClipRect.right=size.cx;
	ClipRect.bottom=size.cy;

	int x0=GetArgLong( szX0 );
	int y0=GetArgLong( szY0 );

	smart_alloc(srcRows, byte *, size.cy);
	smart_alloc(dstRows, byte *, size.cy);
	{for(int y=0; y<size.cy; y++)
	{
		pBinSrc->GetRow(&srcRows[y], y, false);
		pBinDst->GetRow(&dstRows[y], y, false);
	}}

	//завершили инициализацию - начинаем работать
	if(0){ //debug - test
		smart_alloc(poly_x, int, 12);
		smart_alloc(poly_y, int, 12);
		for(int i=0; i<12; i++)
		{ 
			poly_x[i]=rand()%size.cx;
			poly_y[i]=rand()%size.cy;
		}
		FillPoly(dstRows, poly_x, poly_y, 12, 255);
	}

	x0=360; y0=320; //debug

	{ //debug - test 2
		int n=256;
		smart_alloc(x, double, n);
		smart_alloc(y, double, n);
		smart_alloc(poly_x, int, n);
		smart_alloc(poly_y, int, n);
		for(int i=0; i<n; i++)
		{ 
			x[i]=x0+10*cos(i*PI*2/n);
			y[i]=y0+10*sin(i*PI*2/n);
		}
		
		for(i=0; i<n; i++)
		{ 
			poly_x[i]=int(x[i]);
			poly_y[i]=int(y[i]);
		}
		FillPoly(dstRows, poly_x, poly_y, n, 255);

		for(i=0; i<5000; i++)
		{ 
			//LimitCurvature(srcRows, x, y, n);
			ExpandBubble(srcRows, x, y, n);
			//CorrectBubble(x, y, n);
			if (i%10==0) 
				RegridBubble(x, y, n);
		}

		for(i=0; i<10; i++)
		{ 
			//ExpandBubble(srcRows, x, y, n, false);
		}

		for(i=0; i<n; i++)
		{ 
			poly_x[i]=int(x[i]);
			poly_y[i]=int(y[i]);
		}
		FillPoly(dstRows, poly_x, poly_y, n, 1);
		for(i=0; i<n; i++)
		{ 
			if(0) Line(dstRows, poly_x[i],poly_y[i], x0,y0, 2);
		}

	}

	FinishNotification();
	return true;
}

