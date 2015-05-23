// Factory.cpp : implementation file for class CVTFactory
#include "stdAfx.h"
#include "Factory.h"
//#include "TransTable.h"
#include "Utils.h"
#include "Core5.h"

#include "Registry.h"
#include <AfxPriv.h>
#include <AfxCtl.h>
#include "\vt5\awin\profiler.h"


bool _UnregisterObject(REFCLSID clsid, LPCTSTR szProgID);
bool _RegisterObject(REFCLSID clsid, LPCTSTR szProgID);


IMPLEMENT_DYNAMIC(CNoGuardFactory, COleObjectFactory);

CNoGuardFactory::CNoGuardFactory(REFCLSID clsid, CRuntimeClass* pRuntimeClass, BOOL bMultiInstance, LPCTSTR lpszProgID)
: CRuntimeInfoPatch( pRuntimeClass ), COleObjectFactory(clsid, pRuntimeClass, bMultiInstance, lpszProgID) 
{
}


IMPLEMENT_DYNAMIC(CVTFactory, COleObjectFactory);
CVTFactory::CVTFactory(REFCLSID clsid, CRuntimeClass* pRuntimeClass, BOOL bMultiInstance, LPCTSTR lpszProgID)
: CRuntimeInfoPatch( pRuntimeClass ), COleObjectFactory(clsid, pRuntimeClass, bMultiInstance, lpszProgID) 
{
}

REFCLSID CVTFactory::GetClassID() const
{	
	ASSERT_VALID(this); 
	return m_clsid; 
}

REFCLSID CVTFactory::GetExternClassID() const
{
	ASSERT_VALID(this); 
	return m_guidExtern; 
}

CString CVTFactory::GetProgID()
{
	return CString(m_lpszProgID);
}

DWORD_PTR CVTFactory::creation_func(DWORD_PTR dwParam)
{
	DWORD dwRet = 0;
	CVTFactory * pFac = (CVTFactory*)dwParam;
	ASSERT_VALID(pFac);

	ASSERT(AfxIsValidAddress(pFac->m_pRuntimeClass, sizeof(CRuntimeClass), FALSE));
		// this implementation needs a runtime class

	// allocate object, throw exception on failure
	CCmdTarget* pTarget = (CCmdTarget*)pFac->m_pRuntimeClass->CreateObject();
	if (pTarget == NULL)
		AfxThrowMemoryException();

	// make sure it is a CCmdTarget
	ASSERT_KINDOF(CCmdTarget, pTarget);
	ASSERT_VALID(pTarget);

	// return the new CCmdTarget object
	return (DWORD_PTR)pTarget;
}

CCmdTarget* CVTFactory::OnCreateObject()
{
	return (CCmdTarget*)creation_func((DWORD_PTR)this);
}


void CVTFactory::LoadInfo()
{
	IVTApplicationPtr sptrApp;
	HMODULE hMod = GetModuleHandle(0);
	if (hMod)
	{
		typedef  IUnknown* (*PGET_APP_UNKNOWN)(bool bAddRef /*= false*/);
		PGET_APP_UNKNOWN pGetAppUnknown = (PGET_APP_UNKNOWN)GetProcAddress(hMod, "GuardGetAppUnknown"); 
		if (pGetAppUnknown)
			sptrApp = pGetAppUnknown(false);
	}

	if (sptrApp == 0)
		sptrApp = GetAppUnknown(false);

	if (sptrApp)
	{
		m_guidExtern = INVALID_KEY;
		// Get External GUID from internal/external ProgID : mode 0
		sptrApp->GetModeData(0, (DWORD*)&m_guidExtern, _bstr_t(m_lpszProgID));
	}
//	ASSERT (m_guidExtern != INVALID_KEY);
}


BEGIN_INTERFACE_MAP(CVTFactory, COleObjectFactory)
	INTERFACE_PART(CVTFactory, __uuidof(IVTClass), VTClass)
END_INTERFACE_MAP()


STDMETHODIMP_(ULONG) CVTFactory::XVTClass::AddRef()
{
	METHOD_PROLOGUE_EX_(CVTFactory, VTClass)
	return pThis->InternalAddRef();
}

STDMETHODIMP_(ULONG) CVTFactory::XVTClass::Release()
{
	METHOD_PROLOGUE_EX_(CVTFactory, VTClass)
	return pThis->InternalRelease();
}

STDMETHODIMP CVTFactory::XVTClass::QueryInterface(REFIID iid, LPVOID* ppvObj)
{
	METHOD_PROLOGUE_EX_(CVTFactory, VTClass)
	return pThis->InternalQueryInterface(&iid, ppvObj);
}

STDMETHODIMP CVTFactory::XVTClass::VTCreateObject(LPUNKNOWN pUnkOuter, LPUNKNOWN /* pUnkReserved */, 
												REFIID riid, BSTR bstrKey, LPVOID* ppvObject)
{
	METHOD_PROLOGUE_EX(CVTFactory, VTClass)
	ASSERT_VALID(pThis);

	ASSERT (pThis->m_guidExtern != INVALID_KEY);
	if (ppvObject == NULL)
		return E_POINTER;

	*ppvObject = NULL;

	// outer objects must ask for IUnknown only
	ASSERT(pUnkOuter == NULL || riid == IID_IUnknown);

	// attempt to create the object
	CCmdTarget* pTarget = NULL;
	SCODE sc = E_OUTOFMEMORY;
	TRY
	{
		// attempt to create the object
		pTarget = (CCmdTarget*)CVTFactory::creation_func((DWORD_PTR)pThis);
		if (pTarget != NULL)
		{
			// check for aggregation on object not supporting it
			sc = CLASS_E_NOAGGREGATION;
			if (pUnkOuter == NULL || pTarget->m_xInnerUnknown != 0)
			{
				// create aggregates used by the object
				pTarget->m_pOuterUnknown = pUnkOuter;
				sc = E_OUTOFMEMORY;
				if (pTarget->OnCreateAggregates())
					sc = S_OK;
			}
		}
	}
	END_TRY

	// finish creation
	if (sc == S_OK)
	{
		DWORD dwRef = 1;
		if (pUnkOuter != NULL)
		{
			// return inner unknown instead of IUnknown
			*ppvObject = &pTarget->m_xInnerUnknown;
		}
		else
		{
			// query for requested interface
			sc = pTarget->InternalQueryInterface(&riid, ppvObject);
			if (sc == S_OK)
			{
				dwRef = pTarget->InternalRelease();
				ASSERT(dwRef != 0);
			}
		}
		if (dwRef != 1)
			TRACE1("Warning: object created with reference of %ld\n", dwRef);
	}

	// cleanup in case of errors
	if (sc != S_OK)
		delete pTarget;

	return sc;
}

STDMETHODIMP CVTFactory::XVTClass::VTRegister(BOOL bRegister)
{
	METHOD_PROLOGUE_EX_(CVTFactory, VTClass)
	return pThis->UpdateRegistry(bRegister);
}

STDMETHODIMP CVTFactory::XVTClass::VTGetProgID(BSTR * pbstrProgID)
{
	METHOD_PROLOGUE_EX_(CVTFactory, VTClass)

	if (!pbstrProgID)
		return E_POINTER;

	CString str(pThis->m_lpszProgID);
	*pbstrProgID = str.AllocSysString();
	return S_OK;

}

BOOL CVTFactory::UpdateRegistry(BOOL bRegister)
{
	BOOL bRes = bRegister ? RegisterThis() : UnregisterThis();
	ASSERT(bRes);
	return bRes;
//	return COleObjectFactory::UpdateRegistry(bRegister);
}

/*
если мы оказались на вызове функции CreateObject,
значит, 


*/

/*
Алгоритм создания компонента:
В программе (shell.exe) вызывается CoCreateInstance(given_CLSID...)
вызов перехватывается нашей функцией 
{ 
	// эта функуия динамически закриптована и перед использованием раскриптовывается.
	по открытой таблице трансляций (ТТ) ищется given_CLSID.
	еслит не найден - возвращается INVALID_GUID, в противном случае - закриптованный inner_CLSID.
	он раскриптовывается и подается на вход оригинальной CoCreateInstance.
	Понятно, что по INVALID_GUID ничего не создается.
	если given_CLSID == раскриптованный inner_CLSID, то это значит, что создается разрешенный 
	  чужой объект, тогда просто вызывается оригинальной CoCreateInstance.
	если не так, что вызывается наш код CoCreateInstanceNew, который работает через IVTClass.
}

IClassFactory объекта.
наши объекты могут создаваться через стандартный IClassFactory,
который вызывает методы нашего интерфейса, при этом
IClassFactory->CreateInstance & IClassFactory->CreateInstanceLic
через посредство переписанной виртуальной функции OnCreateObject
вызывают статическую функцию DWORD CVTFactory::creation_func(DWORD_PTR dwParam), 
которая выполняет ту же работу, что и методы интерфейса IVTClass.
Разумеется, они могут создаваться и через методы интерфейса IVTClass.

DWORD_PTR creation_func(DWORD_PTR dwParam)
{
	DWORD dwRet = 0;
	CVTFactory * pFac = (CVTFactory*)dwParam;
	ASSERT_VALID(pFac);

	ASSERT(AfxIsValidAddress(pFac->m_pRuntimeClass, sizeof(CRuntimeClass), FALSE));
		// this implementation needs a runtime class

	// allocate object, throw exception on failure
	CCmdTarget* pTarget = (CCmdTarget*)pFac->m_pRuntimeClass->CreateObject();
	if (pTarget == NULL)
		AfxThrowMemoryException();

	// make sure it is a CCmdTarget
	ASSERT_KINDOF(CCmdTarget, pTarget);
	ASSERT_VALID(pTarget);

	// return the new CCmdTarget object
	return (DWORD)pTarget;
}

Этот код динамически криптуется. В нем может проводиться аутентификация файла ТТ, или проверка наличия 
записи создаваемого объекта, или и то, и другое, да еще и не каждый раз (для введения в заблуждение)

IVTClass : IUnknown
{
	// analog CreateInstance
	STDMETHODIMP CreateObject(LPUNKNOWN pUnkOuter, LPUNKNOWN pUnkReserved = 0, REFIID riid, BSTR bstrKey, LPVOID* ppvObject)
	// check id is exists
	STDMETHODIMP SetID(REFIID rFakeCLSID, BSTR bstrKey, DWORD dwAutCode);
	STDMETHODIMP SetID2(DWORD dwCode, BSTR bstrKey);

}


*/

/*
Аутентификация файла с ТТ.
Проводится вычисление имитовставки файла и сравнение с заданной. 
В случае несовпадения выставляется флаг INVALID_FILE_TT
*/

/*
Проверка наличия заданного CLSID:
проводится поиск по ТТ, если найдено, то ра
*/



/*
также требуется дописать CoGetClassObject подобно CoCreateInstance
*/

/*
ВАЖНО:
по поводу построения файла с таблицей трансляций:
Нужно написать вставку, которая будет во время выполнения программы
смотреть используемые GUID, etc., чтобы знать, какие не наши компоненты
используются и включить необходимые из них в таблицу.
В противном случае, программа работать не будет.
*/

BOOL CVTFactory::RegisterThis()
{
	// try unregister
	IVTApplicationPtr sptrApp;
	HMODULE hMod = GetModuleHandle(0);
	if (hMod)
	{
		typedef  IUnknown* (*PGET_APP_UNKNOWN)(bool bAddRef /*= false*/);
		PGET_APP_UNKNOWN pGetAppUnknown = (PGET_APP_UNKNOWN)GetProcAddress(hMod, "GuardGetAppUnknown"); 
		if (pGetAppUnknown)
			sptrApp = pGetAppUnknown(false);
	}

	if (sptrApp == 0)
		sptrApp = GetAppUnknown(false);

	if (sptrApp == 0)
		return false;

	BSTR bstrsuff = 0;
	if (FAILED(sptrApp->GetData(0, 0, &bstrsuff, 0)))
		return false;

	// suffix
	_bstr_t bstrSuff(bstrsuff, false);

	// get old extern guid
	m_guidExtern = INVALID_KEY;
	// get old data
	DWORD dwData = 0;
	// get extern guid by inner guid			: mode 1
	sptrApp->GetEntry(1, (DWORD*)&m_clsid, (DWORD*)&m_guidExtern, &dwData);

	// if this call from fastreg application
	BOOL bRegister = false;
	sptrApp->GetMode(&bRegister);

	if (!bRegister)
	{
		IApplicationPtr strA = sptrApp;
		bRegister = strA != 0;
	}
	if (bRegister)
	{
		// first we need to unregister this component
		UnregisterThis();
		// second : we need remove old entry from table
		if (m_guidExtern != INVALID_KEY)
			sptrApp->RemoveEntry((DWORD*)&m_guidExtern, _bstr_t(m_lpszProgID));
		else
		{
			// and create new CLSID if it's needed
			if (FAILED(::CoCreateGuid(&m_guidExtern)) || m_guidExtern == INVALID_KEY)
				return false;
		}
	}

	// Create extern ProgID
	CString strProgID(m_lpszProgID);
#if _MSC_VER >= 1300
	strProgID += (char*)bstr_t( bstrSuff );
#else
	strProgID += bstrSuff;
#endif

	// and register object		
	bool bRet = m_guidExtern != INVALID_KEY ? _RegisterObject(m_guidExtern, strProgID) : false;
	if (bRet && bRegister)
	{
		if (FAILED(sptrApp->AddEntry((DWORD*)&m_clsid, (DWORD*)&m_guidExtern, _bstr_t(m_lpszProgID), dwData)))
			return false;
	}

	return bRet;
}

BOOL CVTFactory::UnregisterThis()
{
	// get external progID and external CLSID to unregister
	IVTApplicationPtr sptrApp;
	HMODULE hMod = GetModuleHandle(0);
	if (hMod)
	{
		typedef  IUnknown* (*PGET_APP_UNKNOWN)(bool bAddRef /*= false*/);
		PGET_APP_UNKNOWN pGetAppUnknown = (PGET_APP_UNKNOWN)GetProcAddress(hMod, "GuardGetAppUnknown"); 
		if (pGetAppUnknown)
			sptrApp = pGetAppUnknown(false);
	}

	if (sptrApp == 0)
		sptrApp = GetAppUnknown(false);

	if (sptrApp == 0)
		return false;

	// really unregister ? 
	BOOL bReg = false;
	if (FAILED(sptrApp->GetMode(&bReg)))
		return true;

	if (!bReg)
	{
		IApplicationPtr strA = sptrApp;
		bReg = strA != 0;
	}
	if (!bReg)
		return true;

	BSTR bstrsuff = 0;
	if (FAILED(sptrApp->GetData(0, 0, &bstrsuff, 0)))
		return false;
	// suffix
	_bstr_t bstrSuff(bstrsuff, false);

	// get extern clsid from innerProgID (use TranslateTable)
	GuidKey guidKey;
	// Get External GUID from internal/external ProgID : mode 0
	if (FAILED(sptrApp->GetModeData(0, (DWORD*)&guidKey, _bstr_t(m_lpszProgID))))
		return false;

	if (guidKey != INVALID_KEY)
	{
		// remove from table
		sptrApp->RemoveEntry((DWORD*)&guidKey, _bstr_t(m_lpszProgID));
	}

	_UnregisterObject(GuidKey(m_clsid), m_lpszProgID);
//	_UnregisterObject(INVALID_KEY, strProgID + "D");

	CString strProgID(m_lpszProgID);
	strProgID += (LPCTSTR)bstrSuff;
	
	return guidKey != INVALID_KEY ? _UnregisterObject(guidKey, strProgID) : true;
}

BOOL CVTFactory::UpdateRegistryCtrl(BOOL bRegister, HINSTANCE nInstance, UINT idTypeName, UINT idBitmap, 
									int nRegFlags, DWORD dwMiscStatus, REFGUID tlid, 
									WORD wVerMajor, WORD wVerMinor)
{
	// get instance
	if (!nInstance)
		nInstance = AfxGetInstanceHandle();

	IVTApplicationPtr sptrApp;
	HMODULE hMod = GetModuleHandle(0);
	if (hMod)
	{
		typedef  IUnknown* (*PGET_APP_UNKNOWN)(bool bAddRef /*= false*/);
		PGET_APP_UNKNOWN pGetAppUnknown = (PGET_APP_UNKNOWN)GetProcAddress(hMod, "GuardGetAppUnknown"); 
		if (pGetAppUnknown)
			sptrApp = pGetAppUnknown(false);
	}

	if (sptrApp == 0)
		sptrApp = GetAppUnknown(false);
	if (sptrApp == 0)
		return false;

	BSTR bstrsuff = 0;
	if (FAILED(sptrApp->GetData(0, 0, &bstrsuff, 0)))
		return false;

	// suffix
	_bstr_t bstrSuff(bstrsuff, false);

	// get old extern guid
	m_guidExtern = INVALID_KEY;
	// get old data
	DWORD dwData = 0;
	// get extern guid by inner guid			: mode 1
	sptrApp->GetEntry(1, (DWORD*)&m_clsid, (DWORD*)&m_guidExtern, &dwData);

	// Create extern ProgID
	CString strProgID(m_lpszProgID);
#if _MSC_VER >= 1300
	strProgID += (char*)bstr_t( bstrSuff );
#else
	strProgID += bstrSuff;
#endif

	// if this call from fastreg application
	BOOL bModeRegister = false;
	sptrApp->GetMode(&bModeRegister);
	if (!bModeRegister)
	{
		IApplicationPtr strA = sptrApp;
		bModeRegister = strA != 0;
	}
	if (bModeRegister)
	{
		if (m_guidExtern != INVALID_KEY)
		{
			// first we need to unregister this component
			AfxOleUnregisterClass(m_guidExtern, strProgID);
			// second : we need remove old entry from table
			sptrApp->RemoveEntry((DWORD*)&m_guidExtern, _bstr_t(m_lpszProgID));
		}

		// and create new CLSID
		if (FAILED(::CoCreateGuid(&m_guidExtern)))
			return false;
	}

	if (m_guidExtern == INVALID_KEY)
		return false;

	// if we need to register ctrl
	if (bRegister)
	{
		// register object		
		BOOL bRet = AfxOleRegisterControlClass(nInstance, m_guidExtern, strProgID, idTypeName, idBitmap,
												nRegFlags, dwMiscStatus, tlid, wVerMajor, wVerMinor);
		// if register is success -> add to table
		if (bRet && bModeRegister)
		{
			if (FAILED(sptrApp->AddEntry((DWORD*)&m_clsid, (DWORD*)&m_guidExtern, _bstr_t(m_lpszProgID), dwData)))
				return false;
		}
	}
	else
	{
		// remove from table
		sptrApp->RemoveEntry((DWORD*)&m_guidExtern, _bstr_t(m_lpszProgID));
		// unregister
		if (!AfxOleUnregisterClass(m_guidExtern, strProgID))
			return false;
	}
	return true;
}

BOOL CVTFactory::UpdateRegistryPage(BOOL bRegister, HINSTANCE nInstance, UINT idTypeName)
{
	// get instance
	if (!nInstance)
		nInstance = AfxGetInstanceHandle();

	IVTApplicationPtr sptrApp;
	HMODULE hMod = GetModuleHandle(0);
	if (hMod)
	{
		typedef  IUnknown* (*PGET_APP_UNKNOWN)(bool bAddRef /*= false*/);
		PGET_APP_UNKNOWN pGetAppUnknown = (PGET_APP_UNKNOWN)GetProcAddress(hMod, "GuardGetAppUnknown"); 
		if (pGetAppUnknown)
			sptrApp = pGetAppUnknown(false);
	}

	if (sptrApp == 0)
		sptrApp = GetAppUnknown(false);
	if (sptrApp == 0)
		return false;

	BSTR bstrsuff = 0;
	if (FAILED(sptrApp->GetData(0, 0, &bstrsuff, 0)))
		return false;

	// suffix
	_bstr_t bstrSuff(bstrsuff, false);

	// get old extern guid
	m_guidExtern = INVALID_KEY;
	// get old data
	DWORD dwData = 0;
	// get extern guid by inner guid			: mode 1
	sptrApp->GetEntry(1, (DWORD*)&m_clsid, (DWORD*)&m_guidExtern, &dwData);

	// Create extern ProgID
	CString strProgID(m_lpszProgID);
#if _MSC_VER >= 1300
	strProgID += (char*)bstr_t( bstrSuff );
#else
	strProgID += bstrSuff;
#endif

	// if this call from fastreg application
	BOOL bModeRegister = false;
	sptrApp->GetMode(&bModeRegister);
	if (!bModeRegister)
	{
		IApplicationPtr strA = sptrApp;
		bModeRegister = strA != 0;
	}
	if (bModeRegister)
	{
		if (m_guidExtern != INVALID_KEY)
		{
			// first we need to unregister this component
			AfxOleUnregisterClass(m_guidExtern, NULL);
			// second : we need remove old entry from table
			sptrApp->RemoveEntry((DWORD*)&m_guidExtern, _bstr_t(m_lpszProgID));
		}

		// and create new CLSID
		if (FAILED(::CoCreateGuid(&m_guidExtern)))
			return false;
	}

	if (m_guidExtern == INVALID_KEY)
		return false;

	// if we need to register ctrl
	if (bRegister)
	{
		// register object		
		BOOL bRet = AfxOleRegisterPropertyPageClass(nInstance, m_guidExtern, idTypeName);
		// if register is success -> add to table
		if (bRet && bModeRegister)
		{
			if (FAILED(sptrApp->AddEntry((DWORD*)&m_clsid, (DWORD*)&m_guidExtern, _bstr_t(m_lpszProgID), dwData)))
				return false;
		}
	}
	else
	{
		// remove from table
		sptrApp->RemoveEntry((DWORD*)&m_guidExtern, _bstr_t(m_lpszProgID));
		// unregister
		if (!AfxOleUnregisterClass(m_guidExtern, NULL))
			return false;
	}
	return true;
}


bool _RegisterObject(REFCLSID clsid, LPCTSTR szProgID)
{
	BSTR bstr = 0;
	if (FAILED(::StringFromCLSID(clsid, &bstr)) || !bstr)
		return false;

	// string {CLSID}
	CString strCLSID = bstr;
	::CoTaskMemFree(bstr);
	// string {ProgID}
	CString strProgID(szProgID);
	// string CLSID\\{CLSID}
	CString strRegGuid = _T("CLSID\\") + strCLSID;
	// string CLSID\\{CLSID}\\InprocServer32
	CString strRegInprocServer = strRegGuid + _T("\\InprocServer32");
	// string CLSID\\{CLSID}\\ProgID
	CString strRegProgID = strRegGuid + _T("\\ProgID");
	// string {ProgID}\\CLSID
	CString strProgID_CLSID = strProgID + _T("\\CLSID");

	// get module name
	CString strModuleName;
	AfxGetModuleShortFileName(AfxGetInstanceHandle(), strModuleName);
	if (strModuleName.IsEmpty())
		return false;

// string CLSID\\{CLSID}
	CVTRegKey reg;
	if (reg.Create(HKEY_CLASSES_ROOT, strRegGuid))
	{
		CVTRegValue valProgID;
		valProgID.SetString(strProgID);
		if (!reg.SetValue(valProgID))
		return false;
	}
	else
		return false;

// string CLSID\\{CLSID}\\InprocServer32
	CVTRegKey regInProcServer;
	if (regInProcServer.Create(HKEY_CLASSES_ROOT, strRegInprocServer))
	{
		// set module name
		CVTRegValue val;
		val.SetString(strModuleName);
		if (!regInProcServer.SetValue(val))
			return false;

// threadingmodel
		val.SetName(_T("ThreadingModel"));
		val.SetString(_T("Apartment"));
		if (!regInProcServer.SetValue(val))
			return false;
	}
	else
		return false;

// string CLSID\\{CLSID}\\ProgID
	CVTRegKey regProgID;
	if (regProgID.Create(HKEY_CLASSES_ROOT, strRegProgID))
	{
		// set value
		CVTRegValue valProgID;
		valProgID.SetString(strProgID);
		if (!regProgID.SetValue(valProgID))
			return false;
	}
	else
		return false;

// string {ProgID}\\CLSID
	if (regProgID.Create(HKEY_CLASSES_ROOT, strProgID_CLSID))
	{
		CVTRegValue valCLSID;
		valCLSID.SetString(strCLSID);
		if (!regProgID.SetValue(valCLSID))
			return false;
	}
	else
		return false;

	return true;
}

bool _UnregisterObject(REFCLSID clsid, LPCTSTR szProgID)
{
	CString strProgID(szProgID);
	GuidKey guid(clsid);
	if (!strProgID.IsEmpty())
	{
		CVTRegKey regProg;
		if (regProg.Open(HKEY_CLASSES_ROOT, strProgID + _T("\\CLSID")))
		{
			CString strCLSID;

			CVTRegValue val;
			val.SetType(REG_SZ);
			if (regProg.GetValue(val))
				strCLSID = val.GetString();

			if (!strCLSID.IsEmpty())
			{
				CVTRegKey regCLSID;
				if (regCLSID.Open(HKEY_CLASSES_ROOT, _T("CLSID\\") + strCLSID))
					regCLSID.DeleteKey();
			}

		}
		if (regProg.Open(HKEY_CLASSES_ROOT, strProgID))
			regProg.DeleteKey();
	}

	if (guid != INVALID_KEY)
	{
		// create string key
		BSTR bstr = 0;
		if (SUCCEEDED(::StringFromCLSID(guid, &bstr)) && bstr)
		{
			CString strCLSID = bstr;
			::CoTaskMemFree(bstr);

			CString strRegKey = _T("CLSID\\") + strCLSID;
			CString strRegKeyProgID = strRegKey + _T("\\ProgID");
			CString strRealProgID;

			// check progID exists and get real progID if it's needed
			CVTRegKey regProg;
			if (regProg.Open(HKEY_CLASSES_ROOT, strRegKeyProgID))
			{
				// get default value (registered extern ProgID)
				CVTRegValue val;
				val.SetType(REG_SZ);
				if (regProg.GetValue(val))
					strRealProgID = val.GetString();

				regProg.Close();
			}

			// delete CLSID entry
			CVTRegKey regCLSID;
			if (regCLSID.Open(HKEY_CLASSES_ROOT, strRegKey))
				regCLSID.DeleteKey();

			// if real progID is Valid
			if (!strRealProgID.IsEmpty())
			{
				// delete real ProgID 
				_UnregisterObject(INVALID_KEY, strRealProgID);
			}
		}
	}
	return true;
}

