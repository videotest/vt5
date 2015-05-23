#include "stdafx.h"
#include "AreaSeg.h"
#include "math.h"
#include <stdio.h>

#define szSmooth "smooth"
#define szK9 "k9"
#define szMinObjSize "min_obj_size"
#define szBgSelectionMethod "bg_selection_method"
#define szBgExpandMethod "bg_expand_method"
#define szBgExpandLevel "bg_expand_level"

_ainfo_base::arg CAreaSegInfo::s_pargs[] = 
{       
	{szSmooth,  szArgumentTypeInt, "4", true, false }, //насколько сильно размывать изображение перед работой
	{szK9,  szArgumentTypeDouble, "3", true, false }, //чем больше, тем более резкие границы выделяем
	{szMinObjSize,  szArgumentTypeDouble, "1.0", true, false }, //минимальный размер областей (в процентах от полупериметра)
	{szBgSelectionMethod, szArgumentTypeInt, "3", true, false }, //метод выбора первой области фона: 1 - задана точка, 2 - максимальная площадь, 3 - максимальный линейный размер
	{szBgExpandMethod, szArgumentTypeInt, "2", true, false }, //метод выбора последующих областей фона: 1 - ближайшие по средней яркости к первой, 2 - ближайшие по разбросу яркости
	{szBgExpandLevel, szArgumentTypeDouble, "0.1", true, false }, //разброс параметров для фона (=1 - все-фон)

	{"Img",       szArgumentTypeImage, 0, true, true },
	{"Result",    szArgumentTypeBinaryImage, 0, false, true },
	//{"Areas",    szArgumentTypeImage, 0, false, true },
	{0, 0, 0, false, false },
};


/////////////////////////////////////////////////////////////////////////////
CAreaSeg::CAreaSeg()
{
	m_smooth=1;
	m_k9=4;
	p_ColorAvg=NULL;
	p_C2Avg=NULL;
	p_XAvg=NULL;
	p_YAvg=NULL;
	p_R2Avg=NULL;
	p_C2R2=NULL;
	p_AreaSize=NULL;
}

#define del(par) {if(par) {delete(par); par=0;}}

/////////////////////////////////////////////////////////////////////////////
CAreaSeg::~CAreaSeg()
{
	del(p_ColorAvg);
	del(p_C2Avg);
	del(p_XAvg);
	del(p_YAvg);
	del(p_R2Avg);
	del(p_C2R2);
	del(p_AreaSize);
}


/////////////////////////////////////////////////////////////////////////////
void CAreaSeg::FillSmallSegments1(int cx,int cy,DWORD MinObjSize)
{
	int i, j, x, y;
	DWORD *p_ObjNumber_sh;

	DWORD *p_ObjNumber1=new DWORD[(cx+2)*(cy+2)+1];
	ZeroMemory(p_ObjNumber1,((cx+2)*(cy+2)+1)*sizeof(DWORD));
	DWORD *p_ObjNumber1_sh;
	DWORD *p_Save;
	int cx2=cx+2;
	int cy2=cy+2;

	// p_ObjNumber[cx,cy]
	// p_ObjNumber1[cx+2,cy+2]
	// заполним внутренность p_ObjNumber1 точками из p_ObjNumber
	for (j=0; j<cy; j++)
	{
		p_ObjNumber_sh=p_ObjNumber+j*cx;
		p_ObjNumber1_sh=p_ObjNumber1+(j+1)*cx2+1;
		for (i=0;i<cx;i++)
		{
			*p_ObjNumber1_sh++=*p_ObjNumber_sh++;
		}
	}


	// заливка всех мелких сегментов цветом 4294967295
	for (j=1;j<cy2-1;j++)
	{
		p_ObjNumber1_sh=p_ObjNumber1+j*cx2+1;
		for (i=1;i<cx2-1;i++)
		{	
		    if (DWORD(p_ObjSize[*p_ObjNumber1_sh])<MinObjSize)		    
			{
				*p_ObjNumber1_sh=4294967295;
			}
			p_ObjNumber1_sh++;
		}
	}

	// заливка внутренних сегментов цветом окружающего сегмента


	DWORD k;
	DWORD MinObjSizeK=(DWORD)((double)MinObjSize*1.5);
	DWORD Count1,Count2;
	DWORD Color;
	DWORD FillColor;
	DWORD FillColor1;
	DWORD *p_Count1 = new DWORD[cx*cy/5];
	DWORD *p_Count2 = new DWORD[cx*cy/5];
	int *di = new int[8];
	di[0] = -cx2;	
	di[1] =  1;
	di[2] =  cx2;
	di[3] = -1;
	di[4] = -cx2+1;
	di[5] =  cx2+1;	
	di[6] =  cx2-1;	
	di[7] = -cx2-1;

	j=0;

	for (y=1;y<cy2-1;y++)
	{
		p_ObjNumber1_sh = p_ObjNumber1+y*cx2+1;
		for (x=1;x<cx2-1;x++)
		{
			if(*p_ObjNumber1_sh==4294967295)
			{
				FillColor=4294967295;
				if (*(p_ObjNumber1_sh+1)!=4294967295)
					FillColor=*(p_ObjNumber1_sh+1);
				else
				{
					if (*(p_ObjNumber1_sh-1)!=4294967295)
						FillColor=*(p_ObjNumber1_sh-1);
					else
						if (*(p_ObjNumber1_sh+cx2)!=4294967295)
							FillColor=*(p_ObjNumber1_sh+cx2);
						else
							FillColor=*(p_ObjNumber1_sh-cx2);
				} //now FillColor - непустой цвет из 5-точечного шаблона (если есть)
				if (FillColor!=4294967295) 
				{
					p_Save=p_ObjNumber1_sh;
					FillColor1=4294967295-1-FillColor;
					*p_ObjNumber1_sh=FillColor1;
					k=1;
					Count1=1;
					p_Count1[0]=p_ObjNumber1_sh-p_ObjNumber1;
					do
					{ 
						Count2=0;
						for (i=0;i<(int)Count1;i++)
							for (j=0;j<4;j++)
							{
								Color=*(p_ObjNumber1+p_Count1[i]+di[j]);
								if(Color==4294967295)
								{
									p_Count2[Count2] = p_Count1[i]+di[j];
									*(p_ObjNumber1+p_Count2[Count2])=FillColor1;
									k++;
									Count2++;
								}
								else
									if (Color<2147483648 && Color!=FillColor)
									{										  
										j=10;
										Count1=0;
										Count2=0;
									}
							}
							if (k>MinObjSizeK)
							{
								j=11;
								Count1=0;
								Count2=0;
							}
							Count1=Count2;
							CopyMemory(p_Count1,p_Count2,Count2*sizeof(DWORD));
					}while(Count2);	
					if (j>10)  // рекурсия
					{						  
						FillColor1=2147483648;
						*p_Save=FillColor1;
						//					      k=1;
						Count1=1;
						p_Count1[0]=p_Save-p_ObjNumber1;
						do
						{ 
							Count2=0;
							for (i=0;i<(int)Count1;i++)
								for (j=0;j<4;j++)								  
									if(*(p_ObjNumber1+p_Count1[i]+di[j])>FillColor1)
									{
										p_Count2[Count2] = p_Count1[i]+di[j];
										*(p_ObjNumber1+p_Count2[Count2])=FillColor1;
										//									      k++;
										Count2++;
									}
									Count1=Count2;
									CopyMemory(p_Count1,p_Count2,Count2*sizeof(DWORD));
						}while(Count2);	
					}
					
				}
			}
			p_ObjNumber1_sh++;
		}
	}

	delete p_Count1;
	delete p_Count2;
	delete di;

	for (j=0;j<cy;j++)
	{
		p_ObjNumber_sh=p_ObjNumber+j*cx;
		p_ObjNumber1_sh=p_ObjNumber1+(j+1)*cx2+1;
		for (i=0;i<cx;i++)
		{
			if (*p_ObjNumber1_sh>2147483648)
			{
				p_ObjSize[*p_ObjNumber_sh]=0;
                *p_ObjNumber_sh=4294967295-1-*p_ObjNumber1_sh;
			}
//            else if (*p_ObjNumber1_sh==2147483648)
//			{
//				p_ObjSize[*p_ObjNumber_sh]=0;
//				*p_ObjNumber_sh=4294967295;				
//			}			
			p_ObjNumber_sh++;
			p_ObjNumber1_sh++;
		}
	}

	delete p_ObjNumber1;

	return;
}

void CAreaSeg::FillSmallSegments2(int cx,int cy,DWORD MinObjSize)
{
	int FillColor=p_ObjNumber[0];
	for(int iy=0; iy<cy; iy++)
	{
		int i=iy*cx;
		for(int ix=0; ix<cx; ix++)
		{
			if(p_ObjSize[p_ObjNumber[i]]<int(MinObjSize))
			{
				if(p_ObjNumber[i]!=FillColor)
					ReSetObj(FillColor,p_ObjNumber[i],0);
			}
			FillColor=p_ObjNumber[i];
			i++;
		}
		FillColor=p_ObjNumber[iy*cx];
	}
	return;
}


void CAreaSeg::Smoothing( BYTE *p_arg, BYTE *p_res , int cx , int cy ,int colors, int Mask )
{
	if(Mask<=1) 
	{
		CopyMemory(p_res, p_arg, cx*cy);
		return;
	}

	int m=Mask, mm=m*m, m1=(m-1)/2, m2=m-m1;
	int *buf=new int[cx+m];

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
	delete buf;
}


bool CAreaSeg::ReSetObj(DWORD seg1,DWORD seg2,DWORD weight)
{
	DWORD wd1=0;
	DWORD wd2=0;
	DWORD k1=0;
	DWORD k2=0;


// reset objects pixels

    DWORD Posit=seg2;
	DWORD Posit1;

    do
	{   
		Posit1=Posit;
		p_ObjNumber[Posit1]=seg1;
		Posit=p_PixMap[Posit1];
	}while(Posit!=-1);

	// connect pixelmap
	p_PixMap[Posit1]=p_PixMap[seg1];
	p_PixMap[seg1]=seg2;

	// recalc inwide and size

    p_ObjSize[seg1]+=p_ObjSize[seg2];

	if (weight==0)
		weight=1;

	float InW=
		p_InWeight[seg1]*p_ObgCoutE[seg1]+
		p_InWeight[seg2]*p_ObgCoutE[seg2]+weight;

	p_ObgCoutE[seg1]+=p_ObgCoutE[seg2]+1;
	p_InWeight[seg1]=InW/p_ObgCoutE[seg1];
	// zero
	p_ObjSize[seg2]=0;
	p_ObgCoutE[seg2]=0;
	p_InWeight[seg2]=0;
	return true;
}

void CAreaSeg::CorrectBord(BYTE * pArg,int cx,int cy)
{
	int x,y,j;

	BYTE *p_w1=new BYTE[cx*cy+1];
	BYTE *p_w2=new BYTE[cx*cy+1];
	ZeroMemory(p_w1,cx*cy+1);	
	BYTE *p_w1y;
	BYTE *p_w2y;
	BYTE _hypot[64*64];
	for(int i=0; i<64; i++)
	{
		for(int j=0; j<64; j++)
		{
			_hypot[i*64+j]=int(sqrt(double((i*i+j*j))));
		}
	}

	Smoothing(pArg,p_w2,cx,cy,1,3);
	Smoothing(p_w2,p_w1,cx,cy,1,3);
	
	//p_w1 - размытый grayscale image
	
	ZeroMemory(p_w2,cx*cy+1);
	//p_w2 забит нулями

	int sum1=0;
	int sum2=0;

	for (y=1;y<cy-1;y++)
	{
		p_w1y=p_w1+y*cx+1;
		p_w2y=p_w2+y*cx+1;
		for(x=1;x<cx-1;x++)
		{
			sum1 = p_w1y[1] - p_w1y[-1];
			if(sum1<0) sum1=-sum1;
			if(sum1>63) sum1=63;
			sum2 = p_w1y[cx] - p_w1y[-cx];
			if(sum2<0) sum2=-sum2;
			if(sum2>63) sum2=63;

			*p_w2y=_hypot[sum1*64+sum2];
			p_w1y++;
			p_w2y++;
		}
	}
	//p_w2 ~ модуль градиента сглаженного изобр-я

	int *di = new int[8];
	int *diy1;
	di[0] = -cx;	
	di[1] =  1;
	di[2] =  cx;
	di[3] = -1;
	di[4] = -cx+1;
	di[5] =  cx+1;	
	di[6] =  cx-1;	
	di[7] = -cx-1;

	DWORD ObjNumber1;	
	BYTE Grad;
	BYTE flag=1;

//	for (j1=0;j1<0;j1++)
	{
		for (y=1;y<cy-1;y++)
		{
			p_w2y=p_w2+y*cx+1;
			DWORD * p_ObjNumber_sh=p_ObjNumber+y*cx+1;
			for(x=1;x<cx-1;x++)
			{
				diy1=di;
				ObjNumber1=*p_ObjNumber_sh;
				Grad=*p_w2y; //градиент
				for (j=0;j<4;j++)
				{
					if (ObjNumber1!=*(p_ObjNumber_sh+*diy1))
						if (Grad<*(p_w2y+*diy1))
								*(p_ObjNumber_sh+*diy1)=ObjNumber1;
						//если в соседней точке градиент больше - располземся туда

					diy1++;
				}
				p_w2y++;
				p_ObjNumber_sh++;
			}
		}
		for (y=cy-2;y>0;y--)
		{
			p_w2y=p_w2+y*cx+cx-2;
			DWORD * p_ObjNumber_sh=p_ObjNumber+y*cx+cx-2;
			for(x=cx-2;x>0;x--)
			{
				diy1=di;
				ObjNumber1=*p_ObjNumber_sh;
				Grad=*p_w2y; //градиент
				for (j=0;j<4;j++)
				{
					if (ObjNumber1!=*(p_ObjNumber_sh+*diy1))
						if (Grad<*(p_w2y+*diy1))
								*(p_ObjNumber_sh+*diy1)=ObjNumber1;
						//если в соседней точке градиент больше - располземся туда

					diy1++;
				}
				p_w2y--;
				p_ObjNumber_sh--;
			}
		}
	}

	delete p_w1;
	delete p_w2;
	delete di;
	
}

void CAreaSeg::EnumAreas(int cx, int cy)
//вычислить m_AreaCount,
//определить и заполнить  p_AreaBase
{
	//заполним заново p_ObjSize
	for (int i=0; i<cx*cy; i++) p_ObjSize[i]=0;

	//на всякий случай - уберем некорректные p_ObjNumber
	if (p_ObjNumber[0]==4294967295) p_ObjNumber[0]=0;
	for (i=0;i<cx*cy;i++)	
	{
		if (p_ObjNumber[i]==4294967295) p_ObjNumber[i]=p_ObjNumber[i-1];
		p_ObjSize[p_ObjNumber[i]]++;
	}

	//пересчитаем области
	int *p_AreaNumber2=new int[cx*cy];
	m_AreaCount=0;
	for (i=0; i<cx*cy; i++) 
	{
		if(p_ObjSize[i]!=0) 
		{
			p_AreaNumber2[i]=m_AreaCount;
			m_AreaCount++;
		}
	}

	p_AreaNumber = new int[cx*cy];
	p_AreaBase = new int[m_AreaCount];
	int *p_AreaBase_sh=p_AreaBase;
	for (i=0; i<cx*cy; i++)
	{
		p_AreaNumber[i] = p_AreaNumber2[p_ObjNumber[i]];
		if(p_ObjSize[i]!=0)
		{
			*(p_AreaBase_sh++)=i;
		}
	}
	delete(p_AreaNumber2);
}

void CAreaSeg::EvalAreaParams(BYTE *p_work, int cx, int cy)
//рассчитать параметры областей: площадь,
//линейный размер, средний цвет, средний градиент
{
	p_ColorAvg  = new double[m_AreaCount]; //переделать в целые?
	p_C2Avg  = new double[m_AreaCount]; //переделать в целые?
	p_XAvg  = new double[m_AreaCount]; //переделать в целые?
	p_YAvg  = new double[m_AreaCount]; //переделать в целые?
	p_R2Avg  = new double[m_AreaCount]; //переделать в целые?
	p_C2R2  = new double[m_AreaCount]; //переделать в целые?
	p_AreaSize  = new int[m_AreaCount];
	for (int i=0; i<m_AreaCount; i++) 
	{
		p_ColorAvg[i] = 0;
		p_C2Avg[i] = 0;
		p_XAvg[i] = 0;
		p_YAvg[i] = 0;
		p_R2Avg[i] = 0;
		p_AreaSize[i]=0;
	}

	i=0;
	for (int iy=0; iy<cy; iy++)
	{
		for (int ix=0; ix<cx; ix++)
		{
			int areaNumber=p_AreaNumber[i];
			p_AreaSize[areaNumber] ++;
			p_ColorAvg[areaNumber] += p_work[i];
			p_C2Avg[areaNumber] += int(p_work[i])*p_work[i];
			p_XAvg[areaNumber] += ix;
			p_YAvg[areaNumber] += iy;
			p_R2Avg[areaNumber] += ix*ix+iy*iy;
			i++;
		}
	}

	for (i=0; i<m_AreaCount; i++) 
	{
		p_ColorAvg[i] /= p_AreaSize[i];
		p_C2Avg[i] /= p_AreaSize[i];
		p_C2Avg[i] -= p_ColorAvg[i]*p_ColorAvg[i];
		p_XAvg[i] /= p_AreaSize[i];
		p_YAvg[i] /= p_AreaSize[i];
		p_R2Avg[i] /= p_AreaSize[i];
		p_R2Avg[i] -= p_XAvg[i]*p_XAvg[i] + p_YAvg[i]*p_YAvg[i];
		p_C2R2[i] = sqrt(p_C2Avg[i]/(p_R2Avg[i]+1));
	}
}

void CAreaSeg::FindObjects(BYTE *p_work, WORD *Palette, int cx,int cy)
//Найти объекты (после того, как все поделено на области)
//Методы:
//1. По площади.
//2. По линейному размеру.
//3. По среднему цвету области.
//4. ? По maxcolor-mincolor в области
{
	int *p_ObjSize_sh=p_ObjSize;
	int max_i;

	//выбрать первую область фона
	if(m_SelectionMethod==1)
	{
		int x0=GetValueInt(GetAppUnknown(), "\\AreaSeg", "BgBaseX", 1);
		int y0=GetValueInt(GetAppUnknown(), "\\AreaSeg", "BgBaseY", 1);
		max_i=p_AreaNumber[y0*cx+x0];
	}
	else if(m_SelectionMethod==2)
	{
		int maxSize=-1;
		for (int i=0; i<m_AreaCount; i++)	
		{
			if (p_AreaSize[i]>maxSize)
			{
				maxSize=p_AreaSize[i];
				max_i=i;
			}
		}
	}
	else //if(m_SelectionMethod==3)
	{
		double maxR2=-1;
		for (int i=0; i<m_AreaCount; i++)	
		{
			if (p_R2Avg[i]>maxR2)
			{
				maxR2=p_R2Avg[i];
				max_i=i;
			}
		}
	}

	//расширить фон в соответствии с параметрами
	double *p_SearchParam;
	if(m_ExpandMethod==2)
	{
		p_SearchParam=p_C2R2;
	}
	else
	{
		p_SearchParam=p_ColorAvg;
	}
	double par0=p_SearchParam[max_i];

	double paramMax=-1e40, paramMin=1e40;
	for (int i=0; i<m_AreaCount; i++) 
	{
		if(p_SearchParam[i]>paramMax) paramMax=p_SearchParam[i];
		if(p_SearchParam[i]<paramMin) paramMin=p_SearchParam[i];
	}
	double delta=m_ExpandPercent*
		max(fabs(paramMax-par0), fabs(paramMin-par0));

	for (i=0; i<m_AreaCount; i++) 
	{
		if(fabs(p_SearchParam[i]-par0) < delta)
		{
			Palette[p_AreaBase[i]]=0;
		}
		else
		{
			Palette[p_AreaBase[i]]=65535;
		}
	}
}

//todo:
//все выделения памяти - через _ptr_t
//защититься от некорректных параметров (e.g. smooth1<1)
//добавить параметр - удаление малых областей.
//переделать FillSmallObjects - а то у Семена вылезают отдельные кусочки :-(
//возможно - удалить p_PixMap, p_firstpix

//переделать с RGB на любой или на Grayscale

// поиск первой фоновой области:
// 1. Самая большая площадь.
// 2. Самый большой линейный размер
// 3. Указанные координаты.

// поиск остальных фоновых областей
// 1. По близости среднего цвета к первой
// 2. По близости среднего градиента к первой.


void FireEvent(const char *pszEvent, IUnknown *punkFrom, IUnknown *punkData, void *pdata, long cbSize )
{
	INotifyControllerPtr	sptr( GetAppUnknown() );

	if( sptr== 0 )
		return;

	_bstr_t	bstrEvent( pszEvent );
	sptr->FireEvent( bstrEvent, punkData, punkFrom, pdata, cbSize );
}

bool CAreaSeg::InvokeFilter()
{

	long dwTickCount=12345;

	//IUnknown *punkAction = &IFilterAction(*this);
	//::FireEvent( szEventAfterActionExecute, 0, punkAction, &dwTickCount, sizeof(dwTickCount) );

	DWORD T1,T2,T3,T4,T5,T6,T7,T8;
	T1=GetTickCount();
    
	IImage2Ptr pArg	= GetDataArgument();
	IBinaryImagePtr pResult	= GetDataResult();

	if( pArg ==0 || pResult ==0 )
		return false;

	SIZE size;
	pArg->GetSize((int *)(&size.cx), (int *)(&size.cy));
	POINT ptOffset;
	pArg->GetOffset(&ptOffset);

	pResult->CreateNew(size.cx, size.cy);
	pResult->SetOffset( ptOffset, TRUE);

	StartNotification( size.cy, 3 );

    int i,j,x,y;
	int cx = size.cx;
	int cy = size.cy;

	BYTE *p_work0 = new BYTE[cx*cy+1];
	BYTE *p_work0y;

	BYTE *p_work1 = new BYTE[cx*cy+1];
	BYTE *p_work9y=0;

	int colors;
	IColorConvertor2* pIClrConv=0;
	pArg->GetColorConvertor((IUnknown**)&pIClrConv);
	if( !pIClrConv ) return false;
	colors = ::GetImagePaneCount( pArg );
	IColorConvertorPtr	ptrC( pIClrConv );
	pIClrConv->Release();


	_ptr_t2<color*>	ptr_colors;
	color** ppcolors = ptr_colors.alloc( colors );

	for (j=0;j<cy;j++)
	{
		p_work0y=p_work0+j*cx;
		color *pArg0, *pArg1, *pArg2;
		pArg->GetRow(j,0,&pArg0);
		pArg->GetRow(j,1,&pArg1);
		pArg->GetRow(j,2,&pArg2);

		for( long c = 0; c  < colors; c++ )
			pArg->GetRow(j, c, &ppcolors[c]);

		ptrC->ConvertImageToDIB( ppcolors, p_work0y, cx, false );

		/*
		for (i=0;i<cx;i++)
		{
			*p_work0y++=((*pArg0++)+(*pArg1++)+(*pArg2++))/768;
		}
		*/		
	}
	//сформировали в p_work0 grayscale картинку.

	m_smooth=GetArgLong(szSmooth);
	m_k9=GetArgDouble(szK9);
	// Необходимые presets:
	//	1	1	4	четкие границы (r=1.4, k9=4)
	//	1	3	4	средние (r=3.16, k9=4)
	//	5	3	3	нечеткие границы (r=5.8, k9=3)

	if(m_smooth>min(cx,cy)) m_smooth=min(cx,cy);
	if(m_smooth<1) m_smooth=1;
	m_smooth1=m_smooth, m_smooth2=m_smooth;
	if (m_smooth%2==0) 
	{
		m_smooth1++;
		m_smooth2--;
	}

	T2=GetTickCount();

	// smoothing
	Smoothing(p_work0,p_work1,cx,cy,1, m_smooth1);
	Smoothing(p_work1,p_work0,cx,cy,1, m_smooth2);
	//теперь в p_work0 размытая картинка.

	T3=GetTickCount();


	// begin segmentation 

	DWORD *Pos = new DWORD[256];
	DWORD *Count = new DWORD[256];

	BYTE *p_EWeight = new BYTE[cx*cy*2+2]; //веса границ
	DWORD *p_Pos  = new DWORD[cx*cy*2+2];	//позиции ключевых точек границ
	p_ObjNumber  = new DWORD[cx*cy+1];
	p_PixMap  = new int[cx*cy+1];
	//p_FirstPix  = new int[cx*cy+1];	
	p_InWeight  = new float[cx*cy+1];
	p_ObjSize  = new int[cx*cy+1];
	p_ObgCoutE  = new int[cx*cy+1];

	DWORD CountPix=cx*cy;
	DWORD CountEdg=(cx-1)*(cy-1)*2;

	BYTE *p_EWeight_sh=p_EWeight;
	DWORD *p_Pos_sh=p_Pos;
	DWORD *p_ObjNumber_sh=p_ObjNumber;
	int *p_PixMap_sh=p_PixMap;
	//p_FirstPix_sh=p_FirstPix;	
	float *p_InWeight_sh=p_InWeight;
	int *p_ObjSize_sh=p_ObjSize;
	int *p_ObgCoutE_sh=p_ObgCoutE;

	for (i=0;i<256;i++)
	{
		Pos[i]=0;
		Count[i]=0;
	}

	for (y=0;y<cy-1;y++)
	{
		p_work0y=p_work0+y*cx;
		for (x=0;x<cx-1;x++)
		{
            Count[abs(*p_work0y-*(p_work0y+1))]++;			
			Count[abs(*p_work0y-*(p_work0y+cx))]++;
			p_work0y++;
		}        
	}
	//count - гистограмма abs(grad(color))

	for (i=1;i<256;i++)
	{
		Pos[i]=Pos[i-1]+Count[i-1];
	}
	//Pos - первообразная от этой гистограммы.
	//Pos[i] - первая граница с весом i

	//инициализация областей
	for (i=0;i<int(CountPix);i++)	
	{
		*p_ObjNumber_sh++=i; //каждая область ссылается на свой единственный пиксел
		//*p_FirstPix_sh++=i; //он же - первая точка
		*p_PixMap_sh++=-1;
		*p_InWeight_sh++=0;	//вес области = 0
		*p_ObjSize_sh++=1; //площадь - 1 пиксел
		*p_ObgCoutE_sh++=0;
	}

	BYTE index1,index2;
	DWORD Pos1,Pos2;

//  set position and whide

	for (y=0;y<cy-1;y++)
	{
		p_work0y=p_work0+y*cx;
		for (x=0;x<cx-1;x++)
		{
			index1=abs(*p_work0y-*(p_work0y+1)); 
			Pos1=Pos[index1];
			p_EWeight[Pos1]=index1;
			p_Pos[Pos1]=(p_work0y-p_work0)*2; // граница вправо от пикселя = pixelnum*2			
			Pos[index1]++;


			index2=abs(*p_work0y-*(p_work0y+cx)); 
			Pos2=Pos[index2];
			p_EWeight[Pos2]=index2;
			p_Pos[Pos2]=(p_work0y-p_work0)*2+1;	// граница вниз от пикселя = pixelnum*2+1
			Pos[index2]++;

			// занесли две границы в соответствующие позиции
			// списка границ (256 начальных позиций в нем рассчитаны
			// заранее, внутри подсписков распределение произвольное).
			// можно было проще: не сравнивать их между собой,
			// а занести по одной

			p_work0y++;
		}
	}

// set outside edge of objects
	p_ObjNumber_sh=p_ObjNumber;

	// connect segments

	DWORD seg1,seg2,pos;
	DWORD weight;
	DWORD KK=200;
	DWORD CountEdgG=(cx-1)*(cy-1)*2;

	DWORD qwe=0;
	
	T4=GetTickCount();

	for (i=0;i<int(CountEdgG);i++)
	{
        //  Поиск общей границы seg1 and seg2
		pos=p_Pos[i]/2; //текущая граница
		if (pos+pos==p_Pos[i]) //если граница вертикальная
		{
			seg1=p_ObjNumber[pos];
		    seg2=p_ObjNumber[pos+1];
		}
		else //если горизонтальная
		{
			seg1=p_ObjNumber[pos];
		    seg2=p_ObjNumber[pos+cx];
		}
		//seg1, seg2 - две околограничные точки

	    weight=p_EWeight[i]; 

		int flagg=0;
		if (seg1!=seg2)
		{
			if (
				weight <= p_InWeight[seg1]*m_k9+KK/p_ObjSize[seg1] &&
				weight <= p_InWeight[seg2]*m_k9+KK/p_ObjSize[seg2])
				//weight <= 4+200/p_ObjSize[seg1] &&
				//weight <= 4+200/p_ObjSize[seg2])
				//weight <= 4+100/)
			{ //если выполняется условие слияния областей
				if (p_ObjSize[seg2]<p_ObjSize[seg1])
					ReSetObj(seg1,seg2,weight); //присоединяем меньшую область к большей
				else
					ReSetObj(seg2,seg1,weight);
			}
		}
	}

	T5=GetTickCount();

	T6=GetTickCount();

	m_MinObjSize=GetArgDouble(szMinObjSize);

	if(m_MinObjSize>0)
	{
		double MinObjSize=m_MinObjSize*(cx+cy)/100;
		MinObjSize = MinObjSize*MinObjSize;
		if (MinObjSize<20) MinObjSize=20;
		if (MinObjSize>cx*cy) MinObjSize=cx*cy;
		FillSmallSegments2(cx,cy, DWORD(MinObjSize));
	}

	// Восстановление границ.
	CorrectBord(p_work1,cx,cy);
	// end восстановления границ.

	// end segmentation 
    WORD *Palette =new WORD[cx*cy];

	T7=GetTickCount();
	//AAM
	EnumAreas(cx,cy);
	EvalAreaParams(p_work0, cx, cy);

	//AAM: выясним, какие из областей относятся к объектам, а какие - к фону
	m_SelectionMethod=GetArgLong(szBgSelectionMethod); // 3 - по лин.размеру
	m_ExpandMethod=GetArgLong(szBgExpandMethod); // 2 - по цвету
	m_ExpandPercent=GetArgDouble(szBgExpandLevel); //0.1

	FindObjects(p_work0, Palette, cx, cy);
	T8=GetTickCount();

	//заполнить изображение в соответствии с палитрой
	for (j=0;j<cy;j++)
	{
		p_ObjNumber_sh=p_ObjNumber+j*cx;

		byte *pRes;
		pResult->GetRow(&pRes, j, false);
		int *p_AreaNumber_sh=p_AreaNumber+j*cx;
		for (i=0;i<cx;i++, pRes++, p_ObjNumber_sh++, p_AreaNumber_sh++)
		{
			if (*p_ObjNumber_sh!=4294967295)
			{
				//*pRes=Palette[*p_ObjNumber_sh]==0 ? 0 : 255;
				*pRes=Palette[*p_ObjNumber_sh]==0 ? 0 : 256 - *(p_AreaNumber_sh)%20;
			}
			else
			{
				*pRes=255;
			}
			//*pRes=p_work0[j*cx+i]<128?0:255; //debug;
		}
	}

	/*
	IImage2Ptr pAreas	= GetDataResult("Areas");
	_bstr_t	bstrCC( "GRAY" );
	pAreas->CreateImage(size.cx, size.cy,bstrCC);
	pAreas->SetOffset( ptOffset, TRUE);
	for (j=0;j<cy;j++)
	{
		int *p_AreaNumber_sh=p_AreaNumber+j*cx;

		color *pRes;
		pAreas->GetRow(j,0,&pRes);
		for (i=0; i<cx; i++)
		{
			*pRes++=*(p_AreaNumber_sh++)*65535/(m_AreaCount);
		}
	}
	*/


	del(p_ColorAvg);
	del(p_C2Avg);
	del(p_XAvg);
	del(p_YAvg);
	del(p_R2Avg);
	del(p_C2R2);
	del(p_AreaSize);

	delete Palette;
	delete p_EWeight;
	delete p_Pos;
	delete p_ObjNumber;
	delete p_PixMap;
	//delete p_FirstPix;	
	delete p_InWeight;
	delete p_ObjSize;	
	delete p_ObgCoutE;

	delete Pos;
	delete Count;

    delete p_work0;
	delete p_work1;

	delete p_AreaNumber;
	delete p_AreaBase;

	//MessageBox(0,"Hell.....",0,0);

	_bstr_t s("AreaSeg: ");
	char s1[200];
	sprintf(s1,"%d",T4-T3);
	s=s+" time:"+s1;
	sprintf(s1,"%d",T5-T4);
	s=s+" "+s1;
	sprintf(s1,"%d",T6-T5);
	s=s+" "+s1;
	sprintf(s1,"%d",T8-T7);
	s=s+" "+s1;
	
	/*
	HWND hWnd=CreateWindow(
		"STATIC", "Info:", WS_POPUPWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, 200, 100,
		0, 0, app::handle(), 0);
	SetWindowText(hWnd, s);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	*/

	return true;
}

