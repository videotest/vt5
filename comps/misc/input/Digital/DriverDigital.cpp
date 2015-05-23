// DriverDigital.cpp: implementation of the CDriverDigital class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Digital.h"
#include "DriverDigital.h"
#include "GrabImg.h"
#include "SetupDialog.h"
#include "DigSettings.h"
#include "Accumulate.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


class CDibDraw
{
public:
	HDRAWDIB	hdd;

	CDibDraw()
	{ hdd = ::DrawDibOpen();}
	~CDibDraw()
	{ DrawDibClose( hdd );}
};

CDibDraw	hdd;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDriverDigital, CDriverBase)


CDriverDigital::CDriverDigital()
{
	EnableAutomation();
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	_OleLockApp( this );
	m_dwFlags = FG_SUPPORTSVIDEO;
	m_strDriverName = "Digital";
	m_strDriverFullName.LoadString(IDS_DIGITAL);
	m_strCategory = "Input";
	m_dwFlags = FG_INPUTFRAME|FG_ACCUMULATION;
	m_lpbi = NULL;
	m_nUseCount = 0;
	ReadVideoSettings();
}

CDriverDigital::~CDriverDigital()
{
	_OleUnlockApp( this );
}

BEGIN_MESSAGE_MAP(CDriverDigital, CDriverBase)
	//{{AFX_MSG_MAP(CDriverDigital)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CDriverDigital, CDriverBase)
	//{{AFX_DISPATCH_MAP(CDriverDigital)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IDriverDigital to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {BB893CED-21CE-11D4-A98A-FC9799051A4B}
static const IID IID_IDriverDigital = 
{ 0xbb893ced, 0x21ce, 0x11d4, { 0xa9, 0x8a, 0xfc, 0x97, 0x99, 0x5, 0x1a, 0x4b } };

BEGIN_INTERFACE_MAP(CDriverDigital, CDriverBase)
	INTERFACE_PART(CDriverDigital, IID_IDriverDigital, Dispatch)
	INTERFACE_PART(CDriverDigital, IID_IInputPreview, InpPrv)
END_INTERFACE_MAP()

// {BB893CEF-21CE-11D4-A98A-FC9799051A4B}
GUARD_IMPLEMENT_OLECREATE(CDriverDigital, "IDriver.DriverDigital", 0xbb893cef, 0x21ce, 0x11d4, 0xa9, 0x8a, 0xfc, 0x97, 0x99, 0x5, 0x1a, 0x4b);

bool CDriverDigital::OnBeginPreview(IInputPreviewSite *pSite)
{
	if (m_nUseCount++ == 0)
	{
		if (!CSetupDialog::m_pbi)
			ReadVideoSettings();
		unsigned cx = ulNewColSrc;
		unsigned cy = ulNewRow*2;
		unsigned nSize = sizeof(BITMAPINFOHEADER)+256*sizeof(BITMAPINFOHEADER)+cx*cy;
		m_lpbi = (LPBITMAPINFOHEADER)calloc(nSize, 1);
	}
	return true;
}

void CDriverDigital::OnEndPreview(IInputPreviewSite *pSite)
{
	if (--m_nUseCount <= 0)
	{
		free(m_lpbi);
		m_lpbi = NULL;
	}
}

void CDriverDigital::OnGetSize(short &cx, short &cy)
{
	cx = ulNewColSrc;
	cy = ulNewRow*2;
}

void CDriverDigital::OnGetPreviewFreq(DWORD &dwFreq)
{
	dwFreq = GetProfileInt(_T("RedrawFrequency"), 300);
}

void CDriverDigital::OnDrawRect(HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst)
{
	if (m_lpbi)
	{
		LPBITMAPINFOHEADER lpbi = CSetupDialog::m_pbi?CSetupDialog::m_pbi:m_lpbi;
		LPBYTE pData = (LPBYTE)(((RGBQUAD *)(lpbi+1))+256);
		CSize sz(lpbi->biWidth,lpbi->biHeight);
		CRect rcSrc(*lpRectSrc);
		CRect rcDst(*lpRectDst);
		bool bStretch = rcSrc.Width() != rcDst.Width() || rcSrc.Height() != rcDst.Height();
		if (rcSrc.Width() > sz.cx)
		{
			rcDst.right = rcDst.left + rcDst.Width()*sz.cx/rcSrc.Width();
			rcSrc.right = rcSrc.left + sz.cx;
		}
		if (rcSrc.Height() > sz.cx)
		{
			rcDst.bottom = rcDst.top + rcDst.Height()*sz.cy/rcSrc.Height();
			rcSrc.bottom = rcSrc.top + sz.cy;
		}
		if (!CSetupDialog::m_pbi)
			GrabImage(m_lpbi,0,1);
		if (bStretch)
				DrawDibDraw(hdd.hdd, hDC, lpRectDst->left, lpRectDst->top, rcDst.Width(), rcDst.Height(), lpbi,
					pData, rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height(), 0);
		else
			SetDIBitsToDevice(hDC, lpRectDst->left-rcSrc.left, lpRectDst->top-rcSrc.top, sz.cx, sz.cy, 0,
				0, 0, sz.cy, pData, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);
	}
	else
	{
		HBRUSH br = ::CreateSolidBrush(RGB(0,0,0));
		FillRect(hDC, lpRectDst, br);
		DeleteObject(br);
	}
	// Do not call CStatic::OnPaint() for painting messages
}

void CDriverDigital::OnSetValue(int nDev, LPCTSTR lpName, VARIANT var)
{
	CDriverBase::OnSetValue(nDev, lpName, var);
	if (m_lpbi) // Preview
		ReadVideoSettings();
}

VARIANT CDriverDigital::OnGetValue(int nDev, LPCTSTR lpName)
{
	return CDriverBase::OnGetValue(nDev, lpName);
}


void CDriverDigital::OnInputImage(IUnknown *pUnkImg, IUnknown *punkTarget, int nDevice, BOOL bDialog)
{
	ReadVideoSettings();
	unsigned cx = ulNewColSrc;
	unsigned cy = ulNewRow*2;
	unsigned nSize = sizeof(BITMAPINFOHEADER)+256*sizeof(BITMAPINFOHEADER)+cx*cy;
	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)calloc(nSize, 1);
	if (!lpbi) return;
	BOOL bAccum = g_DigProfile.GetProfileInt(NULL, _T("Accumulate"), FALSE);
	if (bAccum)
	{
		int nImagesNum = g_DigProfile.GetProfileInt(NULL, _T("AccumulateImagesNum"), FALSE);
		CAccumulate Accum;
		for (int i = 0; i < nImagesNum; i++)
		{
			GrabImage(lpbi,0,0);
			Accum.Add(lpbi);
		}
		Accum.GetResult(lpbi, true);
		CImageWrp img(pUnkImg);
		LPBYTE pData = (LPBYTE)(((RGBQUAD *)(lpbi+1))+lpbi->biClrUsed);
		img.AttachDIBBits(lpbi, pData);
	}
	else
	{
		GrabImage(lpbi,0,0);
		CImageWrp img(pUnkImg);
		LPBYTE pData = (LPBYTE)(((RGBQUAD *)(lpbi+1))+256);
		img.AttachDIBBits(lpbi, pData);
	}
	free(lpbi);
}

bool CDriverDigital::OnExecuteSettings(HWND hwndParent, IUnknown *punkTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput)
{
	if (nMode == 0)
		return true;
	else
	{
		ReadVideoSettings();
		CSetupDialog dlg;
		int r = dlg.DoModal();
		SaveVideoSettings();
		return r == IDOK;
	}
}





