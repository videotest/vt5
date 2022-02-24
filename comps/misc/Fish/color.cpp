// color.cpp: implementation of the color class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Fish.h"
#include "color.h"
#include "misc_utils.h"
#include "fish_int.h"
#include "fishcontextviewer.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "ccmacro.h"
#include <math.h>


int nInterpolationMethod=-1; //метод интерполяции при увеличении
// 0 - по ближайшим соседям, 1 - билинейная
// -1 - прочитать значение из shell.data
int nMaskInterpolationMethod=-1; //метод интерполяции масок при увеличении
// 0 - квадратами, 1 - под углами 90 и 45 градусов


CIntIniValue g_bDefColors(FISH_ROOT, _T("DefaultCombiColors"), 0);
CColorIniValue g_clrCombi(FISH_ROOT, _T("UnknownCombi"), RGB(0,0,0));

#define ScaleDoubleToColor(clr) ((color)max( 0, min( 65535,(((clr)+0.5)*65535) ) ))
#define ScaleDoubleToByte(clr)  ((byte)max(0, min(255,(((clr)+0.5)*255))))

#define ScaleColorToDouble(clr) ((double)(((clr)/65535.0)-0.5))
#define ScaleByteToDouble(clr) ((double)(((clr)/255.0)-0.5))

CFishProps::CFishProps(IUnknown *punkImage, IUnknown *punkParams)
{
	m_sptrBag1 = punkParams;
	m_sptrBag2 = punkImage;
	IUnknownPtr ptr3(GetActualImageFromView(punkParams), false);
	m_sptrBag3 = ptr3;
	{
		long lIgnoreViewSettings=0;
		::GetLongProp(&lIgnoreViewSettings, "IgnoreViewSettings", m_sptrBag2); // тут нам важно только мнение image
		if(lIgnoreViewSettings)
		{
			m_sptrBag1 = punkImage;
			m_sptrBag2 = punkParams;
		}
	}
}

CFishProps::~CFishProps()
{
}


void CFishData::Init(CColorConvertorFISH *pCnv, IUnknown *punkImage, IUnknown *punkParams, int nPane)
{
	CFishProps cf(punkImage, punkParams);

	nPaneCount = ::GetImagePaneCount( punkImage );
	if (nPaneCount>32) nPaneCount=32; // не более 32 пан
	iShownPane = -1; // composed by default
	uPanesUsed = ((1<<nPaneCount)-1) & (~3); // по умолчанию все, кроме 0 и 1 паны - ratio и DAPI

	cf.GetLongProp(&iShownPane, FISH_SHOWN_PANE);
	cf.GetLongProp(&uPanesUsed, FISH_USED_PANES);

	if (nPane != INT_MAX) iShownPane = nPane;
	
	if (iShownPane >= 0) uPanesUsed = 1<<iShownPane;

	char szName[128];
	for(int i=0; i<nPaneCount; i++)
	{
        arrThresholds[i]=32768;
		sprintf(szName, "FishThreshold%u", i);
		cf.GetLongProp(&arrThresholds[i], szName);
	}

	nCombos=(1<<nPaneCount);
	cf.GetLongProp(&nCombos, "FishCombos");

	arrComboIdx.alloc(nCombos);
	arrComboR.alloc(nCombos);
	arrComboG.alloc(nCombos);
	arrComboB.alloc(nCombos);
	for(int i=0; i<nCombos; i++)
	{
        arrComboIdx[i] = i%(1<<nPaneCount);
        arrComboIdx[i] &= uPanesUsed;
		COLORREF clrDefault = pCnv->DefCombiColor(arrComboIdx[i],nPaneCount);
        arrComboR[i] = GetRValue(clrDefault);
        arrComboG[i] = GetGValue(clrDefault);
        arrComboB[i] = GetBValue(clrDefault);

		sprintf(szName, "FishComboIdx%u", i);
		cf.GetLongProp(&arrComboIdx[i], szName);

		sprintf(szName, "FishComboColor%u", i);
		COLORREF cr = -1;
		if( cf.GetLongProp(&cr, szName) )
		{
			arrComboR[i] = GetRValue(cr);
			arrComboG[i] = GetGValue(cr);
			arrComboB[i] = GetBValue(cr);
		}
	}
	bInvert = false;
	if(iShownPane>=0)
	{
		_variant_t	var;
		sprintf(szName, "FishInvert%d", iShownPane);
		cf.GetLongProp(&bInvert, szName);
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CColorConvertorFISH, CCmdTargetEx);


// {205A7A46-6374-45ca-94C2-E0D5C4F2F80E}
GUARD_IMPLEMENT_OLECREATE(CColorConvertorFISH, "FISH.ColorConvertorFISH",
0x205a7a46, 0x6374, 0x45ca, 0x94, 0xc2, 0xe0, 0xd5, 0xc4, 0xf2, 0xf8, 0xe);


/////////////////////////////////////////////////////////////////
//color convertor FISH
/////////////////////////////////////////////////////////////////

BEGIN_INTERFACE_MAP(CColorConvertorFISH, CColorConvertorBase)
	INTERFACE_PART(CColorConvertorFISH, IID_IColorConvertorEx, CnvEx)
	INTERFACE_PART(CColorConvertorFISH, IID_IProvideHistColors, Hist)
	INTERFACE_PART(CColorConvertorFISH, IID_IEventListener, EvList)
	INTERFACE_PART(CColorConvertorFISH, IID_IColorConvertorFISH, FishCC)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CColorConvertorFISH, CnvEx)
IMPLEMENT_UNKNOWN(CColorConvertorFISH, Hist)
IMPLEMENT_UNKNOWN(CColorConvertorFISH, FishCC)

HRESULT CColorConvertorFISH::XFishCC::ReloadColorValue()
{
	METHOD_PROLOGUE_EX( CColorConvertorFISH, FishCC );
	pThis->_reload_color_values();
	return S_OK;
}

HRESULT CColorConvertorFISH::XFishCC::SetColorValue( UINT nID, DWORD dwColor )
{
	METHOD_PROLOGUE_EX( CColorConvertorFISH, FishCC );
	pThis->_reload_color_values_id( nID, dwColor );
	return S_OK;
}

COLORREF CColorConvertorFISH::DefCombiColor(unsigned uMask, int nPaneCount)
{
	if (g_bDefColors)
	{
		long lR = 0, lG = 0, lB = 0; 
		for( int i = 1; i < nPaneCount; i++ ) // 0 пана зарезервирована под цветокодированное и здесь не исп-ся
		{
			if( uMask & ( 1 << i ) )
			{
				lR += m_ColorR[i];
				lG += m_ColorG[i];
				lB += m_ColorB[i];
			}
		}
		return RGB(min(255,lR),min(255,lG),min(255,lB));
	}
	else
		return g_clrCombi;
}

HRESULT CColorConvertorFISH::XCnvEx::ConvertImageToDIBRect( BITMAPINFOHEADER *pbi, BYTE *pdibBits,
	POINT ptBmpOfs, IImage2 *pimage, RECT rectDest, POINT pointImageOffest, double fZoom,
	POINT ptScroll, DWORD dwFillColor, DWORD dwFlags, IUnknown *punkParams)
{
	METHOD_PROLOGUE_EX(CColorConvertorFISH, CnvEx);
	return pThis->DoConvertImageToDIBRect(pbi, pdibBits, ptBmpOfs, pimage, rectDest, pointImageOffest,
		fZoom, ptScroll, dwFillColor, dwFlags, punkParams, INT_MAX);
}

HRESULT CColorConvertorFISH::DoConvertImageToDIBRect( BITMAPINFOHEADER *pbi, BYTE *pdibBits,
	POINT ptBmpOfs, IImage2 *pimage, RECT rectDest, POINT pointImageOffest, double fZoom,
	POINT ptScroll, DWORD dwFillColor, DWORD dwFlags, IUnknown *punkParams, int nForcePane)
{
	byte	*pLookupTableR = m_pLookupTableR;
	byte	*pLookupTableG = m_pLookupTableG;
	byte	*pLookupTableB = m_pLookupTableB;

	byte	*ptemp = 0;
	if( dwFlags & cidrHilight )
	{
		ptemp = new byte[65536];
		memset( ptemp, 255, 65536 );

		byte	fillR = GetRValue(dwFillColor);
		byte	fillG = GetGValue(dwFillColor);
		byte	fillB = GetBValue(dwFillColor);

		if( fillR )pLookupTableR = ptemp;
		if( fillG )pLookupTableG = ptemp;
		if( fillB )pLookupTableB = ptemp;
	}

	CFishProps cf(pimage, punkParams);

	_reload_color_values(pimage);

	{ // возьмем из вьюхи настройки
		INamedPropBagPtr ptrBag = INamedPropBagPtr(punkParams);
        if(ptrBag!=0)
		{
			_variant_t	var;
			ptrBag->GetProperty( _bstr_t( FISH_COMPOSED_METHOD ), &var );
			if( var.vt != VT_EMPTY )
				m_nComposedMethod = var.lVal;
			else
			{
				m_nComposedMethod = ::GetValueInt( ::GetAppUnknown(), FISH_ROOT, FISH_COMPOSED_METHOD, 0 );
				ptrBag->SetProperty( _bstr_t( FISH_COMPOSED_METHOD ), _variant_t(long(m_nComposedMethod)) );
			}
		}
	}

	if(nInterpolationMethod==-1 || nMaskInterpolationMethod==-1)
	{
		nInterpolationMethod = GetValueInt(GetAppUnknown(), "\\General", "ImageInterpolationMethod", 0);
		nMaskInterpolationMethod = GetValueInt(GetAppUnknown(), "\\General", "ImageMaskInterpolationMethod", 0);
	}
	if(nInterpolationMethod==1 && fZoom>1.1)
	{
		CONVERTTODIB_PART1_BILINEAR

		Rbyte = pLookupTableR?pLookupTableR[R]:(R>>8);
		Gbyte = pLookupTableG?pLookupTableG[G]:(G>>8);
		Bbyte = pLookupTableB?pLookupTableB[B]:(B>>8);
			
		CONVERTTODIB_PART2_BILINEAR
	}
	else
	{
		int nPaneCount = ::GetImagePaneCount( pimage );
		if (nPaneCount>32) nPaneCount=32; // не более 32 пан
		long iShownPane = -1; // composed by default
		unsigned uPanesUsed = 0;
		uPanesUsed = ((1<<nPaneCount)-1); // Решено показывать все-таки все плоскости (BT 4179)
		long lEnabled=1;

		if ((dwFlags&cidrExportImage) == 0)
			cf.GetLongProp(&iShownPane, FISH_SHOWN_PANE);
		cf.GetLongProp(&uPanesUsed, FISH_USED_PANES);
		cf.GetLongProp(&lEnabled, FISH_ENABLED);

		if (nForcePane != INT_MAX) iShownPane = nForcePane;

		if (iShownPane >= 0) uPanesUsed = 1<<iShownPane;
		//else if( !uPanesUsed ) uPanesUsed = ((1<<nPaneCount)-1) & (~3); // исключим 0 и 1 пану - ratio и DAPI

		int arrThresholds[32];
		char szName[128];
		for(int i=0; i<nPaneCount; i++)
		{
            arrThresholds[i]=32768;
			sprintf(szName, "FishThreshold%u", i);
			cf.GetLongProp(&arrThresholds[i], szName);
		}
		//for (i = 2; i < nPaneCount; i++)
		//	if (arrThresholds[i] < arrThresholds[1])
		//		arrThresholds[i] = arrThresholds[1];

		long nCombos=g_bDefColors?(1<<nPaneCount):0;
		cf.GetLongProp(&nCombos, "FishCombos");

		smart_alloc(arrComboIdx, unsigned long, nCombos);
		smart_alloc(arrComboR, byte, nCombos);
		smart_alloc(arrComboG, byte, nCombos);
		smart_alloc(arrComboB, byte, nCombos);
		for(int i=0; i<nCombos; i++)
		{
            arrComboIdx[i] = i%(1<<nPaneCount);
            arrComboIdx[i] &= uPanesUsed;
			COLORREF clrDefault = DefCombiColor(arrComboIdx[i],nPaneCount);
            arrComboR[i] = GetRValue(clrDefault);
            arrComboG[i] = GetGValue(clrDefault);
            arrComboB[i] = GetBValue(clrDefault);

			sprintf(szName, "FishComboIdx%u", i);
			cf.GetLongProp(&arrComboIdx[i], szName);

			sprintf(szName, "FishComboColor%u", i);
			COLORREF cr = -1;
			if( cf.GetLongProp(&cr, szName) )
			{
				arrComboR[i] = GetRValue(cr);
				arrComboG[i] = GetGValue(cr);
				arrComboB[i] = GetBValue(cr);
			}
			if (!lEnabled)
				arrComboR[i] = arrComboG[i] = arrComboB[i] = Bright(arrComboB[i],arrComboG[i],arrComboR[i]);
		}

		byte *pColorR,*pColorG,*pColorB;
		CArray<byte,byte&> arrColorR,arrColorG,arrColorB;
		if (lEnabled && iShownPane != -1)
		{
			pColorR = m_ColorR;
			pColorG = m_ColorG;
			pColorB = m_ColorB;
		}
		else
		{
			arrColorR.SetSize(nPaneCount);
			arrColorG.SetSize(nPaneCount);
			arrColorB.SetSize(nPaneCount);
			pColorR = arrColorR.GetData();
			pColorG = arrColorG.GetData();
			pColorB = arrColorB.GetData();
			if (lEnabled)
			{
				for (int i = 0; i < nPaneCount; i++)
				{
					long nColorCoef = 100;
					sprintf(szName, "FishColorCoef%u", i);
					if( cf.GetLongProp(&nColorCoef, szName) )
						nColorCoef = min(max(nColorCoef,0),100);
					pColorR[i] = byte(long(m_ColorR[i])*nColorCoef/100);
					pColorG[i] = byte(long(m_ColorG[i])*nColorCoef/100);
					pColorB[i] = byte(long(m_ColorB[i])*nColorCoef/100);
				}
			}
			else
			{
				for (int i = 0; i < nPaneCount; i++)
					pColorR[i] = pColorG[i] = pColorB[i] = Bright(m_ColorB[i],m_ColorG[i],m_ColorR[i]);
			}
		}
		long lInvert = 0;
		if(iShownPane>=0)
		{
			sprintf(szName, "FishInvert%d", iShownPane);
			cf.GetLongProp(&lInvert, szName);
		}

		CONVERTTODIB_PART1

		color arrColors[] = { R, G, B, pan3, pan4, pan5, pan6, pan7, pan8,
							pan9,  pan10, pan11, pan12, pan13, pan14, pan15, pan16, pan17, pan18,
							pan19, pan20, pan21, pan22, pan23, pan24, pan25, pan26, pan27, pan28,
							pan29, pan30, pan31 };

//		if(lFishMode==1)
		if (iShownPane == -2)
		{	// цветокодированное изображение
			unsigned long u=0;
			if (yBmp == 256)
				u = 0;
			for( int i = 0; i < nPaneCount; i++ )
			{
				if( arrColors[i] >= arrThresholds[i] ) u |= (1<<i);
			}
			if(u==0)
			{
				Rbyte = Gbyte = Bbyte = 0;
			}
			else
			{
				Rbyte = GetRValue(g_clrCombi);
				Gbyte = GetGValue(g_clrCombi);
				Bbyte = GetBValue(g_clrCombi);
				for(int i=0; i<nCombos; i++)
				{
					if( u == arrComboIdx[i] )
					{
						Rbyte = arrComboR[i];
						Gbyte = arrComboG[i];
						Bbyte = arrComboB[i];
					}
				}
			}
		}
		else
		{	// обычное отображение
			long lR = 0, lG = 0, lB = 0; 

			if (lInvert) // pane can be inverted only when iShownPane >= 0
			{
				Rbyte = Gbyte = Bbyte = (byte)((0xFFFF-arrColors[iShownPane])>>8);
			}
			else
			{
				int cMax=0, cSum=0;
				if(m_nComposedMethod==1)
				{
					for( int i = 1; i < nPaneCount; i++ ) // 0 пана зарезервирована под цветокодированное и здесь не исп-ся
					{
						if( uPanesUsed & ( 1 << i ) )
						{
							lR += pColorR[i] * arrColors[i];
							lG += pColorG[i] * arrColors[i];
							lB += pColorB[i] * arrColors[i];
							if(arrColors[i]>cMax) cMax=arrColors[i];
							cSum += arrColors[i];
						}
					}
					if(cSum)
					{
						lR = lR/cSum*cMax;
						lG = lG/cSum*cMax;
						lB = lB/cSum*cMax;
					}
				}
				else if(m_nComposedMethod==2)
				{
					for( int i = 1; i < nPaneCount; i++ ) // 0 пана зарезервирована под цветокодированное и здесь не исп-ся
					{
						if( uPanesUsed & ( 1 << i ) )
						{
							lR = max(lR, pColorR[i] * arrColors[i]);
							lG = max(lG, pColorG[i] * arrColors[i]);
							lB = max(lB, pColorB[i] * arrColors[i]);
						}
					}
				}
				else 
				{
					for( int i = 1; i < nPaneCount; i++ ) // 0 пана зарезервирована под цветокодированное и здесь не исп-ся
					{
						if( uPanesUsed & ( 1 << i ) )
						{
							lR += pColorR[i] * arrColors[i];
							lG += pColorG[i] * arrColors[i];
							lB += pColorB[i] * arrColors[i];
						}
					}
				}
				Rbyte = (byte)min( 255, lR / 65536);
				Gbyte = (byte)min( 255, lG / 65536);
				Bbyte = (byte)min( 255, lB / 65536);
			}
		}
			
		CONVERTTODIB_PART2
	}

	if( ptemp )delete ptemp;

	return S_OK;
}


CColorConvertorFISH::CColorConvertorFISH()
{
	_OleLockApp( this );


	INamedDataPtr ptrData = ::GetAppUnknown();

	ptrData->SetupSection( _bstr_t( FISH_COLORS ) );
	ptrData->GetEntriesCount( &m_nPaneColors );

	if( !m_nPaneColors )
		m_nPaneColors = 32;

	m_ColorR = 0;
	m_ColorG = 0;
	m_ColorB = 0;


	srand( (unsigned)time( 0 ) );
	_reload_color_values();

	m_bShowPerc = GetValueInt(GetAppUnknown(), "ImagePane", "Percent", FALSE);
	Register( ::GetAppUnknown() );
}

CColorConvertorFISH::~CColorConvertorFISH()
{
	_OleUnlockApp( this );

	if( m_ColorR )
		delete []m_ColorR;
	if( m_ColorG )
		delete []m_ColorG;
	if( m_ColorB )
		delete []m_ColorB;

}

void CColorConvertorFISH::_reload_color_values()
{
	if( m_ColorR )
		delete []m_ColorR;
	if( m_ColorG )
		delete []m_ColorG;
	if( m_ColorB )
		delete []m_ColorB;

	m_ColorR = new byte[m_nPaneColors];
	m_ColorG = new byte[m_nPaneColors];
	m_ColorB = new byte[m_nPaneColors];

	for( int i = 0; i < m_nPaneColors; i++ )
	{
		CString str;
		str.Format( "%d", i );

		COLORREF clDef = RGB( rand() % 256, rand() % 256, rand() % 256 );
		COLORREF Colors = ::GetValueColor( ::GetAppUnknown(), FISH_COLORS, CString( FISH_COLOR_PANE ) + str, clDef );

		m_ColorR[i] = GetRValue( Colors );
		m_ColorG[i] = GetGValue( Colors );
		m_ColorB[i] = GetBValue( Colors );
	}
}
void CColorConvertorFISH::_reload_color_values_id(UINT nID, DWORD dwColor)
{
	if( nID < (UINT)m_nPaneColors )
	{
		m_ColorR[nID] = GetRValue( dwColor );
		m_ColorG[nID] = GetGValue( dwColor );
		m_ColorB[nID] = GetBValue( dwColor );
	}
}
void CColorConvertorFISH::_reload_color_values(IUnknown *punk)
{
	if (::GetObjectKey(punk) != m_guidImage)
	{
		INamedDataPtr ptrData = punk;
		if (ptrData == 0)
			return;
		ptrData->SetupSection(_bstr_t(FISH_COLORS));
		long nPaneColors;
		ptrData->GetEntriesCount(&nPaneColors);
		if (nPaneColors == 0)
			return;
		m_guidImage = ::GetObjectKey(punk);
		if( m_ColorR )
			delete []m_ColorR;
		if( m_ColorG )
			delete []m_ColorG;
		if( m_ColorB )
			delete []m_ColorB;
		m_ColorR = new byte[m_nPaneColors];
		m_ColorG = new byte[m_nPaneColors];
		m_ColorB = new byte[m_nPaneColors];
		for( int i = 0; i < m_nPaneColors; i++ )
		{
			CString str;
			str.Format( "%d", i );

			COLORREF clDef = RGB( rand() % 256, rand() % 256, rand() % 256 );
			COLORREF Colors = ::GetValueColor( punk, FISH_COLORS, CString( FISH_COLOR_PANE ) + str, clDef );

			m_ColorR[i] = GetRValue( Colors );
			m_ColorG[i] = GetGValue( Colors );
			m_ColorB[i] = GetBValue( Colors );
		}
	}
}



void CColorConvertorFISH::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !strcmp( szBeforeAppTerminate, pszEvent ) )
		UnRegister( ::GetAppUnknown() );
}

const char *CColorConvertorFISH::GetCnvName()
{
	return _T("FISH");
}

int CColorConvertorFISH::GetColorPanesCount()
{
	return 1;
}

const char *CColorConvertorFISH::GetPaneName(int numPane)
{
	return _T("");
}

const char *CColorConvertorFISH::GetPaneShortName( int numPane )
{
	return _T("");
}

CString CColorConvertorFISH::GetPaneShortName( int numPane, IUnknown *punkImage, IUnknown *punkParams)
{
	CString sPane;
	sPane.Format(_T("%s%d"), (const char *)FISH_PANE_SNAME, numPane);
	CString sPaneSName = GetValueString(punkImage, FISH_ROOT, sPane, 0);
	if (!sPaneSName.IsEmpty())
		return sPaneSName;
	sPane.Format(_T("%s%d"), (const char *)FISH_NAME_PANES, numPane);
	CString sPaneName = GetValueString(punkImage, FISH_ROOT, sPane, sPane);
	return sPaneName;
}

color CColorConvertorFISH::ConvertToHumanValue( color colorInternal, int numPane )
{
	return color( double(colorInternal) * 256.0 / double(colorMax) );
}

CString CColorConvertorFISH::GetColorValueString(IUnknown *punkImage, IUnknown *punkParams,
	int iPane, int nColor)
{
	char szBuff[4];
	if (m_bShowPerc)
		nColor = nColor*100/256;
	CString sName(GetPaneShortName(iPane,punkImage,punkParams));
	if (sName.IsEmpty())
		return CString(_itoa(nColor, szBuff, 10));
	else
	{
		CString s1;
		s1.Format(_T("%s:%d"), sName, nColor);
		return s1;
	}
}

bool CColorConvertorFISH::GetColorString(IUnknown *punkImage, IUnknown *punkParams, POINT pt, CString &s)
{
	int nPanes,iShownPane;
	unsigned int uMask;
	GetParamsByViewAndImg(punkImage, punkParams, nPanes, uMask, iShownPane);
	IImage2Ptr sptrImage(punkImage);
	color *pcolor;
	s = _T("(");
	bool bFirst = true;
	if (iShownPane < 0)
	{
		for (int i = 1; i < nPanes; i++) // pane 0 reserved
		{
			if (uMask&(1 << i))
			{
				if (bFirst)
					bFirst = false;
				else
					s += _T(",");
				sptrImage->GetRow(pt.y, i, &pcolor);
				int nColor = pcolor[pt.x]/256;
				s += GetColorValueString(punkImage, punkParams, i, nColor);
			}
		}
	}
	else
	{
		int iPane = min(iShownPane,nPanes-1);
		sptrImage->GetRow(pt.y, iPane, &pcolor);
		int nColor = pcolor[pt.x]/256;
		s += GetColorValueString(punkImage, punkParams, iPane, nColor);
	}
	s += _T(")");
	return true;
}

DWORD CColorConvertorFISH::GetPaneFlags(int numPane)
{
	return pfOrdinary | pfGray;
}

void CColorConvertorFISH::GetConvertorIcon( HICON *phIcon )
{
	if(phIcon)
		*phIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_FISH));
}

void CColorConvertorFISH::GetPaneColor(int numPane, DWORD* pdwColor)
{
	if( !pdwColor ) 
		return;

	if( numPane >= 0 && numPane < m_nPaneColors )
		*pdwColor = RGB( m_ColorR[numPane], m_ColorG[numPane], m_ColorB[numPane] );
	else
		*pdwColor = 0;
}

void CColorConvertorFISH::GetCorrespondPanesColorValues(int numPane, DWORD* pdwColorValues)
{
	const int NotUsed = 0;
	if(pdwColorValues == 0) return;
	switch(numPane)
	{
	case 0:
		*pdwColorValues =  RGB(0, 0, NotUsed);
		break;
	case 1:
		*pdwColorValues =  RGB(0, NotUsed, 0);
		break;
	case 2:
		*pdwColorValues =  RGB(NotUsed, 0, 0);
		break;
	default:
		*pdwColorValues = 0;
	}
}


/*** Color conversation *****************************************************************************/
void CColorConvertorFISH::ConvertImageToRGB( color **_ppColor, byte *_pRGB, int _cx )
{
/*
	for( long n = 0, c = 0; n < _cx; n++ )
	{
		long lR = 0, lG = 0, lB = 0; 
		for( int i = 0; i < nPaneCount; i++ )
		{
			lR += m_ColorR[i] * _ppColor[i][n];
			lG += m_ColorG[i] * _ppColor[i][n];
			lB += m_ColorB[i] * _ppColor[i][n];
		}

		_pRGB[c++] = min( 255, lR / 65536);
		_pRGB[c++] = min( 255, lG / 65536);
		_pRGB[c++] = min( 255, lB / 65536);
	}
*/
}


void CColorConvertorFISH::ConvertImageToGray( color **_ppColor, byte *pGray, int cx )
{
/*
	for( long n = 0; n < cx; n++ )
	{
		long lR = 0, lG = 0, lB = 0; 
		for( int i = 0; i < nPaneCount; i++ )
		{
			lR += m_ColorR[i] * _ppColor[i][n];
			lG += m_ColorG[i] * _ppColor[i][n];
			lB += m_ColorB[i] * _ppColor[i][n];
		}

		pGray[n] = Bright( min( 255, lR / 65536), 
						   min( 255, lG / 65536), 
						   min( 255, lB / 65536) );

	}
*/
}

void CColorConvertorFISH::ConvertImageToLAB( color **ppColor, double *pLab, int cx )
{
/*
	double X, Y, Z, fX, fY, fZ;
	double R, G, B;
	double L, a, b;
	for( long n = 0, c = 0; n < cx; n++ )
	{							  
		long lR = 0, lG = 0, lB = 0; 
		for( int i = 0; i < nPaneCount; i++ )
		{
			lR += m_ColorR[i] * _ppColor[i][n];
			lG += m_ColorG[i] * _ppColor[i][n];
			lB += m_ColorB[i] * _ppColor[i][n];
		}

		R = ScaleColorToDouble(lR);
		G = ScaleColorToDouble(lG);
		B = ScaleColorToDouble(lB);

		X = (0.412453*R + 0.357580*G + 0.180423*B)/0.950456;
		Y = (0.212671*R + 0.715160*G + 0.072169*B);
	    Z = (0.019334*R + 0.119193*G + 0.950227*B)/1.088754;

		if (Y > 0.008856)
	    {
	       fY = pow(Y, 1.0/3.0);
		   L = 116.0*fY - 16.0;
		}
		else
		{
			fY = 7.787*Y + 16.0/116.0;
			L = 903.3*Y;
		}
		if (X > 0.008856)
			fX = pow(X, 1.0/3.0);
		else
			fX = 7.787*X + 16.0/116.0;
		if (Z > 0.008856)
			fZ = pow(Z, 1.0/3.0);
		else
			fZ = 7.787*Z + 16.0/116.0;

		a = 500.0*(fX - fY);
		b = 200.0*(fY - fZ);

		pLab[c++] = L;
		pLab[c++] = a;
		pLab[c++] = b;
	}
*/
}


void CColorConvertorFISH::ConvertRGBToImage( byte *_pRGB, color **_ppColor, int _cx )
{
	ConvertRGBToImageEx(_pRGB, _ppColor, _cx, 1);
}

void CColorConvertorFISH::ConvertRGBToImageEx( byte *_pRGB, color **_ppColor, int _cx, int nPanes )
{
    byte	r, g, b, byteC;
	for( long n = 0, c = 0; n < _cx; n++ )
	{
		b = _pRGB[c++];
		g = _pRGB[c++];
		r = _pRGB[c++];
		byteC = Bright(r, g, b);
		for( int i = 0; i < nPanes; i++ )
			_ppColor[i][n] = ByteAsColor(byteC);
	}
}


void CColorConvertorFISH::ConvertGrayToImage( byte *pGray, color **ppColor, int cx )
{
}

void CColorConvertorFISH::ConvertLABToImage( double *pLab, color **ppColor, int cx )
{
}

void CColorConvertorFISH::ConvertImageToRGB2(color **ppColor, byte *pRGB, int cx, int nPane,
	IUnknown *punkImage, IUnknown *punkParams)
{
	CFishData FishData;
	FishData.Init(this, punkImage, punkParams, nPane);
	for (int x = 0; x <cx; x++)
	{
		color arrColors[32];
		for (int i = 0; i < FishData.nPaneCount; i++)
			arrColors[i] = (FishData.uPanesUsed & (1<<i))?ppColor[i][x]:0;
		ConvertPoint(arrColors, FishData, pRGB[3*x+2], pRGB[3*x+1], pRGB[3*x]);
	}
}

void CColorConvertorFISH::ConvertPoint(color *arrColors, CFishData &FishData, byte &Rbyte, byte &Gbyte, byte &Bbyte)
{
	if (FishData.iShownPane == -2)
	{
		unsigned long u=0;
		for( int i = 0; i < FishData.nPaneCount; i++ )
			if( arrColors[i] >= FishData.arrThresholds[i] ) u |= (1<<i);
		if(u==0)
		{
			Rbyte = Gbyte = Bbyte = 0;
		}
		else
		{
			Rbyte = Gbyte = Bbyte = 192;
			for(int i=0; i<FishData.nCombos; i++)
			{
				if( u == FishData.arrComboIdx[i] )
				{
					Rbyte = FishData.arrComboR[i];
					Gbyte = FishData.arrComboG[i];
					Bbyte = FishData.arrComboB[i];
				}
			}
		}
	}
	else
	{	// обычное отображение
		long lR = 0, lG = 0, lB = 0; 

		for( int i = 1; i < FishData.nPaneCount; i++ ) // 0 пана зарезервирована под цветокодированное и здесь не исп-ся
		{
			if( FishData.uPanesUsed & ( 1 << i ) )
			{
				lR += m_ColorR[i] * arrColors[i];
				lG += m_ColorG[i] * arrColors[i];
				lB += m_ColorB[i] * arrColors[i];
			}
		}

		Rbyte = (byte)min( 255, lR / 65536);
		Gbyte = (byte)min( 255, lG / 65536);
		Bbyte = (byte)min( 255, lB / 65536);
	}
}

void CColorConvertorFISH::ConvertImageToGray2(color **ppColor, byte *pGray, int cx, int nPane,
	IUnknown *punkImage, IUnknown *punkParams)
{
	CFishData FishData;
	FishData.Init(this, punkImage, punkParams, nPane);
	for (int x = 0; x <cx; x++)
	{
		if (FishData.iShownPane < 0)
		{
			color arrColors[32];
			for (int i = 0; i < FishData.nPaneCount; i++)
				arrColors[i] = (FishData.uPanesUsed & (1<<i))?ppColor[i][x]:0;
			byte r,g,b;
			ConvertPoint(arrColors, FishData, r, g, b);
			pGray[x] = Bright(r,g,b);
		}
		else
			pGray[x] = (ppColor[FishData.iShownPane][x]>>8);
	}
}


void CColorConvertorFISH::GetParamsByViewAndImg(IUnknown *punkImage, IUnknown *punkParams,
	int &nPaneCount, unsigned &uPanesUsed, int &iShownPane)
{
	INamedPropBagPtr ptrBag(punkParams);
	nPaneCount = ::GetImagePaneCount(punkImage);
	if (nPaneCount>32) nPaneCount=32; // не более 32 пан
	iShownPane = -1; // composed by default
	uPanesUsed = ((1<<nPaneCount)-1) & (~3); // по умолчанию все, кроме 0 и 1 паны - ratio и DAPI

	CFishProps cf(punkImage, punkParams);

	cf.GetLongProp(&iShownPane, FISH_SHOWN_PANE);
	cf.GetLongProp(&uPanesUsed, FISH_USED_PANES);

	if (iShownPane >= 0) uPanesUsed = 1<<iShownPane;
	//else if( !uPanesUsed ) uPanesUsed = ((1<<nPaneCount)-1) & (~3); // исключим 0 и 1 пану - ratio и DAPI
}


bool CColorConvertorFISH::OnGetUsedPanes(IUnknown *punkImage, IUnknown *punkParams, bool *pbUsedPanes, int nSize)
{
	int nPaneCount,iShownPane;
	unsigned uPanesUsed;
	GetParamsByViewAndImg(punkImage, punkParams, nPaneCount, uPanesUsed, iShownPane);
	for (int i = 0; i < min(nPaneCount,nSize); i++)
	{
		if (uPanesUsed&(1 << i))
			pbUsedPanes[i] = true;
		else
			pbUsedPanes[i] = false;
	}
	for (; i < nSize; i++)
		pbUsedPanes[i] = false;
	return true;
}

bool CColorConvertorFISH::OnGetPalette(IUnknown *punkImage, IUnknown *punkView, COLORREF *pColors, int nColors)
{
	int nPaneCount,iShownPane;
	unsigned uPanesUsed;
	GetParamsByViewAndImg(punkImage, punkView, nPaneCount, uPanesUsed, iShownPane);
	for (int iColor = 0; iColor < nColors; iColor++)
	{
		long lR = 0, lG = 0, lB = 0; 
		for (int i = 1; i < nPaneCount; i++) // 0 пана зарезервирована под цветокодированное и здесь не исп-ся
		{
			if (uPanesUsed & (1 << i))
			{
				lR += m_ColorR[i]*iColor;
				lG += m_ColorG[i]*iColor;
				lB += m_ColorB[i]*iColor;
			}
		}
		lR = min(255, lR/255);
		lG = min(255, lG/255);
		lB = min(255, lB/255);
		pColors[iColor] = RGB(lR,lG,lB);
	}
	return true;
}

void CColorConvertorFISH::OnInitObjectAfterThresholding(IUnknown *punkNewObjImg, IUnknown *punkBaseImg, INIT_OBJECT_DATA *pData)
{
	INamedPropBagPtr ptrBag(punkNewObjImg);
	if (ptrBag != 0)
	{
		_variant_t	var1((long)pData->m_nPane);
		ptrBag->SetProperty(_bstr_t( FISH_SHOWN_PANE ), var1);
		_variant_t	var2((long)-1L);
		ptrBag->SetProperty(_bstr_t( FISH_USED_PANES ), var2);
		_variant_t	var3((long)1L);
		ptrBag->SetProperty(_bstr_t( "IgnoreViewSettings" ), var3);
	}
}

HRESULT CColorConvertorFISH::XHist::GetHistColors( int nPaneNo, COLORREF *pcrArray, COLORREF *pcrCurve )
{
	METHOD_PROLOGUE_EX(CColorConvertorFISH, Hist);
	
	if( nPaneNo >= pThis->m_nPaneColors )
		return S_FALSE;

	*pcrCurve = RGB( pThis->m_ColorR[nPaneNo], pThis->m_ColorG[nPaneNo], pThis->m_ColorB[nPaneNo] );

	for( int i = 0; i < 256; i++, pcrArray++ )
		*pcrArray = RGB( pThis->m_ColorR[nPaneNo] * i / 256, pThis->m_ColorG[nPaneNo] * i / 256, pThis->m_ColorB[nPaneNo] * i / 256 );

	return S_OK;
}
