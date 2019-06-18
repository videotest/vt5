// ActiveXSite.cpp : implementation file
//

#include "stdafx.h"
#include "axform.h"
#include "ActiveXSite.h"
#include "afxpriv.h"
#include "ScriptNotifyInt.h"
#include "button_messages.h"
#include "FormManager.h"

// [vanek] : register message WM_SETCANCLOSEWND - 22.10.2004
UINT WM_SETCANCLOSEWND = RegisterWindowMessage("SET_CAN_CLOSE_WINDOW");

//#pragma message( "Linking with htmlhelp.lib" )
//#pragma comment(lib, "htmlhelp.lib")


BEGIN_MESSAGE_MAP(CAxWnd, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CAxWnd::OnPaint()
{
	{
		CPaintDC	dc( this );
		CRect	rect;
		GetClientRect( rect );

		dc.SelectStockObject( BLACK_PEN );
		dc.SelectStockObject( WHITE_BRUSH );
		dc.SelectStockObject( SYSTEM_FONT );
		dc.Rectangle( rect );
		rect.InflateRect( -2, -2 );

		dc.SetBkMode( TRANSPARENT );
		dc.SetTextColor( RGB( 255, 0, 0 ) );

		CString	str;
		str.Format( "Can't create control\nName - %s\nProgID - %s", 
			(const char *)m_strControlName, (const char *)m_strControlProgID );

		dc.DrawText( str, rect, DT_WORD_ELLIPSIS );
	}
}



/////////////////////////////////////////////////////////////////////////////
// CActiveXSite

extern GUID IID_ITypeLibID;

// {4666C0CC-FD8F-48ef-9F7B-2D9D5F3E9C99}
GUARD_IMPLEMENT_OLECREATE(CActiveXSite, "axform.ActiveXSite", 
0x4666c0cc, 0xfd8f, 0x48ef, 0x9f, 0x7b, 0x2d, 0x9d, 0x5f, 0x3e, 0x9c, 0x99);
// {9B6C9468-F9C4-460f-BD44-5D41BAF3205D}
static const IID IID_IActiveXSiteEvents = 
{ 0x9b6c9468, 0xf9c4, 0x460f, { 0xbd, 0x44, 0x5d, 0x41, 0xba, 0xf3, 0x20, 0x5d } };
// {C7D2F99D-E12E-4edf-8FB4-0A77826EB865}
static const IID IID_IActiveXSiteDispatch = 
{ 0xc7d2f99d, 0xe12e, 0x4edf, { 0x8f, 0xb4, 0xa, 0x77, 0x82, 0x6e, 0xb8, 0x65 } };


IMPLEMENT_DYNCREATE(CActiveXSite, CWnd)
IMPLEMENT_OLETYPELIB(CActiveXSite, IID_ITypeLibID, 1, 0)



CActiveXSite::CActiveXSite()
{
	EnableAutomation();
	EnableConnections();

	m_piidEvents = &IID_IActiveXSiteEvents;
	m_piidPrimary = &IID_IActiveXSiteDispatch;

	m_sName = _T( "Form" );

	m_bTerminated = false;
	m_bClosingWindow = false;
	
	
	_OleLockApp( this );

	m_nEdgeType = 0;// 0 - вогнутая; 1 - выпуклая; 2 - никакая

	//registration moved to OnCreate...
	m_bHelpDisplayed = false;
	m_bContinueModal = false;
	m_bFormInited = false;
	m_bCancelSheduled = false;

	m_dw_script_context = -1;
	m_bLockFireEvent = false;
}

CActiveXSite::~CActiveXSite()
{
	if( m_bProcessPreview )
		DoLeavePreviewMode();

	IScriptSitePtr	sptrSSite = ::GetAppUnknown();
	if( sptrSSite != 0 )
		sptrSSite->UnregisterScript( m_ptrActiveScript, fwFormScript );


	if( m_ptrActiveScript )
	{
		SCRIPTSTATE ss;
		if( m_ptrActiveScript->GetScriptState(&ss) == S_OK && ss == SCRIPTSTATE_CONNECTED )
		{
			m_ptrActiveScript->SetScriptState( SCRIPTSTATE_DISCONNECTED );
		}
	}

	m_ptrActiveScriptParse = 0;

	if( m_ptrActiveScript != 0 )
		m_ptrActiveScript->Close();
	
	m_ptrActiveScript = 0;

	//Active Scripting debug support
	deinit_debuger();

	POSITION	pos = m_windows.GetHeadPosition();

	while( pos )
		delete m_windows.GetNext( pos );

	m_nIDEvent = 0;
	m_nIDTimer = 0;
	
	_OleUnlockApp( this );
}

void CActiveXSite::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	if( GetSafeHwnd() )
		DestroyWindow();

	//CWnd::OnFinalRelease();
	delete this;
}


BEGIN_MESSAGE_MAP(CActiveXSite, CWnd)
	//{{AFX_MSG_MAP(CActiveXSite)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
	ON_WM_HELPINFO()
	ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
	ON_MESSAGE(WM_GETINTERFACE, OnGetInterface)
	ON_MESSAGE(WM_INITIALIZE_FORM, OnFireInitialize)
	ON_MESSAGE(WM_FIRE_ON_CREATE_FORM, OnFireCreateForm)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CActiveXSite, CWnd)
	//{{AFX_DISPATCH_MAP(CActiveXSite)
	DISP_PROPERTY_NOTIFY(CActiveXSite, "Title", m_strTitle, OnTitleChanged, VT_BSTR)
	DISP_FUNCTION(CActiveXSite, "ApplyPreview", ApplyPreview, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CActiveXSite, "CancelPreview", CancelPreview, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CActiveXSite, "AddHeadActionArgument", AddHeadActionArgument, VT_EMPTY, VTS_DISPATCH)
	DISP_FUNCTION(CActiveXSite, "InitializePreview", InitializePreview, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CActiveXSite, "UpdateSystemSettings", UpdateSystemSettings, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CActiveXSite, "ContiueModal", ContiueModal, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CActiveXSite, "debug_break", debug_break, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CActiveXSite, "GetFormScript", GetFormScript, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CActiveXSite, "BeginFilterGroup", BeginFilterGroup, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CActiveXSite, "EndFilterGroup", EndFilterGroup, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CActiveXSite, "EnableButtons", EnableButtons, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CActiveXSite, "ShowButtons", ShowButtons, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CActiveXSite, "TerminateCurrentAction", TerminateCurrentAction, VT_EMPTY, VTS_NONE)

	DISP_FUNCTION(CActiveXSite, "Preview2_Init", Preview2_Init, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CActiveXSite, "Preview2_DeInit", Preview2_DeInit, VT_EMPTY, VTS_NONE)
	DISP_PROPERTY_EX(CActiveXSite, "Preview2_StagesCount", Preview2_GetStagesCount, Preview2_SetStagesCount, VT_I4)
	DISP_FUNCTION(CActiveXSite, "Preview2_LoadState", Preview2_LoadState, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CActiveXSite, "Preview2_SaveState", Preview2_SaveState, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CActiveXSite, "Preview2_Invalidate", Preview2_Invalidate, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CActiveXSite, "Preview2_Cancel", Preview2_Cancel, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CActiveXSite, "Preview2_Apply", Preview2_Apply, VT_EMPTY, VTS_NONE)

	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IActiveXSite to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

/*// {0B785B62-A7E4-4DDA-8B71-9599A7C0AA03}
static const IID IID_IActiveXSite =
{ 0xb785b62, 0xa7e4, 0x4dda, { 0x8b, 0x71, 0x95, 0x99, 0xa7, 0xc0, 0xaa, 0x3 } };*/

BEGIN_INTERFACE_MAP(CActiveXSite, CWnd)
	INTERFACE_PART(CActiveXSite, IID_IActiveXSiteDispatch, Dispatch)
    INTERFACE_PART(CActiveXSite, IID_IActiveScriptSite, Site)
    INTERFACE_PART(CActiveXSite, IID_IActiveScriptSiteWindow, SiteWnd)
	INTERFACE_PART(CActiveXSite, IID_IConnectionPointContainer, ConnPtContainer)
	INTERFACE_PART(CActiveXSite, IID_IProvideClassInfo, ProvideClassInfo)
	INTERFACE_PART(CActiveXSite, IID_INamedObject2, Name)
	INTERFACE_PART(CActiveXSite, IID_IAXSite, AxSite)
	INTERFACE_PART(CActiveXSite, IID_IPreviewSite, Preview)
	INTERFACE_PART(CActiveXSite, IID_IHelpInfo, Help)
	INTERFACE_PART(CActiveXSite, IID_IHelpInfo2, Help)
	INTERFACE_AGGREGATE(CActiveXSite, m_punkScriptDebug)	
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CActiveXSite, Site);
IMPLEMENT_UNKNOWN(CActiveXSite, SiteWnd);
IMPLEMENT_UNKNOWN(CActiveXSite, AxSite);
IMPLEMENT_UNKNOWN(CActiveXSite, Help);

BEGIN_EVENT_MAP(CActiveXSite, COleEventSourceImpl)
	//{{AFX_EVENT_MAP(CActiveXSite)
	EVENT_CUSTOM("OnOK", FireOnOK, VTS_NONE)
	EVENT_CUSTOM("OnCancel", FireOnCancel, VTS_NONE)
	EVENT_CUSTOM("OnLoadState", FireOnLoadState, VTS_DISPATCH)
	EVENT_CUSTOM("OnSaveState", FireOnSaveState, VTS_DISPATCH)
	EVENT_CUSTOM("OnInitialize", FireOnInitialize, VTS_NONE)
	EVENT_CUSTOM("OnDestroy", FireOnDestroy, VTS_NONE)
	EVENT_CUSTOM("OnNext", FireOnNext, VTS_NONE)
	EVENT_CUSTOM("OnPrev", FireOnPrev, VTS_NONE)
	EVENT_CUSTOM("OnChangePreview", FireOnChangePreview, VTS_NONE)
	EVENT_CUSTOM("OnShow", FireOnShow, VTS_NONE)
	EVENT_CUSTOM("OnCreate", FireOnCreate, VTS_NONE)	
	EVENT_CUSTOM("OnHide", FireOnHide, VTS_NONE)
	EVENT_CUSTOM("OnProcess", FireOnProcess, VTS_I4)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()

/////////////////////////////////////////////////////////////////////////////
// CActiveXSite message handlers

void CActiveXSite::SetForm( IUnknown *punkForm )
{
	m_ptrForm = punkForm;
	GetCaption();
}

///////////////////////////////////////////////////////////////////////////////
//init script engine
HRESULT CActiveXSite::_CreateScriptEngine()
{
	init_debugger( DEBUGGER_FORM_NAME );

	m_ptrActiveScript.CreateInstance( _T("VBScript") );
	m_ptrActiveScriptParse = m_ptrActiveScript;

	m_ptrActiveScript->SetScriptSite( &m_xSite );
	m_ptrActiveScriptParse->InitNew();

	//add "Form" to namespace
	{
		DWORD	dwAddFlag = SCRIPTITEM_ISVISIBLE|SCRIPTITEM_ISSOURCE;
		CString strName = ::GetObjectName( GetControllingUnknown() );
		BSTR	bstr = strName.AllocSysString();
		m_ptrActiveScript->AddNamedItem( bstr, dwAddFlag );
		::SysFreeString( bstr );
	}

	//add "ActionHelper" to namespace
	{
		DWORD	dwAddFlag = SCRIPTITEM_ISVISIBLE;
		CString strName = ::GetObjectName( m_helper.GetControllingUnknown() );
		BSTR	bstr = strName.AllocSysString();
		m_ptrActiveScript->AddNamedItem( bstr, dwAddFlag );
		::SysFreeString( bstr );
	}


	for( int i = 0; i < _GetAppComponentCount(); i++ )
	{
		IUnknown *punk = _GetAppComponent( i );

		if( !CheckInterface( punk, IID_INamedObject2 ) )
		{
			punk->Release();
			continue;
		}
//check, if item provide class info for himself
		DWORD	dwAddFlag = SCRIPTITEM_ISVISIBLE;

		if( CheckInterface( punk, IID_IProvideClassInfo ) )
			dwAddFlag |= SCRIPTITEM_ISSOURCE;
		if( CheckInterface( punk, IID_IActionManager ) )
			dwAddFlag |= SCRIPTITEM_GLOBALMEMBERS;

		CString strName = ::GetObjectName( punk );
		punk->Release();

		BSTR	bstr = strName.AllocSysString();

		m_ptrActiveScript->AddNamedItem( bstr, dwAddFlag );

		::SysFreeString( bstr );
	}

	POSITION	pos = m_windows.GetHeadPosition();

	while( pos )
	{
		CAxWnd	*pwnd = m_windows.GetNext( pos );
		IUnknown	*punkCtrl = pwnd->GetControlUnknown();
		IUnknown	*punkData = pwnd->GetControlObjectUnknown();

		if( !punkCtrl )continue;

//check, if item provide class info for himself
		DWORD	dwAddFlag = SCRIPTITEM_ISVISIBLE;

		if( CheckInterface( punkCtrl, IID_IProvideClassInfo ) )
			dwAddFlag |= SCRIPTITEM_ISSOURCE;


		BSTR	bstr = 0;
		IActiveXCtrlPtr	ptrCtrl( punkData );
		ptrCtrl->GetName( &bstr );
		
		m_ptrActiveScript->AddNamedItem( bstr, dwAddFlag );

		::SysFreeString( bstr );
	}	

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//execute script
HRESULT	CActiveXSite::_ExecuteFormScript()
{
	IActiveXFormPtr	ptrForm( m_ptrForm );
	IUnknown	*punkScript = 0;
	ptrForm->GetScript( &punkScript );

	IScriptDataObjectPtr	ptrScript( punkScript );

	HRESULT hr = 0;

	if( punkScript )
	{
		punkScript->Release();

		BSTR	bstrScript = 0;
		ptrScript->GetText( &bstrScript );

		if( bstrScript )
		{
			//Active Scripting debug support
			CString strScriptText(bstrScript);
			DWORD dwContext = 0;
			//SetScriptText(strScriptText, &dwContext);

			CString str_name = ::GetObjectName( m_ptrForm );

			IDebugHostPtr ptr_debug = get_debugger();
			if( ptr_debug )
				ptr_debug->OnRunScript( _bstr_t( strScriptText ), 
										_bstr_t( str_name ), 
										_bstr_t( str_name ),
										m_ptrActiveScript, &m_dw_script_context );

			EXCEPINFO ei;      
			if( m_ptrActiveScriptParse->ParseScriptText( bstrScript, NULL, NULL, NULL, dwContext, 0, 0L, NULL, &ei ) == S_OK )
				m_ptrActiveScript->SetScriptState( SCRIPTSTATE_CONNECTED );

			::SysFreeString( bstrScript );
		}
	
		IScriptSitePtr	sptrSSite = ::GetAppUnknown();
		if( sptrSSite != 0 )
			sptrSSite->RegisterScript( m_ptrActiveScript, fwFormScript );
	}

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//load controls	
HRESULT	CActiveXSite::_LoadControls()
{
	ASSERT( m_ptrForm!=0 );
	CObjectListWrp	list( m_ptrForm );

	POSITION	pos = list.GetFirstObjectPosition();

	while( pos )
	{
		IUnknown	*punkControl = list.GetNextObject( pos );

		DWORD	dwStyle;
		UINT	nID;
		CRect	rect;


		BSTR	bstrProgID = 0;
		sptrIActiveXCtrl	sptrC( punkControl );
		sptrC->GetProgID( &bstrProgID );
		sptrC->GetRect( &rect );
		sptrC->GetStyle( &dwStyle );
		sptrC->GetControlID( &nID );

		CString strProgID = bstrProgID;
		::SysFreeString( bstrProgID );
		CString	strName = ::GetObjectName( punkControl );

		CAxWnd	*pwnd = new CAxWnd();
		pwnd->SetControlObjectUnknown( punkControl );
		pwnd->m_strControlProgID = strProgID;
		pwnd->m_strControlName = strName;

		punkControl->Release();

		pwnd->CreateControl( strProgID, strName, dwStyle, rect, this, nID );
		SetWindowLong( *pwnd, GWL_ID, nID );

		UINT	n = pwnd->GetDlgCtrlID();
		
		DWORD	dwStyleEx = pwnd->GetExStyle();
//		dwStyleEx |= WS_EX_CONTEXTHELP;
		SetWindowLong( *pwnd, GWL_EXSTYLE, dwStyleEx );

		pwnd->SetWindowContextHelpId( nID );

		UINT	n1 = pwnd->GetDlgCtrlID();

		
		

		IUnknown	*punk = pwnd->GetControlUnknown();

		if( punk )
		{
			::RestoreContainerFromDataObject( sptrC, punk );

			CSize size( rect.Size() );
			CClientDC dc(NULL);
			dc.DPtoHIMETRIC(&size);
			
			IOleObjectPtr ptrOLECtrl( punk );
			if( ptrOLECtrl )
			{
				VERIFY( S_OK == ptrOLECtrl->SetExtent( DVASPECT_CONTENT, &size ) );
			}
			

			//if( CheckInterface( punk, IID_IVtActiveXCtrl ) )
			//{
			//	IVtActiveXCtrlPtr	sptrVTC( punk );
			//	sptrVTC->SetSite( GetAppUnknown(), GetControllingUnknown() );
			//}
		}
		else
		{
			pwnd->Create( "static", strName, WS_CHILD|WS_VISIBLE, rect, this, nID );
		}
		m_windows.AddTail( pwnd );
	}

	return S_OK;
}

int CActiveXSite::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;


	//SetWindowLong( GetSafeHwnd(), GWL_STYLE, GetStyle() | WS_CLIPCHILDREN );


	//Active Scripting debug support
	//InitDebug();


	// Странная ошибка: мастер вылетает при запуске в отладочной версии, но без отладчика.
	// вылет происходит внутри CActiveXSite::_CreateScriptEngine (при вызове отсюда),
	// в строке 292 (на сегодня) при вызове m_ptrActiveScript->AddNamedItem( bstr, dwAddFlag ),
	// bstr = "ActionManager". При этом dwAddFlag должен содержать флаг SCRIPTITEM_GLOBALMEMBERS
	// Если закомментарить строки 284-285 (на сегодня), где инициализируется этот флаг, вылет
	// пропадает. С этим try-catch вылет тоже пропадает.
	 
	try{

	//moved from constructor
	IUnknown	*punk = _GetOtherComponent( GetAppUnknown(), IID_IOperationManager );

	ASSERT( punk );
	m_ptrOperationManager = punk;
	punk->Release();

	m_ptrOperationManager->RegisterPermanent( GetControllingUnknown() );

	_LoadControls();
	_CreateScriptEngine();

	_ExecuteFormScript();

	POSITION	pos = m_windows.GetHeadPosition();
	while( pos )
	{
		CAxWnd	*pwnd = m_windows.GetNext( pos );
		IUnknown	*punk = pwnd->GetControlUnknown();
		if( CheckInterface( punk, IID_IVtActiveXCtrl ) )
		{
			IVtActiveXCtrlPtr	sptrVTC( punk );
			sptrVTC->SetSite( GetAppUnknown(), GetControllingUnknown() );
		}
	}

	m_nIDEvent = 779;
	m_nIDTimer = SetTimer( m_nIDEvent, 100, 0 );

	//19.02.2003 Cos if Interactive action run in Form_OnInitialize
	ShowWindow( SW_SHOW );

	//xx.01.2003 If change appearance of on Form_OnInitialize, avoid flicking
	SendMessage( WM_INITIALIZE_FORM );

	//24.06.2003 
	PostMessage( WM_FIRE_ON_CREATE_FORM );
	}
	catch(...)
	{
	}

	return 0;
}

CSize	CActiveXSite::GetSize()
{
	IActiveXFormPtr	ptrF( m_ptrForm );
	CSize	size;

	ptrF->GetSize( &size );
	return size;
}

CString CActiveXSite::GetCaption()
{
	IActiveXFormPtr	ptrF( m_ptrForm );
	BSTR	bstr;

	ptrF->GetTitle( &bstr );
	m_strTitle = bstr;
	::SysFreeString( bstr );

	return m_strTitle;
}

BOOL CActiveXSite::PreCreateWindow(CREATESTRUCT& cs) 
{
	DWORD dwStyle = 0;
	
	switch(m_nEdgeType)// 0 - вогнутая; 1 - выпуклая; 2 - никакая
	{
	case 0:
		dwStyle = WS_EX_STATICEDGE;
		break;
	case 1:
		dwStyle = WS_EX_DLGMODALFRAME;
		break;
	}

//	cs.dwExStyle = WS_EX_TRANSPARENT|WS_EX_CONTROLPARENT|dwStyle|WS_EX_CONTEXTHELP;

	cs.style |= DS_CONTROL;
	
	cs.style |= WS_CLIPCHILDREN;
	cs.style |= WS_CLIPSIBLINGS;
	
	return CWnd::PreCreateWindow(cs);
}

BOOL CActiveXSite::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	if( GetSafeHwnd() )
	{
		CRect rc;
		GetClientRect( &rc );
		pDC->FillRect( &rc, &(CBrush( ::GetSysColor(COLOR_BTNFACE) ) ) );
		return TRUE;		
	}
	
	
	return CWnd::OnEraseBkgnd(pDC);
	//return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// interface implementation 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CActiveXSite::XSite::GetLCID(LCID *plcid)
{
	return E_NOTIMPL;     // Use system settings
}

STDMETHODIMP CActiveXSite::XSite::GetItemInfo
(
  LPCOLESTR   pstrName,
  DWORD       dwReturnMask,
  IUnknown**  ppunkItemOut,
  ITypeInfo** pptinfoOut
)
{
	//HRESULT hr;
	METHOD_PROLOGUE(CActiveXSite, Site)

	if( pptinfoOut )
		*pptinfoOut = 0;
	if( ppunkItemOut )
		*ppunkItemOut = 0;

	CString	strItemName = pstrName;

	//TRACE( _T("GetItemInfo called (%s)\n"), (const char *)strItemName );

	if (dwReturnMask & SCRIPTINFO_ITYPEINFO)
	{
		if (!pptinfoOut)
			return E_INVALIDARG;
		*pptinfoOut = NULL;

		IUnknownPtr sptrUnk( pThis->_GetScriptItemUnk( strItemName ), false );

		if( CheckInterface( sptrUnk, IID_IProvideClassInfo ) )
		{
			IProvideClassInfoPtr sptrPCI( sptrUnk );
			sptrPCI->GetClassInfo( pptinfoOut );
		}
	}

	if (dwReturnMask & SCRIPTINFO_IUNKNOWN)
	{
		if (!ppunkItemOut)
			return E_INVALIDARG;

		*ppunkItemOut = pThis->_GetScriptItemUnk( strItemName );
    }

	return S_OK;
}


STDMETHODIMP CActiveXSite::XSite::GetDocVersionString(BSTR *pbstrVersion)
{
	return E_NOTIMPL;   // UNDONE: Implement this method
}

STDMETHODIMP CActiveXSite::XSite::RequestItems(void)
{
	return E_NOTIMPL;
}

STDMETHODIMP CActiveXSite::XSite::RequestTypeLibs(void)
{
	METHOD_PROLOGUE(CActiveXSite,Site);
	return E_NOTIMPL;
}

STDMETHODIMP CActiveXSite::XSite::OnScriptTerminate(const VARIANT   *pvarResult,const EXCEPINFO *pexcepinfo)
{
	// UNDONE: Put up error dlg here
	return S_OK;
}


//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
STDMETHODIMP CActiveXSite::XSite::OnStateChange(SCRIPTSTATE ssScriptState)
{
	// Don't care about notification
	return S_OK;
}


//---------------------------------------------------------------------------
// Display the error
//---------------------------------------------------------------------------
STDMETHODIMP CActiveXSite::XSite::OnScriptError(IActiveScriptError *pse)
{
	METHOD_PROLOGUE(CActiveXSite,Site);
  
	CString   strError;
	CString	  strArrow;
	CString	  strDesc;
	CString	  strLine;
	
	EXCEPINFO ei;
	DWORD     dwSrcContext;
	ULONG     ulLine;
	LONG      ichError;
	BSTR      bstrLine = NULL;
	
	HRESULT   hr;

	pse->GetExceptionInfo(&ei);
	pse->GetSourcePosition(&dwSrcContext, &ulLine, &ichError);
	hr = pse->GetSourceLineText(&bstrLine);
	if (hr)
		hr = S_OK;  // Ignore this error, there may not be source available
  
	if (!hr)
    {
		strError=ei.bstrSource;
		
		strDesc=ei.bstrDescription;
		strLine=bstrLine;

		if (ichError > 0 && ichError < 255)
		{
			strArrow=CString(_T('-'),ichError);
			strArrow.SetAt(ichError-1,_T('v'));
			
		}

		CString strErrorCopy=strError;
		strError.Format(_T("Source:'%s'\nFile:'%s'  Line:%d  Char:%ld\nError:%d  '%s'\n%s\n%s"),
						LPCTSTR(strErrorCopy),
						_T("File"),
						ulLine,
						ichError,
						(int)ei.wCode,
						LPCTSTR(strDesc),
						LPCTSTR(strArrow),
						LPCTSTR(strLine));
		
		AfxMessageBox(strError, MB_ICONERROR );
    }

	if (bstrLine)
		SysFreeString(bstrLine);

	return hr;
}
STDMETHODIMP CActiveXSite::XSite::OnEnterScript(void)
{
  // No need to do anything
  return S_OK;
}

STDMETHODIMP CActiveXSite::XSite::OnLeaveScript(void)
{
  // No need to do anything
  return S_OK;
}

//***************************************************************************
// IActiveSiteWnd Interface
//***************************************************************************

//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
STDMETHODIMP CActiveXSite::XSiteWnd::GetWindow(HWND *phwndOut)
{
  if (!phwndOut)
    return E_INVALIDARG;

  METHOD_PROLOGUE(CActiveXSite,SiteWnd);

  *phwndOut = AfxGetMainWnd()->GetSafeHwnd();
  return S_OK;
}


//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
STDMETHODIMP CActiveXSite::XSiteWnd::EnableModeless(BOOL fEnable)
{
	AfxGetApp()->EnableModeless( fEnable );
  return S_OK;
}
// #####  END  ACTIVEX SCRIPTING SUPPORT #####
int	CActiveXSite::_GetAppComponentCount()
{
	ICompManagerPtr	ptrM( GetAppUnknown() );
	int	lCount = 0;
	ptrM->GetCount( &lCount );

	return lCount;
}


HRESULT CActiveXSite::XHelp::GetHelpInfo( BSTR *pbstrHelpFile, BSTR *pbstrHelpTopic, DWORD *pdwFlags )
{
	METHOD_PROLOGUE_EX(CActiveXSite, Help)
	CString	strPageName = GetObjectName( pThis->m_ptrForm );
	CString	strHtmlFileName = "ax";
	strHtmlFileName+=strPageName;

	if( pbstrHelpFile )*pbstrHelpFile = strHtmlFileName.AllocSysString();
	if( pbstrHelpTopic )*pbstrHelpTopic = strPageName.AllocSysString();

	return S_OK;
}

HRESULT CActiveXSite::XHelp::GetHelpInfo2( BSTR *pbstrHelpFile, BSTR *pbstrHelpTopic, DWORD *pdwFlags, BSTR *pbstrHelpPrefix )
{
    METHOD_PROLOGUE_EX(CActiveXSite, Help)
	CString	strPageName = GetObjectName( pThis->m_ptrForm );
	CString	strHtmlFileName = strPageName;
	CString strPrefix( _T("ax") );
	
	if( pbstrHelpFile )*pbstrHelpFile = strHtmlFileName.AllocSysString();
	if( pbstrHelpTopic )*pbstrHelpTopic = strPageName.AllocSysString();
	if( pbstrHelpPrefix )*pbstrHelpPrefix = strPrefix.AllocSysString();

	return S_OK;
}

//////////////////////////////////axsite
HRESULT CActiveXSite::XAxSite::ExecuteAction( BSTR bstrActionName, BSTR bstrActionParam, DWORD dwFlags )
{
	_try_nested( CActiveXSite, AxSite, ExecuteAction )
	{
		CString	strAction = bstrActionName;
		::ExecuteAction( strAction );

		//pThis->ExecuteAction( strAction/*, strActionParams, dwFlags*/ );

		return S_OK;
	}
	_catch_nested;
};

IUnknown* CActiveXSite::_GetAppComponent( int nPos )
{
	ICompManagerPtr	ptrM( GetAppUnknown() );
	IUnknown	*punk = 0;
	ptrM->GetComponentUnknownByIdx( nPos, &punk );

	return punk;
}


int	CActiveXSite::_GetControlComponentsCount()
{
	return m_windows.GetCount();
}

IUnknown* CActiveXSite::_GetControlComponent( int nPos )
{
	POSITION	pos = m_windows.GetHeadPosition();

	while( pos )
	{
		CAxWnd	*pwnd = m_windows.GetNext( pos );
		if( !nPos )
		{
			IUnknown	*punk = pwnd->GetControlUnknown();

			ASSERT( pwnd );

			punk->AddRef();
			return punk;
		}
		nPos--;
	}
	return 0;
}

IUnknown* CActiveXSite::_GetScriptItemUnk( const char *szItemName )
{
	//check is item a "Form" 
	{
		CString strName = ::GetObjectName( GetControllingUnknown() );
		if( strName == szItemName )
		{
			IUnknown	*punk = GetControllingUnknown();
			punk->AddRef();

			return punk;
		}
	}

	//check is item a "ActionHelper" 
	{
		CString strName = ::GetObjectName( m_helper.GetControllingUnknown() );
		if( strName == szItemName )
		{
			IUnknown	*punk = m_helper.GetControllingUnknown();
			punk->AddRef();

			return punk;
		}
	}

	int	nCount = _GetAppComponentCount();

	for( int i = 0; i < nCount; i++ )
	{
		IUnknown	*punkObject = _GetAppComponent( i );
		CString	strName = ::GetObjectName( punkObject );

		if( strName == szItemName )
			return	punkObject;
		punkObject->Release();
	}

	POSITION	pos = m_windows.GetHeadPosition();

	while( pos )
	{
		CAxWnd	*pwnd = m_windows.GetNext( pos );
		IUnknown	*punkCtrl = pwnd->GetControlUnknown();
		IUnknown	*punkData = pwnd->GetControlObjectUnknown();

		IActiveXCtrlPtr	ptrCtrl( punkData );

		BSTR	bstr;
		ptrCtrl->GetName( &bstr );
		CString	strName = bstr;
		::SysFreeString( bstr );

		if( strName == szItemName )
		{
			if( !punkCtrl )return 0;
			punkCtrl->AddRef();
			return punkCtrl;
		}
	}

	return 0;
}

IDebugHostPtr CActiveXSite::get_debugger()
{
	IDebugHostPtr ptr_debug( GetControllingUnknown() );
	return ptr_debug;

	/*
	sptrIApplication	ptr_app( ::GetAppUnknown() );
	if( ptr_app == 0 )	return 0;

	IUnknown* punk_se = 0;
	ptr_app->GetActiveScript( &punk_se );
	if( !punk_se )		return 0;

	IDebugHostPtr ptr_debug( punk_se );
	punk_se->Release();	punk_se = 0;
	*/

	return ptr_debug;
}

void CActiveXSite::OnDestroy() 
{
	FireOnDestroy();

	IDebugHostPtr ptr_debug = get_debugger();
	if( ptr_debug != 0 && m_dw_script_context != -1 )
		ptr_debug->CloseScript( m_dw_script_context );

	m_helper2.DeInit();

	m_helper.DeInit();
	if( m_ptrOperationManager != 0 )
		m_ptrOperationManager->UnRegisterPermanent( GetControllingUnknown() );


	CWnd::OnDestroy();
}


void CActiveXSite::DoLeavePreviewMode()
{
	CPreviewSiteImpl::DoLeavePreviewMode();

	if( !m_bTerminated )
		m_helper.FinalizeCurrentAction( false );
	m_bTerminated = false;
}

void CActiveXSite::DoTerminatePreview()
{
	CPreviewSiteImpl::DoTerminatePreview();
	m_helper.FinalizeCurrentAction( true );
	m_bTerminated = true;
}


BOOL CActiveXSite::PreTranslateMessage(MSG* pMsg) 
{
	POSITION	pos = m_windows.GetHeadPosition();
	while( pos )
	{
		IOleControlPtr	ptrC( m_windows.GetNext( pos )->GetControlUnknown() );
		if( ptrC == 0 )
			continue;
		ptrC->OnMnemonic( pMsg );
	}

	return false;
}	

void CActiveXSite::OnClose() 
{
	KillTimer( 	m_nIDEvent );	
	CWnd::OnClose();
}

static bool bInsideOnTimer = false;

class _CDisableButtons
{
public:
	_CDisableButtons()
	{
		CFormManager::m_pFormManager->SetDisableButtonsMode(true);
	};
	~_CDisableButtons()
	{
		CFormManager::m_pFormManager->SetDisableButtonsMode(false);
	}
	void f()
	{
	}
};

void CActiveXSite::OnTimer(UINT_PTR nIDEvent) 
{
	if( m_nIDEvent == nIDEvent )
	{
		if(bInsideOnTimer)
		{
			//TRACE( "!!!!!! InsideOnTimer recursive call !!!!!!\n");
		}
		else
		{
			{
				_CDisableButtons db;
				CLockerFlag locker(&bInsideOnTimer);
				//bool bOldInsideOnTimer = bInsideOnTimer;
				//bInsideOnTimer = true;

				m_helper.ExecuteCurrentAction();

				// поддержка Preview2
				m_helper2.UpdateStates();
			}

			if(m_bCancelSheduled) // во время таймера мог быть нажат Cancel - отработать его постфактум (но уже отключив DisableButtons)
				CFormManager::m_pFormManager->PressCancel();

			//bInsideOnTimer = bOldInsideOnTimer;
		}

		return;
	}
	CWnd::OnTimer(nIDEvent);
}


void CActiveXSite::ApplyPreview() 
{
	m_helper.ApplyPreview();
}

void CActiveXSite::BeginFilterGroup()
{
	m_helper.BeginFilterGroup();
}

void CActiveXSite::EndFilterGroup()
{
	m_helper.EndFilterGroup();
}


void CActiveXSite::CancelPreview() 
{
	m_helper.RemovePreview();
}

void CActiveXSite::SetSize(CSize size)
{
	IActiveXFormPtr	ptrF( m_ptrForm );
	ptrF->SetSize( size );
}
void CActiveXSite::AddHeadActionArgument(LPDISPATCH pdispObject) 
{
	// TODO: Add your dispatch handler code here
	IUnknownPtr punk(pdispObject);
	m_helper.AddHeadArg(punk);
}


void CActiveXSite::EnableButtons(long dwState)
{
	HWND hwnd = ::GetParent(m_hWnd);
	if( !::GetDlgItem(hwnd, IDOK) )
		hwnd = ::GetParent(hwnd);

	::EnableWindow(::GetDlgItem(hwnd, IDOK), dwState&1?TRUE:FALSE);
	::EnableWindow(::GetDlgItem(hwnd, IDCANCEL), dwState&2?TRUE:FALSE);
	::EnableWindow(::GetDlgItem(hwnd, IDAPPLY), dwState&4?TRUE:FALSE);
	::EnableWindow(::GetDlgItem(hwnd, IDHELP), dwState&8?TRUE:FALSE);

	// [vanek] : update close button's state - 22.10.2004
	{
		HMENU hsysmenu = 0;
		hsysmenu = ::GetSystemMenu( hwnd, FALSE);
		if( hsysmenu )
		{
			DWORD dwflags = MF_BYCOMMAND;
			if( !(dwState & 16) )
				dwflags |= MF_GRAYED;

			::EnableMenuItem( hsysmenu, SC_CLOSE, dwflags);             
		}
		else
		{
			HWND hwnd_parent = hwnd;
			while( hwnd_parent && !::SendMessage( hwnd_parent, WM_SETCANCLOSEWND, (WPARAM)(dwState & 16), 0 ) )
				hwnd_parent = ::GetParent( hwnd_parent );
		}

	}
}

void CActiveXSite::ShowButtons(long dwState)
{
	HWND hwnd = ::GetParent(m_hWnd);
	if( !::GetDlgItem(hwnd, IDOK) )
		hwnd = ::GetParent(hwnd);

	::ShowWindow(::GetDlgItem(hwnd, IDOK), dwState&1?SW_SHOW:SW_HIDE);
	::ShowWindow(::GetDlgItem(hwnd, IDCANCEL), dwState&2?SW_SHOW:SW_HIDE);
	::ShowWindow(::GetDlgItem(hwnd, IDAPPLY), dwState&4?SW_SHOW:SW_HIDE);
	::ShowWindow(::GetDlgItem(hwnd, IDHELP), dwState&8?SW_SHOW:SW_HIDE);
}


void CActiveXSite::InitializePreview() 
{
	m_helper.Init( this );
}

void CActiveXSite::UpdateSystemSettings() 
{
	IApplicationPtr	ptrA( GetAppUnknown() );

	::FireEvent( ptrA, szEventNewSettings );

	long	lposTempl = 0;

	ptrA->GetFirstDocTemplPosition( &lposTempl );

	while( lposTempl )
	{
		long	lposDoc = 0;
		ptrA->GetFirstDocPosition( lposTempl, &lposDoc );


		while( lposDoc )
		{
			IUnknown	*punk = 0;
			ptrA->GetNextDoc( lposTempl, &lposDoc, &punk );

			::FireEvent( punk, szEventNewSettings );

			ASSERT( punk );
			punk->Release();
		}
		ptrA->GetNextDocTempl( &lposTempl, 0, 0 );
	}
	
}

void CActiveXSite::TerminateCurrentAction() 
{
	m_helper.TerminateCurrentAction();
}

void CActiveXSite::OnTitleChanged() 
{
	IActiveXFormPtr	ptrF( m_ptrForm );
	_bstr_t	bstr = m_strTitle;
	ptrF->SetTitle( bstr );

	if( GetParent()->GetSafeHwnd() )
	{
		GetParent()->PostMessage( WM_CAPTIONCHANGE );
	}
	HWND	hwnd;
	IApplication	*pApp = GetAppUnknown();
	pApp->GetMainWindowHandle( &hwnd );
	::SendMessage( hwnd, WM_CAPTIONCHANGE, 0, (LPARAM)(const char*)m_strTitle );
}

BOOL CActiveXSite::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	return OnHelpHitTest( 0, 0 ) != -2;
}

LRESULT CActiveXSite::OnHelpHitTest(WPARAM wParam, LPARAM lParam)
{
	m_bHelpDisplayed = true;
	//TRACE( "CWnd::OnHelpInfo\n" );

	POSITION	pos = m_windows.GetHeadPosition();
	CAxWnd	*pwnd = 0;

	CPoint	point;
	GetCursorPos( &point );

	while( pos )
	{
		pwnd = m_windows.GetNext( pos );
		CRect	rect;
		DWORD	dwStyleEx = pwnd->GetExStyle();
		DWORD	dwID = ::GetWindowContextHelpId( pwnd->GetSafeHwnd() );

		if( dwID < 1000 || !pwnd->IsWindowVisible() )
		{
			pwnd = 0;
			continue;
		}

		pwnd->GetWindowRect( rect ) ;

		if( rect.PtInRect( point ) )
			break;
		pwnd = 0;
	}


	CString	strHtmlFileName;
	CString	strPageName = GetObjectName( m_ptrForm );

	strHtmlFileName = strPageName;

	/*if( pwnd )
	{
		::HelpLoadCollection( 0 );
		::HelpDisplayPopup( strHtmlFileName, "ax", strPageName, pwnd->GetSafeHwnd() );
	}
	else
	{ */
		::HelpDisplayTopic( strHtmlFileName, "ax", strPageName, "$GLOBAL_main" );
	//}

	return (DWORD)-2;
	
}


void CActiveXSite::OnSysCommand(UINT nID, LPARAM lParam) 
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd::OnSysCommand(nID, lParam);
}

LRESULT CActiveXSite::OnFireInitialize( WPARAM, LPARAM )
{
	FireOnInitialize();

	POSITION	pos = m_windows.GetHeadPosition();
	while( pos )
	{
		CAxWnd	*pwnd = m_windows.GetNext( pos );
		if( pwnd && ::IsWindow( pwnd->GetSafeHwnd() ) )
			pwnd->Invalidate();
	}

	return 0;
}

LRESULT CActiveXSite::OnFireCreateForm( WPARAM, LPARAM )
{
	FireOnCreate();
	return 0;
}


void CActiveXSite::ContiueModal() 
{
	m_bContinueModal = true;
}

void CActiveXSite::debug_break() 
{
	IDebugHostPtr ptr_debug = get_debugger();
	if( ptr_debug != 0 )
	{
		ptr_debug->ShowDebugger( m_dw_script_context );
		ptr_debug->BreakIntoDebuger( );
	}
}

LPDISPATCH CActiveXSite::GetFormScript()
{
	if( m_ptrActiveScript == 0 )
		return 0;

	IDispatch* pdisp = 0;
	m_ptrActiveScript->GetScriptDispatch( 0, &pdisp );	
	
	return pdisp;
}

LRESULT CActiveXSite::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message >= WM_MOUSEFIRST && message <= WM_MOUSELAST )
	{
		HWND hwndParent = GetNotChildParent(m_hWnd);
		return hwndParent?::SendMessage(hwndParent,message,wParam,lParam):0;
	}
	return __super::WindowProc(message, wParam, lParam);
}


void CActiveXSite::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);

	if( !m_bLockFireEvent )
	{
		if( bShow )
			FireOnShow();
		else
			FireOnHide();
	}
	// TODO: Add your message handler code here
}

afx_msg void CActiveXSite::Preview2_Init()
{
	IApplicationPtr	sptrApp( GetAppUnknown() );

	IUnknownPtr	ptrDocument = 0;
	sptrApp->GetActiveDocument( &ptrDocument );

	m_helper2.Init(this, ptrDocument);
}

afx_msg void CActiveXSite::Preview2_DeInit()
{
	m_helper2.DeInit();
}

afx_msg void CActiveXSite::Preview2_SetStagesCount(long nCount)
{
	m_helper2.Init(this, m_helper2.GetDocument(), nCount+1);
	// в m_helper2 должно быть на 1 State больше - для начального состояния
}

afx_msg long CActiveXSite::Preview2_GetStagesCount()
{
	return m_helper2.GetStatesCount() - 1;
	// в m_helper2 на 1 State больше - для начального состояния
}

afx_msg void CActiveXSite::Preview2_LoadState(long index)
{
	m_helper2.LoadState(index, false);
}

afx_msg void CActiveXSite::Preview2_SaveState(long index)
{
	m_helper2.SaveState(index);
}

afx_msg void CActiveXSite::Preview2_Invalidate(long index)
{
	m_helper2.Invalidate(index);
}

afx_msg void CActiveXSite::Preview2_Cancel()
{
	m_helper2.Cancel();
}
afx_msg void CActiveXSite::Preview2_Apply()
{
	m_helper2.Ok();
}
