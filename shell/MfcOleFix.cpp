#include "stdafx.h"
#include "CmnHdr.h"
#include <ImageHlp.h>
#pragma comment(lib, "ImageHlp")
#include "\vt5\awin\misc_map.h"
#include "MfcFix.h"
#include "APIHook.h"
#include "MfcOleFix.h"
//#define CWnd_InitControlContainer_Ordinal 8370
//HOOK_METHOD(MFCXXD_DLL,CWnd_InitControlContainer_Ordinal,BOOL,CWnd,InitControlContainer,
struct TCWndInitControlContainerCWnd_InitControlContainer_Ordinal: public CWnd 
{ 
	typedef BOOL (TCWndInitControlContainerCWnd_InitControlContainer_Ordinal::*TMethodHook)(BOOL bCreateFromResource); 
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
 
CAPIHookMethod<CWnd_InitControlContainer_Ordinal,BOOL (CWnd::*) (BOOL bCreateFromResource), TCWndInitControlContainerCWnd_InitControlContainer_Ordinal::TMethodHook>
	g_CWndInitControlContainerCWnd_InitControlContainer_Ordinal(MFCXXD_DLL, &TCWndInitControlContainerCWnd_InitControlContainer_Ordinal::Hook, FALSE); 
BOOL TCWndInitControlContainerCWnd_InitControlContainer_Ordinal::Hook (BOOL bCreateFromResource)
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

HOOK_METHOD(MFCXXD_DLL,COleControlSite_InvokeHelperV_Ordinal,void,COleControlSite,InvokeHelperV,
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

#define COleControlSite_SetPropertyV_Ordinal 14609
HOOK_METHOD(MFCXXD_DLL,COleControlSite_SetPropertyV_Ordinal,void,COleControlSite,SetPropertyV,
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
	((TCOleControlSiteInvokeHelperVCOleControlSite_InvokeHelperV_Ordinal*)this)->Hook(dwDispID, wFlags, VT_EMPTY, NULL, rgbParams, argList);
}
/*
 	7759 mfc90d.dll!COleControlSite::SafeSetProperty(long dwDispID=0xfffffdfa, unsigned short vtProp=0x0008, ...)  Line 1162	C++
*/
HOOK_METHOD(MFCXXD_DLL,COleControlSite_SafeSetProperty_Ordinal,BOOL,COleControlSite,SafeSetProperty,
(DISPID dwDispID, VARTYPE vtProp, ...))
{
	va_list argList;    // really only one arg, but...
	va_start(argList, vtProp);

	BOOL bSuccess;

	TRY
	{
		((TCOleControlSiteSetPropertyVCOleControlSite_SetPropertyV_Ordinal*)this)->Hook(dwDispID, vtProp, argList);
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
HOOK_METHOD(MFCXXD_DLL,COleControlSite_ModifyStyle_Ordinal,BOOL,COleControlSite,ModifyStyle,
(DWORD dwRemove, DWORD dwAdd, UINT nFlags))
{
	m_dwStyle = ((m_dwStyle & ~dwRemove) | dwAdd) & m_dwStyleMask;

	// Enabled property
	if ((dwRemove & WS_DISABLED) || (dwAdd & WS_DISABLED))
	{
		if (((TCOleControlSiteSafeSetPropertyCOleControlSite_SafeSetProperty_Ordinal*)this)->Hook(DISPID_ENABLED, VT_BOOL, (~dwAdd & WS_DISABLED)))
		{
			dwRemove &= ~WS_DISABLED;
			dwAdd &= ~WS_DISABLED;
		}
	}

	// BorderStyle property
	if ((dwRemove & WS_BORDER) || (dwAdd & WS_BORDER))
	{
		if (((TCOleControlSiteSafeSetPropertyCOleControlSite_SafeSetProperty_Ordinal*)this)->Hook(DISPID_BORDERSTYLE, VT_I2, (dwAdd & WS_BORDER)))
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

	if (!((TCOleControlSiteSafeSetPropertyCOleControlSite_SafeSetProperty_Ordinal*)this)->Hook(DISPID_CAPTION, VT_BSTR, lpszString))
		((TCOleControlSiteSafeSetPropertyCOleControlSite_SafeSetProperty_Ordinal*)this)->Hook(DISPID_TEXT, VT_BSTR, lpszString);
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
HOOK_METHOD(MFCXXD_DLL,COleControlSite_CreateControlCommon_Ordinal,HRESULT,COleControlSite,CreateControlCommon,
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
		ASSERT(((TCWndInitControlContainerCWnd_InitControlContainer_Ordinal*)pWndCtrl)->GetOCS() == NULL);
		m_pWndCtrl = pWndCtrl;
		((TCWndInitControlContainerCWnd_InitControlContainer_Ordinal*)pWndCtrl)->GetOCS() = this;
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
		((TCOleControlSiteModifyStyleCOleControlSite_ModifyStyle_Ordinal*)this)->Hook(0, m_dwStyle | (dwStyle & (WS_DISABLED|WS_BORDER)), 0);		
	}

	return hr;
}
/* 	
mfc90d.dll!COleControlSite::CreateControl(CWnd * pWndCtrl=0x000000000bb14450, const _GUID & clsid={...}, const char * lpszWindowName=0x000000006be1a140, unsigned long dwStyle=0x58000000, const tagPOINT * ppt=0x0000000000128700, const tagSIZE * psize=0x0000000000128708, unsigned int nID=0x00000000, CFile * pPersist=0x0000000000000000, int bStorage=0x00000000, wchar_t * bstrLicKey=0x0000000000000000)  Line 191	C++
*/
HOOK_METHOD(MFCXXD_DLL,COleControlSite_CreateControl_5POINT_Ordinal,HRESULT,COleControlSite,CreateControl,
(CWnd* pWndCtrl, REFCLSID clsid,
	LPCTSTR lpszWindowName, DWORD dwStyle, const POINT* ppt, const SIZE* psize,
   UINT nID, CFile* pPersist, BOOL bStorage, BSTR bstrLicKey))
{
	CControlCreationInfo creationInfo;
	return ((TCOleControlSiteCreateControlCommonCOleControlSite_CreateControlCommon_Ordinal*)this)->Hook(pWndCtrl, clsid,creationInfo,
		lpszWindowName, dwStyle, ppt, psize,
		nID, pPersist, bStorage, bstrLicKey);
}
/*
mfc90d.dll!COleControlContainer::CreateControlCommon(CWnd * pWndCtrl=0x000000000bb14450, const _GUID & clsid={...}, const CControlCreationInfo & creationInfo={...}, const char * lpszWindowName=0x000000006be1a140, unsigned long dwStyle=0x58000000, const tagPOINT * ppt=0x0000000000128700, const tagSIZE * psize=0x0000000000128708, unsigned int nID=0x00000000, CFile * pPersist=0x0000000000000000, int bStorage=0x00000000, wchar_t * bstrLicKey=0x0000000000000000, COleControlSite * * ppNewSite=0x0000000000000000)  Line 306 + 0x81 bytes	C++
*/
HOOK_METHOD(MFCXXD_DLL,COleControlContainer_CreateControlCommon_Ordinal,BOOL,COleControlContainer,CreateControlCommon,
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
	  bSuccess = ((TCWndInitControlContainerCWnd_InitControlContainer_Ordinal*)m_pWnd)->CreateControlSite(this, &pSite, nID, clsid);
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
		bCreated = SUCCEEDED(((TCOleControlSiteCreateControlCOleControlSite_CreateControl_5POINT_Ordinal*)pSite)->Hook(pWndCtrl, clsid,
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
HOOK_METHOD(MFCXXD_DLL,COleControlContainer_CreateControl_2GUID_5POINT_Ordinal,BOOL,COleControlContainer,CreateControl,
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

HOOK_METHOD(MFCXXD_DLL,CWnd_CreateControl_4POINT_Ordinal,BOOL,CWnd,CreateControl,
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

	if ((pParentWnd == NULL) || !((TCWndInitControlContainerCWnd_InitControlContainer_Ordinal*)pParentWnd)->InitControlContainer())
		return FALSE;

	return ((TCWndInitControlContainerCWnd_InitControlContainer_Ordinal*)pParentWnd)->GetOCC()->CreateControl(this, clsid, lpszWindowName,
		dwStyle, ppt, psize, nID, pPersist, bStorage, bstrLicKey);
}
	// 	mfc90d.dll!CWnd::CreateControl
	//(const _GUID & clsid={...}, const char * lpszWindowName=0x000000006be1a140, 
	//	unsigned long dwStyle=0x58000000, const tagRECT & rect={...}
	//, CWnd * pParentWnd=0x000000000bb137f8, unsigned int nID=0x00000000
	//	, CFile * pPersist=0x0000000000000000, int bStorage=0x00000000, wchar_t * bstrLicKey=0x0000000000000000)

HOOK_METHOD(MFCXXD_DLL,CWnd_CreateControl_4RECT_Ordinal,BOOL,CWnd,CreateControl,
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
HOOK_METHOD(MFCXXD_DLL,CWnd_CreateControl_Ordinal_1char,BOOL,CWnd,CreateControl,
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
HOOK_METHOD(MFCXXD_DLL,COleControlSite_GetProperty_Ordinal,void,COleControlSite,InvokeHelper,
(DISPID dwDispID, WORD wFlags, VARTYPE vtRet,
	void* pvRet, const BYTE* pbParamInfo, ...))
{
	va_list argList;
	va_start(argList, pbParamInfo);
	((TCOleControlSiteInvokeHelperVCOleControlSite_InvokeHelperV_Ordinal*)this)->Hook(dwDispID, wFlags, vtRet, pvRet, pbParamInfo, argList);
	va_end(argList);
}

/*
  4511      0017B2C0 [NONAME] ?GetProperty@COleControlSite@@UEBAXJGPEAX@Z 
	(public: virtual void __cdecl COleControlSite::GetProperty
	(long,unsigned short,void *)const )
*/
HOOK_METHOD(MFCXXD_DLL,COleControlSite_GetProperty_Ordinal,void,COleControlSite,GetProperty,
(DISPID dwDispID, VARTYPE vtProp,
	void* pvProp))
{
	((TCOleControlSiteInvokeHelperCOleControlSite_GetProperty_Ordinal*)this)->Hook(dwDispID,
		DISPATCH_PROPERTYGET, vtProp, pvProp, NULL);
}
/*
3881      0017B4F0 [NONAME] ?GetExStyle@COleControlSite@@UEBAKXZ (public: virtual unsigned long __cdecl COleControlSite::GetExStyle(void)const )
*/   
HOOK_METHOD(MFCXXD_DLL,COleControlSite_GetExStyle_Ordinal,DWORD,COleControlSite,GetExStyle,
())
{
	DWORD dwExStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);

	TRY
	{
		short sAppearance = 0;
		((TCOleControlSiteGetPropertyCOleControlSite_GetProperty_Ordinal*)this)->Hook(DISPID_APPEARANCE, VT_I2, &sAppearance);
		if (sAppearance == 1)
			dwExStyle |= WS_EX_CLIENTEDGE;
	}
	END_TRY

	return dwExStyle;
}
/*
3882      000ADD80 [NONAME] ?GetExStyle@CWnd@@QEBAKXZ (public: unsigned long __cdecl CWnd::GetExStyle(void)const )
*/
HOOK_METHOD(MFCXXD_DLL,CWnd_GetExStyle_Ordinal,DWORD,CWnd,GetExStyle,
())
{
	ASSERT(::IsWindow(m_hWnd) || (m_pCtrlSite != NULL));

	if (m_pCtrlSite == NULL)
		return (DWORD)GetWindowLong(m_hWnd, GWL_EXSTYLE);
	else
		return ((TCOleControlSiteGetExStyleCOleControlSite_GetExStyle_Ordinal*)m_pCtrlSite)->Hook();
}
