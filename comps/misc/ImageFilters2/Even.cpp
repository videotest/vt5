// EvenFilter.cpp: implementation of the CEvenFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Even.h"
#include "misc_templ.h"
#include "math.h"
#include "ImagePane.h"

#define szSqrSize "sqr_size"
#define szEvenMethod "even_method"
#define szBackgroundType "background_type"
#define szEvenBrightness "even_brightness"
#define szMaxContrast "max_contrast"
#define szAutoContrast "auto_contrast"

_ainfo_base::arg CEvenInfo::s_pargs[] = 
{       
	{szSqrSize,  szArgumentTypeInt, "32", true, false },
	{szEvenMethod,  szArgumentTypeInt, "4", true, false },
	{szBackgroundType,  szArgumentTypeInt, "0", true, false },
	{szEvenBrightness,  szArgumentTypeInt, "0", true, false },
	{szMaxContrast,  szArgumentTypeDouble, "1", true, false },
	{szAutoContrast,  szArgumentTypeInt, "0", true, false },
	{"Img",       szArgumentTypeImage, 0, true, true },
	{"Result",    szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false },
};

#define min4(a,b,c,d) (__min(__min(__min((a),(b)),(c)),(d)))
#define max4(a,b,c,d) (__max(__max(__max((a),(b)),(c)),(d)))
#define med4(a,b,c,d) (((a)+(b)+(c)+(d))/4)

//сколько бит отгрызать у цвета для приведения к гистограмме
#define ColorShift 9

//цвет --> полоска гистограммы (округление вниз)
#define MaxColor color(-1)
#define ColorToBand(c) ((c)>>ColorShift)
static int MaxBand=ColorToBand(MaxColor);

//полоска гистограммы -->цвет:

//низ диапазона, соответствующего полоске гистограммы
#define BandToColor0(c) ((c)<<ColorShift)
//середина диапазона
#define BandToColorM(c) (((c)<<ColorShift) + (1<<(ColorShift-1)))
//верх диапазона
#define BandToColor1(c) (((c)<<ColorShift) + ((1<<ColorShift)-1))


//Выравнивание одной цветовой плоскости.
//SrcRows - исходное изображение, DstRows - результат
// (могут совпадать);   cx, cy - размеры.
// все остальное определяется параметрами фильтра
void CEvenFilter::EvenPlane(color **SrcRows, color **DstRows, int cx, int cy)
{
	int sqr_size=m_SqrSize; //первоначальная сторона квадрата
	if(sqr_size<4) sqr_size=4;
	int nx=cx*2/sqr_size;	//вначале определяем количество квадратов размера
	if(nx<1) nx=1;						//SqrSize/2
	int ny=cy*2/sqr_size;
	if(ny<1) ny=1;
	
	// расстояние между координатами углов квадратов меньше sqr_size

	// координаты углов квадратов
	_ptr_t2<int> smartSqr_x(nx+1);
	int *sqr_x=smartSqr_x.ptr;
	{for(int ix=0; ix<=nx; ix++)
		sqr_x[ix]=cx*ix/nx;}

	// координаты углов квадратов
	_ptr_t2<int> smartSqr_y(ny+1);
	int *sqr_y=smartSqr_y.ptr;
	{for(int iy=0; iy<=ny; iy++)
		sqr_y[iy]=cy*iy/ny;}

	// гистограмма цветов внутри квадрата
	_ptr_t2<int> smartHist(MaxBand+1);
	int* hist=smartHist.ptr;

	
	//количество квадратов - с запасом (на 1 квадрат за пределы)
	int wx=nx+2, wy=ny+2;

	// min по квадрату
	_ptr_t2<int> smartC0(wx*wy);
	int* C0=smartC0.ptr;

	// max по квадрату
	_ptr_t2<int> smartC1(wx*wy);
	int* C1=smartC1.ptr;

	// медиана или что еще по квадрату
	_ptr_t2<int> smartCM(wx*wy);
	int* CM=smartCM.ptr;

	// коэф-т контраста, fixed point 24.8
	_ptr_t2<int> smartContr(wx*wy);
	int* contr=smartContr.ptr;

	// коэф-т для сдвига (его будем интерполировать)
	_ptr_t2<int> smartB_Ground(wx*wy);
	int* B_Ground=smartB_Ground.ptr;

	// второй коэф-т для сдвига (его будем интерполировать)
	_ptr_t2<int> smartC_Dark(wx*wy);
	int* C_Dark=smartC_Dark.ptr;

	// третий коэф-т для сдвига (его будем интерполировать)
	_ptr_t2<int> smartC_Light(wx*wy);
	int* C_Light=smartC_Light.ptr;

//------------------------ code start ------------------------------------

	//определение min/med/max по квадратам
	sqr_x[0]++; sqr_x[nx]--; //Изображение усекается слева и справа, для осреднения 3x3 - исключим край
	sqr_y[0]++; sqr_y[ny]--; //Изображение усекается сверху и снизу, для осреднения 3x3 - исключим край

	// Перемещение по квадратам для заполнения массивов C0, CM и C1.
	{for(int iy=0; iy<ny; iy++)
	{
		for(int ix=0; ix<nx; ix++)
		{
			//построение гистограммы цветов для определения медианы
			smartHist.zero();
			int x0=sqr_x[ix], x1=sqr_x[ix+1];
			int y0=sqr_y[iy], y1=sqr_y[iy+1];

			/*
			for(int y=y0; y<y1; y+=2) 
			{
				color* Src0=SrcRows[y]+x0;
				color* Src1=SrcRows[y+1]+x0;
				for(int x=x0; x<x1; x+=2) 
				{
					int c=*Src0++;	c+=*Src0++;
					c+=*Src1++;	c+=*Src1++;
					c=ColorToBand(c/4);
					hist[c]++;
				}
			}
			*/

			// Построение гистограммы для текущего квадрата

			for(int y=y0; y<y1; y++) 
			{
				color* Src1=SrcRows[y-1]+x0; // установление указателя на x0 на строке (y-1)
				color* Src2=SrcRows[y]+x0; // установление указателя на x0 на строке y
				color* Src3=SrcRows[y+1]+x0; // установление указателя на x0 на строке (y+1)
				for(int x=x0; x<x1; x++,Src1++,Src2++,Src3++) 
				{
					int c=(
						Src1[-1]+Src1[0]*2+Src1[1]+
						Src2[-1]*2+Src2[0]*4+Src2[1]*2+
						Src3[-1]+Src3[0]*2+Src3[1]
						)/16; // усредненное по 3 соседним пикселям значение яркости
					c=ColorToBand(c); // нормирование уровня яркости по 128-битной шкале
					hist[c]++; // увеличение на 1 шкалы, соответствующей этому уровню яркости
				}
			}

			int sum=(sqr_x[ix+1]-sqr_x[ix])*(sqr_y[iy+1]-sqr_y[iy]); // количество пикселей в "квадрате"
			int th0=sum/64, // округленная (1/64) часть пикселей
				th1=sum-sum/64, // оставшаяся часть пикселей
				thm=sum/2; //половина пикселей

			int i=0, s=0;

			while(s<th0) s+=hist[i++]; // (1/64) часть пикселей имеет яркость меньшую или равную i по шкале
			C0[(iy+1)*wx+ix+1]=BandToColor0(i); // возвращение реального уровня яркости для i

			while(s<thm) s+=hist[i++]; // половина пикселей имеет яркость меньшую или равную i по шкале
			CM[(iy+1)*wx+ix+1]=BandToColorM(i); // возвращение реального уровня яркости для i
                                                 
			while(s<th1) s+=hist[i++]; // (1/64) часть пикселей имеет яркость большую или равную i по шкале
			C1[(iy+1)*wx+ix+1]=BandToColor1(i); // возвращение реального уровня яркости для i
		}
		Notify( sqr_y[iy] );
	}}
	sqr_x[0]--; sqr_x[nx]++; // к крайним квадратам возвращаются пиксели
	sqr_y[0]--; sqr_y[ny]++;
	NextNotificationStage( );

	// заполнение квадратов сверху и снизу от image значениями массивов C0, C1 и CM
	{for(int ix=1; ix<=nx; ix++)
	{
		C0[0*wx+ix]=C0[1*wx+ix];
		C0[(ny+1)*wx+ix]=C0[ny*wx+ix];
		C1[0*wx+ix]=C1[1*wx+ix];
		C1[(ny+1)*wx+ix]=C1[ny*wx+ix];
		CM[0*wx+ix]=CM[1*wx+ix];
		CM[(ny+1)*wx+ix]=CM[ny*wx+ix];
	}}

	// заполнение квадратов слева и справа от image значениями массивов C0, C1 и CM
	{for(int iy=0; iy<=ny+1; iy++)
	{
		C0[iy*wx+0]=C0[iy*wx+1];
		C0[iy*wx+nx+1]=C0[iy*wx+nx];
		C1[iy*wx+0]=C1[iy*wx+1];
		C1[iy*wx+nx+1]=C1[iy*wx+nx];
		CM[iy*wx+0]=CM[iy*wx+1];
		CM[iy*wx+nx+1]=CM[iy*wx+nx];
	}}
	
	// перейти от квадратов размера SqrSize/2 к пересекающимся
	// квадратам размера SqrSize
	{for(int iy=0; iy<=ny; iy++)
	{
		int n=iy*wx;
		for(int ix=0; ix<=nx; ix++,n++)
		{
			C0[n]=min4(
				C0[(iy+1)*wx+ix+1], C0[(iy+1)*wx+ix],
				C0[iy*wx+ix+1], C0[iy*wx+ix]);
			C1[n]=max4(
				C1[(iy+1)*wx+ix+1], C1[(iy+1)*wx+ix],
				C1[iy*wx+ix+1], C1[iy*wx+ix]);
			CM[n]=med4(
				CM[(iy+1)*wx+ix+1], CM[(iy+1)*wx+ix],
				CM[iy*wx+ix+1], CM[iy*wx+ix]);
		}
	}}

	// выбрать фон
	int *C;
	if(m_EvenMethod==2)
	{
		//для метода 2 фон = (max+min)/2 - медианное значение
		C=CM;
	}
	else
	{
		switch(m_BackgroundType)
		{
		case 1:
			C=C0; break; // светлый фон
		case 2:
			C=C1; break; // темный фон
		case 3:
			C=CM; break; // медиана
		case 0:
		default:
			{
				//найти наиболее равномерную матрицу
				double C0sum=0,C1sum=0,CMsum=0;
				for(int iy=0; iy<=ny; iy++)
				{
					int n=iy*wx;
					for(int ix=0; ix<nx; ix++,n++)
					{
						C0sum += abs(C0[n+1]-C0[n]);
						C1sum += abs(C1[n+1]-C1[n]);
						CMsum += abs(CM[n+1]-CM[n]);
					}
				}
				for(int iy=0; iy<ny; iy++)
				{
					int n=iy*wx;
					for(int ix=0; ix<=nx; ix++,n++)
					{
						C0sum += abs(C0[n+wx]-C0[n]);
						C1sum += abs(C1[n+wx]-C1[n]);
						CMsum += abs(CM[n+wx]-CM[n]);
					}
				}
				C=CM; double Csum=CMsum;					
				if(C0sum<Csum && (m_EvenMethod!=4)) {C=C0; Csum=C0sum; } // при методе 4 фон не может быть темным
				if(C1sum<Csum) {C=C1; Csum=C1sum; }
			}
			break;
		}
	}

	int cAvg, cMaxAvg, cMinAvg;
	int CMin, CMax;
	if(m_EvenMethod==2)
	{
		// при методе 2 средний уровень выводим к 128
		for(int iy=0; iy<=ny; iy++)
		{
			for(int ix=0; ix<=nx; ix++)
			{
				CM[iy*wx+ix]=(C0[iy*wx+ix]+C1[iy*wx+ix])/2;
			}
		}
		cAvg=128*256;
	}
	else
	{
		CMin=MaxColor; CMax=0;
		double cSum=0, cMaxSum=0, cMinSum=0;
		for(int iy=1; iy<=ny; iy++)
		{
			for(int ix=1; ix<=nx; ix++)
			{
				cSum+=C[iy*wx+ix];
				cMinSum+=C0[iy*wx+ix];
				cMaxSum+=C1[iy*wx+ix];
				if(C0[iy*wx+ix]<CMin) CMin=C0[iy*wx+ix];
				if(C1[iy*wx+ix]>CMax) CMax=C1[iy*wx+ix];
			}
		}
		cAvg=int(cSum/(nx*ny));
		cMinAvg=int(cMinSum/(nx*ny));
		cMaxAvg=int(cMaxSum/(nx*ny));
		
		// max contrast on whole image
		//if(C==C0) cAvg=0;
		//if(C==C1) cAvg=MaxColor;
	}

	{for(int iy=0; iy<=ny+1; iy++)
	{
		int n=iy*wx;
		for(int ix=0; ix<=nx+1; ix++,n++)
		{
			switch(m_EvenMethod)
			{
			case 1:
				C_Dark[n]=C_Light[n]=256;
				break;
			case 2:
				double c;
				c=double(MaxColor+1)/(C1[n]-C0[n]+1);
				if(c>1) c=(m_MaxContrast*c-1)/(m_MaxContrast+c-2); //плавное ограничение контраста
				C_Dark[n]=C_Light[n]=int(c*256);
				break;
			case 3:
				c=double(cAvg-CMin)/(C[n]-C0[n]+1);
				if(c>1) c=(m_MaxContrast*c-1)/(m_MaxContrast+c-2); //плавное ограничение контраста
				C_Dark[n]=int(c*256);

				c=double(CMax-cAvg)/(C1[n]-C[n]+1);
				if(c>1) c=(m_MaxContrast*c-1)/(m_MaxContrast+c-2); //плавное ограничение контраста
				C_Light[n]=int(c*256);

				if(C==C0) C_Dark[n]=C_Light[n];
				if(C==C1) C_Light[n]=C_Dark[n];
				break;
			case 4:
				c=double(cAvg)/(C[n]+1);
				if(c>1) c=(m_MaxContrast*c-1)/(m_MaxContrast+c-2); //плавное ограничение контраста
				C_Dark[n]=C_Light[n]=256*cAvg/C[n];
				break;
			}
		}
	}}

	{for(int iy=0; iy<=ny; iy++)
	{
		for(int ix=0; ix<=nx; ix++)
		{
			B_Ground[iy*wx+ix]=C[iy*wx+ix];
		}
	}}


	// интерполяция цвета фона и контраста по всему изображению,
	// (контрастирование, если m_EvenMethod = 2 или 3)
	{for(int iy=0; iy<ny; iy++)
	{
		for(int ix=0; ix<nx; ix++)
		{
			int x0=sqr_x[ix], x1=sqr_x[ix+1];
			int y0=sqr_y[iy], y1=sqr_y[iy+1];

			int c0=C0[iy*wx+ix];
			int c1=C1[iy*wx+ix];
			int cm=CM[iy*wx+ix];
			double BackGround0=B_Ground[iy*wx+ix];
			double BackGround1=B_Ground[iy*wx+ix+1];
			double dBackGround0_dy=double(B_Ground[(iy+1)*wx+ix]-BackGround0)/(sqr_y[iy+1]-sqr_y[iy]);
			double dBackGround1_dy=double(B_Ground[(iy+1)*wx+ix+1]-BackGround1)/(sqr_y[iy+1]-sqr_y[iy]);
			double ContrDark0=C_Dark[iy*wx+ix];
			double ContrDark1=C_Dark[iy*wx+ix+1];
			double dContrDark0_dy=double(C_Dark[(iy+1)*wx+ix]-ContrDark0)/(sqr_y[iy+1]-sqr_y[iy]);
			double dContrDark1_dy=double(C_Dark[(iy+1)*wx+ix+1]-ContrDark1)/(sqr_y[iy+1]-sqr_y[iy]);
			double ContrLight0=C_Light[iy*wx+ix];
			double ContrLight1=C_Light[iy*wx+ix+1];
			double dContrLight0_dy=double(C_Light[(iy+1)*wx+ix]-ContrLight0)/(sqr_y[iy+1]-sqr_y[iy]);
			double dContrLight1_dy=double(C_Light[(iy+1)*wx+ix+1]-ContrLight1)/(sqr_y[iy+1]-sqr_y[iy]);
			for(int y=y0; y<y1; y++) 
			{
				int BackGround=int(BackGround0*256);
				int dBackGround_dx=int((BackGround1-BackGround0)/(sqr_x[ix+1]-sqr_x[ix])*256);
				BackGround0+=dBackGround0_dy;
				BackGround1+=dBackGround1_dy;
				int ContrDark=int(ContrDark0*256);
				int dContrDark_dx=int((ContrDark1-ContrDark0)/(sqr_x[ix+1]-sqr_x[ix])*256);
				ContrDark0+=dContrDark0_dy;
				ContrDark1+=dContrDark1_dy;
				int ContrLight=int(ContrLight0*256);
				int dContrLight_dx=int((ContrLight1-ContrLight0)/(sqr_x[ix+1]-sqr_x[ix])*256);
				ContrLight0+=dContrLight0_dy;
				ContrLight1+=dContrLight1_dy;

				color *Dst=DstRows[y]+x0;
				color *Src=SrcRows[y]+x0;
				for(int x=x0; x<x1; x++)
				{
					int temp=*(Src++)-(BackGround/256);
					temp = temp<0 ? temp*(ContrDark/256) : temp*(ContrLight/256);
					temp = temp/256+cAvg;
					if (temp<0) temp=0;
					if (temp>MaxColor) temp=MaxColor;
					*(Dst++)=temp;
					BackGround+=dBackGround_dx;
					ContrDark+=dContrDark_dx;
					ContrLight+=dContrLight_dx;
				}
			}
		}
		Notify( sqr_y[iy] );
	}}
	NextNotificationStage( );
}

bool CEvenFilter::InvokeFilter()
{
	m_SqrSize=GetArgLong(szSqrSize);
	m_EvenMethod=GetArgLong(szEvenMethod);
	m_BackgroundType=GetArgLong(szBackgroundType);
	m_EvenBrightness=GetArgLong(szEvenBrightness);
	m_MaxContrast=GetArgDouble(szMaxContrast);

	IImage3Ptr	pArg( GetDataArgument() );
	IImage3Ptr	pResult( GetDataResult() );
    
	if( pArg == NULL || pResult == NULL )
		return false;

	// [vanek] 12.12.2003: works with color convertors panes
	CImagePaneCache paneCache( pArg );

//	CopyPropertyBag(pArg, pResult, true);

	{	
		ICompatibleObjectPtr ptrCO( pResult );
		
		if( ptrCO == NULL )
			return false;

		if( S_OK != ptrCO->CreateCompatibleObject( pArg, NULL, 0 ) )
		{			
			return false;
		}		
	}

	SIZE size;
	pArg->GetSize((int*)&size.cx,(int*)&size.cy); 
	POINT ptOffset;
	pArg->GetOffset(&ptOffset);

	int Panes;
	BSTR bstrName;
	IColorConvertor2* pIClrConv;
	pArg->GetColorConvertor((IUnknown**)&pIClrConv);
	//Panes = ::GetImagePaneCount( pArg );
	Panes = paneCache.GetPanesCount( );
	pIClrConv->GetCnvName(&bstrName);
	pResult->CreateImage(size.cx, size.cy,bstrName, ::GetImagePaneCount( pArg ) ); 
	pResult->SetOffset( ptOffset ,TRUE);
	pResult->InitRowMasks();
	//_bstr_t bstrImagePane = _bstr_t("ImagePane\\")+bstrName;
	SysFreeString(bstrName);

	//[AY] it is not correct
//	Panes = 3;

	//строки исходного изображения
	_ptr_t2<color*> smartSrcRows(size.cy*Panes);
	color** SrcRows=smartSrcRows.ptr;

	//строки результата
	_ptr_t2<color*> smartDstRows(size.cy*Panes);
	color** DstRows=smartDstRows.ptr;
	
	for( int nPane=0;nPane<Panes;nPane++ )
	{
		for (int y=0; y<size.cy; y++)
		{
			pResult->GetRow(y,nPane,&DstRows[nPane*size.cy+y]);
			pArg->GetRow(y,nPane,&SrcRows[nPane*size.cy+y]);
		}
	}

	if(m_EvenBrightness && Panes==3) //надо бы корректно разбирать разные цветовые модели...
	{
		StartNotification( size.cy, 3+Panes );
		//изображение - яркость (перевод в grayscale)
		_ptr_t2<color> smartGrayImage(size.cy*size.cx);
		color* GrayImage=smartGrayImage.ptr;
		//строки изображения-яркости
		_ptr_t2<color*> smartGrayRows(size.cy);
		color** GrayRows=smartGrayRows.ptr;

		//выровненная яркость
		_ptr_t2<color> smartEvenGrayImage(size.cy*size.cx);
		color* EvenGrayImage=smartEvenGrayImage.ptr;
		//строки выровненного изображения-яркости
		_ptr_t2<color*> smartEvenGrayRows(size.cy);
		color** EvenGrayRows=smartEvenGrayRows.ptr;

		{for (int y=0; y<size.cy; y++)
		{
			GrayRows[y]=GrayImage+y*size.cx;
			EvenGrayRows[y]=EvenGrayImage+y*size.cx;
		}}

		//построим grayscale картинку
		{for (int y=0; y<size.cy; y++)
		{
			color *Gray=GrayRows[y];
			color *Src0=SrcRows[0*size.cy+y]; //R
			color *Src1=SrcRows[1*size.cy+y]; //G
			color *Src2=SrcRows[2*size.cy+y]; //B
			for(int x=0; x<size.cx; x++)
			{
				int c0=*(Src0++);
				int c1=*(Src1++);
				int c2=*(Src2++);
								
				if(c0>c1) {int t=c0; c0=c1; c1=t;}
				if(c1>c2) {int t=c1; c1=c2; c2=t;}
				if(c0>c1) {int t=c0; c0=c1; c1=t;}
				//упорядочили компоненты: c0<=c1<=c2
				*(Gray++)=(c0+c2)/2;
				//*(Gray++)=(c0+c1+c2)/3;
			}
			Notify( y );
		}}
		NextNotificationStage( );

		EvenPlane(
			GrayRows, EvenGrayRows,
			size.cx, size.cy);

		//таблица синусов
		_ptr_t2<int> smartSIN(256);
		int* SIN=smartSIN;
		{for(int i=0; i<256; i++) 
			SIN[i]=int(sin(3.14159*(i+.5)/256)*4096)+1;
		}
		
		for( int nPane=0;nPane<Panes;nPane++ )
		{
			// [vanek] 12.12.2003: CImagePaneCache
			//BSTR bstrPaneName;
			//pIClrConv->GetPaneName(nPane, &bstrPaneName);
			if( IsPaneEnable(nPane) && paneCache.IsPaneEnable(nPane) /*&& GetValueInt(GetAppUnknown(), bstrImagePane, _bstr_t(bstrPaneName), 1)*/)
			{
				for (int y=0; y<size.cy; y++)
				{
					color* Gray=GrayRows[y];
					color* EvenGray=EvenGrayRows[y];
					color* Src=SrcRows[nPane*size.cy+y];
					color* Dst=DstRows[nPane*size.cy+y];
					for (int x=0; x<size.cx; x++,Src++,Dst++,Gray++,EvenGray++)
					{
						int c=int(*Src)-*Gray;
						c=c*SIN[*EvenGray/256]/SIN[*Gray/256];
						c+=*EvenGray;
						if (c<0) c=0;
						if (c>MaxColor) c=MaxColor;
						*Dst=c;
					}
					Notify( y );
				}
				NextNotificationStage( );
			}
		}
	}
	else
	{
		StartNotification( size.cy, Panes*2 );
		for( int nPane=0;nPane<Panes;nPane++ )
		{
			// [vanek] 12.12.2003: CImagePaneCache 
			//BSTR bstrPaneName;
			//pIClrConv->GetPaneName(nPane, &bstrPaneName);
			if( IsPaneEnable(nPane) && paneCache.IsPaneEnable(nPane) /*&& GetValueInt(GetAppUnknown(), bstrImagePane, _bstr_t(bstrPaneName), 1)*/ )
			{
				EvenPlane(
					SrcRows+nPane*size.cy, DstRows+nPane*size.cy,
					size.cx, size.cy);
			}
			else
			{
				for (int y = 0; y < size.cy; y++)
				{
					color *pSrc,*pDst;
					pArg->GetRow(y, nPane, &pSrc);
					pResult->GetRow(y, nPane, &pDst);
					memcpy(pDst, pSrc, size.cx*sizeof(color));
				}
				NextNotificationStage( );
				NextNotificationStage( );
			}
		}
	}


	//Autocontrast


	FinishNotification();
	pIClrConv->Release();
	return true;
}

void CEvenFilter::AutoContrast(color **Rows, int cx, int cy)
{
	// гистограмма цветов
	_ptr_t2<int> smartHist(MaxBand+1);
	int* hist=smartHist.ptr;
	smartHist.zero();
	{for(int iy=1; iy<cy-1; iy++)
	{
		color* Src1=Rows[iy-1]+1;
		color* Src2=Rows[iy]+1;
		color* Src3=Rows[iy+1]+1;
		for(int ix=1; ix<cx-1; ix++,Src1++,Src2++,Src3++)
		{
			//построение гистограммы цветов
			int c=(
				Src1[-1]+Src1[0]*2+Src1[1]+
				Src2[-1]*2+Src2[0]*4+Src2[1]*2+
				Src3[-1]+Src3[0]*2+Src3[1]
				)/16;
			c=ColorToBand(c);
			hist[c]++;
		}
	}}
	int sum=(cx-2)*(cy-2);
	int th0=sum/64, th1=sum-sum/64, thm=sum/2;

	int i=0, s=0;
	while(s<th0) s+=hist[i++];
	int c0=BandToColor0(i);
	while(s<thm) s+=hist[i++];
	int cm=BandToColorM(i);
	while(s<th1) s+=hist[i++];
	int c1=BandToColor1(i);
	double fcontr=(MaxColor+1.0)/(c1-c0+1);
	int contr=int(min(fcontr,64.0)*256);

	{for(int iy=0; iy<cy; iy++)
	{
		color* Src=Rows[iy];
		for(int ix=0; ix<cx; ix++,Src++)
		{
			//построение гистограммы цветов
			*Src=(*Src-c0)*contr/256+c0;
		}
	}}
}
