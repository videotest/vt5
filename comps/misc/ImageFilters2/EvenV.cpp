// EvenV.cpp: implementation of the CEvenVFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EvenV.h"
#include "misc_templ.h"
#include "math.h"
#include "ImagePane.h"
#include "alloc.h"

#define szSqrNumber "sqr_number"
#define szEvenMethod "even_method"
#define szBackgroundType "background_type"
#define szEvenBrightness "even_brightness"
#define szAutoContrast "auto_contrast"
#define szContrast "contrast"

_ainfo_base::arg CEvenVInfo::s_pargs[] = 
{       
	{szSqrNumber,  szArgumentTypeInt, "10", true, false },
	{szEvenMethod,  szArgumentTypeInt, "0", true, false },
	{szBackgroundType,  szArgumentTypeInt, "0", true, false },
	{szEvenBrightness,  szArgumentTypeInt, "0", true, false },
	{szContrast,  szArgumentTypeInt, "0", true, false },
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
int y1edge, x1edge, y2edge, x2edge;
unsigned int ContrCoeffInt;
int tempPart, klineInt, blineInt, coeff1edgeInt;

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
void CEvenVFilter::EvenPlane(color **SrcRows, color **DstRows, int cx, int cy)
{
	//сторона квадрата
	int sqr_size=max(cx,cy)/m_SqrNumber;
	if(sqr_size<4) sqr_size=4;
	int nx=cx*2/sqr_size;	//вначале определяем количество квадратов размера
	if(nx<1) nx=1;						//SqrSize/2
	int ny=cy*2/sqr_size;
	if(ny<1) ny=1;

	// счетчики
	int ix, iy, x0, x1, y0, y1, n;

	//количество квадратов - с запасом (на 1 квадрат за пределы)
	int wx=nx+2, wy=ny+2;

	int numw = wx*wy;
	int numn = nx*ny;
	
	// расстояние между координатами углов квадратов меньше sqr_size

	// координаты углов квадратов
	_ptr_t2<int> smartSqr_x(nx+1);
	int *sqr_x=smartSqr_x.ptr;
	for(ix=0; ix<=nx; ix++)
		sqr_x[ix]=cx*ix/nx;

	// координаты углов квадратов
	_ptr_t2<int> smartSqr_y(ny+1);
	int *sqr_y=smartSqr_y.ptr;
	for(iy=0; iy<=ny; iy++)
		sqr_y[iy]=cy*iy/ny;

	// гистограмма цветов внутри квадрата
	_ptr_t2<int> smartHist(MaxBand+1);
	int* hist=smartHist.ptr;

	// min по квадрату
	_ptr_t2<int> smartC0(numw);
	int* C0=smartC0.ptr;

	// max по квадрату
	_ptr_t2<int> smartC1(numw);
	int* C1=smartC1.ptr;

	// медиана или что еще по квадрату
	_ptr_t2<int> smartCM(numw);
	int* CM=smartCM.ptr;

	// коэф-т контраста, fixed point 24.8
	_ptr_t2<int> smartContr(numw);
	int* contr=smartContr.ptr;

	// коэф-т для сдвига (его будем интерполировать)
	_ptr_t2<int> smartB_Ground(numw);
	int* B_Ground=smartB_Ground.ptr;

	// второй коэф-т для сдвига (его будем интерполировать)
	_ptr_t2<int> smartC_coeff(numw);
	int* C_coeff=smartC_coeff.ptr;


//------------------------ code start ------------------------------------

	//определение min/med/max по квадратам
	sqr_x[0]++; sqr_x[nx]--; //Изображение усекается слева и справа, для осреднения 3x3 - исключим край
	sqr_y[0]++; sqr_y[ny]--; //Изображение усекается сверху и снизу, для осреднения 3x3 - исключим край

	// Перемещение по квадратам для заполнения массивов C0, CM и C1.
	for(iy=0; iy<ny; iy++)
	{
		for(ix=0; ix<nx; ix++)
		{
			//построение гистограммы цветов для определения медианы
			smartHist.zero();
			x0=sqr_x[ix]; x1=sqr_x[ix+1];
			y0=sqr_y[iy]; y1=sqr_y[iy+1];

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

			int order = (iy+1)*wx+ix+1;
		
			while(s<th0) s+=hist[i++]; // (1/64) часть пикселей имеет яркость меньшую или равную i по шкале
			C0[order]=max(0,min(BandToColor0(i),MaxColor)); // возвращение реального уровня яркости для i

			while(s<thm) s+=hist[i++]; // половина пикселей имеет яркость меньшую или равную i по шкале
			CM[order]=max(0,min(BandToColorM(i),MaxColor)); // возвращение реального уровня яркости для i
                                                 
			while(s<th1) s+=hist[i++]; // (1/64) часть пикселей имеет яркость большую или равную i по шкале
			C1[order]=max(0,min(BandToColor1(i),MaxColor)); // возвращение реального уровня яркости для i
		}
		Notify( sqr_y[iy] );
	}

	sqr_x[0]--; sqr_x[nx]++; // к крайним квадратам возвращаются пиксели
	sqr_y[0]--; sqr_y[ny]++;
	NextNotificationStage( );

	// заполнение квадратов сверху и снизу от image значениями массивов C0, C1 и CM
	for(ix=1; ix<=nx; ix++)
	{
		C0[ix]=C0[wx+ix];
		C0[(ny+1)*wx+ix]=C0[ny*wx+ix];
		C1[ix]=C1[wx+ix];
		C1[(ny+1)*wx+ix]=C1[ny*wx+ix];
		CM[ix]=CM[wx+ix];
		CM[(ny+1)*wx+ix]=CM[ny*wx+ix];
	}

	// заполнение квадратов слева и справа от image значениями массивов C0, C1 и CM
	for(iy=0; iy<=ny+1; iy++)
	{
		C0[iy*wx]=C0[iy*wx+1];
		C0[iy*wx+nx+1]=C0[iy*wx+nx];
		C1[iy*wx]=C1[iy*wx+1];
		C1[iy*wx+nx+1]=C1[iy*wx+nx];
		CM[iy*wx]=CM[iy*wx+1];
		CM[iy*wx+nx+1]=CM[iy*wx+nx];
	}
	
	// перейти от квадратов размера SqrSize/2 к пересекающимся
	// квадратам размера SqrSize
	for(iy=0; iy<=ny; iy++)
	{
		n=iy*wx;
		for(ix=0; ix<=nx; ix++,n++)
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
	}

	// выбрать фон
	int *C;

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
			for(iy=0; iy<=ny; iy++)
			{
				n=iy*wx;
				for(int ix=0; ix<nx; ix++,n++)
				{
					C0sum += abs(C0[n+1]-C0[n]);
					C1sum += abs(C1[n+1]-C1[n]);
					CMsum += abs(CM[n+1]-CM[n]);
				}
			}
			for(iy=0; iy<ny; iy++)
			{
				n=iy*wx;
				for(int ix=0; ix<=nx; ix++,n++)
				{
					C0sum += abs(C0[n+wx]-C0[n]);
					C1sum += abs(C1[n+wx]-C1[n]);
					CMsum += abs(CM[n+wx]-CM[n]);
				}
			}
			C=CM; double Csum=CMsum;					
			if(C0sum<Csum && (m_EvenMethod!=0)) {C=C0; Csum=C0sum; } // при методе 4 фон не может быть темным
			if(C1sum<Csum) {C=C1; Csum=C1sum; }
		}
		break;
	}

	int cAvg, cMaxAvg, cMinAvg;
	int CMin=MaxColor, CMax=0;

	double cSum=0, cMaxSum=0, cMinSum=0;
	for(iy=1; iy<=ny; iy++)
	{
		for(ix=1; ix<=nx; ix++)
		{
			cSum+=C[iy*wx+ix];
			cMinSum+=C0[iy*wx+ix];
			cMaxSum+=C1[iy*wx+ix];
			if(C0[iy*wx+ix]<CMin) CMin=C0[iy*wx+ix];
			if(C1[iy*wx+ix]>CMax) CMax=C1[iy*wx+ix];
		}
	}
	cAvg=int(cSum/numn);
	cMinAvg=int(cMinSum/numn);
	cMaxAvg=int(cMaxSum/numn);

	int evenCoeff = 256*cAvg;
		
	// Коеффициенты контраста
	if(m_EvenMethod == 1) // фон вычитается
	{
		for(iy=0; iy<=ny+1; iy++)
		{
			n=iy*wx;
			for(ix=0; ix<=nx+1; ix++,n++)
				C_coeff[n]=256;
		}
	}
	else // фон делится
	{
		for(iy=0; iy<=ny+1; iy++)
		{
			n=iy*wx;
			for(ix=0; ix<=nx+1; ix++,n++)
				C_coeff[n]=evenCoeff/C[n];
		}
	}

	for(iy=0; iy<=ny; iy++)
		for(ix=0; ix<=nx; ix++)
			B_Ground[iy*wx+ix]=C[iy*wx+ix];

	// интерполяция цвета фона и контрастирование по всему изображению 
	for(iy=0; iy<ny; iy++)
	{
		for(ix=0; ix<nx; ix++)
		{
			x0=sqr_x[ix]; x1=sqr_x[ix+1];
			y0=sqr_y[iy]; y1=sqr_y[iy+1];
			int dx = x1-x0;
			int dy = y1-y0;
			
			int c0=C0[iy*wx+ix];
			int c1=C1[iy*wx+ix];
			int cm=CM[iy*wx+ix];
			
			double BackGround0=B_Ground[iy*wx+ix];
			double BackGround1=B_Ground[iy*wx+ix+1];
			double dBackGround0_dy=static_cast<double>(B_Ground[(iy+1)*wx+ix]-BackGround0)/dy;
			double dBackGround1_dy=static_cast<double>(B_Ground[(iy+1)*wx+ix+1]-BackGround1)/dy;

    		double Contr0=C_coeff[iy*wx+ix];
			double Contr1=C_coeff[iy*wx+ix+1];
			double dContr0_dy=static_cast<double>(C_coeff[(iy+1)*wx+ix]-Contr0)/dy;
			double dContr1_dy=static_cast<double>(C_coeff[(iy+1)*wx+ix+1]-Contr1)/dy;

			for(int y=y0; y<y1; y++) 
			{
				int BackGround=static_cast<int>(BackGround0*256);
				int dBackGround_dx=static_cast<int>((BackGround1-BackGround0)/dx*256);
				BackGround0+=dBackGround0_dy;
				BackGround1+=dBackGround1_dy;

				int Contr=static_cast<int>(Contr0*256);
				int dContr_dx=static_cast<int>((Contr1-Contr0)*256/dx);
				Contr0+=dContr0_dy;
				Contr1+=dContr1_dy;

				color *Dst=DstRows[y]+x0;
				color *Src=SrcRows[y]+x0;

				for(int x=x0; x<x1; x++)
				{
					int temp=((*(Src++)-(BackGround/256))*(Contr/256))/256+cAvg;

					// Контрастирование
					if(temp > x1edge && temp <= x2edge)
						temp = temp*ContrCoeffInt/256+tempPart;
					else
						if(temp <= x1edge)
							temp = coeff1edgeInt*temp/256;
						else
							temp = temp*klineInt/256+blineInt;
					
					if (temp<0) temp=0;
					if (temp>MaxColor) temp=MaxColor;

					*(Dst++)=temp;

					BackGround+=dBackGround_dx;
					Contr+=dContr_dx;
				}
			}
		}
		Notify( sqr_y[iy] );
	}
	NextNotificationStage( );
}

bool CEvenVFilter::InvokeFilter()
{
	m_SqrNumber=GetArgLong(szSqrNumber);
	m_EvenMethod=GetArgLong(szEvenMethod);
	m_BackgroundType=GetArgLong(szBackgroundType);
	m_EvenBrightness=GetArgLong(szEvenBrightness);
	m_Contrast=GetArgLong(szContrast);

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

	// гистограмма цветов
	_ptr_t2<double> smartHist(MaxBand+1);
	double* hist=smartHist.ptr;
	int halfnum = (size.cy*size.cx)/2;

	// для каждого цвета определяется медианная яркость, находится средняя
	double sumbright=0;
	
	for( int nPane=0;nPane<Panes;nPane++ )
	{
		smartHist.zero();
		
		int i=0, s=0, xturn;

		for (int y=0; y<size.cy; y++)
		{
			color *Srccomp=SrcRows[nPane*size.cy+y];
			for(int x=0; x<size.cx; x++)
				hist[ColorToBand(*(Srccomp++))]++;
		}
		
		while(s<halfnum) s+=hist[i++]; 
		xturn = BandToColorM(i);
		if (xturn<0) xturn=0;
		if (xturn>MaxColor) xturn=MaxColor;

		sumbright += xturn;
	}

	color xturn, yturn;

	// точка, относительно которой производится поворот прямой для контрастирования
	xturn = static_cast<color>(sumbright/Panes);
	yturn = MaxColor/2;

	double ContrastCoeff;

	if (m_Contrast < 100 && m_Contrast > -100)
		ContrastCoeff = tan((45+m_Contrast*0.45)*0.01745329);
	else
		if (m_Contrast == 100) ContrastCoeff = 0xFF;
		else ContrastCoeff = 0.0001;

	// Границы прямых характеристики преобразования яркости
	y1edge = yturn/5;
	x1edge = static_cast<int>((y1edge-xturn+xturn*ContrastCoeff)/ContrastCoeff);
	y2edge = MaxColor-(MaxColor-yturn)/5;
	x2edge = static_cast<int>((y2edge-xturn+xturn*ContrastCoeff)/ContrastCoeff);

	if (m_Contrast == 100) x1edge = x2edge = xturn;

	ContrCoeffInt = static_cast<int>(ContrastCoeff*256);
	tempPart = static_cast<int>(xturn*(1-ContrastCoeff));

	klineInt = 256*(y2edge-MaxColor)/(x2edge-MaxColor);
	blineInt = y2edge-x2edge*(y2edge-MaxColor)/(x2edge-MaxColor);

	coeff1edgeInt = 256*y1edge/x1edge;

	if(m_EvenBrightness && Panes==3) //выравнивание полутонового изображения
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
	else// выравнивание каждой цветовой компоненты
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


	FinishNotification();
	pIClrConv->Release();
	return true;
}
