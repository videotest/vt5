// FereMeasGroup.cpp : implementation file
//

#include "stdafx.h"
#include "objects.h"
#include "FereMeasGroup.h"
#include "Params.h"
#include "units_const.h"

#include <math.h>

/////////////////////////////////////////////////////////////////////////////
// CFereMeasGroup

IMPLEMENT_DYNCREATE(CFereMeasGroup, CMeasParamGroupBase)

CFereMeasGroup::CFereMeasGroup()
{
	_OleLockApp( this );

	m_sName = "Fere";
}

CFereMeasGroup::~CFereMeasGroup()
{
	_OleUnlockApp( this );
}
// {36BE1112-FF82-49e2-AC4D-3FDD93832418}
IMPLEMENT_GROUP_OLECREATE(CFereMeasGroup, "Objects.FereMeasGroup", 0x36be1112, 0xff82, 0x49e2, 0xac, 0x4d, 0x3f, 0xdd, 0x93, 0x83, 0x24, 0x18);

/////////////////////////////////////////////////////////////////////////////
// CFereMeasGroup message handlers

static double sqr(double x)
{ return x*x; }

bool CFereMeasGroup::CalcValues( IUnknown *punkCalcObject, IUnknown *punkSource )
{
//[AY}begin image measurement prologue
	CImageWrp imageCalc = punkSource;
	double	fCalibr = 0.0025;
	ICalcObjectPtr	ptrCalc = punkCalcObject;
	ICalibrPtr	ptrCalibr = imageCalc;
	if( ptrCalibr != 0 )ptrCalibr->GetCalibration( &fCalibr, 0 );
	int nContourCount = imageCalc.GetContoursCount();

	if (imageCalc.IsEmpty())
	{
//		ptrCalc->ClearValues();
		return true;
	}
//end 
		//parameters
		double	__fFMin = 0,		//Minimal diameter Fere
				__fFMax = 0,		//Maximal ...
				__fFAver = 0,		//Average ...
				__fRoundish = 0,	//Roundish
				__fCompactness = 0;	//Компактность



		bool bEnableRoundish = ParamIsEnabledByKey(KEY_ROUNDISH);
		bool bEnableFereMin = ParamIsEnabledByKey(KEY_FEREMIN);
		bool bEnableFereMax = ParamIsEnabledByKey(KEY_FEREMAX);
		bool bEnableFereAvg = ParamIsEnabledByKey(KEY_FEREAVRG);
		bool bEnableCompactness = ParamIsEnabledByKey(KEY_COMPACTNESS);

		double fS = 0;

		if(bEnableFereMin || bEnableFereMax || bEnableFereAvg || bEnableRoundish || bEnableCompactness)
		{
		
//calculate step 1: measure area and calc number of h-lines
			int xmax,ymax;
			int xmin,ymin;
			byte	*pRowMask;

			double	fCalibrXY = fCalibr*fCalibr;
		
			CRect rcMeas = NORECT;//imageCalc.GetRect();

			xmin = 0;
			xmax = imageCalc.GetWidth();
			ymin = 0;
			ymax = imageCalc.GetHeight();

			/*xmin -= rcMeas.left;
			xmax -= rcMeas.left;
			ymin -= rcMeas.top;
			ymax -= rcMeas.top;*/

			for( int  y = ymin; y< ymax; y++ )
			{
				pRowMask = imageCalc.GetRowMask(y);

				for( int  x = xmin; x< xmax; x++ )
				{
					
					if(pRowMask[x] > 1)
						fS+=fCalibrXY;
				}
			}
			

//calculate step 2 - measure Diameter Fere
			if(nContourCount && imageCalc.GetContour(0)->nContourSize != 0 )
			{
				const int iNumOfFDiameters=6;
				double	fCosX[50], 
						fCosY[50],
						fSinX[50],
						fSinY[50],
						fXMin[50],
						fXMax[50],
						fYMin[50],
						fYMax[50];
				Contour* pContour0 = imageCalc.GetContour(0);
				ContourPoint	pt;
				double	fX, fY, fAngle;
				int	i, j;
				//initialize array of diameters
				pt = pContour0->ppoints[0];
				for(i = 0; i<iNumOfFDiameters; i++ )
				{
					fAngle  = PI/iNumOfFDiameters*i;
					fCosX[i] = ::cos( fAngle )*fCalibr;
					fCosY[i] = ::cos( fAngle )*fCalibr;
					fSinX[i] = ::sin( fAngle )*fCalibr;
					fSinY[i] = ::sin( fAngle )*fCalibr;

					fXMin[i] = fXMax[i] = pt.x*fCosX[i] + pt.y*fSinY[i];
					fYMin[i] = fYMax[i] = pt.y*fCosY[i] - pt.x*fSinX[i];
				}
						//main loop - calc diameter
				for(i = 0; i < pContour0->nContourSize; i++ )
				{
					pt = pContour0->ppoints[i];

					for( j = 0; j<iNumOfFDiameters; j++ )
					{
						fX = pt.x*fCosX[j] + pt.y*fSinY[j];
						fY = pt.y*fCosY[j] - pt.x*fSinX[j];

						fXMin[j] = min( fX, fXMin[j] );
						fXMax[j] = max( fX, fXMax[j] );
						fYMin[j] = min( fY, fYMin[j] );
						fYMax[j] = max( fY, fYMax[j] );
					}
				}
						//process measure
				double	fDx, fDy;
				__fFMin = __fFMax = fXMax[0] - fXMin[0];
						
				for(j = 0; j<iNumOfFDiameters; j++ )
				{
					fDx = fXMax[j] - fXMin[j];
					fDy = fYMax[j] - fYMin[j];

					__fFMin = min( __fFMin, fDy );
					__fFMin = min( __fFMin, fDx );
					__fFMax = max( __fFMax, fDy );
					__fFMax = max( __fFMax, fDx );

					__fFAver+=fDx;
					__fFAver+=fDy;
				}
				__fFAver/=2*iNumOfFDiameters;
			}

			if( ::fabs( __fFMax )<MIN_FLOAT_VAL )
				__fFMax = fCalibr;

			__fCompactness = 4 * fS / (PI * sqr( max(__fFAver,1e-30)) );
		}

		if(bEnableRoundish)
		{
			__fRoundish = 4.0*fS/( PI*__fFMax*__fFMax );
			__fRoundish = min( 1, __fRoundish );
		}


		if(bEnableFereMin && nContourCount)
			ptrCalc->SetValue(KEY_FEREMIN, __fFMin);
		if(bEnableFereMax && nContourCount)
			ptrCalc->SetValue(KEY_FEREMAX, __fFMax);
		if(bEnableFereAvg && nContourCount)
			ptrCalc->SetValue(KEY_FEREAVRG, __fFAver);
		if(bEnableRoundish && nContourCount)
			ptrCalc->SetValue(KEY_ROUNDISH, __fRoundish);
		if(bEnableCompactness && nContourCount)
			ptrCalc->SetValue(KEY_COMPACTNESS, __fCompactness);

	return true;
}

bool CFereMeasGroup::LoadCreateParam()
{
	DefineParameter(KEY_FEREMIN,		"Minimal diameter Fere",	"%0.3f");
	DefineParameter(KEY_FEREMAX,		"Maximal diameter Fere",	"%0.3f");
	DefineParameter(KEY_FEREAVRG,		"Average diameter Fere",	"%0.3f");
	DefineParameter(KEY_ROUNDISH,		"Roundness",				"%0.3f");
	DefineParameter(KEY_COMPACTNESS,	"Compactness",				"%0.3f");

	return true;
}

bool CFereMeasGroup::GetUnit( long lKey, long lType, _bstr_t &bstrType, double &fMeterPerUnit )
{
	if( lKey == KEY_ROUNDISH || lKey == KEY_COMPACTNESS )
	{
		bstrType = GetValueString( GetAppUnknown(), szRelativeUnits, szUnitName, szDefRelativeUnits );
		fMeterPerUnit = GetValueDouble( GetAppUnknown(), szRelativeUnits, szUnitCoeff, 1 );
	}
	else if( lKey == KEY_FEREMIN || lKey == KEY_FEREMAX || lKey == KEY_FEREAVRG )
	{
		bstrType = GetValueString( GetAppUnknown(), szLinearUnits, szUnitName, szDefLinearUnits );
		fMeterPerUnit = GetValueDouble( GetAppUnknown(), szLinearUnits, szUnitCoeff, 1 );
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////
//IPersist
void CFereMeasGroup::GetClassID( CLSID* pClassID )
{
	memcpy( pClassID, &guid, sizeof(CLSID) );
}
