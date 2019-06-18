// DriverTwain.cpp: implementation of the CDriverTwain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "newdoc.h"
#include "DriverTwain.h"
#include "TwainWork.h"
#include "Accumulate.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CTwainDataSrcMgr g_DataSrcMgr;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDriverTwain, CDriverBase)

CDriverTwain::CDriverTwain()
{
	EnableAutomation();
	_OleLockApp( this );
	m_strDriverName = "TWAIN";
	m_strDriverFullName.LoadString(IDS_TWAIN);
	m_strCategory.LoadString(IDS_INPUT);
	m_hMem = NULL;
	m_bDSMInited = false;
	m_dwFlags = FG_ACCUMULATION|FG_INPUTFRAME;
}

CDriverTwain::~CDriverTwain()
{
	g_DataSrcMgr.Close();
	_OleUnlockApp( this );
}

BEGIN_MESSAGE_MAP(CDriverTwain, CDriverBase)
	//{{AFX_MSG_MAP(CDriverTwain)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CDriverTwain, CDriverBase)
	//{{AFX_DISPATCH_MAP(CDriverTwain)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IDriverTWAIN to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {34711FB6-CB3F-45EB-BE42-0FF4296FA6BF}
static const IID IID_IDriverTWAIN = 
{ 0x34711fb6, 0xcb3f, 0x45eb, { 0xbe, 0x42, 0xf, 0xf4, 0x29, 0x6f, 0xa6, 0xbf } };

BEGIN_INTERFACE_MAP(CDriverTwain, CDriverBase)
	INTERFACE_PART(CDriverTwain, IID_IDriverTWAIN, Dispatch)
//	INTERFACE_PART(CDriverEmpty, IID_IDriver, Drv)
END_INTERFACE_MAP()

// {1BDF6FDF-8696-4BE5-9E46-E8D720F7D7E6}
GUARD_IMPLEMENT_OLECREATE(CDriverTwain, "IDriver.DriverTwain", 0x1bdf6fdf, 0x8696, 0x4be5, 0x9e, 0x46, 0xe8, 0xd7, 0x20, 0xf7, 0xd7, 0xe6);





void CDriverTwain::OnInputImage(IUnknown *pUnk, IUnknown *punkTarget, int nDevice, BOOL bFromSettings)
{
	if (bFromSettings && m_hMem)
	{
		LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)GlobalLock(m_hMem);
		CImageWrp imgNew(pUnk);
		imgNew.AttachDIBBits(lpbi, (LPBYTE)(((RGBQUAD *)(lpbi+1))+lpbi->biClrUsed));
		GlobalUnlock(m_hMem);
		GlobalFree(m_hMem);
		m_hMem = NULL;
	}
	else
	{
		CTwainDataSource *p = g_DataSrcMgr.m_DataSources.GetAt(nDevice);
		CTwainWork Work;
		HGLOBAL hMem = Work.ScanNative(p, FALSE);
		if (!hMem) return;
		LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)GlobalLock(hMem);
		CImageWrp imgNew(pUnk);
		imgNew.AttachDIBBits(lpbi, (LPBYTE)(((RGBQUAD *)(lpbi+1))+lpbi->biClrUsed));
		GlobalUnlock(hMem);
		GlobalFree(hMem);
	}
}

bool CDriverTwain::OnExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput)
{
	CTwainDataSource *p = g_DataSrcMgr.m_DataSources.GetAt(nDevice);
	CTwainWork Work;
	HGLOBAL hMem = NULL;
	BOOL bAccumulate = GetProfileInt(_T("Accumulate"), FALSE);
	BOOL bUIControllable = GetProfileInt(_T("UIControllable"), FALSE, true);
	if (!bUIControllable && nMode == 0)
		nMode = 1;
	g_DataSrcMgr.SetParent(hwndParent);
	if (bAccumulate && bForInput)
	{
		CAccumulate Accum;
		RGBQUAD rgb[256];
		memset(rgb, 0, sizeof(rgb));
		int  nImages = GetProfileInt(_T("AccumulateImagesNum"),2);
		for (int i = 0; i < nImages; i++)
		{
			hMem = Work.ScanNative(p, i==0&&nMode!=0);
			if (!hMem)
				return false;
			LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)GlobalLock(hMem);
			if (lpbi->biBitCount == 8)
				memcpy(rgb, lpbi+1, sizeof(rgb));
			Accum.Add(lpbi);
			GlobalUnlock(hMem);
			GlobalFree(hMem);
		}
		bool bTC = Accum.IsTrueColor();
		int cx = Accum.GetCX();
		int cy = Accum.GetCY();
		HGLOBAL hMemNew = NULL;
		LPBITMAPINFOHEADER lpbiNew = NULL;
		if (bTC)
		{
			DWORD dw = ((3*cx+3)>>2)<<2;
			hMemNew = GlobalAlloc(GHND, sizeof(BITMAPINFOHEADER)+dw*cy);
			lpbiNew = (LPBITMAPINFOHEADER)GlobalLock(hMemNew);
			lpbiNew->biBitCount = 24;
			lpbiNew->biClrUsed = lpbiNew->biClrImportant = 0;
		}
		else
		{
			DWORD dw = ((cx+3)>>2)<<2;
			hMemNew = GlobalAlloc(GHND, sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD)+dw*cy);
			lpbiNew = (LPBITMAPINFOHEADER)GlobalLock(hMemNew);
			lpbiNew->biBitCount = 8;
			lpbiNew->biClrUsed = lpbiNew->biClrImportant = 256;
			memcpy(lpbiNew+1, rgb, sizeof(rgb));
		}
		lpbiNew->biSize = sizeof(BITMAPINFOHEADER);
		lpbiNew->biWidth = cx;
		lpbiNew->biHeight = cy;
		lpbiNew->biCompression = BI_RGB;
		lpbiNew->biSizeImage = 0;
		lpbiNew->biPlanes = 1;
		lpbiNew->biXPelsPerMeter = lpbiNew->biYPelsPerMeter = 0;
		Accum.GetResult(lpbiNew);
		GlobalUnlock(hMemNew);
		hMem = hMemNew;
	}
	else
		hMem = Work.ScanNative(p, nMode!=0);
	if (!hMem) return false;
	if (bForInput)
		m_hMem = hMem;
	else
		GlobalFree(hMem);

	return true;
}

void CDriverTwain::OnSetValue(int nDev, LPCTSTR lpName, VARIANT var)
{
	CDriverBase::OnSetValue(nDev, lpName, var);
}

VARIANT CDriverTwain::OnGetValue(int nDev, LPCTSTR lpName)
{
	return CDriverBase::OnGetValue(nDev, lpName);
}

void CDriverTwain::InitDSM()
{
	sptrIApplication sptrA(GetAppUnknown());
	HWND hwnd;
	sptrA->GetMainWindowHandle(&hwnd);
	g_DataSrcMgr.SetParent(hwnd);
	g_DataSrcMgr.Open();
	CTwainDataSource *pDataSource = g_DataSrcMgr.FirstDataSource();
	while (pDataSource)
	{
		pDataSource = g_DataSrcMgr.NextDataSource();
	}
	m_bDSMInited = true;
}

int CDriverTwain::OnGetDevicesCount()
{
	if (!m_bDSMInited) InitDSM();
	return g_DataSrcMgr.m_DataSources.GetSize();
}

void CDriverTwain::OnGetDeviceNames(int nDev, CString &sShortName, CString &sLongName, CString &sCat)
{
	if (!m_bDSMInited) InitDSM();
	CTwainDataSource *p = g_DataSrcMgr.m_DataSources.GetAt(nDev);
	sShortName = p->GetIdentity().ProductName;
	sLongName = p->GetIdentity().ProductName;
	sCat = m_strCategory;
}



