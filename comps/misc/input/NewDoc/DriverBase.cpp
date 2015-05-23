// DriverBase.cpp : implementation file
//

#include "stdafx.h"
#include "DriverBase.h"
#include "NewDocStdProfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CEntry::CEntry(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, int nValue, BOOL bIgnoreMethodic)
{
	m_nValueType = VT_I4;
	m_nCurDev = nCurDev;
	m_sSection = sSection;
	m_sEntry = sEntry;
	m_nValue = nValue;
	m_bIgnoreMethodic = bIgnoreMethodic;
}

CEntry::CEntry(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, CString strValue, BOOL bIgnoreMethodic)
{
	m_nValueType = VT_BSTR;
	m_nCurDev = nCurDev;
	m_sSection = sSection;
	m_sEntry = sEntry;
	m_strValue = strValue;
	m_bIgnoreMethodic = bIgnoreMethodic;
}

CEntry::CEntry(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, double dValue, BOOL bIgnoreMethodic)
{
	m_nValueType = VT_R8;
	m_nCurDev = nCurDev;
	m_sSection = sSection;
	m_sEntry = sEntry;
	m_dValue = dValue;
	m_bIgnoreMethodic = bIgnoreMethodic;
}

void CEntryArray::DeInit()
{
	for (int i = GetSize()-1; i>= 0; i--)
	{
		CEntry *p = GetAt(i);
		delete p;
	}
	RemoveAll();
}

CInputPreviewImpl::CInputPreviewImpl()
{
}

CInputPreviewImpl::~CInputPreviewImpl()
{
}

void CInputPreviewImpl::ChangeSize()
{
	for (int i = 0; i < m_arrSites.GetSize(); i++)
		m_arrSites.GetAt(i)->OnChangeSize();
}

void CInputPreviewImpl::Invalidate()
{
	for (int i = 0; i < m_arrSites.GetSize(); i++)
		m_arrSites.GetAt(i)->Invalidate();
}


bool CInputPreviewImpl::OnBeginPreview(IInputPreviewSite *pSite)
{
	for (int i = 0; i < m_arrSites.GetSize(); i++)
	{
		if (m_arrSites[i] == pSite)
			return false;
	}
	pSite->AddRef();
	m_arrSites.Add(pSite);
	return true;
}

void CInputPreviewImpl::OnEndPreview(IInputPreviewSite *pSite)
{
	for (int i = 0; i < m_arrSites.GetSize(); i++)
	{
		if (m_arrSites[i] == pSite)
		{
			pSite->Release();
			m_arrSites.RemoveAt(i);
		}
	}
}


/*
void CInputPreviewImpl::OnSetPreviewSite(IInputPreviewSite *pSite)
{
	if (m_pSite)
		m_pSite->Release();
	m_pSite = pSite;
	if (m_pSite)
		m_pSite->AddRef();
}
*/

IMPLEMENT_UNKNOWN_BASE(CInputPreviewImpl, InpPrv);

HRESULT CInputPreviewImpl::XInpPrv::BeginPreview(IInputPreviewSite *pSite)
{
	_try_nested_base(CInputPreviewImpl, InpPrv, BeginPreview)
	{
		pThis->OnBeginPreview(pSite);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CInputPreviewImpl::XInpPrv::EndPreview(IInputPreviewSite *pSite)
{
	_try_nested_base(CInputPreviewImpl, InpPrv, EndPreview)
	{
		pThis->OnEndPreview(pSite);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CInputPreviewImpl::XInpPrv::GetPreviewFreq(DWORD *pdwFreq)
{
	_try_nested_base(CInputPreviewImpl, InpPrv, GetPreviewFreq)
	{
		pThis->OnGetPreviewFreq(*pdwFreq);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CInputPreviewImpl::XInpPrv::GetSize(short *pdx, short *pdy)
{
	_try_nested_base(CInputPreviewImpl, InpPrv, GetSize)
	{
		pThis->OnGetSize(*pdx, *pdy);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CInputPreviewImpl::XInpPrv::DrawRect(HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst)
{
	_try_nested_base(CInputPreviewImpl, InpPrv, DrawRect)
	{
		pThis->OnDrawRect(hDC, lpRectSrc, lpRectDst);
		return S_OK;
	}
	_catch_nested;
}

/*
HRESULT CInputPreviewImpl::XInpPrv::SetPreviewSite(IInputPreviewSite *pSite)
{
	_try_nested_base(CInputPreviewImpl, InpPrv, SetPreviewSite)
	{
		pThis->OnSetPreviewSite(pSite);
		return S_OK;
	}
	_catch_nested;
}
*/

CInputWindowImpl::CInputWindowImpl()
{
}

CInputWindowImpl::~CInputWindowImpl()
{
}

IMPLEMENT_UNKNOWN_BASE(CInputWindowImpl, InpWnd);

HRESULT CInputWindowImpl::XInpWnd::CreateInputWindow(int nDevice, HWND hwndParent, long *plCap)
{
	_try_nested_base(CInputWindowImpl, InpWnd, CreateInputWindow)
	{
		HWND hwnd = pThis->OnCreateWindow(nDevice,hwndParent);
		if (hwnd)
		{
			pThis->m_arr.Add(hwnd);
			pThis->m_arrDev.Add((UINT)nDevice);
		}
		if (plCap) *plCap = (long)hwnd;
		return hwnd?S_OK:E_UNEXPECTED;
	}
	_catch_nested;
}

HRESULT CInputWindowImpl::XInpWnd::DestroyInputWindow(long lCap)
{
	_try_nested_base(CInputWindowImpl, InpWnd, DestroyInputWindow)
	{
		for(int i = 0; i < pThis->m_arr.GetSize(); i++)
		{
			if (pThis->m_arr[i] == (HWND)lCap)
			{
				pThis->m_arr.RemoveAt(i);
				pThis->m_arrDev.RemoveAt(i);
			}
		}
		pThis->OnDestroyWindow((HWND)lCap);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CInputWindowImpl::XInpWnd::OnSize(long lCap, HWND hwndParent, SIZE sz)
{
	_try_nested_base(CInputWindowImpl, InpWnd, OnSize)
	{
		pThis->OnSize((HWND)lCap, hwndParent, sz);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CInputWindowImpl::XInpWnd::GetSize(long lCap, short *pdx, short *pdy)
{
	_try_nested_base(CInputWindowImpl, InpWnd, GetSize)
	{
		pThis->OnGetSize((HWND)lCap, pdx, pdy);
		return S_OK;
	}
	_catch_nested;
}

IMPLEMENT_UNKNOWN_BASE(CInputWndOverlayImpl, InpWOvr);

HRESULT CInputWndOverlayImpl::XInpWOvr::InitOverlay()
{
	_try_nested_base(CInputWndOverlayImpl, InpWOvr, InitOverlay)
	{
		return pThis->OnInitOverlay()?S_OK:S_FALSE;
	}
	_catch_nested;
}

HRESULT CInputWndOverlayImpl::XInpWOvr::DeinitOverlay()
{
	_try_nested_base(CInputWndOverlayImpl, InpWOvr, DeinitOverlay)
	{
		pThis->OnDeinitOverlay();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CInputWndOverlayImpl::XInpWOvr::GetSize(short *pdx, short *pdy)
{
	_try_nested_base(CInputWndOverlayImpl, InpWOvr, GetSize)
	{
		pThis->OnGetOvrSize(*pdx, *pdy);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CInputWndOverlayImpl::XInpWOvr::StartOverlay(HWND hwndDraw, LPRECT lprcSrc, LPRECT lprcDst)
{
	_try_nested_base(CInputWndOverlayImpl, InpWOvr, StartOverlay)
	{
		return pThis->OnStartOverlay(hwndDraw, lprcSrc, lprcDst)?S_OK:S_FALSE;
	}
	_catch_nested;
}

HRESULT CInputWndOverlayImpl::XInpWOvr::StopOverlay(HWND hwndDraw)
{
	_try_nested_base(CInputWndOverlayImpl, InpWOvr, StopOverlay)
	{
		pThis->OnStopOverlay(hwndDraw);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CInputWndOverlayImpl::XInpWOvr::SetDstRect(HWND hwndDraw, LPRECT lprcDst)
{
	_try_nested_base(CInputWndOverlayImpl, InpWOvr, SetDstRect)
	{
		pThis->OnSetDstRect(hwndDraw,lprcDst);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CInputWndOverlayImpl::XInpWOvr::SetSrcRect(HWND hwndDraw, LPRECT lprcSrc)
{
	_try_nested_base(CInputWndOverlayImpl, InpWOvr, SetSrcRect)
	{
		pThis->OnSetSrcRect(hwndDraw,lprcSrc);
		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
// CDriverBase

IMPLEMENT_DYNCREATE(CDriverBase, CCmdTargetEx)

CDriverBase::CDriverBase()
{
	_OleLockApp( this );
	m_strDriverName = _T("Driver");
	m_dwFlags = 0;
	m_strCategory = "CreationMethod";
	m_nPreviewMode = 0;
	m_pProfMgr = CStdProfileManager::m_pMgr;
}

CDriverBase::~CDriverBase()
{
	_OleUnlockApp( this );
}


BEGIN_MESSAGE_MAP(CDriverBase, CCmdTargetEx)
	//{{AFX_MSG_MAP(CDriverBase)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_INTERFACE_MAP(CDriverBase, CCmdTargetEx)
	INTERFACE_PART(CDriverBase, IID_IDriver, Drv)
	INTERFACE_PART(CDriverBase, IID_IDriver2, Drv)
	INTERFACE_PART(CDriverBase, IID_IDriver3, Drv)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CDriverBase, Drv)


HRESULT CDriverBase::XDrv::InputNative(IUnknown *pUnk, IUnknown *punkTarget, int nDevice, BOOL bDialog)
{
	_try_nested(CDriverBase, Drv, InputNative)
	{
		pThis->OnInputImage(pUnk, punkTarget, nDevice, bDialog);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDriverBase::XDrv::InputDIB(IDIBProvider *pDIBPrv, IUnknown *punkTarget, int nDevice, BOOL bFromSettings)
{
	_try_nested(CDriverBase, Drv, InputDIB)
	{
		return S_FALSE;
	}
	_catch_nested;
}

HRESULT CDriverBase::XDrv::InputVideoFile(BSTR *pbstrVideoFile, int nDevice, int nMode)
{
	_try_nested(CDriverBase, Drv, InputVideoFile)
	{
		if (!pbstrVideoFile) return E_INVALIDARG;
		CString sVideoFileName(*pbstrVideoFile);
		bool b = pThis->OnInputVideoFile(sVideoFileName, nDevice, nMode);
		if (!b)
		{
			if (*pbstrVideoFile) ::SysFreeString(*pbstrVideoFile);
			*pbstrVideoFile = NULL;
			return S_FALSE;
		}
		*pbstrVideoFile = sVideoFileName.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDriverBase::XDrv::ExecuteSettings(HWND hwndParent, IUnknown *punkTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput)
{
	_try_nested(CDriverBase, Drv, ExecuteSettings)
	{
		bool b = pThis->OnExecuteSettings(hwndParent, punkTarget, nDevice, nMode, bFirst, bForInput);
		return b?S_OK:S_FALSE;
	}
	_catch_nested;
}


HRESULT CDriverBase::XDrv::GetFlags(DWORD *pdwFlags)
{
	_try_nested(CDriverBase, Drv, GetFlags)
	{
		if (pdwFlags)
			*pdwFlags = pThis->m_dwFlags;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDriverBase::XDrv::GetShortName(BSTR *pbstrShortName)
{
	_try_nested(CDriverBase, Drv, GetShortName)
	{
		if (pbstrShortName)
			*pbstrShortName = pThis->m_strDriverName.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDriverBase::XDrv::GetLongName(BSTR *pbstrLongName)
{
	_try_nested(CDriverBase, Drv, GetLongName)
	{
		if (pbstrLongName)
		{
			if (pThis->m_strDriverFullName.IsEmpty())
				*pbstrLongName = pThis->m_strDriverName.AllocSysString();
			else
				*pbstrLongName = pThis->m_strDriverFullName.AllocSysString();
		}
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDriverBase::XDrv::GetValue(int nDev, BSTR bstrName, VARIANT *pValue)
{
	_try_nested(CDriverBase, Drv, GetValue)
	{
		CString sName(bstrName);
		if (pValue)
			*pValue = pThis->OnGetValue(nDev, sName);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDriverBase::XDrv::SetValue(int nDev, BSTR bstrName, VARIANT Value)
{
	_try_nested(CDriverBase, Drv, SetValue)
	{
		CString sName(bstrName);
		pThis->OnSetValue(nDev, sName, Value);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDriverBase::XDrv::ExecuteDriverDialog(int nDev, LPCTSTR lpstrName)
{
	_try_nested(CDriverBase, Drv, ExecuteDriverDialog)
	{
		pThis->OnExecuteDriverDialog(nDev, lpstrName);
		return S_OK;
	}
	_catch_nested;
}


HRESULT CDriverBase::XDrv::GetDevicesCount(int *pnCount)
{
	_try_nested(CDriverBase, Drv, GetDevicesCount)
	{
		if (pnCount)
			*pnCount = pThis->OnGetDevicesCount();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDriverBase::XDrv::GetDeviceNames(int nDev, BSTR *pbstrShortName, BSTR *pbstrLongName, BSTR *pbstrCategory)
{
	_try_nested(CDriverBase, Drv, GetDeviceNames)
	{
		CString s1,s2,s3;
		pThis->OnGetDeviceNames(nDev, s1, s2, s3);
		if (pbstrShortName)
			*pbstrShortName = s1.AllocSysString();
		if (pbstrLongName)
			*pbstrLongName = s2.AllocSysString();
		if (pbstrCategory)
			*pbstrCategory = s3.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDriverBase::XDrv::UpdateSizes()
{
	_try_nested(CDriverBase, Drv, UpdateSizes)
	{
		pThis->OnUpdateSizes();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDriverBase::XDrv::BeginPreviewMode()
{
	_try_nested(CDriverBase, Drv, BeginPreviewMode)
	{
		pThis->OnBeginPreviewMode();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDriverBase::XDrv::EndPreviewMode()
{
	_try_nested(CDriverBase, Drv, EndPreviewMode)
	{
		pThis->OnEndPreviewMode();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDriverBase::XDrv::CancelChanges()
{
	_try_nested(CDriverBase, Drv, CancelChanges)
	{
		pThis->OnCancelChanges();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDriverBase::XDrv::ApplyChanges()
{
	_try_nested(CDriverBase, Drv, ApplyChanges)
	{
		pThis->OnApplyChanges();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDriverBase::XDrv::GetValueInt(int nCurDev, BSTR bstrSection, BSTR bstrEntry, int *pnValue, UINT nFlags)
{
	_try_nested(CDriverBase, Drv, GetValueInt)
	{
		CString sSection(bstrSection);
		CString sEntry(bstrEntry);
		*pnValue = pThis->OnGetValueInt(nCurDev, sSection, sEntry, *pnValue, nFlags);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDriverBase::XDrv::SetValueInt(int nCurDev, BSTR bstrSection, BSTR bstrEntry, int nValue, UINT nFlags)
{
	_try_nested(CDriverBase, Drv, SetValueInt)
	{
		CString sSection(bstrSection);
		CString sEntry(bstrEntry);
		pThis->OnSetValueInt(nCurDev, sSection, sEntry, nValue, nFlags);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDriverBase::XDrv::GetValueString(int nCurDev, BSTR bstrSection, BSTR bstrEntry, BSTR *pbstrValue, UINT nFlags)
{
	_try_nested(CDriverBase, Drv, GetValueString)
	{
		CString sSection(bstrSection);
		CString sEntry(bstrEntry);
		CString sDef(*pbstrValue);
		::SysFreeString(*pbstrValue);
		CString s = pThis->OnGetValueString(nCurDev, sSection, sEntry, sDef, nFlags);
		*pbstrValue = s.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDriverBase::XDrv::SetValueString(int nCurDev, BSTR bstrSection, BSTR bstrEntry, BSTR bstrValue, UINT nFlags)
{
	_try_nested(CDriverBase, Drv, SetValueString)
	{
		CString sSection(bstrSection);
		CString sEntry(bstrEntry);
		CString sValue(bstrValue);
		pThis->OnSetValueString(nCurDev, sSection, sEntry, sValue, nFlags);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDriverBase::XDrv::GetValueDouble(int nCurDev, BSTR bstrSection, BSTR bstrEntry, double *pdValue, UINT nFlags)
{
	_try_nested(CDriverBase, Drv, GetValueDouble)
	{
		CString sSection(bstrSection);
		CString sEntry(bstrEntry);
		*pdValue = pThis->OnGetValueDouble(nCurDev, sSection, sEntry, *pdValue, nFlags);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDriverBase::XDrv::SetValueDouble(int nCurDev, BSTR bstrSection, BSTR bstrEntry, double dValue, UINT nFlags)
{
	_try_nested(CDriverBase, Drv, SetValueDouble)
	{
		CString sSection(bstrSection);
		CString sEntry(bstrEntry);
		pThis->OnSetValueDouble(nCurDev, sSection, sEntry, dValue, nFlags);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDriverBase::XDrv::ReinitPreview()
{
	_try_nested(CDriverBase, Drv, ReinitPreview)
	{
		pThis->ReinitPreview();
		return S_OK;
	}
	_catch_nested;
}

int CDriverBase::GetProfileInt(LPCTSTR lpKeyName, int nDefault, bool bWriteToProfile)
{
	sptrINamedData sptrND(GetAppUnknown());
	CString sSecName(_T("Input"));
	IUnknown *punkCM = _GetOtherComponent( GetAppUnknown(), IID_IDriverManager );
	if (punkCM)
	{
		sptrIDriverManager sptrDM(punkCM);
		punkCM->Release();
		BSTR bstr;
		sptrDM->GetSectionName(&bstr);
		sSecName = bstr;
		::SysFreeString(bstr);
	}
	COleVariant var;
	sSecName += CString("\\")+m_strDriverName;
	CString sKeyName(lpKeyName);
	sptrND->SetupSection(_bstr_t(sSecName));
	sptrND->GetValue(_bstr_t(sKeyName), &var);
	if (var.vt == VT_I4)
		return var.lVal;
	else if (var.vt == VT_I2)
		return var.iVal;
	else
	{
		if (bWriteToProfile)
		{
			COleVariant var1((long)nDefault);
			sptrND->SetValue(_bstr_t(sKeyName), var1);
		}
		return nDefault;
	}
}

CString CDriverBase::GetProfileString(LPCTSTR lpKeyName, LPCTSTR lpDefault)
{
	sptrINamedData sptrND(GetAppUnknown());
	CString sSecName(_T("Input"));
	IUnknown *punkCM = _GetOtherComponent( GetAppUnknown(), IID_IDriverManager );
	if (punkCM)
	{
		sptrIDriverManager sptrDM(punkCM);
		punkCM->Release();
		BSTR bstr;
		sptrDM->GetSectionName(&bstr);
		sSecName = bstr;
		::SysFreeString(bstr);
	}
	VARIANT var;
	::VariantInit(&var);
	sSecName += CString("\\")+m_strDriverName;
	CString sKeyName(lpKeyName);
	sptrND->SetupSection(_bstr_t(sSecName));
	sptrND->GetValue(_bstr_t(sKeyName), &var);
	if (var.vt == VT_BSTR)
	{
		CString s(var.bstrVal);
		return s;
	}
	else
		return CString(lpDefault);
}

void CDriverBase::OnSetValue(int nDev, LPCTSTR lpName, VARIANT var)
{
	sptrINamedData sptrND(GetAppUnknown());
	CString sSecName(_T("Input"));
	IUnknown *punkCM = _GetOtherComponent( GetAppUnknown(), IID_IDriverManager );
	if (punkCM)
	{
		sptrIDriverManager sptrDM(punkCM);
		punkCM->Release();
		BSTR bstr;
		sptrDM->GetSectionName(&bstr);
		sSecName = bstr;
		::SysFreeString(bstr);
	}
	sSecName += CString("\\")+m_strDriverName;
	CString sKeyName(lpName);
	sptrND->SetupSection(_bstr_t(sSecName));
	sptrND->SetValue(_bstr_t(sKeyName), var);
}

VARIANT CDriverBase::OnGetValue(int nDev, LPCTSTR lpName)
{
	if (_tcsnicmp(lpName, _T("Is"), 2) == 0)
	{
		VARIANT var;
		var.vt = VT_I4;
		var.lVal = 0;
		return var;
	}
	sptrINamedData sptrND(GetAppUnknown());
	CString sSecName(_T("Input"));
	IUnknown *punkCM = _GetOtherComponent( GetAppUnknown(), IID_IDriverManager );
	if (punkCM)
	{
		sptrIDriverManager sptrDM(punkCM);
		punkCM->Release();
		BSTR bstr;
		sptrDM->GetSectionName(&bstr);
		sSecName = bstr;
		::SysFreeString(bstr);
	}
	VARIANT var;
	::VariantInit(&var);
	sSecName += CString("\\")+m_strDriverName;
	CString sKeyName(lpName);
	sptrND->SetupSection(_bstr_t(sSecName));
	sptrND->GetValue(_bstr_t(sKeyName), &var);
	if (var.vt == VT_EMPTY)
		return OnGetDefaultValue(nDev, lpName);
	else
		return var;
}

VARIANT CDriverBase::OnGetDefaultValue(int nDev, LPCTSTR lpName)
{
	VARIANT var;
	::VariantInit(&var);
	return var;
}

int CDriverBase::OnGetDevicesCount()
{
	return 1;
}

void CDriverBase::OnGetDeviceNames(int nDev, CString &sShortName, CString &sLongName, CString &sCat)
{
	sShortName = m_strDriverName;
	if (m_strDriverFullName.IsEmpty())
		sLongName = m_strDriverName;
	else
		sLongName = m_strDriverFullName;
	sCat = m_strCategory;
}

void CDriverBase::OnBeginPreviewMode()
{
	if (m_nPreviewMode++ == 0)
		m_EntryArray.DeInit();
}

void CDriverBase::OnEndPreviewMode()
{
	if (--m_nPreviewMode == 0)
		m_EntryArray.DeInit();
}

void CDriverBase::OnApplyChanges()
{
	for (int i = 0; i < m_EntryArray.GetSize(); i++)
	{
		CEntry *p = m_EntryArray.GetAt(i);
		if (p->m_nValueType == VT_I4)
			DoSetValueInt(p->m_nCurDev, p->m_sSection, p->m_sEntry, p->m_nValue, p->m_bIgnoreMethodic);
		else if (p->m_nValueType == VT_BSTR)
			DoSetValueString(p->m_nCurDev, p->m_sSection, p->m_sEntry, p->m_strValue, p->m_bIgnoreMethodic);
		else if (p->m_nValueType == VT_R8)
			DoSetValueDouble(p->m_nCurDev, p->m_sSection, p->m_sEntry, p->m_dValue, p->m_bIgnoreMethodic);
	}
	m_EntryArray.DeInit();
	ReinitPreview();
}

void CDriverBase::OnCancelChanges()
{
	m_EntryArray.DeInit();
	ReinitPreview();
}

CEntry *CDriverBase::FindPreviewEntry(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, BOOL bIgnoreMethodic)
{
	CString sSec(sSection);
	CString sEnt(sEntry);
	for (int i = 0; i < m_EntryArray.GetSize(); i++)
	{
		CEntry *p = m_EntryArray.GetAt(i);
		if (p->m_nCurDev == nCurDev && p->m_sSection == sSec && p->m_sEntry == sEnt &&
			p->m_bIgnoreMethodic == bIgnoreMethodic)
		{
			return p;
		}
	}
	return NULL;
}


int  CDriverBase::OnGetValueInt(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, int nDef, UINT nFlags)
{
	BOOL bIgnoreMethodic = nFlags&ID2_IGNOREMETHODIC?TRUE:FALSE;
	BOOL bStore = nFlags&ID2_STORE?TRUE:FALSE;
	if (m_nPreviewMode)
	{
		CEntry *p = FindPreviewEntry(nCurDev, sSection, sEntry, bIgnoreMethodic);
		if (p && p->m_nValueType == VT_I4)
			return p->m_nValue;
	}
	int r = m_pProfMgr->GetProfileInt(sSection, sEntry, nDef, bStore?true:false, bIgnoreMethodic?true:false);
	if (r == nDef && (nFlags&ID2_BOARDIFDEFAULT))
		r = OnGetBoardInt(nCurDev, sSection, sEntry, nDef);
	return r;
}

void CDriverBase::OnSetValueInt(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, int nValue, UINT nFlags)
{
	BOOL bIgnoreMethodic = nFlags&ID2_IGNOREMETHODIC?TRUE:FALSE;
	if (m_nPreviewMode)
	{
		CEntry *p = FindPreviewEntry(nCurDev, sSection, sEntry, bIgnoreMethodic);
		if (p)
		{
			p->m_nCurDev = nCurDev;
			p->m_sSection = sSection;
			p->m_sEntry = sEntry;
			p->m_nValueType = VT_I4;
			p->m_nValue = nValue;
			p->m_bIgnoreMethodic = bIgnoreMethodic;
		}
		else
			m_EntryArray.Add(new CEntry(nCurDev, sSection, sEntry, nValue, bIgnoreMethodic));
		OnSetValueIntPrv(nCurDev, sSection, sEntry, nValue);
	}
	else
	{
		DoSetValueInt(nCurDev, sSection, sEntry, nValue, nFlags);
		OnSetValueIntPrv(nCurDev, sSection, sEntry, nValue);
	}
}

CString CDriverBase::OnGetValueString(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, LPCTSTR sDefault, UINT nFlags)
{
	BOOL bIgnoreMethodic = nFlags&ID2_IGNOREMETHODIC?TRUE:FALSE;
	BOOL bStore = nFlags&ID2_STORE?TRUE:FALSE;
	if (m_nPreviewMode)
	{
		CEntry *p = FindPreviewEntry(nCurDev, sSection, sEntry, bIgnoreMethodic);
		if (p && p->m_nValueType == VT_BSTR)
			return p->m_strValue;
	}
	CString s = m_pProfMgr->GetProfileString(sSection, sEntry, sDefault, bStore?true:false, bIgnoreMethodic?true:false);
	if (s == sDefault && (nFlags&ID2_BOARDIFDEFAULT))
		s = OnGetBoardString(nCurDev, sSection, sEntry, sDefault);
	return s;
}

void CDriverBase::OnSetValueString(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, LPCTSTR sValue, UINT nFlags)
{
	BOOL bIgnoreMethodic = nFlags&ID2_IGNOREMETHODIC?TRUE:FALSE;
	if (m_nPreviewMode)
	{
		CEntry *p = FindPreviewEntry(nCurDev, sSection, sEntry, bIgnoreMethodic);
		if (p)
		{
			p->m_nCurDev = nCurDev;
			p->m_sSection = sSection;
			p->m_sEntry = sEntry;
			p->m_nValueType = VT_BSTR;
			p->m_strValue = sValue;
			p->m_bIgnoreMethodic = bIgnoreMethodic;
		}
		else
			m_EntryArray.Add(new CEntry(nCurDev, sSection, sEntry, sValue, bIgnoreMethodic));
		OnSetValueStringPrv(nCurDev, sSection, sEntry, sValue);
	}
	else
	{
		DoSetValueString(nCurDev, sSection, sEntry, sValue, nFlags);
		OnSetValueStringPrv(nCurDev, sSection, sEntry, sValue);
	}
}

double CDriverBase::OnGetValueDouble(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, double dDefault, UINT nFlags)
{
	BOOL bIgnoreMethodic = nFlags&ID2_IGNOREMETHODIC?TRUE:FALSE;
	BOOL bStore = nFlags&ID2_STORE?TRUE:FALSE;
	if (m_nPreviewMode)
	{
		CEntry *p = FindPreviewEntry(nCurDev, sSection, sEntry, bIgnoreMethodic);
		if (p && p->m_nValueType == VT_R8)
			return p->m_dValue;
	}
	bool bDefUsed;
	double d = m_pProfMgr->GetProfileDouble(sSection, sEntry, dDefault, &bDefUsed, bStore?true:false, bIgnoreMethodic?true:false);
	if (d == dDefault && (nFlags&ID2_BOARDIFDEFAULT))
		d = OnGetBoardDouble(nCurDev, sSection, sEntry, dDefault);
	return d;
}

void CDriverBase::OnSetValueDouble(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, double dValue, UINT nFlags)
{
	BOOL bIgnoreMethodic = nFlags&ID2_IGNOREMETHODIC?TRUE:FALSE;
	if (m_nPreviewMode)
	{
		CEntry *p = FindPreviewEntry(nCurDev, sSection, sEntry, bIgnoreMethodic);
		if (p)
		{
			p->m_nCurDev = nCurDev;
			p->m_sSection = sSection;
			p->m_sEntry = sEntry;
			p->m_nValueType = VT_R8;
			p->m_dValue = dValue;
			p->m_bIgnoreMethodic = bIgnoreMethodic;
		}
		else
			m_EntryArray.Add(new CEntry(nCurDev, sSection, sEntry, dValue, bIgnoreMethodic));
		OnSetValueDoublePrv(nCurDev, sSection, sEntry, dValue);
	}
	else
	{
		DoSetValueDouble(nCurDev, sSection, sEntry, dValue, nFlags);
		OnSetValueDoublePrv(nCurDev, sSection, sEntry, dValue);
	}
}

void CDriverBase::DoSetValueInt(int nCurDev, LPCTSTR strSection, LPCTSTR strEntry, int nValue, UINT nFlags)
{
	BOOL bIgnoreMethodic = nFlags&ID2_IGNOREMETHODIC?TRUE:FALSE;
	m_pProfMgr->WriteProfileInt(strSection, strEntry, nValue, bIgnoreMethodic?true:false);
}

void CDriverBase::DoSetValueString(int nCurDev, LPCTSTR strSection, LPCTSTR strEntry, LPCTSTR strValue, UINT nFlags)
{
	BOOL bIgnoreMethodic = nFlags&ID2_IGNOREMETHODIC?TRUE:FALSE;
	m_pProfMgr->WriteProfileString(strSection, strEntry, strValue, bIgnoreMethodic?true:false);
}

void CDriverBase::DoSetValueDouble(int nCurDev, LPCTSTR strSection, LPCTSTR strEntry, double dValue, UINT nFlags)
{
	BOOL bIgnoreMethodic = nFlags&ID2_IGNOREMETHODIC?TRUE:FALSE;
	m_pProfMgr->WriteProfileDouble(strSection, strEntry, dValue, bIgnoreMethodic?true:false);
}


void CDriverBase::OnSetValueIntPrv(int nCurDev, LPCTSTR strSection, LPCTSTR strEntry, int nValue)
{
	ReinitPreview();
}

void CDriverBase::OnSetValueStringPrv(int nCurDev, LPCTSTR strSection, LPCTSTR sEntry, LPCTSTR sValue)
{
	ReinitPreview();
}

void CDriverBase::OnSetValueDoublePrv(int nCurDev, LPCTSTR strSection, LPCTSTR sEntry, double dValue)
{
	ReinitPreview();
}

HRESULT CDriverBase::XDrv::GetImage(int nCurDev, LPVOID *ppData, DWORD *pdwDataSize)
{
	METHOD_PROLOGUE_EX(CDriverBase, Drv)
		return pThis->OnGetImage(nCurDev,ppData,pdwDataSize)?S_OK:S_FALSE;
}

HRESULT CDriverBase::XDrv::ConvertToNative(int nCurDev, LPVOID lpData, DWORD dwDataSize, IUnknown *punk)
{
	METHOD_PROLOGUE_EX(CDriverBase, Drv)
	return pThis->OnConvertToNative(nCurDev,lpData,dwDataSize,punk)?S_OK:S_FALSE;
}

HRESULT CDriverBase::XDrv::GetTriggerMode(int nCurDev, BOOL *pgTriggerMode)
{
	return E_NOTIMPL;
}

HRESULT CDriverBase::XDrv::SetTriggerMode(int nCurDev, BOOL bSet)
{
	return E_NOTIMPL;
}

HRESULT CDriverBase::XDrv::DoSoftwareTrigger(int nCurDev)
{
	return E_NOTIMPL;
}

HRESULT CDriverBase::XDrv::StartImageAquisition(int nCurDev, BOOL bStart)
{
	return E_NOTIMPL;
}

HRESULT CDriverBase::XDrv::IsImageAquisitionStarted(int nCurDev, BOOL *pbAquisition)
{
	return E_NOTIMPL;
}

HRESULT CDriverBase::XDrv::AddDriverSite(int nCurDev, IDriverSite *pSite)
{
	return E_NOTIMPL;
}

HRESULT CDriverBase::XDrv::RemoveDriverSite(int nCurDev, IDriverSite *pSite)
{
	return E_NOTIMPL;
}



/////////////////////////////////////////////////////////////////////////////
// CDriverBase message handlers
