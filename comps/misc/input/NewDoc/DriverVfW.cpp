// DriverVfW.cpp: implementation of the CDriverVfW class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "newdoc.h"
#include "DriverVfW.h"
#include "InputDlg.h"
#include "VideoDlg.h"
#include "VfWProfile.h"
#include "InputUtils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDriverVfW, CDriverBase)

CDriverVfW::CDriverVfW()
{
	EnableAutomation();
	_OleLockApp( this );
	RebuildArray();
	m_dwFlags = FG_SUPPORTSVIDEO|FG_ACCUMULATION|FG_INPUTFRAME;
	m_strCategory.LoadString(IDS_INPUT);
	m_strDriverName = _T("vfw");
	m_lpbi = NULL;
}

CDriverVfW::~CDriverVfW()
{
	if (m_lpbi) free(m_lpbi);
	_OleUnlockApp( this );
}


BEGIN_MESSAGE_MAP(CDriverVfW, CDriverBase)
	//{{AFX_MSG_MAP(CDriverVfW)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CDriverVfW, CDriverBase)
	//{{AFX_DISPATCH_MAP(CDriverVfW)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IDriverVfW to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {DA35C789-2FAA-48C4-A351-631390E24462}
static const IID IID_IDriverVfW = 
{ 0xda35c789, 0x2faa, 0x48c4, { 0xa3, 0x51, 0x63, 0x13, 0x90, 0xe2, 0x44, 0x62 } };

BEGIN_INTERFACE_MAP(CDriverVfW, CDriverBase)
	INTERFACE_PART(CDriverVfW, IID_IDriverVfW, Dispatch)
	INTERFACE_PART(CDriverVfW, IID_IInputWindow, InpWnd)
END_INTERFACE_MAP()

// {A0654EF5-94FD-43A7-B3C2-9A27FBB5A910}
GUARD_IMPLEMENT_OLECREATE(CDriverVfW, "IDriver.DriverVfW", 0xa0654ef5, 0x94fd, 0x43a7, 0xb3, 0xc2, 0x9a, 0x27, 0xfb, 0xb5, 0xa9, 0x10);

void CDriverVfW::RebuildArray()
{
	m_arrDevices.RemoveAll();
	char szDrvName[100], szDrvVer[20];
	for (int i=0; i<10; i++)
	{
		if (capGetDriverDescription(i, szDrvName, 79, szDrvVer, 19))
		{
			CVfwDeviceDescr dev;
			dev.m_sDrvName = szDrvName;
			dev.m_sDrvVer = szDrvVer;
			m_arrDevices.Add(dev);
		}
	}

}

int CDriverVfW::OnGetDevicesCount()
{
	return (int)m_arrDevices.GetSize();
}

void CDriverVfW::OnGetDeviceNames(int nDev, CString &sShortName, CString &sLongName, CString &sCat)
{
	sShortName = m_arrDevices[nDev].m_sDrvName;
	sLongName = m_arrDevices[nDev].m_sDrvName;
	sCat = m_strCategory;
}

void CDriverVfW::OnInputImage(IUnknown *pUnk, IUnknown *punkTarget, int nDevice, BOOL bFromSettings)
{
	CImageWrp imageNew(pUnk);
	if (m_lpbi/*bFromSettings*/)
	{
		imageNew.AttachDIBBits(m_lpbi);
		free(m_lpbi);
		m_lpbi = NULL;
	}
}

bool CDriverVfW::OnInputVideoFile(CString &sVideoFileName, int nDevice, int nMode)
{
	int r = VCInput(NULL, NULL, nDevice, m_arrDevices[nDevice].m_sDrvName, nMode == 0, TRUE, FALSE, TRUE);
	if (r != IDOK) return false;
	sVideoFileName = g_VfWProfile._GetProfileString(_T("VideoDlg"), _T("FileName"), "");
	return true;
}

bool CDriverVfW::OnExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput)
{
	LPBITMAPINFOHEADER lpbi;
	if (nMode == 0 && !bForInput)
		return true;
	int r = VCInput(hwndParent, &lpbi, nDevice, m_arrDevices[nDevice].m_sDrvName, nMode == 0, TRUE, bForInput);
	if (bForInput)
		m_lpbi = lpbi;
	return r == IDOK;
}

int CDriverVfW::VCInput(HWND hWndParent, LPBITMAPINFOHEADER *plpbi, int nDevice, const char *pszDescr, BOOL bNoModal,
	BOOL bClose, BOOL bSaveImage, BOOL bForAVI)
{
	int r = IDOK;
	LPBITMAPINFOHEADER lpbi = NULL;
	HWND hwndPrev = GetForegroundWindow();
	::EnableWindow(hWndParent, FALSE);
	if (!CInputDlg::m_pInputDlg)
	{
		CInputDlg::m_pInputDlg = new CInputDlg(nDevice,pszDescr);
		if (bForAVI)
			CInputDlg::m_pInputDlg->m_DialogPurpose = CInputDlg::ForAVI;
		CInputDlg::m_pInputDlg->Create(IDD_INPUT,CWnd::FromHandle(hWndParent));
	}
	else if (nDevice != CInputDlg::m_pInputDlg->GetDrvNum())
	{
		CInputDlg::m_pInputDlg->DestroyWindow();
		CInputDlg::m_pInputDlg = new CInputDlg(nDevice,pszDescr);
		if (bForAVI)
			CInputDlg::m_pInputDlg->m_DialogPurpose = CInputDlg::ForAVI;
		CInputDlg::m_pInputDlg->Create(IDD_INPUT,CWnd::FromHandle(hWndParent));
	}
	else
		CInputDlg::m_pInputDlg->SetForegroundWindow();
	if (!bNoModal)
	{
		CInputDlg::m_pInputDlg->BeginModalState();
		MSG msg;
		while (GetMessage(&msg, NULL/*m_pInputDlg->m_hWnd*/, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (CInputDlg::m_pInputDlg && CInputDlg::m_pInputDlg->m_hWnd)
		{
			if (bSaveImage)
				lpbi = CInputDlg::m_pInputDlg->SaveImage();
			if (bForAVI && CInputDlg::m_pInputDlg->m_strAVIName.IsEmpty())
				r = IDCANCEL;
			CInputDlg::m_pInputDlg->EndModalState();
			if (bClose)
				CInputDlg::m_pInputDlg->DestroyWindow();
		}
		else 
			r = IDCANCEL;
	}
	else if (bSaveImage)
	{
		// No modal state by default
		CInputDlg::m_pInputDlg->DoSave(); // Prepare image
		lpbi = CInputDlg::m_pInputDlg->SaveImage(); // Save it
		if (bClose)
			CInputDlg::m_pInputDlg->DestroyWindow();
	}
	::EnableWindow(hWndParent, TRUE);
	::SetForegroundWindow(hwndPrev);
	if (plpbi)
		*plpbi = lpbi;
	return r;
}

void CDriverVfW::CancelDlg()
{
	if (CInputDlg::m_pInputDlg)
		CInputDlg::m_pInputDlg->DestroyWindow();
	CInputDlg::m_pInputDlg = NULL;
}

HWND CDriverVfW::OnCreateWindow(int nDevice, HWND hwndParent)
{
	RECT r;
	::GetClientRect(hwndParent, &r);
	HWND hwnd = capCreateCaptureWindow(NULL, WS_CHILD|WS_VISIBLE, r.left, r.top, r.right-r.left, r.bottom-r.top,
		hwndParent, 1995);
	if (!capDriverConnect(hwnd, nDevice))
		AfxMessageBox( "Cannot connect to video capturing device" );
	capPreviewRate(hwnd, 33);     // Set the preview rate to 33 milliseconds
	CAPDRIVERCAPS CapDriverCaps;
    if (!capDriverGetCaps(hwnd, &CapDriverCaps, sizeof(CapDriverCaps)))
        memset(&CapDriverCaps, 0, sizeof(CapDriverCaps));
	if (CapDriverCaps.fHasOverlay && g_VfWProfile._GetProfileInt(_T("Settings"),_T("Overlay"),TRUE))  // Enable overlay if the card supports it
		capOverlay(hwnd, TRUE);
	else
		capPreview(hwnd, TRUE);
	return hwnd;
}

void CDriverVfW::OnDestroyWindow(HWND hwnd)
{
	::DestroyWindow(hwnd);
}

void CDriverVfW::OnSize(HWND hwndCap, HWND hwndParent, SIZE sz)
{
	RECT r;
	::GetClientRect(hwndParent, &r);
	::MoveWindow(hwndCap, r.left, r.top, r.right-r.left, r.bottom-r.top, TRUE);
}

HWND CDriverVfW::FindHwndForDevice(int nDev)
{
	HWND hwnd = NULL;
	for (int i = 0; i < m_arr.GetSize(); i++)
	{
		if (m_arrDev[i] == nDev)
			hwnd = m_arr[i];
	}
	return hwnd;
}

void CDriverVfW::OnExecuteDriverDialog(int nDev, LPCTSTR lpstrName)
{
	HWND hwnd = FindHwndForDevice(nDev);
	if (hwnd)
	{
		if (!_tcsicmp(lpstrName, _T("Source")))
			capDlgVideoFormat(hwnd);
		else if (!_tcsicmp(lpstrName, _T("Format")))
			capDlgVideoSource(hwnd);
		else if (!_tcsicmp(lpstrName, _T("Display")))
			capDlgVideoDisplay(hwnd);
		else if (!_tcsicmp(lpstrName, _T("VideoSettings")))
		{
			CAPTUREPARMS cp;
			BOOL bOK = capCaptureGetSetup(hwnd, &cp, sizeof(CAPTUREPARMS));
			char szFile[255];
			capFileGetCaptureFile(hwnd, szFile, 255);
			CVideoDlg dlg(&g_VfWProfile, NULL);
			dlg.m_strFile = szFile;
			if (bOK)
			{
				dlg.m_iFPS = 1000000L/cp.dwRequestMicroSecPerFrame;
				dlg.m_iTime = cp.wTimeLimit;
			}
			dlg.DoModal();
		}
	}
}

void CDriverVfW::OnSetValue(int nDev, LPCTSTR lpName, VARIANT var)
{
	CString sSec,sEntry;
	_SectionAndEntry(lpName, sSec, sEntry);
	g_VfWProfile.WriteProfileValue(sSec, sEntry, var);
	if (sSec == _T("Settings") && sEntry == _T("Overlay") && var.vt == VT_I4)
	{
		HWND hwnd = FindHwndForDevice(nDev);
		capOverlay(hwnd, var.lVal);
		capPreview(hwnd, !var.lVal);
	}
}

VARIANT CDriverVfW::OnGetValue(int nDev, LPCTSTR lpName)
{
	VARIANT varEmpty;
	varEmpty.vt = VT_EMPTY;
	if (!_tcsicmp(lpName, _T("CanOverlay")))
	{
		HWND hwnd = FindHwndForDevice(nDev);
		if (hwnd)
		{
			CAPDRIVERCAPS CapDriverCaps;
			if (!capDriverGetCaps(hwnd, &CapDriverCaps, sizeof(CapDriverCaps)))
				memset(&CapDriverCaps, 0, sizeof(CapDriverCaps));
			VARIANT var;
			var.vt = VT_I4;
			var.lVal = CapDriverCaps.fHasOverlay;
			return var;
		}
		return varEmpty;
	}
	else if (!_tcsicmp(lpName, _T("HasSourceDlg")) || !_tcsicmp(lpName, _T("IsSource")))
	{
		HWND hwnd = FindHwndForDevice(nDev);
		if (hwnd)
		{
			CAPDRIVERCAPS CapDriverCaps;
			if (!capDriverGetCaps(hwnd, &CapDriverCaps, sizeof(CapDriverCaps)))
				memset(&CapDriverCaps, 0, sizeof(CapDriverCaps));
			VARIANT var;
			var.vt = VT_I4;
			var.lVal = CapDriverCaps.fHasDlgVideoSource;
			return var;
		}
		return varEmpty;
	}
	else if (!_tcsicmp(lpName, _T("HasFormatDlg")) || !_tcsicmp(lpName, _T("IsFormatDlg")))
	{
		HWND hwnd = FindHwndForDevice(nDev);
		if (hwnd)
		{
			CAPDRIVERCAPS CapDriverCaps;
			if (!capDriverGetCaps(hwnd, &CapDriverCaps, sizeof(CapDriverCaps)))
				memset(&CapDriverCaps, 0, sizeof(CapDriverCaps));
			VARIANT var;
			var.vt = VT_I4;
			var.lVal = CapDriverCaps.fHasDlgVideoFormat;
			return var;
		}
		return varEmpty;
	}
	else if (!_tcsicmp(lpName, _T("HasDisplayDlg")) || !_tcsicmp(lpName, _T("IsDisplay")))
	{
		HWND hwnd = FindHwndForDevice(nDev);
		if (hwnd)
		{
			CAPDRIVERCAPS CapDriverCaps;
			if (!capDriverGetCaps(hwnd, &CapDriverCaps, sizeof(CapDriverCaps)))
				memset(&CapDriverCaps, 0, sizeof(CapDriverCaps));
			VARIANT var;
			var.vt = VT_I4;
			var.lVal = CapDriverCaps.fHasDlgVideoDisplay;
			return var;
		}
		return varEmpty;
	}
	else if (!_tcsicmp(lpName, _T("IsOverlay")))
	{
		HWND hwnd = FindHwndForDevice(nDev);
		if (hwnd)
		{
			CAPSTATUS CapStatus;
			capGetStatus(hwnd, &CapStatus, sizeof(CAPSTATUS));
			VARIANT var;
			var.vt = VT_I4;
			var.lVal = CapStatus.fOverlayWindow;
			return var;
		}
		return varEmpty;
	}
	else if (!_tcsicmp(lpName, _T("IsPreview")))
	{
		HWND hwnd = FindHwndForDevice(nDev);
		if (hwnd)
		{
			CAPSTATUS CapStatus;
			capGetStatus(hwnd, &CapStatus, sizeof(CAPSTATUS));
			VARIANT var;
			var.vt = VT_I4;
			var.lVal = CapStatus.fLiveWindow;
			return var;
		}
		return varEmpty;
	}
	else if (!_tcsnicmp(lpName, _T("Is"), 2))
	{
		VARIANT var;
		var.vt = VT_I4;
		var.lVal = 0;
		return var;
	}
	else
	{
		CString sSec,sEntry;
		_SectionAndEntry(lpName, sSec, sEntry);
		return g_VfWProfile.GetProfileValue(sSec, sEntry);
	}
//	return CDriverBase::OnGetValue(nDev, lpName);
}

