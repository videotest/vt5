#include "stdafx.h"
#include "CmnHdr.h"
#include <ImageHlp.h>
#pragma comment(lib, "ImageHlp")
#include "\vt5\awin\misc_map.h"
#include "MfcFix.h"
#include "APIHook.h"
#include "MfcOleFix.h"
/*
	mfc90d.dll!COleControlSite::InvokeHelperV(long dwDispID=0xfffffdfa, unsigned short wFlags=0x0004, unsigned short vtRet=0x0000, void * pvRet=0x0000000000000000, const unsigned char * pbParamInfo=0x00000000001281d0, char * argList=0x0000000000128258)  Line 1093	C++
 	8394 mfc90d.dll!COleControlSite::SetPropertyV(long dwDispID=0xfffffdfa, unsigned short vtProp=0x000e, char * argList=0x0000000000128258)  Line 1126	C++
 	7759 mfc90d.dll!COleControlSite::SafeSetProperty(long dwDispID=0xfffffdfa, unsigned short vtProp=0x0008, ...)  Line 1162	C++
 	8605 mfc90d.dll!COleControlSite::SetWindowTextA(const char * lpszString=0x000000006be1a140)  Line 1286 + 0x23 bytes	C++
 	2234 mfc90d.dll!COleControlSite::CreateControlCommon(CWnd * pWndCtrl=0x000000000bb14450, const _GUID & clsid={...}, const CControlCreationInfo & creationInfo={...}, const char * lpszWindowName=0x000000006be1a140, unsigned long dwStyle=0x58000000, const tagPOINT * ppt=0x0000000000128700, const tagSIZE * psize=0x0000000000128708, unsigned int nID=0x00000000, CFile * pPersist=0x0000000000000000, int bStorage=0x00000000, wchar_t * bstrLicKey=0x0000000000000000)  Line 337	C++
 	mfc90d.dll!COleControlSite::CreateControl(CWnd * pWndCtrl=0x000000000bb14450, const _GUID & clsid={...}, const char * lpszWindowName=0x000000006be1a140, unsigned long dwStyle=0x58000000, const tagPOINT * ppt=0x0000000000128700, const tagSIZE * psize=0x0000000000128708, unsigned int nID=0x00000000, CFile * pPersist=0x0000000000000000, int bStorage=0x00000000, wchar_t * bstrLicKey=0x0000000000000000)  Line 191	C++
 	mfc90d.dll!COleControlContainer::CreateControlCommon(CWnd * pWndCtrl=0x000000000bb14450, const _GUID & clsid={...}, const CControlCreationInfo & creationInfo={...}, const char * lpszWindowName=0x000000006be1a140, unsigned long dwStyle=0x58000000, const tagPOINT * ppt=0x0000000000128700, const tagSIZE * psize=0x0000000000128708, unsigned int nID=0x00000000, CFile * pPersist=0x0000000000000000, int bStorage=0x00000000, wchar_t * bstrLicKey=0x0000000000000000, COleControlSite * * ppNewSite=0x0000000000000000)  Line 306 + 0x81 bytes	C++
 	mfc90d.dll!COleControlContainer::CreateControl(CWnd * pWndCtrl=0x000000000bb14450, const _GUID & clsid={...}, const char * lpszWindowName=0x000000006be1a140, unsigned long dwStyle=0x58000000, const tagPOINT * ppt=0x0000000000128700, const tagSIZE * psize=0x0000000000128708, unsigned int nID=0x00000000, CFile * pPersist=0x0000000000000000, int bStorage=0x00000000, wchar_t * bstrLicKey=0x0000000000000000, COleControlSite * * ppNewSite=0x0000000000000000)  Line 342	C++
 	mfc90d.dll!CWnd::CreateControl(const _GUID & clsid={...}, const char * lpszWindowName=0x000000006be1a140, unsigned long dwStyle=0x58000000, const tagPOINT * ppt=0x0000000000128700, const tagSIZE * psize=0x0000000000128708, CWnd * pParentWnd=0x000000000bb137f8, unsigned int nID=0x00000000, CFile * pPersist=0x0000000000000000, int bStorage=0x00000000, wchar_t * bstrLicKey=0x0000000000000000)  Line 71	C++
 	mfc90d.dll!CWnd::CreateControl(const _GUID & clsid={...}, const char * lpszWindowName=0x000000006be1a140, unsigned long dwStyle=0x58000000, const tagRECT & rect={...}, CWnd * pParentWnd=0x000000000bb137f8, unsigned int nID=0x00000000, CFile * pPersist=0x0000000000000000, int bStorage=0x00000000, wchar_t * bstrLicKey=0x0000000000000000)  Line 50	C++
 	mfc90d.dll!CWnd::CreateControl(const char * lpszClass=0x000000000a766a28, const char * lpszWindowName=0x000000006be1a140, unsigned long dwStyle=0x58000000, const tagRECT & rect={...}, CWnd * pParentWnd=0x000000000bb137f8, unsigned int nID=0x00000000, CFile * pPersist=0x0000000000000000, int bStorage=0x00000000, wchar_t * bstrLicKey=0x0000000000000000)  Line 35	C++


       8394      0017B130 [NONAME] ?SetPropertyV@COleControlSite@@UEAAXJGPEAD@Z 
	(public: virtual void __cdecl COleControlSite::SetPropertyV
	(long,unsigned short,char *))

			7759      0017B380 [NONAME] ?SafeSetProperty@COleControlSite@@UEAAHJGZZ 
	(public: virtual int __cdecl COleControlSite::SafeSetProperty
	(long,unsigned short,...))
       
			 8605      0017B780 [NONAME] ?SetWindowTextA@COleControlSite@@UEAAXPEBD@Z 
	(public: virtual void __cdecl COleControlSite::SetWindowTextA
	(char const *))

			 2233      00174470 [NONAME] ?CreateControlCommon@COleControlContainer@@QEAAHPEAVCWnd@@AEBU_GUID@@AEBVCControlCreationInfo@@PEBDKPEBUtagPOINT@@PEBUtagSIZE@@IPEAVCFile@@HPEA_WPEAPEAVCOleControlSite@@@Z 
	(public: int __cdecl COleControlContainer::CreateControlCommon
	(class CWnd *,
struct _GUID const &,
class CControlCreationInfo const &,
char const *,
unsigned long,
struct tagPOINT const *,
struct tagSIZE const *,
unsigned int,
class CFile *,
int,
wchar_t *,
class COleControlSite * *))
       2234      00177E80 [NONAME] ?CreateControlCommon@COleControlSite@@MEAAJPEAVCWnd@@AEBU_GUID@@AEBVCControlCreationInfo@@PEBDKPEBUtagPOINT@@PEBUtagSIZE@@IPEAVCFile@@HPEA_W@Z 
	(protected: virtual long __cdecl COleControlSite::CreateControlCommon
	(class CWnd *,struct _GUID const &,class CControlCreationInfo const &,char const *,unsigned long,struct tagPOINT const *,struct tagSIZE const *,unsigned int,class CFile *,int,wchar_t *))

       2229      001735B0 [NONAME] ?CreateControl@CWnd@@QEAAHAEBU_GUID@@PEBDKAEBUtagRECT@@PEAV1@IPEAVCFile@@HPEA_W@Z 
	(public: int __cdecl CWnd::CreateControl
	(struct _GUID const &,char const *,
	unsigned long,struct tagRECT const &,class CWnd *,unsigned int,
	class CFile *,int,wchar_t *))
       2230      001736B0 [NONAME] ?CreateControl@CWnd@@QEAAHAEBU_GUID@@PEBDKPEBUtagPOINT@@PEBUtagSIZE@@PEAV1@IPEAVCFile@@HPEA_W@Z 
	(public: int __cdecl CWnd::CreateControl
	(struct _GUID const &,char const *,
	unsigned long,struct tagPOINT const *,struct tagSIZE const *,class CWnd *,unsigned int,class CFile *,int,wchar_t *))
       2231      00173840 [NONAME] ?CreateControl@CWnd@@QEAAHAEBVCControlCreationInfo@@KPEBUtagPOINT@@PEBUtagSIZE@@PEAV1@I@Z 
	(public: int __cdecl CWnd::CreateControl
	(class CControlCreationInfo const &,unsigned long,struct tagPOINT const *,struct tagSIZE const *,class CWnd *,unsigned int))
       2232      001734B0 [NONAME] ?CreateControl@CWnd@@QEAAHPEBD0KAEBUtagRECT@@PEAV1@IPEAVCFile@@HPEA_W@Z 
	(public: int __cdecl CWnd::CreateControl
	(char const *,char const *,unsigned long,struct tagRECT const &,class CWnd *,unsigned int,class CFile *,int,wchar_t *))

	BOOL CWnd::CreateControl(LPCTSTR lpszClass, LPCTSTR lpszWindowName,
	DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID,
	CFile* pPersist, BOOL bStorage, BSTR bstrLicKey)


			 2223      001742C0 [NONAME] ?CreateControl@COleControlContainer@@QEAAHPEAVCWnd@@AEBU_GUID@@PEBDKAEBUtagRECT@@IPEAVCFile@@HPEA_WPEAPEAVCOleControlSite@@@Z 
	(public: int __cdecl COleControlContainer::CreateControl
	(class CWnd *,struct _GUID const &
	,char const *,unsigned long,struct tagRECT const &,unsigned int,class CFile *,int,wchar_t *,class COleControlSite * *))
       2224      001748B0 [NONAME] ?CreateControl@COleControlContainer@@QEAAHPEAVCWnd@@AEBU_GUID@@PEBDKPEBUtagPOINT@@PEBUtagSIZE@@IPEAVCFile@@HPEA_WPEAPEAVCOleControlSite@@@Z 
	(public: int __cdecl COleControlContainer::CreateControl
	(class CWnd *,struct _GUID const &
	,char const *,unsigned long,struct tagPOINT const *,struct tagSIZE const *,unsigned int,class CFile *,int,wchar_t *,class COleControlSite * *))
       2225      001743C0 [NONAME] ?CreateControl@COleControlContainer@@QEAAHPEAVCWnd@@AEBVCControlCreationInfo@@KPEBUtagPOINT@@PEBUtagSIZE@@I@Z 
	(public: int __cdecl COleControlContainer::CreateControl
	(class CWnd *,class CControlCreationInfo const &,unsigned long,struct tagPOINT const *,struct tagSIZE const *,unsigned int))

       2226      00177DD0 [NONAME] ?CreateControl@COleControlSite@@QEAAJPEAVCWnd@@AEBVCControlCreationInfo@@KPEBUtagPOINT@@PEBUtagSIZE@@I@Z 
	(public: long __cdecl COleControlSite::CreateControl
	(class CWnd *,class CControlCreationInfo const &,unsigned long,struct tagPOINT const *,struct tagSIZE const *,unsigned int))
       2227      00177BE0 [NONAME] ?CreateControl@COleControlSite@@UEAAJPEAVCWnd@@AEBU_GUID@@PEBDKAEBUtagRECT@@IPEAVCFile@@HPEA_W@Z 
	(public: virtual long __cdecl COleControlSite::CreateControl
	(class CWnd *,struct _GUID const &,
	char const *,unsigned long,struct tagRECT const &,unsigned int,class CFile *,int,wchar_t *))
       2228      00177CE0 [NONAME] ?CreateControl@COleControlSite@@UEAAJPEAVCWnd@@AEBU_GUID@@PEBDKPEBUtagPOINT@@PEBUtagSIZE@@IPEAVCFile@@HPEA_W@Z 
	(public: virtual long __cdecl COleControlSite::CreateControl
	(class CWnd *,struct _GUID const &
	,char const *,unsigned long,struct tagPOINT const *,struct tagSIZE const *,unsigned int,class CFile *,int,wchar_t *))
*/

//HOOK_METHOD(MFCXXD_DLL,5422,BOOL,CWnd,InitControlContainer,
struct TCWndInitControlContainer5422: public CWnd 
{ 
	typedef BOOL (TCWndInitControlContainer5422::*TMethodHook)(BOOL bCreateFromResource); 
	BOOL Hook (BOOL bCreateFromResource); 
	BOOL InitControlContainer(BOOL bCreateFromResource=FALSE)
	{
		return CWnd::InitControlContainer(bCreateFromResource);
	}
	COleControlContainer* GetOCC(void)
	{
		return m_pCtrlCont;
	}
	COleControlSite*& GetOCS(void){return m_pCtrlSite;}
   virtual BOOL CreateControlSite(COleControlContainer* pContainer, 
		 COleControlSite** ppSite, UINT nID, REFCLSID clsid)
	 {
		    return CWnd::CreateControlSite(pContainer, ppSite, nID, clsid);
	 }
}; 
 
CAPIHookMethod<5422,BOOL (CWnd::*) (BOOL bCreateFromResource), TCWndInitControlContainer5422::TMethodHook>* 
	pCWndInitControlContainer5422 = new 
CAPIHookMethod<5422,BOOL (CWnd::*) (BOOL bCreateFromResource), TCWndInitControlContainer5422::TMethodHook> 
(MFCXXD_DLL, &TCWndInitControlContainer5422::Hook, FALSE); 
BOOL TCWndInitControlContainer5422::Hook (BOOL bCreateFromResource)
{
   if (m_pCtrlCont == NULL)
   {
	  BOOL bSuccess;

	  bSuccess = CreateControlContainer( &m_pCtrlCont );
	  if (bSuccess && (m_pCtrlCont == NULL))
	  {
		 // The window wants to use the default control container.
		  TRY
		  {
			m_pCtrlCont = afxOccManager->CreateContainer(this);
		  }
		  END_TRY
	  }
	  //When Container is created and it is not during creation from resources, 
	  //populate the list with all resource created Win32 controls.
	  if (!bCreateFromResource)
	  {
			m_pCtrlCont->FillListSitesOrWnds(GetOccDialogInfo());		
	  }
   }

	// Mark all ancestor windows as containing OLE controls.
	if (m_pCtrlCont != NULL)
	{
		CWnd* pWnd = this;
		while ((pWnd != NULL) && !(pWnd->m_nFlags & WF_OLECTLCONTAINER))
		{
			pWnd->m_nFlags |= WF_OLECTLCONTAINER;
			pWnd = pWnd->GetParent();
			if (! (GetWindowLong(pWnd->GetSafeHwnd(), GWL_STYLE) & WS_CHILD))
				break;
		}
	}
	
	return (m_pCtrlCont != NULL);
}

#if 0
class __COleControlSite__ : public COleControlSite
{
public:
	void __InvokeHelperV__( DISPID dwDispID, WORD wFlags,
		VARTYPE vtRet, void* pvRet, const BYTE* pbParamInfo, va_list argList );
};

typedef void (COleControlSite::*PFN_ODD_Orig)(DISPID dwDispID, WORD wFlags, VARTYPE vtRet, void*pvRet, const BYTE*pbParamInfo, va_list argList); 
typedef void (__COleControlSite__::*PFN_ODD_Hook)(DISPID dwDispID, WORD wFlags, VARTYPE vtRet, void*pvRet, const BYTE*pbParamInfo, va_list argList); 
CAPIHookMethod<PFN_ODD_Orig,PFN_ODD_Hook> 
	g_COleControlSite(MFCXXD_DLL, (PSTR)InvokeHelperV_COleControlSite_Ordinal, 
	&COleControlSite::InvokeHelperV, 
	&__COleControlSite__::__InvokeHelperV__, FALSE);
#endif
/*
       5554      0017AFE0 [NONAME] ?InvokeHelperV@COleControlSite@@UEAAXJGGPEAXPEBEPEAD@Z 
	(public: virtual void __cdecl COleControlSite::InvokeHelperV
			 (long,unsigned short,unsigned short,void *,unsigned char const *,char *))
*/
HOOK_METHOD(MFCXXD_DLL,5554,void,COleControlSite,InvokeHelperV,
	(DISPID dwDispID, WORD wFlags,
	VARTYPE vtRet, void* pvRet, const BYTE* pbParamInfo, va_list argList))
{
	if (m_dispDriver.m_lpDispatch == NULL)
	{
		// no dispatch pointer yet; find it now
		LPDISPATCH pDispatch;

		if ((m_pObject != NULL) &&
			SUCCEEDED(m_pObject->QueryInterface(IID_IDispatch,
				(LPVOID*)&pDispatch)))
		{
			ASSERT(pDispatch != NULL);
			m_dispDriver.AttachDispatch(pDispatch);
		}
	}

	if (m_dispDriver.m_lpDispatch == NULL)
	{
		// couldn't find dispatch pointer
		TRACE(traceOle, 0, "Warning: control has no IDispatch interface.");
		return;
	}

	// delegate call to m_dispDriver
	m_dispDriver.InvokeHelperV(dwDispID, wFlags, vtRet, pvRet, pbParamInfo,
		argList);
}
/*
	8394 mfc90d.dll!COleControlSite::SetPropertyV(long dwDispID=0xfffffdfa, unsigned short vtProp=0x000e, char * argList=0x0000000000128258)  Line 1126	C++
*/
HOOK_METHOD(MFCXXD_DLL,8394,void,COleControlSite,SetPropertyV,
(DISPID dwDispID, VARTYPE vtProp, va_list argList))
{
	BYTE rgbParams[2];
	if (vtProp & VT_BYREF)
	{
		vtProp &= ~VT_BYREF;
		vtProp |= VT_MFCBYREF;
	}

#if !defined(_UNICODE)
	if (vtProp == VT_BSTR)
		vtProp = VT_BSTRA;
#endif

	WORD wFlags;
	if (vtProp & VT_MFCFORCEPUTREF)
	{
		wFlags = DISPATCH_PROPERTYPUTREF;
		vtProp &= ~VT_MFCFORCEPUTREF;
	}
	else
	{
		if (vtProp == VT_DISPATCH)
			wFlags = DISPATCH_PROPERTYPUTREF;
		else
			wFlags = DISPATCH_PROPERTYPUT;
	}

	rgbParams[0] = (BYTE)vtProp;
	rgbParams[1] = 0;
	((TCOleControlSiteInvokeHelperV5554*)this)->Hook(dwDispID, wFlags, VT_EMPTY, NULL, rgbParams, argList);
}
/*
 	7759 mfc90d.dll!COleControlSite::SafeSetProperty(long dwDispID=0xfffffdfa, unsigned short vtProp=0x0008, ...)  Line 1162	C++
*/
HOOK_METHOD(MFCXXD_DLL,7759,BOOL,COleControlSite,SafeSetProperty,
(DISPID dwDispID, VARTYPE vtProp, ...))
{
	va_list argList;    // really only one arg, but...
	va_start(argList, vtProp);

	BOOL bSuccess;

	TRY
	{
		((TCOleControlSiteSetPropertyV8394*)this)->Hook(dwDispID, vtProp, argList);
		bSuccess = TRUE;
	}
	CATCH_ALL(e)
	{
		DELETE_EXCEPTION(e);
		bSuccess = FALSE;
	}
	END_CATCH_ALL

	va_end(argList);
	return bSuccess;
}
/*
5909      0017B570 [NONAME] ?ModifyStyle@COleControlSite@@UEAAHKKI@Z 
(public: virtual int __cdecl COleControlSite::ModifyStyle
(unsigned long,unsigned long,unsigned int))
*/
HOOK_METHOD(MFCXXD_DLL,5909,BOOL,COleControlSite,ModifyStyle,
(DWORD dwRemove, DWORD dwAdd, UINT nFlags))
{
	m_dwStyle = ((m_dwStyle & ~dwRemove) | dwAdd) & m_dwStyleMask;

	// Enabled property
	if ((dwRemove & WS_DISABLED) || (dwAdd & WS_DISABLED))
	{
		if (((TCOleControlSiteSafeSetProperty7759*)this)->Hook(DISPID_ENABLED, VT_BOOL, (~dwAdd & WS_DISABLED)))
		{
			dwRemove &= ~WS_DISABLED;
			dwAdd &= ~WS_DISABLED;
		}
	}

	// BorderStyle property
	if ((dwRemove & WS_BORDER) || (dwAdd & WS_BORDER))
	{
		if (((TCOleControlSiteSafeSetProperty7759*)this)->Hook(DISPID_BORDERSTYLE, VT_I2, (dwAdd & WS_BORDER)))
		{
			dwRemove &= ~WS_BORDER;
			dwAdd &= ~WS_BORDER;
		}
	}

   if( m_hWnd != NULL )
   {
	  return CWnd::ModifyStyle(m_hWnd, dwRemove, dwAdd, nFlags);
   }
   else
   {
	  return TRUE;
   }
}
/*
 	8605 mfc90d.dll!COleControlSite::SetWindowTextA(const char * lpszString=0x000000006be1a140)  Line 1286 + 0x23 bytes	C++
*/
HOOK_METHOD(MFCXXD_DLL,8605,void,COleControlSite,SetWindowText,
(LPCTSTR lpszString))
{
	// We do not assert m_hWnd is non-null since this is a valid case
	ENSURE(lpszString);

	if (!((TCOleControlSiteSafeSetProperty7759*)this)->Hook(DISPID_CAPTION, VT_BSTR, lpszString))
		((TCOleControlSiteSafeSetProperty7759*)this)->Hook(DISPID_TEXT, VT_BSTR, lpszString);
}
/*2233 
	mfc90d.dll!COleControlSite::CreateControlCommon(CWnd * pWndCtrl=0x000000000bb14450,
 const _GUID & clsid={...},
 const CControlCreationInfo & creationInfo={...},
 const char * lpszWindowName=0x000000006be1a140,
 unsigned long dwStyle=0x58000000,
 const tagPOINT * ppt=0x0000000000128700,
 const tagSIZE * psize=0x0000000000128708,
 unsigned int nID=0x00000000,
 CFile * pPersist=0x0000000000000000,
 int bStorage=0x00000000,
 wchar_t * bstrLicKey=0x0000000000000000)  Line 337	C++
*/
HOOK_METHOD(MFCXXD_DLL,2234,HRESULT,COleControlSite,CreateControlCommon,
(CWnd* pWndCtrl, REFCLSID clsid,const CControlCreationInfo& creationInfo,
	LPCTSTR lpszWindowName, DWORD dwStyle, const POINT* ppt, const SIZE* psize,
   UINT nID, CFile* pPersist, BOOL bStorage, BSTR bstrLicKey))
{
	HRESULT hr = E_FAIL;
	m_hWnd = NULL;
   CSize size;

	// Connect the OLE Control with its proxy CWnd object
	if (pWndCtrl != NULL)
	{
		ASSERT(((TCWndInitControlContainer5422*)pWndCtrl)->GetOCS() == NULL);
		m_pWndCtrl = pWndCtrl;
		((TCWndInitControlContainer5422*)pWndCtrl)->GetOCS() = this;
	}

	// Initialize OLE, if necessary
	_AFX_THREAD_STATE* pState = AfxGetThreadState();
	if (!pState->m_bNeedTerm && !AfxOleInit())
		return hr;

	if (creationInfo.IsManaged())
	{
		hr=CreateOrLoad(creationInfo);
	} else
	{
		hr=CreateOrLoad(clsid, pPersist, bStorage, bstrLicKey);
	}

	if (SUCCEEDED(hr))
	{
		ENSURE(m_pObject != NULL);
		m_nID = nID;

		ENSURE_ARG(ppt!=NULL);
		if (psize == NULL)
		{
			// If psize is NULL, ask the object how big it wants to be.
			CClientDC dc(NULL);

			m_pObject->GetExtent(DVASPECT_CONTENT, &size);
			dc.HIMETRICtoDP(&size);
			m_rect = CRect(*ppt, size);
		}
		else
		{
			m_rect = CRect(*ppt, *psize);
		}

		m_dwStyleMask = WS_GROUP | WS_TABSTOP;

		if (m_dwMiscStatus & OLEMISC_ACTSLIKEBUTTON)
			m_dwStyleMask |= BS_DEFPUSHBUTTON;

		if (m_dwMiscStatus & OLEMISC_INVISIBLEATRUNTIME)
			dwStyle &= ~WS_VISIBLE;

		m_dwStyle = dwStyle & m_dwStyleMask;

		// If control wasn't quick-activated, then connect sinks now.
		if (hr != S_FALSE)
		{
			m_dwEventSink = ConnectSink(m_iidEvents, &m_xEventSink);
			m_dwPropNotifySink = ConnectSink(IID_IPropertyNotifySink,
				&m_xPropertyNotifySink);
		}
		m_dwNotifyDBEvents = ConnectSink(IID_INotifyDBEvents, &m_xNotifyDBEvents);

		// Now that the object has been created, attempt to
		// in-place activate it.

		SetExtent();

		if (SUCCEEDED(hr = m_pObject->QueryInterface(IID_IOleInPlaceObject,
				(LPVOID*)&m_pInPlaceObject)))
		{
			if (dwStyle & WS_VISIBLE)
			{
				// control is visible: just activate it
				hr = DoVerb(OLEIVERB_INPLACEACTIVATE);
			}
			else
			{
				// control is not visible: activate off-screen, hide, then move
				m_rect.OffsetRect(-32000, -32000);
				if (SUCCEEDED(hr = DoVerb(OLEIVERB_INPLACEACTIVATE)) &&
					SUCCEEDED(hr = DoVerb(OLEIVERB_HIDE)))
				{
					m_rect.OffsetRect(32000, 32000);
					hr = m_pInPlaceObject->SetObjectRects(m_rect, m_rect);
				}
			}

			//Newly created control gets the focus.
			if (SUCCEEDED(hr))
			{
				m_pCtrlCont->m_pSiteFocus = this; 
			}
		}
		else
		{
			TRACE(traceOle, 0, "IOleInPlaceObject not supported on OLE control (dialog ID %d).\n", nID);
			TRACE(traceOle, 0, ">>> Result code: 0x%08lx\n", hr);
		}

		if (SUCCEEDED(hr))
			GetControlInfo();

		// if QueryInterface or activation failed, cleanup everything
		if (FAILED(hr))
		{
			if (m_pInPlaceObject != NULL)
			{
				m_pInPlaceObject->Release();
				m_pInPlaceObject = NULL;
			}
			DisconnectSink(m_iidEvents, m_dwEventSink);
			DisconnectSink(IID_IPropertyNotifySink, m_dwPropNotifySink);
			DisconnectSink(IID_INotifyDBEvents, m_dwNotifyDBEvents);
			m_dwEventSink = 0;
			m_dwPropNotifySink = 0;
			m_dwNotifyDBEvents = 0;
			m_pObject->Release();
			m_pObject = NULL;
		}
	}

	if (SUCCEEDED(hr))
	{
		AttachWindow();

		// Initialize the control's Caption or Text property, if any
		if (lpszWindowName != NULL)
			((TCOleControlSiteSetWindowText8605*)this)->Hook(lpszWindowName);

		// Initialize styles
		((TCOleControlSiteModifyStyle5909*)this)->Hook(0, m_dwStyle | (dwStyle & (WS_DISABLED|WS_BORDER)), 0);		
	}

	return hr;
}
/* 	
mfc90d.dll!COleControlSite::CreateControl(CWnd * pWndCtrl=0x000000000bb14450, const _GUID & clsid={...}, const char * lpszWindowName=0x000000006be1a140, unsigned long dwStyle=0x58000000, const tagPOINT * ppt=0x0000000000128700, const tagSIZE * psize=0x0000000000128708, unsigned int nID=0x00000000, CFile * pPersist=0x0000000000000000, int bStorage=0x00000000, wchar_t * bstrLicKey=0x0000000000000000)  Line 191	C++
*/
HOOK_METHOD(MFCXXD_DLL,2228,HRESULT,COleControlSite,CreateControl,
(CWnd* pWndCtrl, REFCLSID clsid,
	LPCTSTR lpszWindowName, DWORD dwStyle, const POINT* ppt, const SIZE* psize,
   UINT nID, CFile* pPersist, BOOL bStorage, BSTR bstrLicKey))
{
	CControlCreationInfo creationInfo;
	return ((TCOleControlSiteCreateControlCommon2234*)this)->Hook(pWndCtrl, clsid,creationInfo,
		lpszWindowName, dwStyle, ppt, psize,
		nID, pPersist, bStorage, bstrLicKey);
}
/*
mfc90d.dll!COleControlContainer::CreateControlCommon(CWnd * pWndCtrl=0x000000000bb14450, const _GUID & clsid={...}, const CControlCreationInfo & creationInfo={...}, const char * lpszWindowName=0x000000006be1a140, unsigned long dwStyle=0x58000000, const tagPOINT * ppt=0x0000000000128700, const tagSIZE * psize=0x0000000000128708, unsigned int nID=0x00000000, CFile * pPersist=0x0000000000000000, int bStorage=0x00000000, wchar_t * bstrLicKey=0x0000000000000000, COleControlSite * * ppNewSite=0x0000000000000000)  Line 306 + 0x81 bytes	C++
*/
HOOK_METHOD(MFCXXD_DLL,2233,BOOL,COleControlContainer,CreateControlCommon,
(CWnd* pWndCtrl, REFCLSID clsid,

	const CControlCreationInfo& creationInfo,
	LPCTSTR lpszWindowName, DWORD dwStyle, const POINT* ppt, const SIZE* psize,
   UINT nID, CFile* pPersist, BOOL bStorage, BSTR bstrLicKey,
   COleControlSite** ppNewSite))
{
	COleControlSite* pSite = NULL;
	COleControlSiteOrWnd *pSiteOrWnd = NULL;

	TRY
	{
	  BOOL bSuccess;
	  bSuccess = ((TCWndInitControlContainer5422*)m_pWnd)->CreateControlSite(this, &pSite, nID, clsid);
	  if (bSuccess && (pSite == NULL))
	  {
		 ENSURE(afxOccManager!=NULL);
		 // Use the default site.
		 pSite = afxOccManager->CreateSite(this,creationInfo);
	  }
	}
	END_TRY

	if (pSite == NULL)
		return FALSE;

	BOOL bCreated = FALSE;
	//If creation information, call that overload, else, call the CLSID overload
	if (creationInfo.IsManaged())
	{
		bCreated = SUCCEEDED( pSite->CreateControl(pWndCtrl, creationInfo,
			dwStyle, ppt, psize, nID) );
	} else
	{
		bCreated = SUCCEEDED(((TCOleControlSiteCreateControl2228*)pSite)->Hook(pWndCtrl, clsid,
		lpszWindowName, dwStyle, ppt, psize, nID, pPersist, bStorage, bstrLicKey ) );
	}

	if (bCreated)
	{
		//WinForms control on CDialog Visible=False --> The handle is not yet created.
		ASSERT( (pSite->m_hWnd != NULL) || pSite->m_bIsWindowless );
		pSiteOrWnd = new COleControlSiteOrWnd(pSite);
		m_listSitesOrWnds.AddTail(pSiteOrWnd);
		if( pSite->m_hWnd != NULL )
		{
		   m_siteMap.SetAt(pSite->m_hWnd, pSite);
		}
		if (ppNewSite != NULL)
			*ppNewSite = pSite;
	}
	else
	{
		delete pSite;
	}

	return bCreated;
}

//     5422      00173980 [NONAME] ?InitControlContainer@CWnd@@IEAAHH@Z (protected: int __cdecl CWnd::InitControlContainer(int))

// 	mfc90d.dll!COleControlContainer::CreateControl(CWnd * pWndCtrl=0x000000000bb14450,
//const _GUID & clsid={...},
//const char * lpszWindowName=0x000000006be1a140,
//unsigned long dwStyle=0x58000000,
//const tagPOINT * ppt=0x0000000000128700,
//const tagSIZE * psize=0x0000000000128708,
//unsigned int nID=0x00000000,
//CFile * pPersist=0x0000000000000000,
//int bStorage=0x00000000,
//wchar_t * bstrLicKey=0x0000000000000000,
//COleControlSite * * ppNewSite=0x0000000000000000)  Line 342	C++
HOOK_METHOD(MFCXXD_DLL,2224,BOOL,COleControlContainer,CreateControl,
(CWnd* pWndCtrl, REFCLSID clsid,
	LPCTSTR lpszWindowName, DWORD dwStyle, const POINT* ppt, const SIZE* psize,
   UINT nID, CFile* pPersist, BOOL bStorage, BSTR bstrLicKey,
   COleControlSite** ppNewSite))
{
	CControlCreationInfo controlInfo;
	controlInfo.m_clsid=clsid;
	return CreateControlCommon(pWndCtrl, clsid,controlInfo,lpszWindowName, 
								dwStyle, ppt, psize,nID, pPersist, bStorage, bstrLicKey,
								ppNewSite);
}
// 	mfc90d.dll!CWnd::CreateControl(const _GUID & clsid={...},
//const char * lpszWindowName=0x000000006be1a140,
//unsigned long dwStyle=0x58000000,
//const tagPOINT * ppt=0x0000000000128700,
//const tagSIZE * psize=0x0000000000128708,
//CWnd * pParentWnd=0x000000000bb137f8,
//unsigned int nID=0x00000000,
//CFile * pPersist=0x0000000000000000,
//int bStorage=0x00000000,
//wchar_t * bstrLicKey=0x0000000000000000)  Line 71	C++

HOOK_METHOD(MFCXXD_DLL,2230,BOOL,CWnd,CreateControl,
(REFCLSID clsid, LPCTSTR lpszWindowName, DWORD dwStyle,
	const POINT* ppt, const SIZE* psize, CWnd* pParentWnd, UINT nID,
   CFile* pPersist, BOOL bStorage, BSTR bstrLicKey))
{
	ASSERT(pParentWnd != NULL);

#ifdef _DEBUG
	if (afxOccManager == NULL)
	{
		TRACE(traceOle, 0, "Warning: AfxEnableControlContainer has not been called yet.\n");
		TRACE(traceOle, 0, ">>> You should call it in your app's InitInstance function.\n");
	}
#endif

	if ((pParentWnd == NULL) || !((TCWndInitControlContainer5422*)pParentWnd)->InitControlContainer())
		return FALSE;

	return ((TCWndInitControlContainer5422*)pParentWnd)->GetOCC()->CreateControl(this, clsid, lpszWindowName,
		dwStyle, ppt, psize, nID, pPersist, bStorage, bstrLicKey);
}
	// 	mfc90d.dll!CWnd::CreateControl
	//(const _GUID & clsid={...}, const char * lpszWindowName=0x000000006be1a140, 
	//	unsigned long dwStyle=0x58000000, const tagRECT & rect={...}
	//, CWnd * pParentWnd=0x000000000bb137f8, unsigned int nID=0x00000000
	//	, CFile * pPersist=0x0000000000000000, int bStorage=0x00000000, wchar_t * bstrLicKey=0x0000000000000000)

HOOK_METHOD(MFCXXD_DLL,2229,BOOL,CWnd,CreateControl,
( REFCLSID clsid, LPCTSTR lpszWindowName,
   DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID,
   CFile* pPersist, BOOL bStorage, BSTR bstrLicKey ))
{
   CRect rect2( rect );
   CPoint pt;
   CSize size;

   pt = rect2.TopLeft();
   size = rect2.Size();

   return( CreateControl( clsid, lpszWindowName, dwStyle, &pt, &size,
	  pParentWnd, nID, pPersist, bStorage, bstrLicKey ) );
}
// 	mfc90d.dll!CWnd::CreateControl(const char * lpszClass=0x000000000a766a28, const char * lpszWindowName=0x000000006be1a140, unsigned long dwStyle=0x58000000, const tagRECT & rect={...}, CWnd * pParentWnd=0x000000000bb137f8, unsigned int nID=0x00000000, CFile * pPersist=0x0000000000000000, int bStorage=0x00000000, wchar_t * bstrLicKey=0x0000000000000000)  Line 35	C++
HOOK_METHOD(MFCXXD_DLL,2232,BOOL,CWnd,CreateControl,
	(LPCTSTR lpszClass, LPCTSTR lpszWindowName, 
	DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, 
	CFile* pPersist, BOOL bStorage, BSTR bstrLicKey))
{
	ASSERT(lpszClass != NULL);

	CLSID clsid;
	HRESULT hr = AfxGetClassIDFromString(lpszClass, &clsid);
	if (FAILED(hr))
		return FALSE;

	return CreateControl(clsid, lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey);
}
/*
5551      0017B230 [NONAME] ?InvokeHelper@COleControlSite@@UEAAXJGGPEAXPEBEZZ 
(public: virtual void __cdecl COleControlSite::InvokeHelper
(long,unsigned short,unsigned short,void *,unsigned char const *,...))
*/
HOOK_METHOD(MFCXXD_DLL,5551,void,COleControlSite,InvokeHelper,
(DISPID dwDispID, WORD wFlags, VARTYPE vtRet,
	void* pvRet, const BYTE* pbParamInfo, ...))
{
	va_list argList;
	va_start(argList, pbParamInfo);
	((TCOleControlSiteInvokeHelperV5554*)this)->Hook(dwDispID, wFlags, vtRet, pvRet, pbParamInfo, argList);
	va_end(argList);
}

/*
  4511      0017B2C0 [NONAME] ?GetProperty@COleControlSite@@UEBAXJGPEAX@Z 
	(public: virtual void __cdecl COleControlSite::GetProperty
	(long,unsigned short,void *)const )
*/
HOOK_METHOD(MFCXXD_DLL,4511,void,COleControlSite,GetProperty,
(DISPID dwDispID, VARTYPE vtProp,
	void* pvProp))
{
	((TCOleControlSiteInvokeHelper5551*)this)->Hook(dwDispID,
		DISPATCH_PROPERTYGET, vtProp, pvProp, NULL);
}
/*
3881      0017B4F0 [NONAME] ?GetExStyle@COleControlSite@@UEBAKXZ (public: virtual unsigned long __cdecl COleControlSite::GetExStyle(void)const )
*/   
HOOK_METHOD(MFCXXD_DLL,3881,DWORD,COleControlSite,GetExStyle,
())
{
	DWORD dwExStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);

	TRY
	{
		short sAppearance = 0;
		((TCOleControlSiteGetProperty4511*)this)->Hook(DISPID_APPEARANCE, VT_I2, &sAppearance);
		if (sAppearance == 1)
			dwExStyle |= WS_EX_CLIENTEDGE;
	}
	END_TRY

	return dwExStyle;
}
/*
3882      000ADD80 [NONAME] ?GetExStyle@CWnd@@QEBAKXZ (public: unsigned long __cdecl CWnd::GetExStyle(void)const )
*/
HOOK_METHOD(MFCXXD_DLL,3882,DWORD,CWnd,GetExStyle,
())
{
	ASSERT(::IsWindow(m_hWnd) || (m_pCtrlSite != NULL));

	if (m_pCtrlSite == NULL)
		return (DWORD)GetWindowLong(m_hWnd, GWL_EXSTYLE);
	else
		return ((TCOleControlSiteGetExStyle3881*)m_pCtrlSite)->Hook();
}
