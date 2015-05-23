// DriverScionFG.cpp: implementation of the CDriverScion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScionFG.h"
#include "DriverScionFG.h"
#include "PreviewDialog.h"
#include "Image5.h"
#include <vfw.h>
//#include "debug.h"
//#include "Settings.h"
#include "Vt5Profile.h"
#include "core5.h"
#include "misc_utils.h"
#include "SettingsDialog.h"
//#include <math.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//IMPLEMENT_DYNCREATE(CDriverScion, CDriverBase)

CArray<IDriverSite *,IDriverSite *&> CDriverScion::m_arrSites;
CRITICAL_SECTION CDriverScion::m_csSites;
bool CDriverScion::m_bCSSitesInited = false;
static DWORD s_dwPeriod;

void InitSettings()
{
}

CDriverScion::CDriverScion()
{
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	AfxOleLockApp();
	m_nGrabCount = 0;
	if (!m_bCSSitesInited)
	{
		m_bCSSitesInited = true;
		InitializeCriticalSection(&m_csSites);
	}
}

CDriverScion::~CDriverScion()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	AfxOleUnlockApp();
}

IUnknown *CDriverScion::DoGetInterface( const IID &iid )
{
	if (iid == IID_IDriver)
		return (IDriver *)this;
	else if (iid == IID_IDriver2)
		return (IDriver2 *)this;
	else if (iid == IID_IDriver3)
		return (IDriver3 *)this;
	else if (iid == IID_IDriver4)
		return (IDriver4 *)this;
	else if (iid == IID_IInputPreview2)
		return (IInputPreview2 *)this;
	else if (iid == IID_IInputPreview3)
		return (IInputPreview3 *)this;
	else if (iid == IID_IInputPreview)
		return (IInputPreview *)this;
	else
		return ComObjectBase::DoGetInterface(iid);
}

HRESULT CDriverScion::BeginPreview(int nDev, IInputPreviewSite *pSite)
{
	HRESULT hr = StartImageAquisition(0,TRUE);
	if (FAILED(hr)) return hr;
	return AddDriverSite(0,pSite);
}

HRESULT CDriverScion::EndPreview(int nDev, IInputPreviewSite *pSite)
{
	RemoveDriverSite(0,pSite);
	return StartImageAquisition(0,FALSE);
}

HRESULT CDriverScion::GetSize(int nDev, short *pdx, short *pdy)
{
	*pdx = 768;
	*pdy = 576;
	return S_OK;
}

HRESULT CDriverScion::GetPreviewFreq(int nDev, DWORD *pdwFreq)
{
	*pdwFreq = 15;
	return S_OK;
}


HRESULT CDriverScion::DrawRect(int nDev, HDC hDC, LPRECT prcSrc, LPRECT prcDst)
{
	bool bDraw = false;
/*	if (g_camera && m_DrawData.pData)
	{
		__EnterCriticalSection(&m_csSites);
		if (prcSrc->bottom-prcSrc->top == prcDst->bottom-prcDst->top &&
			prcSrc->right-prcSrc->left == prcDst->right-prcDst->left)
		{
			int nWidth = prcDst->right-prcDst->left;
			int nHeight = prcDst->bottom-prcDst->top;
			SetDIBitsToDevice(hDC, prcDst->left, prcDst->top, nWidth, nHeight,
				prcSrc->left,m_bi.bmiHeader.biHeight-prcSrc->top-nHeight-1,0,
				m_bi.bmiHeader.biHeight,m_DrawData.pData,(LPBITMAPINFO)&m_bi,
				DIB_RGB_COLORS);
		}
		else
			DrawDibDraw(dd.hdd, hDC, prcDst->left, prcDst->top, prcDst->right-prcDst->left, prcDst->bottom-prcDst->top,
				&m_bi.bmiHeader, m_DrawData.pData, prcSrc->left,prcSrc->top,prcSrc->right-prcSrc->left,prcSrc->bottom-prcSrc->top, 0);
		__LeaveCriticalSection(&m_csSites);
		bDraw = true;
	}*/
	if (!bDraw)
	{
		HBRUSH br = ::CreateSolidBrush(RGB(0,255,64));
		FillRect(hDC, prcDst, br);
		DeleteObject(br);
	}
	return S_OK;
}

HRESULT CDriverScion::GetPeriod(int nDevice, int nMode, DWORD *pdwPeriod)
{
	*pdwPeriod = s_dwPeriod;
	return S_OK;
}

HRESULT CDriverScion::GetPreviewMode(int nDevice, int *pnMode)
{
	*pnMode = 0;
	return S_OK;
}

HRESULT CDriverScion::SetPreviewMode(int nDevice, int nMode)
{
	return S_OK;
}


HRESULT CDriverScion::SetValue(int nCurDev, BSTR bstrName, VARIANT Value)
{
	CString s(bstrName);
	((CVT5ProfileManager*)CStdProfileManager::m_pMgr)->WriteProfileValue(NULL, s, Value);

	return S_OK;
}

HRESULT CDriverScion::GetValue(int nCurDev, BSTR bstrName, VARIANT *pValue)
{
	CString s(bstrName);
	*pValue = ((CVT5ProfileManager*)CStdProfileManager::m_pMgr)->GetProfileValue(NULL, s);
	return S_OK;
}

HRESULT CDriverScion::InputNative(IUnknown *pUnkImg, IUnknown *punkTarget, int nDevice, BOOL bUseSettings)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IImage3Ptr sptrImg(pUnkImg);
	return S_OK;
}

HRESULT CDriverScion::InputDIB(IDIBProvider *pDIBPrv, IUnknown *punkTarget, int nDevice, BOOL bFromSettings)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CDriverScion::InputVideoFile(BSTR *pbstrVideoFile, int nDevice, int nMode)
{
	return E_NOTIMPL;
}

HRESULT CDriverScion::ExecuteSettings(HWND hwndParent, IUnknown *punkTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (nMode == 0)
		return S_OK;
	else
	{
		BOOL bRight = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("RightControls"), FALSE, true, true);
		CPreviewDialog dlg((IInputPreview *)this,nDevice,bRight?true:false);
		return dlg.DoModal() == IDOK ? S_OK : S_FALSE;
	}
}

HRESULT CDriverScion::GetFlags(DWORD *pdwFlags)
{
	*pdwFlags = FG_ACCUMULATION|FG_INPUTFRAME;
	return S_OK;
}

HRESULT CDriverScion::GetShortName(BSTR *pbstrShortName)
{
	CString strDriverName("Scion FG");
	*pbstrShortName = strDriverName.AllocSysString();
	return S_OK;
}

HRESULT CDriverScion::GetLongName(BSTR *pbstrLongName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strDriverFullName;
	strDriverFullName.LoadString(IDS_SCIONFG);
	*pbstrLongName = strDriverFullName.AllocSysString();
	return S_OK;
}


HRESULT CDriverScion::ExecuteDriverDialog(int nDev, LPCTSTR lpstrName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
//	CSettingsDialog dlg((IInputPreview *)this,nDev);
	CDialog dlg(IDD_SETTINGS);
	return dlg.DoModal() == IDOK ? S_OK : S_FALSE;
}

HRESULT CDriverScion::GetDevicesCount(int *pnCount)
{
	*pnCount = 1;
	return S_OK;
}

HRESULT CDriverScion::GetDeviceNames(int nDev, BSTR *pbstrShortName, BSTR *pbstrLongName, BSTR *pbstrCategory)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (nDev != 0) return E_INVALIDARG;
	CString strDriverName("ScionFG");
	CString strDriverFullName,strCategory;
	strDriverFullName.LoadString(IDS_SCIONFG);
	strCategory.LoadString(IDS_INPUT);
	*pbstrShortName = strDriverName.AllocSysString();
	*pbstrLongName = strDriverFullName.AllocSysString();
	*pbstrCategory = strCategory.AllocSysString();
	return S_OK;
}



HRESULT CDriverScion::GetImage(int nCurDev, LPVOID *lplpData, DWORD *pdwSize)
{
	*lplpData = 0;
	*pdwSize = 0;
	return S_OK;
}

HRESULT CDriverScion::ConvertToNative(int nCurDev, LPVOID lpData, DWORD dwSize, IUnknown *punkImg)
{
	return S_OK;
}

HRESULT CDriverScion::GetTriggerMode(int nCurDev, BOOL *pgTriggerMode)
{
	return S_OK;
}

HRESULT CDriverScion::SetTriggerMode(int nCurDev, BOOL bSet)
{
	return S_OK;
}

HRESULT CDriverScion::DoSoftwareTrigger(int nCurDev)
{
	return S_OK;
}

HRESULT CDriverScion::StartImageAquisition(int nCurDev, BOOL bStart)
{
	if (bStart)
	{
		m_nGrabCount++;
		if (m_nGrabCount==1)
		{
		}
	}
	else
	{
		if (--m_nGrabCount == 0)
		{
		}
	}
	return S_OK;
}

HRESULT CDriverScion::IsImageAquisitionStarted(int nCurDev, BOOL *pbAquisition)
{
	*pbAquisition = m_nGrabCount > 0;
	return S_OK;
}

HRESULT CDriverScion::AddDriverSite(int nCurDev, IDriverSite *pSite)
{
	bool bFound = false;
	for (int i = 0; i < m_arrSites.GetSize(); i++)
	{
		if (m_arrSites[i] == pSite)
		{
			bFound = true;
			break;
		}
	}
	if (!bFound)
	{
		pSite->AddRef();
		m_arrSites.Add(pSite);
	}
	return S_OK;
}

HRESULT CDriverScion::RemoveDriverSite(int nCurDev, IDriverSite *pSite)
{
	for (int i = 0; i < m_arrSites.GetSize(); i++)
	{
		if (m_arrSites[i] == pSite)
		{
			m_arrSites.RemoveAt(i);
			pSite->Release();
		}
	}
	return S_OK;
}

HRESULT CDriverScion::SetConfiguration(BSTR bstrName)
{
	CString s1(CStdProfileManager::m_pMgr->GetMethodic());
	HRESULT hr = CDriver::SetConfiguration(bstrName);
	CString s2(CStdProfileManager::m_pMgr->GetMethodic());
	if (SUCCEEDED(hr) && s1 != s2)
		InitSettings();
	return hr;
}


HRESULT CDriverScion::SetValueInt(int nCurDev, BSTR bstrSec, BSTR bstrEntry, int nValue, UINT nFlags)
{
	CStdProfileManager::m_pMgr->WriteProfileInt(_bstr_t(bstrSec), _bstr_t(bstrEntry), nValue);
	return S_OK;
}

HRESULT CDriverScion::GetValueInt(int nCurDev, BSTR bstrSec, BSTR bstrEntry, int *pnValue, UINT nFlags)
{
	*pnValue = CStdProfileManager::m_pMgr->_GetProfileInt(_bstr_t(bstrSec), _bstr_t(bstrEntry), *pnValue);
	return S_OK;
}




