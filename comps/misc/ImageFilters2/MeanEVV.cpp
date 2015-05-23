// MeanEV.cpp: implementation of the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MeanEVV.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "math.h"


#define MaxColor color(-1)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//
//		MeanEVV
//
//
/////////////////////////////////////////////////////////////////////////////
#define szParam "nParam"
#define szMaskSize "mask_size"

_ainfo_base::arg	CMeanEVVInfo::s_pargs[] = 
{	
	{szMaskSize,	szArgumentTypeInt, "20", true, false },
	{szParam,	szArgumentTypeInt, "100", true, false },
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"Result",	szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false },
};

int xinitmax, limity, limitx, half_Mask, yinitmaxmax, xinitmaxmax, squarestep, Panessquarestep;
int shift, localshift;
int xleft, xright, yup, ydown;
int x, y, center_Mask;
//double denomdenomi;
int denomdenomi;
int reddenom, denomi, indexcoeffROAD;

double indexcoeffdenom;
// Количество элементов в массиве, заменяющем вычисление экспоненты
const int expArraynum = 10000;
// xmax для exp(-xmax)
const int maxvalue = 20;

/////////////////////////////////////////////////////////////////////////////

bool CMeanEVVF::InvokeFilter()
{

	IImage3Ptr	ptrSrcImage( GetDataArgument() );
	IImage3Ptr	ptrDestImage( GetDataResult() );
	
	if( ptrSrcImage == NULL || ptrDestImage == NULL )
		return false;
	{	
		ICompatibleObjectPtr ptrCO( ptrDestImage );
		
		if( ptrCO == NULL )
			return false;

		if( S_OK != ptrCO->CreateCompatibleObject( ptrSrcImage, NULL, 0 ) )
		{			
			return false;
		}		
	}

	// [vanek] 12.12.2003: works with color convertors panes
	CImagePaneCache paneCache( ptrSrcImage );
	//int m_PaneCount = ::GetImagePaneCount( ptrSrcImage );
	m_PaneCount = paneCache.GetPanesCount( );
	
	ptrSrcImage->GetSize((int*)&m_size.cx,(int*)&m_size.cy);

	m_MaskSize = (long)(GetArgLong(szMaskSize));

	// Сторона квадрата может быть меньше половины меньшей стороны изображения
	if(m_MaskSize > min(m_size.cx,m_size.cy)/2) m_MaskSize = min(m_size.cx,m_size.cy)/2;

	// Сторона квадрата будет равна нечетному числу
	m_MaskSize = m_MaskSize/2*2+1;

	// Параметр для радиометрического коэффициента, умножается на 2, чтобы сохранить шкалу от 0 до 100,
	// т.к. для импульсного шума хорошие результаты достигаются при m_Param = 40...90 
	m_Param = GetArgLong(szParam)*2;

	MeanEV(&paneCache, ptrDestImage);

	return true;
}

bool CMeanEVVF::MeanEV(CImagePaneCache *pImageSrcPC, IImage2* pResult)
{
	if( !pImageSrcPC || !pResult )
		return false;

	IUnknown *punk_src_image = 0;
	pImageSrcPC->GetImage( &punk_src_image );
	IImage2Ptr sptr_src( punk_src_image );
    if( punk_src_image )
		punk_src_image->Release( ); punk_src_image = 0;

	if( sptr_src == 0 )
        return false;

	byte *pmask=0;

	center_Mask = m_MaskSize/2+1; // Номер центрального пикселя
	half_Mask = center_Mask-1; // Половина маски

	xinitmax = min(half_Mask,m_size.cx-1); // Правая сторона левого квадрата, с которого начинается расчет гистограмм
	limity = m_size.cy-center_Mask; // Нижняя граница координаты центра квадрата, для которой квадрат еще вписывается в изображение
	limitx = m_size.cx-half_Mask; // Правая граница координаты центра квадрата, для которой квадрат еще вписывается в изображение
	squarestep = m_size.cx-m_MaskSize;
	Panessquarestep = squarestep+(m_PaneCount-1)*m_size.cx;

	if(limity < center_Mask)
		limity = center_Mask+1;
	if(limitx < center_Mask)
		limitx = center_Mask+1;

	yinitmaxmax = m_size.cy-1;
	xinitmaxmax = m_size.cx-1;

	// Пространственная компонента фильтра, которая распределена по гауссовскому закону
	_ptr_t2<double> smartGaussian(center_Mask*center_Mask);
	double* pGaussian = smartGaussian.ptr;

	int k,l;
	double denomS = static_cast<double>(m_MaskSize)/m_coeffGaus;
	denomS = 2*denomS*denomS; // Параметр для коэффициента, осуществляющего гаусовское размытие
	int sqrcenter_Mask = center_Mask*center_Mask;

	for(k=0; k<center_Mask; k++)
		for(l = 0; l<center_Mask; l++)
			pGaussian[k*center_Mask+l] = exp(-(k*k+l*l)/denomS);

	//for(k=0; k<center_Mask; k++)
	//	for(l = 0; l<center_Mask; l++)
	//		pGaussian[k*center_Mask+l] = max(0,(1-(double((k*k+l*l))/sqrcenter_Mask))*(1-(double((k*k+l*l))/sqrcenter_Mask)));

	// Шаг dx от 0 до xmax
	const int indexcoeff = expArraynum/maxvalue;

	// Массив, заменяющий вычисление экспоненты
	_ptr_t2<double> smartexpArray(expArraynum+1);
	double* pexpArray = smartexpArray.ptr;

	for(k=0; k<=expArraynum; k++)
		pexpArray[k] = exp(-double(k)/indexcoeff);

	// Массив, содержащий гистограмму
	_ptr_t2<long> smartAprHist(256);
	long *pAprHist = smartAprHist.ptr;

	// Массив, содержащий строчки изображения
	_ptr_t2<color*> smartRows(m_size.cy);
	color** pRows = smartRows.ptr;

	// Массив, содержащий гистограмму для левого столбца квадрата
	_ptr_t2<color*> smartMaskRowsL(m_MaskSize);
	pMaskRowsL = smartMaskRowsL.ptr;

	// Массив, содержащий гистограмму для правого столбца квадрата
	_ptr_t2<color*> smartMaskRowsR(m_MaskSize);
	pMaskRowsR = smartMaskRowsR.ptr;

	// Массив, содержащий коэффиценты ROAD, умноженные на нормирующие коэффициенты
	_ptr_t2<int> smartROADsumArray(m_size.cx*m_size.cy);
	int* pROADsumArray = smartROADsumArray.ptr;
	ZeroMemory(pROADsumArray,sizeof(int)*m_size.cx*m_size.cy);

	color *pDst=0,*pSrc=0,*pTemp=0;

	// Знаменатель степени экспоненты для коэффициента wR
	double denom = 2*double(m_Param)*m_Param;

	int redParam = m_Param/256;
	reddenom = 2*redParam*redParam;

	double sigmai = static_cast<double>(m_MaskSize)/m_coeffSigmai;
	// Знаменатель степени экспоненты для коэффициента wI
	denomi = 2*sigmai*sigmai;

	double denomROAD = 4*denom/65025;
	indexcoeffROAD = int(256*sqrt(indexcoeff/denomROAD)); 
	indexcoeffdenom = indexcoeff*65536/denom;
	denomdenomi = denom/denomi;
	
	int PaneRightStart = m_PaneCount*m_size.cx-xinitmaxmax-1;
	int j;

	StartNotification(m_size.cy,m_PaneCount);

	// Заполнение массивов pROADsumArray и psqrROADsumArray
	for(int nPane=0;nPane<m_PaneCount;nPane++)
	{
		if( !pImageSrcPC->IsPaneEnable( nPane ) ) 
		{			
			continue;
		}

		for(y=0; y<m_size.cy; y++)
			sptr_src->GetRow(y,nPane,&pRows[y]);
		
		for( y=0;y<m_size.cy;y++)
		{
			sptr_src->GetRowMask( y, &pmask );
			pSrc=pRows[y];
			// гистограмма для первого квадрата слева
			InitHist(y,pRows,pAprHist);

			for( x = 0; x<half_Mask; x++, pmask++, pSrc++)
			{
				if( *pmask>=128 )
					ROADsum(pSrc, pROADsumArray, pAprHist);

				// для левой части гистограмма для квадрата, смещенного вправо на один пиксель
				for(j=0; j<m_MaskSize; j++) // Add right
				{ 
					if(pMaskRowsR[j]!=0)
						pAprHist[*(pMaskRowsR[j]++)>>8]++;
				} 
			}

			for( x = half_Mask; x<limitx; x++, pmask++, pSrc++)
			{
				if( *pmask>=128 )
					ROADsum(pSrc, pROADsumArray, pAprHist);

				// для центральной части гистограмма для квадрата, смещенного вправо на один пиксель
				for(j=0; j<m_MaskSize; j++)// Add right
				{ 
					if(pMaskRowsR[j]!=0)
						pAprHist[*(pMaskRowsR[j]++)>>8]++;
					if(pMaskRowsL[j]!=0)
						pAprHist[*(pMaskRowsL[j]++)>>8]--;
				} 
			}

			for( x = limitx; x<m_size.cx; x++, pmask++, pSrc++)
			{
				if( *pmask>=128 )
					ROADsum(pSrc, pROADsumArray, pAprHist);

				// для правой части гистограмма для квадрата, смещенного вправо на один пиксель
				for(j=0; j<m_MaskSize; j++)// Remove left
				{ 
					if(pMaskRowsL[j]!=0)
						pAprHist[*(pMaskRowsL[j]++)>>8]--;
				} 
			}
			Notify( y );
		}
	}

	for(int nPane=0;nPane<m_PaneCount;nPane++)
	{
		for(y=0; y<m_size.cy; y++)
			sptr_src->GetRow(y,nPane,&pRows[y]);
		
		for( y=0;y<m_size.cy;y++ )
		{
			sptr_src->GetRowMask( y, &pmask );
			pResult->GetRow( y, nPane , &pDst);
			pSrc=pRows[y];
			shift = y*m_size.cx;

			yup = max(0,y-half_Mask);
			ydown = min(yinitmaxmax,y+half_Mask);
			int localshiftstart = yup*m_size.cx;
            
			// левая часть
		    xleft = 0;
			xright = half_Mask;

			for(x = 0; x<half_Mask; x++, pmask++, pSrc++, pDst++, shift++, xright++)
			{
				if( *pmask>=128 )
				{
					localshift = localshiftstart;
					pTemp = pRows[yup];

					*pDst = BrDst(pSrc, pTemp, pROADsumArray, pexpArray, pGaussian, m_PaneCount*m_size.cx-xright-1);
				}				
			}

			// центральная часть
			xleft = 0;
			xright = m_MaskSize-1;

			for(x = half_Mask; x<limitx; x++, pmask++, pSrc++, pDst++, shift++, xleft++, xright++, localshift++)
			{
				if( *pmask>=128 )
				{
					localshift = localshiftstart+xleft;
					pTemp = pRows[yup] + xleft;

					*pDst = BrDst(pSrc, pTemp, pROADsumArray, pexpArray, pGaussian, Panessquarestep);
				}
			}

			// правая часть
			xleft = limitx-half_Mask;
			xright = xinitmaxmax;

			for(x = limitx; x<m_size.cx; x++, pmask++, pSrc++, pDst++, shift++, xleft++)
			{
				if( *pmask>=128 )
				{
					localshift = localshiftstart+xleft;
					pTemp = pRows[yup] + xleft;

					*pDst = BrDst(pSrc, pTemp, pROADsumArray, pexpArray, pGaussian, PaneRightStart+xleft);
				}				
			}

			Notify( y );
		}
		NextNotificationStage();
	}
	FinishNotification();

	return true;
}

bool CMeanEVVF::InitHist(int y, color **pRows, long *pAprHist)
{
	ZeroMemory(pMaskRowsL,sizeof(color*)*m_MaskSize);
	ZeroMemory(pMaskRowsR,sizeof(color*)*m_MaskSize);
	ZeroMemory(pAprHist,sizeof(long)*256);

	int yinitmax, yinitmin;
    if(y<limity)  yinitmax=y+half_Mask; else  yinitmax = yinitmaxmax;
	if(y>half_Mask)  yinitmin=y-half_Mask; else  yinitmin = 0;

	int k=0;

	for(long j=yinitmin; j<=yinitmax; j++)
	{
		for(long x=0; x<=xinitmax; x++)
			pAprHist[pRows[j][x]>>8]++;

		pMaskRowsL[k] = pRows[j];
		pMaskRowsR[k] = &pRows[j][x];
		k++;
	}
	return true;
}

inline	bool CMeanEVVF::ROADsum(color* pSrc, int* pROADsumArray, long *pAprHist)
{
	int bAprPoint, nCount = 0, ROADsum = 0, il, ir, differ = 0;

	bAprPoint = (*pSrc)>>8;
	il = bAprPoint;
	ir = bAprPoint;

	while (nCount < m_ROADnum) // Количество пикселей, яркость которых близка к центральному, меньше m_ROADnum
	{
		if(--il >= 0 && ++ir < 256) // Существуют обе компоненты гистограммы с индексами il и ir
		{
			nCount += pAprHist[il]+pAprHist[ir];
			ROADsum += (pAprHist[il]+pAprHist[ir])*(differ++);
		}
		else
			if(--il >= 0) // Существует только компонента гистограммы с индексом il
			{
				nCount += pAprHist[il]; ROADsum += pAprHist[il]*(differ++);
			}
			else
				if(++ir < 256) // Существует только компонента гистограммы с индексом ir
				{
					nCount += pAprHist[ir]; ROADsum += pAprHist[ir]*(differ++);
				}
				else
					break;
	}
	// Элемент массива будет содержать наибольший среди коэффициентов ROAD для трех слоев 
	if(ROADsum > pROADsumArray[y*m_size.cx+x])
		pROADsumArray[y*m_size.cx+x] = ROADsum;
	return true;
}

inline color CMeanEVVF::BrDst(color* pSrc, color* pTemp, int* pROADsumArray, double* pexpArray, double* pGaussian, int pTempStep)
{
	double w,wsum = 0,wBrsum = 0;
	int index, ww;

	for(int k=yup; k<=ydown; k++, localshift += squarestep, pTemp += pTempStep)
	{
		for(int l=xleft; l<=xright; l++, pTemp++, localshift++)
		{
			// Числитель степени экспоненты для радиометрического коэффициента
			ww = (*pSrc-*pTemp)/256;
			ww *=ww;

			// Числитель степени экспоненты для J
			index = indexcoeffROAD*(pROADsumArray[shift]+pROADsumArray[localshift])/256;
			index *= index;

			// wR^(1-J)*(wI^J)
			if(index <= expArraynum && index >= 0)
				index = int(indexcoeffdenom*((pROADsumArray[localshift]*pROADsumArray[localshift]*reddenom/denomi+ww)*(1-pexpArray[index])+ww));
			else
				index = int(indexcoeffdenom*(pROADsumArray[localshift]*pROADsumArray[localshift]*reddenom/denomi+ww*2));

			// wS*wR^(1-J)*(wI^J)
			if(index <= expArraynum && index >= 0)
				w = (1e-10)+pGaussian[abs(k-y)*center_Mask+abs(l-x)]*pexpArray[index];
			else
				w = 1e-10;

			wsum += w;
			wBrsum += w*(*pTemp);
		}
	}
	return color(max(0,min(65535,wBrsum/wsum)));
}