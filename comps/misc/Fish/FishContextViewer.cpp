#include "stdafx.h"
#include "fishcontextviewer.h"
#include "resource.h"
#include "misc_utils.h"
#include "PropBag.h"
#include "ScriptNotifyInt.h"
#include "color.h"
#include "Settings.h"

const int nScrollW = 15;

IMPLEMENT_DYNCREATE(CFishContextViewer, CWnd)

CFishContextViewer::CFishContextViewer() : m_ResultPart(this,-1), m_Combi(this,-2),
	m_brBack((COLORREF)g_BackgroundColor)
{
	m_dwUsedPanes = 0xFFFF;
	_OleLockApp( this );

	m_dwDockSide = AFX_IDW_DOCKBAR_BOTTOM;
	m_size = CSize(250, 100);

	m_sName = "FishContextViewer";
	m_sUserName.LoadString( IDS_FISH_CONTEXT_TITLE );

	EnableAutomation();

	Register(0);
	m_lLastViewKey = INVALID_KEY;
	m_nItemCount = 0;
	m_nActiveItem = -1;
	m_nGapBetweenControls = ::GetValueInt(::GetAppUnknown(), FISH_ROOT, "GapBetweenControls", 1);
	m_nLeftOffset = ::GetValueInt(::GetAppUnknown(), FISH_ROOT, "LeftOffset", 1);
	m_nTopOffset = ::GetValueInt(::GetAppUnknown(), FISH_ROOT, "TopOffset", 1);
	m_nHeight = 0;
	m_phrzScoll = NULL;
	m_pvrtScoll = NULL;
}

CFishContextViewer::~CFishContextViewer()
{
	UnRegister(0);
	
	_OleUnlockApp( this );

	for( IT it = m_arrLayers.begin(); it != m_arrLayers.end(); ++it )
		delete (*it);
	m_arrLayers.clear();

}

BEGIN_MESSAGE_MAP(CFishContextViewer, CWnd)
	//{{AFX_MSG_MAP(CFishContextViewer)
    ON_WM_VSCROLL()
    ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_GETINTERFACE, OnGetInterface)
END_MESSAGE_MAP()

BEGIN_INTERFACE_MAP(CFishContextViewer, CWnd)
	INTERFACE_PART(CFishContextViewer, IID_IWindow,			Wnd			)
	INTERFACE_PART(CFishContextViewer, IID_IDockableWindow, Dock		)
	INTERFACE_PART(CFishContextViewer, IID_IRootedObject,	Rooted		)
	INTERFACE_PART(CFishContextViewer, IID_IEventListener,	EvList		)
	INTERFACE_PART(CFishContextViewer, IID_INamedObject2,	Name		)
	INTERFACE_PART(CFishContextViewer, IID_IMsgListener,	MsgList		)
	INTERFACE_PART(CFishContextViewer, IID_IHelpInfo,		Help		)
END_INTERFACE_MAP()

// {A3511C1B-2AD8-401a-9F74-A4405F863BFE}
GUARD_IMPLEMENT_OLECREATE( CFishContextViewer, "FISH.FishContextViewer", 
0xa3511c1b, 0x2ad8, 0x401a, 0x9f, 0x74, 0xa4, 0x40, 0x5f, 0x86, 0x3b, 0xfe );

BEGIN_DISPATCH_MAP(CFishContextViewer, CWnd)
	//{{AFX_DISPATCH_MAP(CFishContextViewer)
	DISP_FUNCTION(CFishContextViewer, "RefreshWindow", RefreshWindow, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CFishContextViewer, "GetFiltersCount", GetFiltersCount, VT_I4, VTS_NONE)
	DISP_FUNCTION(CFishContextViewer, "GetFilterName", GetFilterName, VT_BSTR, VTS_I4)
	DISP_FUNCTION(CFishContextViewer, "GetFilterShortName", GetFilterShortName, VT_BSTR, VTS_I4)
	DISP_FUNCTION(CFishContextViewer, "GetFilterState", GetFilterState, VT_I4, VTS_I4)
	DISP_FUNCTION(CFishContextViewer, "GetFilterColor", GetFilterColor, VT_I4, VTS_I4)
	DISP_FUNCTION(CFishContextViewer, "SetFilterName", SetFilterName, VT_EMPTY, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CFishContextViewer, "SetFilterShortName", SetFilterShortName, VT_EMPTY, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CFishContextViewer, "SetFilterState", SetFilterState, VT_EMPTY, VTS_I4 VTS_I4)
	DISP_FUNCTION(CFishContextViewer, "SetFilterColor", SetFilterColor, VT_EMPTY, VTS_I4 VTS_I4)
	DISP_FUNCTION(CFishContextViewer, "GetActiveItem", GetActiveItem, VT_I4, VTS_NONE)
	DISP_FUNCTION(CFishContextViewer, "SetActiveItem", SetActiveItem, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CFishContextViewer, "EnableItem", EnableItem, VT_EMPTY, VTS_I4 VTS_I4)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


CWnd * CFishContextViewer::GetWindow()
{
	return this;
}

IUnknown* GetActualImageFromView(IUnknown *punkView)
{	// получить актуальный image со вьюхи - либо активный,
	// либо, если нет активного, но есть активный ObjectList -
	// то его базовый Image (совпадающий по BaseKey)
	// возвращаем с AddRef
	IUnknownPtr ptrunkObject(::GetActiveObjectFromContext(punkView, szTypeImage), false);
	IImage3Ptr sptrI = ptrunkObject;

	if(sptrI==0)
	{
		IUnknownPtr ptrunkList(::GetActiveObjectFromContext(punkView, szTypeObjectList), false);
		INamedDataObject2Ptr sptrNDO(ptrunkList);
		// попытаемся найти image c тем же base_key (т.е. из той же группы)
		if(sptrNDO!=0)
		{
			GUID base_key = {0};
			sptrNDO->GetBaseKey(&base_key);
			IViewPtr sptrView(punkView);
			IUnknownPtr ptrDoc;
			if(sptrView!=0) sptrView->GetDocument( &ptrDoc );
			IDataContext2Ptr sptrC(ptrDoc);
			if(sptrC!=0)
			{
				LONG_PTR lPos=0;
				sptrC->GetFirstObjectPos(_bstr_t(szTypeImage), &lPos);
				while(lPos)
				{
					IUnknownPtr ptrI2;
					sptrC->GetNextObject(_bstr_t(szTypeImage), &lPos, &ptrI2);
					INamedDataObject2Ptr sptrNDO2(ptrI2);
					if(sptrNDO2!=0)
					{
						GUID base_key2 = {0};
						sptrNDO->GetBaseKey(&base_key2);
						if(base_key2==base_key)
						{
							sptrI = ptrI2;
							break;
						}
					}
				}
			}						
		}
	}
	if(sptrI!=0)
	{
		IUnknownPtr ptrParent = 0;
		INamedDataObject2Ptr ptrO( sptrI );
		ptrO->GetParent( &ptrParent );
		if(ptrParent!=0)
		{
			sptrI = ptrParent;
		}
	}

	if(sptrI!=0) sptrI.AddRef();
	return sptrI;
}

void CFishContextViewer::RefreshWindow()
{
	IUnknownPtr punkObject(GetActualImageFromView( m_sptrView ), false);
	if (punkObject)
	{
		m_nItemCount = ::GetImagePaneCount( punkObject ) - 1;
		m_nItemCount = max(0, m_nItemCount);
		if(m_nItemCount)
		{
			m_ResultPart.m_Data.RedrawWindow();
			m_Combi.m_Data.RedrawWindow();
			if (m_nActiveItem > m_nItemCount)
				_SetActiveItem(-1, true, true);
			if( GetFromNamedData( punkObject, _bstr_t( FISH_ROOT ), _bstr_t( FISH_USED_PANES ) ).vt == VT_EMPTY )
				_reload_to_ndata( ::GetAppUnknown(), punkObject );
			else
				_apply_own_ndata( punkObject );
			int dy = re_create_context( punkObject );
			_reset_scroller( m_nHeight = dy );
		}
		else _create_empty_bar();
	}
	else
		_create_empty_bar();
}

void CFishContextViewer::_RefreshWindow()
{
	m_ResultPart.m_Data.RedrawWindow();
	m_Combi.m_Data.RedrawWindow();
	for( int i = 0; i < m_nItemCount; i++ )
		m_arrLayers[i]->m_Data.RedrawWindow();
}

bool CFishContextViewer::DoCreate(DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID)
{
	if (!Create(WS_CHILD | WS_VISIBLE /* | WS_CLIPCHILDREN | WS_CLIPSIBLINGS*/, rc, pparent, nID))
		return false;

	// and addition style
	ModifyStyle(WS_BORDER|WS_VSCROLL, 0);

	// set client edge
	SetWindowText("Fish Context Viewer");

	m_ResultPart.Create(CResultPartDlg::IDD, this);
	m_ResultPart.ModifyStyle(0, WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	m_ResultPart.SetWindowPos(0, m_nLeftOffset, m_nTopOffset, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
	CRect rcRes;
	m_ResultPart.GetWindowRect(rcRes);
	m_size = rcRes.Size();
	ScreenToClient(rcRes);
	m_Combi.Create(CResultPartDlg::IDD, this);
	m_Combi.ModifyStyle( 0, WS_CLIPCHILDREN | WS_CLIPSIBLINGS );
	CString	strScrollClass = GetValueString( GetAppUnknown(), "\\Interface", "ScrollClass", "SCROLLBAR" );
	m_phrzScoll = new CScrollBar;
	m_pvrtScoll = new CScrollBar;
	m_phrzScoll->CWnd::Create( strScrollClass, 0, WS_CHILD|WS_VISIBLE|SBS_HORZ, CRect( 0, 0, 100, 100 ), this, 100 );
	m_pvrtScoll->CWnd::Create( strScrollClass, 0, WS_CHILD|WS_VISIBLE|SBS_VERT, CRect( 0, 0, 100, 100 ), this, 104 );
	m_StaticRightBottom.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,100,100), this);
	bool bHideCombi = ::GetValueInt(::GetAppUnknown(), FISH_ROOT, "HideCombi", 0) != 0;
	if (m_dwDockSide == AFX_IDW_DOCKBAR_BOTTOM)
	{
		m_Combi.SetWindowPos(0, bHideCombi ? rcRes.left : rcRes.right+m_nGapBetweenControls, rcRes.top, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
		m_size = CSize(2*rcRes.Width()+m_nGapBetweenControls, rcRes.Height());
//		EnableScrollBarCtrl(SB_HORZ);
	}
	else
	{
		m_Combi.SetWindowPos(0, rcRes.left, bHideCombi ? rcRes.top : rcRes.bottom+m_nGapBetweenControls, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
		m_size = CSize(rcRes.Width(), 2*rcRes.Height()+m_nGapBetweenControls);
//		EnableScrollBarCtrl(SB_VERT);
	}
	_create_empty_bar();
	return true;
}

BOOL CFishContextViewer::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	return CWnd::Create(AfxRegisterWndClass(/*CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW*/0, 0, (HBRUSH)(COLOR_3DFACE + 1), 0), 
						"FishContextViewer", dwStyle | WS_CHILD | WS_VISIBLE| WS_CLIPCHILDREN | WS_CLIPSIBLINGS , rect, pParentWnd, nID, NULL);
}


void CFishContextViewer::OnChangeViewOrObject()
{
	IApplicationPtr sptrApp(GetAppUnknown());
	IUnknownPtr punkActiveDoc;
	sptrApp->GetActiveDocument(&punkActiveDoc);
	IDocumentSitePtr sptrDSActiveDoc(punkActiveDoc);
	IUnknownPtr punkActiveView;
	if (sptrDSActiveDoc != 0)
		sptrDSActiveDoc->GetActiveView(&punkActiveView);
	bool bSuccess = false;
	IUnknownPtr punkObject(GetActualImageFromView(punkActiveView), false);
	IImage3Ptr sptrI = punkObject;
	m_ptrLastImage = punkObject;
	if (sptrI != 0)
	{
		IUnknownPtr punkCC;
		sptrI->GetColorConvertor(&punkCC);
		if (punkCC != 0)
		{
			IColorConvertorFISHPtr sptrFISH = punkCC;
			if( sptrFISH != 0 )
				bSuccess = true;
		}
	}
	if (bSuccess)
	{
		if (m_sptrView == 0)
		{
			m_sptrView = punkActiveView;
			Register(m_sptrView);
			m_lLastViewKey = GetObjectKey(punkActiveView);
		}
		if (GetObjectKey(m_ptrLastImage) != GetObjectKey(punkObject))
			m_ptrLastImage = punkObject;
		CFishProps cf(m_ptrLastImage, m_sptrView);
		cf.GetLongProp(&m_nActiveItem, FISH_SHOWN_PANE);
		cf.GetLongProp(&m_dwUsedPanes, FISH_USED_PANES);
		m_nItemCount = ::GetImagePaneCount( punkObject ) - 1;
		m_nItemCount = max(0, m_nItemCount);
		if(m_nItemCount)
		{
			m_ResultPart.SetNData( punkObject );
			m_Combi.SetNData( punkObject );
			if (m_nActiveItem > m_nItemCount)
				_SetActiveItem(-1, true, true);
			if( GetFromNamedData( punkObject, _bstr_t( FISH_ROOT ), _bstr_t( FISH_USED_PANES ) ).vt == VT_EMPTY )
				_reload_to_ndata( ::GetAppUnknown(), punkObject );
			else
				_apply_own_ndata( punkObject );
			int dy = re_create_context( punkObject );
			_reset_scroller( m_nHeight = dy );
		}
		else _create_empty_bar();
	}
	else
	{
		m_ptrLastImage = 0;
		if (m_sptrView != 0)
		{
			UnRegister(m_sptrView);
			m_sptrView = 0;
			m_lLastViewKey = INVALID_KEY;
		}
		_create_empty_bar();
	}
}


void CFishContextViewer::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	// check for this control exists
	if (!GetSafeHwnd())
		return;

	_try(CFishContextViewer, OnNotify)
	{
		if (!lstrcmp(pszEvent, szAppActivateView) ||
			!lstrcmp(pszEvent, szEventActivateObject) ||
			!lstrcmp(pszEvent, szEventChangeObject) )
		{
			if (!lstrcmp(pszEvent, szEventChangeObject) )
			{
				if( punkFrom != (IUnknown*)(m_ptrLastImage) )
					return; // изменился не наш имейдж
				OnSettingsChanged();
			}

			// Use current active view from application because event szEventActivateObject can
			// occur with punkFrom which points to inactive view
			IApplicationPtr sptrApp(GetAppUnknown());
			IUnknownPtr punkActiveDoc;
			sptrApp->GetActiveDocument(&punkActiveDoc);
			IDocumentSitePtr sptrDSActiveDoc(punkActiveDoc);
			IUnknownPtr punkActiveView;
			if (sptrDSActiveDoc != 0) sptrDSActiveDoc->GetActiveView(&punkActiveView);
			if(!lstrcmp(pszEvent, szAppActivateView) && punkFrom==m_sptrView)
				return; // повторно активируем ту же вьюху - не надо ничего делать
			if(!lstrcmp(pszEvent, szEventActivateObject))
			{
				if(punkFrom!=punkActiveView)
					return; // сменился имейдж на неактивной вьюхе - пофиг
			}

			punkFrom = punkActiveView;
			// NOTE:
			// punkFrom == view
			// punkHit == document (NamedData)
			
//			if ( !lstrcmp(pszEvent, szEventActivateObject) )
			{
				bool bSuccess = false;
				IUnknownPtr ptrunkObject(GetActualImageFromView(punkFrom), false);
				IImage3Ptr sptrI = ptrunkObject;

				if(!lstrcmp(pszEvent, szEventActivateObject))
				{
					if( (IUnknown*)(ptrunkObject) == (IUnknown*)(m_ptrLastImage) )
						return; // актуальный имейдж на самом деле не сменился
					m_ptrLastImage = ptrunkObject;
				}
				if(!lstrcmp(pszEvent, szAppActivateView) && punkFrom!=m_sptrView)
				{
					m_ptrLastImage = ptrunkObject;
				}

				if(!lstrcmp(pszEvent, szEventChangeObject))
				{
					if( (IUnknown*)(ptrunkObject) != (IUnknown*)(m_ptrLastImage) )
						return; // актуальный имейдж на самом деле не сменился
					m_ptrLastImage = ptrunkObject;
				}

				if( sptrI != 0 )
				{
					IUnknown *punkCC = 0;
					sptrI->GetColorConvertor( &punkCC );

					if( punkCC )
					{
						IColorConvertorFISHPtr sptrFISH = punkCC;
						punkCC->Release();
						punkCC = 0;
						if( sptrFISH != 0 )
							bSuccess = true;
					}
					// [vanek] BT2000:4021 b всетаки не будем за нее цепляться - 30.09.2004
					// bSuccess = true; // независимо от фишовости картинки - зацепимся за нее
				}

				if(!bSuccess && m_sptrView != 0 )
				{
					UnRegister(m_sptrView);
					m_sptrView = 0;
					m_lLastViewKey = INVALID_KEY;
					_create_empty_bar(); 
				}

				if( !bSuccess )
					return;
			}

			if (!punkFrom) // last view removed
			{
				m_lLastViewKey = INVALID_KEY;

				if( m_sptrView != 0 )
				{
					UnRegister( m_sptrView );
					m_sptrView = 0;
				}
				return;
			}
			
			bool bImageCh = false;

			if ( !lstrcmp(pszEvent, szEventActivateObject) )
				bImageCh = true;

			if ( !lstrcmp(pszEvent, szEventChangeObject) )
				bImageCh = true;

			// if view is same as current ->simply return
			GuidKey	viewKey = ::GetObjectKey( punkFrom );

			if ( !bImageCh && m_lLastViewKey == viewKey )
				return;


			// else unregister viewer from current view
			if( m_sptrView )
				UnRegister( m_sptrView );

			// and register viewer to new view
			m_sptrView = punkFrom;
			CFishProps cf(0, m_sptrView);
			cf.GetLongProp(&m_nActiveItem, FISH_SHOWN_PANE);
			cf.GetLongProp(&m_dwUsedPanes, FISH_USED_PANES);

			/*INamedPropBagPtr ptrBag = m_sptrView;
			if( ptrBag != 0 )
			{
				_variant_t	var;
				ptrBag->GetProperty( _bstr_t( FISH_SHOWN_PANE ), &var );
				if( var.vt != VT_EMPTY ) 
					m_nActiveItem = var.lVal;
				else
					m_nActiveItem = -1;
				ptrBag->GetProperty( _bstr_t( FISH_USED_PANES ), &var );
				if( var.vt != VT_EMPTY ) 
					m_dwUsedPanes = var.lVal;
			}*/
			

			if( m_sptrView == 0 )
				return;

			Register(m_sptrView);
			m_lLastViewKey = viewKey;
			
			IUnknownPtr punkObject(GetActualImageFromView( m_sptrView ), false);
			if( punkObject )
			{
				m_nItemCount = ::GetImagePaneCount( punkObject ) - 1;
				m_nItemCount = max(0, m_nItemCount);
				if(m_nItemCount)
				{
					m_ResultPart.SetNData( punkObject );
					m_Combi.SetNData( punkObject );
					if (m_nActiveItem > m_nItemCount)
						_SetActiveItem(-1, true, true);
					if( GetFromNamedData( punkObject, _bstr_t( FISH_ROOT ), _bstr_t( FISH_USED_PANES ) ).vt == VT_EMPTY )
						_reload_to_ndata( ::GetAppUnknown(), punkObject );
					else
						_apply_own_ndata( punkObject );
					int dy = re_create_context( punkObject );
					_reset_scroller( m_nHeight = dy );
				}
				else _create_empty_bar();
			}
			else
			{
				m_sptrView = 0;
				UnRegister(punkFrom);
				m_lLastViewKey = INVALID_KEY;
				_create_empty_bar(); 
			}
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
				_create_empty_bar(); 
			}
		}
		else if (!lstrcmp(pszEvent, szEventNewSettings))
			OnSettingsChanged();
		else if (!lstrcmp(pszEvent, szEventNewSettingsView))
			_RefreshWindow();
		else if (!lstrcmp(pszEvent, szEventAfterActionExecute))
		{
			IActionPtr sptrAction(punkHit);
			if (sptrAction != 0)
			{
				IUnknownPtr punkAI;
				sptrAction->GetActionInfo(&punkAI);
				IActionInfoPtr sptrAI(punkAI);
				if (sptrAI != 0)
				{
					_bstr_t bstrName,bstrUserName,bstrHelpString,bstrGroupName;
					sptrAI->GetCommandInfo(bstrName.GetAddress(),bstrUserName.GetAddress(),
						bstrHelpString.GetAddress(), bstrGroupName.GetAddress());
					if (bstrName == _bstr_t("ObjectDeleteSelection") || bstrName == _bstr_t("Undo") ||
						bstrName == _bstr_t("Redo"))
						_RefreshWindow();
					else if (bstrName == _bstr_t("MovePrev") || bstrName == _bstr_t("MoveNext") ||
						bstrName == _bstr_t("MoveFirst") || bstrName == _bstr_t("MoveLast"))
						OnChangeViewOrObject();
				}
			}
		}
	}
	_catch;
}

void CFishContextViewer::Register(IUnknown *punkCtrl, const char *pszEvString)
{
	if (punkCtrl == NULL || CheckInterface(punkCtrl, IID_INotifyController))
		CEventListenerImpl::Register(punkCtrl, pszEvString);
	IViewPtr sptrView(punkCtrl);
	if (sptrView != 0)
	{
		IUnknownPtr punkDoc;
		sptrView->GetDocument(&punkDoc);
		if (punkDoc != 0)
			Register(punkDoc, pszEvString);
	}
}

void CFishContextViewer::UnRegister(IUnknown *punkCtrl, const char *pszEvString)
{
	if (punkCtrl == NULL || CheckInterface(punkCtrl, IID_INotifyController))
		CEventListenerImpl::UnRegister(punkCtrl, pszEvString);
	IViewPtr sptrView(punkCtrl);
	if (sptrView != 0)
	{
		IUnknownPtr punkDoc;
		sptrView->GetDocument(&punkDoc);
		if (punkDoc != 0)
			UnRegister(punkDoc, pszEvString);
	}
}


void CFishContextViewer::OnSettingsChanged()
{
	CIniValue::Reset();
	int nGapBetweenControls = ::GetValueInt(::GetAppUnknown(), FISH_ROOT, "GapBetweenControls", 1);
	int nLeftOffset = ::GetValueInt(::GetAppUnknown(), FISH_ROOT, "LeftOffset", 1);
	int nTopOffset = ::GetValueInt(::GetAppUnknown(), FISH_ROOT, "TopOffset", 1);
	if (nGapBetweenControls != m_nGapBetweenControls || nLeftOffset != m_nLeftOffset ||
		nTopOffset != m_nTopOffset)
	{
		m_nGapBetweenControls = nGapBetweenControls;
		m_nLeftOffset = nLeftOffset;
		m_nTopOffset = nTopOffset;
		m_ResultPart.SetWindowPos(0, m_nLeftOffset, m_nTopOffset, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
		CRect rcRes;
		m_ResultPart.GetWindowRect(rcRes);
		bool bHideCombi = ::GetValueInt(::GetAppUnknown(), FISH_ROOT, "HideCombi", 0) != 0;
		m_Combi.SetWindowPos(0, bHideCombi ? rcRes.left : rcRes.right+m_nGapBetweenControls, rcRes.top, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
		RefreshWindow();
	}
	else
	{
		m_ResultPart.m_wBorder.Invalidate();
		m_Combi.m_wBorder.Invalidate();
		for (unsigned i = 0; i < m_arrLayers.size(); i++)
		{
			m_arrLayers[i]->SetBackgroundColor(i+1==m_nActiveItem?g_ActiveColor:g_BackgroundColor);
			m_arrLayers[i]->m_wBorder.Invalidate();
		}
	}
	Invalidate();
}

IUnknownPtr CFishContextViewer::GetNData()
{
	IUnknownPtr punkObject(GetActualImageFromView(m_sptrView),false);
	if (punkObject == 0)
		punkObject = GetAppUnknown();
	return punkObject;
}

int CFishContextViewer::GetFiltersCount()
{
	return m_arrLayers.size();
}

BSTR CFishContextViewer::GetFilterName(int iLayer)
{
	if (iLayer > 0 && iLayer <= (int)m_arrLayers.size())
	{
		CLayerPartDlg *pDlg = m_arrLayers[iLayer-1];
		CString str;
		str.Format("%d", pDlg->m_nID);
		CString strName = GetValueString(GetNData(), FISH_ROOT, FISH_NAME_PANES + str, FISH_NAME_PANES + str);
		return strName.AllocSysString();
	}
	return NULL;
}

BSTR CFishContextViewer::GetFilterShortName(int iLayer)
{
	if (iLayer > 0 && iLayer <= (int)m_arrLayers.size())
	{
		CLayerPartDlg *pDlg = m_arrLayers[iLayer-1];
		CString str;
		str.Format("%d", pDlg->m_nID);
		IUnknownPtr punkObject(GetActualImageFromView(m_sptrView),false);
		if (punkObject == 0)
			punkObject = GetAppUnknown();
		CString strName = GetValueString(punkObject, FISH_ROOT, FISH_PANE_SNAME+str, 0);
		return strName.AllocSysString();
	}
	return NULL;
}

int CFishContextViewer::GetFilterState(int iLayer)
{
	if (iLayer > 0 && iLayer <= (int)m_arrLayers.size())
	{
		CLayerPartDlg *pDlg = m_arrLayers[iLayer-1];
		DWORD nMask = 1<<pDlg->m_nID;
		return UsedPanes()&nMask?1:0;
	}
	return 0;
}

int CFishContextViewer::GetFilterColor(int iLayer)
{
	if (iLayer > 0 && iLayer <= (int)m_arrLayers.size())
	{
		CLayerPartDlg *pDlg = m_arrLayers[iLayer-1];
		CString str;
		str.Format("%d", pDlg->m_nID);
		COLORREF c = ::GetValueColor(GetNData(), FISH_COLORS, CString(FISH_COLOR_PANE)+str, 0);
		return c;
	}
	return 0;
}

void CFishContextViewer::SetFilterName(int iLayer, LPCTSTR bstrName)
{
	if (iLayer > 0 && iLayer <= (int)m_arrLayers.size())
	{
		CLayerPartDlg *pDlg = m_arrLayers[iLayer-1];
		CString str;
		str.Format("%d", pDlg->m_nID);
		IUnknownPtr punk(GetNData());
		SetValue(punk, FISH_ROOT, FISH_NAME_PANES + str, CString(bstrName));
		pDlg->Refresh(pDlg->m_nID, punk);
		FireEvent(GetAppUnknown(), szEventNamedPropsChanged, NULL, m_sptrView, NULL, 0);
	}
}

void CFishContextViewer::SetFilterShortName(int iLayer, LPCTSTR bstrName)
{
	if (iLayer > 0 && iLayer <= (int)m_arrLayers.size())
	{
		CLayerPartDlg *pDlg = m_arrLayers[iLayer-1];
		CString str;
		str.Format("%d", pDlg->m_nID);
		IUnknownPtr punk(GetNData());
		SetValue(punk, FISH_ROOT, FISH_PANE_SNAME+str, CString(bstrName));
		pDlg->Refresh(pDlg->m_nID, punk);
		FireEvent(GetAppUnknown(), szEventNamedPropsChanged, NULL, m_sptrView, NULL, 0);
	}
}

void CFishContextViewer::SetFilterState(int iLayer, int nState)
{
	if (iLayer > 0 && iLayer <= (int)m_arrLayers.size())
	{
		CLayerPartDlg *pDlg = m_arrLayers[iLayer-1];
		pDlg->ShowPane(nState>0);
	}
}

void CFishContextViewer::SetFilterColor(int iLayer, int nColor)
{
	if (iLayer > 0 && iLayer <= (int)m_arrLayers.size())
	{
		CLayerPartDlg *pDlg = m_arrLayers[iLayer-1];
		pDlg->ChangeColor(nColor);
		pDlg->SetColor(nColor);
	}
}

static void EnableControls(CWnd *pWnd, BOOL bEnabled)
{
	pWnd->EnableWindow(bEnabled);
	for (CWnd *p = pWnd->GetWindow(GW_CHILD); p; p = p->GetWindow(GW_HWNDNEXT))
		p->EnableWindow(bEnabled);
	pWnd->Invalidate();
};

static void EnableResult(CResultPartDlg *pResDlg, BOOL bEnabled)
{
	EnableControls(pResDlg, bEnabled);
	pResDlg->m_Data.Invalidate();
	pResDlg->m_wBorder.Invalidate();
}

static void EnableLayer(CLayerPartDlg *pLayerDlg, BOOL bEnabled)
{
	EnableControls(pLayerDlg, bEnabled);
	pLayerDlg->m_Data.Invalidate();
	pLayerDlg->m_wBorder.Invalidate();
}

void CFishContextViewer::EnableItem(int iLayer, int nEnabled)
{
	if (iLayer == m_nActiveItem)
	{
		int nNewActiveItem = -3;
		if (iLayer != -1 && m_ResultPart.IsWindowEnabled())
			nNewActiveItem = -1;
		else if (iLayer != -2 && m_Combi.IsWindowEnabled())
			nNewActiveItem = -2;
		else
		{
			for (int i = 0; i < (int)m_arrLayers.size(); i++)
			{
				CLayerPartDlg *pDlg = m_arrLayers[i];
				if (i+1 != iLayer && pDlg->IsWindowEnabled())
					nNewActiveItem = i+1;
			}
		}
		_SetActiveItem(nNewActiveItem, true, false);
	}
	if (iLayer > 0 && iLayer <= (int)m_arrLayers.size())
	{
		CLayerPartDlg *pDlg = m_arrLayers[iLayer-1];
		EnableLayer(pDlg, nEnabled);
	}
	else if (iLayer == -1)
		EnableResult(&m_ResultPart, nEnabled);
	else if (iLayer == -2)
		EnableResult(&m_Combi, nEnabled);
}


//--------------------------------------------------------------
int CFishContextViewer::re_create_context(IUnknown *punkNData, bool bAdd)
{
	// [vanek] BT2000:4021 reset panes - 30.09.2004
	m_ResultPart.SetNData( 0 );
	m_Combi.SetNData( 0 );

	int dx = m_nGapBetweenControls, dy = m_nGapBetweenControls;
	if (!bAdd)
	{
		for( IT it = m_arrLayers.begin(); it != m_arrLayers.end(); ++it )
		{
			if( (*it)->m_hWnd )
				(*it)->DestroyWindow();
			delete (*it);
		}
		m_arrLayers.clear();
	}

	int nnew_height = 0;
	if( m_nItemCount )
	{
		CRect rcRes, rcLayer;
		m_Combi.GetWindowRect( &rcRes );
		ScreenToClient(&rcRes);
		int cx = -1, cy = -1;
		int nPrevSize = m_arrLayers.size();
		m_ResultPart.SetNData( punkNData );
		m_Combi.SetNData( punkNData );
		for (int i = bAdd?nPrevSize:0; i < m_nItemCount; i++)
		{
			m_arrLayers.push_back( new CLayerPartDlg( this ) );
			m_arrLayers[i]->Create( CLayerPartDlg::IDD, this );
			m_arrLayers[i]->Refresh( i + 1, punkNData );
			m_arrLayers[i]->ShowPane( ( m_dwUsedPanes & 1 << (i + 1) ) != 0, true );
			m_arrLayers[i]->SetNData( punkNData );
			m_arrLayers[i]->ModifyStyle( 0, WS_CLIPCHILDREN | WS_CLIPSIBLINGS );
			if (!i || bAdd&&i==nPrevSize)
			{
				m_arrLayers[0]->GetWindowRect( &rcLayer );
				ScreenToClient( &rcLayer );
				cx = rcLayer.Width();
				cy = rcLayer.Height();
			}
			if (m_dwDockSide == AFX_IDW_DOCKBAR_BOTTOM)
				m_arrLayers[i]->MoveWindow(rcRes.right+dx*(i+1)+cx*i, rcRes.top, cx, cy);
			else
				m_arrLayers[i]->MoveWindow(rcRes.left, rcRes.bottom+dy*(i+1)+(cy+dy)*i, cx, cy);
		}

		m_arrLayers[i-1]->GetWindowRect( &rcLayer );
		ScreenToClient(&rcLayer);

		for( IT it = m_arrLayers.begin(); it != m_arrLayers.end(); ++it )
			(*it)->ShowWindow( SW_SHOW );

		if (!bAdd) _SetActiveItem(m_nActiveItem,false,false);
		if (m_phrzScoll) m_phrzScoll->SetWindowPos(&CWnd::wndTop, 0, 0,0,0, SWP_NOMOVE|SWP_NOSIZE);
		if (m_pvrtScoll) m_phrzScoll->SetWindowPos(&CWnd::wndTop, 0, 0,0,0, SWP_NOMOVE|SWP_NOSIZE);
		m_StaticRightBottom.SetWindowPos(&CWnd::wndTop, 0, 0,0,0, SWP_NOMOVE|SWP_NOSIZE);
		if (m_dwDockSide == AFX_IDW_DOCKBAR_BOTTOM)
			nnew_height = rcLayer.right + GetScrollPos(SB_HORZ);
		else
			nnew_height = rcLayer.bottom + GetScrollPos(SB_VERT);
	}

	// [vanek] : redraw anyway - 30.09.2004
    m_ResultPart.m_Data.RedrawWindow();
	m_Combi.m_Data.RedrawWindow();
   	return nnew_height;
}

void CFishContextViewer::UpdateCC()
{
	IUnknown *punkObject = GetActualImageFromView( m_sptrView );
	if( punkObject )
	{
		IImagePtr ptrImage = punkObject;
		punkObject->Release();
		punkObject = 0;

		if( ptrImage != 0 )
		{
			IUnknown *punkCC = 0;
			ptrImage->GetColorConvertor( &punkCC );
			if( punkCC )
			{
				IColorConvertorFISHPtr ptrCC = punkCC;
				punkCC->Release();
				punkCC = 0;

				if( ptrCC != 0 )
				{
					ptrCC->ReloadColorValue();
					CWnd *pWnd = ::GetWindowFromUnknown( m_sptrView );
					if( pWnd )
						pWnd->Invalidate();
					_RefreshWindow();
				}
			}
		}
	}
}

void CFishContextViewer::UpdateCC_ID( UINT nID, COLORREF clColor )
{
	IUnknown *punkObject = GetActualImageFromView( m_sptrView );
	if( punkObject )
	{
		IImagePtr ptrImage = punkObject;
		punkObject->Release();
		punkObject = 0;

		if( ptrImage != 0 )
		{
			IUnknown *punkCC = 0;
			ptrImage->GetColorConvertor( &punkCC );
			if( punkCC )
			{
				IColorConvertorFISHPtr ptrCC = punkCC;
				punkCC->Release();
				punkCC = 0;

				if( ptrCC != 0 )
				{
					ptrCC->SetColorValue( nID, clColor );
					CWnd *pWnd = ::GetWindowFromUnknown( m_sptrView );
					if( pWnd )
						pWnd->Invalidate();
					_RefreshWindow();
				}
			}
		}
	}
}


void CFishContextViewer::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO siH = {0}, siV = {0};
	GetScrollInfo(SB_HORZ, &siH);
	GetScrollInfo(SB_VERT, &siV);
	bool bStatus = true;
	if( nSBCode == SB_LINEDOWN )
		siV.nPos++;
	else if( nSBCode == SB_LINEUP )
		siV.nPos--;
	else if( nSBCode == SB_PAGEDOWN )
		siV.nPos += siV.nPage;
	else if( nSBCode == SB_PAGEUP )
		siV.nPos -= siV.nPage;
	else if( nSBCode == SB_THUMBTRACK )
		siV.nPos = siV.nTrackPos;
	else
		bStatus = false;
	if( bStatus )
	{
		siV.fMask = SIF_POS;
		SetScrollInfo(SB_VERT, &siV, TRUE);
		GetScrollInfo(SB_VERT, &siV, SIF_POS);
		_reposition(-siH.nPos, -siV.nPos);
	}
	__super::OnVScroll(nSBCode, nPos, pScrollBar );
}

void CFishContextViewer::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO siH = {0}, siV = {0};
	GetScrollInfo(SB_HORZ, &siH);
	GetScrollInfo(SB_VERT, &siV);
	bool bStatus = true;
	if( nSBCode == SB_LINEDOWN )
		siH.nPos++;
	else if( nSBCode == SB_LINEUP )
		siH.nPos--;
	else if( nSBCode == SB_PAGEDOWN )
		siH.nPos += siH.nPage;
	else if( nSBCode == SB_PAGEUP )
		siH.nPos -= siH.nPage;
	else if( nSBCode == SB_THUMBTRACK )
		siH.nPos = siH.nTrackPos;
	else
		bStatus = false;
	if( bStatus )
	{
		siH.fMask = SIF_POS;
		SetScrollInfo(SB_HORZ, &siH, TRUE);
		GetScrollInfo(SB_HORZ, &siH, SIF_POS);
		_reposition(-siH.nPos, -siV.nPos);
	}
	__super::OnHScroll(nSBCode, nPos, pScrollBar );
}

void CFishContextViewer::_reset_scroller(int nHeight)
{
	CRect rcC;
	GetClientRect(&rcC);
	if (rcC.Width() == 0 || rcC.Height() == 0)
		return;
	bool bHorz = m_dwDockSide == AFX_IDW_DOCKBAR_BOTTOM;
	SCROLLINFO siH = {0},siV = {0};
	siV.cbSize = siH.cbSize = sizeof( SCROLLINFO );
	siV.fMask = siH.fMask = SIF_PAGE | SIF_RANGE;
	siH.nPage = rcC.Width();
	siV.nPage = rcC.Height();
	if (bHorz)
	{
		siH.nMax = m_nLeftOffset+nHeight;
		if (rcC.Width() < siH.nMax)
			siV.nMax = m_nTopOffset+m_size.cy+::GetSystemMetrics(SM_CYHSCROLL);
		else
			siV.nMax = m_nTopOffset+m_size.cy;
		if (siV.nMax > rcC.Height())
			siH.nMax += ::GetSystemMetrics(SM_CXVSCROLL);
		m_fPageCount = nHeight/siH.nPage;
	}
	else
	{
		siV.nMax = m_nTopOffset+nHeight;
		if (rcC.Height() < siV.nMax)
			siH.nMax = m_nLeftOffset+m_size.cx+::GetSystemMetrics(SM_CXVSCROLL);
		else
			siH.nMax = m_nLeftOffset+m_size.cx;
		if (siH.nMax > rcC.Width())
			siV.nMax += ::GetSystemMetrics(SM_CYHSCROLL);
		m_fPageCount = nHeight/siV.nPage;
	}
	SetScrollInfo(SB_HORZ, &siH, true);
	SetScrollInfo(SB_VERT, &siV, true);
	_reposition_scrollers();
	_reposition(-GetScrollPos(SB_HORZ), -GetScrollPos(SB_VERT));
}

void CFishContextViewer::_reposition_scrollers()
{
	if (m_phrzScoll == NULL || m_pvrtScoll == NULL) return;
 	CRect rcC = NORECT;
	GetClientRect(&rcC);
	SCROLLINFO siH,siV;
	GetScrollInfo(SB_HORZ, &siH, SIF_RANGE);
	GetScrollInfo(SB_VERT, &siV, SIF_RANGE);
	bool bHorzScroll = siH.nMax-siH.nMin>rcC.Width();
	bool bVertScroll = siV.nMax-siV.nMin>rcC.Height();
	m_pvrtScoll->ShowWindow(bVertScroll?SW_SHOW:SW_HIDE);
	m_phrzScoll->ShowWindow(bHorzScroll?SW_SHOW:SW_HIDE);
	int cxVScroll = ::GetSystemMetrics( SM_CXVSCROLL );
	int cyHScroll = ::GetSystemMetrics( SM_CYHSCROLL );
	CRect rcH,rcV;
	if (bVertScroll && bHorzScroll || !bVertScroll && !bHorzScroll)
	{
		m_phrzScoll->MoveWindow(CRect(rcC.left, rcC.bottom-cyHScroll, rcC.right-cxVScroll, rcC.bottom));
		m_pvrtScoll->MoveWindow(CRect(rcC.right-cxVScroll, rcC.top, rcC.right, rcC.bottom-cyHScroll));
	}
	else if (bVertScroll)
		m_pvrtScoll->MoveWindow(CRect(rcC.right-cxVScroll, rcC.top, rcC.right, rcC.bottom));
	else
		m_phrzScoll->MoveWindow(CRect(rcC.left, rcC.bottom-cyHScroll, rcC.right, rcC.bottom));
	m_phrzScoll->SetWindowPos(&CWnd::wndTop, 0, 0,0,0, SWP_NOMOVE|SWP_NOSIZE);
	m_pvrtScoll->SetWindowPos(&CWnd::wndTop, 0, 0,0,0, SWP_NOMOVE|SWP_NOSIZE);
	if (bVertScroll && bHorzScroll)
	{
		m_StaticRightBottom.MoveWindow(CRect(rcC.right-cxVScroll, rcC.bottom-cyHScroll, rcC.right, rcC.bottom));
		m_StaticRightBottom.ShowWindow(SW_SHOW);
	}
	else
		m_StaticRightBottom.ShowWindow(SW_HIDE);
}

void CFishContextViewer::_reposition(int x, int y)
{
	const int dx = m_nGapBetweenControls, dy = m_nGapBetweenControls;
	if (m_ResultPart.m_hWnd == 0 || m_Combi.m_hWnd == 0) return;
	m_ResultPart.SetWindowPos(NULL, m_nLeftOffset+x, m_nTopOffset+y, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	CRect rcRes;
	m_ResultPart.GetWindowRect(&rcRes);
	ScreenToClient( &rcRes );
	bool bHideCombi = ::GetValueInt(::GetAppUnknown(), FISH_ROOT, "HideCombi", 0) != 0;
	if (m_dwDockSide == AFX_IDW_DOCKBAR_BOTTOM)
		m_Combi.SetWindowPos(NULL, bHideCombi ? rcRes.left : rcRes.right+dx, rcRes.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	else
		m_Combi.SetWindowPos(NULL, rcRes.left, bHideCombi ? rcRes.top : rcRes.bottom+dy, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	m_Combi.GetWindowRect(&rcRes);
	ScreenToClient(&rcRes);
	int cx = -1, cy = -1;
	for( int i = 0; i < m_nItemCount; i++ )
	{
		if( !i )
		{
			CRect rcLayer;
			m_arrLayers[i]->GetWindowRect(&rcLayer);
			ScreenToClient( &rcLayer );
			cx = rcLayer.Width();
			cy = rcLayer.Height();
		}
		if (m_dwDockSide == AFX_IDW_DOCKBAR_BOTTOM)
			m_arrLayers[i]->SetWindowPos(0, rcRes.right+dx*(i+1)+cx*i, rcRes.top,
				0, 0, SWP_NOSIZE|SWP_NOZORDER);
		else
			m_arrLayers[i]->SetWindowPos(0, rcRes.left, rcRes.bottom+dy*(i+1)+cy*i,
				0, 0, SWP_NOSIZE|SWP_NOZORDER);
	}
}

int CFishContextViewer::GetScrollPos(int nBar)
{
	SCROLLINFO si;
	GetScrollInfo(nBar, &si, SIF_POS);
	return si.nPos;
}

void CFishContextViewer::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	_reposition_scrollers();
	_reset_scroller( m_nHeight );
}

void CFishContextViewer::UnpressBtns(CWnd *pPressed)
{
	if( pPressed != (CWnd *)&m_ResultPart )
	{
		if( m_ResultPart.m_wBorder.SetState( false, 0 ) )
		{
			RECT rc = {0};
			::GetClientRect( m_ResultPart.m_wBorder.m_hWnd, &rc );
			::InvalidateRect( m_ResultPart.m_wBorder.m_hWnd, &rc, 1 );
		}
	}
	if( pPressed != (CWnd *)&m_Combi )
	{
		if( m_Combi.m_wBorder.SetState( false, 0 ) )
		{
			RECT rc = {0};
			::GetClientRect( m_Combi.m_wBorder.m_hWnd, &rc );
			::InvalidateRect( m_Combi.m_wBorder.m_hWnd, &rc, 1 );
		}
	}
	
	for( int i = 0; i < m_nItemCount; i++ )
	{
		if( pPressed == (CWnd *)m_arrLayers[i] )
			continue;

		if( m_arrLayers[i]->m_wBorder.SetState( false, 0 ) )
		{
			m_arrLayers[i]->SetBackgroundColor(g_BackgroundColor);
			RECT rc = {0};
			::GetClientRect( m_arrLayers[i]->m_wBorder.m_hWnd, &rc );
			::InvalidateRect( m_arrLayers[i]->m_wBorder.m_hWnd, &rc, 0 );
		}
	}

}

void CFishContextViewer::StoreToNamedData( IUnknownPtr sptrImage, const _bstr_t &bstrSect, const _bstr_t &bstrKey, const _variant_t &var )
{
	INamedDataObject2Ptr ptrNDO = sptrImage;
	if( ptrNDO == 0 )
		return;

	INamedDataPtr sptrNDO = sptrImage;

	if( sptrNDO == 0 )
	{
		ptrNDO->InitAttachedData();
		sptrNDO = ptrNDO;
	}

	sptrNDO->SetupSection( bstrSect );
	sptrNDO->SetValue( bstrKey, var );
}

_variant_t CFishContextViewer::GetFromNamedData( IUnknownPtr sptrImage, const _bstr_t &bstrSect, const _bstr_t &bstrKey )
{
	INamedDataObject2Ptr ptrNDO = sptrImage;

	INamedDataPtr sptrNDO = sptrImage;

	if( sptrNDO == 0 )
	{
		ptrNDO->InitAttachedData();
		sptrNDO = ptrNDO;
	}

	_variant_t var;
	sptrNDO->SetupSection( bstrSect );
	sptrNDO->GetValue( bstrKey, &var );
	return var;
}

void CFishContextViewer::_reload_to_ndata(IUnknown * punkNDataFrom, IUnknown * punkNDataTo )
{
	int nItemsInShellData = ::GetValueInt(::GetAppUnknown(), FISH_ROOT, FISH_PANES_COUNT, 0);
	for( int i = 0; i < max(m_nItemCount,nItemsInShellData); i++ )
	{
		CString str;
		str.Format( "%d", i + 1 );
		COLORREF clColor = ::GetValueColor( punkNDataFrom, FISH_COLORS, CString( FISH_COLOR_PANE ) + str, 0 );
		
		_bstr_t bstrName = ::GetValueString( punkNDataFrom, _bstr_t(FISH_ROOT), CString(FISH_NAME_PANES) + str, "" );
		_bstr_t bstrSName = ::GetValueString( punkNDataFrom, _bstr_t(FISH_ROOT), CString(FISH_PANE_SNAME) + str, "" );

		int	r = GetRValue( clColor );
		int	g = GetGValue( clColor );
		int	b = GetBValue( clColor );

		char	sz[30];
		sprintf( sz, "(%d, %d, %d)", r, g, b );

		StoreToNamedData( punkNDataTo, _bstr_t( FISH_COLORS ), _bstr_t( CString( FISH_COLOR_PANE ) + str ), _bstr_t( sz ) );
		StoreToNamedData( punkNDataTo, _bstr_t( FISH_ROOT ), _bstr_t( CString( FISH_NAME_PANES ) + str ), _variant_t( bstrName ) );
		StoreToNamedData( punkNDataTo, _bstr_t( FISH_ROOT ), _bstr_t( CString( FISH_PANE_SNAME ) + str ), _variant_t( bstrSName ) );
	}
	StoreToNamedData( punkNDataTo, _bstr_t( FISH_ROOT ), _bstr_t( FISH_USED_PANES ), _variant_t( (long)UsedPanes() ) );
}

void CFishContextViewer::_apply_own_ndata( IUnknown * punkNData )
{
	// CFishProps cf(0, m_sptrView);
	// if( !cf.GetLongProp(&m_dwUsedPanes, FISH_USED_PANES) )
	{
		if (::GetValueInt(::GetAppUnknown(), FISH_ROOT, FISH_BAR_SETS_PROPS, 1) != 0 )
		{
			INamedPropBagPtr ptrBag = m_sptrView;
			if( ptrBag != 0 )
				ptrBag->SetProperty( _bstr_t( FISH_USED_PANES ), _variant_t( m_dwUsedPanes ) );
		}
	}

	/*INamedPropBagPtr ptrBag = m_sptrView;
	if( ptrBag != 0 )
	{
		_variant_t	var;
		ptrBag->GetProperty( _bstr_t( FISH_USED_PANES ), &var );
		if (var.vt == VT_I4)
			m_dwUsedPanes = var.lVal;
		else
		{
			m_dwUsedPanes = GetFromNamedData( punkNData, _bstr_t( FISH_ROOT ), _bstr_t( FISH_USED_PANES ) ).lVal;
			if (::GetValueInt(::GetAppUnknown(), FISH_ROOT, FISH_BAR_SETS_PROPS, 1) != 0)
				ptrBag->SetProperty( _bstr_t( FISH_USED_PANES ), _variant_t( m_dwUsedPanes ) );
		}
	}*/

	IImagePtr ptrImage = punkNData;

	if( ptrImage != 0 )
	{
		IUnknown *punkCC = 0;
		ptrImage->GetColorConvertor( &punkCC );
		if( punkCC )
		{
			IColorConvertorFISHPtr ptrCC = punkCC;
			punkCC->Release();
			punkCC = 0;

			if( ptrCC != 0 )
			{

				for( int i = 0; i < m_nItemCount; i++ )
				{
					CString str;
					str.Format( "%d", i + 1 );
					COLORREF clColor = ::GetValueColor( punkNData, FISH_COLORS, CString( FISH_COLOR_PANE ) + str, 0 );

					ptrCC->SetColorValue( i + 1, clColor );
				}
			}
		}
	}
}

void CFishContextViewer::_create_empty_bar()
{
	m_nItemCount = ::GetValueInt(::GetAppUnknown(), FISH_ROOT, FISH_PANES_COUNT, 0);
	m_dwUsedPanes = ::GetValueInt(::GetAppUnknown(), FISH_ROOT, FISH_USED_PANES, 0);
	m_nActiveItem = ::GetValueInt(::GetAppUnknown(), FISH_ROOT, FISH_SHOWN_PANE, -1);
	m_nHeight = re_create_context(GetAppUnknown());
	_reset_scroller(m_nHeight);
}

void CFishContextViewer::_SetActiveItem(int nActiveItem, bool bSetToNPG, bool bFire)
{
	if (bSetToNPG)
	{
		IUnknownPtr punkObject(GetActualImageFromView( m_sptrView ), false);
		INamedPropBagPtr ptrBagImage(punkObject);
		if( ptrBagImage != 0 )
		{
			_variant_t	var = long(nActiveItem);
			if (::GetValueInt(::GetAppUnknown(), FISH_ROOT, FISH_BAR_SETS_PROPS, 1) != 0)
				ptrBagImage->SetProperty( _bstr_t( FISH_SHOWN_PANE ), var);
		}

		INamedPropBagPtr ptrBag = m_sptrView;
		if( ptrBag != 0 )
		{
			_variant_t	var = long(nActiveItem);
			if (::GetValueInt(::GetAppUnknown(), FISH_ROOT, FISH_BAR_SETS_PROPS, 1) != 0)
				ptrBag->SetProperty( _bstr_t( FISH_SHOWN_PANE ), var);
			FireEvent(GetAppUnknown(), szEventNamedPropsChanged, NULL, ptrBag, NULL, 0);
		}
	}
	m_nActiveItem = nActiveItem;
	if (m_nActiveItem > 0 && m_nActiveItem <= m_nItemCount)
		m_arrLayers[m_nActiveItem-1]->Select();
	else if (m_nActiveItem == -2)
		m_Combi.Select();
	else if (m_nActiveItem == -1)
		m_ResultPart.Select();
	if (bFire)
		FireOnChangeActive(nActiveItem);
}

void CFishContextViewer::SetActiveItem(int nActiveItem)
{
	_SetActiveItem(nActiveItem,true,false);
}

void CFishContextViewer::FireScriptEvent(LPCTSTR lpszEventName, int nParams, VARIANT *pvarParams)
{
	IScriptSitePtr	sptrSS(::GetAppUnknown());
	if (sptrSS == 0) return;
	_bstr_t bstrEvent("FishContextViewer_");
	bstrEvent += lpszEventName;
	sptrSS->Invoke(bstrEvent, pvarParams, nParams, 0, fwFormScript);
	sptrSS->Invoke(bstrEvent, pvarParams, nParams, 0, fwAppScript);
}

void CFishContextViewer::FireOnChangeActive(int nNewActive)
{
	_variant_t varNewActive((long)nNewActive);
	FireScriptEvent(_T("OnChangeActive"),1,&varNewActive);
}

void CFishContextViewer::FireOnChangeName(int nItem, LPCTSTR lpstrNewName)
{
	_variant_t vars[2];
	vars[0] = _variant_t((long)nItem);
	vars[1] = _variant_t(lpstrNewName);
	FireScriptEvent(_T("OnChangeName"),2,vars);
}

void CFishContextViewer::FireOnChangeColor(int nItem, int nNewColor)
{
	_variant_t vars[2];
	vars[1] = _variant_t((long)nItem);
	vars[0] = _variant_t((long)nNewColor);
	CString s;
	FireScriptEvent(_T("OnChangeColor"),2,vars);
}

void CFishContextViewer::FireOnChangeCheck(int nItem, BOOL bNewCheck)
{
	_variant_t vars[2];
	vars[1] = _variant_t((long)nItem);
	vars[0] = _variant_t((long)bNewCheck);
	FireScriptEvent(_T("OnChangeCheck"),2,vars);
}

void CFishContextViewer::OnDestroy()
{
	delete m_phrzScoll;
	delete m_pvrtScoll;
	__super::OnDestroy();
	IUnknownPtr	sptrUnkMainFrame(::_GetOtherComponent(::GetAppUnknown(), IID_IMainWindow), false);
	IWindow2Ptr sptrMainFrame = sptrUnkMainFrame;
	if (sptrMainFrame != 0)
		sptrMainFrame->DetachMsgListener(GetControllingUnknown());
}

void CFishContextViewer::PostNcDestroy()
{
	delete this;
}

void CFishContextViewer::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	ExecuteContextMenu(point);
}

BOOL CFishContextViewer::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect(rc, g_BackgroundColor);
	return TRUE;
}

HBRUSH CFishContextViewer::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);
	if (nCtlColor == CTLCOLOR_STATIC)
		hbr = m_brBack;
	return hbr;
}
