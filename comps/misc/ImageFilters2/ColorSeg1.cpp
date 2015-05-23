// ColorSeg1.cpp: implementation of the CColorSeg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ColorSeg1.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//#include "..\stdafx.h"


#include "math.h"

#define szPreset "preset"
#define szSegColors "seg_colors"
#define szRadius "radius"
#define szNmin "n_min"
#define szNmin1 "n_min1"
#define szNcon "n_con"
#define szCountLim "count_lim"

_ainfo_base::arg CColorSegInfo::s_pargs[] = 
{       
	{szPreset,  szArgumentTypeInt, "1", true, false }, //номер пресета (1=high, 2=middle, 3=low, 0 - не задан)
	{szSegColors,  szArgumentTypeInt, "0", true, false }, //1 - пренебречь яркостью (важен только цвет).
	{szRadius,  szArgumentTypeInt, "5", true, false }, //радиус цветовой сферы для определения основного цвета
	{szNmin,  szArgumentTypeInt, "400", true, false }, //выделяются только такие цвета, что не менее Nmin точек имеют близкий цвет
	{szNmin1,  szArgumentTypeInt, "200", true, false }, //сегменты меньшей площади удаляются? (должны бы... но почему-то нет)
	{szNcon,  szArgumentTypeInt, "30", true, false }, //связные области площадью меньше Ncon удаляются.
	{szCountLim,  szArgumentTypeInt, "5", true, false }, //максимальное число выделяемых цветов.

	//имеет смысл варьировать только radius, CountLim, Ncon (ну и SegColors, само собой)
	//если на результате есть мелкие шумы (точки) - увеличивать Ncon
	//если слишком много цветов и области порезаны на куски - увеличивать radius
	//если слишком много цветов, но границы сегментов верны - уменьшать CountLim
	//если области сливаются - уменьшать CountLim или radius.
	//вообще при достаточно большом radius CountLim неважен.

	{"Img",       szArgumentTypeImage, 0, true, true },
//	{"Result",    szArgumentTypeImage, 0, false, true },	
	{"Result",    szArgumentTypeBinaryImage, 0, false, true },
	{0, 0, 0, false, false },
};

CColorSeg::CColorSeg()
{
	m_Preset=1;
	m_Radius=5;
	m_Nmin=400;
	m_Nmin1=200;
	m_Ncon=30;
	m_CountLim=5;
	m_SegColors=false;
	
	p_Gist=0;
	p_Gistz=0;
	p_Gistyz=0;
    iRow = 65;
	iUp = 4225;
}

/////////////////////////////////////////////////////////////////////////////
CColorSeg::~CColorSeg()
{
}


/////////////////////////////////////////////////////////////////////////////

bool CColorSeg::FindNoZerPoint(int cx,int cy,int ic,int jc,BYTE *p_work1 ,BYTE **p_p_work1y)
// Найти ближайшую ненулевую точку байтового изображения;
// Вернуть в p_p_work1y указатель на нее.
// Результат - успешно ли найдена
{
    int cx3 = cx*3;
	int flag=0;
	int i1,j1,i2,j2,i,j;
	int SizeSq=1;
	BYTE *p_work1y=*p_p_work1y;
	
	while (flag<50)
	{
		p_work1y-=(cx3+3);
		
		i1=ic-SizeSq;
		i2=ic+SizeSq;
		j1=jc-SizeSq;
		j2=jc+SizeSq;
		
		/////////////////////////////////////////////
		if (j1>0)
			for (i=i1;i<=i2;i++)
			{
				if (i>0 && i<cx)
					if(*p_work1y!=0)
					{
						*p_p_work1y=p_work1y;
						return true;
					}
					
					p_work1y+=3;
			}
			else
				p_work1y+=SizeSq*6+3;
			p_work1y-=3;
			///////////////////////////////////////////////
			if (i2<cx)
				for (j=j1;j<=j2;j++)
				{
					if (j>0 && j<cy)
						if(*p_work1y!=0)
						{
							*p_p_work1y=p_work1y;
							return true;
						}
						
						p_work1y+=cx3;
				}
				else
					p_work1y+=(SizeSq+SizeSq+1)*cx3;
				p_work1y-=cx3;
				///////////////////////////////////////////////
				if (j2<cy)
					for (i=i2;i>=i1;i--)
					{
						if (i>0 && i<cx)
							if(*p_work1y!=0)
							{
								*p_p_work1y=p_work1y;
								return true;
							}
							
							p_work1y-=3;
					}
					else
						p_work1y-=SizeSq*6+3;
					p_work1y+=3;
					///////////////////////////////////////////////
					if (i1>0)
						for (j=j2;j>=j1;j--)
						{
							if (j>0 && j<cy)
								if(*p_work1y!=0)
								{
									*p_p_work1y=p_work1y;
									return true;
								}
								
								p_work1y-=cx3;
						}
						else
							p_work1y-=(SizeSq+SizeSq+1)*cx3;
						p_work1y+=cx3;
						///////////////////////////////////////////////
						
						SizeSq++; 
						flag++;
						//		if (p_work1y!=ppp)
						//			ppp=ppp;
	}
	
	return false;
}

BYTE CColorSeg::FindNoZerPoint1(int cx,int cy,int ic,int jc,BYTE *p_work1 ,BYTE *p_work1y)
// Найти ближайшую ненулевую точку байтового изображения;
// Вернуть ее значение.
{
	int flag=0;
	int i1,j1,i2,j2,i,j;
	int SizeSq=1;
	
	while (flag<50)
	{
		
		p_work1y-=(cx+1);
		
		i1=ic-SizeSq;
		i2=ic+SizeSq;
		j1=jc-SizeSq;
		j2=jc+SizeSq;
		
		/////////////////////////////////////////////
		if (j1>0)
		{
			for (i=i1;i<=i2;i++)
			{
				if (i>0 && i<cx)
					if(*p_work1y!=0)											
						return *p_work1y;
					p_work1y++;
			}
		}
		else p_work1y+=(SizeSq+SizeSq+1);
		p_work1y--;
		///////////////////////////////////////////////
		if (i2<cx)
		{
			for (j=j1;j<=j2;j++)
			{
				if (j>0 && j<cy)
					if(*p_work1y!=0)
						return *p_work1y;
					p_work1y+=cx;
			}
		}
		else p_work1y+=(SizeSq+SizeSq+1)*cx;
		p_work1y-=cx;
		///////////////////////////////////////////////
		if (j2<cy)
		{
			for (i=i2;i>=i1;i--)
			{
				if (i>0 && i<cx)
					if(*p_work1y!=0)
						return *p_work1y;
					p_work1y--;
			}
		}
		else
			p_work1y-=(SizeSq+SizeSq+1);
		p_work1y++;
		///////////////////////////////////////////////
		if (i1>0)
		{
			for (j=j2;j>=j1;j--)
			{
				if (j>0 && j<cy)
					if(*p_work1y!=0)
						return *p_work1y;
					p_work1y-=cx;
			}
		}
		else p_work1y-=(SizeSq+SizeSq+1)*cx;
		p_work1y+=cx;
		///////////////////////////////////////////////
		
		SizeSq++; 
		flag++;
		//		if (p_work1y!=ppp)
		//			ppp=ppp;
	}
	
	return 0;
}


bool CColorSeg::FindMed(int x0,int y0,int z0,int r,double *xm0,double *ym0,double *zm0,int *P0)
// Вернуть среднее взвешенное цветов из r-окрестности (x0,y0,z0)
// Результат - (xm0,ym0,zm0), кол-во пикселей в окрестности - P0
{
	DWORD P=0;
	BYTE D=2*r+1;
	
	int x,y,z,x1,y1,z1,x2,y2,z2;
	DWORD xm=0;
	DWORD ym=0;
	DWORD zm=0;
	
	if (x0<r) x0=r;
	if (x0>64-r) x0=64-r;
	if (y0<r) y0=r;
	if (y0>64-r) y0=64-r;
	if (z0<r) z0=r;
	if (z0>64-r) z0=64-r;
	
	x1=x0-r;
	x2=x0+r;
	y1=y0-r;
	y2=y0+r;
	z1=z0-r;
	z2=z0+r;
	
	BYTE iRow = 65;
	WORD iUp = 4225;
	
	
	for (z=z1;z<=z2;z++)
	{
		p_Gistz=p_Gist+z*iUp+y1*iRow+x1;
		for (y=y1;y<=y2;y++)
		{
			for (x=x1;x<=x2;x++)
			{
				xm+=(*p_Gistz)*x;
				ym+=(*p_Gistz)*y;
				zm+=(*p_Gistz)*z;
				P+=*p_Gistz++;
			}
			p_Gistz+=(iRow-D);
		}
	}
	
	if (P!=0)
	{
		*xm0=(double)xm/P;
		*ym0=(double)ym/P;
		*zm0=(double)zm/P;
		*P0=P;
	}
	else
	{
		*xm0=(double)x0;
		*ym0=(double)y0;
		*zm0=(double)z0;
		*P0=P;
	}
	
	return true;
}

bool CColorSeg::FindProb(int x0,int y0,int z0,int r,int *P0)
// Получить количество точек с цветом в r-окрестности точки (x0,y0,z0)
// результат - в P0.
{
	DWORD P=0;
	BYTE D=2*r+1;
	
	int x,y,z,x1,y1,z1,x2,y2,z2;
	
	if (x0<r) x0=r;
	if (x0>64-r) x0=64-r;
	if (y0<r) y0=r;
	if (y0>64-r) y0=64-r;
	if (z0<r) z0=r;
	if (z0>64-r) z0=64-r;
	
	x1=x0-r;
	x2=x0+r;
	y1=y0-r;
	y2=y0+r;
	z1=z0-r;
	z2=z0+r;
	
	BYTE iRow = 65;
	WORD iUp = 4225;
	
	
	for (z=z1;z<=z2;z++)
	{
		p_Gistz=p_Gist+z*iUp+y1*iRow+x1;
		for (y=y1;y<=y2;y++)
		{
			for (x=x1;x<=x2;x++)
			{
				P+=*p_Gistz++;
			}
			p_Gistz+=(iRow-D);
		}
	}
	
	*P0=P;
	
	return true;
}

bool CColorSeg::FindMax(int x0,int y0,int z0,int r,double dmin,double d1,
						int *xmax,int *ymax,int *zmax,int *P0)
// Найти соответствующий точке (x0,y0,z0) локальный максимум гистограммы.
// (движемся от точки в сторону возрастания плотности)
// Результат - (xmax,ymax,zmax), кол-во точек в окрестности - P0
{
	double xm,ym,zm,x,y,z,x1,y1,z1,k;
	int P;
	int res;
	double delta;
	int Count=0;
	x=x0;
	y=y0;
	z=z0;
	k=1;
	//	delta=100000;
	
lab:
	
	x1=(int)(x+0.5);
	y1=(int)(y+0.5);
	z1=(int)(z+0.5);
	
	res = FindMed((int)x1,(int)y1,(int)z1,r,&xm,&ym,&zm,&P);
	
	if (Count++>300)
	{
		*xmax=(int)xm;
		*ymax=(int)ym;
		*zmax=(int)zm;
		*P0=P;
		return true;
	}
	
	//	delta1=delta;
	
	delta=sqrt((x1-xm)*(x1-xm)+(y1-ym)*(y1-ym)+(z1-zm)*(z1-zm));
	
	//	if (delta>delta1)
	//		k=k/2;
	
	if (delta>dmin)
	{
		if (delta<d1)
		{
			x=xm+(xm-x1)+(xm-x1);
			y=ym+(ym-y1)+(ym-y1);
			z=zm+(zm-z1)+(zm-z1);
		}
		else
		{
			x=x+(xm-x1);//*k;
			y=y+(ym-y1);//*k;
			z=z+(zm-z1);//*k;
		}
		goto lab;
	}
	
    *xmax=(int)xm;
	*ymax=(int)ym;
	*zmax=(int)zm;
	*P0=P;
	return true;
}

bool CColorSeg::ZerroSpher(int x0,int y0,int z0,int r)
//Обнулить участок гистограммы (r-окрестность точки (x0,y0,z0))
{
	BYTE D=2*r+1;
	
	int x,y,z,x1,y1,z1,x2,y2,z2;
	
	if (x0<r) x0=r;
	if (x0>64-r) x0=64-r;
	if (y0<r) y0=r;
	if (y0>64-r) y0=64-r;
	if (z0<r) z0=r;
	if (z0>64-r) z0=64-r;
	
	x1=x0-r;
	x2=x0+r;
	y1=y0-r;
	y2=y0+r;
	z1=z0-r;
	z2=z0+r;
	
	BYTE iRow = 65;
	WORD iUp = 4225;
	
	for (z=z1;z<=z2;z++)
	{
		p_Gistz=p_Gist+z*iUp+y1*iRow+x1;
		for (y=y1;y<=y2;y++)
		{
			for (x=x1;x<=x2;x++)
			{
				*p_Gistz++=0;
			}
			p_Gistz+=(iRow-D);
		}
	}
	
	return true;
}

void CColorSeg::DeleteSmolSegments(BYTE* p_Image,int cx,int cy,int Nmin,BYTE *p_VectCol)
//Для областей с площадью больше заданной задать
//p_VectCol[Color-1]=Color;
//попутно портит (обнуляет) Image
{
	int y,x,k,i,j,Count1,Count2;
	BYTE *p_Imagey;
	BYTE Color;
	DWORD *p_Count1 = new DWORD[cx*cy/5];
	DWORD *p_Count2 = new DWORD[cx*cy/5];
	int *di = new int[8]; //directions
	di[0] = -cx;
	di[1] =  1;
	di[2] =  cx;
	di[3] = -1;
	di[4] = -cx+1;
	di[5] =  cx+1;	
	di[6] =  cx-1;	
	di[7] = -cx-1;
	
	//заполнить края Image нулями
	p_Imagey=p_Image;
	for (y=0;y<cy;y++)
	{
		*p_Imagey=0;
		*(p_Imagey+cx-1)=0;
		p_Imagey+=cx;
	}
	
	//заполнить края Image нулями
	p_Imagey=p_Image;
	for (x=0;x<cx;x++)
		*p_Imagey++=0;
	
	//заполнить края Image нулями
	p_Imagey=p_Image+cx*(cy-1);
	for (x=0;x<cx;x++)
		*p_Imagey++=0;
	
	for (y=1;y<cy-1;y++)
	{
		p_Imagey = p_Image+y*cx+1;
		for (x=1;x<cx-1;x++)
		{	//идем по всем ненулевым внутренним точкам Image
			if(*p_Imagey!=0)
			{
				Color=*p_Imagey;
				*p_Imagey=0;
				k=1;
				Count1=1;
				p_Count1[0]=p_Imagey-p_Image; //"указатель" на текущую точку
				do
				{
					Count2=0;
					//AAM разобраться в этом цикле. Вроде что-то типа заливки
					//(точнее, поиск связной области).
					//Возможно, поддается оптимизации
					for (i=0;i<(int)Count1;i++)
						for (j=0;j<4;j++)
							if(*(p_Image+p_Count1[i]+di[j])==Color)
							{
								p_Count2[Count2] = p_Count1[i]+di[j];
								*(p_Image+p_Count2[Count2])=0;
								k++;
								Count2++;
							}
							Count1=Count2;
							CopyMemory(p_Count1,p_Count2,Count2*sizeof(DWORD));
				}while(Count2);	
				if (k>Nmin)
					p_VectCol[Color-1]=Color;
			}
			p_Imagey++;
		}
	}
	
	delete p_Count1;
	delete p_Count2;
	delete di;
	
}

void CColorSeg::NconDelete(BYTE *p_DImage,BYTE *p_DImage1,int cx,int cy,int VectCount,int Ncon)
{
	int y,x,k,i,j,Count1,Count2;
	BYTE *p_DImagey;
	BYTE *p_DImage1y;
	DWORD p_Pix;
	BYTE Color;
	DWORD *p_Count1 = new DWORD[cx*cy/5];
	DWORD *p_Count2 = new DWORD[cx*cy/5];
	int *di = new int[8];
	di[0] = -cx;	
	di[1] =  1;
	di[2] =  cx;
	di[3] = -1;
	di[4] = -cx+1;
	di[5] =  cx+1;	
	di[6] =  cx-1;	
	di[7] = -cx-1;
	
	//края DImage и DImage1 забиваем нулями.
	p_DImagey=p_DImage;
	p_DImage1y=p_DImage1;
	for (y=0;y<cy;y++)
	{
		*p_DImagey=0;
		*(p_DImagey+cx-1)=0;
		p_DImagey+=cx;
		
		*p_DImage1y=0;
		*(p_DImage1y+cx-1)=0;
		p_DImage1y+=cx;
	}
	
	p_DImagey=p_DImage;
	p_DImage1y=p_DImage1;
	for (x=0;x<cx;x++)
	{
		*p_DImagey++=0;
		*p_DImage1y++=0;
	}
	
	p_DImagey=p_DImage+cx*(cy-1);
	p_DImage1y=p_DImage1+cx*(cy-1);
	for (x=0;x<cx;x++)
	{
		*p_DImagey++=0;
		*p_DImage1y++=0;
	}
	
	//По всей внутренней области
	for (y=1;y<cy-1;y++)
	{
		p_DImagey = p_DImage+y*cx+1;			  
		for (x=1;x<cx-1;x++)
		{	
			if(*p_DImagey!=0)
			{	
				p_Pix=p_DImagey-p_DImage;
				Color=*p_DImagey;
				*p_DImagey=0;
				k=1;
				Count1=1;
				p_Count1[0]=p_DImagey-p_DImage;
				do
				{ 
					Count2=0;
					for (i=0;i<(int)Count1;i++)
						for (j=0;j<4;j++)
							if(*(p_DImage+p_Count1[i]+di[j])==Color)
							{
								p_Count2[Count2] = p_Count1[i]+di[j];
								*(p_DImage+p_Count2[Count2])=0;
								k++;
								Count2++;
							}
							Count1=Count2;
							CopyMemory(p_Count1,p_Count2,Count2*sizeof(DWORD));
				}while(Count2);	
				if (k<Ncon)
				{
					p_DImage1y=p_DImage1+p_Pix;
					*p_DImage1y=0;
					Count1=1;
					p_Count1[0]=p_DImage1y-p_DImage1;
					do
					{ 
						Count2=0;
						for (i=0;i<(int)Count1;i++)
							for (j=0;j<4;j++)
								if(*(p_DImage1+p_Count1[i]+di[j])==Color)
								{
									p_Count2[Count2] = p_Count1[i]+di[j];
									*(p_DImage1+p_Count2[Count2])=0;									
									Count2++;
								}
								Count1=Count2;
								CopyMemory(p_Count1,p_Count2,Count2*sizeof(DWORD));
					}while(Count2);	
				}
				
			}
			p_DImagey++;
		}
	}
	
	
	delete p_Count1;
	delete p_Count2;
	delete di;
}

void CColorSeg::FillNcon(BYTE *p_DImage,BYTE *p_DImage1,int cx,int cy)
// заполнить DImage1 - для каждой точки - ближайшее (по x,y) ненулевое
// значение из DImage
{
	int y,x;
	BYTE *p_DImagey;	
	BYTE *p_DImage1y;
	
	//значения DImage1 на краях изображения - с соседних точек.
	p_DImage1y=p_DImage1;
	for (y=0;y<cy;y++)
	{
		*p_DImage1y=*(p_DImage1y+1);
		*(p_DImage1y+cx-1)=*(p_DImage1y+cx-2);
		p_DImage1y+=cx;
	}
	
	//значения DImage1 на краях изображения - с соседних точек.
	p_DImage1y=p_DImage1;
	for (x=0;x<cx;x++)
	{
		*p_DImage1y=*(p_DImage1y+cx);
		p_DImage1y++;
	}
	
	//значения DImage1 на краях изображения - с соседних точек.
	p_DImage1y=p_DImage1+cx*(cy-1);
	for (x=0;x<cx;x++)
	{
		*p_DImage1y=*(p_DImage1y-cx);
		p_DImage1y++;
	}
	
	//Для каждой точки - поиск ближайшей ненулевой в DImage.
	//наверняка можно соптимизировать.
	for (y=0;y<cy;y++)
	{
		p_DImagey = p_DImage+y*cx;			  
		p_DImage1y = p_DImage1+y*cx;			  
		
		for (x=0;x<cx;x++)
		{
			if (*p_DImage1y==0)		  
				*(p_DImage1y)=FindNoZerPoint1(cx,cy,x,y,p_DImage,p_DImagey);
			p_DImagey++;
			p_DImage1y++;
		}
	}
}

bool CColorSeg::InvokeFilter()
{
	
	DWORD t0,t1,t2,t3,t4;
	//	t0=GetTickCount();
	
	color MaxCol = 65535;
	// Константы:
	int RadiusSmol = 1;// 4;  2  // Радиус маленькой сферы 
	int Radius = 5;    // 7; 4  // Радиус сферы
	int Radius2 = (int)((double)Radius*1.5);
	int Nmin=400;
	int Nmin1=400;
	int Ncon = 30;
	DWORD CountLim = 10;
	int SegColors=1;
	
	m_Preset=GetArgLong(szPreset);
	m_SegColors=GetArgLong(szSegColors)!=0;
	m_Radius=GetArgLong(szRadius);
	m_Nmin=GetArgLong(szNmin);
	m_Nmin1=GetArgLong(szNmin1);
	m_Ncon=GetArgLong(szNcon);
	m_CountLim=GetArgLong(szCountLim);

	IImage2Ptr pArg	= GetDataArgument();
	//IImage2Ptr pResult	= GetDataResult();
	IBinaryImagePtr pResult	= GetDataResult();
	
	if( pArg ==0 || pResult ==0 )
		return false;
	
	SIZE size;
	pArg->GetSize((int *)(&size.cx), (int *)(&size.cy));
	POINT ptOffset;
	pArg->GetOffset(&ptOffset);
	
	//BSTR bstrName;
	//IColorConvertor2* pIClrConv;
	//pArg->GetColorConvertor((IUnknown**)&pIClrConv);
	//pIClrConv->GetCnvName(&bstrName);
	//pResult->CreateImage(size.cx, size.cy,bstrName); 
	//pResult->SetOffset( ptOffset, TRUE);
	//pIClrConv->Release();
	//SysFreeString(bstrName);
	
	pResult->CreateNew(size.cx, size.cy);
	pResult->SetOffset( ptOffset, TRUE);

	StartNotification( size.cy, 10 );
	
    int i, i1, j, j1, x, y, z, xm, ym, zm;  
	int cx = size.cx;
	int cy = size.cy;
	int cx3 = cx*3;
	//	byte *pmask;
	int VectCount=0;

	//если выставлен пресет - задать соответствующие значения остальных параметров
	switch(m_Preset)
	{
	case 1:
		m_Radius = 5;
		m_Nmin=400;
		m_Nmin1=200;
		m_Ncon = 30;
		m_CountLim=5;
		break;
	case 2:
		m_Radius = 5;
		m_Nmin=200;
		m_Nmin1=100;
		m_Ncon = 30;
		m_CountLim=8;
		break;
	case 3:
		m_Radius = 5; 
		m_Nmin=100;
		m_Nmin1=50;
		m_Ncon = 15;
		m_CountLim=13;
		break;
	}

	Radius=m_Radius;
	Nmin=m_Nmin*cx*cy/(300*300);
	Nmin1=m_Nmin1;
	Ncon=m_Ncon;
	CountLim=m_CountLim;
	Radius2 = (int)((double)Radius*1.5);
	
	if (m_SegColors)
		SegColors=0;
	else
		SegColors=1;
	
	t0=GetTickCount();
	
	
	BYTE *p_Vectx = new BYTE[300];
	BYTE *p_Vecty = new BYTE[300];
	BYTE *p_Vectz = new BYTE[300];
	BYTE *p_VectCol = new BYTE[300];
	DWORD *p_PVect = new DWORD[300];
	
	BYTE *p_work0 = new BYTE[cx3*cy+3];
	BYTE *p_work1 = new BYTE[cx3*cy+3];
	BYTE *p_work2 = new BYTE[cx3*cy+3];	
	BYTE  *p_DImage = new BYTE[cx*cy+1];
	BYTE  *p_DImage1 = new BYTE[cx*cy+1];
	BYTE  *p_DImagey; 
	BYTE  *p_DImage1y;
	BYTE *p_work0y;
	BYTE *p_work1y;
	
	ZeroMemory(p_DImage,cx*cy+1);
	
	// Определим пределы изменения L,a,b
	WORD *AGist = new WORD[66000];
	WORD *BGist = new WORD[66000];
	WORD *LGist = new WORD[66000];
	ZeroMemory(AGist,66000*sizeof(WORD));
	ZeroMemory(BGist,66000*sizeof(WORD));
	ZeroMemory(LGist,66000*sizeof(WORD));
	
	//построим гистограммы (пока независимые) по L,a,b
	for (j=0;j<cy;j++)
	{		
		color *pArg0, *pArg1, *pArg2;
		pArg->GetRow(j,0,&pArg0);
		pArg->GetRow(j,1,&pArg1);
		pArg->GetRow(j,2,&pArg2);
		int a,b;
		color hh;
		for (i=0;i<cx;i++)
		{		
			hh=*pArg1;
			a=*pArg0-hh+32768;
			b=hh-*pArg2+32768;
			if (a<0) a=0;
			if (a>MaxCol) a=MaxCol;
			if (b<0) b=0;
			if (b>MaxCol) b=MaxCol;
			
			AGist[a]++;
			BGist[b]++;
			LGist[(int)((*pArg1+*pArg2+*pArg0)/3)]++;
			pArg0++;
			pArg1++;
			pArg2++;
		}
		Notify(j);
	}
	NextNotificationStage();
	
	WORD min_a,max_a,min_b,max_b,min_l,max_l;
	DWORD sum_a,sum_b,sum_l,porog;
	porog=cx*cy/50;
	
	sum_a=0;
	for (min_a=0;min_a<MaxCol;min_a++)
	{
		sum_a+=AGist[min_a];
		if (sum_a>porog)
			break;
	}
	
	sum_a=0;
	for (max_a=MaxCol;max_a>=0;max_a--)
	{
		sum_a+=AGist[max_a];
		if (sum_a>porog)
			break;
	}
	
	sum_b=0;
	for (min_b=0;min_b<MaxCol;min_b++)
	{
		sum_b+=BGist[min_b];
		if (sum_b>porog)
			break;
	}
	
	sum_b=0;
	for (max_b=MaxCol;max_b>=0;max_b--)
	{
		sum_b+=BGist[max_b];
		if (sum_b>porog)
			break;
	}
	
	sum_l=0;
	for (min_l=0;min_l<MaxCol;min_l++)
	{
		sum_l+=LGist[min_l];
		if (sum_l>porog)
			break;
	}
	
	sum_l=0;
	for (max_l=MaxCol;max_l>=0;max_l--)
	{
		sum_l+=LGist[max_l];
		if (sum_l>porog)
			break;
	}
	
	delete AGist;
	delete BGist;
	delete LGist;
	// end Определим пределы изменения L,a,b
	
	double K_a = (double)MaxCol/(double)abs(max_a-min_a);
	double K_b = (double)MaxCol/(double)abs(max_b-min_b);
	double K_l = (double)MaxCol/(double)abs(max_l-min_l);
	int delt_a = min_a;
	int delt_b = min_b;
	int delt_l = min_l;
	K_a=K_a/1024;
	K_b=K_b/1024;
	K_l=K_l/1024;
	
	
	delt_a=delt_a-32768;
	delt_b=delt_b-32768;
	
	
	int A,B,L;
	
	//	t1=GetTickCount();
	
	//заполним изображения в Lab (конверченные как надо -
	//чтобы растянуть гистограмму)
	if (SegColors==0)
	{
		for (j=0;j<cy;j++)
		{
			p_work0y=p_work0+j*cx3;
			p_work1y=p_work1+j*cx3;
			color *pArg0, *pArg1, *pArg2;
			pArg->GetRow(j,0,&pArg0);
			pArg->GetRow(j,1,&pArg1);
			pArg->GetRow(j,2,&pArg2);
			color h0,h1,h2;		
			for (i=0;i<cx;i++)
			{
				h0=*pArg0;
				h1=*pArg1;
				h2=*pArg2;
				L=(int)(((h0+h1+h2)/3-delt_l)*K_l)+1;
				A=(int)((h0-h1-delt_a)*K_a)+1;
				B=(int)((h1-h2-delt_b)*K_b)+1;
				if (A<1) A=1;
				if (B<1) B=1;
				if (A>64) A=64;
				if (B>64) B=64;
				*p_work0y++=L;
				*p_work0y++=A;
				*p_work0y++=B;
				*p_work1y++=L;
				*p_work1y++=A;
				*p_work1y++=B;
				pArg0++;
				pArg1++;
				pArg2++;
			}
			Notify(j);
		}
	}
	else
	{
		for (j=0;j<cy;j++)
		{
			p_work0y=p_work0+j*cx3;
			p_work1y=p_work1+j*cx3;
			color *pArg0, *pArg1, *pArg2;
			pArg->GetRow(j,0,&pArg0);
			pArg->GetRow(j,1,&pArg1);
			pArg->GetRow(j,2,&pArg2);
			color h0,h1,h2;		
			for (i=0;i<cx;i++)
			{
				h0=*pArg0;
				h1=*pArg1;
				h2=*pArg2;
				L=33;
				A=(int)((h0-h1-delt_a)*K_a)+1;
				B=(int)((h1-h2-delt_b)*K_b)+1;
				if (A<1) A=1;
				if (B<1) B=1;
				if (A>64) A=64;
				if (B>64) B=64;
				*p_work0y++=L;
				*p_work0y++=A;
				*p_work0y++=B;
				*p_work1y++=L;
				*p_work1y++=A;
				*p_work1y++=B;
				pArg0++;
				pArg1++;
				pArg2++;
			}
			Notify(j);
		}
	}
	NextNotificationStage();
	
	int *di = new int[9];
	int *diy;
	di[0] = -cx3;
	di[1] = -cx3+3;
	di[2] =  3;
	di[3] =  cx3+3;
	di[4] =  cx3;
	di[5] =  cx3-3;
	di[6] = -3;
	di[7] = -cx3-3;
	di[8] = -cx3;
	
	
	int *di1 = new int[9];
	int *di1y;
	di1[0] = -cx;
	di1[1] = -cx+1;
	di1[2] =  1;
	di1[3] =  cx+1;
	di1[4] =  cx;
	di1[5] =  cx-1;
	di1[6] = -1;
	di1[7] = -cx-1;
	di1[8] = -cx;
	p_Gist = new WORD[280000];
	ZeroMemory(p_Gist,274625*sizeof(WORD));
	iRow = 65;
	iUp = 4225;
	
	// Заполнение гистограммы
	
	for (j=0;j<cy;j++)
	{
		p_work1y=p_work1+j*cx3;
		for (i=0;i<cx;i++)
		{	
			x=(*p_work1y++);
			y=(*p_work1y++);
			z=(*p_work1y++);
			p_Gistyz=p_Gist+x+y*iRow+z*iUp;
			(*p_Gistyz)++;			
		}
		Notify(j);
	}
	NextNotificationStage();
	
	//  Поиск сегментов
	
	int path5y = cy/5;
	int path5x = cx/5;
	int path5x3 = path5x*3;
	int P,Pmax;   
	BYTE *p_col1;
	DWORD Count=0;
	
	//  Поиск точки с максимальной плотностью
	t1=GetTickCount();
	
newround:

	Notify((VectCount)*cy/255);
	
	Pmax = 0;
	for (j=1;j<cy-1;j+=path5y)
	{
		p_work1y=p_work1+j*cx3+3;
		for (i=1;i<cx-1;i+=path5x)
		{
			//AAM можно подсократить - тут одно и то же два раза...
			if (*p_work1y!=0)			
			{
				j1=1;
				diy=di;
				
				x=*p_work1y;
				y=*(p_work1y+1);
				z=*(p_work1y+2);
				
				for (i1=0;i1<8;i1++)
					if (*(p_work1y+*diy++)!=0)
					{
						x+=*(p_work1y+*(diy-1));
						y+=*(p_work1y+*(diy-1)+1);
						z+=*(p_work1y+*(diy-1)+2);
						j1++;
					}
					x=x/j1;
					y=y/j1;
					z=z/j1;
					
					FindProb(x,y,z,RadiusSmol,&P); 
					if (P>Pmax)
					{
						Pmax=P;
						xm=x;
						ym=y;
						zm=z;
					}
					
					p_work1y+=path5x3;
			}
			else
			{
				p_col1=p_work1y;
				if (FindNoZerPoint(cx,cy,i,j,p_work1,&p_work1y)==true)
				{
					
					j1=1;
					diy=di;
					
					x=*p_work1y;
					y=*(p_work1y+1);
					z=*(p_work1y+2);
					
					//					if (*p_work1y==0)
					//						i1=i1;
					
					for (i1=0;i1<8;i1++)
						if (*(p_work1y+*diy++)!=0)
						{
							x=+*(p_work1y+*(diy-1));
							y=+*(p_work1y+*(diy-1)+1);
							z=+*(p_work1y+*(diy-1)+2);
							j1++;
						}
						x=x/j1;
						y=y/j1;
						z=z/j1;
						
						FindProb(x,y,z,RadiusSmol,&P); 
						if (P>Pmax)
						{
							Pmax=P;
							xm=x;
							ym=y;
							zm=z;
						}
				}
				
				p_work1y=p_col1;
				p_work1y+=path5x3;
			}
		}		
	}
	
	x=xm;
	y=ym;
	z=zm;
	
	FindMax(x,y,z,Radius,0.01,0.2,&xm,&ym,&zm,&P);
	
	if (VectCount>254)
		goto out;
	
	if (P<Nmin)
		goto out;
	
	p_Vectx[VectCount]=xm;
	p_Vecty[VectCount]=ym;
	p_Vectz[VectCount]=zm;
	p_PVect[VectCount]=P;
	VectCount++;
	
	// Обнуление точек
	
	// Обнуление точек на изображении  +
	// Установка точек на сегментированном изображении
	for (j=1;j<cy-1;j++)
	{
		p_work1y=p_work1+j*cx3+3;
		for (i=1;i<cx-1;i++)
		{	
			x=*p_work1y;
			y=*(p_work1y+1);
			z=*(p_work1y+2);
			
			if (x!=0/* && y!=0 && z!=0*/)
			{				
				if (max(max(abs(x-xm),abs(y-ym)),abs(z-zm))<Radius)
				{					
					*p_work1y=0;
					*(p_work1y+1)=0;
					*(p_work1y+2)=0;
					
					diy=di;
					for (i1=0;i1<8;i1++)					
					{
						*(p_work1y+*diy)=0;
						*(p_work1y+*diy+1)=0;
						*(p_work1y+*diy+2)=0;				
						diy++;
					}
					for (j1=j-1;j1<j+2;j1++) 
					{
						p_DImagey=p_DImage+cx*j1+i-1;
						for (i1=i-1;i1<i+2;i1++) 
							*p_DImagey++=VectCount;
					}
				}
			}
			
			p_work1y+=3;
		}
	}
	
	// Обнуление точек в гистограмме
	ZerroSpher(xm,ym,zm,Radius);
	
	goto newround;
	
	
out:
	NextNotificationStage();
	
	t2=GetTickCount();
	
	// Удаление мелких сегментов
	
	for (i=0;i<300;i++)  p_VectCol[i]=255;
	
	DeleteSmolSegments(p_DImage,cx,cy,Nmin1,p_VectCol);

	NextNotificationStage();
	
	//  count limit
	Count=0;
	for (i=0;i<VectCount;i++)
		if (p_VectCol[i]!=255)
			Count++;
		if (Count>CountLim)
			for (i=0;i<(int)(Count-CountLim);i++)
			{
				Pmax=100000;
				for (j=0;j<VectCount;j++)
					if (p_VectCol[j]!=255)
						if (p_PVect[j]<(DWORD)Pmax)
							j1=j;
						p_VectCol[j1]=255;
			}
			
			
	// Удаление цветовых векторов
	
	for (i=0;i<VectCount;i++)
	if (p_VectCol[i]==255)
	{
		for (j=i+1;j<300;j++)
		{
			p_VectCol[j-1]=p_VectCol[j];
			p_Vectx[j-1]=p_Vectx[j];
			p_Vecty[j-1]=p_Vecty[j];
			p_Vectz[j-1]=p_Vectz[j];
		}
		VectCount--;
		i--;
	}
	for (i=0;i<VectCount;i++)
		p_VectCol[i]=i+1;
	
	
	ZeroMemory(p_DImage,cx*cy+1);
	

	// Установка точкам сегментированных цветов
	
	CopyMemory(p_work1,p_work0,cx3*cy+3); 
	
	for (j=0;j<cy;j++)
	{
		Notify(j);
		p_work1y=p_work1+j*cx3;
		p_DImagey=p_DImage+j*cx;
		for (i=0;i<cx;i++)
		{	
			x=*p_work1y++;
			y=*p_work1y++;
			z=*p_work1y++;
			
			for (i1=0;i1<VectCount;i1++)
				if( max( max(abs(x-p_Vectx[i1]),abs(y-p_Vecty[i1])),abs(z-p_Vectz[i1]) ) 
					<= Radius)
				{
					*p_DImagey=p_VectCol[i1];
					i1=VectCount;
				}
				p_DImagey++;
		}
	}
	NextNotificationStage();
	
	CopyMemory(p_DImage1,p_DImage,cx*cy+1); //////???????
	
	BYTE a;
	
	for (j=1;j<cy-1;j++)
	{
		p_work1y=p_work1+j*cx3+3;
		p_DImagey=p_DImage+j*cx+1;
		p_DImage1y=p_DImage1+j*cx+1;
		for (i=1;i<cx-1;i++)			
		{	
			if(*p_DImagey==0)
			{
				di1y=di1;
				for (i1=0;i1<8;i1++)
					if (*(p_DImagey+*di1y++)!=0)
					{
						a=*(p_DImagey+*(di1y-1));
						x=*p_work1y;
						y=*(p_work1y+1);
						z=*(p_work1y+2);
						if( max( max(abs(x-p_Vectx[a-1]),abs(y-p_Vecty[a-1])),abs(z-p_Vectz[a-1]) ) 
							<= Radius2 )
						{
							*p_DImage1y=a;
							i1=10;
						}
					}
					
			}
			p_DImagey++;
			p_DImage1y++;
			p_work1y+=3;
		}
		Notify(j);
	}
	NextNotificationStage();
	
	// Задается цвет оставшимся точкам ( самый ближний )
	
	CopyMemory(p_DImage,p_DImage1,cx*cy+1); //////???????
	
	double mindel; 
	
	for (j=0;j<cy;j++)
	{
		p_work1y=p_work1+j*cx3;
		p_DImagey=p_DImage+j*cx;
		for (i=0;i<cx;i++)			
		{	
			if(*p_DImagey==0)
			{		
				x=*p_work1y;
				y=*(p_work1y+1);
				z=*(p_work1y+2);
				mindel=1000;
				
				for (i1=0;i1<VectCount;i1++)  if( sqrt((double)((x-p_Vectx[i1])*(x-p_Vectx[i1])+
					(y-p_Vecty[i1])*(y-p_Vecty[i1])+
					(z-p_Vectz[i1])*(z-p_Vectz[i1]))) 
					<= mindel)
				{
					mindel=sqrt((double)((x-p_Vectx[i1])*(x-p_Vectx[i1])+
						(y-p_Vecty[i1])*(y-p_Vecty[i1])+
						(z-p_Vectz[i1])*(z-p_Vectz[i1])));
					*p_DImagey=p_VectCol[i1];  //  =i1+1;
				}
				
			}
			p_DImagey++;
			p_work1y+=3;
		}
		Notify(j);
	}
	NextNotificationStage();
	
	t3=GetTickCount();
	
	if (VectCount>1)
	{
		CopyMemory(p_DImage1,p_DImage,cx*cy+1);
		NconDelete(p_DImage1,p_DImage,cx,cy,VectCount,Ncon);
		
		CopyMemory(p_DImage1,p_DImage,cx*cy+1);
		FillNcon(p_DImage1,p_DImage,cx,cy);
	}
	
	DWORD *p_dVectx = new DWORD[300];
	DWORD *p_dVecty = new DWORD[300];
	DWORD *p_dVectz = new DWORD[300];
	DWORD *p_dVectCount = new DWORD[300];
	ZeroMemory(p_dVectx,300*sizeof(DWORD));
	ZeroMemory(p_dVecty,300*sizeof(DWORD));
	ZeroMemory(p_dVectz,300*sizeof(DWORD));
	ZeroMemory(p_dVectCount,300*sizeof(DWORD));
	
	for (j=0;j<cy;j++)
	{		
		p_DImagey=p_DImage+cx*j;
		color *pArg0, *pArg1, *pArg2;
		pArg->GetRow(j,0,&pArg0);
		pArg->GetRow(j,1,&pArg1);
		pArg->GetRow(j,2,&pArg2);
		for (i=0;i<cx;i++)
		{	
			p_dVectx[*p_DImagey]+=(*pArg0++)/255;
			p_dVecty[*p_DImagey]+=(*pArg1++)/255;
			p_dVectz[*p_DImagey]+=(*pArg2++)/255;
			p_dVectCount[*p_DImagey++]++;
		}
		Notify(j);
	}
	NextNotificationStage();
	
	for (i=0;i<300;i++)  if(p_dVectCount[i]>0)
	{
		p_dVectx[i]=p_dVectx[i]/p_dVectCount[i]*255;
		p_dVecty[i]=p_dVecty[i]/p_dVectCount[i]*255;
		p_dVectz[i]=p_dVectz[i]/p_dVectCount[i]*255;
	}

	DWORD dwDefColor = RGB(255,255,0);
	color Pal_R[256];
	color Pal_G[256];
	color Pal_B[256];
	long nCounter = GetValueInt(GetAppUnknown(), "\\Binary", "IndexedColorsCount", 1);
	nCounter = max(0, min(256, nCounter));
	char str[200];
	for(i = 0; i < 256; i++)
	{
		DWORD c;
		if(i < nCounter)
		{
			sprintf(str,"Fore_%d", i);
			c = GetValueColor(GetAppUnknown(), "\\Binary", str, dwDefColor);
		}
		else c=dwDefColor;
		Pal_R[i]=byte(c&0xFF);
		Pal_G[i]=byte((c>>8)&0xFF);
		Pal_B[i]=byte((c>>16)&0xFF);
	}
	
	byte pal[256];
	int min_dist[256];
	bool color_free[256];
	for(i=0; i<256; i++)
	{
		pal[i]=254-i; //на всякий случай - если цветов в палитре не хватит
		min_dist[i]=256*256*256; //заведомо больше максимума
		color_free[i]=true;
	}
	for(i=0; i<=VectCount; i++)
	{
		for(j=0; j<255; j++)
		{
			if(color_free[j])
			{
				int r=int(p_dVectx[i]/256)-Pal_R[j];
				int g=int(p_dVecty[i]/256)-Pal_G[j];
				int b=int(p_dVectz[i]/256)-Pal_B[j];
				int d=r*r+g*g+b*b+(r+g+b)*(r+g+b);
				if(d<min_dist[i])
				{
					pal[i]=j;
					min_dist[i]=d;
				}
			}
		}
		color_free[pal[i]]=false;
	}

	for (j=0;j<cy;j++)
	{
		p_DImagey=p_DImage+cx*j;
		//color *pRes0, *pRes1, *pRes2;
		//pResult->GetRow(j, 0, &pRes0);
		//pResult->GetRow(j, 1, &pRes1);
		//pResult->GetRow(j, 2, &pRes2);
		byte *pRes;
		pResult->GetRow(&pRes, j, false);
		for (i=0;i<cx;i++)
		{	
			//*pRes0++=(color)p_dVectx[*p_DImagey];
			//*pRes1++=(color)p_dVecty[*p_DImagey];
			//*pRes2++=(color)p_dVectz[*p_DImagey++];
			*pRes++=pal[*p_DImagey++]+1;
		}
		Notify(j);
	}  
	
	delete p_dVectx;
	delete p_dVecty;
	delete p_dVectz;
	delete p_dVectCount;
	
	t4=GetTickCount();
	
	
	FinishNotification();
	delete p_work0 ;
	delete p_work1 ;
	delete p_work2 ;
	delete p_DImage;
	delete p_DImage1;
	delete di;
	delete di1;
	delete p_Gist;
	
	delete p_Vectx;
	delete p_Vecty;
	delete p_Vectz;
	delete p_VectCol;	
	delete p_PVect;
	
	return true;
}

