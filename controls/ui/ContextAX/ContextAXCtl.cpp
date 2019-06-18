// ContextAXCtl.cpp : Implementation of the CContextAXCtrl ActiveX Control class.

#include "stdafx.h"
#include "ContextAX.h"
#include "ContextAXCtl.h"
#include "ContextAXPpg.h"

#include "NameConsts.h"
#include "TimeTest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_TYPE_TREE	0x1ff1
#define ID_OBJ_TREE		0x1ff2

#define IDC_BUTTON_TYPE			0x1ff3
#define IDC_BUTTON_OBJ			0x1ff4
#define IDC_BUTTON_NUMBER_OFF	0x1ff5
#define IDC_BUTTON_NUMBER_ON	0x1ff6
#define IDC_BUTTON_EXPAND		0x1ff7
#define IDC_BUTTON_COLLAPSE		0x1ff8
#define IDC_BUTTON_ABC			0x1ff9
#define IDC_BUTTON_TIME			0x1ffa

bool SaveBMPFile(UINT idRes, LPCTSTR szFile);
CString GetTempFileName();

IMPLEMENT_DYNCREATE(CContextAXCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CContextAXCtrl, COleControl)
	//{{AFX_MSG_MAP(CContextAXCtrl)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_EDIT, OnEdit)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CContextAXCtrl, COleControl)
    //{{AFX_EVENTSINK_MAP(CContextAXCtrl)
	ON_EVENT(CContextAXCtrl, IDC_BUTTON_TYPE,		-600 /* Click */, OnClickButtonType,		VTS_NONE)
	ON_EVENT(CContextAXCtrl, IDC_BUTTON_OBJ,		-600 /* Click */, OnClickButtonObj,			VTS_NONE)
	ON_EVENT(CContextAXCtrl, IDC_BUTTON_NUMBER_OFF, -600 /* Click */, OnClickButtonNumberOn,	VTS_NONE)
	ON_EVENT(CContextAXCtrl, IDC_BUTTON_NUMBER_ON,	-600 /* Click */, OnClickButtonNumberOff,	VTS_NONE)
	ON_EVENT(CContextAXCtrl, IDC_BUTTON_EXPAND,		-600 /* Click */, OnClickButtonExpand,		VTS_NONE)
	ON_EVENT(CContextAXCtrl, IDC_BUTTON_COLLAPSE,	-600 /* Click */, OnClickButtonCollapse,	VTS_NONE)
	ON_EVENT(CContextAXCtrl, IDC_BUTTON_ABC,		-600 /* Click */, OnClickButtonAbc,			VTS_NONE)
	ON_EVENT(CContextAXCtrl, IDC_BUTTON_TIME,		-600 /* Click */, OnClickButtonTime,		VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CContextAXCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CContextAXCtrl)
	DISP_PROPERTY_NOTIFY(CContextAXCtrl, "EnableTitleTips", m_bEnableTitleTips, OnEnableTitleTipsChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CContextAXCtrl, "Editable", m_bEditable, OnEditableChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CContextAXCtrl, "Border", m_nBorder, OnBorderChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CContextAXCtrl, "IconSize", m_nIconSize, OnIconSizeChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CContextAXCtrl, "ImageOffset", m_nImageOffset, OnImageOffsetChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CContextAXCtrl, "Indent", m_nIndent, OnIndentChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CContextAXCtrl, "Margin", m_nMargin, OnMarginChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CContextAXCtrl, "NumberOffset", m_nNumberOffset, OnNumberOffsetChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CContextAXCtrl, "ShowButtons", m_bShowButtons, OnShowButtonsChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CContextAXCtrl, "Tracking", m_bTracking, OnTrackingChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CContextAXCtrl, "ViewType", m_bViewType, OnViewTypeChanged, VT_BOOL)
	DISP_PROPERTY_EX(CContextAXCtrl, "View", GetView, SetView, VT_DISPATCH)
	DISP_PROPERTY_EX(CContextAXCtrl, "ReadOnly", GetReadOnly, SetReadOnly, VT_BOOL)
	DISP_PROPERTY_EX(CContextAXCtrl, "SelectedObject", GetSelectedObject, SetSelectedObject, VT_DISPATCH)
	DISP_PROPERTY_EX(CContextAXCtrl, "TitleTipsColor", GetTitleTipsColor, SetTitleTipsColor, VT_COLOR)
	DISP_PROPERTY_EX(CContextAXCtrl, "TitleTipsBackColor", GetTitleTipsBackColor, SetTitleTipsBackColor, VT_COLOR)
	DISP_FUNCTION(CContextAXCtrl, "Register", Register, VT_EMPTY, VTS_BOOL)
	DISP_FUNCTION(CContextAXCtrl, "GetButtonType", GetButtonType, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CContextAXCtrl, "GetButtonObj", GetButtonObj, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CContextAXCtrl, "GetButtonAbc", GetButtonAbc, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CContextAXCtrl, "GetButtonTime", GetButtonTime, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CContextAXCtrl, "GetButtonNumOn", GetButtonNumOn, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CContextAXCtrl, "GetButtonNumOff", GetButtonNumOff, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CContextAXCtrl, "GetButtonExpand", GetButtonExpand, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CContextAXCtrl, "GetButtonCollapse", GetButtonCollapse, VT_DISPATCH, VTS_NONE)
	DISP_STOCKPROP_ENABLED()
	DISP_STOCKPROP_BACKCOLOR()
	DISP_STOCKPROP_FONT()
	DISP_STOCKPROP_FORECOLOR()
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CContextAXCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CContextAXCtrl, COleControl)
	//{{AFX_EVENT_MAP(CContextAXCtrl)
	EVENT_CUSTOM("BeforeViewChange", FireBeforeViewChange, VTS_DISPATCH)
	EVENT_CUSTOM("AfterViewChange", FireAfterViewChange, VTS_DISPATCH)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CContextAXCtrl, 1)
	PROPPAGEID(CContextAXPropPage::guid)
END_PROPPAGEIDS(CContextAXCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid
GUARD_IMPLEMENT_OLECREATE_CTRL(CContextAXCtrl, "CONTEXTAX.ContextAXCtrl.1",
	0x753b82d9, 0xb5e3, 0x4429, 0xaf, 0xf8, 0xb, 0x4a, 0xcd, 0xe, 0xb5, 0x1)

/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CContextAXCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs
const IID BASED_CODE IID_DContextAX =
		{ 0x18c5f51c, 0x98b6, 0x4656, { 0xa4, 0xd8, 0x4b, 0xa6, 0x80, 0xbe, 0xb8, 0xd5 } };
const IID BASED_CODE IID_DContextAXEvents =
		{ 0xa5f6a790, 0x8463, 0x4334, { 0xb3, 0xf, 0xaa, 0xbf, 0x41, 0x85, 0xeb, 0xfd } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwContextAXOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;
	
IMPLEMENT_OLECTLTYPE(CContextAXCtrl, IDS_CONTEXTAX, _dwContextAXOleMisc)


BEGIN_INTERFACE_MAP(CContextAXCtrl, COleControl)
	INTERFACE_PART(CContextAXCtrl, IID_IVtActiveXCtrl,	ActiveXCtrl)
	INTERFACE_PART(CContextAXCtrl, IID_IEventListener,	EvList)
	INTERFACE_PART(CContextAXCtrl, IID_IMenuInitializer,Viewer)
	INTERFACE_PART(CContextAXCtrl, IID_IContextView,	ContextView	)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CContextAXCtrl, ActiveXCtrl);
IMPLEMENT_UNKNOWN(CContextAXCtrl, Viewer);
IMPLEMENT_UNKNOWN(CContextAXCtrl, ContextView);


/////////////////////////////////////////////////////////////////////////////
// CContextAXCtrl IVtActiveXCtrl implement

HRESULT CContextAXCtrl::XActiveXCtrl::SetSite(IUnknown * punkVtApp, IUnknown * punkSite)
{
	METHOD_PROLOGUE_EX(CContextAXCtrl, ActiveXCtrl)

//	pThis->m_sptrSite = punkSite;
//	pThis->m_sptrApp = punkVtApp;	

	return S_OK;
}
  
HRESULT CContextAXCtrl::XActiveXCtrl::GetName(BSTR * pbstrName)
{
	METHOD_PROLOGUE_EX(CContextAXCtrl, ActiveXCtrl)	
	*pbstrName = NULL;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CContextAXCtrl IVtActiveXCtrl implement
//
HRESULT CContextAXCtrl::XViewer::OnInitPopup(BSTR bstrMenuName, HMENU hMenu, HMENU *phOutMenu)
{
	_try_nested(CContextAXCtrl, Viewer, OnInitPopup)
	{
		if (!hMenu)
			return E_INVALIDARG;

		*phOutMenu = ::CopyMenu(hMenu);
		if (!*phOutMenu)
			return E_INVALIDARG;
		// get menu
		CMenu* pmenu = CMenu::FromHandle(*phOutMenu);
		if (!pmenu)
			return E_FAIL;

		// get command Manager
		IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager2 );

		ASSERT(punk != NULL);
		if (!punk)
			return E_FAIL;

		sptrICommandManager2 sptrCmdMgr(punk);
		punk->Release();
		

		CContextCtrl * pTree = pThis->m_bViewType ? pThis->m_ViewType.GetTree() : 
													pThis->m_ViewObj.GetTree();
		ASSERT (pTree);
		HCONTEXTITEM hItem = pTree->GetItemFocus();
		// get selected object data
		CContextItemData* pSelObjectData = (hItem) ? (CContextItemData*)pTree->GetItemData(hItem) : NULL;

		CString strObjectName;
		CString strObjectType;

		if (!pSelObjectData)
		{
			strObjectName = _T("...");
			strObjectType = _T("...");
		}
		else if (pSelObjectData->GetPointer(false))
		{
			strObjectName = pSelObjectData->GetName(); 
			strObjectType = ::GetObjectKind(pSelObjectData->GetPointer(false)); 
		} 
		else
		{
			strObjectName = _T("..."); 
			strObjectType = pSelObjectData->GetName(); 
		}

		CString strMenuName;
		BSTR bstrName = 0;

		int nCount = (int)pmenu->GetMenuItemCount();
		for (int i = 0; i < nCount; i++)
		{
			UINT uFlag = MF_BYPOSITION | MF_STRING;
			UINT uItem = pmenu->GetMenuItemID(i);
			bool bChange = true;

			if (!SUCCEEDED(sptrCmdMgr->GetActionName(&bstrName, uItem)))
				continue;
			
			CString strActionName = bstrName;
			::SysFreeString(bstrName);

			AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
/*
			// return action name by Resource ID
			if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_ACTION_NEWOBJECT)))
				strMenuName = "Create new " + strObjectType;

			else if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_ACTION_LOADOBJECT)))
				strMenuName = "Load new " + strObjectType;

			else if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_ACTION_SELECTOBJECT)))
				strMenuName = "Select " + strObjectName;

			else if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_ACTION_DELETEOBJECT)))
				strMenuName = "Delete " + strObjectName;

			else if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_ACTION_RENAMEOBJECT)))
				strMenuName = "Rename " + strObjectName;

			else if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_ACTION_OBJECTPROPS)))
				strMenuName = "Properties of " + strObjectName;

			else if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_ACTION_SAVEOBJECT)))
				strMenuName = "Save " + strObjectName;

			else
				bChange = false;

*/
			if (bChange)
				pmenu->ModifyMenu(i, uFlag, uItem, strMenuName);

		}

		*phOutMenu = pmenu->Detach();

		return S_OK;
	}
	_catch_nested;
}


HRESULT CContextAXCtrl::XContextView::GetView(IUnknown ** ppunkView)
{
	METHOD_PROLOGUE_EX(CContextAXCtrl, ContextView)
	{
		if (!ppunkView)
			return E_INVALIDARG;

		*ppunkView = pThis->m_sptrView;
		if (*ppunkView)
			(*ppunkView)->AddRef();
		return S_OK;
	}
}

HRESULT CContextAXCtrl::XContextView::SetView(IUnknown * punkView)
{
	METHOD_PROLOGUE_EX(CContextAXCtrl, ContextView)
	{
		return E_NOTIMPL;
	}
}

HRESULT CContextAXCtrl::XContextView::SetReadOnlyMode(BOOL bMode)
{
	METHOD_PROLOGUE_EX(CContextAXCtrl, ContextView)
	{
		if (pThis->GetReadOnly() != bMode)
			pThis->SetReadOnly(bMode);
		return S_OK;
	}
}

HRESULT CContextAXCtrl::XContextView::GetReadOnlyMode(BOOL * pbMode)
{
	METHOD_PROLOGUE_EX(CContextAXCtrl, ContextView)
	{
		if (!pbMode)
			return E_INVALIDARG;

		*pbMode = pThis->GetReadOnly();
		return S_OK;
	}
}



/////////////////////////////////////////////////////////////////////////////
// CContextAXCtrl::CContextAXCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CContextAXCtrl

BOOL CContextAXCtrl::CContextAXCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegInsertable | afxRegApartmentThreading to afxRegInsertable.
	return UpdateRegistryCtrl(bRegister, AfxGetInstanceHandle(), IDS_CONTEXTAX, IDB_CONTEXTAX,
							  afxRegInsertable | afxRegApartmentThreading, _dwContextAXOleMisc,
							  _tlid, _wVerMajor, _wVerMinor);
}


/////////////////////////////////////////////////////////////////////////////
// CContextAXCtrl::CContextAXCtrl - Constructor

CContextAXCtrl::CContextAXCtrl()
{
	InitializeIIDs(&IID_DContextAX, &IID_DContextAXEvents);

	EnableAutomation();
	_OleLockApp( this );

	CContextCtrl * pTree = m_ViewType.GetTree();
	m_bEnableTitleTips = pTree ? pTree->GetTitleTips() : true;
	m_bEditable		= pTree ? pTree->IsEditable() : true;
	m_nBorder		= pTree ? pTree->GetBorder() : 1;
	m_nIconSize		= pTree ? pTree->GetIconSize() : 32;
	m_nImageOffset	= pTree ? pTree->GetImageOffset() : m_nMargin;
	m_nIndent		= pTree ? pTree->GetIndent() : 20;
	m_nMargin		= pTree ? pTree->GetMargin() : 3;
	m_nNumberOffset = pTree ? pTree->GetNumberOffset() : m_nMargin;
	m_bTracking		= pTree ? pTree->IsTracking() : true;
	m_bReadOnlyMode = false;
	m_bViewType		= true;
	m_bAttachedToView = false;

	m_bShowButtons	= true;
	m_nBarHeight = 30;
	m_ButtonSize = CSize(26, 26);
	m_arrTempFiles.RemoveAll();

//	Register(0);
}


/////////////////////////////////////////////////////////////////////////////
// CContextAXCtrl::~CContextAXCtrl - Destructor

CContextAXCtrl::~CContextAXCtrl()
{
	RemoveTempFiles();
	// unregister from view
	if (m_sptrView)
	{
		UnRegister(m_sptrView);
	}

	UnRegister(0);
	
	_OleUnlockApp( this );

	m_lLastViewKey = INVALID_KEY;
	m_lLastDocKey = INVALID_KEY;
}


/////////////////////////////////////////////////////////////////////////////
// CContextAXCtrl::OnDraw - Drawing function

void CContextAXCtrl::OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	
}


/////////////////////////////////////////////////////////////////////////////
// CContextAXCtrl::DoPropExchange - Persistence support

void CContextAXCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CContextAXCtrl::OnResetState - Reset control to default state

void CContextAXCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CContextAXCtrl::AboutBox - Display an "About" box to the user

void CContextAXCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_CONTEXTAX);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CContextAXCtrl message handlers

int CContextAXCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!CreateButtons())
		return -1;

	CRect rect;
	GetClientRect(rect);
	rect.top += m_nBarHeight;

	DWORD dwStyle = WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_TABSTOP;
	if (!m_ViewType.Create(dwStyle, rect, this, ID_TYPE_TREE))
		return -1;

	m_ViewType.ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	if (!m_ViewObj.Create(dwStyle, rect, this, ID_OBJ_TREE))
		return -1;

	m_ViewObj.ModifyStyleEx(0, WS_EX_CLIENTEDGE);

	OnClickButtonTime();

	if (m_bViewType)
	{
		if (!m_btnType.IsPressed())
			m_btnType.SetPressedState();
		if (m_btnObj.IsPressed())
			m_btnObj.ResetPressedState();

		m_ViewType.ShowWindow(SW_SHOW);
		m_ViewObj.ShowWindow(SW_HIDE);
	}
	else
	{
		if (!m_btnObj.IsPressed())
			m_btnObj.SetPressedState();
		if (m_btnType.IsPressed())
			m_btnType.ResetPressedState();

		m_ViewType.ShowWindow(SW_HIDE);
		m_ViewObj.ShowWindow(SW_SHOW);
	}
	
	IUnknownPtr	sptrUnkMainFrame(::_GetOtherComponent(::GetAppUnknown(), IID_IMainWindow), false);
	IWindow2Ptr sptrMainFrame = sptrUnkMainFrame;
	if (sptrMainFrame != 0)
		sptrMainFrame->AttachMsgListener(GetControllingUnknown());

	return 0;
}

BOOL CContextAXCtrl::OnEraseBkgnd(CDC* pDC) 
{
	if (!pDC)
		return true;

	if (m_bShowButtons)
	{
		CRect rect;
		GetClientRect(rect);
		rect.bottom = rect.top + m_nBarHeight;

		CWnd* pThis = this;
		CWnd* pChild = pThis->GetWindow(GW_CHILD);
		while (pChild && ::IsWindow(pChild->GetSafeHwnd()))
		{
			CRect ChildRect;
			if (::IsWindowVisible(pChild->GetSafeHwnd()))
			{
				pChild->GetWindowRect(ChildRect);
				ScreenToClient(ChildRect);
				if (!ChildRect.IsRectEmpty())
					pDC->ExcludeClipRect(ChildRect);
			}

			pChild = pChild->GetWindow(GW_HWNDNEXT);
		}
		CBrush brush(::GetSysColor(COLOR_3DFACE));
		pDC->FillRect(rect, &brush);
	}
		
	return true;
	
//	return COleControl::OnEraseBkgnd(pDC);
}

void CContextAXCtrl::OnSize(UINT nType, int cx, int cy) 
{
	COleControl::OnSize(nType, cx, cy);
	
	CalcLayout();
}

void CContextAXCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CRect rc;
	CContextCtrl * pTree = m_bViewType ? m_ViewType.GetTree() : m_ViewObj.GetTree();
	ASSERT (pTree);

	HCONTEXTITEM hItem = pTree->GetItemFocus();

	if (point.x == -1 && point.y == -1)
	{
		// get coordinates
		if (hItem)
			pTree->GetItemRect(hItem, &rc);
		else
			GetClientRect(&rc);

		ClientToScreen(&rc);
		
		point.x = rc.left;
		point.y = rc.top;
	}

	GetClientRect(&rc);
	
	ClientToScreen(&rc);

	if (!rc.PtInRect(point))
		ClientToScreen(&point);

	// get command Manager
	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager2 );

	ASSERT(punk != NULL);
	if (!punk)
		return;

	sptrICommandManager2 sptrCmdMgr(punk);
	punk->Release();


	// get selected object data
	CContextItemData* pSelObjectData = (hItem) ? (CContextItemData*)pTree->GetItemData(hItem) : NULL;

	CString strObjectName;
	CString strObjectType;

	if (pSelObjectData)
	{	
		IUnknown * punkObj = pSelObjectData->GetPointer(false);

		strObjectName = (punkObj) ? pSelObjectData->GetName() : _T(""); 
		strObjectType = (punkObj) ? ::GetObjectKind(punkObj) : pSelObjectData->GetName(); 
	}


	UINT uCMD;
	_bstr_t	bstrMenuName = _T("Context Viewer");

	// register context menu initializer
	sptrCmdMgr->RegisterMenuCreator((IUnknown*)&(this->m_xViewer));

	// execute context menu (return value is action's ID which selected by user)
	sptrCmdMgr->ExecuteContextMenu3(bstrMenuName, point, TPM_RETURNCMD, &uCMD);

	sptrCmdMgr->UnRegisterMenuCreator((IUnknown*)&(this->m_xViewer));

	// set selected action arguments
	bool bExec = true;

	BSTR	bstrActionName;
	CString strArgs;
	DWORD	dwFlag = 0;

	if (!SUCCEEDED(sptrCmdMgr->GetActionName(&bstrActionName, uCMD)))
		return;
	
	CString strActionName = bstrActionName;
	::SysFreeString(bstrActionName);


//	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
// return action name by Resource ID
/*
	if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_ACTION_NEWOBJECT)))
	{
		strArgs = CString("\"") + strObjectType + CString("\"");
		dwFlag = aefShowInterfaceAnyway;
	}
	else if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_ACTION_SELECTOBJECT)))
	{
		strArgs = CString("\"") + strObjectName + CString("\"");
		dwFlag = aefShowInterfaceByRequired;
	}
	else if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_ACTION_DELETEOBJECT)))
	{
		strArgs = CString("\"") + strObjectName + CString("\"");
		dwFlag = aefShowInterfaceByRequired;
	}
	else if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_ACTION_RENAMEOBJECT)))
	{
		strArgs = CString("\"") + strObjectName + CString("\"");
		dwFlag = aefShowInterfaceAnyway;
	}
	else if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_ACTION_OBJECTPROPS)))
	{
		strArgs = CString("\"") + strObjectName + CString("\"");
		dwFlag = aefShowInterfaceByRequired;
	}
	else if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_ACTION_LOADOBJECT)))
	{
		strArgs = CString("\"") + strObjectType + CString("\"");
		dwFlag = aefShowInterfaceByRequired;
	}
	else if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_ACTION_SAVEOBJECT)))
	{
		strArgs = CString("\"") + strObjectName + CString("\"");
		dwFlag = aefShowInterfaceByRequired;
	}
	else
*/
		bExec = false;

	if (bExec)
		::ExecuteAction(strActionName, strArgs, dwFlag);
	
}



/////////////////////////////////////////////////////////////////////////////
// CContextAXCtrl message handlers
/*
bool CContextAXCtrl::DoCreate(DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID)
{
	if (!Create(WS_CHILD | WS_VISIBLE | WS_EX_CLIENTEDGE, rc, pparent, nID))
		return false;

	// and addition style
	ModifyStyle(WS_BORDER, 0);

	// set client edge
//	ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	
	SetWindowText("Context Viewer");

	if (m_bViewType)
	{
		m_ViewType.ShowWindow(SW_SHOW);
		m_ViewObj.ShowWindow(SW_HIDE);

		m_btnNumOff.SetDisabled(false);
		m_btnNumOn.SetDisabled(false);
	}
	else
	{
		m_ViewType.ShowWindow(SW_HIDE);
		m_ViewObj.ShowWindow(SW_SHOW);

		m_btnNumOff.SetDisabled(true);
		m_btnNumOn.SetDisabled(true);
	}

	return true;
}
*/
void  CContextAXCtrl::SetViewType(BOOL bShow)
{
	if (m_bViewType == bShow)
		return;

	m_bViewType = bShow;
	if (!::IsWindow(GetSafeHwnd()))
		return;

	m_ViewObj.ShowWindow(SW_HIDE);
	m_ViewType.ShowWindow(SW_HIDE);
	CalcLayout();
	if (m_bViewType)
	{
		m_ViewType.SetRedraw(false, false);

		m_ViewType.Init(m_ViewObj.GetContext(false), m_ViewObj.GetNamedData(false));

		IUnknownPtr sptr = m_ViewObj.GetFocusObject(false);
		if (sptr != 0)
			m_ViewType.SetFocusObject(sptr);

		m_ViewType.SetRedraw(true, true);
		m_ViewType.ShowWindow(SW_SHOW);

		m_btnNumOff.SetDisabled(false);
		m_btnNumOn.SetDisabled(false);
		if (GetFocus() != &m_ViewType)
			m_ViewType.SetFocus();
	}
	else
	{
		m_ViewObj.SetRedraw(false, false);

		
		m_ViewObj.Init(m_ViewType.GetContext(false), m_ViewType.GetNamedData(false));

		IUnknownPtr sptr = m_ViewType.GetFocusObject(false);
		if (sptr != 0)
			m_ViewObj.SetFocusObject(sptr);

		m_ViewObj.SetRedraw(true, true);
		m_ViewObj.ShowWindow(SW_SHOW);

		m_btnNumOff.SetDisabled(true);
		m_btnNumOn.SetDisabled(true);
		if (GetFocus() != &m_ViewObj)
			m_ViewObj.SetFocus();
	}
}


void CContextAXCtrl::SetNumeric()
{
	if (m_bViewType)
		m_ViewType.SetNumeric();
	else
		m_ViewObj.SetNumeric();
}

void CContextAXCtrl::RemoveNumeric()
{
	if (m_bViewType)
		m_ViewType.RemoveNumeric();
	else
		m_ViewObj.RemoveNumeric();
}

void CContextAXCtrl::Expand(bool bExpand)
{
	CContextCtrl * pTree = m_bViewType ? m_ViewType.GetTree() : m_ViewObj.GetTree();
	ASSERT (pTree);
	if (pTree)
		pTree->ExpandAll(bExpand ? CVE_EXPAND : CVE_COLLAPSE, true);
}

void CContextAXCtrl::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	// check for this control exists
	if (!GetSafeHwnd())
		return;

	if (m_bViewType)
		OnNotifyViewType(pszEvent, punkHit, punkFrom, pdata, cbSize );
	else 
		OnNotifyViewObject(pszEvent, punkHit, punkFrom, pdata, cbSize );
}

void CContextAXCtrl::OnNotifyViewType( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	// lock window for update
	BOOL bPrevRedraw = m_ViewType.SetRedraw(false, false);

	_try(CContextAXCtrl, OnNotifyViewType)
	{
		// if "AppActivateView"
		if (!lstrcmp(pszEvent, szAppActivateView))
		{
			// NOTE:
			// punkFrom == view
			// punkHit == document (NamedData)

			if (!punkFrom) // last view removed
			{
				m_ViewType.ClearContens();
				m_lLastViewKey = INVALID_KEY;
				m_lLastDocKey = INVALID_KEY;

				if (m_sptrView) // criminal
				{
					ASSERT (false);
					m_sptrView = 0;
				}
				m_ViewType.SetRedraw(bPrevRedraw, bPrevRedraw);
				return;
			}

			// if view is same as current ->simply return 
			GuidKey	viewKey = ::GetObjectKey(punkFrom);
			if (m_lLastViewKey == viewKey)
			{
				m_ViewType.SetRedraw(bPrevRedraw, bPrevRedraw);
				return;
			}

			// else unregister viewer from current view
			if (m_sptrView)
				UnRegister(m_sptrView);

			// and register viewer to new view
			m_sptrView = punkFrom;
			if (m_sptrView == 0)
			{
				m_ViewType.SetRedraw(bPrevRedraw, bPrevRedraw);
				return;
			}

			Register(m_sptrView);
			m_lLastViewKey = viewKey;

			// set interfaces 
			m_ViewType.AttachData(punkFrom, punkHit);

			// check new doc is same as current
			GuidKey	docKey = ::GetObjectKey(punkHit);
			if (docKey != m_lLastDocKey)
			{
				// if not so -> clear content
				m_ViewType.ClearContens();
				m_lLastDocKey = docKey;

				// and process view's contens
				m_ViewType.AddAllTypes(true);
			}
//			else
//				m_ViewType.UpdateEnables();

			// update selection and active
			m_ViewType.UpdateSelections();
			m_ViewType.UpdateActive();

			// release interfaces
			m_ViewType.AttachData(0, 0);

		}// if "DestroyView"
		else if (!lstrcmp(pszEvent, szAppDestroyView))
		{
			// if view destroyed is current view
			if (::GetObjectKey(punkFrom) == m_lLastViewKey)
			{
				// unegister from current view
				m_sptrView = 0;
				UnRegister(punkFrom);
				m_lLastViewKey = INVALID_KEY;
			}
		}// if "ChangeObjectKey"
		else if (!lstrcmp(pszEvent, szEventActiveContextChangeObjectKey))
		{
			GuidKey viewKey = ::GetObjectKey(punkFrom);

			// if objects view  is own view
			if (m_lLastViewKey == viewKey)
			{
				sptrINamedDataObject sptrObj(punkHit);	// NamedObject
				GuidKey PrevKey = *(GUID*)pdata;

				if (PrevKey != INVALID_KEY)
				{
					m_ViewType.AttachData(punkFrom, 0);

					m_ViewType.ChangeObjectKey(punkHit, PrevKey);

					m_ViewType.AttachData(0, 0);
				}
			}
		}// if "ActiveContextChange"
		else if (!lstrcmp(pszEvent, szEventActiveContextChange))
		{
			NotifyCodes type = *(NotifyCodes*)pdata;
			GuidKey viewKey = ::GetObjectKey(punkFrom);
			bool bRet = true;

			// if objects view is own view
			if (m_lLastViewKey == viewKey)
			{
				sptrINamedDataObject sptrObj(punkHit);	// NamedObject

				// set interfaces pointers
				m_ViewType.AttachData(punkFrom, 0);
				
				// get type of object's activation and make corresponding respond 
				switch (type)
				{
				// add new object and set it to active state
				case ncAddObject:
					bRet = m_ViewType.AddObject(punkHit, true);
					break;

				// we just removed object
				case ncRemoveObject:
					bRet  = m_ViewType.RemoveObject(punkHit);
					break;
				// empty context
				case ncEmpty:
					break;

				// upadte selection
				case ncUpdateSelection:
					m_ViewType.UpdateSelections();
					m_ViewType.UpdateActive();
					break;

				// change object's base key
				case ncChangeBase:
					m_ViewType.UpdateSelections();
					m_ViewType.UpdateActive();
					break;

				// if "ContextRescan"
				case ncRescan:
					{
						SCROLLINFO sf;
						BOOL bScroll = m_ViewType.SaveScrollInfo(&sf);

						// we need to clear contens
						m_ViewType.ClearContens();
						// and fill types
						m_ViewType.AddAllTypes(true);

						m_ViewType.UpdateSelections();
						m_ViewType.UpdateActive();

						if (bScroll)
						{
							SCROLLINFO sfNew;
							m_ViewType.SaveScrollInfo(&sfNew);
							if (sfNew.nPage == sf.nPage)
								m_ViewType.RestoreScrollInfo(&sf, bPrevRedraw == TRUE);

							HCONTEXTITEM hFocus = m_ViewType.GetTree()->GetItemFocus();
							if (hFocus)
								m_ViewType.GetTree()->EnsureVisible(hFocus);
						}
					}
					break;
				}// switch

				// release interfaces
				m_ViewType.AttachData(0, 0);
			}// if own view
		}
		else if (!strcmp(pszEvent, szEventNewSettings))
		{
			if (GetSafeHwnd())
				Invalidate();
		}
		
	}// _try
	_catch;
	// unlock window for update
	m_ViewType.SetRedraw(bPrevRedraw, bPrevRedraw);
}

void CContextAXCtrl::OnNotifyViewObject( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	// lock window for update
	BOOL bPrevRedraw = m_ViewObj.SetRedraw(false, false);

	_try(CContextAXCtrl, OnNotifyViewObject)
	{
		// if "AppActivateView"
		if (!lstrcmp(pszEvent, szAppActivateView))
		{
			// NOTE:
			// punkFrom == view
			// punkHit == document (NamedData)

			if (!punkFrom) // last view removed
			{
				m_ViewObj.ClearContens();
				m_lLastViewKey = INVALID_KEY;
				m_lLastDocKey = INVALID_KEY;

				if (m_sptrView) // criminal
				{
					ASSERT (false);
					m_sptrView = 0;
				}
				m_ViewObj.SetRedraw(bPrevRedraw, bPrevRedraw);
				return;
			}

			// if view is same as current ->simply return 
			GuidKey	viewKey = ::GetObjectKey(punkFrom);
			if (m_lLastViewKey == viewKey)
			{
				m_ViewObj.SetRedraw(bPrevRedraw, bPrevRedraw);
				return;
			}

			// else unregister viewer from current view
			if (m_sptrView)
				UnRegister(m_sptrView);

			// and register viewer to new view
			m_sptrView = punkFrom;
			Register(m_sptrView);
			m_lLastViewKey = viewKey;

			// set interfaces 
			m_ViewObj.AttachData(punkFrom, punkHit);

			// check new doc is same as current
			GuidKey	docKey = ::GetObjectKey(punkHit);
			if (docKey != m_lLastDocKey)
			{
				// if not so -> clear content
				m_ViewObj.ClearContens();
				m_lLastDocKey = docKey;

				// and process view's contens
				m_ViewObj.AddAllObjects();
			}

			// update selection and active
			m_ViewObj.UpdateSelections();
			m_ViewObj.UpdateActive();

			// release interfaces
			m_ViewObj.AttachData(0, 0);

		}// if "DestroyView"
		else if (!lstrcmp(pszEvent, szAppDestroyView))
		{
			// if view destroyed is current view
			if (::GetObjectKey(punkFrom) == m_lLastViewKey)
			{
				// unegister from current view
				m_sptrView = 0;
				UnRegister(punkFrom);
				m_lLastViewKey = INVALID_KEY;
			}
		}// if "ChangeObjectKey"
		else if (!lstrcmp(pszEvent, szEventActiveContextChangeObjectKey))
		{
			GuidKey viewKey = ::GetObjectKey(punkFrom);

			// if objects view  is own view
			if (m_lLastViewKey == viewKey)
			{
				sptrINamedDataObject sptrObj(punkHit);	// NamedObject
				GuidKey PrevKey = *(GuidKey*)pdata;

				if (PrevKey != INVALID_KEY)
				{
					m_ViewObj.AttachData(punkFrom, 0);

					m_ViewObj.ChangeObjectKey(punkHit, PrevKey);

					m_ViewObj.AttachData(0, 0);
				}
			}
		}// if "ActiveContextChange"
		else if (!lstrcmp(pszEvent, szEventActiveContextChange))
		{
			NotifyCodes type = *(NotifyCodes*)pdata;
			GuidKey viewKey = ::GetObjectKey(punkFrom);
			bool bRet = true;

			// if objects view  is own view
			if (m_lLastViewKey == viewKey)
			{
				sptrINamedDataObject sptrObj(punkHit);	// NamedObject

				m_ViewObj.AttachData(punkFrom, 0);
				// get type of object's activation and make corresponding respond 
				switch (type)
				{
				// add new object and set it to active state
				case ncAddObject:
					bRet = m_ViewObj.AddObject(sptrObj, true);
					break;

				// we just removed object
				case ncRemoveObject:
					bRet  = m_ViewObj.RemoveObject(sptrObj);
					break;

				// only from just synchoronized context
				case ncUpdateSelection:
					m_ViewObj.UpdateSelections();
					m_ViewObj.UpdateActive();
					break;
				// change object's base key
				case ncChangeBase:
					bRet = m_ViewObj.ChangeBaseObject(sptrObj);
					break;

				case ncEmpty:
					bRet = true;
					break;

				case ncRescan:
					{	
						SCROLLINFO sf;
						BOOL bScroll = m_ViewObj.SaveScrollInfo(&sf);

						// we need to clear contens
						m_ViewObj.ClearContens();
						// and fill types
						m_ViewObj.AddAllObjects();
						m_ViewObj.UpdateSelections();
						m_ViewObj.UpdateActive();
						if (bScroll)
						{
							sf.fMask = SIF_ALL;
							m_ViewObj.RestoreScrollInfo(&sf);
							HCONTEXTITEM hFocus = m_ViewObj.GetTree()->GetItemFocus();
							if (hFocus)
								m_ViewObj.GetTree()->EnsureVisible(hFocus);
						}
					}
					break;
				}// switch

				m_ViewObj.AttachData(0, 0);
			}// if own view
		}
		else if (!strcmp(pszEvent, szEventNewSettings))
		{
			if (GetSafeHwnd())
				Invalidate();
		}
	}// _try
	_catch;
	// unlock window for update
	m_ViewObj.SetRedraw(bPrevRedraw, bPrevRedraw);
}


///////////////////////////////////////////////////////////////////////////////
// User events handlers

bool CContextAXCtrl::CalcLayout()
{
	if (!::IsWindow(GetSafeHwnd()))
		return true;

	CRect ClientRect;
	GetClientRect(ClientRect);
	if (ClientRect.IsRectEmpty())
		return true;

	if (m_bShowButtons)
	{
		int dy = (m_nBarHeight - m_ButtonSize.cy) / 2;

		CRect ButtonRect(0, 0, m_ButtonSize.cx, m_ButtonSize.cy);
		ButtonRect.OffsetRect(dy, dy);
		if (::IsWindow(m_btnType.GetSafeHwnd()))
		{
			m_btnType.MoveWindow(ButtonRect);
			if (!::IsWindowVisible(m_btnType.GetSafeHwnd()))
				m_btnType.ShowWindow(SW_SHOW);
		}

		ButtonRect.OffsetRect(m_ButtonSize.cx + 1, 0);
		if (::IsWindow(m_btnObj.GetSafeHwnd()))
		{
			m_btnObj.MoveWindow(ButtonRect);
			if (!::IsWindowVisible(m_btnObj.GetSafeHwnd()))
				m_btnObj.ShowWindow(SW_SHOW);
		}

		ButtonRect.OffsetRect(m_ButtonSize.cx + 1, 0);
		if (::IsWindow(m_btnAbc.GetSafeHwnd()))
		{
			m_btnAbc.MoveWindow(ButtonRect);
			if (!::IsWindowVisible(m_btnAbc.GetSafeHwnd()))
				m_btnAbc.ShowWindow(SW_SHOW);
		}

		ButtonRect.OffsetRect(m_ButtonSize.cx + 1, 0);
		if (::IsWindow(m_btnTime.GetSafeHwnd()))
		{
			m_btnTime.MoveWindow(ButtonRect);
			if (!::IsWindowVisible(m_btnTime.GetSafeHwnd()))
				m_btnTime.ShowWindow(SW_SHOW);
		}

		ButtonRect.OffsetRect(m_ButtonSize.cx + 1, 0);
		if (::IsWindow(m_btnNumOn.GetSafeHwnd()))
		{
			m_btnNumOn.MoveWindow(ButtonRect);
			if (!::IsWindowVisible(m_btnNumOn.GetSafeHwnd()))
				m_btnNumOn.ShowWindow(SW_SHOW);
		}
		
		ButtonRect.OffsetRect(m_ButtonSize.cx + 1, 0);
		if (::IsWindow(m_btnNumOff.GetSafeHwnd()))
		{
			m_btnNumOff.MoveWindow(ButtonRect);
			if (!::IsWindowVisible(m_btnNumOff.GetSafeHwnd()))
				m_btnNumOff.ShowWindow(SW_SHOW);
		}

		ButtonRect.OffsetRect(m_ButtonSize.cx + 1, 0);
		if (::IsWindow(m_btnExpand.GetSafeHwnd()))
		{
			m_btnExpand.MoveWindow(ButtonRect);
			if (!::IsWindowVisible(m_btnExpand.GetSafeHwnd()))
				m_btnExpand.ShowWindow(SW_SHOW);
		}

		ButtonRect.OffsetRect(m_ButtonSize.cx + 1, 0);
		if (::IsWindow(m_btnCollapse.GetSafeHwnd()))
		{
			m_btnCollapse.MoveWindow(ButtonRect);
			if (!::IsWindowVisible(m_btnCollapse.GetSafeHwnd()))
				m_btnCollapse.ShowWindow(SW_SHOW);
		}

		ClientRect.top += m_nBarHeight;
	}
	else
	{
		if (::IsWindow(m_btnType.GetSafeHwnd()) && ::IsWindowVisible(m_btnType.GetSafeHwnd()))
			m_btnType.ShowWindow(SW_HIDE);

		if (::IsWindow(m_btnObj.GetSafeHwnd()) && ::IsWindowVisible(m_btnObj.GetSafeHwnd()))
			m_btnObj.ShowWindow(SW_HIDE);

		if (::IsWindow(m_btnAbc.GetSafeHwnd()) && ::IsWindowVisible(m_btnAbc.GetSafeHwnd()))
			m_btnAbc.ShowWindow(SW_HIDE);
		
		if (::IsWindow(m_btnTime.GetSafeHwnd()) && ::IsWindowVisible(m_btnTime.GetSafeHwnd()))
			m_btnTime.ShowWindow(SW_HIDE);

		if (::IsWindow(m_btnNumOn.GetSafeHwnd()) && ::IsWindowVisible(m_btnNumOn.GetSafeHwnd()))
			m_btnNumOn.ShowWindow(SW_HIDE);
		
		if (::IsWindow(m_btnNumOff.GetSafeHwnd()) && ::IsWindowVisible(m_btnNumOff.GetSafeHwnd()))
			m_btnNumOff.ShowWindow(SW_HIDE);

		if (::IsWindow(m_btnExpand.GetSafeHwnd()) && ::IsWindowVisible(m_btnExpand.GetSafeHwnd()))
			m_btnExpand.ShowWindow(SW_HIDE);

		if (::IsWindow(m_btnCollapse.GetSafeHwnd()) && ::IsWindowVisible(m_btnCollapse.GetSafeHwnd()))
			m_btnCollapse.ShowWindow(SW_HIDE);

	}

	if (m_bViewType)
	{
		if (::IsWindow(m_ViewType.GetSafeHwnd()))
			m_ViewType.MoveWindow(ClientRect, false);
	}
	else
	{
		if (::IsWindow(m_ViewObj.GetSafeHwnd()))
			m_ViewObj.MoveWindow(ClientRect, false);
	}

	return true;
}

bool CContextAXCtrl::CreateButtons()
{
	int dy = (m_nBarHeight - m_ButtonSize.cy) / 2;

	CRect ButtonRect(0, 0, m_ButtonSize.cx, m_ButtonSize.cy);
	ButtonRect.OffsetRect(dy, dy);

		
	DWORD dwButtonStyle = WS_CHILD|WS_CLIPSIBLINGS|WS_TABSTOP;

// button_type
	// create temp file
	CString strName = GetTempFileName();
	if (strName.IsEmpty())
		return false;
	// save bitmap from resource to temp file
	if (!::SaveBMPFile(IDB_BUTTON_TYPE_SM, strName))
		return false;
	m_arrTempFiles.Add(strName);

	// create button
	if (!m_btnType.Create(0, dwButtonStyle|BS_PUSHBUTTON /*BS_RADIOBUTTON*/, ButtonRect, this, IDC_BUTTON_TYPE))
		return false;
	// and hide button
	m_btnType.ShowWindow(SW_HIDE);

	// set file name to button
	m_btnType.SetBMPFileName(strName);
	m_btnType.SetButtonHeight(m_ButtonSize.cy);
	m_btnType.SetButtonWidth(m_ButtonSize.cx);
	m_btnType.SetButtonText("");
//	m_btnType.SetStretch(true);
	m_btnType.SetMouseInPicColor(true);
	m_btnType.SetMouseOutPicColor(true);
	m_btnType.SetButtonPressedPicColor(true);
	m_btnType.SetAdvancedShadow(true);
	m_btnType.SetThreeStateButton(true);
	m_btnType.SetDrawBorder(false);
	m_btnType.SetDrawFocusRect(false);
	m_btnType.SetDefaultButton(false);
	m_btnType.SetColorTransparent(::GetSysColor(COLOR_3DFACE));


// button_Object
	// get new temp file name
	strName = GetTempFileName();
	if (strName.IsEmpty())
		return false;
	// save bitmap from resource to temp file
	if (!::SaveBMPFile(IDB_BUTTON_OBJ_SM, strName))
		return false;
	m_arrTempFiles.Add(strName);
	
	// create button
	ButtonRect.OffsetRect(m_ButtonSize.cx + 1, 0);
	if (!m_btnObj.Create(0, dwButtonStyle|BS_PUSHBUTTON /*BS_RADIOBUTTON*/, ButtonRect, this, IDC_BUTTON_OBJ))
		return false;
	// and hide button
	m_btnObj.ShowWindow(SW_HIDE);

	// set file name to button
	m_btnObj.SetBMPFileName(strName);
	m_btnObj.SetButtonHeight(m_ButtonSize.cy);
	m_btnObj.SetButtonWidth(m_ButtonSize.cx);
	m_btnObj.SetButtonText("");
//	m_btnObj.SetStretch(true);
	m_btnObj.SetMouseInPicColor(true);
	m_btnObj.SetMouseOutPicColor(true);
	m_btnObj.SetButtonPressedPicColor(true);
	m_btnObj.SetAdvancedShadow(true);
	m_btnObj.SetThreeStateButton(true);
	m_btnObj.SetDrawBorder(false);
	m_btnObj.SetDrawFocusRect(false);
	m_btnObj.SetDefaultButton(false);
	m_btnObj.SetColorTransparent(::GetSysColor(COLOR_3DFACE));

// button_Abc
	// get new temp file name
	strName = GetTempFileName();
	if (strName.IsEmpty())
		return false;
	// save bitmap from resource to temp file
	if (!::SaveBMPFile(IDB_BUTTON_ABC, strName))
		return false;
	m_arrTempFiles.Add(strName);
	
	// create button
	ButtonRect.OffsetRect(m_ButtonSize.cx + 1, 0);
	if (!m_btnAbc.Create(0, dwButtonStyle|BS_PUSHBUTTON /*BS_RADIOBUTTON*/, ButtonRect, this, IDC_BUTTON_ABC))
		return false;
	// and hide button
	m_btnAbc.ShowWindow(SW_HIDE);

	// set file name to button
	m_btnAbc.SetBMPFileName(strName);
	m_btnAbc.SetButtonHeight(m_ButtonSize.cy);
	m_btnAbc.SetButtonWidth(m_ButtonSize.cx);
	m_btnAbc.SetButtonText("");
//	m_btnAbc.SetStretch(true);
	m_btnAbc.SetMouseInPicColor(true);
	m_btnAbc.SetMouseOutPicColor(true);
	m_btnAbc.SetButtonPressedPicColor(true);
	m_btnAbc.SetAdvancedShadow(true);
	m_btnAbc.SetThreeStateButton(true);
	m_btnAbc.SetDrawBorder(false);
	m_btnAbc.SetDrawFocusRect(false);
	m_btnAbc.SetDefaultButton(false);
	m_btnAbc.SetColorTransparent(::GetSysColor(COLOR_3DFACE));

// button_Time
	// get new temp file name
	strName = GetTempFileName();
	if (strName.IsEmpty())
		return false;
	// save bitmap from resource to temp file
	if (!::SaveBMPFile(IDB_BUTTON_TIME, strName))
		return false;
	m_arrTempFiles.Add(strName);
	
	// create button
	ButtonRect.OffsetRect(m_ButtonSize.cx + 1, 0);
	if (!m_btnTime.Create(0, dwButtonStyle|BS_PUSHBUTTON /*BS_RADIOBUTTON*/, ButtonRect, this, IDC_BUTTON_TIME))
		return false;
	// and hide button
	m_btnTime.ShowWindow(SW_HIDE);

	// set file name to button
	m_btnTime.SetBMPFileName(strName);
	m_btnTime.SetButtonHeight(m_ButtonSize.cy);
	m_btnTime.SetButtonWidth(m_ButtonSize.cx);
	m_btnTime.SetButtonText("");
//	m_btnTime.SetStretch(true);
	m_btnTime.SetMouseInPicColor(true);
	m_btnTime.SetMouseOutPicColor(true);
	m_btnTime.SetButtonPressedPicColor(true);
	m_btnTime.SetAdvancedShadow(true);
	m_btnTime.SetThreeStateButton(true);
	m_btnTime.SetDrawBorder(false);
	m_btnTime.SetDrawFocusRect(false);
	m_btnTime.SetDefaultButton(false);
	m_btnTime.SetColorTransparent(::GetSysColor(COLOR_3DFACE));

// button_NumOn
	// get new temp file name
	strName = GetTempFileName();
	if (strName.IsEmpty())
		return false;
	// save bitmap from resource to temp file
	if (!::SaveBMPFile(IDB_BUTTON_NUMBER_ON_SM, strName))
		return false;
	m_arrTempFiles.Add(strName);
	
	// create button
	ButtonRect.OffsetRect(m_ButtonSize.cx + 1, 0);
	if (!m_btnNumOn.Create(0, dwButtonStyle|BS_PUSHBUTTON, ButtonRect, this, IDC_BUTTON_NUMBER_OFF))
		return false;
	// and hide button
	m_btnNumOn.ShowWindow(SW_HIDE);

	// set file name to button
	m_btnNumOn.SetBMPFileName(strName);
	m_btnNumOn.SetButtonHeight(m_ButtonSize.cy);
	m_btnNumOn.SetButtonWidth(m_ButtonSize.cx);
	m_btnNumOn.SetButtonText("");
//	m_btnNumOn.SetStretch(true);
	m_btnNumOn.SetMouseInPicColor(true);
	m_btnNumOn.SetMouseOutPicColor(true);
	m_btnNumOn.SetButtonPressedPicColor(true);
	m_btnNumOn.SetAdvancedShadow(true);
	m_btnNumOn.SetThreeStateButton(false);
	m_btnNumOn.SetDrawBorder(false);
	m_btnNumOn.SetDrawFocusRect(false);
	m_btnNumOn.SetDefaultButton(false);
	m_btnNumOn.SetColorTransparent(::GetSysColor(COLOR_3DFACE));

// button_NumOff
	// get new temp file name
	strName = GetTempFileName();
	if (strName.IsEmpty())
		return false;
	// save bitmap from resource to temp file
	if (!::SaveBMPFile(IDB_BUTTON_NUMBER_OFF_SM, strName))
		return false;
	m_arrTempFiles.Add(strName);
	
	// create button
	ButtonRect.OffsetRect(m_ButtonSize.cx + 1, 0);
	if (!m_btnNumOff.Create(0, dwButtonStyle|BS_PUSHBUTTON, ButtonRect, this, IDC_BUTTON_NUMBER_ON))
		return false;
	// and hide button
	m_btnNumOff.ShowWindow(SW_HIDE);
	// set file name to button
	m_btnNumOff.SetBMPFileName(strName);
	m_btnNumOff.SetButtonHeight(m_ButtonSize.cy);
	m_btnNumOff.SetButtonWidth(m_ButtonSize.cx);
	m_btnNumOff.SetButtonText("");
//	m_btnNumOff.SetStretch(true);
	m_btnNumOff.SetMouseInPicColor(true);
	m_btnNumOff.SetMouseOutPicColor(true);
	m_btnNumOff.SetButtonPressedPicColor(true);
	m_btnNumOff.SetAdvancedShadow(true);
	m_btnNumOff.SetThreeStateButton(false);
	m_btnNumOff.SetDrawBorder(false);
	m_btnNumOff.SetDrawFocusRect(false);
	m_btnNumOff.SetDefaultButton(false);
	m_btnNumOff.SetColorTransparent(::GetSysColor(COLOR_3DFACE));

// button_Expand
	// get new temp file name
	strName = GetTempFileName();
	if (strName.IsEmpty())
		return false;
	// save bitmap from resource to temp file
	if (!::SaveBMPFile(IDB_BUTTON_EXPAND_SM, strName))
		return false;
	m_arrTempFiles.Add(strName);
	
	// create button
	ButtonRect.OffsetRect(m_ButtonSize.cx + 1, 0);
	if (!m_btnExpand.Create(0, dwButtonStyle|BS_PUSHBUTTON, ButtonRect, this, IDC_BUTTON_EXPAND))
		return false;
	// and hide button
	m_btnExpand.ShowWindow(SW_HIDE);
	// set file name to button
	m_btnExpand.SetBMPFileName(strName);
	m_btnExpand.SetButtonHeight(m_ButtonSize.cy);
	m_btnExpand.SetButtonWidth(m_ButtonSize.cx);
	m_btnExpand.SetButtonText("");
//	m_btnExpand.SetStretch(true);
	m_btnExpand.SetMouseInPicColor(true);
	m_btnExpand.SetMouseOutPicColor(true);
	m_btnExpand.SetButtonPressedPicColor(true);
	m_btnExpand.SetAdvancedShadow(true);
	m_btnExpand.SetThreeStateButton(false);
	m_btnExpand.SetDrawBorder(false);
	m_btnExpand.SetDrawFocusRect(false);
	m_btnExpand.SetDefaultButton(false);
	m_btnExpand.SetColorTransparent(::GetSysColor(COLOR_3DFACE));

// button_Collapse
	// get new temp file name
	strName = GetTempFileName();
	if (strName.IsEmpty())
		return false;
	// save bitmap from resource to temp file
	if (!::SaveBMPFile(IDB_BUTTON_COLLAPSE_SM, strName))
		return false;
	m_arrTempFiles.Add(strName);
	
	// create button
	ButtonRect.OffsetRect(m_ButtonSize.cx + 1, 0);
	if (!m_btnCollapse.Create(0, dwButtonStyle|BS_PUSHBUTTON, ButtonRect, this, IDC_BUTTON_COLLAPSE))
		return false;
	// and hide button
	m_btnCollapse.ShowWindow(SW_HIDE);
	// set file name to button
	m_btnCollapse.SetBMPFileName(strName);
	m_btnCollapse.SetButtonHeight(m_ButtonSize.cy);
	m_btnCollapse.SetButtonWidth(m_ButtonSize.cx);
	m_btnCollapse.SetButtonText("");
//	m_btnCollapse.SetStretch(true);
	m_btnCollapse.SetMouseInPicColor(true);
	m_btnCollapse.SetMouseOutPicColor(true);
	m_btnCollapse.SetButtonPressedPicColor(true);
	m_btnCollapse.SetAdvancedShadow(true);
	m_btnCollapse.SetThreeStateButton(false);
	m_btnCollapse.SetDrawBorder(false);
	m_btnCollapse.SetDrawFocusRect(false);
	m_btnCollapse.SetDefaultButton(false);
	m_btnCollapse.SetColorTransparent(::GetSysColor(COLOR_3DFACE));

	return true;
}

void CContextAXCtrl::OnClickButtonType()
{
	if (!m_btnType.IsPressed())
		m_btnType.SetPressedState();
	if (m_btnObj.IsPressed())
		m_btnObj.ResetPressedState();

	m_btnNumOff.SetDisabled(false);
	m_btnNumOn.SetDisabled(false);

	if (m_bViewType)
		return;
	SetViewType(true);
}

void CContextAXCtrl::OnClickButtonObj()
{
	if (!m_btnObj.IsPressed())
		m_btnObj.SetPressedState();
	if (m_btnType.IsPressed())
		m_btnType.ResetPressedState();

	m_btnNumOff.SetDisabled(true);
	m_btnNumOn.SetDisabled(true);

	if (!m_bViewType)
		return;
	SetViewType(false);
}

void CContextAXCtrl::OnClickButtonAbc()
{
	if (!m_btnAbc.IsPressed())
		m_btnAbc.SetPressedState();
	if (m_btnTime.IsPressed())
		m_btnTime.ResetPressedState();

	m_ViewObj.SetSortByTime(false);
	m_ViewType.SetSortByTime(false);
}

void CContextAXCtrl::OnClickButtonTime()
{
	if (!m_btnTime.IsPressed())
		m_btnTime.SetPressedState();
	if (m_btnAbc.IsPressed())
		m_btnAbc.ResetPressedState();

	m_ViewObj.SetSortByTime(true);
	m_ViewType.SetSortByTime(true);
}

void CContextAXCtrl::OnClickButtonNumberOn()
{
	SetNumeric();
}

void CContextAXCtrl::OnClickButtonNumberOff()
{
	RemoveNumeric();
}

void CContextAXCtrl::OnClickButtonExpand()
{
	Expand(true);
}

void CContextAXCtrl::OnClickButtonCollapse()
{
	Expand(false);
}

void CContextAXCtrl::RemoveTempFiles()
{
	for (int i = 0; i < m_arrTempFiles.GetSize(); i++)
		CFile::Remove(m_arrTempFiles[i]);

	m_arrTempFiles.RemoveAll();
}

bool SaveBMPFile(UINT idRes, LPCTSTR szFile)
{
	CDC memDC;
	if (!memDC.CreateCompatibleDC(NULL))
		return false;

	CBitmap bitmap;
	if (!bitmap.LoadBitmap(idRes))
		return false;

	CBitmap * pOldBitmap = memDC.SelectObject(&bitmap);

	PBITMAPINFO pbmi = 0;
	BITMAP		bmp;
	WORD		cClrBits; 

	// Retrieve the bitmap's color format, width, and height.
	if (!bitmap.GetBitmap(&bmp))
	{
		memDC.SelectObject(pOldBitmap);
		bitmap.DeleteObject();
		return false;
	}

	// Convert the color format to a count of bits. 
	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 

	if (cClrBits == 1) 
		cClrBits = 1; 
	else if (cClrBits <= 4) 
		cClrBits = 4; 
	else if (cClrBits <= 8) 
		cClrBits = 8; 
	else if (cClrBits <= 16) 
		cClrBits = 16; 
	else if (cClrBits <= 24) 
		cClrBits = 24; 
    else 
        cClrBits = 32; 
 
//	 Allocate memory for the BITMAPINFO structure. (This structure 
//	 contains a BITMAPINFOHEADER structure and an array of RGBQUAD data 
//	 structures.)
 	if (cClrBits != 24)
	{
		pbmi = (PBITMAPINFO)new BYTE [sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1<<cClrBits)];
		memset(pbmi, 0, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1<<cClrBits));
	}
	else
	{
	 // There is no RGBQUAD array for the 24-bit-per-pixel format.
		pbmi = (PBITMAPINFO)new BYTE [sizeof(BITMAPINFOHEADER)];
		memset(pbmi, 0, sizeof(BITMAPINFOHEADER));
	}
	if (!pbmi)
	{
		memDC.SelectObject(pOldBitmap);
		bitmap.DeleteObject();
		return false;
	}
  
    // Initialize the fields in the BITMAPINFO structure.
    pbmi->bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth		= bmp.bmWidth;
    pbmi->bmiHeader.biHeight	= bmp.bmHeight;
    pbmi->bmiHeader.biPlanes	= bmp.bmPlanes;
    pbmi->bmiHeader.biBitCount	= bmp.bmBitsPixel;
    
	if (cClrBits < 24) 
        pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 
 
    // If the bitmap is not compressed, set the BI_RGB flag.
    pbmi->bmiHeader.biCompression = BI_RGB;
 
//     Compute the number of bytes in the array of color 
//     indices and store the result in biSizeImage. 
	pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 7)/8 * pbmi->bmiHeader.biHeight * cClrBits; 
 
//  Set biClrImportant to 0, indicating that all of the 
//  device colors are important. 
	pbmi->bmiHeader.biClrImportant = 0;
	::GetDIBColorTable(memDC, 0, 1<<cClrBits, (RGBQUAD*)&(pbmi->bmiColors));

    BITMAPFILEHEADER	hdr;	// bitmap file-header
    PBITMAPINFOHEADER	pbih = &pbmi->bmiHeader;// bitmap info-header
    DWORD				ColorsUsed = pbih->biClrUsed;;
 
    LPBYTE lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

    if (!lpBits)
	{
		memDC.SelectObject(pOldBitmap);
		bitmap.DeleteObject();
		delete pbmi;
		return false;
	}
 
//	Retrieve the color table (RGBQUAD array) and the bits 
//	(array of palette indices) from the DIB. 
    if (!GetDIBits(memDC, bitmap, 0, (WORD)pbih -> biHeight, lpBits, pbmi, DIB_RGB_COLORS)) 
	{
		// Free memory
		GlobalFree((HGLOBAL)lpBits);
		// release dc
		memDC.SelectObject(pOldBitmap);
		bitmap.DeleteObject();
		delete pbmi;
	}
 
 
    hdr.bfType = 0x4d42;// 0x42 = "B" 0x4d = "M" 
    // Compute the size of the entire file.
    hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) + pbih->biSize + ColorsUsed * sizeof(RGBQUAD) + pbih->biSizeImage); 
    hdr.bfReserved1 = 0; 
    hdr.bfReserved2 = 0; 
    // Compute the offset to the array of color indices.
    hdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + pbih->biSize + ColorsUsed * sizeof(RGBQUAD); 
 
	try
	{
		// Create the .BMP file.
		CFile	file(szFile, CFile::modeCreate|CFile::modeWrite );
		// Copy the BITMAPFILEHEADER into the .BMP file.
		file.Write(&hdr, sizeof(BITMAPFILEHEADER));
		  // Copy the BITMAPINFOHEADER and RGBQUAD array into the file.
		file.Write((LPVOID)pbih, sizeof(BITMAPINFOHEADER) + ColorsUsed * sizeof(RGBQUAD));
 
		// Copy the array of color indices into the .BMP file.
		file.Write(lpBits, pbih->biSizeImage);
	}
	catch(CException * e)
	{
		e->Delete();

		// Free memory
		GlobalFree((HGLOBAL)lpBits);
		// release dc
		memDC.SelectObject(pOldBitmap);
		bitmap.DeleteObject();
		delete pbmi;

		return false;
	}
 
	// Free memory
	GlobalFree((HGLOBAL)lpBits);
	// release dc
	memDC.SelectObject(pOldBitmap);
	bitmap.DeleteObject();
	delete pbmi;

	return true;
}

CString GetTempFileName()
{
	CString strTempPath;
	DWORD dwRet = ::GetTempPath(1024, strTempPath.GetBuffer(1024));
	strTempPath.ReleaseBuffer();

	CString strName;
	dwRet = ::GetTempFileName(strTempPath, "Context", 0, strName.GetBuffer(1024));
	strName.ReleaseBuffer();

	return strName;
}


// register to app
void CContextAXCtrl::Register(BOOL bRegister) 
{
	if (bRegister)
		CEventListenerImpl::Register(0);
	else
		CEventListenerImpl::UnRegister(0);
}

// read_only mode
BOOL CContextAXCtrl::GetReadOnly() 
{
	return m_bReadOnlyMode;
}

void CContextAXCtrl::SetReadOnly(BOOL bNewValue) 
{
	if (m_bReadOnlyMode == bNewValue)
		return;

	m_bReadOnlyMode = bNewValue;

	SetModifiedFlag();
}

// view
LPDISPATCH CContextAXCtrl::GetView() 
{
	// TODO: Add your property handler here
	IDispatchPtr sptrD = m_sptrView;
	sptrD.AddRef();
	return sptrD;
}

void CContextAXCtrl::SetView(LPDISPATCH newValue) 
{
	IViewPtr sptrV = newValue;
	AttachToView(sptrV);
	SetModifiedFlag();
}

void CContextAXCtrl::AttachToView(IView * pView)
{
	bool bSameView = ::GetObjectKey(pView) == ::GetObjectKey(m_sptrView);
//	if (!pView)
/*
	if (bSameView)
	{
		if (!m_bAttachedToView)
		{
			m_bAttachedToView = true;
		}
		IUnknownPtr sptrDoc;
		if (pView)
			pView->GetDocument(&sptrDoc);


	}
	else
	{
	}
*/
}


///////////////////////////////////////////////////////////////////////////////
// buttons
LPDISPATCH CContextAXCtrl::GetButtonType() 
{
	return m_btnType.GetIDispatch(true);
}

LPDISPATCH CContextAXCtrl::GetButtonObj() 
{
	return m_btnObj.GetIDispatch(true);
}

LPDISPATCH CContextAXCtrl::GetButtonAbc() 
{
	return m_btnAbc.GetIDispatch(true);
}

LPDISPATCH CContextAXCtrl::GetButtonTime() 
{
	return m_btnTime.GetIDispatch(true);
}

LPDISPATCH CContextAXCtrl::GetButtonNumOn() 
{
	return m_btnNumOn.GetIDispatch(true);
}

LPDISPATCH CContextAXCtrl::GetButtonNumOff() 
{
	return m_btnNumOff.GetIDispatch(true);
}

LPDISPATCH CContextAXCtrl::GetButtonExpand() 
{
	return m_btnExpand.GetIDispatch(true);
}

LPDISPATCH CContextAXCtrl::GetButtonCollapse() 
{
	return m_btnCollapse.GetIDispatch(true);
}


///////////////////////////////////////////////////////////////////////////////

void CContextAXCtrl::OnEnableTitleTipsChanged() 
{
	CContextCtrl * pTree = m_bViewType ? m_ViewType.GetTree() : m_ViewObj.GetTree();
	if (pTree && pTree->GetTitleTips() == m_bEnableTitleTips)
		return;

	if (m_ViewType.GetTree())
		m_ViewType.GetTree()->EnableTitleTips(m_bEnableTitleTips);
	
	if (m_ViewObj.GetTree())
		m_ViewObj.GetTree()->EnableTitleTips(m_bEnableTitleTips);

	SetModifiedFlag();
}

void CContextAXCtrl::OnEditableChanged() 
{
	CContextCtrl * pTree = m_bViewType ? m_ViewType.GetTree() : m_ViewObj.GetTree();
	if (pTree && pTree->IsEditable() == m_bEditable)
		return;

	if (m_ViewType.GetTree())
		m_ViewType.GetTree()->SetEditable(m_bEditable);
	
	if (m_ViewObj.GetTree())
		m_ViewObj.GetTree()->SetEditable(m_bEditable);

	SetModifiedFlag();
}

void CContextAXCtrl::OnBorderChanged() 
{
	CContextCtrl * pTree = m_bViewType ? m_ViewType.GetTree() : m_ViewObj.GetTree();
	if (pTree && pTree->GetBorder() == m_nBorder)
		return;

	if (m_ViewType.GetTree())
		m_ViewType.GetTree()->SetBorder(m_nBorder);
	
	if (m_ViewObj.GetTree())
		m_ViewObj.GetTree()->SetBorder(m_nBorder);

	SetModifiedFlag();
}

void CContextAXCtrl::OnIconSizeChanged() 
{
	CContextCtrl * pTree = m_bViewType ? m_ViewType.GetTree() : m_ViewObj.GetTree();
	if (pTree && pTree->GetIconSize() == m_nIconSize)
		return;

	if (m_ViewType.GetTree())
		m_ViewType.GetTree()->SetIconSize(m_nIconSize);
	
	if (m_ViewObj.GetTree())
		m_ViewObj.GetTree()->SetIconSize(m_nIconSize);

	SetModifiedFlag();
}

void CContextAXCtrl::OnImageOffsetChanged() 
{
	CContextCtrl * pTree = m_bViewType ? m_ViewType.GetTree() : m_ViewObj.GetTree();
	if (pTree && pTree->GetImageOffset() == m_nImageOffset)
		return;
 
	if (m_ViewType.GetTree())
		m_ViewType.GetTree()->SetImageOffset(m_nImageOffset);
	
	if (m_ViewObj.GetTree())
		m_ViewObj.GetTree()->SetImageOffset(m_nImageOffset);

	SetModifiedFlag();
}

void CContextAXCtrl::OnIndentChanged() 
{
	CContextCtrl * pTree = m_bViewType ? m_ViewType.GetTree() : m_ViewObj.GetTree();
	if (pTree && pTree->GetIndent() == m_nIndent)
		return;

	if (m_ViewType.GetTree())
		m_ViewType.GetTree()->SetIndent(m_nIndent);
	
	if (m_ViewObj.GetTree())
		m_ViewObj.GetTree()->SetIndent(m_nIndent);

	SetModifiedFlag();
}

void CContextAXCtrl::OnMarginChanged() 
{
	CContextCtrl * pTree = m_bViewType ? m_ViewType.GetTree() : m_ViewObj.GetTree();
	if (pTree && pTree->GetMargin() == m_nMargin)
		return;

	if (m_ViewType.GetTree())
		m_ViewType.GetTree()->SetMargin(m_nMargin);
	
	if (m_ViewObj.GetTree())
		m_ViewObj.GetTree()->SetMargin(m_nMargin);

	SetModifiedFlag();
}

void CContextAXCtrl::OnNumberOffsetChanged() 
{
	CContextCtrl * pTree = m_bViewType ? m_ViewType.GetTree() : m_ViewObj.GetTree();
	if (pTree && pTree->GetNumberOffset() == m_nNumberOffset)
		return;

	if (m_ViewType.GetTree())
		m_ViewType.GetTree()->SetNumberOffset(m_nNumberOffset);
	
	if (m_ViewObj.GetTree())
		m_ViewObj.GetTree()->SetNumberOffset(m_nNumberOffset);

	SetModifiedFlag();
}

void CContextAXCtrl::OnTrackingChanged() 
{
	CContextCtrl * pTree = m_bViewType ? m_ViewType.GetTree() : m_ViewObj.GetTree();
	if (pTree && pTree->IsTracking() == m_bTracking)
		return;

	if (m_ViewType.GetTree())
		m_ViewType.GetTree()->EnableTracking(m_bTracking);
	
	if (m_ViewObj.GetTree())
		m_ViewObj.GetTree()->EnableTracking(m_bTracking);

	SetModifiedFlag();
}

void CContextAXCtrl::OnShowButtonsChanged() 
{
	if (::IsWindow(GetSafeHwnd()))
	{
		CalcLayout();
		Invalidate();
	}
	SetModifiedFlag();
}

void CContextAXCtrl::OnViewTypeChanged() 
{
	SetViewType(m_bViewType);
	SetModifiedFlag();
}


LPDISPATCH CContextAXCtrl::GetSelectedObject() 
{
	// TODO: Add your property handler here

	return NULL;
}

void CContextAXCtrl::SetSelectedObject(LPDISPATCH newValue) 
{
	// TODO: Add your property handler here

	SetModifiedFlag();
}

void CContextAXCtrl::OnFontChanged() 
{
	CFontHolder & fh = InternalGetFont();
	HFONT hFont = fh.GetFontHandle();
	if (hFont)
	{
		if (::IsWindow(m_ViewType.GetSafeHwnd()))
			m_ViewType.SendMessage(WM_SETFONT, (WPARAM)hFont, MAKELPARAM(m_bViewType == TRUE, 0));

		if (::IsWindow(m_ViewObj.GetSafeHwnd()))
			m_ViewObj.SendMessage(WM_SETFONT, (WPARAM)hFont, MAKELPARAM(m_bViewType != TRUE, 0));
	}

	COleControl::OnFontChanged();
}

void CContextAXCtrl::OnForeColorChanged() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	COleControl::OnForeColorChanged();
}

void CContextAXCtrl::OnBackColorChanged() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	COleControl::OnBackColorChanged();
}

OLE_COLOR CContextAXCtrl::GetTitleTipsColor() 
{
	// TODO: Add your property handler here

	return RGB(0,0,0);
}

void CContextAXCtrl::SetTitleTipsColor(OLE_COLOR nNewValue) 
{
	// TODO: Add your property handler here

	SetModifiedFlag();
}

OLE_COLOR CContextAXCtrl::GetTitleTipsBackColor() 
{
	// TODO: Add your property handler here

	return RGB(0,0,0);
}

void CContextAXCtrl::SetTitleTipsBackColor(OLE_COLOR nNewValue) 
{
	// TODO: Add your property handler here

	SetModifiedFlag();
}

void CContextAXCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	COleControl::OnKillFocus(pNewWnd);
	
	HWND hOwnWnd = m_hWnd;
	if (pNewWnd && ::IsWindow(pNewWnd->m_hWnd) && hOwnWnd)
	{
		HWND hNext = pNewWnd->m_hWnd;
		while (hNext && hOwnWnd != hNext)
			hNext = ::GetParent(hNext);

		if (hNext)
			m_hFocusWnd = pNewWnd->m_hWnd;
	}
}

void CContextAXCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	COleControl::OnSetFocus(pOldWnd);
	
	if (pOldWnd)
	{
		HWND hWnd = pOldWnd->m_hWnd;
		while (m_hWnd && hWnd && hWnd != m_hWnd)
			hWnd = ::GetParent(hWnd);

		if (hWnd)
		{
			if (m_hFocusWnd != pOldWnd->m_hWnd)
				m_hFocusWnd = pOldWnd->m_hWnd;
		}
	}
/*	if (m_pFocusWnd && ::IsWindow(m_pFocusWnd->GetSafeHwnd()) && ::IsWindowEnabled(m_pFocusWnd->GetSafeHwnd()))
	{
		m_pFocusWnd->SetFocus();
		return;
	}

	if (m_bViewType)
		m_ViewType.SetFocus();
	else
		m_ViewObj.SetFocus();
*/
}

BOOL CContextAXCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
	{
		int nKey = (int)pMsg->wParam;
		if (nKey == VK_RETURN)
			return ::SendMessage(pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam);

		return CWnd::IsDialogMessage(pMsg);
	}
	return COleControl::PreTranslateMessage(pMsg);

}

void CContextAXCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    switch (nChar)
    {
    case VK_TAB:    
//		if (!m_pFocusWnd)
//		{
//			m_pFocusWnd = m_bViewType ? &m_ViewType : &m_ViewObj;
//			m_pFocusWnd->SetFocus();
//		}
//		else
		{
			m_hFocusWnd = ::GetNextDlgTabItem(m_hWnd, m_hFocusWnd, IsSHIFTpressed());
			
			if (m_bViewType && m_hFocusWnd == m_ViewObj.GetSafeHwnd())
				m_hFocusWnd = m_ViewType.GetSafeHwnd();
			else if (m_hFocusWnd == m_ViewType.GetSafeHwnd() && !m_bViewType)
				m_hFocusWnd = m_ViewObj.GetSafeHwnd();

			if (m_hFocusWnd)
				::SetFocus(m_hFocusWnd);
			else
				COleControl::OnKeyDown(nChar, nRepCnt, nFlags);
		}
        break;

    case VK_DOWN:   
    case VK_RIGHT:
		{
			m_hFocusWnd = ::GetNextDlgTabItem(m_hWnd, m_hFocusWnd, false);
			
			if (m_bViewType && m_hFocusWnd == m_ViewObj.GetSafeHwnd())
				m_hFocusWnd = m_ViewType.GetSafeHwnd();
			else if (m_hFocusWnd == m_ViewType.GetSafeHwnd() && !m_bViewType)
				m_hFocusWnd = m_ViewObj.GetSafeHwnd();

			if (m_hFocusWnd)
				::SetFocus(m_hFocusWnd);
			else
				COleControl::OnKeyDown(nChar, nRepCnt, nFlags);
		}
        break;
        
    case VK_UP:     
    case VK_LEFT:   
		{
			m_hFocusWnd = ::GetNextDlgTabItem(m_hWnd, m_hFocusWnd, true);
			
			if (m_bViewType && m_hFocusWnd == m_ViewObj.GetSafeHwnd())
				m_hFocusWnd = m_ViewType.GetSafeHwnd();
			else if (m_hFocusWnd == m_ViewType.GetSafeHwnd() && !m_bViewType)
				m_hFocusWnd = m_ViewObj.GetSafeHwnd();

			if (m_hFocusWnd)
				::SetFocus(m_hFocusWnd);
			else
				COleControl::OnKeyDown(nChar, nRepCnt, nFlags);
		}
        break;

    default:
		COleControl::OnKeyDown(nChar, nRepCnt, nFlags);
        return;
    }
}

UINT CContextAXCtrl::OnGetDlgCode() 
{
	return COleControl::OnGetDlgCode() | DLGC_WANTARROWS | DLGC_WANTTAB;
}

void CContextAXCtrl::OnDestroy() 
{
	IUnknownPtr	sptrUnkMainFrame(::_GetOtherComponent(::GetAppUnknown(), IID_IMainWindow), false);
	IWindow2Ptr sptrMainFrame = sptrUnkMainFrame;
	if (sptrMainFrame != 0)
		sptrMainFrame->DetachMsgListener(GetControllingUnknown());

	COleControl::OnDestroy();
}

BOOL CContextAXCtrl::OnListenMessage( MSG * pmsg, LRESULT *plResult )
{
	if (!pmsg)return 0;

	*plResult = CWnd::WalkPreTranslateTree( m_hWnd, pmsg );
	return *plResult ? true:false;
}

// ReadOnly mode
/*
	без активизации и отображения видимых объектов, все объекты разрешены.
	из функциональности - только нумерация и получение пронумерованных объектов, 
	а также фокусированного объекта
	возможна работа в режиме привязки ко вью
	нет контекстного меню

*/

// attaced to View
/*
	привязан к указанному вью, 
	запрещен переход к любому другому вью, при убивании оного - оторваться ото всего.
	возможна работа в режиме ReadOnly
	возможно контекстное меню
*/
