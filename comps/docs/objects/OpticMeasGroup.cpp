// OpticMeasGroup.cpp : implementation file
//

#include "stdafx.h"
#include "objects.h"
#include "OpticMeasGroup.h"
#include "Params.h"
#include "math.h"
#include "units_const.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COpticMeasGroup

IMPLEMENT_DYNCREATE(COpticMeasGroup, CCmdTarget)

COpticMeasGroup::COpticMeasGroup()
{
	_OleLockApp( this );

	m_sName = "Optic";
}

COpticMeasGroup::~COpticMeasGroup()
{
	_OleUnlockApp( this );
}
// {7503EF25-5BA2-4667-9928-37B85BD7EACF}
IMPLEMENT_GROUP_OLECREATE(COpticMeasGroup, "Objects.OpticMeasGroup", 0x7503ef25, 0x5ba2, 0x4667, 0x99, 0x28, 0x37, 0xb8, 0x5b, 0xd7, 0xea, 0xcf);

/////////////////////////////////////////////////////////////////////////////
// COpticMeasGroup message handlers


bool COpticMeasGroup::CalcValues( IUnknown *punkCalcObject, IUnknown *punkSource )
{
	{
		if(m_sptrOpticMan == 0)
		{
			IUnknown* punkOM = _GetOtherComponent(GetAppUnknown(), IID_IOpticMeasureMan);
			m_sptrOpticMan = punkOM;
			if(punkOM)
				punkOM->Release();

			if(m_sptrOpticMan == 0)
				return false;
		}

		if (!::CheckInterface(punkCalcObject, IID_ICalcObject))
			return false;

		CImageWrp ImgWrp = punkSource;
		ICalcObjectPtr		sptrCO = punkCalcObject;
		
		if (ImgWrp.IsEmpty())
		{
//			sptrCO->ClearValues();
			return true;
		}

		//parameters
		double	__fOptDens = 0,		//Optic density
				__fIntOptDens = 0;	//Integral optic density


		bool bOptDens = ParamIsEnabledByKey(KEY_OPTICAL_DENS);
		bool bIntOptDens = ParamIsEnabledByKey(KEY_INT_OD);

				
		int xmax,ymax;
		int xmin,ymin;
		int		iNumOfLines = 0;
		long nColors = ImgWrp.GetColorsCount();
		byte	*pRowMask;
		long iNum = 0;
		long bright = 0;
	
		CRect rcMeas = ImgWrp.GetRect();

		xmin = 0;
		xmax = ImgWrp.GetWidth();
		ymin = 0;
		ymax = ImgWrp.GetHeight();

		/*xmin -= rcMeas.left;
		xmax -= rcMeas.left;
		ymin -= rcMeas.top;
		ymax -= rcMeas.top;*/

		long nMethod = 0;
		BYTE nAvgBack = 0;
		IUnknown* punkBack = 0;
		IUnknown* punkDark = 0;
		m_sptrOpticMan->GetMeasOpticStuff(&nMethod, &nAvgBack, &punkBack, &punkDark);
		CImageWrp imgBack(punkBack);
		if(punkBack)
			punkBack->Release();
		CImageWrp imgDark(punkDark);
		if(punkDark)
			punkDark->Release();

		long i = 0;
		bool bWorkWithBackAndDark = ((nMethod == 1 || nMethod == 2) && (imgDark != 0) && (imgBack != 0));
		color** pRowsBack = 0;
		color** pRowsDark = 0;
		if(bWorkWithBackAndDark)
		{
			pRowsBack = new color*[imgBack.GetColorsCount()];
			pRowsDark = new color*[imgDark.GetColorsCount()];
		}

		for( int  y = ymin; y< ymax; y++ )
		{
			pRowMask = ImgWrp.GetRowMask(y);
			color* pRowR = ImgWrp.GetRow(y, 0);
			color* pRowG = ImgWrp.GetRow(y, 1);
			color* pRowB = ImgWrp.GetRow(y, 2);
			
			bool bBackRows = false;
			bool bDarkRows = false;
			if(bWorkWithBackAndDark)
			{
				if(rcMeas.top + y < imgBack.GetHeight())
				{
					for(i = 0; i < imgBack.GetColorsCount(); i++)
						pRowsBack[i] = imgBack.GetRow(rcMeas.top + y, i);
					bBackRows = true;
				}
				if(rcMeas.top + y < imgDark.GetHeight())
				{
					for(i = 0; i < imgDark.GetColorsCount(); i++)
						pRowsDark[i] = imgDark.GetRow(rcMeas.top + y, i);
					bDarkRows = true;
				}
			}
			
			for( int  x = xmin; x< xmax; x++ )
			{
				if(pRowMask[x] > 1)
				{
					iNum++;
					long xc = x;
					if(nColors > 1)//true color
						bright = Bright(ColorAsByte(pRowB[xc]), ColorAsByte(pRowG[xc]), ColorAsByte(pRowR[xc]));
					else
						bright = ColorAsByte(pRowR[xc]);
					
					xc = rcMeas.left + x;
					//m_sptrOpticMan->DoMeasOptic(x, y, bright, &__fIntOptDens);
					if(nMethod == 0)
					{
						double  fAvrBack = nAvgBack;
						if(bright && fAvrBack/bright > 1)
							__fIntOptDens += log10(fAvrBack/bright);
					}
					else if(bWorkWithBackAndDark)
					{
						double O = 0.,F = 0., D = 0.;
						O = (double)bright * 0x100;
						if(bDarkRows && xc < imgDark.GetWidth())
						{
							if (imgDark.GetColorsCount() == 1)
								D = pRowsDark[0][xc];
							else
								D = Bright(pRowsDark[2][xc], pRowsDark[1][xc], pRowsDark[0][xc]);
						}

						if(bBackRows && xc < imgBack.GetWidth())
						{
							if (imgBack.GetColorsCount() == 1)
								F = pRowsBack[0][xc];
							else
								F = Bright(pRowsBack[2][xc], pRowsBack[1][xc], pRowsBack[0][xc]);
						}
						if (O - D > 0)
						{
							double dd = (float)(F-D)/(O-D);
							if (dd > 1)
								__fIntOptDens += log10( dd );
						}
					}
					continue;
				}
			}
		}

		if(bWorkWithBackAndDark)
		{
			delete pRowsBack;
			delete pRowsDark;
		}
		
		if( !iNum )
			iNum = 1;
		__fOptDens = __fIntOptDens/iNum;

		m_sptrOpticMan->CalibrateOpticalDensity(&__fOptDens);
		__fIntOptDens = __fOptDens*iNum;
		
				
		if(bOptDens)
			sptrCO->SetValue(KEY_OPTICAL_DENS, __fOptDens);
		if(bIntOptDens)
			sptrCO->SetValue(KEY_INT_OD, __fIntOptDens);
		
	}
	return true;
}

bool COpticMeasGroup::LoadCreateParam()
{
	DefineParameter(KEY_OPTICAL_DENS,		"Optic density",			"%0.3f");
	DefineParameter(KEY_INT_OD,				"Integral optic density",	"%0.3f");
	return true;
}

bool COpticMeasGroup::GetUnit( long lKey, long lType, _bstr_t &bstrType, double &fMeterPerUnit )
{
	if( lKey == KEY_OPTICAL_DENS )
	{
		bstrType = GetValueString( GetAppUnknown(), szOpticalUnits, szUnitName, szDefOpticalUnits );
		fMeterPerUnit = GetValueDouble( GetAppUnknown(), szOpticalUnits, szUnitCoeff, 1 );
		return true;
	}
	if( lKey == KEY_INT_OD )
	{
		bstrType = GetValueString( GetAppUnknown(), szOpticalUnits, szUnitName, szDefOpticalUnits );
		fMeterPerUnit = GetValueDouble( GetAppUnknown(), szOpticalUnits, szUnitCoeff, 1 );
		return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
//IPersist
void COpticMeasGroup::GetClassID( CLSID* pClassID )
{
	memcpy( pClassID, &guid, sizeof(CLSID) );
}
