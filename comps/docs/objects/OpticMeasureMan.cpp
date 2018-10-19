// OpticMeasureMan.cpp : implementation file
//

#include "stdafx.h"
#include "objects.h"
#include "OpticMeasureMan.h"
#include <math.h>
#include <float.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//double COptDensSet::s_dMaxDiff = 0.2;

void COptDensSet::_CalcMedium()
{
	m_fMedium = 0;
	long nCount = (long)m_mapData.GetCount();
	if(nCount)
	{
		POSITION pos = m_mapData.GetStartPosition();
		double fKey = 0, fVal = 0;
		while(pos)
		{
			m_mapData.GetNextAssoc(pos, fKey, fVal);
			m_fMedium += fVal;
		}
		m_fMedium /= nCount;
	}
}

/*bool COptDensSet::CheckSample(double dCalc)
{
	static bool bRead = false;
	if (!bRead)
	{
		bRead = true;
	}
	if (m_aData.GetSize() == 0)
		return true;
	double dPrev = m_aData[0];
	double dDiff = dPrev > dCalc ? dPrev - dCalc : dCalc - dPrev;
	return dDiff <= s_dMaxDiff;
}*/

void CExampleOptDens::AddSample(double dCalc, double dNominal)
{
	COptDensSet* pSet = 0;
	MAPA::iterator it;
	if((it = m_mapCalcOptDens.find(dNominal)) == m_mapCalcOptDens.end())
	{
		pSet = new COptDensSet;
		m_mapCalcOptDens.insert(MAPA::value_type(dNominal, pSet));
	}
	else
	{
		pSet = it->second;
	}
	ASSERT(pSet != 0);

	if(pSet)
		pSet->Add(dCalc);
}


/////////////////////////////////////////////////////////////////////////////
// COpticMeasureMan

IMPLEMENT_DYNCREATE(COpticMeasureMan, CCmdTarget)

COpticMeasureMan::COpticMeasureMan()
{
	EnableAutomation();
	
	_OleLockApp( this );

	m_pimgDarkField = NULL;
	m_pimgBackground = NULL;
	m_nMethod = 0;

	m_nAvrBack = 255;

	m_sName = "OpticMeasure";
}

COpticMeasureMan::~COpticMeasureMan()
{
	_OleUnlockApp( this );
}


void COpticMeasureMan::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(COpticMeasureMan, CCmdTarget)
	//{{AFX_MSG_MAP(COpticMeasureMan)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(COpticMeasureMan, CCmdTarget)
	//{{AFX_DISPATCH_MAP(COpticMeasureMan)
	DISP_PROPERTY_NOTIFY(COpticMeasureMan, "Method", m_nMethod, OnMethodChanged, VT_I2)
	DISP_FUNCTION(COpticMeasureMan, "SetDarkField", SetDarkField, VT_EMPTY, VTS_DISPATCH)
	DISP_FUNCTION(COpticMeasureMan, "SetBackground", SetBackground, VT_EMPTY, VTS_DISPATCH)
	DISP_FUNCTION(COpticMeasureMan, "Free", Free, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(COpticMeasureMan, "AddExample", AddExample, VT_R8, VTS_R8 VTS_DISPATCH)
	DISP_FUNCTION(COpticMeasureMan, "RemoveSampleCalc", RemoveSampleCalc, VT_BOOL, VTS_R8 VTS_R8)
	DISP_FUNCTION(COpticMeasureMan, "CalkAvgBack", CalkAvgBack, VT_I4, VTS_DISPATCH)
	DISP_FUNCTION(COpticMeasureMan, "Interpolate", Interpolate, VT_R8, VTS_R8)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IOpticMeasureMan to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {67C3AAD6-0466-4F36-8997-EA56BF924502}
static const IID IID_IOpticMeasureManDisp =
{ 0x67c3aad6, 0x466, 0x4f36, { 0x89, 0x97, 0xea, 0x56, 0xbf, 0x92, 0x45, 0x2 } };

BEGIN_INTERFACE_MAP(COpticMeasureMan, CCmdTarget)
	INTERFACE_PART(COpticMeasureMan, IID_IOpticMeasureManDisp, Dispatch)
	INTERFACE_PART(COpticMeasureMan, IID_IOpticMeasureMan, OpticMan)
	INTERFACE_PART(COpticMeasureMan, IID_INamedObject2, Name)
END_INTERFACE_MAP()

// {02E20CFD-ACA4-460a-A22A-FE9875E0487F}
GUARD_IMPLEMENT_OLECREATE(COpticMeasureMan, "Objects.OpticMeasureMan", 
0x2e20cfd, 0xaca4, 0x460a, 0xa2, 0x2a, 0xfe, 0x98, 0x75, 0xe0, 0x48, 0x7f);

IMPLEMENT_UNKNOWN(COpticMeasureMan, OpticMan);

/*HRESULT COpticMeasureMan::XOpticMan::DoMeasOptic(long x, long y, long nBrightness, double* pfIntOptDens)
{
	METHOD_PROLOGUE_EX(COpticMeasureMan, OpticMan);

	pThis->DoMeasOptic(*pfIntOptDens, x, y, nBrightness);
	return S_OK;
}*/
HRESULT COpticMeasureMan::XOpticMan::GetMeasOpticStuff(long* pnMethod, BYTE* pnAvgBack, IUnknown** ppunkBack, IUnknown** ppunkDark)
{
	METHOD_PROLOGUE_EX(COpticMeasureMan, OpticMan);
	if(pnMethod)
		*pnMethod = pThis->m_nMethod;
	if(pnAvgBack)
		*pnAvgBack = (BYTE)pThis->m_nAvrBack;
	if(ppunkBack)
	{
		*ppunkBack = (IUnknown*)pThis->m_pimgBackground;
		if(*ppunkBack)
			(*ppunkBack)->AddRef();
	}
	if(ppunkDark)
	{
		*ppunkDark = (IUnknown*)pThis->m_pimgDarkField;
		if(*ppunkDark)
			(*ppunkDark)->AddRef();
	}
	return S_OK;
}

HRESULT COpticMeasureMan::XOpticMan::CalibrateOpticalDensity(double* pfOptDens)
{
	METHOD_PROLOGUE_EX(COpticMeasureMan, OpticMan);
	pThis->CalibrateOpticalDensity(*pfOptDens);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// Dispatch
void COpticMeasureMan::OnMethodChanged() 
{
}

void COpticMeasureMan::SetDarkField(LPDISPATCH pImage) 
{
	// TODO: Add your dispatch handler code here
	m_pimgDarkField = pImage;
}

void COpticMeasureMan::SetBackground(LPDISPATCH pImage) 
{
	// TODO: Add your dispatch handler code here
	m_pimgBackground = pImage;
}

void COpticMeasureMan::Free() 
{
	// TODO: Add your dispatch handler code here
	Deinit();
}


/*double COpticMeasureMan::GetDarkFieldPoint(long x, long y)
{
	double D;
	ASSERT(m_pimgDarkField != 0);
	if (m_pimgDarkField == NULL) return 0;
	if (x < 0 || x >= m_pimgDarkField.GetWidth()) return 0.;
	if (y < 0 || y >= m_pimgDarkField.GetHeight()) return 0.;
	if (m_pimgDarkField.GetColorsCount() == 1)
		D = m_pimgDarkField.GetRow(y, 0)[x];
	else
		D = Bright(m_pimgDarkField.GetRow(y, 2)[x], m_pimgDarkField.GetRow(y, 1)[x], m_pimgDarkField.GetRow(y, 0)[x]);
	return D;
}*/

/*double COpticMeasureMan::GetBackgroundPoint(long x, long y)
{
	double F;
	ASSERT(m_pimgBackground != 0);
	if (m_pimgBackground == NULL) return 255;
	if (x < 0 || x >= m_pimgBackground.GetWidth()) return 0xFFFF;
	if (y < 0 || y >= m_pimgBackground.GetHeight()) return 0xFFFF;
	if (m_pimgBackground.GetColorsCount() == 1)
		F = m_pimgBackground.GetRow(y, 0)[x];
	else
		F = Bright(m_pimgBackground.GetRow(y, 2)[x], m_pimgBackground.GetRow(y, 1)[x], m_pimgBackground.GetRow(y, 0)[x]);
	return F;
}*/

void COpticMeasureMan::Deinit()
{
	m_pimgDarkField = 0;
	m_pimgBackground = 0;

	COptDensSet* pSet = 0;
	MAPA::iterator it;
	for(it = m_ExampleOptDens.m_mapCalcOptDens.begin(); it != m_ExampleOptDens.m_mapCalcOptDens.end(); it++)
	{
		pSet = it->second;
		if(pSet)
			delete pSet;
	}
	m_ExampleOptDens.m_mapCalcOptDens.clear();
}


/*void COpticMeasureMan::DoMeasOptic(double &fIntOptDens, long x, long y, long nBrightness)
{
	if (m_nMethod == 0)
	{
		double  fAvrBack = m_nAvrBack;
		if (nBrightness && fAvrBack/nBrightness > 1)
			fIntOptDens += log10(fAvrBack/nBrightness);
	}
	else if (m_nMethod == 1 || m_nMethod == 2)
	{
		if (m_pimgDarkField != 0 && m_pimgBackground != 0)
		{
			double O,F,D;
			O = (double)nBrightness * 0x100;
			D = GetDarkFieldPoint(x,y);
			F = GetBackgroundPoint(x,y);
			if (O - D > 0)
			{
				double dd = (float)(F-D)/(O-D);
				if (dd > 1)
					fIntOptDens += log10( dd );
			}
		}
	}
}*/

double COpticMeasureMan::LinearInterpolate(double Val)
{
	if (m_pimgDarkField == 0 || m_pimgBackground == 0) return 0.;
	// Linear interpolation/extrapolation using La Grange polynome
	double px[2] = {0., 0.}; // Calculated optical density
	double py[2] = {0., 0.}; // Nominal optical density

	COptDensSet* pSet = 0;
	MAPA::iterator it;
	long nCounter = 0;
	for(it = m_ExampleOptDens.m_mapCalcOptDens.begin(); it != m_ExampleOptDens.m_mapCalcOptDens.end(); it++)
	{
		py[nCounter] = it->first;
		pSet = it->second;
		if(pSet)
			px[nCounter] = pSet->GetMedium();
			
		nCounter++;
	}
	
  double fDivider = (px[0]-px[1]);
  if(fDivider != 0)
  {
	  double a1 = (py[0]-py[1])/fDivider;
	  double a0 = (py[1]*px[0]-py[0]*px[1])/fDivider;
	  return a1*Val+a0;
  }

  return 0.0;
}

double COpticMeasureMan::SquareInterpolate(double Val)
{
	if (m_pimgDarkField == 0 || m_pimgBackground == 0) return 0.;
	long nSize = m_ExampleOptDens.GetSize();
	// Square interpolation/extrapolation using La Grange polynome
	double px[3] = {0., 0., 0.};
	double py[3] = {0., 0., 0.};

	COptDensSet* pSet = 0;
	MAPA::iterator it;
	long nCounter = 0;
	// Find 3 points for the interpolation polynome
	if (nSize == 3)
	{
		// Thre is only 3 examples - using it for the linear interpolation/extrapolation
		for(it = m_ExampleOptDens.m_mapCalcOptDens.begin(); it != m_ExampleOptDens.m_mapCalcOptDens.end(); it++)
		{
			py[nCounter] = it->first;
			pSet = it->second;
			if(pSet)
				px[nCounter] = pSet->GetMedium();
				
			nCounter++;
		}		
	}
	else
	{
		double xmin = DBL_MAX;
		double xmax = 0;
		double x = 0;
		for(it = m_ExampleOptDens.m_mapCalcOptDens.begin(); it != m_ExampleOptDens.m_mapCalcOptDens.end(); it++)
		{
			pSet = it->second;
			if(pSet)
			{
				x = pSet->GetMedium();
				if (x < xmin) xmin = x;
				if (x > xmax) xmax = x;
			}
		}
		
		long i1,i2,i3;
		if (Val >= xmin && Val <= xmax)
		{
			// Interpolation only in this case
			double d;
			bool bFirst = true;
			nCounter = 0;
			for(it = m_ExampleOptDens.m_mapCalcOptDens.begin(); it != m_ExampleOptDens.m_mapCalcOptDens.end(); it++)
			{
				pSet = it->second;
				if(pSet)
				{
					x = pSet->GetMedium();
					if (x == Val)
						return it->first;
					else if (x < Val)
					{
						if (bFirst || Val - x < d)
						{
							bFirst = false;
							i1 = nCounter;
							d = Val - x;
						}
					}
				}
				nCounter++;
			}
			ASSERT(!bFirst);
			bFirst = true;

			nCounter = 0;
			for(it = m_ExampleOptDens.m_mapCalcOptDens.begin(); it != m_ExampleOptDens.m_mapCalcOptDens.end(); it++)
			{
				pSet = it->second;
				if(pSet)
				{
					x = pSet->GetMedium();
					if (x > Val)
					{
						if (bFirst || x - Val < d)
						{
							bFirst = false;
							i2 = nCounter;
							d = x - Val;
						}
					}
				}
				nCounter++;
			}
			ASSERT(!bFirst);
			bFirst = true;


			nCounter = 0;
			for(it = m_ExampleOptDens.m_mapCalcOptDens.begin(); it != m_ExampleOptDens.m_mapCalcOptDens.end(); it++)
			{
				pSet = it->second;
				if(pSet)
				{
					double x = pSet->GetMedium();
					double dx = x > Val ? x - Val : Val - x;
					if ((bFirst || dx < d) && nCounter != i1 && nCounter != i2)
					{
						bFirst = false;
						i3 = nCounter;
						d = dx;
					}
				}
				nCounter++;
			}
			ASSERT(!bFirst);
		}
		else if (Val < xmin)
		{
			// Find three minimal values
//			long i1,i2,i3;
			double d;
			bool bFirst = true;
			nCounter = 0;
			for(it = m_ExampleOptDens.m_mapCalcOptDens.begin(); it != m_ExampleOptDens.m_mapCalcOptDens.end(); it++)
			{
				pSet = it->second;
				if(pSet)
				{
					double x = pSet->GetMedium();
					if (bFirst || x < d)
					{
						bFirst = false;
						d = x;
						i1 = nCounter;
					}
				}
				nCounter++;
			}
			ASSERT(!bFirst);
			bFirst = true;

			nCounter = 0;
			for(it = m_ExampleOptDens.m_mapCalcOptDens.begin(); it != m_ExampleOptDens.m_mapCalcOptDens.end(); it++)
			{
				if (nCounter == i1) 
				{
					nCounter++;
					continue;
				}
				pSet = it->second;
				if(pSet)
				{
					double x = pSet->GetMedium();
					if (bFirst || x < d)
					{
						bFirst = false;
						d = x;
						i2 = nCounter;
					}
				}
				nCounter++;
			}
			ASSERT(!bFirst);
			bFirst = true;

			nCounter = 0;
			for(it = m_ExampleOptDens.m_mapCalcOptDens.begin(); it != m_ExampleOptDens.m_mapCalcOptDens.end(); it++)
			{
				if (nCounter == i1 || nCounter == i2) 
				{
					nCounter++;
					continue;
				}
				pSet = it->second;
				if(pSet)
				{
					double x = pSet->GetMedium();
					if (bFirst || x < d)
					{
						bFirst = false;
						d = x;
						i3 = nCounter;
					}
				}
				nCounter++;
			}
			ASSERT(!bFirst);
		}
		else // if (Val > xmax)
		{
			// Find three maximal values
//			long i1,i2,i3;
			double d;
			bool bFirst = true;
			nCounter = 0;
			for(it = m_ExampleOptDens.m_mapCalcOptDens.begin(); it != m_ExampleOptDens.m_mapCalcOptDens.end(); it++)
			{
				pSet = it->second;
				if(pSet)
				{
					double x = pSet->GetMedium();
					if (bFirst || x > d)
					{
						bFirst = false;
						d = x;
						i3 = nCounter;
					}
				}
				nCounter++;
			}
			ASSERT(!bFirst);
			bFirst = true;
			nCounter = 0;
			for(it = m_ExampleOptDens.m_mapCalcOptDens.begin(); it != m_ExampleOptDens.m_mapCalcOptDens.end(); it++)
			{
				if (nCounter == i3) continue;
				pSet = it->second;
				if(pSet)
				{
					double x = pSet->GetMedium();
					if (bFirst || x > d)
					{
						bFirst = false;
						d = x;
						i2 = nCounter;
					}
				}
				nCounter++;
			}
			ASSERT(!bFirst);
			bFirst = true;
			nCounter = 0;
			for(it = m_ExampleOptDens.m_mapCalcOptDens.begin(); it != m_ExampleOptDens.m_mapCalcOptDens.end(); it++)
			{
				if (nCounter == i3 || nCounter == i2) continue;
				pSet = it->second;
				if(pSet)
				{
					double x = pSet->GetMedium();
					if (bFirst || x > d)
					{
						bFirst = false;
						d = x;
						i1 = nCounter;
					}
				}
				nCounter++;
			}
			ASSERT(!bFirst);
		}

		nCounter = 0;
		for(it = m_ExampleOptDens.m_mapCalcOptDens.begin(); it != m_ExampleOptDens.m_mapCalcOptDens.end(); it++)
		{
			pSet = it->second;
			if(pSet)
			{
				if(nCounter == i1)
				{
					px[0] = pSet->GetMedium();
					py[0] = it->first;
				}
				else if(nCounter == i2)
				{
					px[1] = pSet->GetMedium();
					py[1] = it->first;
				}
				else if(nCounter == i3)
				{
					px[2] = pSet->GetMedium();
					py[2] = it->first;
				}
			}
			nCounter++;
		}
	}
	// Find the coefficients of the interpolation polynome.
	double g = (px[0]-px[1])*(px[1]-px[2])*(px[0]-px[2]);
	if(g != 0)
	{
		double a2 = (py[0]*(px[1]-px[2])-py[1]*(px[0]-px[2])+py[2]*(px[0]-px[1]))/g;
		double a1 = (-py[0]*(px[1]*px[1]-px[2]*px[2])+py[1]*(px[0]*px[0]-px[2]*px[2])-py[2]*(px[0]*px[0]-px[1]*px[1]))/g;
		double a0 = (py[0]*px[1]*px[2]*(px[1]-px[2])-py[1]*px[0]*px[2]*(px[0]-px[2])+py[2]*px[0]*px[1]*(px[0]-px[1]))/g;
		// Find the output value.
		return a2*Val*Val+a1*Val+a0;
	}
	return 0.;
}

void COpticMeasureMan::CalibrateOpticalDensity(double &fOptDens)
{
	if (m_nMethod == 2)
	{
		if (m_ExampleOptDens.GetSize() == 2)
		{
			fOptDens = LinearInterpolate(fOptDens);
		}
		else if (m_ExampleOptDens.GetSize() > 2)
		{
			fOptDens = SquareInterpolate(fOptDens);
		}
	}
}


double COpticMeasureMan::AddExample(double fDensity, LPDISPATCH dispImage) 
{
	if(m_pimgBackground == 0 || m_pimgDarkField == 0)
		return 0.;
	double O = 0., F = 0., D = 0.;
	double dOptDens = 0;
	CImageWrp img(dispImage);
	long nColors = img.GetColorsCount();
	color** pRowsBack = new color*[m_pimgBackground.GetColorsCount()];
	color** pRowsDark = new color*[m_pimgDarkField.GetColorsCount()];
	long i = 0;
	for (long y = 0; y < img.GetHeight(); y++)
	{
		color *pDataR = img.GetRow(y, 0);
		color *pDataG = img.GetRow(y, 1);
		color *pDataB = img.GetRow(y, 2);

		bool bBackRows = false;
		bool bDarkRows = false;
		if(y < m_pimgBackground.GetHeight())
		{
			for(i = 0; i < m_pimgBackground.GetColorsCount(); i++)
				pRowsBack[i] = m_pimgBackground.GetRow(y, i);
			bBackRows = true;
		}
		if(y < m_pimgDarkField.GetHeight())
		{
			for(i = 0; i < m_pimgDarkField.GetColorsCount(); i++)
				pRowsDark[i] = m_pimgDarkField.GetRow(y, i);
			bDarkRows = true;
		}

		for (long x = 0; x < img.GetWidth(); x++)
		{
			if(nColors == 1)
				O = pDataR[x];
			else
				O = Bright(pDataB[x],pDataG[x],pDataR[x]);

			if(bDarkRows && x < m_pimgDarkField.GetWidth())
			{
				if (m_pimgDarkField.GetColorsCount() == 1)
					D = pRowsDark[0][x];
				else
					D = Bright(pRowsDark[2][x], pRowsDark[1][x], pRowsDark[0][x]);
			}

			if(bBackRows && x < m_pimgBackground.GetWidth())
			{
				if (m_pimgBackground.GetColorsCount() == 1)
					F = pRowsBack[0][x];
				else
					F = Bright(pRowsBack[2][x], pRowsBack[1][x], pRowsBack[0][x]);
			}


			//D = GetDarkFieldPoint(x,y);
			//F = GetBackgroundPoint(x,y);
			if (O - D > 0)
			{
				double dd = (float)(F-D)/(O-D);
				if (dd > 1)
					dOptDens += log10( dd );
			}
		}
	}

	delete pRowsBack;
	delete pRowsDark;

	long iS = img.GetWidth()*img.GetHeight();
	double dVal = dOptDens/iS;

	m_ExampleOptDens.AddSample(dVal,fDensity);
	return dVal;
}

bool COpticMeasureMan::RemoveSampleCalc(double dNominal, double dCalcToRemove) 
{
	// TODO: Add your dispatch handler code here
	COptDensSet* pSet = 0;

	MAPA::iterator it;
	if((it = m_ExampleOptDens.m_mapCalcOptDens.find(dNominal)) != m_ExampleOptDens.m_mapCalcOptDens.end())
	{
		pSet = it->second;
		ASSERT(pSet != 0);
		if(pSet)
		{
			if(!pSet->RemoveVal(dCalcToRemove))
			{
				delete pSet;
				m_ExampleOptDens.m_mapCalcOptDens.erase(it);
				return true;
			}
		}
	}
	return false;
}

long COpticMeasureMan::CalkAvgBack(LPDISPATCH dispImage) 
{
	// TODO: Add your dispatch handler code here
	CImageWrp img(dispImage);
	if(img == 0)
		return 0;

	double r = 0, g = 0, b = 0;
	long nCX = img.GetWidth();
	long nCY = img.GetHeight();

	long nPixels = 0;
	for(long y = 0; y < nCY; y++)
	{
		color* pRowR = img.GetRow(y, 0);
		color* pRowG = img.GetRow(y, 1);
		color* pRowB = img.GetRow(y, 2);
		byte* pMask = img.GetRowMask(y);
		for(long x = 0; x < nCX; x++)
		{
			if(pMask[x] != 0)
			{
				r += pRowR[x];
				g += pRowG[x];
				b += pRowB[x];

				nPixels++;
			}
		}
	}

	if(img.GetColorsCount() == 1)
		m_nAvrBack = ColorAsByte((color)(r/nPixels));
	else
	{
		byte nR = ColorAsByte((color)(r/nPixels));
		byte nG = ColorAsByte((color)(g/nPixels));
		byte nB = ColorAsByte((color)(b/nPixels));
		m_nAvrBack = (byte)Bright(nB, nG, nR);
	}
	return m_nAvrBack;
}

double COpticMeasureMan::Interpolate(double fVal) 
{
	// TODO: Add your dispatch handler code here
	if(m_ExampleOptDens.GetSize() == 2)
	{
		fVal = LinearInterpolate(fVal);
	}
	else if(m_ExampleOptDens.GetSize() > 2)
	{
		fVal = SquareInterpolate(fVal);
	}
	return fVal;
}
