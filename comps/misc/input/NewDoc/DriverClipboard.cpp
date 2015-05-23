// DriverClipboard.cpp: implementation of the CDriverClipboard class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "newdoc.h"
#include "DriverClipboard.h"
#include "ClipboardDlg.h"
#include "..\\DrvCommon2\\ddib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CDibDraw	hdd;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDriverClipboard, CDriverBase)

CDriverClipboard::CDriverClipboard()
{
	EnableAutomation();
	
	_OleLockApp( this );

	m_strDriverName = "clipboard";
	m_strDriverFullName.LoadString(IDS_CLIPBOARD);
}

CDriverClipboard::~CDriverClipboard()
{
	_OleUnlockApp( this );
}


void CDriverClipboard::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.
	CDriverBase::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CDriverClipboard, CDriverBase)
	//{{AFX_MSG_MAP(CDriverClipboard)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CDriverClipboard, CDriverBase)
	//{{AFX_DISPATCH_MAP(CDriverClipboard)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IDriverEmpty to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {C3BF0811-2353-4C91-A4B0-52F229C7F672}
static const IID IID_IDriverClipboard = 
{ 0xc3bf0811, 0x2353, 0x4c91, { 0xa4, 0xb0, 0x52, 0xf2, 0x29, 0xc7, 0xf6, 0x72 } };

BEGIN_INTERFACE_MAP(CDriverClipboard, CDriverBase)
	INTERFACE_PART(CDriverClipboard, IID_IDriverClipboard, Dispatch)
//	INTERFACE_PART(CDriverClipboard, IID_IInputPreview, InpPrv)
END_INTERFACE_MAP()

// {317A60B4-8D82-4BEA-9F9B-203ABFC90732}
GUARD_IMPLEMENT_OLECREATE(CDriverClipboard, "IDriver.DriverClipboard", 0x317a60b4, 0x8d82, 0x4bea, 0x9f, 0x9b, 0x20, 0x3a, 0xbf, 0xc9, 0x7, 0x32);


bool CDriverClipboard::OnBeginPreview(IInputPreviewSite *pSite)
{
	return true;
}

void CDriverClipboard::OnEndPreview(IInputPreviewSite *pSite)
{
}

void CDriverClipboard::OnInputImage(IUnknown *pUnk, IUnknown *punkTarget, int nDevice, BOOL bDialog)
{
	CClipboardDIB Dib;
	if (Dib.lpbi)
	{
		CImageWrp imgNew(pUnk);
		imgNew.AttachDIBBits(Dib.lpbi, Dib.pData);
	}
}

bool CDriverClipboard::OnExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput)
{
	if (nMode != 0)
	{
		CClipboardDlg Dlg(this, CWnd::FromHandle(hwndParent));
		int r = Dlg.DoModal();
		return r == IDOK;
	}
	return true;
}


void CDriverClipboard::OnGetSize(short &cx, short &cy)
{
	CClipboardDIB Dib;
	if (Dib.lpbi)
	{
		cx = (short)Dib.lpbi->biWidth;
		cy = (short)Dib.lpbi->biHeight;
	}
	else
	{
		cx = 0;
		cy = 0;
	}
}

void CDriverClipboard::OnDrawRect(HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst)
{
	CClipboardDIB Dib;
	if (Dib.lpbi)
	{
		CRect rcSrc(*lpRectSrc);
		CRect rcDst(*lpRectDst);
//		LPBYTE lpData = (LPBYTE)((RGBQUAD *)(Dib.lpbi+1))+Dib.lpbi->biClrUsed*sizeof(RGBQUAD);
		LPBYTE lpData = Dib.pData;
		Dib.lpbi->biSizeImage = 0;
		Dib.lpbi->biXPelsPerMeter = Dib.lpbi->biYPelsPerMeter = 0;
		DrawDibDraw(hdd.hdd, hDC, lpRectDst->left, lpRectDst->top, rcDst.Width(), rcDst.Height(), Dib.lpbi,
			lpData, rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height(), 0);
	}
	else
	{
		
		HBRUSH hbr = ::GetSysColorBrush(COLOR_BTNFACE);
		::FillRect(hDC, lpRectDst, hbr);
		::SetBkMode(hDC,TRANSPARENT);
		CString s;
		s.LoadString(IDS_CLIPBOARD_MESSAGE);
		::DrawText(hDC, s, -1, lpRectDst, DT_WORDBREAK|DT_CENTER);
	}
}

void CDriverClipboard::OnSetValue(int nDev, LPCTSTR lpName, VARIANT var)
{
	CDriverBase::OnSetValue(nDev, lpName, var);
}

VARIANT CDriverClipboard::OnGetValue(int nDev, LPCTSTR lpName)
{
	if (!_tcsicmp(lpName,_T("ClipboardAvailable")))
	{
		int n = 0;
		CClipboardDIB Dib;
		if (Dib.lpbi)
			n = 1;
		VARIANT var;
		var.vt = VT_I4;
		var.lVal = n;
		return var;
	}
	else
		return CDriverBase::OnGetValue(nDev, lpName);
}

void CDriverClipboard::OnGetPreviewFreq(DWORD &dwFreq)
{
	dwFreq = GetProfileInt(_T("RedrawFrequency"), 1000);
}

/////////////////////////////////////////////////////////////////////////////
// CDriverClipboard message handlers

