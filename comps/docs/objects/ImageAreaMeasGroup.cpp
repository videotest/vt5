// ImageAreaMeasGroup.cpp : implementation file
//

#include "stdafx.h"
#include "Objects.h"
#include "ImageAreaMeasGroup.h"
#include "Params.h"
#include "NameConsts.h"
#include "calibrint.h"
#include "units_const.h"
#include <Math.h>
#include "alloc.h"
#include "Skeleton.h"
#include "misc_utils.h"

CDoubleIniValue g_BriOrderRel("\\measurement", "BriOrderRel", 0.5);
CIntIniValue g_BriOrderAbs("\\measurement", "BriOrderAbs", 0);


//задача: поточнее измерить периметр объекта

//алгоритм.
//разбиваем весь контур на участки, на которых меняется только координата x
//или только координата y или однопиксельные диагональные участки.

int get_dir( int dx, int dy )
{
	if( dx == 0 )return (dy<0)?0:4;
	else if( dy == 0 )return (dx>0)?2:6;
	else if( dx > 0 )return (dy<0)?1:3;
	else return (dy<0)?7:5;
}

int get_dir( ContourPoint &pt, ContourPoint &pt_o )
{	return get_dir( pt.x-pt_o.x, pt.y-pt_o.y );}

int sub_dirs( int dir1, int dir2 )
{
	int	res = dir1-dir2;
	if( res >= 4 )res -= 8;
	if( res < -4 )res += 8;
	return res;
}

double calc_len( double dx, double dy )
{	return sqrt( dx*dx+dy*dy );}

//AAM: какое мясо... это же все гораздо проще делается!!!
double measure_perimeter( Contour *pcntr )
{
	double perimeter = 0;

	struct	segment
	{
		int	xs, ys, xe, ye, dir;
		double	xc, yc;
	};

	segment	prev, current, first;
	::ZeroMemory( &prev, sizeof(segment) );
	::ZeroMemory( &current, sizeof(segment) );
	::ZeroMemory( &first, sizeof(segment) );


	ContourPoint	pt_old, pt = pcntr->ppoints[0];

	int	dir = -1;
	long	new_flag = false, first_call = true;

//в любом случае первый сегмент начинается с первой точки контура	
	current.dir = -1;
	current.xs = current.xe = pcntr->ppoints[0].x;
	current.ys = current.ye = pcntr->ppoints[0].y;
	int idx_old = pcntr->nContourSize-1;

	for( int idx = 1; idx < pcntr->nContourSize; idx++ )
	{
		pt_old = pt;

		pt = pcntr->ppoints[idx];
		dir = get_dir( pt, pt_old );

		if( current.dir == -1 )//только начали обходить
		{
			if( (dir & 1) )	//у нас случай когда первая точка контура одиночная
			{	//в этом случае направление на первую с последней - искомое
				current.dir = get_dir( pt_old, pcntr->ppoints[idx_old] );
				new_flag = true;	//потом - добавим и пойдем дальше
			}
			else
			{
				current.dir = dir;	//иначе - в начале у нас неединичный сегмент с указанным
				//направлением
			}
		}

		idx_old = idx;

		if( new_flag || dir != current.dir )
		{
			current.xc = (double)(current.xs+current.xe)/2;
			current.yc = (double)(current.ys+current.ye)/2;


			if( !first_call )	
				//в случае, когда это первый сегмент, нам нельзя считать
				//расстояние между сегментами - только сохраним на будущее последний	
			{
				//нашли сегмент - тут надо обновить периметр

				int	delta_dir = sub_dirs( prev.dir, current.dir );
				if( abs( delta_dir ) < 2 )
				{
					//в этом случае мы можем соединять центры
					perimeter+=calc_len( prev.xc-current.xc, prev.yc-current.yc );
				}
				else
				{
					//в этом случае - до конца старого интервала, 
					//до начала текущего, до центра текущего
					perimeter+=calc_len( prev.xc-prev.xe, prev.yc-prev.ye );
					if( prev.xe != current.xs || prev.ye != current.ys )
						perimeter+=calc_len( prev.xe-current.xs, prev.ye-current.ys );
					perimeter+=calc_len( current.xs-current.xc, current.ys-current.yc );
				}
			}
			else
			{
				memcpy( &first, &current, sizeof( segment ) );
				first_call = false;
			}

			//передвинулись дальше
			memcpy( &prev, &current, sizeof( segment ) );
			
			current.dir = dir;

			if( new_flag )
			{
				current.xe = current.xs = pt.x;
				current.ye = current.ys = pt.y;
			}
			else
			{
				current.xs = pt_old.x;
				current.ys = pt_old.y;
				current.xe = pt.x;
				current.ye = pt.y;
			}
			
			if( dir & 1 )
				current.dir = -1;
			new_flag = false;
		}
		else
		{
			current.xe = pt.x;
			current.ye = pt.y;
		}
	}
	//проверяем, относится ли первая точка к последнему сегменту
	pt_old = pt;
	pt = pcntr->ppoints[0];

	dir = get_dir( pt, pt_old );

	if( (dir & 1) )
	{
		//не относится - диагональное направление
	}
	else
	{
		current.xe = pt.x;
		current.ye = pt.y;
	}
//добавляем последний сегмент
	if(!first_call)
	{
		current.xc = (double)(current.xs+current.xe)/2;
		current.yc = (double)(current.ys+current.ye)/2;
		int	delta_dir = sub_dirs( prev.dir, current.dir );
		
		if( abs( delta_dir ) < 2 )
		{
			//в этом случае мы можем соединять центры
			perimeter+=calc_len( prev.xc-current.xc, prev.yc-current.yc );
		}
		else
		{
			//в этом случае - до конца старого интервала, 
			//до начала текущего, до центра текущего
			perimeter+=calc_len( prev.xc-prev.xe, prev.yc-prev.ye );
			if( prev.xe != current.xs || prev.ye != current.ys )
				perimeter+=calc_len( prev.xe-current.xs, prev.ye-current.ys );
			perimeter+=calc_len( current.xs-current.xc, current.ys-current.yc );
		}	
		
		memcpy( &prev, &current, sizeof( segment ) );
		memcpy( &current, &first, sizeof( segment ) );
	//добавляем участок от последнего к первому
		delta_dir = sub_dirs( prev.dir, current.dir );
		if( abs( delta_dir ) < 2 )
		{
			//в этом случае мы можем соединять центры
			perimeter+=calc_len( prev.xc-current.xc, prev.yc-current.yc );
		}
		else
		{
			//в этом случае - до конца старого интервала, 
			//до начала текущего, до центра текущего
			perimeter+=calc_len( prev.xc-prev.xe, prev.yc-prev.ye );
			if( prev.xe != current.xs || prev.ye != current.ys )
				perimeter+=calc_len( prev.xe-current.xs, prev.ye-current.ys );
			perimeter+=calc_len( current.xs-current.xc, current.ys-current.yc );
		}	
	}

	return perimeter;
}

double measure_perimeter_smooth( Contour *pcntr, int nSmooth )
{	// вычисление периметра контура
	// используем простое эффективное сглаживание:
	// для отрезка R[i], R[i+1] к периметру прибавляем длину его проекции
	// на отрезок R[i-nSmooth], R[i+1+nSmooth]
	// ограничение: nSmooth меньше четверти контура - ограничим nSize/6

	int nSize=pcntr->nContourSize;
	ContourPoint *pp=pcntr->ppoints;
	int di=min(nSmooth,nSize/6);
	double per = 0;

	for(int i0=0; i0<nSize; i0++)
	{
		int i1=i0+1; if(i1==nSize) i1=0;
		int j0=i0-di; if(j0<0) j0+=nSize;
		int j1=i1+di; if(j1>=nSize) j1-=nSize;
		int dx = pp[j1].x - pp[j0].x;
		int dy = pp[j1].y - pp[j0].y;
		double d = _hypot(dx,dy);
		if(d<0.1)
			per += _hypot(pp[i1].x-pp[i0].x,pp[i1].y-pp[i0].y);
		else per += ( (pp[i1].x-pp[i0].x)*dx + (pp[i1].y-pp[i0].y)*dy ) / d;
	}
	return per;
}

void measure_concavity_len( Contour *pcntr, int nSmooth,
						   int *pnConvex, int *pnConcave)
{	// Сосчитать длину выпуклых и вогнутых участков периметра
	// (прибавляется соответственно к *pnConvex и *pnConcave
	int nSize=pcntr->nContourSize;
	ContourPoint *pp=pcntr->ppoints;
	int di = max(1, min(nSmooth,nSize/6) );

	for(int i=0; i<nSize; i++)
	{
		int j0=i-di; if(j0<0) j0+=nSize;
		int j1=i+di; if(j1>=nSize) j1-=nSize;
		int dx0 = pp[j0].x - pp[i].x;
		int dy0 = pp[j0].y - pp[i].y;
		int dx1 = pp[j1].x - pp[i].x;
		int dy1 = pp[j1].y - pp[i].y;
		int nCross = dx0*dy1 - dy0*dx1; // векторное произведение векторов, идущих к двум соседним точкам
		if(nCross>0)
			(*pnConcave)++;
		else if(nCross<0)
			(*pnConvex)++;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CImageAreaMeasGroup

IMPLEMENT_DYNCREATE(CImageAreaMeasGroup, CMeasParamGroupBase)

// {141365F5-8A88-4b16-8880-219B1E6633E8}
static const GUID clsidImageAreaMeasGroup = 
{ 0x141365f5, 0x8a88, 0x4b16, { 0x88, 0x80, 0x21, 0x9b, 0x1e, 0x66, 0x33, 0xe8 } };
// {9073A7C5-7B4C-48b5-B7F1-AD28D903FEC0}
IMPLEMENT_GROUP_OLECREATE(CImageAreaMeasGroup, "Objects.ImageAreaMeasGroup", 
0x9073a7c5, 0x7b4c, 0x48b5, 0xb7, 0xf1, 0xad, 0x28, 0xd9, 0x3, 0xfe, 0xc0);


CImageAreaMeasGroup::CImageAreaMeasGroup()
{
	_OleLockApp( this );
	m_sName = "Common";
	m_bUseSmooth = GetValueInt(GetAppUnknown(), "\\measurement", "ApplySmoothFilter", 0) == 1;
	m_nSmoothLevel = GetValueInt(GetAppUnknown(), "\\measurement", "PerimeterSmoothLevel", 4);
}

CImageAreaMeasGroup::~CImageAreaMeasGroup()
{
	_OleUnlockApp( this );
}

static void RGB_to_HSL( double r, double g, double b, double *h, double *s, double *l)
{	// from Graphics Gems
	double v;
	double m;
	double vm;
	double r2, g2, b2;
	//double r, g, b;

	// r = ((double)r1) / 255.;
	// g = ((double)g1) / 255.;
	// b = ((double)b1) / 255.;

	v = max( r, max( g, b ) );
	m = min( r, min( g, b ) );

	if ((*l = (m + v) / 2.0) <= 0.0)
		return;
	if ((*s = vm = v - m) > 0.0) {
		*s /= (*l <= 0.5) ? (v + m ) : (2.0 - v - m);
	} else return;

	r2 = (v - r) / vm;
	g2 = (v - g) / vm;
	b2 = (v - b) / vm;

	if (r == v)
		*h = (g == m ? 5.0 + b2 : 1.0 - g2);
	else if (g == v)
		*h = (b == m ? 1.0 + r2 : 3.0 - b2);
	else
		*h = (r == m ? 3.0 + g2 : 5.0 - r2);

	*h /= 6;
}

/////////////////////////////////////////////////////////////////////////////
// CImageAreaMeasGroup message handlers

#pragma optimize("", off)

bool CImageAreaMeasGroup::CalcValues( IUnknown *punkCalcObject, IUnknown *punkSource )
{
//[AY}begin image measurement prologue
	CImageWrp imageCalc = punkSource;
	double	fCalibr = 0.0025;
	ICalcObjectPtr	ptrCalc = punkCalcObject;
	
	//init calibratin and image
	ICalibrPtr	ptrCalibr = imageCalc;
	if( ptrCalibr != 0 )ptrCalibr->GetCalibration( &fCalibr, 0 );

	if (imageCalc.IsEmpty())
	{
//		ptrCalc->ClearValues();
		return true;
	}
//end 


		//parameters
		double	__fPint = 0,		//internal perimeter
				__fP = 0;			//and full perimeter
		double	__fS = 0,			//body area
				__fSInt = 0;		//holes area
		double	__fXCoord = 0,		//Center of object X
				__fYCoord = 0;		//Center of object Y
		double	__fRed = 0,			//reg
				__fGreen = 0,		//green
				__fBlue = 0;		//blue comp
		double	__fBigAxis = 0,		//Big axis of ellipse
				__fSmallAxis = 0,	//Small axis of ellipse
				__fAngle = 0;		//Orientation	(Warning! in deg)
		double	__fHeight = 0,		//Height
				__fWidth = 0,		//Width
				__fXSize = 0,		//size by x
				__fYSize = 0,		//size by y
				__fAvrChord = 0,	//average chord
				__fLineL = 0,		//Length(line)
				__fLineW = 0,		//Width(line)
				__fRibbonL = 0,		//Length(ribbon)
				__fRibbonW = 0,		//Width(ribbon)
				__fNoun = 0,		//Elongation
				__fDimesion = 0;
		double	__fDE = 0,	
				__fFFCircle = 0,	//Form Factor Circle
				__fFFCircle2 = 0,	//Form Factor Circle
				__fFFEllipse = 0;	//Form Factor Ellipse
		double	__fBriAverage = 0,	//average brightness
				__fBriDev =0,		//deviation of brightness
				__fBriMin = 0,		//min brightness
				__fBriMax = 0,		//max brightness
				__fBriInt = 0,		//brightness interval
				__fBriInteg = 0,	//integrall bri
				__fBriOrdered = 0;	//ordered brightness
		double	__fLC = 0;			//Фактор локальной корреляции
		double	__fHue = 0,
				__fSaturation = 0;
		double	__fConcavePercent = 0;			//Процент вогнутого
	

		bool bEnablePint = ParamIsEnabledByKey(KEY_INT_PERIMETER);	
		bool bEnablePext = ParamIsEnabledByKey(KEY_EXT_PERIMETER);	
		bool bEnableP = ParamIsEnabledByKey(KEY_PERIMETER);	
		bool bEnableSint = ParamIsEnabledByKey(KEY_INT_AREA);	
		bool bEnableS = ParamIsEnabledByKey(KEY_AREA);	
		bool bEnableXCoord = ParamIsEnabledByKey(KEY_XCOORD);	
		bool bEnableYCoord = ParamIsEnabledByKey(KEY_YCOORD);	
		bool bEnableRed = ParamIsEnabledByKey(KEY_RED);	
		bool bEnableGreen = ParamIsEnabledByKey(KEY_GREEN);	
		bool bEnableBlue = ParamIsEnabledByKey(KEY_BLUE);	
		bool bEnableHue = ParamIsEnabledByKey(KEY_HUE);
		bool bEnableSaturation = ParamIsEnabledByKey(KEY_SATURATION);
		bool bEnableBigAxis = ParamIsEnabledByKey(KEY_ELPSMAX);	
		bool bEnableSmallAxis = ParamIsEnabledByKey(KEY_ELPSMIN);	
		bool bEnableAngle = ParamIsEnabledByKey(KEY_ANGLE);	
		bool bEnableHeight = ParamIsEnabledByKey(KEY_LENGTH);	
		bool bEnableWidth = ParamIsEnabledByKey(KEY_WIDTH);	
		bool bEnableXSize = ParamIsEnabledByKey(KEY_XLEN);	
		bool bEnableYSize = ParamIsEnabledByKey(KEY_YLEN);	
		bool bEnableAvrChord = ParamIsEnabledByKey(KEY_AVRCHORD);	
		bool bEnableLineL = ParamIsEnabledByKey(KEY_LINEL);	
		bool bEnableLineW = ParamIsEnabledByKey(KEY_LINEW);	
		bool bEnableRibbonL = ParamIsEnabledByKey(KEY_RIBBONL);	
		bool bEnableRibbonW = ParamIsEnabledByKey(KEY_RIBBONW);	
		bool bEnableNoun = ParamIsEnabledByKey(KEY_NOUN);	
		bool bEnableDimesion = ParamIsEnabledByKey(KEY_DIM);	
		bool bEnableDE = ParamIsEnabledByKey(KEY_DECV);	
		bool bEnableFFCircle = ParamIsEnabledByKey(KEY_CIRCLE);	
		bool bEnableFFCircle2 = ParamIsEnabledByKey(KEY_CIRCLE2);	
		bool bEnableFFEllipse = ParamIsEnabledByKey(KEY_ELLIPSE);	
		bool bEnableBriAverage = ParamIsEnabledByKey(KEY_BRIAVR);	
		bool bEnableBriDev = ParamIsEnabledByKey(KEY_BRIDEV);	
		bool bEnableBriMin = ParamIsEnabledByKey(KEY_BRIMIN);	
		bool bEnableBriMax = ParamIsEnabledByKey(KEY_BRIMAX);	
		bool bEnableBriInt = ParamIsEnabledByKey(KEY_BRIINT);	
		bool bEnableBriInteg = ParamIsEnabledByKey(KEY_BRIINTEGRAL);	
		bool bEnableBriOrdered = ParamIsEnabledByKey(KEY_BRIORDERED);	
		bool bEnableSkeletonPerimeterRatio =  ParamIsEnabledByKey(KEY_SKELETONPERIMETERRATIO);
		bool bEnableLC =  ParamIsEnabledByKey(KEY_LC);
		bool bEnableConcavePercent =  ParamIsEnabledByKey(KEY_CONCAVE_PERCENT);


		double	fCalibrXY = fCalibr*fCalibr;
		double	fSumX, 
				fSumY;
		double  fSumXX,
				fSumYY,
				fSumXY;
		int nContourCount = imageCalc.GetContoursCount();

		long	iNum = 0;
		fSumX = fSumY = fSumXY = fSumYY = fSumXX = 0;


		
		if(nContourCount)
		{
//calculate step 1 - measure external perimeter
			//__fP = ContourMeasurePerimeter(imageCalc.GetContour(0), m_bUseSmooth, fCalibr, fCalibr);
			// Ко всем вычисленным периметрам контуров прибавляем по 3-4 пиксела - они примерно на столько занижены из-за того, что контур сдвинут на полпиксела внутрь объекта
			if(m_bUseSmooth)
				__fP = ( 3.0 + measure_perimeter_smooth(imageCalc.GetContour(0), m_nSmoothLevel) ) * fCalibr;
			else
				__fP = ( 4.0 + measure_perimeter( imageCalc.GetContour(0) ) )*fCalibr;

			int nConvex=0, nConcave=0;
			if(bEnableConcavePercent)
			{
				measure_concavity_len( imageCalc.GetContour(0), m_nSmoothLevel,
					&nConvex, &nConcave);
			}

//calculate step 2 - measure internal perimeter
			__fPint=0;
			for(long i = 1; i< nContourCount; i++ )
			{
				//__fPint += ContourMeasurePerimeter(imageCalc.GetContour(i), m_bUseSmooth, fCalibr, fCalibr);
				// Из всех вычисленным длин контуров вычитаем 3 пиксела - они примерно на столько занижены из-за того, что контур сдвинут на полпиксела внутрь объекта
				if(m_bUseSmooth)
					__fPint += max( 0, -3.0 + measure_perimeter_smooth(imageCalc.GetContour(i), m_nSmoothLevel) ) * fCalibr;
				else
					__fPint += max( 0, -3.0 + measure_perimeter( imageCalc.GetContour(i) ) ) * fCalibr;
				if(bEnableConcavePercent)
				{
					measure_concavity_len( imageCalc.GetContour(i), m_nSmoothLevel,
						&nConcave, &nConvex); // concave и convex для внутренних меняются местами
				}
			}
			__fConcavePercent = double(nConcave) / max(1, nConcave+nConvex);
		}
		else
		{	// 28.04.2006 build 103 BT 5185
			// Вычисление периметра без использования контуров
			int xmin = 0;
			int xmax = imageCalc.GetWidth();
			int ymin = 0;
			int ymax = imageCalc.GetHeight();
			double dp[5]={0.0, 0.7071*fCalibr, 1.0*fCalibr ,0.7071*fCalibr ,0.0};

			for( int  y = ymin-1; y<ymax; y++ ) // идем с запасом
			{
				byte* pm = imageCalc.GetRowMask(y);
				byte* pm1 = imageCalc.GetRowMask(y+1);
				for( int  x = xmin-1; x<xmax; x++ )
				{
					int n=0;
					if(y>=ymin) // можно сэкономить время на проверках, если вынести обработку карев прямоугольника за цикл
					{
						if(x>=xmin && pm[x]>=128) n++;
						if(x+1<xmax && pm[x+1]>=128) n++;
					}
					if(y+1<ymax)
					{
						if(x>=xmin && pm1[x]>=128) n++;
						if(x+1<xmax && pm1[x+1]>=128) n++;
					}
					__fP += dp[n];
				}
			}
		}
		__fP+=__fPint;


//calculate step 3: measure area and calc number of h-lines
		BOOL	bPrevBody;
		int	bright;
		int		xc;
		int	br, bg, bb;
		int xmax,ymax;
		int xmin,ymin;
		int		iNumOfLines = 0;
		DWORD	pdwHist[256];
		ZeroMemory(pdwHist, sizeof(DWORD)*256);
		long nColors = imageCalc.GetColorsCount();
		byte	*pRowMask;
	
		CRect rcMeas = imageCalc.GetRect();

		xmin = 0;
		xmax = imageCalc.GetWidth();
		ymin = 0;
		ymax = imageCalc.GetHeight();

		/*xmin -= rcMeas.left;
		xmax -= rcMeas.left;
		ymin -= rcMeas.top;
		ymax -= rcMeas.top;*/

		double __fXOffset=0, __fYOffset=0; // смещение кадра, хранящееся как const параметры
        ptrCalc->GetValue(KEY_X_OFFSET, &__fXOffset);
        ptrCalc->GetValue(KEY_Y_OFFSET, &__fYOffset);
		// Если этих смещений не получим - будет (0,0) - все по старому


		for( int  y = ymin; y< ymax; y++ )
		{
			bPrevBody = FALSE;
			
			pRowMask = imageCalc.GetRowMask(y);

			color* pRowR = imageCalc.GetRow(y, 0);
			color* pRowG = imageCalc.GetRow(y, 1);
			color* pRowB = imageCalc.GetRow(y, 2);
			for( int  x = xmin; x< xmax; x++ )
			{
				
				if(pRowMask[x] > 1)
				{
					__fS+=fCalibrXY;
					if( !bPrevBody )
					{
						bPrevBody = TRUE;
						iNumOfLines++;
					}
					__fXCoord+=x*fCalibr;
					__fYCoord+=y*fCalibr;

					fSumX += x;
					fSumY += y;
					fSumXX += (double)x*x;
					fSumYY += (double)y*y;
					fSumXY += (double)x*y;
					iNum++;
					
					if(nColors > 1)//true color
					{
						xc = x;
						
						br = ColorAsByte(pRowR[xc]);
						bg = ColorAsByte(pRowG[xc]);
						bb = ColorAsByte(pRowB[xc]);
						bright = Bright(bb, bg, br);

						__fRed += pRowR[xc];
						__fGreen += pRowG[xc];
						__fBlue += pRowB[xc];
					}
					else
					{
						xc = x;
						bb = br = bg = bright = ColorAsByte(pRowR[xc]);

						__fRed += pRowR[xc];
						__fGreen += pRowR[xc];
						__fBlue += pRowR[xc];

					}

					pdwHist[bright]++;


					continue;
				}
				//holes
				if(pRowMask[x] == 1)
				{
					__fSInt+=fCalibrXY;
				}
				bPrevBody = FALSE;
			}
		}
		if (__fS != 0.)
			__fXCoord/=__fS/fCalibrXY;
		if (__fS != 0.)
			__fYCoord/=__fS/fCalibrXY;

		__fXCoord += rcMeas.left*fCalibr;
		__fYCoord += rcMeas.top*fCalibr;

		__fXCoord += __fXOffset*fCalibr; // для blood - дополнительно к смещению картинки еще смещение кадра на столе
		__fYCoord += __fYOffset*fCalibr;

		if( !iNum )
			iNum = 1;
		if( iNum )
		{
			__fRed /= iNum;
			__fGreen /= iNum;
			__fBlue /= iNum;
		}
		

//calculate step 4: measure x, y moments
//warning: need fSumX, fSumXX, fSumY, fSumYY
		if(nContourCount && (bEnableAngle||bEnableBigAxis||bEnableSmallAxis||
			bEnableWidth||bEnableHeight||bEnableXSize||bEnableYSize||
			bEnableFFEllipse||bEnableDimesion||bEnableNoun))
		{
			double	fCenterX,
					fCenterY;
			double	fMx, fMy, fMxy, fMavr, fMtmp;
			double	fMa, fMb;
			
			fCenterX = fSumX/iNum;
			fCenterY = fSumY/iNum;

			fMx  = ( fSumYY - 2*fCenterY*fSumY + fCenterY*fCenterY*iNum )
						   *fCalibr*fCalibr*fCalibr*fCalibr;
			fMy  = ( fSumXX - 2*fCenterX*fSumX + fCenterX*fCenterX*iNum )
						   *fCalibr*fCalibr*fCalibr*fCalibr;
			fMxy = ( fSumXY - fCenterY*fSumX - fCenterX*fSumY + fCenterX*fCenterY*iNum )
						   *fCalibr*fCalibr*fCalibr*fCalibr;

/*			if( !fMx )fMx = 1;
			if( !fMy )fMy = 1;
			if( !fMxy )fMxy = 1;*/
			
			fMavr = ( fMx+fMy )/2;
			fMtmp = ::sqrt( fMavr*fMavr - fMx*fMy +	fMxy*fMxy );

			fMa = fMavr+fMtmp;
			fMb = fMavr-fMtmp;

			__fAngle = ::atan2( 2*fMxy, fMy-fMx )/2;
			if(__fAngle < 0)
				__fAngle += PI;

//			if( fMb <= 0 ) fMb = 1.0;
//			if( fMa <= 0 ) fMa = 1.0;


			if( fMb !=0 )
				__fBigAxis = ( 2.*::pow( 4./PI, .25 )*::pow( ::pow( fMa, 3. )/fMb, .125 ) );
			else
				__fBigAxis = fCalibr;
			if( fMa !=0 )
				__fSmallAxis = ( 2.*::pow( 4./PI, .25 )*::pow( ::pow( fMb, 3. )/fMa, .125 ) );
			else
				__fSmallAxis = fCalibr;

		}

//calculate step 5: measure sizes
//warning: need __fAngle, __fS, iNumOfLines, __fP
		if(nContourCount && imageCalc.GetContour(0)->nContourSize != 0 )
		{
			Contour* pContour0 = imageCalc.GetContour(0);
			ContourPoint	pt;
			double	fCosX = ::cos( __fAngle ) * fCalibr, 
					fCosY = ::cos( __fAngle ) * fCalibr,
					fSinX = ::sin( __fAngle ) * fCalibr,
					fSinY = ::sin( __fAngle ) * fCalibr,
					fXMinA, fXMin,
					fXMaxA,	fXMax,
					fYMinA, fYMin,
					fYMaxA, fYMax, fX, fY;

			pt = pContour0->ppoints[0];

			fXMinA = fXMaxA = pt.x*fCosX + pt.y*fSinY;
			fYMinA = fYMaxA = pt.y*fCosY - pt.x*fSinX;
			fXMin = fXMax = pt.x*fCalibr;
			fYMin = fYMax = pt.y*fCalibr;

			for(long i = 0; i < pContour0->nContourSize; i++ )
			{
				pt = pContour0->ppoints[i];

				fX = pt.x*fCosX + pt.y*fSinY;
				fY = pt.y*fCosY - pt.x*fSinX;

				fXMinA = min( fX, fXMinA );
				fXMaxA = max( fX, fXMaxA );
				fYMinA = min( fY, fYMinA );
				fYMaxA = max( fY, fYMaxA );

				fX = pt.x*fCalibr;
				fY = pt.y*fCalibr;

				fXMin = min( fX, fXMin );
				fXMax = max( fX, fXMax );
				fYMin = min( fY, fYMin );
				fYMax = max( fY, fYMax );
			}

			__fWidth = fYMaxA - fYMinA + fCalibr;
			__fHeight = fXMaxA - fXMinA + fCalibr;
			__fXSize = fXMax - fXMin + fCalibr;
			__fYSize = fYMax - fYMin + fCalibr;
			iNumOfLines = max( 1, iNumOfLines );
			__fAvrChord = (__fS)/fCalibr/iNumOfLines;
	
			__fDimesion = (__fHeight+__fWidth)/2;
			if( __fWidth>MIN_FLOAT_VAL )
				__fNoun = __fHeight/__fWidth;
		}


		if(bEnableRibbonL || bEnableRibbonW)
		{
			double fFactor1 = __fP*__fP-16*__fS;
			fFactor1 = max( 0, fFactor1 );
			fFactor1 = ::sqrt( fFactor1 );
			__fRibbonL = (__fP+fFactor1)/4; // by Mick
			__fRibbonW = (__fP-fFactor1)/4;
		}

		if(bEnableLineL || bEnableLineW)
		{
			__fLineW = 2*__fS/__fP;
			__fLineL = 0.5*__fP;
		}



//calculate step 6 - some additional params
//need __fS, __fP, __fSmallAxis, __fBigAxis
		if( nContourCount )	
		{
			if(bEnableFFEllipse)
			{
				if( ::fabs( __fSmallAxis )<MIN_FLOAT_VAL )
					__fSmallAxis = fCalibr;
				__fFFEllipse = 4.0*__fS/( __fSmallAxis*__fBigAxis*PI );
				__fFFEllipse = min( 1, __fFFEllipse );
				if( !__fFFEllipse )__fFFEllipse = 1;
			}


			if( ::fabs( __fP )<MIN_FLOAT_VAL )
				__fP = fCalibr;


			//true math
			if(bEnableDE)
				__fDE = sqrt( __fS*4/PI );
			
			if(bEnableFFCircle)
			{
				__fFFCircle = 4.0*PI*__fS/(__fP*__fP);
				//some adjusting
				__fFFCircle = min( 1, __fFFCircle );
			}
			
			if(bEnableFFCircle2)
			{
				__fFFCircle2 = sqrt(4.0*PI*__fS/(__fP*__fP));
				//some adjusting
				__fFFCircle2 = min( 1, __fFFCircle2 );
			}

		}

//calculate step 7 - brightness measure
//need pdwHist
		if(bEnableBriAverage||bEnableBriDev||bEnableBriMin||bEnableBriMax||bEnableBriInt||bEnableBriInteg||bEnableBriOrdered)
		{
			int	iStart = -1;
			int	iEnd = -1;
			DWORD	dwHistSize = 0;

			for( int i = 0; i< 256; i++ )
			{
				dwHistSize+=pdwHist[i];
				__fBriAverage+=i*pdwHist[i];

				if( pdwHist[i] )
				{
					if( iStart < 0 )
						iStart = i;
					iEnd = i;
				}
			}
			__fBriInteg = __fBriAverage;
			if( dwHistSize )
				__fBriAverage /= dwHistSize;
			__fBriMin = iStart;
			__fBriMax = iEnd;
			__fBriInt = iEnd-iStart;
			
			
			for( i = 0; i< 256; i++ )
				__fBriDev += (i-__fBriAverage)*(i-__fBriAverage)*pdwHist[i];
			if( dwHistSize-1 )
				__fBriDev/=dwHistSize-1;
			__fBriDev = ::sqrt( __fBriDev );
			int nBriOrder = int(dwHistSize*g_BriOrderRel) + g_BriOrderAbs;
			for( i = 0; i< 256; i++ )
			{
				nBriOrder -= pdwHist[i];
				if(nBriOrder<=0 && pdwHist[i]) break;
			}
			__fBriOrdered = i;
		}

		if(bEnableHue || bEnableSaturation)
		{
			double h=0,s=0,l=0;
			RGB_to_HSL(__fRed/65535.0, __fGreen/65535.0, __fBlue/65535.0, &h, &s, &l);
			__fHue = h;
			__fSaturation = s;
		}

		double __fSkeletonPerimeterRatio = 0;
		if(bEnableSkeletonPerimeterRatio)
		{
			double __fPerSkeleton=0;
			int cy=ymax+2, cx=xmax+2;
			smart_alloc(buf,byte,cx*cy);
			smart_buf.zero();
			smart_alloc(buf2,byte,cx*cy);
			for(int y=ymin; y<ymax; y++)
			{
				byte *pRowMask = imageCalc.GetRowMask(y);
				byte *pRowBuf = buf + (y+1)*cx + 1;
				for(int x=xmin; x<xmax; x++)
				{
					if(pRowMask[x]>=128) pRowBuf[x]=1;
				}
			}
			memcpy(buf2, buf, cx*cy);

			for(int i=0; i<100; i++)
			{ //цикл до конца скелетизации, но не более 100 раз
				if( !skeleton(buf, buf2, cx, cy) ) break;
			}

			double dp[9]={ 0.0,0.7,1.4,  0.7,1.0,0.7,  1.4,0.7,0.0};
			for(int y=1; y<cy; y++)
			{
				byte * p0 = buf + y*cx;
				byte * p1 = buf + (y-1)*cx;
				for(int x=1; x<cx; x++)
				{
					int a = p0[x] + p1[x-1];
					int b = p1[x] + p0[x-1];
					__fPerSkeleton += dp[a*3+b];
				}
			}
			__fPerSkeleton *= fCalibr;
			__fSkeletonPerimeterRatio = __fPerSkeleton / (2* max(__fP, fCalibr/100));
		}

		if(bEnableLC)
		{
			double sumX=0, sumY=0, sumXX=0, sumXY=0, sumYY=0;
			int count=0;
			for(int nCont=0; nCont<nContourCount; nCont++)
			{
				Contour* pCont = imageCalc.GetContour(nCont);
				for(int i = 0; i < pCont->nContourSize; i++ )
				{
					double x = pCont->ppoints[i].x;
					double y = pCont->ppoints[i].y;
					sumX += x;  sumY += y;
					sumXX += x*x;  sumXY += x*y;  sumYY += y*y;
					count++;
				}
			}
			if(count>0)
			{
				double d = (count*sumXX - sumX*sumX)*(count*sumYY - sumY*sumY);
				double d1 = (count*sumXY-sumX*sumY);
                __fLC = d1*d1 / max(d,1e-5);
			}
		}

		if(bEnablePint)// && nContourCount)
			ptrCalc->SetValue(KEY_INT_PERIMETER,	__fPint);
		if(bEnablePext)// && nContourCount)
			ptrCalc->SetValue(KEY_EXT_PERIMETER,	__fP - __fPint);
		if(bEnableP)// && nContourCount)
			ptrCalc->SetValue(KEY_PERIMETER, __fP);
		if(bEnableSint && nContourCount)
			ptrCalc->SetValue(KEY_INT_AREA, __fSInt);
		if(bEnableS)
			ptrCalc->SetValue(KEY_AREA, __fS);
		if(bEnableXCoord)
			ptrCalc->SetValue(KEY_XCOORD, __fXCoord);
		if(bEnableYCoord)
			ptrCalc->SetValue(KEY_YCOORD, __fYCoord);
		if(bEnableRed)
			ptrCalc->SetValue(KEY_RED, __fRed);
		if(bEnableGreen)
			ptrCalc->SetValue(KEY_GREEN, __fGreen);
		if(bEnableBlue)
			ptrCalc->SetValue(KEY_BLUE, __fBlue);
		if(bEnableHue)
			ptrCalc->SetValue(KEY_HUE, __fHue);
		if(bEnableSaturation)
			ptrCalc->SetValue(KEY_SATURATION, __fSaturation);
		if(bEnableBigAxis && nContourCount)
			ptrCalc->SetValue(KEY_ELPSMAX, __fBigAxis);
		if(bEnableSmallAxis && nContourCount)
			ptrCalc->SetValue(KEY_ELPSMIN, __fSmallAxis);
		if(bEnableHeight && nContourCount)
			ptrCalc->SetValue(KEY_LENGTH, __fHeight);
		if(bEnableWidth && nContourCount)
			ptrCalc->SetValue(KEY_WIDTH,	__fWidth);
		if(bEnableXSize && nContourCount)
			ptrCalc->SetValue(KEY_XLEN, __fXSize);
		if(bEnableYSize && nContourCount)
			ptrCalc->SetValue(KEY_YLEN, __fYSize);
		if(bEnableAvrChord && nContourCount)
			ptrCalc->SetValue(KEY_AVRCHORD, __fAvrChord);
		if(bEnableLineL && nContourCount)
			ptrCalc->SetValue(KEY_LINEL,	__fLineL);
		if(bEnableLineW && nContourCount)
			ptrCalc->SetValue(KEY_LINEW,	__fLineW);
		if(bEnableRibbonL && nContourCount)
			ptrCalc->SetValue(KEY_RIBBONL, __fRibbonL);
		if(bEnableRibbonW && nContourCount)
			ptrCalc->SetValue(KEY_RIBBONW, __fRibbonW);
		if(bEnableDE && nContourCount)
			ptrCalc->SetValue(KEY_DECV, __fDE);
		if(bEnableFFCircle)// && nContourCount)
			ptrCalc->SetValue(KEY_CIRCLE, __fFFCircle);
		if(bEnableFFCircle2)// && nContourCount)
			ptrCalc->SetValue(KEY_CIRCLE2, __fFFCircle2);
		if(bEnableFFEllipse && nContourCount)
			ptrCalc->SetValue(KEY_ELLIPSE, __fFFEllipse);
		if(bEnableAngle && nContourCount)
			ptrCalc->SetValue(KEY_ANGLE,	__fAngle);
		if(bEnableBriAverage)
			ptrCalc->SetValue(KEY_BRIAVR, __fBriAverage);
		if(bEnableBriDev)
			ptrCalc->SetValue(KEY_BRIDEV, __fBriDev);
		if(bEnableBriMin)
			ptrCalc->SetValue(KEY_BRIMIN, __fBriMin);
		if(bEnableBriMax)
			ptrCalc->SetValue(KEY_BRIMAX, __fBriMax);
		if(bEnableBriInt)
			ptrCalc->SetValue(KEY_BRIINT, __fBriInt);
		if(bEnableBriInteg)
			ptrCalc->SetValue(KEY_BRIINTEGRAL, __fBriInteg);
		if(bEnableBriOrdered)
			ptrCalc->SetValue(KEY_BRIORDERED, __fBriOrdered);
		if(bEnableDimesion && nContourCount)
			ptrCalc->SetValue(KEY_DIM, __fDimesion);
		if(bEnableNoun && nContourCount)
			ptrCalc->SetValue(KEY_NOUN, __fNoun);
		if(bEnableSkeletonPerimeterRatio)// && nContourCount)
			ptrCalc->SetValue(KEY_SKELETONPERIMETERRATIO, __fSkeletonPerimeterRatio);
		if(bEnableLC && nContourCount)
			ptrCalc->SetValue(KEY_LC, __fLC);
		if(bEnableConcavePercent && nContourCount)
			ptrCalc->SetValue(KEY_CONCAVE_PERCENT, __fConcavePercent);

	if (m_dwPanesMask != 0 && bEnableBriOrdered)
		CalcValuesByPanes(ptrCalc,imageCalc);

	return true;
}// inner area yet not calculate. It will be created some later :-))

void CImageAreaMeasGroup::CalcValuesByPanes(ICalcObject2Ptr	ptrCalc, CImageWrp &imageCalc)
{
	if (ptrCalc == 0)
		return;
	int nPanes = ::GetImagePaneCount(imageCalc);
	for (int iPane = 0; iPane < nPanes; iPane++)
	{
		if ((m_dwPanesMask & (1<<iPane)) == 0)
			continue;
		DWORD adwPaneHist[256];
		memset(adwPaneHist, 0, sizeof(adwPaneHist));
		int xmax = imageCalc.GetWidth();
		int ymax = imageCalc.GetHeight();
		for (int  y = 0; y< ymax; y++)
		{
			byte *pRowMask = imageCalc.GetRowMask(y);
			color* pRow = imageCalc.GetRow(y, iPane);
			for (int x = 0; x< xmax; x++)
			{
				
				if (pRowMask[x] > 1)
				{
					int nBright = ColorAsByte(pRow[x]);
					adwPaneHist[nBright]++;
				}
			}
		}
		DWORD dwHistSize = 0;
		for (int i = 0; i< 256; i++)
			dwHistSize += adwPaneHist[i];
		int nBriOrder = int(dwHistSize*g_BriOrderRel) + g_BriOrderAbs;
		for (i = 0; i< 256; i++ )
		{
			nBriOrder -= adwPaneHist[i];
			if (nBriOrder <= 0 && adwPaneHist[i]) break;
		}
		double fBriOrdered = i;
		ParameterAdditionalInfo add_info;
		add_info.byPane = (byte)iPane;
		add_info.byPhase = 0;
		ptrCalc->SetValueEx(KEY_BRIORDERED, &add_info, fBriOrdered);
	}
}


#pragma optimize("", on)

bool CImageAreaMeasGroup::LoadCreateParam()
{
	DefineParameter(KEY_AREA,			"Extern Area",			"%0.3f");
	DefineParameter(KEY_INT_AREA,		"Inner Area",			"%0.3f");
	DefineParameter(KEY_PERIMETER,		"Full Perimeter",		"%0.3f");
	DefineParameter(KEY_INT_PERIMETER,	"Inner Perimeter",		"%0.3f");
	DefineParameter(KEY_EXT_PERIMETER,	"Extern Perimeter",		"%0.3f");
	DefineParameter(KEY_ELPSMAX,		"Big axis of ellipse",	"%0.3f");
	DefineParameter(KEY_ELPSMIN,		"Small axis of ellipse","%0.3f");
	DefineParameter(KEY_LENGTH,			"Height",				"%0.3f");
	DefineParameter(KEY_WIDTH,			"Width",				"%0.3f");
	DefineParameter(KEY_XLEN,			"Size by x",			"%0.3f");
	DefineParameter(KEY_YLEN,			"Size by y",			"%0.3f");
	DefineParameter(KEY_AVRCHORD,		"Average chord",		"%0.3f");
	DefineParameter(KEY_LINEL,			"Length(line)",			"%0.3f");
	DefineParameter(KEY_LINEW,			"Width(line)",			"%0.3f");
	DefineParameter(KEY_RIBBONL,		"Length(ribbon)",		"%0.3f");
	DefineParameter(KEY_RIBBONW,		"Width(ribbon)",		"%0.3f");
	DefineParameter(KEY_XCOORD,			"Center of object x",	"%0.3f");
	DefineParameter(KEY_YCOORD,			"Center of object y",	"%0.3f");
	DefineParameter(KEY_DECV,			"Equivalent diameter",	"%0.3f");
	DefineParameter(KEY_CIRCLE,			"Form Factor Circle",	"%0.3f");
	DefineParameter(KEY_CIRCLE2,		"Form Factor Circle 2",	"%0.3f");
	DefineParameter(KEY_ELLIPSE,		"Form Factor Ellipse",	"%0.3f");
	DefineParameter(KEY_ANGLE,			"Orientation",			"%0.3f");
	DefineParameter(KEY_BRIAVR,			"Average brightness",	"%0.3f");
	DefineParameter(KEY_BRIDEV,			"Deviation of brightness","%0.3f");
	DefineParameter(KEY_BRIMIN,			"Min brightness",		"%0.3f");
	DefineParameter(KEY_BRIMAX,			"Max brightness",		"%0.3f");
	DefineParameter(KEY_BRIINT,			"Brightness interval",	"%0.3f");
	DefineParameter(KEY_BRIINTEGRAL,	"Brightness integral",	"%0.3f");
	DefineParameter(KEY_BRIORDERED,		"Ordered Brightness",	"%0.3f");
	DefineParameter(KEY_RED,			"Red",					"%0.3f");
	DefineParameter(KEY_GREEN,			"Green",				"%0.3f");
	DefineParameter(KEY_BLUE,			"Blue",					"%0.3f");
	DefineParameter(KEY_HUE,			"Hue",					"%0.3f");
	DefineParameter(KEY_SATURATION,		"Saturation",			"%0.3f");
	DefineParameter(KEY_DIM,			"Dimesion",				"%0.3f");
	DefineParameter(KEY_NOUN,			"Elongation",			"%0.3f");
	DefineParameter(KEY_SKELETONPERIMETERRATIO,			"Skel.P.Ratio",			"%0.3f");
	DefineParameter(KEY_LC,				"Local Correlation",			"%0.3f");
	DefineParameter(KEY_CONCAVE_PERCENT,"Concave Percent",			"%0.3f");

	return true;
}

bool CImageAreaMeasGroup::OnInitCalculation()
{
	CIniValue::Reset(); // Перед пересчетом заново прочитать данные
	return __super::OnInitCalculation();
}

bool CImageAreaMeasGroup::GetUnit( long lKey, long lType, _bstr_t &bstrType, double &fMeterPerUnit )
{
	switch (lKey)
	{
	case KEY_AREA:
	case KEY_INT_AREA:
	{
		CString	strUnitName, str;

		str = GetValueString( GetAppUnknown(), szLinearUnits, szUnitName, szDefLinearUnits );
		fMeterPerUnit = GetValueDouble( GetAppUnknown(), szLinearUnits, szUnitCoeff, 1 );
		strUnitName.Format( "%s*%s", (const char*)str, (const char*)str );

		fMeterPerUnit*=fMeterPerUnit;
		bstrType = strUnitName;
		break;
	}
	case KEY_PERIMETER:
	case KEY_INT_PERIMETER:
	case KEY_EXT_PERIMETER:
	case KEY_ELPSMAX:
	case KEY_ELPSMIN:
	case KEY_LENGTH:
	case KEY_WIDTH:
	case KEY_XLEN:
	case KEY_YLEN:
	case KEY_AVRCHORD:
	case KEY_LINEL:
	case KEY_LINEW:
	case KEY_RIBBONL:
	case KEY_RIBBONW:
	case KEY_XCOORD:
	case KEY_YCOORD:
	case KEY_DECV:
	case KEY_DIM:
	{
		bstrType = GetValueString( GetAppUnknown(), szLinearUnits, szUnitName, szDefLinearUnits );
		fMeterPerUnit = GetValueDouble( GetAppUnknown(), szLinearUnits, szUnitCoeff, 1 );
		break;
	}

	case KEY_CIRCLE:
	case KEY_CIRCLE2:
	case KEY_ELLIPSE:
	case KEY_NOUN:
	case KEY_SKELETONPERIMETERRATIO:
	case KEY_LC:
	case KEY_HUE:
	case KEY_SATURATION:
	case KEY_CONCAVE_PERCENT:
	{
		bstrType = GetValueString( GetAppUnknown(), szRelativeUnits, szUnitName, szDefRelativeUnits );
		fMeterPerUnit = GetValueDouble( GetAppUnknown(), szRelativeUnits, szUnitCoeff, 1 );
		break;
	}

	case KEY_ANGLE:
	{
		bstrType = GetValueString( GetAppUnknown(), szAngleUnits, szUnitName, szDefAngleUnits );
		fMeterPerUnit = GetValueDouble( GetAppUnknown(), szAngleUnits, szUnitCoeff, c_dblDefAngle );
		break;
	}

	case KEY_BRIAVR:
	case KEY_BRIDEV:
	case KEY_BRIMIN:
	case KEY_BRIMAX:
	case KEY_BRIINT:
	case KEY_BRIINTEGRAL:
	case KEY_BRIORDERED:
	{
		bstrType = GetValueString( GetAppUnknown(), szBrightnessUnits, szUnitName, szDefBrightnessUnits );
		fMeterPerUnit = GetValueDouble( GetAppUnknown(), szBrightnessUnits, szUnitCoeff, c_dblDefColor );
		return true;
	}
	case KEY_RED:
	case KEY_GREEN:
	case KEY_BLUE:
	{
		bstrType = GetValueString( GetAppUnknown(), szColorUnits, szUnitName, szDefColorUnits );
		fMeterPerUnit = GetValueDouble( GetAppUnknown(), szColorUnits, szUnitCoeff, c_dblDefColor );
		return true;
	}
	default:
		return false;
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
//IPersist
void CImageAreaMeasGroup::GetClassID( CLSID* pClassID )
{
	memcpy( pClassID, &guid, sizeof(CLSID) );
}
