// InputDriverManager.cpp : implementation file
//

#include "stdafx.h"
#include "input.h"
#include "NewDoc.h"
#include "InputDriverManager.h"
#include "SelPropertySheet.h"
#include "InputUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDeviceInfo

class CDeviceInfo
{
public:
	CDeviceInfo(int nDriver, int nDevInDriver, CString sShortName, CString sLongName, CString sCategory)
	{
		m_sShortDevName = sShortName;
		m_sLongDevName = sLongName;
		m_sCategory = sCategory;
		m_nDriver = nDriver;
		m_nDevInDriver = nDevInDriver;
	};
	CString m_sShortDevName;
	CString m_sLongDevName;
	CString m_sCategory;
	int m_nDriver;
	int m_nDevInDriver;
};


/////////////////////////////////////////////////////////////////////////////
// CInputDriverManager

IMPLEMENT_DYNCREATE(CInputDriverManager, CCmdTargetEx)

CInputDriverManager::CInputDriverManager()
{
	EnableAutomation();
	
	_OleLockApp( this );

	SetName( _T("InputDriverManager") );
	AttachComponentGroup( szPluginInputDrv );
	CCompManager::Init();
	m_sSectName = _T("Input");
	m_bInited = false;
//	RebuildDevices();
}

CInputDriverManager::~CInputDriverManager()
{
	_OleUnlockApp( this );
	for (int n = 0; n < m_Devices.GetSize(); n++)
		delete m_Devices.GetAt(n);
	m_Devices.RemoveAll();
}


void CInputDriverManager::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTargetEx::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CInputDriverManager, CCmdTargetEx)
	//{{AFX_MSG_MAP(CInputDriverManager)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CInputDriverManager, CCmdTargetEx)
	//{{AFX_DISPATCH_MAP(CInputDriverManager)
	DISP_PROPERTY_EX(CInputDriverManager, "CurrentDriver", GetCurrentDriver1, SetCurrentDriver, VT_BSTR)
	DISP_PROPERTY_EX(CInputDriverManager, "CurrentDevice", GetCurrentDevice, SetCurrentDevice, VT_BSTR)
	DISP_PROPERTY_EX(CInputDriverManager, "CurrentDeviceName", GetCurrentDeviceName, SetCurrentDeviceName, VT_BSTR)
	DISP_FUNCTION(CInputDriverManager, "SetValue", SetValue, VT_EMPTY, VTS_BSTR VTS_VARIANT)
	DISP_FUNCTION(CInputDriverManager, "GetValue", GetValue, VT_VARIANT, VTS_BSTR)
	DISP_FUNCTION(CInputDriverManager, "GetDevicesNum", GetDevicesNum, VT_I4, VTS_NONE)
	DISP_FUNCTION(CInputDriverManager, "GetDeviceName", GetDeviceName, VT_BSTR, VTS_I4)
	DISP_FUNCTION(CInputDriverManager, "GetDeviceCategory", GetDeviceCategory, VT_BSTR, VTS_I4)
	DISP_FUNCTION(CInputDriverManager, "GetDeviceFullName", GetDeviceFullName, VT_BSTR, VTS_I4)
	DISP_FUNCTION(CInputDriverManager, "ExecuteDriverDialog", ExecuteDriverDialog, VT_EMPTY, VTS_BSTR)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IInputDriverManager to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

/*
// {394913EA-2CEC-463B-AE00-1388A2FD6F3D}
IID IID_IDriverManager =
{ 0x394913ea, 0x2cec, 0x463b, { 0xae, 0x0, 0x13, 0x88, 0xa2, 0xfd, 0x6f, 0x3d } };
// {274498B8-961A-4B1D-9059-FBBF5C94298E}
IID IID_IDriver = 
{ 0x274498b8, 0x961a, 0x4b1d, { 0x90, 0x59, 0xfb, 0xbf, 0x5c, 0x94, 0x29, 0x8e } };
*/

BEGIN_INTERFACE_MAP(CInputDriverManager, CCmdTargetEx)
	INTERFACE_PART(CInputDriverManager, IID_INamedObject2, Name)
	INTERFACE_PART(CInputDriverManager, IID_ICompManager, CompMan)
	INTERFACE_PART(CInputDriverManager, IID_IRootedObject, Rooted)
	INTERFACE_PART(CInputDriverManager, IID_IDriverManager, DrvMan)
	INTERFACE_PART(CInputDriverManager, IID_IDriverManager2, DrvMan)
END_INTERFACE_MAP()

// {BB2A4DD0-9D8C-4F9D-914A-B8922D247CA9}
GUARD_IMPLEMENT_OLECREATE(CInputDriverManager, "IDManager.InputDriverManager", 0xbb2a4dd0, 0x9d8c, 0x4f9d, 0x91, 0x4a, 0xb8, 0x92, 0x2d, 0x24, 0x7c, 0xa9)

IMPLEMENT_UNKNOWN(CInputDriverManager, DrvMan)

HRESULT CInputDriverManager::XDrvMan::GetCurrentDeviceName(BSTR *pbstrShortName)
{
	_try_nested(CInputDriverManager, DrvMan, GetCurrentDeviceName)
	{
		if (pbstrShortName)
		{
			CString s = pThis->OnGetCurrentDeviceName();
			*pbstrShortName = s.AllocSysString();
		}
		return S_OK;
	}
	_catch_nested;
}

HRESULT CInputDriverManager::XDrvMan::SetCurrentDeviceName(BSTR bstrShortName)
{
	_try_nested(CInputDriverManager, DrvMan, SetCurrentDeviceName)
	{
		CString s(bstrShortName);
		pThis->OnSetCurrentDeviceName(s);
		return S_OK;
	}
	_catch_nested;
}

HRESULT CInputDriverManager::XDrvMan::IsInputAvailable(BOOL *pbAvail)
{
	_try_nested(CInputDriverManager, DrvMan, IsInputAvailable)
	{
		if (pbAvail)
			*pbAvail = pThis->m_Devices.GetSize() > 0;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CInputDriverManager::XDrvMan::ExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nSelectDriver,
	int nDialogMode, BOOL bFirst, BOOL bFromSettings)
{
	_try_nested(CInputDriverManager, DrvMan, ExecuteSettings)
	{
		if (pThis->OnExecuteSettings(hwndParent, pTarget, nSelectDriver, nDialogMode, bFirst, bFromSettings, NULL))
			return S_OK;
		else
			return S_FALSE;
	}
	_catch_nested;
}

HRESULT CInputDriverManager::XDrvMan::ExecuteSettings2(HWND hwndParent, IUnknown *pTarget, int nSelectDriver,
	int nDialogMode, BOOL bFirst, BOOL bFromSettings, BSTR bstrConf)
{
	_try_nested(CInputDriverManager, DrvMan, ExecuteSettings)
	{
		if (pThis->OnExecuteSettings(hwndParent, pTarget, nSelectDriver, nDialogMode, bFirst, bFromSettings, bstrConf))
			return S_OK;
		else
			return S_FALSE;
	}
	_catch_nested;
}


HRESULT CInputDriverManager::XDrvMan::GetCurrentDeviceAndDriver(BOOL *pbFound, IUnknown **ppunkDriver,
	int *pnDeviceInDriver)
{
	_try_nested(CInputDriverManager, DrvMan, GetCurrentDeviceAndDriver)
	{
		CString s = pThis->OnGetCurrentDeviceName();
		BOOL b = pThis->FindDriver(s, ppunkDriver, pnDeviceInDriver);
		if (pbFound) *pbFound = b;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CInputDriverManager::XDrvMan::GetSectionName(BSTR *pbstrSecName)
{
	_try_nested(CInputDriverManager, DrvMan, GetSectionName)
	{
		CString s(pThis->m_sSectName);
		*pbstrSecName = s.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CInputDriverManager::XDrvMan::GetDevicesCount(int *pnDeviceCount)
{
	_try_nested(CInputDriverManager, DrvMan, GetDevicesCount)
	{
		if (pnDeviceCount)
			*pnDeviceCount = pThis->OnGetDevicesCount();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CInputDriverManager::XDrvMan::GetDeviceInfo(int nDevice, BSTR *pbstrDevShortName, BSTR *pbstrDevLongName,
	BSTR *pbstrCategory, int *pnDeviceDriver, int *pnDeviceInDriver)
{
	_try_nested(CInputDriverManager, DrvMan, GetDeviceInfo)
	{
		CString sShortName;
		CString sLongName;
		CString sCategory;
		pThis->OnGetDeviceInfo(nDevice, sShortName, sLongName, sCategory, pnDeviceDriver, pnDeviceInDriver);
		if (pbstrDevShortName)
			*pbstrDevShortName = sShortName.AllocSysString();
		if (pbstrDevLongName)
			*pbstrDevLongName = sLongName.AllocSysString();
		if (pbstrCategory)
			*pbstrCategory = sCategory.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

CString CInputDriverManager::OnGetCurrentDeviceName()
{
	sptrINamedData sptrND(GetAppUnknown());
	VARIANT var;
	VariantInit(&var);
	CString sName(_T("Device"));
	sptrND->SetupSection(_bstr_t(m_sSectName));
	sptrND->GetValue(_bstr_t(sName), &var);
	CString s;
	if (var.vt == VT_BSTR)
		s = var.bstrVal;
	::VariantClear(&var);
	return s;
}

void CInputDriverManager::OnSetCurrentDeviceName(CString s)
{
	sptrINamedData sptrND(GetAppUnknown());
	VARIANT var;
	CString sName(_T("Device"));
	sptrND->SetupSection(_bstr_t(m_sSectName));
	var.vt = VT_BSTR;
	var.bstrVal = s.AllocSysString();
	sptrND->SetValue(_bstr_t(sName), var);
}

BOOL CInputDriverManager::FindDriver(CString sDevName, IUnknown **ppunkDriver, int *pnDevice)
{
	if (!m_bInited) RebuildDevices();
	if (!sDevName.IsEmpty())
	{
		for (int i = 0; i < m_Devices.GetSize(); i++)
		{
			CDeviceInfo *p = m_Devices.GetAt(i);
			if (sDevName == p->m_sShortDevName)
			{
				if (ppunkDriver)
					*ppunkDriver = GetDriver(p->m_nDriver);
				if (pnDevice) *pnDevice = p->m_nDevInDriver;
				return TRUE;
			}
		}
	}
	// No current device or device not found. Use device 0 as default
	if (m_Devices.GetSize() > 0)
	{
		CDeviceInfo *p = m_Devices.GetAt(0);
		if (ppunkDriver)
			*ppunkDriver = GetDriver(p->m_nDriver);
		if (pnDevice) *pnDevice = p->m_nDevInDriver;
	}
	else
	{
		if (ppunkDriver)
			*ppunkDriver = NULL;
	}
	return FALSE;
}

IUnknown *CInputDriverManager::GetDriver(int nNum)
{
	return GetComponent(nNum);
}

void CInputDriverManager::RebuildDevices()
{
	// Clear list
	for (int n = 0; n < m_Devices.GetSize(); n++)
		delete m_Devices.GetAt(n);
	m_Devices.RemoveAll();
	// Fill devices
	int nCount = GetComponentCount();
	for (int iDrv = 0; iDrv < nCount; iDrv++)
	{
		IUnknown *punkDrv = GetDriver(iDrv);
		if (punkDrv)
		{
			sptrIDriver sptrID(punkDrv);
			punkDrv->Release();
			int nDevices = 0;
			HRESULT hr = sptrID->GetDevicesCount(&nDevices);
			if (SUCCEEDED(hr))
			{
				for (int iDev = 0; iDev < nDevices; iDev++)
				{
					BSTR bstrShortDevName,bstrLongDevName,bstrCategory;
					hr = sptrID->GetDeviceNames(iDev, &bstrShortDevName, &bstrLongDevName, &bstrCategory);
					if (SUCCEEDED(hr))
					{
						CString sShortName(bstrShortDevName);
						::SysFreeString(bstrShortDevName);
						CString sLongName(bstrLongDevName);
						::SysFreeString(bstrLongDevName);
						CString sCategory(bstrCategory);
						::SysFreeString(bstrCategory);
#ifdef DEMOVERSION
						if (sCategory == _T("CreationMethod"))
						{
#endif
						CDeviceInfo *p = new CDeviceInfo(iDrv, iDev, sShortName, sLongName, sCategory);
						m_Devices.Add(p);
#ifdef DEMOVERSION
						}
#endif
					}
				}
			}
		}
	}
	m_bInited = true;
}

int CInputDriverManager::OnGetDevicesCount()
{
	if (!m_bInited) RebuildDevices();
	return (int)m_Devices.GetSize();
}

void CInputDriverManager::OnGetDeviceInfo(int nDev, CString &sShortName, CString &sLongName, CString &sCategory,
	int *pnDriver, int *pnDeviceInDriver)
{
	if (!m_bInited) RebuildDevices();
	if (nDev < 0 || nDev >= m_Devices.GetSize())
	{
		sShortName.Empty();
		sLongName.Empty();
		if (pnDriver) *pnDriver = -1;
		if (pnDeviceInDriver) *pnDeviceInDriver = -1;
		return;
	}
	CDeviceInfo *p = m_Devices.GetAt(nDev);
	sShortName = p->m_sShortDevName;
	sLongName = p->m_sLongDevName;
	sCategory = p->m_sCategory;
	if (pnDriver) *pnDriver = p->m_nDriver;
	if (pnDeviceInDriver) *pnDeviceInDriver= p->m_nDevInDriver;
}


/////////////////////////////////////////////////////////////////////////////
// CInputDriverManager message handlers

void CInputDriverManager::SetValue(LPCTSTR Name, const VARIANT FAR& Value) 
{
	if (!m_bInited) RebuildDevices();
	CString s = OnGetCurrentDeviceName();
	IUnknown *punkDrv;
	int nDev;
	FindDriver(s, &punkDrv, &nDev);
	if (punkDrv)
	{
		sptrIDriver sptrID(punkDrv);
		punkDrv->Release();
		_bstr_t bstr(Name);
		sptrID->SetValue(nDev, bstr, Value);
	}
}

VARIANT CInputDriverManager::GetValue(LPCTSTR pName) 
{
	if (!m_bInited) RebuildDevices();
	VARIANT vaResult;
	VariantInit(&vaResult);
	CString s = OnGetCurrentDeviceName();
	IUnknown *punkDrv;
	int nDev;
	FindDriver(s, &punkDrv, &nDev);
	if (punkDrv)
	{
		sptrIDriver sptrID(punkDrv);
		punkDrv->Release();
		_bstr_t bstr(pName);
		if (_tcscmp(pName,_T("IsLiveVideo"))==0)
		{
			IInputPreviewPtr sptrIP(sptrID);
			IInputWindowPtr sptrIW(sptrID);
			vaResult.vt = VT_I4;
			vaResult.lVal = (sptrIP != 0 || sptrIW != 0)?1:0;
		}
		else
			sptrID->GetValue(nDev, bstr, &vaResult);
	}
	return vaResult;
}

BSTR CInputDriverManager::GetCurrentDriver1() 
{
	if (!m_bInited) RebuildDevices();
	CString s = OnGetCurrentDeviceName();
	int nDev;
	IUnknown *punkDriver;
	BOOL b = FindDriver(s, &punkDriver, &nDev);
	if (punkDriver)
	{
		sptrIDriver sptrD(punkDriver);
		punkDriver->Release();
		BSTR bstr = NULL;
		sptrD->GetShortName(&bstr);
		return bstr;
	}
	return NULL;
}

void CInputDriverManager::SetCurrentDriver(LPCTSTR lpszNewValue) 
{
}

BSTR CInputDriverManager::GetCurrentDevice() 
{
	if (!m_bInited) RebuildDevices();
	CString strResult(OnGetCurrentDeviceName());
	return strResult.AllocSysString();
}

void CInputDriverManager::SetCurrentDevice(LPCTSTR lpszNewValue) 
{
	if (!m_bInited) RebuildDevices();
	OnSetCurrentDeviceName(lpszNewValue);
}


BSTR CInputDriverManager::GetCurrentDeviceName() 
{
	if (!m_bInited) RebuildDevices();
	CString s = OnGetCurrentDeviceName();
	for (int i = 0; i < m_Devices.GetSize(); i++)
	{
		CDeviceInfo *p = m_Devices.GetAt(i);
		if (p->m_sShortDevName == s)
		{
			CString strResult = p->m_sLongDevName;
			return strResult.AllocSysString();
			break;
		}
	}
	return NULL;
}

void CInputDriverManager::SetCurrentDeviceName(LPCTSTR lpszNewValue) 
{
	if (!m_bInited) RebuildDevices();
	CString sNewDev;
	for (int i = 0; i < m_Devices.GetSize(); i++)
	{
		CDeviceInfo *p = m_Devices.GetAt(i);
		if (p->m_sLongDevName == lpszNewValue)
		{
			sNewDev = p->m_sShortDevName;
			break;
		}
	}
	if (!sNewDev.IsEmpty())
		OnSetCurrentDeviceName(sNewDev);
}

long CInputDriverManager::GetDevicesNum() 
{
	if (!m_bInited) RebuildDevices();
	return m_Devices.GetSize();
}

BSTR CInputDriverManager::GetDeviceName(long iDev) 
{
	if (!m_bInited) RebuildDevices();
	CDeviceInfo *p = m_Devices.GetAt(iDev);
	CString strResult(p->m_sShortDevName);
	return strResult.AllocSysString();
}

BSTR CInputDriverManager::GetDeviceCategory(long iDev) 
{
	if (!m_bInited) RebuildDevices();
	if (iDev < 0 || iDev >= m_Devices.GetSize()) return NULL;
	CDeviceInfo *p = m_Devices.GetAt(iDev);
	CString strResult(p->m_sCategory);
	return strResult.AllocSysString();
}

BSTR CInputDriverManager::GetDeviceFullName(long iDev) 
{
	if (!m_bInited) RebuildDevices();
	if (iDev < 0 || iDev >= m_Devices.GetSize()) return NULL;
	CDeviceInfo *p = m_Devices.GetAt(iDev);
	CString strResult(p->m_sLongDevName);
	return strResult.AllocSysString();
}

bool CInputDriverManager::SelectDriver(HWND hwndParent, int nMode)
{
	if (!m_bInited) RebuildDevices();
	CSelPropertySheet propSheet;
	if (nMode == 2)
	{
		if (::GetValueInt(::GetAppUnknown(), "\\Input", "OnlyCamerasInMode2", FALSE))
			propSheet.m_Page2.m_bDisableBack = true;
		propSheet.SetActivePage(1);
	}
	return propSheet.DoModal() == ID_WIZFINISH;
}

bool CInputDriverManager::OnExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nSelectDriver,
	int nDialogMode, BOOL bFirst, BOOL bFromSettings, BSTR bstrConf)
{
	if (!m_bInited) RebuildDevices();
	if (m_Devices.GetSize() == 0)
		return false; // No input devices
	// Select device if needed
	if (nSelectDriver != 0)
	{
		if (!SelectDriver(hwndParent, nSelectDriver))
			return false;
	}
	CString s = OnGetCurrentDeviceName();
	IUnknown *punkDrv = NULL;
	int nDev;
	FindDriver(s, &punkDrv, &nDev);
	if (punkDrv)
	{
		sptrIDriver sptrID(punkDrv);
		punkDrv->Release();
		_CUseState use(sptrID, bstrConf);
		HRESULT hr = sptrID->ExecuteSettings(hwndParent, pTarget, nDev, nDialogMode, bFirst, bFromSettings);
		if (hr == S_OK)
			return true;
	}
	return false;
}

void CInputDriverManager::ExecuteDriverDialog(LPCTSTR strName)
{
	if (!m_bInited) RebuildDevices();
	CString s = OnGetCurrentDeviceName();
	IUnknown *punkDrv = NULL;
	int nDev;
	FindDriver(s, &punkDrv, &nDev);
	if (punkDrv)
	{
		sptrIDriver sptrID(punkDrv);
		punkDrv->Release();
		sptrID->ExecuteDriverDialog(nDev, strName);
	}
}


