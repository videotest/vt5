#include "stdafx.h"
#include "FilterNucleus.h"
#include "misc_utils.h"
#include "math.h"
#include "ImagePane.h"


_ainfo_base::arg	CFilterNucleusInfo::s_pargs[] = 
{	
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"Result",	szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false },
};


CFilterNucleus::CFilterNucleus()
{

}

CFilterNucleus::~CFilterNucleus()
{

}

#define CHECK \
{if((dst[y][x]&255)==0 && src[y][x]<=c)\
{int i = (nInserted++)%POINTS; px[i]=x; py[i]=y; dst[y][x]=m_FillColor; colorSum+=src[y][x];}\
else{/*colorSumBack+=src[y][x]; nBackCount++*/;};\
}

void CFilterNucleus::SelectDarkArea(color **src, color **dst, int x0, int y0, SIZE size, int tolerance)
{
	const int POINTS=1024;
	int px[POINTS], py[POINTS]; //кольцевой буфер координат точек
	int nInserted=0; //сколько точек добавлено в список
	int nDone=0; //сколько точек обработано
	double colorSum=0; //сумма цветов по добавленным точкам
	double colorSumBack=0; //сумма цветов по добавленным точкам
	int nBackCount=0;

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

	int cur = nInserted%POINTS;
	px[cur]=x0; py[cur]=y0;
	colorSum+=src[y0][x0];
	nInserted++;

	while(nInserted>nDone)
	{
		int cur = (nDone++)%POINTS;
		int y=py[cur], x=px[cur];

		if(y>=5 && y<size.cy-5 && x>=5 && x<size.cx-5)
		{
			int cBack=int(colorSumBack/nBackCount);
			int cFore=int(colorSum/nInserted);
			//int c=max(cFore+tolerance, src[y][x]);
			int c=cFore+(cBack-cFore)*tolerance/100;
			c=max(c, src[y][x]);
			
			/*//поиск по 4 соседним точкам
			y--; CHECK; y++;
			x++; CHECK; x--;
			y++; CHECK; y--;
			x--; CHECK;	x++;
			*/
			
			for(int iy=-5; iy<5; iy++)
			{
				for(int ix=-5; ix<5; ix++)
	 			{
					x+=ix; y+=iy; CHECK; x-=ix; y-=iy; 
				}
			}

			/*
			y--;
			if(dst[y][x]!=65535 && src[y][x]<=c)
			{
				int i = (nInserted++)%POINTS;
				px[i]=x; py[i]=y;
				dst[y][x]=65535;
				colorSum+=src[y][x];
			}
			y++;
			y++; 
			if(dst[y][x]!=65535 && src[y][x]<=c)
			{
				int i = (nInserted++)%POINTS;
				px[i]=x; py[i]=y;
				dst[y][x]=65535;
				colorSum+=src[y][x];
			}
			y--;
			x++;
			if(dst[y][x]!=65535 && src[y][x]<=c)
			{
				int i = (nInserted++)%POINTS;
				px[i]=x; py[i]=y;
				dst[y][x]=65535;
				colorSum+=src[y][x];
			}
			x--;
			x--;
			if(dst[y][x]!=65535 && src[y][x]<=c)
			{
				int i = (nInserted++)%POINTS;
				px[i]=x; py[i]=y;
				dst[y][x]=65535;
				colorSum+=src[y][x];
			}
			x++;*/
		}
	}
}

void CFilterNucleus::LoadIni()
{
	char path[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];   
	char ext[_MAX_EXT];

	GetModuleFileName(0, path, _MAX_PATH);
	_splitpath(path, drive, dir, fname, ext);
	_makepath(path, drive, dir, fname, "ini");
	char section[]="nucleus";

	m_SmoothSize=GetPrivateProfileInt(section, "SmoothSize", 5, path);
	m_EvenSize=GetPrivateProfileInt(section, "EvenSize", 127, path);
	m_FindAreaSize=GetPrivateProfileInt(section, "FindAreaSize", 32, path);
	m_NucleiTolerance=GetPrivateProfileInt(section, "NucleiTolerance", 20, path);
}

void CFilterNucleus::Smoothing( BYTE *p_arg, BYTE *p_res , int cx , int cy, int Mask )
{
	if(Mask<=1) 
	{
		CopyMemory(p_res, p_arg, cx*cy);
		return;
	}

	int m=Mask, mm=m*m, m1=(m-1)/2, m2=m-m1;
	_ptr_t2<int> smartBuf(cx+m);
	int *buf=smartBuf.ptr;

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
	for(int ix=0; ix<m1; ix++) buf[ix]=buf[m1];
	for(int ix=0; ix<m2; ix++) buf[m1+cx+ix]=buf[m1+cx-1];

	BYTE *p_r=p_res;
	for(int iy=0; iy<cy; iy++)
	{
		int sum=0;
		for (int ix=0; ix<m; ix++)	sum+=buf[ix];
		for (int ix=0; ix<cx; ix++)
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

bool CFilterNucleus::InvokeFilter()
{
	IImage3Ptr	pArg( GetDataArgument() );
	IImage3Ptr	pResult( GetDataResult() );
	
	if( pArg==0 || pResult==0 )
		return false;
	{	
		ICompatibleObjectPtr ptrCO( pResult );
		
		if( ptrCO == NULL )
			return false;

		if( S_OK != ptrCO->CreateCompatibleObject( pArg, NULL, 0 ) )
		{			
			return false;
		}		
	}

	LoadIni();
	
	// [vanek] 12.12.2003: works with color convertors panes
	CImagePaneCache paneCache( pArg );
//	int nPaneCount = ::GetImagePaneCount( pArg );
	int nPaneCount = paneCache.GetPanesCount( );

	SIZE size;
	
	pArg->GetSize((int*)&size.cx,(int*)&size.cy);

	StartNotification( size.cy, 1 );

	int T1=GetTickCount();

	int wx=size.cx/2, wy=size.cy/2, ww=wx*wy;
	_ptr_t2<byte> smartGrayImg(wx*wy);
	byte *grayImg=smartGrayImg.ptr;

	//перевод Arg в Grayscale с одновременным уменьшением в 2 раза
	for(int yGray=0; yGray<wy; yGray++)
	{
		int y=yGray*2;
		color* pSrc0_0;
		color* pSrc1_0;
		color* pSrc2_0;
		color* pSrc0_1;
		color* pSrc1_1;
		color* pSrc2_1;

		pArg->GetRow( y, 0 ,&pSrc0_0 );
		pArg->GetRow( y, 1 ,&pSrc1_0 );
		pArg->GetRow( y, 2 ,&pSrc2_0 );
		pArg->GetRow( y+1, 0 ,&pSrc0_1 );
		pArg->GetRow( y+1, 1 ,&pSrc1_1 );
		pArg->GetRow( y+1, 2 ,&pSrc2_1 );

		byte *pDst=grayImg+wx*yGray;
		for(int xGray = 0; xGray < wx; xGray++)
		{
			int s=
				(*pSrc0_0++)+(*pSrc1_0++)*2+(*pSrc2_0++)+
				(*pSrc0_1++)+(*pSrc1_1++)*2+(*pSrc2_1++);
			s +=
				(*pSrc0_0++)+(*pSrc1_0++)*2+(*pSrc2_0++)+
				(*pSrc0_1++)+(*pSrc1_1++)*2+(*pSrc2_1++);
			*pDst++ = s/(16*256);
		}
	}
	NextNotificationStage( );

	T1=GetTickCount()-T1;

	int T2=GetTickCount();
	_ptr_t2<byte> smartGrayImg2(wx*wy);
	byte *grayImg2=smartGrayImg2.ptr;
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

	T2=GetTickCount()-T2;

	//pResult->CreateNew( wx, wy );
	for( int nPane=0;nPane<nPaneCount;nPane++ )
	{
		if( paneCache.IsPaneEnable(nPane) && IsPaneEnable(nPane) )
		for(int y=0; y<wy*2; y++)
		{
			color *pDst;
			pResult->GetRow( y, nPane ,&pDst);
			byte *pSrc=grayImg2+wx*(y/2);
			for(int ix = 0; ix<wx; ix++)
			{
				*pDst++ = (*pSrc)*256;
				*pDst++ = (*pSrc++)*256;
			}
		}
	}

	int T3=GetTickCount();
	#define maxNNuclei 10
	//Теперь начинаем искать локальные минимумы.
	int minX[maxNNuclei], minY[maxNNuclei], minV[maxNNuclei];
	for(int i=0; i<maxNNuclei; i++)
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
		for(int y=y0; y<y1; y++)
		{
			byte *pSrc=grayImg2+wx*y+x0;
			for(int x = x0; x<x1; x++)
			{
				*pSrc++=255;
			}
		}
	}
	
	//посчитаем, сколько из них нам интересны
	int nNuclei=0;
	int maxDV=3; //параметр - на случай, если вообще нет интересных точек
	
	for(int i=1; i<maxNNuclei; i++) 
	{
		if(minV[i]-minV[i-1]>5/*maxDV*/)
		{
			maxDV=minV[i]-minV[i-1];
			nNuclei=i;
		}
	}

	//удвоим координаты - для нормального изображения
	for(int i=0; i<maxNNuclei; i++)
	{
		minX[i]*=2;
		minY[i]*=2;
	}

	T3=GetTickCount()-T3;

	//select areas
	int T4=GetTickCount();
	_ptr_t2<color *> smartSrcRows(size.cy);
	color **srcRows=smartSrcRows.ptr;
	_ptr_t2<color *> smartDstRows(size.cy);
	color **dstRows=smartDstRows.ptr;
	for(int y=0; y<size.cy; y++)
	{
		pArg->GetRow( y, 1 ,&srcRows[y]);
		pResult->GetRow( y, 1 ,&dstRows[y]);
	}
	for(i=0; i<nNuclei; i++)
	{
		m_FillColor=rand()%16*2048+32767;
		SelectDarkArea(srcRows, dstRows, minX[i], minY[i], size, m_NucleiTolerance);
	}
	T4=GetTickCount()-T4;
	

	//mark minimums
	int crossSize=5;
	int dc=(nNuclei<=1) ? 0 : 65535/(nNuclei-1);
	for(i=0; i<nNuclei; i++)
	{
		int x0=max(0,minX[i]-crossSize);
		int x1=min(size.cx,minX[i]+crossSize+1);
		color *pDst;
		pResult->GetRow( minY[i], 0 ,&pDst);
		pDst += x0;
		for(int x=x0; x<x1; x++) *pDst++=65535-i*dc;
		pResult->GetRow( minY[i], 1 ,&pDst);
		pDst += x0;
		for(int x=x0; x<x1; x++) *pDst++=0;
		pResult->GetRow( minY[i], 2 ,&pDst);
		pDst += x0;
		for(x=x0; x<x1; x++) *pDst++=i*dc;

		int y0=max(0,minY[i]-crossSize);
		int y1=min(size.cy,minY[i]+crossSize+1);
		for(int y=y0; y<y1; y++)
		{
			pResult->GetRow( y, 0 ,&pDst);
			pDst[minX[i]] = 65535-i*dc;
			pResult->GetRow( y, 1 ,&pDst);
			pDst[minX[i]] = 0;
			pResult->GetRow( y, 2 ,&pDst);
			pDst[minX[i]] = i*dc;
		}
	}
	

	FinishNotification();

/*	
	CString str;
	str.Format( 
		"Convert: %d,\n"
		"Smooth: %d,\n"
		"Find minimums: %d\n",
		T1, T2, T3 );
	AfxMessageBox( str );

	
	CString s;
	s="minimums:\n";
	for(i=0; i<9; i++)
	{
		str.Format("%d\t", minV[i]);
		s=s+str;
	}
	s=s+"\n";
	for(i=0; i<9; i++)
	{
		str.Format("%d\t", minV[i+1]-minV[i]);
		s=s+str;
	}
	AfxMessageBox( s );
*/	

	return true;

}
