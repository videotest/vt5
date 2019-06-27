// ContextViewer.cpp : implementation file
//

#include "stdafx.h"
#include "Data.h"
#include "NameConsts.h"
#include "ContextViewer.h"
#include "DataTreeCtrl.h"
#include "DataContext.h"
#include "wndbase.h"
#include "TimeTest.h"
#include "Utils.h"

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


/////////////////////////////////////////////////////////////////////////////
// CContextViewer

IMPLEMENT_DYNCREATE(CContextViewer, CWnd)


CContextViewer::CContextViewer()
{
	m_bViewType = GetValueInt( GetAppUnknown(), "\\General", "ContextShowMode", 1 ) == 1;
	m_bShowButtons = true;
	m_nBarHeight = 30;
	m_ButtonSize = CSize(23, 23);
	m_arrTempFiles.RemoveAll();
	m_nButtonStyle = 0;

    // vanek : read from shell.data
	m_nButtonStyle = ::GetValueInt( GetAppUnknown(), "\\General", "ButtonStyle", m_nButtonStyle );
	m_ButtonSize.cx = ::GetValueInt( GetAppUnknown(), "\\General", "SizeButtonCX", m_ButtonSize.cx );
	m_ButtonSize.cy = ::GetValueInt( GetAppUnknown(), "\\General", "SizeButtonCY", m_ButtonSize.cy );
    
	m_hFocusWnd = 0;
	_OleLockApp( this );

	m_dwDockSide = AFX_IDW_DOCKBAR_RIGHT;
	m_size = CSize(120, 100);

	m_sName = "ContextViewer";
	m_sUserName.LoadString( IDS_CONTEXT_TITLE );

	EnableAutomation();

	Register(0);
}

CContextViewer::~CContextViewer()
{
	m_hFocusWnd = 0;
	RemoveTempFiles();
	IUnknown * punkOldDoc = ::GetDocByKey(m_lLastDocKey);
	// unregister for document
	if (punkOldDoc)
	{
		UnRegister(punkOldDoc);
		punkOldDoc->Release();
	}

	_OleUnlockApp( this );
	m_lLastViewKey = INVALID_KEY;
	m_lLastDocKey = INVALID_KEY;
	AddRef();
	UnRegister(0);
}

BOOL CContextViewer::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	if( GetValueInt( GetAppUnknown(), "\\ContextViewer", "SmallIcons", 0 ) )
	{
		//m_ViewType.m_Tree.SetExtSize( 1 );
		m_ViewType.m_Tree.SetIconSize( 16 );
		m_ViewType.m_Tree.SetDefItemHeight( 22 );

		//m_ViewObj.m_Tree.SetExtSize( 1 );
		m_ViewObj.m_Tree.SetIconSize( 16 );
		m_ViewObj.m_Tree.SetDefItemHeight( 22 );
	}

	return CWnd::Create(AfxRegisterWndClass(/*CS_DBLCLKS | */CS_HREDRAW | CS_VREDRAW, 0, (HBRUSH)(COLOR_3DFACE + 1), 0), 
						"ContextViewer", dwStyle | WS_CHILD | WS_VISIBLE| WS_CLIPCHILDREN | WS_CLIPSIBLINGS, rect, pParentWnd, nID, NULL);
}


BEGIN_MESSAGE_MAP(CContextViewer, CWnd)
	//{{AFX_MSG_MAP(CContextViewer)
	ON_WM_CONTEXTMENU()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_GETINTERFACE, OnGetInterface)

//	ON_NOTIFY(NM_DBLCLK, ID_INNER_TREE, OnDblclkTree)
//	ON_NOTIFY(NM_RDBLCLK, ID_INNER_TREE, OnRdblclkTree)
//	ON_NOTIFY(NM_RETURN, ID_INNER_TREE, OnReturnTree)
//	ON_NOTIFY(TVN_BEGINLABELEDIT, ID_INNER_TREE, OnBeginlabeleditTree)
//	ON_NOTIFY(TVN_ENDLABELEDIT, ID_INNER_TREE, OnEndlabeleditTree)
	
   ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
   ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CContextViewer, CWnd)
    //{{AFX_EVENTSINK_MAP(CContextViewer)
	ON_EVENT(CContextViewer, IDC_BUTTON_TYPE,		-600 /* Click */, OnClickButtonType,		VTS_NONE)
	ON_EVENT(CContextViewer, IDC_BUTTON_OBJ,		-600 /* Click */, OnClickButtonObj,			VTS_NONE)
	ON_EVENT(CContextViewer, IDC_BUTTON_NUMBER_OFF, -600 /* Click */, OnClickButtonNumberOn,	VTS_NONE)
	ON_EVENT(CContextViewer, IDC_BUTTON_NUMBER_ON,	-600 /* Click */, OnClickButtonNumberOff,	VTS_NONE)
	ON_EVENT(CContextViewer, IDC_BUTTON_EXPAND,		-600 /* Click */, OnClickButtonExpand,		VTS_NONE)
	ON_EVENT(CContextViewer, IDC_BUTTON_COLLAPSE,	-600 /* Click */, OnClickButtonCollapse,	VTS_NONE)
	ON_EVENT(CContextViewer, IDC_BUTTON_ABC,		-600 /* Click */, OnClickButtonAbc,			VTS_NONE)
	ON_EVENT(CContextViewer, IDC_BUTTON_TIME,		-600 /* Click */, OnClickButtonTime,		VTS_NONE)
	//}}AFX_EVENTSINK_MAP
//	ON_EVENT(CContextViewer, IDC_PUSHBUTTONCTRL1,	-600 /* Click */, OnClickPushbuttonctrl1, VTS_NONE)
END_EVENTSINK_MAP()


void CContextViewer::OnClickButtonType()
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

void CContextViewer::OnClickButtonObj()
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

void CContextViewer::OnClickButtonAbc()
{
	if (!m_btnAbc.IsPressed())
		m_btnAbc.SetPressedState();
	if (m_btnTime.IsPressed())
		m_btnTime.ResetPressedState();

	m_ViewObj.SetSortByTime(false);
	m_ViewType.SetSortByTime(false);
}

void CContextViewer::OnClickButtonTime()
{
	if (!m_btnTime.IsPressed())
		m_btnTime.SetPressedState();
	if (m_btnAbc.IsPressed())
		m_btnAbc.ResetPressedState();

	m_ViewObj.SetSortByTime(true);
	m_ViewType.SetSortByTime(true);
}

void CContextViewer::OnClickButtonNumberOn()
{
	SetNumeric();
}

void CContextViewer::OnClickButtonNumberOff()
{
	RemoveNumeric();
}

void CContextViewer::OnClickButtonExpand()
{
	Expand(true);
}

void CContextViewer::OnClickButtonCollapse()
{
	Expand(false);
}

void CContextViewer::RemoveTempFiles()
{
	for (int i = 0; i < m_arrTempFiles.GetSize(); i++)
		CFile::Remove(m_arrTempFiles[i]);

	m_arrTempFiles.RemoveAll();
}


/////////////////////////////////////////////////////////////////////////////
// CContextViewer message handlers
void CContextViewer::OnFinalRelease()
{
	CWnd::OnFinalRelease();
}


BEGIN_INTERFACE_MAP(CContextViewer, CWnd)
	INTERFACE_PART(CContextViewer, IID_IWindow,			Wnd			)
	INTERFACE_PART(CContextViewer, IID_IDockableWindow, Dock		)
	INTERFACE_PART(CContextViewer, IID_IRootedObject,	Rooted		)
	INTERFACE_PART(CContextViewer, IID_IEventListener,	EvList		)
	INTERFACE_PART(CContextViewer, IID_IMenuInitializer,Viewer		)
	INTERFACE_PART(CContextViewer, IID_INamedObject2,	Name		)
	INTERFACE_PART(CContextViewer, IID_IMsgListener,	MsgList		)
	INTERFACE_PART(CContextViewer, IID_IContextView,	ContextView	)
	INTERFACE_PART(CContextViewer, IID_IHelpInfo,		Help		)
	INTERFACE_PART(CContextViewer, IID_IMenuInitializer2,Viewer		)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CContextViewer, Viewer);
IMPLEMENT_UNKNOWN(CContextViewer, ContextView);


// {3CAE43F2-4B16-11d3-87F8-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CContextViewer, "Data.ContextViewer", 
0x3cae43f2, 0x4b16, 0x11d3, 0x87, 0xf8, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);

/////////////////////////////////////////////////////////////////////////////
// CContextViewer message handlers

int CContextViewer::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect;
	GetClientRect(rect);
	rect.top += m_nBarHeight;

	DWORD dwStyle = WS_CHILD| WS_CLIPCHILDREN | WS_CLIPSIBLINGS|WS_TABSTOP;
	if (!m_ViewType.Create(dwStyle, rect, this, ID_TYPE_TREE))
		return -1;

	if (!m_ViewObj.Create(dwStyle, rect, this, ID_OBJ_TREE))
		return -1;

	CreateButtons();		

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

	EnableToolTips(TRUE);

	return 0;
}

void CContextViewer::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	CalcLayout();
}

void CContextViewer::PostNcDestroy() 
{
	delete this;	
}

INT_PTR CContextViewer::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	INT_PTR r = __super::OnToolHitTest(point,pTI);
//	if (r != -1)
//		pTI->uFlags |= TTF_ALWAYSTIP;
	if (r == 1)
		r = ((UINT)(WORD)::GetDlgCtrlID((HWND)pTI->uId));
	return r;
}

BOOL CContextViewer::OnToolTipNotify(UINT id, NMHDR *pNMHDR,
   LRESULT *pResult)
{
   // need to handle both ANSI and UNICODE versions of the message
   TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
   TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
   CString strTipText;
   UINT_PTR nID = pNMHDR->idFrom;
   if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
      pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
   {
      // idFrom is actually the HWND of the tool
      nID = ::GetDlgCtrlID((HWND)nID);
   }

	int ids = 0;
	if (nID == IDC_BUTTON_TYPE)
		ids = IDS_BUTTON_OBJECTS_BY_TYPE;
	else if (nID == IDC_BUTTON_OBJ)
		ids = IDS_BUTTON_OBJECTS_BY_OBJ;
	else if (nID == IDC_BUTTON_ABC)
		ids = IDS_BUTTON_OBJECTS_ABC;
	else if (nID == IDC_BUTTON_TIME)
		ids = IDS_BUTTON_OBJECTS_NUM;
	else if (nID == IDC_BUTTON_NUMBER_ON)
		ids = IDS_BUTTON_OBJECTS_NUMBERS_OFF;
	else if (nID == IDC_BUTTON_NUMBER_OFF)
		ids = IDS_BUTTON_OBJECTS_NUMBERS_ON;
	if (ids != 0)
		strTipText.LoadString(ids);
	TRACE("Tooltip: %s\n", (LPCTSTR)strTipText);

   if (pNMHDR->code == TTN_NEEDTEXTA)
      lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
   else
      _mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
   *pResult = 0;

   return TRUE;    // message was handled
}

CWnd * CContextViewer::GetWindow()
{
	return this;
}

bool CContextViewer::DoCreate(DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID)
{
	if (!Create(WS_CHILD | WS_VISIBLE | WS_EX_CLIENTEDGE/* | WS_CLIPCHILDREN | WS_CLIPSIBLINGS*/, rc, pparent, nID))
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

void  CContextViewer::SetViewType(bool bShow)
{
	if (m_bViewType == bShow)
		return;

	m_bViewType = bShow;
	SetValue( GetAppUnknown(), "\\General", "ContextShowMode", (long)m_bViewType );

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
		if (CWnd::GetFocus() != &m_ViewType)
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
		if (CWnd::GetFocus() != &m_ViewObj)
			m_ViewObj.SetFocus();
	}
}

void  CContextViewer::SetViewObj(bool bShow)
{
	m_bViewType = !bShow;
	SetValue( GetAppUnknown(), "\\General", "ContextShowMode", (long)m_bViewType );

	if (!::IsWindow(GetSafeHwnd()))
		return;

	m_ViewObj.ShowWindow(SW_HIDE);
	m_ViewType.ShowWindow(SW_HIDE);

	CalcLayout();
	if (m_bViewType)
	{
		m_ViewType.SetRedraw(false, false);
		m_ViewType.Init(m_ViewObj.GetContext(false), m_ViewObj.GetNamedData(false));
		m_ViewType.SetRedraw(true, true);
		m_ViewType.ShowWindow(SW_SHOW);

		m_btnNumOff.SetDisabled(false);
		m_btnNumOn.SetDisabled(false);
	}
	else
	{
		m_ViewObj.SetRedraw(false, false);
		m_ViewObj.Init(m_ViewType.GetContext(false), m_ViewType.GetNamedData(false));
		m_ViewObj.SetRedraw(true, true);
		m_ViewObj.ShowWindow(SW_SHOW);

		m_btnNumOff.SetDisabled(true);
		m_btnNumOn.SetDisabled(true);
	}
}

void CContextViewer::SetNumeric()
{
	if (m_bViewType)
		m_ViewType.SetNumeric();
	else
		m_ViewObj.SetNumeric();
}

void CContextViewer::RemoveNumeric()
{
	if (m_bViewType)
		m_ViewType.RemoveNumeric();
	else
		m_ViewObj.RemoveNumeric();
}

void CContextViewer::Expand(bool bExpand)
{
	CContextCtrl * pTree = m_bViewType ? m_ViewType.GetTree() : m_ViewObj.GetTree();
	ASSERT (pTree);
	if (pTree)
		pTree->ExpandAll(bExpand ? CVE_EXPAND : CVE_COLLAPSE, true);
}

void CContextViewer::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	// check for this control exists
	if (!GetSafeHwnd())
		return;

	if (m_bViewType)
		OnNotifyViewType(pszEvent, punkHit, punkFrom, pdata, cbSize );
	else 
		OnNotifyViewObject(pszEvent, punkHit, punkFrom, pdata, cbSize );
}

BOOL CContextViewer::OnListenMessage( MSG * pmsg, LRESULT *plResult )
{
	if (!pmsg)return 0;

	*plResult = CWnd::WalkPreTranslateTree( m_hWnd, pmsg );

	if( *plResult )
	{
//		*plResult = CWnd::WalkPreTranslateTree( m_hWnd, pmsg );
		return true;
	}
	else 
		return false;
/*
BOOL PASCAL 
{
	ASSERT(hWndStop == NULL || ::IsWindow(hWndStop));
	ASSERT(pMsg != NULL);

	// walk from the target window up to the hWndStop window checking
	//  if any window wants to translate this message

	for (HWND hWnd = pMsg->hwnd; hWnd != NULL; hWnd = ::GetParent(hWnd))
	{
		CWnd * pWnd = CWnd::FromHandlePermanent(hWnd);
		if (pWnd != NULL)
		{
			// target window is a C++ window
			if (pWnd->PreTranslateMessage(pMsg))
				return TRUE; // trapped by target window (eg: accelerators)
		}

		// got to hWndStop window without interest
		if (hWnd == hWndStop)
			break;
	}
	return FALSE;       // no special processing
}
*/
}

void CContextViewer::OnNotifyViewType( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	// lock window for update
	BOOL bPrevRedraw = m_ViewType.SetRedraw(false, false);

	_try(CContextViewer, OnNotifyViewType)
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
//					ASSERT (false);
					UnRegister(m_sptrView);
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
				GuidKey PrevKey = *(GuidKey*)pdata;

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
				//paul
				CContextCtrl* ptree = m_ViewType.GetTree();
				if( ptree )
					ptree->EndEditing();


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

void CContextViewer::OnNotifyViewObject( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	// lock window for update
	BOOL bPrevRedraw = m_ViewObj.SetRedraw(false, false);

	_try(CContextViewer, OnNotifyViewObject)
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
					UnRegister(m_sptrView);
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
				UnRegister(m_sptrView);
				m_sptrView = 0;
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


////////////////////////////////////////////////////////////////////////////////////

HRESULT CContextViewer::XViewer::OnInitPopup(BSTR bstrMenuName, HMENU hMenu, HMENU *phOutMenu)
{
	_try_nested(CContextViewer, Viewer, OnInitPopup)
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

		bool	bObject = true;
		bool	bType = true;

		if (!pSelObjectData)
		{
			bObject = bType = false;
			strObjectName = _T("...");
			strObjectType = _T("...");
		}
		else if (pSelObjectData->GetPointer(false))
		{
			strObjectName = pSelObjectData->GetName(); 
			strObjectType = ::GetTypeAliase( ::GetObjectKind(pSelObjectData->GetPointer(false)) ); 
		} 
		else
		{
			bObject = false;
			strObjectName = _T("..."); 
			strObjectType = ::GetTypeAliase( pSelObjectData->GetName() );
			//strObjectType = pSelObjectData->GetName();
		}


		CString strMenuName;
		BSTR bstrName = 0;

		IUnknown	*punkAM = 0;
		GetAppUnknown()->GetActionManager( &punkAM );
		IActionManagerPtr	ptrActionMan = punkAM;
		punkAM->Release();


		int nCount = (int)pmenu->GetMenuItemCount();
		for (int i = 0; i < nCount; i++)
		{
			UINT uFlag = MF_BYPOSITION | MF_STRING;
			UINT uItem = pmenu->GetMenuItemID(i);
			bool bChange = false;


			if (!SUCCEEDED(sptrCmdMgr->GetActionName(&bstrName, uItem)))
				continue;

			IUnknown	*punkAI = 0;
			ptrActionMan->GetActionInfo( bstrName, &punkAI );
			IActionInfoPtr	ptrAI = punkAI;
			if( !punkAI )continue;
			punkAI->Release();
			

			BSTR	bstrMenuName;
			ptrAI->GetCommandInfo( 0, &bstrMenuName, 0, 0 );

			CString	strMenuNameBase = bstrMenuName;
			::SysFreeString( bstrMenuName );

			int idx = strMenuNameBase.Find( "...", 0 );
			if( idx != -1 )strMenuNameBase = strMenuNameBase.Left( idx );
			strMenuNameBase+=" ";

			CString strActionName = bstrName;
			::SysFreeString(bstrName);

			AFX_MANAGE_STATE(AfxGetStaticModuleState()); 

			// return action name by Resource ID
			if( bType )
			{
				if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_ACTION_NEWOBJECT)))
				{
					bChange = true;
					//strMenuName = strMenuNameBase + strObjectType;
					strMenuName.Format( IDS_CONTEXT_NEWOBJECT, strObjectType );
				}
				else if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_ACTION_LOADOBJECT)))
				{
					bChange = true;
					//strMenuName = strMenuNameBase + strObjectType;
					strMenuName.Format( IDS_CONTEXT_LOADOBJECT, strObjectType );
				}
			}

			if( bObject )
			{
				if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_ACTION_SELECTOBJECT)))
				{
					bChange = true;
					//strMenuName = strMenuNameBase + strObjectName;
					strMenuName.Format( IDS_CONTEXT_SELECTOBJECT, strObjectName );
				}

				else if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_ACTION_DELETEOBJECT)))
				{
					bChange = true;
					//strMenuName = strMenuNameBase + strObjectName;
					strMenuName.Format( IDS_CONTEXT_DELETEOBJECT, strObjectName );
				}

				else if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_ACTION_RENAMEOBJECT)))
				{
					bChange = true;
					//strMenuName = strMenuNameBase + strObjectName;
					strMenuName.Format( IDS_CONTEXT_RENAMEOBJECT, strObjectName );
				}

				else if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_ACTION_OBJECTPROPS)))
				{
					bChange = true;
					//strMenuName = strMenuNameBase + strObjectName;
					strMenuName.Format( IDS_CONTEXT_OBJECTPROPS, strObjectName );
				}

				else if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_ACTION_SAVEOBJECT)))
				{
					bChange = true;
					//strMenuName = strMenuNameBase + strObjectName;
					strMenuName.Format( IDS_CONTEXT_SAVEOBJECT, strObjectName );
				}

				else if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_ACTION_EDITCOPY)))
				{
					bChange = true;
					//strMenuName = strMenuNameBase + strObjectName;
					strMenuName.Format( IDS_CONTEXT_EDITCOPY, strObjectName );
				}
			}


			if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_SEND_TO)) && strObjectName != _T("..."))
			{
				bChange = true;
				/*
				CString strPrefix, strPostFix;
				strPrefix.LoadString( IDS_SEND_TO_PREFIX );
				strPostFix.LoadString( IDS_SEND_TO_POSTFIX );								
				strMenuName = strPrefix + " " + strObjectName + " " + strPostFix;
				*/
				strMenuName.Format( IDS_SEND_OBJECT_TO, strObjectName );
			}

			if (bChange)
				pmenu->ModifyMenu(i, uFlag, uItem, strMenuName);

		}

		*phOutMenu = pmenu->Detach();

		return S_OK;
	}
	_catch_nested;
}

HRESULT CContextViewer::XViewer::UpdateActionState( BSTR bstrAction, UINT id, DWORD *pdwFlags )
{
	_try_nested(CContextViewer, Viewer, UpdateActionState)
	{
		_bstr_t bstrActionName(bstrAction);
		_bstr_t bstrEditCopy(GetActionNameByResID(IDS_ACTION_EDITCOPY));
		_bstr_t bstrNewObject(GetActionNameByResID(IDS_ACTION_NEWOBJECT));
		if (bstrActionName == bstrEditCopy || bstrActionName == bstrNewObject)
		{
			CContextCtrl * pTree = pThis->m_bViewType ? pThis->m_ViewType.GetTree() : pThis->m_ViewObj.GetTree();
			ASSERT (pTree);
			HCONTEXTITEM hItem = pTree->GetItemFocus();
			// get selected object data
			CContextItemData* pSelObjectData = (hItem) ? (CContextItemData*)pTree->GetItemData(hItem) : NULL;
			if (pSelObjectData)
			{
				IUnknown *punkObj = pSelObjectData->GetPointer(false);
			CString strObjectType;
				// A.M. fix BT5037.
				// EditCopy has parameter only if pSelObjectData->GetPointer(false) not null.
				// NewObject uses pSelObjectData->GetName() as type name in this case.
				// see CContextViewer::XViewer::OnContextMenu.
				if (bstrActionName == bstrEditCopy)
					strObjectType = punkObj==NULL?_T(""):GetObjectKind(punkObj);
			else
					strObjectType = punkObj==NULL?pSelObjectData->GetName():GetObjectKind(punkObj);
				DWORD dwObjectFlags = 0;
				if (bstrActionName == bstrEditCopy) // A.M. fix SBT1672
				{
					INamedDataObject2Ptr ptrNDO2(punkObj);
					if (ptrNDO2 != 0)
						ptrNDO2->GetObjectFlags(&dwObjectFlags);
				}
				bool bEnableInSD = true;
			if (!strObjectType.IsEmpty())
					bEnableInSD = GetValueInt(::GetAppUnknown(), "\\ContextViewer\\EnableCopy", strObjectType, 1) != 0;
				if ((dwObjectFlags&nofCantSendTo) == 0 && bEnableInSD)
					*pdwFlags = afEnabled;
				else
					*pdwFlags = 0;
			}
		}
		return S_OK;
	}
	_catch_nested;
}



HRESULT CContextViewer::XContextView::GetView(IUnknown ** ppunkView)
{
	METHOD_PROLOGUE_EX(CContextViewer, ContextView)
	{
		if (!ppunkView)
			return E_INVALIDARG;

		*ppunkView = pThis->m_sptrView;
		if (*ppunkView)
			(*ppunkView)->AddRef();
		return S_OK;
	}
}

HRESULT CContextViewer::XContextView::SetView(IUnknown * punkView)
{
	METHOD_PROLOGUE_EX(CContextViewer, ContextView)
	{
		return E_NOTIMPL;
	}
}

HRESULT CContextViewer::XContextView::SetReadOnlyMode(BOOL bMode)
{
	METHOD_PROLOGUE_EX(CContextViewer, ContextView)
	{
		return E_NOTIMPL;
	}
}

HRESULT CContextViewer::XContextView::GetReadOnlyMode(BOOL * pbMode)
{
	METHOD_PROLOGUE_EX(CContextViewer, ContextView)
	{
		return E_NOTIMPL;
	}
}


///////////////////////////////////////////////////////////////////////////////
// User events handlers


void CContextViewer::OnContextMenu(CWnd* pWnd, CPoint point) 
{

	CRect rc;
	CContextCtrl * pTree = m_bViewType ? m_ViewType.GetTree() : m_ViewObj.GetTree();
	ASSERT (pTree);

	CPoint ptTree(point);
	pTree->ScreenToClient(&ptTree);
	UINT uFlag = 0;
	HCONTEXTITEM hItem = pTree->HitTest(ptTree, &uFlag);
	if (!hItem) return; // BT5096
//	HCONTEXTITEM hItem = pTree->GetItemFocus();

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
	if (pSelObjectData == NULL) //BT 5096
		return;

	CString strObjectName;
	CString strObjectType;

	if (pSelObjectData)
	{	
		IUnknown * punkObj = pSelObjectData->GetPointer(false);
		if (punkObj == NULL) //BT 5096
			return;

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


	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
// return action name by Resource ID

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
	else if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_ACTION_EDITCOPY)))
	{
		strArgs = CString("\"") + strObjectName + CString("\"");
		dwFlag = aefShowInterfaceByRequired;
	}

	else
		bExec = false;

	//if (bExec)
		::ExecuteAction(strActionName, strArgs, dwFlag);
}

BOOL CContextViewer::OnEraseBkgnd(CDC* pDC) 
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
}

bool CContextViewer::CalcLayout()
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
		/*if (::IsWindow(m_btnExpand.GetSafeHwnd()))
		{
			m_btnExpand.MoveWindow(ButtonRect);
			if (!::IsWindowVisible(m_btnExpand.GetSafeHwnd()))
				m_btnExpand.ShowWindow(SW_SHOW);
		}*/

		ButtonRect.OffsetRect(m_ButtonSize.cx + 1, 0);
		/*if (::IsWindow(m_btnCollapse.GetSafeHwnd()))
		{
			m_btnCollapse.MoveWindow(ButtonRect);
			if (!::IsWindowVisible(m_btnCollapse.GetSafeHwnd()))
				m_btnCollapse.ShowWindow(SW_SHOW);
		} */

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

/*		if (::IsWindow(m_btnExpand.GetSafeHwnd()) && ::IsWindowVisible(m_btnExpand.GetSafeHwnd()))
			m_btnExpand.ShowWindow(SW_HIDE);

		if (::IsWindow(m_btnCollapse.GetSafeHwnd()) && ::IsWindowVisible(m_btnCollapse.GetSafeHwnd()))
			m_btnCollapse.ShowWindow(SW_HIDE);*/

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

void CContextViewer::ShowButtons(BOOL bShow)
{
	m_bShowButtons = bShow == TRUE;
	if (::IsWindow(GetSafeHwnd()))
	{
		CalcLayout();
		Invalidate();
	}
}

BOOL CContextViewer::IsShowButtons()
{
	return m_bShowButtons;
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

/*
CString GetTempFileName()
{
	char sz_temp_path[MAX_PATH];	sz_temp_path[0] = 0;
	::GetTempPath( sizeof(sz_temp_path), sz_temp_path );	

	char sz_long[256]; sz_long[0] = 0;
	strcpy( sz_long, sz_temp_path );
	GetLongPathName( sz_temp_path, sz_long, sizeof(sz_long) );
	
	char sz_name[MAX_PATH];	sz_name[0] = 0;
	::GetTempFileName( sz_long, "con", 0, sz_name );

	return sz_name;
}
*/


bool CContextViewer::CreateButtons()
{
	int dy = (m_nBarHeight - m_ButtonSize.cy) / 2;

	CRect ButtonRect(0, 0, m_ButtonSize.cx, m_ButtonSize.cy);
	ButtonRect.OffsetRect(dy, dy);

		
	DWORD dwButtonStyle = WS_CHILD|WS_CLIPSIBLINGS;

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
	if (!m_btnType.Create(0, dwButtonStyle|BS_PUSHBUTTON|WS_TABSTOP /*BS_RADIOBUTTON*/, ButtonRect, this, IDC_BUTTON_TYPE))
		return false;
	// and hide button
	m_btnType.ShowWindow(SW_HIDE);

	// set file name to button
	m_btnType.SetBMPFileName(strName);
	m_btnType.SetButtonHeight((short)m_ButtonSize.cy);
	m_btnType.SetButtonWidth((short)m_ButtonSize.cx);
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
	m_btnType.SetColorTransparent( GetPushBtnTransparentColor() );
	::SetWindowLong(m_btnType.m_hWnd, GWL_ID, IDC_BUTTON_TYPE);


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
	if (!m_btnObj.Create(0, dwButtonStyle|BS_PUSHBUTTON|WS_TABSTOP /*BS_RADIOBUTTON*/, ButtonRect, this, IDC_BUTTON_OBJ))
		return false;
	// and hide button
	m_btnObj.ShowWindow(SW_HIDE);

	// set file name to button
	m_btnObj.SetBMPFileName(strName);
	m_btnObj.SetButtonHeight((short)m_ButtonSize.cy);
	m_btnObj.SetButtonWidth((short)m_ButtonSize.cx);
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
	m_btnObj.SetColorTransparent( GetPushBtnTransparentColor() );
	::SetWindowLong(m_btnObj.m_hWnd, GWL_ID, IDC_BUTTON_OBJ);

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
	if (!m_btnAbc.Create(0, dwButtonStyle|BS_PUSHBUTTON|WS_TABSTOP /*BS_RADIOBUTTON*/, ButtonRect, this, IDC_BUTTON_ABC))
		return false;
	// and hide button
	m_btnAbc.ShowWindow(SW_HIDE);

	// set file name to button
	m_btnAbc.SetBMPFileName(strName);
	m_btnAbc.SetButtonHeight((short)m_ButtonSize.cy);
	m_btnAbc.SetButtonWidth((short)m_ButtonSize.cx);
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
	m_btnAbc.SetColorTransparent( GetPushBtnTransparentColor() );
	::SetWindowLong(m_btnAbc.m_hWnd, GWL_ID, IDC_BUTTON_ABC);

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
	if (!m_btnTime.Create(0, dwButtonStyle|BS_PUSHBUTTON|WS_TABSTOP /*BS_RADIOBUTTON*/, ButtonRect, this, IDC_BUTTON_TIME))
		return false;
	// and hide button
	m_btnTime.ShowWindow(SW_HIDE);

	// set file name to button
	m_btnTime.SetBMPFileName(strName);
	m_btnTime.SetButtonHeight((short)m_ButtonSize.cy);
	m_btnTime.SetButtonWidth((short)m_ButtonSize.cx);
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
	m_btnTime.SetColorTransparent( GetPushBtnTransparentColor() );
	::SetWindowLong(m_btnTime.m_hWnd, GWL_ID, IDC_BUTTON_TIME);

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
	if (!m_btnNumOn.Create(0, dwButtonStyle|BS_PUSHBUTTON|WS_TABSTOP, ButtonRect, this, IDC_BUTTON_NUMBER_OFF))
		return false;
	// and hide button
	m_btnNumOn.ShowWindow(SW_HIDE);

	// set file name to button
	m_btnNumOn.SetBMPFileName(strName);
	m_btnNumOn.SetButtonHeight((short)m_ButtonSize.cy);
	m_btnNumOn.SetButtonWidth((short)m_ButtonSize.cx);
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
	m_btnNumOn.SetColorTransparent( GetPushBtnTransparentColor() );
	::SetWindowLong(m_btnNumOn.m_hWnd, GWL_ID, IDC_BUTTON_NUMBER_OFF);

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
	if (!m_btnNumOff.Create(0, dwButtonStyle|BS_PUSHBUTTON|WS_TABSTOP, ButtonRect, this, IDC_BUTTON_NUMBER_ON))
		return false;
	// and hide button
	m_btnNumOff.ShowWindow(SW_HIDE);
	// set file name to button
	m_btnNumOff.SetBMPFileName(strName);
	m_btnNumOff.SetButtonHeight((short)m_ButtonSize.cy);
	m_btnNumOff.SetButtonWidth((short)m_ButtonSize.cx);
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
	m_btnNumOff.SetColorTransparent( GetPushBtnTransparentColor() );
	::SetWindowLong(m_btnNumOff.m_hWnd, GWL_ID, IDC_BUTTON_NUMBER_ON);

// button_Expand
	// get new temp file name
	strName = GetTempFileName();
	if (strName.IsEmpty())
		return false;
	// save bitmap from resource to temp file
	if (!::SaveBMPFile(IDB_BUTTON_EXPAND_SM, strName))
		return false;
	m_arrTempFiles.Add(strName);
	
/*	// create button
	ButtonRect.OffsetRect(m_ButtonSize.cx + 1, 0);
	if (!m_btnExpand.Create(0, dwButtonStyle|BS_PUSHBUTTON|WS_TABSTOP, ButtonRect, this, IDC_BUTTON_EXPAND))
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
	m_btnExpand.SetColorTransparent( GetPushBtnTransparentColor() );

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
	if (!m_btnCollapse.Create(0, dwButtonStyle|BS_PUSHBUTTON|WS_TABSTOP, ButtonRect, this, IDC_BUTTON_COLLAPSE))
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
	m_btnCollapse.SetColorTransparent( GetPushBtnTransparentColor() );*/

	// vanek : set buttons style
	/*m_btnType.SetFlatButton( FALSE );
	m_btnObj.SetFlatButton( FALSE );
	m_btnAbc.SetFlatButton( FALSE );
	m_btnTime.SetFlatButton( FALSE );
	m_btnNumOn.SetFlatButton( FALSE );
	m_btnNumOff.SetFlatButton( FALSE );

	m_btnType.SetSemiFlat( FALSE );
	m_btnObj.SetSemiFlat( FALSE );
	m_btnAbc.SetSemiFlat( FALSE );
	m_btnTime.SetSemiFlat( FALSE );
	m_btnNumOn.SetSemiFlat( FALSE );
	m_btnNumOff.SetSemiFlat( FALSE );*/

	// SemiFlat 
    m_btnType.SetSemiFlat( TRUE );
	m_btnObj.SetSemiFlat( TRUE );
	m_btnAbc.SetSemiFlat( TRUE );
	m_btnTime.SetSemiFlat( TRUE );
	m_btnNumOn.SetSemiFlat( TRUE );
	m_btnNumOff.SetSemiFlat( TRUE );        

	if( m_nButtonStyle == 0 )
	{	// Flat
		m_btnType.SetFlatButton( TRUE );
		m_btnObj.SetFlatButton( TRUE );
		m_btnAbc.SetFlatButton( TRUE );
		m_btnTime.SetFlatButton( TRUE );
		m_btnNumOn.SetFlatButton( TRUE );
		m_btnNumOff.SetFlatButton( TRUE );

		m_btnType.SetDrawBorder( TRUE );
		m_btnObj.SetDrawBorder( TRUE );
		m_btnAbc.SetDrawBorder( TRUE );
		m_btnTime.SetDrawBorder( TRUE );
		m_btnNumOn.SetDrawBorder( TRUE );
		m_btnNumOff.SetDrawBorder( TRUE );
	}
	else
	{	
		m_btnType.SetFlatButton( FALSE );
		m_btnObj.SetFlatButton( FALSE );
		m_btnAbc.SetFlatButton( FALSE );
		m_btnTime.SetFlatButton( FALSE );
		m_btnNumOn.SetFlatButton( FALSE );
		m_btnNumOff.SetFlatButton( FALSE );

		m_btnType.SetDrawBorder( FALSE );
		m_btnObj.SetDrawBorder( FALSE );
		m_btnAbc.SetDrawBorder( FALSE );
		m_btnTime.SetDrawBorder( FALSE );
		m_btnNumOn.SetDrawBorder( FALSE );
		m_btnNumOff.SetDrawBorder( FALSE );
	}
	    
	return true;
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
		pbmi = (PBITMAPINFO)new BYTE [sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (UINT)(1<<cClrBits)];
		memset(pbmi, 0, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (UINT)(1<<cClrBits));
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
 
    LPBYTE lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage+pbih->biSize);

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
		CFile	file(szFile, CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone );
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

void CContextViewer::OnDestroy() 
{
	IUnknownPtr	sptrUnkMainFrame(::_GetOtherComponent(::GetAppUnknown(), IID_IMainWindow), false);
	IWindow2Ptr sptrMainFrame = sptrUnkMainFrame;
	if (sptrMainFrame != 0)
		sptrMainFrame->DetachMsgListener(GetControllingUnknown());

	CWnd::OnDestroy();
}

UINT CContextViewer::OnGetDlgCode() 
{
    UINT nCode = DLGC_WANTARROWS/*|DLGC_BUTTON|DLGC_DEFPUSHBUTTON*/|DLGC_WANTTAB/*|DLGC_WANTALLKEYS*/;
    return /*CWnd::OnGetDlgCode();*/nCode;
}

void CContextViewer::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    switch (nChar)
    {
/*    case VK_TAB:    
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
				CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
		}
        break;*/

    /*case VK_DOWN:   
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
				CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
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
				CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
		}
        break;*/
	case VK_ESCAPE://return focus to the active view
		ReturnFocus();
		return;

    default:
        CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
        return;
    }
}

void CContextViewer::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);

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

void CContextViewer::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);

/*	if (pOldWnd)
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
	if (m_pFocusWnd && ::IsWindow(m_pFocusWnd->GetSafeHwnd()) && ::IsWindowEnabled(m_pFocusWnd->GetSafeHwnd()))
	{
		m_pFocusWnd->SetFocus();
		return;
	}
*/
	if (m_bViewType)
		m_ViewType.SetFocus();
	else
		m_ViewObj.SetFocus();

}

void CContextViewer::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd::OnChar(nChar, nRepCnt, nFlags);
}

BOOL CContextViewer::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (/*pMsg->message == WM_KEYUP || */pMsg->message == WM_KEYDOWN)// || pMsg->message == WM_SYSKEYDOWN)
	{
		int nKey = (int)pMsg->wParam;
		if (nKey == VK_RETURN)
		{
			::SendMessage(pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam);// 
			ReturnFocus();
			return TRUE;
		}
		if (nKey == VK_ESCAPE)
		{
			ReturnFocus();
			return TRUE;
		}


		return CWnd::IsDialogMessage(pMsg);//CWnd::PreTranslateMessage(pMsg);
	}
	return CWnd::PreTranslateMessage(pMsg);
	
}

void CContextViewer::ReturnFocus()
{
	IApplicationPtr	ptrApp( GetAppUnknown() );
	IUnknown	*punkDoc = 0;
	ptrApp->GetActiveDocument( &punkDoc );

	if( !punkDoc )return;
	IDocumentSitePtr	ptrDoc( punkDoc );
	punkDoc->Release();

	IUnknown	*punkV = 0;
	ptrDoc->GetActiveView( &punkV );
	if( !punkV )return;
	IWindowPtr	ptrWin( punkV );
	punkV->Release();

	HWND	hwnd;
	ptrWin->GetHandle( (HANDLE*)&hwnd );
	::SetFocus( hwnd );
}