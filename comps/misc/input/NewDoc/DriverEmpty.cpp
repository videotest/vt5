// DriverEmpty.cpp : implementation file
//

#include "stdafx.h"
#include "NewDoc.h"
#include "DriverEmpty.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDriverEmpty

IMPLEMENT_DYNCREATE(CDriverEmpty, CDriverBase)

CDriverEmpty::CDriverEmpty()
{
	EnableAutomation();
	
	_OleLockApp( this );
	m_strDriverName = "Empty";
	m_strDriverName.LoadString( IDS_EMPTY );
}

CDriverEmpty::~CDriverEmpty()
{
	_OleUnlockApp( this );
}


void CDriverEmpty::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CDriverBase::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CDriverEmpty, CDriverBase)
	//{{AFX_MSG_MAP(CDriverEmpty)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CDriverEmpty, CDriverBase)
	//{{AFX_DISPATCH_MAP(CDriverEmpty)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IDriverEmpty to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {FDE17BB1-7E51-4C52-A87A-EB2ECC2410B5}
static const IID IID_IDriverEmpty =
{ 0xfde17bb1, 0x7e51, 0x4c52, { 0xa8, 0x7a, 0xeb, 0x2e, 0xcc, 0x24, 0x10, 0xb5 } };


BEGIN_INTERFACE_MAP(CDriverEmpty, CDriverBase)
	INTERFACE_PART(CDriverEmpty, IID_IDriverEmpty, Dispatch)
	INTERFACE_PART(CDriverEmpty, IID_IInputPreview, InpPrv)
//	INTERFACE_PART(CDriverEmpty, IID_IDriver, Drv)
END_INTERFACE_MAP()

// {6A6CD424-8DD2-4283-9AC2-300E27821666}
GUARD_IMPLEMENT_OLECREATE(CDriverEmpty, "IDriver.DriverEmpty", 0x6a6cd424, 0x8dd2, 0x4283, 0x9a, 0xc2, 0x30, 0xe, 0x27, 0x82, 0x16, 0x66)


void CDriverEmpty::OnInputImage(IUnknown *pUnk, IUnknown *punkTarget, int nDevice, BOOL bDialog)
{
	int cx = GetProfileInt(_T("Width"), 768);
	int cy = GetProfileInt(_T("Height"), 576);
	CString sCC = GetProfileString(_T("ColorConvertor"), _T("RGB"));
	CImageWrp	imageNew( pUnk );
	imageNew.CreateNew(cx, cy, sCC);
	int nColorsCount = imageNew.GetColorsCount();
	for (int nClr = 0; nClr < nColorsCount; nClr++)
	{
		CString sPane;
		sPane.Format(_T("Pane%d"), nClr);
		color clr = GetProfileInt(sPane, 196<<8);
		for (int y = 0; y < cy; y++)
		{
			color *pRow = imageNew.GetRow(y, nClr);
			for (int x = 0; x < cx; x++)
				pRow[x] = clr;
		}
	}
}

void CDriverEmpty::OnGetSize(short &cx, short &cy)
{
	cx = (short)GetProfileInt(_T("Width"), 768);
	cy = (short)GetProfileInt(_T("Height"), 576);
}


void CDriverEmpty::OnDrawRect(HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst)
{
	CString sCC = GetProfileString(_T("ColorConvertor"), _T("RGB"));
	if (sCC == "GRAY")
	{
		color clr = GetProfileInt("Pane0", 196<<8);
		clr >>= 8;
		HBRUSH hbr = ::CreateSolidBrush(RGB(clr,clr,clr));
		::FillRect(hDC, lpRectDst, hbr);
		::DeleteObject(hbr);
	}
	else
	{
		sptrIColorConvertor2 sptrCC;
		sptrCC = GetCCByName(sCC);
		int nCount = 0;
		HRESULT hr = sptrCC->GetColorPanesDefCount(&nCount);
		if (SUCCEEDED(hr))
		{
			color *pclr = new color[nCount];
			color **ppclr = new color*[nCount];
			byte aRGB[3];
			for (int i = 0; i < nCount; i++)
			{
				CString sPane;
				sPane.Format(_T("Pane%d"), i);
				color clr = GetProfileInt(sPane, 196<<8);
				pclr[i] = clr;
				ppclr[i] = &pclr[i];
			}
			sptrCC->ConvertImageToDIB(ppclr, aRGB, 1, TRUE);
			delete ppclr;
			delete pclr;
			HBRUSH hbr = ::CreateSolidBrush(RGB(aRGB[2],aRGB[1],aRGB[0]));
			::FillRect(hDC, lpRectDst, hbr);
			::DeleteObject(hbr);

		}
		else
		{
			HBRUSH hbr = ::CreateSolidBrush(RGB(0,64,0));
			::FillRect(hDC, lpRectDst, hbr);
			::DeleteObject(hbr);
		}
	}
}

void CDriverEmpty::OnSetValue(int nDev, LPCTSTR lpName, VARIANT var)
{
	CDriverBase::OnSetValue(nDev, lpName, var);
	if ( !strcmp(lpName, "ColorConvertor") || !strncmp(lpName, "Pane", 4))
	{
		for (int i = 0; i < m_arrSites.GetSize(); i++)
			m_arrSites[i]->Invalidate();
	}
	else if (!strcmp(lpName, "Width") || !strcmp(lpName, "Height"))
	{
		for (int i = 0; i < m_arrSites.GetSize(); i++)
			m_arrSites[i]->OnChangeSize();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDriverEmpty message handlers
