// ContextTree.cpp : implementation file
//

#include "stdafx.h"
#include "ContextTree.h"
#include "ContextItem.h"
#include "MemDC.h"
#include "InPlaceEdit.h"
#include "Resource.h"
#include <math.h>

#ifndef CONTEXTCTRL_CLASSNAME
#define CONTEXTCTRL_CLASSNAME    _T("MFCContextCtrl")  // Window class name
#endif


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CContextCtrl, CWnd);
static bool sbBreak = false;


// Get the number of lines to scroll with each mouse wheel notch
// Why doesn't windows give us this function???
UINT GetMouseScrollLines()
{
    int nScrollLines = 3;            // reasonable default
    HKEY hKey;

    if (RegOpenKeyEx(HKEY_CURRENT_USER,  _T("Control Panel\\Desktop"),
                     0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
    {
        TCHAR szData[128];
        DWORD dwKeyDataType;
        DWORD dwDataBufSize = sizeof(szData);

        if (RegQueryValueEx(hKey, _T("WheelScrollLines"), NULL, &dwKeyDataType,
                           (LPBYTE) &szData, &dwDataBufSize) == ERROR_SUCCESS)
        {
            nScrollLines = _tcstoul(szData, NULL, 10);
        }
        RegCloseKey(hKey);
    }

    return nScrollLines;
}



/////////////////////////////////////////////////////////////////////////////
// CContextCtrl
/*
bool CContextCtrl::CreateUpDownBmp()
{
	if (!CContextItemHeader::s_pBmpUp)
	{
		CContextItemHeader::s_pBmpUp = new CBitmap();
		if (!CContextItemHeader::s_pBmpUp)
			return false;

		if (!CContextItemHeader::s_pBmpUp->LoadBitmap(IDB_ARROWUP))
			return false;
	}	

	if (!CContextItemHeader::s_pBmpDown)
	{
		CContextItemHeader::s_pBmpDown = new CBitmap();
		if (!CContextItemHeader::s_pBmpDown)
			return false;

		if (!CContextItemHeader::s_pBmpDown->LoadBitmap(IDB_ARROWDOWN))
			return false;
	}
	return true;
}

void CContextCtrl::DestroyUpDownBmp()
{
	if (CContextItemHeader::s_pBmpUp)
	{
		CContextItemHeader::s_pBmpUp->DeleteObject();
		delete CContextItemHeader::s_pBmpUp, CContextItemHeader::s_pBmpUp = 0;
	}

	if (CContextItemHeader::s_pBmpDown)
	{
		CContextItemHeader::s_pBmpDown->DeleteObject();
		delete CContextItemHeader::s_pBmpDown, CContextItemHeader::s_pBmpDown = 0;
	}
}
*/


CContextCtrl::CContextCtrl()
{
    RegisterWindowClass();

    // Initialize OLE libraries
    m_bMustUninitOLE = FALSE;

    _AFX_THREAD_STATE* pState = AfxGetThreadState();
    if (!pState->m_bNeedTerm)
    {
        SCODE sc = ::OleInitialize(NULL);
        if (FAILED(sc))
            AfxMessageBox(_T("OLE initialization failed. Make sure that the OLE libraries are the correct version"));
        else
            m_bMustUninitOLE = TRUE;
    }

	m_RootItem.SetTree(this);

	m_FirstVisibleItem	= 0;	
	m_LastVisibleItem	= 0;
	m_FocusItem			= 0;
	m_LeftClickDownItem	= 0;
	m_TrackedItem		= 0;
	m_hPrevFocusItem	= 0;

	m_nMinMoveDistance	= 10;
	m_nDefHeaderHeight  = 22;
	m_nDefItemHeight	= 48;
	m_nMargin			= 3;
	m_nIndent			= 20;
	m_nNumOffset		= m_nMargin;
	m_nImageOffset		= m_nMargin;
	m_nBorder			= 1;
	m_nIconSize			= ICON_SIZE;
	m_nExtSize			= 2;

    SetTextColor();
    SetBkColor();
    SetLightColor();
    SetHighlightColor();
    SetShadowColor();
    SetDKShadowColor();
	SetTrackColor();
	SetDisableColor();
	SetEnableColor();

	CreateLightBrush();

    SetTitleTipTextClr(CLR_DEFAULT);  //FNA
	SetTitleTipBackClr(CLR_DEFAULT); 

    m_nRowsPerWheelNotch = GetMouseScrollLines(); // Get the number of lines

	m_bEnableTracking	= true;
	m_bEditable			= true;
    m_bSingleSelection	= false;
	m_bEnableSelection  = true;
	m_bTitleTips		= true;
    m_bAllowDraw		= true;
    m_nVScrollMax		= 0;
	m_bDoubleBuffer		= true;
	m_MouseMode			= MOUSE_NOTHING;
    m_bMouseButtonDown	= false;
	m_bMouseLDblClick	= false;

	m_bFocusOnSelected	= false;
    // Initially use the system message font for the GridCtrl font
    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0));
    memcpy(&m_Logfont, &(ncm.lfMessageFont), sizeof(LOGFONT));
				
    m_pImageList		= 0;

    m_nVTimerID			= 0;          // For drag-selection
    m_nMTimerID			= 0;          // For drag-selection
    m_nTimerInterval	= 120;         // (in milliseconds)

    m_pRtcItem			= RUNTIME_CLASS(CContextItem);
    m_pRtcHeader		= RUNTIME_CLASS(CContextItemHeader);       

	m_list.RemoveAll();
	m_listSelected.RemoveAll();
	m_listVisible.RemoveAll();
	m_listNumeric.RemoveAll();
}

CContextCtrl::~CContextCtrl()
{
    DeleteAllItems();
    m_TitleTip.DestroyWindow();

    DestroyWindow();

    m_Font.DeleteObject();
    m_BoldFont.DeleteObject();
/*
    COleDataSource * pSource = COleDataSource::GetClipboardOwner();
    if (pSource)
        COleDataSource::FlushClipboard();
*/
    // Uninitialize OLE support
    if (m_bMustUninitOLE)
        ::OleUninitialize();
}


BOOL	CContextCtrl::RegisterWindowClass()
{
    WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetInstanceHandle();

    if (!(::GetClassInfo(hInst, CONTEXTCTRL_CLASSNAME, &wndcls)))
    {
        // otherwise we need to register a new class
        wndcls.style            = CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW;//|CS_OWNDC;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = CONTEXTCTRL_CLASSNAME;

        if (!AfxRegisterClass(&wndcls))
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}

BOOL CContextCtrl::Create(const RECT& rect, CWnd* parent, UINT nID, DWORD dwStyle)
{
    ASSERT(parent->GetSafeHwnd());

    if (!CWnd::Create(CONTEXTCTRL_CLASSNAME, NULL, dwStyle, rect, parent, nID))
        return FALSE;

//	VERIFY(CreateUpDownBmp());
    HFONT hFont = ::CreateFontIndirect(&m_Logfont);
    OnSetFont((LPARAM)hFont, 0);
    DeleteObject(hFont);

    // Create titletips
    if (m_bTitleTips && !IsWindow(m_TitleTip.m_hWnd))
        m_TitleTip.Create(this);

//	EnableTitleTips(m_bTitleTips);
        
	EnableScrollBars(SB_VERT, TRUE);

	m_nMTimerID = SetTimer(WM_MOUSEMOVE, m_nTimerInterval, 0);

    return TRUE;
}

LRESULT CContextCtrl::SendMessageToParent(HCONTEXTITEM hItemNew, HCONTEXTITEM hItemOld, int nMessage, CPoint pt)
{
    if (!IsWindow(m_hWnd))
        return 0;

    NMCONTEXTVIEW nmgv;

	nmgv.hdr.hwndFrom = m_hWnd;
    nmgv.hdr.idFrom	  = GetDlgCtrlID();
    nmgv.hdr.code     = nMessage;
	
	nmgv.itemNew = hItemNew;
	nmgv.itemOld = hItemOld;

	nmgv.action	= nMessage;

	nmgv.point.x = pt.x;
	nmgv.point.y = pt.y;

    CWnd *pOwner = GetOwner();
    if (pOwner && IsWindow(pOwner->m_hWnd))
        return pOwner->SendMessage(WM_NOTIFY, nmgv.hdr.idFrom, (LPARAM)&nmgv);
    else
        return 0;
}

LRESULT CContextCtrl::SendMessageToParentDisp(CVITEM * pItem, int nMessage)
{
    if (!IsWindow(m_hWnd) || !pItem)
        return 0;

    // Send Notification to parent
    NMCVDISPINFO dispinfo;

    dispinfo.hdr.hwndFrom = GetSafeHwnd();
    dispinfo.hdr.idFrom   = GetDlgCtrlID();
    dispinfo.hdr.code     = nMessage;

	CopyCVITEM(pItem, &dispinfo.item);

    CWnd* pOwner = GetOwner();
    if (pOwner && IsWindow(pOwner->m_hWnd))
        return pOwner->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&dispinfo);

    return 0;
}

void CContextCtrl::CopyCVITEM(CVITEM * pFrom, CVITEM * pTo)
{
	if (!pFrom || !pTo)
		return;

	pTo->mask		= pFrom->mask;
	pTo->strText	= pFrom->strText;
	pTo->hItem		= pFrom->hItem;
	pTo->nState		= pFrom->nState;
	pTo->nImage		= pFrom->nImage;
	pTo->nFormat	= pFrom->nFormat;
	pTo->nHeight	= pFrom->nHeight;
	pTo->lParam		= pFrom->lParam;
	pTo->crFgClr	= pFrom->crFgClr;
	pTo->crBkClr	= pFrom->crBkClr;
}



BEGIN_MESSAGE_MAP(CContextCtrl, CWnd)
	//{{AFX_MSG_MAP(CContextCtrl)
    ON_WM_PAINT()
    ON_WM_VSCROLL()
    ON_WM_SIZE()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_TIMER()
    ON_WM_GETDLGCODE()
    ON_WM_KEYDOWN()
    ON_WM_CHAR()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_ERASEBKGND()
    ON_WM_SYSKEYDOWN()
	ON_WM_CANCELMODE()
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_WM_ACTIVATEAPP()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
    ON_WM_RBUTTONDOWN()
//    ON_WM_RBUTTONUP()
    ON_WM_SETCURSOR()
    ON_WM_RBUTTONUP()
    ON_WM_SYSCOLORCHANGE()
    ON_WM_CAPTURECHANGED()
    ON_WM_SETTINGCHANGE()
    ON_WM_MOUSEWHEEL()
    ON_MESSAGE(WM_SETFONT, OnSetFont)
    ON_MESSAGE(WM_GETFONT, OnGetFont)
    ON_NOTIFY(CVN_ENDLABELEDIT, IDC_INPLACE_CONTROL, OnEndInPlaceEdit)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnNeedToolTipText)
END_MESSAGE_MAP()


// Attributes
void CContextCtrl::EnableTitleTips(BOOL bEnable)
{	
	m_bTitleTips = bEnable;
//	EnableToolTips(m_bTitleTips);
}

INT_PTR CContextCtrl::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
    if (m_bTitleTips)
		return -1;

	CContextCtrl * pTree = (CContextCtrl*)(this);
	UINT uFlag = 0;
	HCONTEXTITEM hItem = pTree->HitTest(point, &uFlag);
	if (!hItem)
		return -1;

	CRect ItemRect(0, 0, 0, 0);
	if (hItem && pTree->GetItemRect(hItem, &ItemRect) && !ItemRect.IsRectEmpty())
	{
        // Titletips anyone? anyone?
        CRect TextRect = ItemRect;
        if (!hItem->IsEditing() && hItem->GetTipTextRect(&TextRect) && !TextRect.IsRectEmpty())
        {
			// return positive hit if control ID isn't -1
			int nHit = ::GetWindowLong(GetSafeHwnd(), GWL_ID);

			// hits against child windows always center the tip
			if (pTI != NULL)
			{
				// setup the TOOLINFO structure
				pTI->hwnd = m_hWnd;
				pTI->uId = (UINT)m_hWnd;
				pTI->uFlags |= TTF_NOTBUTTON|TTF_CENTERTIP;
				pTI->lpszText = LPSTR_TEXTCALLBACK;

			}
			return nHit;
		}
	}
	return -1;
}

void CContextCtrl::SetIndent(int nIndent)
{
	bool bRedraw = m_nIndent != nIndent;
	m_nIndent = nIndent;
	if (bRedraw)
		InvalidateRect(NULL);
}

void CContextCtrl::SetMargin(int nMargin)
{
	bool bRedraw = m_nMargin != nMargin;
	m_nMargin = nMargin;
	m_nBorder = m_nBorder < m_nMargin ? m_nBorder : m_nMargin;
	if (bRedraw)
	{
		int nPrevItemHeight = m_nDefItemHeight;
        m_nDefItemHeight = m_nIconSize + 2*m_nExtSize + 2*m_nMargin + 2*m_nBorder;

		double fRatioH = nPrevItemHeight ? (double)m_nDefItemHeight / (double)nPrevItemHeight : 1.;

		for (POSITION pos = m_list.GetHeadPosition(); pos != NULL; )
		{
			HCONTEXTITEM hItem = m_list.GetNext(pos);
			if (!ItemIsHeader(hItem))
			{
				int nNewHeight = (int)(hItem->GetHeight() * fRatioH);
					
				if (nNewHeight < m_nDefItemHeight)
					nNewHeight = m_nDefItemHeight;

				hItem->SetHeight(nNewHeight);
			}
		}
		InvalidateRect(NULL);
	}
}

void CContextCtrl::SetBorder(int nBorder)
{
	nBorder = nBorder < m_nMargin ? nBorder : m_nMargin;
	bool bRedraw = m_nBorder != nBorder;
	m_nBorder = nBorder;
	if (bRedraw)
		InvalidateRect(NULL);
}

void CContextCtrl::SetIconSize(int nIconSize)
{
	bool bRedraw = m_nIconSize != nIconSize;
	m_nIconSize = nIconSize;

	if (bRedraw)
	{
		int nPrevItemHeight = m_nDefItemHeight;
        m_nDefItemHeight = m_nIconSize + 2*m_nExtSize + 2*m_nMargin + 2*m_nBorder;

		double fRatioH = nPrevItemHeight ? (double)m_nDefItemHeight / (double)nPrevItemHeight : 1.;

		for (POSITION pos = m_list.GetHeadPosition(); pos != NULL; )
		{
			HCONTEXTITEM hItem = m_list.GetNext(pos);
			if (!ItemIsHeader(hItem))
			{
				int nNewHeight = m_nDefItemHeight;
				if (hItem->GetImage() < 0)
				{
					nNewHeight = (int)(hItem->GetHeight() * fRatioH);
					
					if (nNewHeight < m_nDefItemHeight)
						nNewHeight = m_nDefItemHeight;
				}
				hItem->SetHeight(nNewHeight);
			}
		}
		InvalidateRect(NULL);
	}
}

void CContextCtrl::SetNumberOffset(int nOffset)
{
	bool bRedraw = m_nNumOffset != nOffset;
	m_nNumOffset = nOffset;

	if (bRedraw)
		InvalidateRect(NULL);
}

void CContextCtrl::SetImageOffset(int nOffset)
{
	bool bRedraw = m_nImageOffset != nOffset;
	m_nImageOffset = nOffset;

	if (bRedraw)
		InvalidateRect(NULL);
}

void CContextCtrl::SetExtSize(int nExtSize)
{
	bool bRedraw = m_nExtSize != nExtSize;

	if (bRedraw)
	{
		int nPrevExtSize = m_nExtSize;
		m_nExtSize = nExtSize;

		int nPrevItemHeight = m_nDefItemHeight;
		int nPrevHeaderHeight = m_nDefHeaderHeight;

		m_nDefHeaderHeight += m_nExtSize - nPrevExtSize;
        m_nDefItemHeight = m_nIconSize + 2*m_nExtSize + 2*m_nMargin + 2*m_nBorder;

		double fRatioI = nPrevItemHeight ? (double)m_nDefItemHeight / (double)nPrevItemHeight : 1.;
		double fRatioH = nPrevHeaderHeight ? (double)m_nDefHeaderHeight / (double)nPrevHeaderHeight : 1.;

		for (POSITION pos = m_list.GetHeadPosition(); pos != NULL; )
		{
			HCONTEXTITEM hItem = m_list.GetNext(pos);
			int nNewHeight = -1;
			if (ItemIsHeader(hItem))
			{
				nNewHeight = (int)(hItem->GetHeight() * fRatioH);
				if (nNewHeight < m_nDefHeaderHeight)
					nNewHeight = m_nDefHeaderHeight;
			}
			else
			{
				nNewHeight = (int)(hItem->GetHeight() * fRatioI);
				if (nNewHeight < m_nDefItemHeight)
					nNewHeight = m_nDefItemHeight;
			}
			hItem->SetHeight(nNewHeight);
		}
		InvalidateRect(NULL);
	}
}

BOOL CContextCtrl::IsItemEditing(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
    return hItem ? hItem->IsEditing() : false;
}

// check item is visible
BOOL CContextCtrl::IsItemVisible(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	if (!hItem || !IsWindow(m_hWnd))
		return false;

	bool bFound = false;
	HCONTEXTITEM hLastVisItem = GetLastVisibleItem();
	// from first visible to last visible
	for (POSITION pos = m_listVisible.Find(GetFirstVisibleItem()); pos != NULL; )
	{
		HCONTEXTITEM nCmp = m_listVisible.GetNext(pos);
		if (nCmp == hItem)
		{
			bFound = true;
			break;
		}

		if (nCmp == hLastVisItem)
			break;
	}
    
    return bFound;
}

BOOL CContextCtrl::IsValid(HCONTEXTITEM hItem)
{
	return hItem != NULL;
}

BOOL CContextCtrl::IsItemSelected(HCONTEXTITEM hItem)
{
	if (ItemIsHeader(hItem))
		return false;
	return m_listSelected.Find(hItem) != NULL;
}

BOOL CContextCtrl::IsItemEditable(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	if (!hItem)
		return false;
    return IsEditable() && ((hItem->GetState() & CVIS_READONLY) != CVIS_READONLY);
}

BOOL CContextCtrl::GetTextRect(HCONTEXTITEM hItem, LPRECT pRect)
{
	ASSERT(hItem);
	if (!hItem || !pRect)
		return false;

	return hItem->GetTextRect(pRect);
}

CSize CContextCtrl::GetTextExtent(HCONTEXTITEM hItem, LPCTSTR str)
{
	ASSERT(hItem);
	if (!hItem)
		return CSize(0, 0);

	return hItem->GetTextExtent(str);
}

HCONTEXTITEM CContextCtrl::GetTrackedItem()
{
	return m_bEnableTracking ? m_TrackedItem : 0;
}

HCONTEXTITEM CContextCtrl::GetNextItem(HCONTEXTITEM hItem, UINT nCode)
{
	HCONTEXTITEM hRetItem = 0;
	switch (nCode)
	{
	case CVNI_FOCUSED:
		hRetItem = GetItemFocus();
		break;

	case CVNI_TRACKED:
		hRetItem = GetTrackedItem();
		break;

	case CVNI_ROOT:
		hRetItem = GetRootItem();
		break;

	case CVNI_NEXT:
		hRetItem = GetNextSiblingItem(hItem);
		break;

	case CVNI_PREVIOUS:
		hRetItem = GetPrevSiblingItem(hItem);
		break;

	case CVNI_PARENT:
		hRetItem = GetParentItem(hItem);
		break;

	case CVNI_CHILD:
		hRetItem = GetChildItem(hItem);
		break;

	case CVNI_FIRSTVISIBLE:
		hRetItem = GetFirstVisibleItem();
		break;
	
	case CVNI_LASTVISIBLE:
		hRetItem = GetLastVisibleItem();
		break;

	case CVNI_NEXTVISIBLE:
		hRetItem = GetNextVisibleItem(hItem);
		break;

	case CVNI_PREVIOUSVISIBLE:
		hRetItem = GetPrevVisibleItem(hItem);
		break;

	case CVNI_SELECTED:
		hRetItem = GetSelectedItem(hItem);
		break;

	case CVNI_EXPANDED:
	case CVNI_DROPHILITED:
	case CVNI_READONLY:
//	case CVNI_BOLD:
		break;

	}

	return hRetItem;
}

HCONTEXTITEM CContextCtrl::GetChildItem(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	HCONTEXTITEM hChild = 0;
	if (!hItem || !hItem->HasChildren())
		return 0;

	return hItem->GetChild(hItem->GetFirstPos());
}

HCONTEXTITEM CContextCtrl::GetNextSiblingItem(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	if (!hItem)
		return 0;

	HCONTEXTITEM hParent = hItem->GetParent();
	if (!hParent)
		return 0;

	return hParent->GetNext(hItem);
}

HCONTEXTITEM CContextCtrl::GetPrevSiblingItem(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	if (!hItem)
		return 0;

	HCONTEXTITEM hParent = hItem->GetParent();
	if (!hParent)
		return 0;

	return hParent->GetPrev(hItem);
}

HCONTEXTITEM CContextCtrl::GetParentItem(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	return hItem ? hItem->GetParent() : 0;
}

HCONTEXTITEM CContextCtrl::GetRootItem()
{
	return m_RootItem;
}

// get item's properties
BOOL CContextCtrl::GetItem(CVITEM* pItem)
{
	ASSERT(pItem);
	if (!pItem)
		return false;

	HCONTEXTITEM hItem = pItem->hItem;
	ASSERT(hItem);
	if (!hItem)
		return false;

	return hItem->ToCVITEM(pItem);
}

CString	  CContextCtrl::GetItemText(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	return hItem ? hItem->GetText() : "";
}

int	CContextCtrl::GetItemImage(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	return hItem ? hItem->GetImage() : -1;
}

UINT CContextCtrl::GetItemState(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	return hItem ? hItem->GetState() : 0;
}

UINT CContextCtrl::GetItemFormat(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	return hItem ? hItem->GetFormat() : 0;
}

SHORT CContextCtrl::GetItemHeight(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	if (!hItem || hItem == GetRootItem())
		return 0;

	SHORT nHeight = hItem->GetHeight();
	if (!nHeight)
		nHeight = ItemIsHeader(hItem) ? GetDefHeaderHeight() : GetDefItemHeight();
	return nHeight;
}

COLORREF CContextCtrl::GetItemTextColor(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	return hItem ? hItem->GetTextClr() : CLR_DEFAULT;
}

COLORREF CContextCtrl::GetItemBkColor(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	return hItem ? hItem->GetBackClr() : CLR_DEFAULT;
}

DWORD CContextCtrl::GetItemData(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	return hItem ? hItem->GetData() : 0;
}

BOOL CContextCtrl::GetItemRect(HCONTEXTITEM hItem, LPRECT lpRect)
{
	ASSERT(hItem);
	if (!hItem || !lpRect || !::IsWindow(GetSafeHwnd()))
		return false;

	memset(lpRect, 0, sizeof(RECT));
	CPoint pt;
	if (!GetItemOrigin(hItem, &pt))
		return false;

	CRect ClientRect;
	GetClientRect(ClientRect);

	lpRect->left = ClientRect.left;
	lpRect->right = ClientRect.right - 1;
	lpRect->top = pt.y;
	lpRect->bottom = lpRect->top + GetItemHeight(hItem) - 1;

	return true;
}

// set item's properties
BOOL  CContextCtrl::SetItem(CVITEM* pItem)
{
	if (!pItem || !pItem->hItem)
		return false;

	return pItem->hItem->FromCVITEM(pItem);
}

BOOL CContextCtrl::SetItem(HCONTEXTITEM hItem, UINT nMask, LPCTSTR lpszItem, int nImage, UINT nState, 
				  UINT nFormat, LPARAM lParam, UINT	nHeight, COLORREF clrBack, COLORREF clrText)
{
	CVITEM cv;

	cv.mask = nMask;//CVIF_TEXT|CVIF_IMAGE|CVIF_PARAM|CVIF_STATE|CVIF_BKCLR|CVIF_FGCLR|CVIF_FORMAT|CVIF_HEIGHT;
	cv.hItem = hItem;
	cv.strText = lpszItem;
	cv.nFormat = nFormat;
	cv.nImage = nImage;
	cv.nState = nState;
	cv.lParam = lParam;
	cv.nHeight = nHeight;
	cv.crBkClr = clrBack;
	cv.crFgClr = clrText;

	return SetItem(&cv);
}

BOOL CContextCtrl::SetItemText(HCONTEXTITEM hItem, LPCTSTR lpszItem)
{
	ASSERT(hItem);
	if (!hItem)
		return false;

	hItem->SetText(lpszItem);
	hItem->Update(CVIF_TEXT);
	return true;
}

BOOL CContextCtrl::SetItemImage(HCONTEXTITEM hItem, int nImage)
{
	ASSERT(hItem);
	if (!hItem)
		return false;

	hItem->SetImage(nImage);
	hItem->Update(CVIF_IMAGE);
	return true;
}

BOOL CContextCtrl::SetItemState(HCONTEXTITEM hItem, UINT nState)
{
	ASSERT(hItem);
	if (!hItem)
		return false;

	hItem->SetState(nState);
	hItem->Update(CVIF_STATE);
	return true;
}

UINT CContextCtrl::SetItemFormat(HCONTEXTITEM hItem, UINT nFormat)
{
	ASSERT(hItem);
	if (!hItem)
		return 0;

	UINT nPrev = hItem->GetFormat();
	hItem->SetFormat(nFormat);
	hItem->Update(CVIF_FORMAT);
	return nPrev;
}

SHORT CContextCtrl::SetItemHeight(HCONTEXTITEM hItem, SHORT cyHeight)
{
	ASSERT(hItem);
	if (!hItem)
		return -1;

	UINT nPrev = hItem->GetHeight();
	hItem->SetHeight(cyHeight);
	hItem->Update(CVIF_HEIGHT);
	return nPrev;
}

COLORREF CContextCtrl::SetItemTextColor(HCONTEXTITEM hItem, COLORREF clr)
{
	ASSERT(hItem);
	if (!hItem)
		return CLR_DEFAULT;

	COLORREF clPrev = hItem->GetTextClr();
	hItem->SetTextClr(clr);
	hItem->Update(CVIF_FGCLR);
	return clPrev;
}

COLORREF CContextCtrl::SetItemBkColor(HCONTEXTITEM hItem, COLORREF clr)
{
	ASSERT(hItem);
	if (!hItem)
		return CLR_DEFAULT;

	COLORREF clPrev = hItem->GetBackClr();
	hItem->SetBackClr(clr);
	hItem->Update(CVIF_BKCLR);
	return clPrev;
}

BOOL CContextCtrl::SetItemData(HCONTEXTITEM hItem, DWORD dwData)
{
	ASSERT(hItem);
	if (!hItem)
		return false;

	hItem->SetData((LPARAM)dwData);
	hItem->Update(CVIF_PARAM);
	return true;
}

BOOL CContextCtrl::ItemHasChildren(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	if (!hItem)
		return false;

	return hItem->HasChildren();
}

BOOL CContextCtrl::ItemIsHeader(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	if (!hItem)
		return false;

	return hItem->GetParent() == GetRootItem();
}

// functions for direct access to context items 
// for inner use only
UINT CContextCtrl::GetCount()
{
	return m_list.GetCount();
}

POSITION CContextCtrl::GetItemPos(HCONTEXTITEM hItem)
{
	ASSERT(!hItem);
	if (!hItem)
		return 0;
	return m_list.Find(hItem);
}

POSITION CContextCtrl::GetFirstItemPos()
{
	return m_list.GetHeadPosition();
}

POSITION CContextCtrl::GetLastItemPos()
{
	return m_list.GetTailPosition();
}

POSITION CContextCtrl::GetNextPos(POSITION pos)
{
	ASSERT(pos);
	m_list.GetNext(pos);
	return pos;
}

POSITION CContextCtrl::GetPrevPos(POSITION pos)
{
	ASSERT(pos);
	m_list.GetPrev(pos);
	return pos;
}

HCONTEXTITEM CContextCtrl::GetItem(POSITION pos)
{
	ASSERT(pos);
	return m_list.GetNext(pos);
}

// Operations
// return only vertical scroll bar
CScrollBar* CContextCtrl::GetScrollBarCtrl(int nBar)
{
	return (nBar == SB_VERT) ? CWnd::GetScrollBarCtrl(SB_VERT) : 0;
}

HCONTEXTITEM CContextCtrl::InsertItem(LPCVINSERTSTRUCT lpInsertStruct)
{
	ASSERT(lpInsertStruct);
	if (!lpInsertStruct || !lpInsertStruct->hInsertAfter)
		return 0;

	HCONTEXTITEM hParent = lpInsertStruct->hParent;
	if (hParent == CVI_ROOT || !hParent)
		hParent = GetRootItem();

	HCONTEXTITEM hAfter = lpInsertStruct->hInsertAfter;
	if (!hAfter)
		hAfter = CVI_LAST;

	CContextItemBase * pItem = (hParent == GetRootItem()) ? (CContextItemBase *)m_pRtcHeader->CreateObject() : 
															(CContextItemBase *)m_pRtcItem->CreateObject();
	if (!pItem)
		return false;

	if (!pItem->FromCVITEM(&lpInsertStruct->item))
	{
		delete pItem;
		return 0;
	}
	pItem->SetTree(this);
/*
	if (pItem->GetHeight() == 0)
	{
		pItem->SetHeight(hParent == GetRootItem() ? GetDefHeaderHeight() : 
						 (pItem->GetImage() < 0)  ? GetDefItemHeight()   : 
													GetDefSmallItemHeight());
	}
*/
	if (!hParent->InsertChild(pItem, hAfter))
	{
		delete pItem;
		return 0;
	}
	m_list.AddTail(pItem);

	pItem->SetExpand(hParent != GetRootItem());

	RecalcVisible();

	return pItem;
}

HCONTEXTITEM CContextCtrl::InsertItem(LPCTSTR lpszItem, int nImage, UINT nState, UINT nFormat,
						UINT nHeight, COLORREF clrBack, COLORREF clrText, LPARAM lParam,
						HCONTEXTITEM hParent, HCONTEXTITEM hInsertAfter)
{
	CVINSERTSTRUCT insert;
	insert.hInsertAfter = hInsertAfter;
	insert.hParent		= hParent;

	insert.item.mask	= 0;
    if (lpszItem)
	{
		insert.item.mask	= CVIF_TEXT;
		insert.item.strText = lpszItem;
	}
	
	if (nImage != -1)
	{
		insert.item.mask   |= CVIF_IMAGE;
		insert.item.nImage	= nImage;
	}

	if (nState)
	{
		insert.item.mask   |= CVIF_STATE;
		insert.item.nState	= nState;
	}
	
	if (nFormat)
	{
		insert.item.mask   |= CVIF_FORMAT;
		insert.item.nFormat	= nFormat;
	}
	
	if (clrBack != CLR_DEFAULT)
	{
	    insert.item.mask   |= CVIF_BKCLR;
		insert.item.crBkClr	= clrBack;
	}
    
	if (clrText != CLR_DEFAULT)
	{
		insert.item.mask   |= CVIF_FGCLR;
		insert.item.crFgClr	= clrText;
	}
    
	if (lParam)
	{
		insert.item.mask   |= CVIF_PARAM;
		insert.item.lParam	= lParam;
	}
    
	if (nHeight)
	{
		insert.item.mask   |= CVIF_HEIGHT;
		insert.item.nHeight	= nHeight;
	}
	return InsertItem(&insert);

}

HCONTEXTITEM CContextCtrl::InsertItem(LPCTSTR lpszItem, HCONTEXTITEM hParent, HCONTEXTITEM hInsertAfter)
{
	CVINSERTSTRUCT insert;
	insert.hInsertAfter = hInsertAfter;
	insert.hParent		= hParent;

	insert.item.mask	= CVIF_TEXT;
	insert.item.strText = lpszItem;
	
	return InsertItem(&insert);
}

// remove item and item's all children recursively
BOOL CContextCtrl::DeleteItem(HCONTEXTITEM hItem)
{
    static bool bAlreadyInsideDeleteItem = false;

	ASSERT(hItem);
	if (!hItem)
		return true;

	// if item is root item
	if (hItem == GetRootItem())
		return DeleteAllItems();

	// try to find position of Context_item
	POSITION pos = m_list.Find(hItem);
	if (!pos)
		return false;

	// check we already in this function recursively
	bool bPrevState = bAlreadyInsideDeleteItem;
	bAlreadyInsideDeleteItem = true;

	// remove item from selected (if any)
	bool bRet = SelectItem(hItem, false) == TRUE;
//	SetItemNumber(hItem, false);

	// remove focus item
	if (hItem == GetItemFocus())
		SetItemFocus(0);

	if (hItem == m_hPrevFocusItem)
		m_hPrevFocusItem;

	// remove tracked item
	if (m_TrackedItem == hItem)
		m_TrackedItem = 0;

	// if item has children
	if (hItem->GetChildrenCount())
	{
		// for all children fomr last
		for (POSITION posChild = hItem->GetFirstPos(); posChild != NULL; )
		{	
			// Get Child
			HCONTEXTITEM hChild = hItem->GetChild(posChild);
			// and goto next pos
			posChild = hItem->GetNextPos(posChild);


/* this code will be work in child item destructor
			// get next child pos
			POSITION posChildNext = hItem->GetNextPos(posChild);

			// remove this child from our item
			hItem->RemoveChild(posChild);

			// restore next child's pos
			posChild = posChildNext;
*/
			// and delete child's item
			if (hChild)
				bRet = (DeleteItem(hChild) == TRUE) && bRet;
		}
	}

	SendMessageToParent(hItem, 0, CVN_DELETEITEM);
	// delete item
	delete hItem;

	// and remove item's pos from list
	m_list.RemoveAt(pos);

	// restore flag from previous
	bAlreadyInsideDeleteItem = bPrevState;

	// if first time call of this function
    if (!bAlreadyInsideDeleteItem)
	{
		RecalcVisible();
		ResetScrollBars();
		InvalidateRect(NULL);
	}

	return bRet;
}

// remove all items
BOOL CContextCtrl::DeleteAllItems()
{
	// remove focus item
	SetItemFocus(0);
	m_hPrevFocusItem = 0;
	// remove tracked item
	m_TrackedItem = 0;
	// remove selected items;
	m_listSelected.RemoveAll();
	// remove VisibleItems
	m_listVisible.RemoveAll();
	// remove numeric items
	m_listNumeric.RemoveAll();

	BOOL bRet = true;
	HCONTEXTITEM hRootItem = GetRootItem();
	if (hRootItem && hRootItem->HasChildren())
	{
		// for all children in root
		for (POSITION posChild = hRootItem->GetFirstPos(); posChild != NULL; )
		{	
			// Get Child
			HCONTEXTITEM hChild = hRootItem->GetChild(posChild);
			// and goto next pos
			posChild = hRootItem->GetNextPos(posChild);

			// and delete Child if any
			if (hChild)
				bRet = (DeleteItem(hChild) == TRUE) && bRet;
		}
	}

	// Clear root's children list 
	ASSERT(!hRootItem->HasChildren());
	if (hRootItem->HasChildren())
		m_RootItem.RemoveAllChildren();

	// Clear list 
	ASSERT(m_list.GetCount() == 0);
	if (m_list.GetCount())
		m_list.RemoveAll();

	// recalc visible items
	RecalcVisible();
	// reset scroll bar
	ResetScrollBars();
	
	if (::IsWindow(m_hWnd))
		InvalidateRect(NULL);

	return true;
}

BOOL CContextCtrl::MakeVisible(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	if (!hItem)
		return false;

	HCONTEXTITEM hParent = hItem->GetParent();
	// need make visible parent
	if (hParent != GetRootItem())
	{
		MakeVisible(hParent);
		Expand(hParent, CVE_EXPAND);
	}

	return true;
}

BOOL CContextCtrl::Expand(HCONTEXTITEM hItem, UINT nCode, BOOL bUpdate)
{
	ASSERT(hItem);
	if (!hItem)// || !IsItemVisible(hItem))
		return false;

	if (nCode == CVE_TOGGLE)
		nCode = hItem->IsExpanded() ? CVE_COLLAPSE : CVE_EXPAND;

	if (nCode == CVE_COLLAPSE && hItem->IsExpanded())
	{
		hItem->SetExpand(false);
	}
	else if (nCode == CVE_EXPAND && !hItem->IsExpanded())
	{
		hItem->SetExpand(true);
	}

	if (bUpdate)
	{	
		if (!RecalcVisible())
			return false;
	
		ResetScrollBars();
		InvalidateRect(NULL);
	}

	return true;
}

BOOL CContextCtrl::ExpandAll(UINT nCode, BOOL bUpdate)
{
	BOOL bRet = true;
	
	for (POSITION pos = m_RootItem.GetFirstPos(); pos != NULL; )
	{
		HCONTEXTITEM hItem = m_RootItem.GetChild(pos);
		pos = m_RootItem.GetNextPos(pos);

		if (hItem)
			bRet = Expand(hItem, nCode, false) && bRet;

	}
/*
	for (POSITION pos = m_list.GetHeadPosition(); pos != NULL; )
	{
		HCONTEXTITEM hItem = m_list.GetNext(pos);
		if (hItem && hItem->HasChildren())//ItemIsHeader(hItem))
			bRet = Expand(hItem, nCode, false) && bRet;
	}
*/
	if (bUpdate)
	{
		if (!RecalcVisible())
			return false;

		ResetScrollBars();
		InvalidateRect(NULL);
	}

	return true;
}

// visible items
BOOL CContextCtrl::RecalcVisible()
{
	if (!m_bAllowDraw || !::IsWindow(GetSafeHwnd()))
		return true;
	m_listVisible.RemoveAll();
	m_FirstVisibleItem = 0;
	m_LastVisibleItem = 0;

	HCONTEXTITEM hRoot = GetRootItem();
	
	return CalcVisible(hRoot, m_listVisible);
}

// recursive function to fill visible list  from item
// if item has expanded parent (or root) itmust be added
// and if item is expanded and have children we need to add its children to list
bool CContextCtrl::CalcVisible(HCONTEXTITEM hRoot, CItemList & itemlist)
{
	bool bRet = true;

	// if has no children  -> return true
	if (!hRoot || !hRoot->GetChildrenCount())
		return true;

	// if item's not expanded so children is not visible
	if (!hRoot->IsExpanded())
		return true;

	for (POSITION pos = hRoot->GetFirstPos(); pos != NULL; )
	{
		HCONTEXTITEM hChild = hRoot->GetChild(pos);
		if (!hChild)
			continue;

		// add child to list 
		itemlist.AddTail(hChild);
		
		// and try to add child's children too if need
		bRet = CalcVisible(hChild, itemlist) && bRet;

		// goto next child position
		pos = hRoot->GetNextPos(pos);
	}
	
	return bRet;
}


// return first visible item
HCONTEXTITEM CContextCtrl::GetFirstVisibleItem()
{
	if (!m_listVisible.GetCount())
		return 0;

    int nVertScroll = GetScrollPos(SB_VERT);

    HCONTEXTITEM hItem = 0;
	int nTop = 0;
	for (POSITION pos = m_listVisible.GetHeadPosition(); pos != NULL; )
	{
		hItem = m_listVisible.GetNext(pos);
		nTop += GetItemHeight(hItem);
		if (nTop >= nVertScroll)
			break;
	}
	return m_FirstVisibleItem = hItem;
}

// return last visible item
HCONTEXTITEM CContextCtrl::GetLastVisibleItem()
{
	if (!m_listVisible.GetCount() || !::IsWindow(GetSafeHwnd()))
		return 0;

	CRect rect;
	GetClientRect(rect);
    int nVertScroll = GetScrollPos(SB_VERT) + rect.Height();

    HCONTEXTITEM hItem = 0;
	int nTop = 0;
	for (POSITION pos = m_listVisible.GetHeadPosition(); pos != NULL && nTop < nVertScroll; )
	{
		hItem = m_listVisible.GetNext(pos);
		nTop += GetItemHeight(hItem);
	}
	if (!hItem)
		hItem = m_listVisible.GetTail();

	return m_LastVisibleItem = hItem;
}

// get next visible item if it's exists
// if param is null -> return first visible
HCONTEXTITEM CContextCtrl::GetNextVisibleItem(HCONTEXTITEM hItem)
{
	if (!m_listVisible.GetCount())
		return 0;

	HCONTEXTITEM hNextItem = m_listVisible.GetTail();
	// try to find item in selected list
	POSITION pos = m_listVisible.Find(hItem);
	// if item is founded
	if (pos)
	{
		// get next 
		m_listVisible.GetNext(pos);
		hNextItem = pos ? m_listVisible.GetNext(pos) : 0;
	}

	return hNextItem;
}

// get previous visible item if it's exists
// if param is null -> return last visible
HCONTEXTITEM CContextCtrl::GetPrevVisibleItem(HCONTEXTITEM hItem)
{
	if (!m_listVisible.GetCount())
		return 0;

	HCONTEXTITEM hPrevItem = m_listVisible.GetHead();
	// try to find item in visible list
	POSITION pos = m_listVisible.Find(hItem);
	// if item is founded
	if (pos)
	{
		// get Prev 
		m_listVisible.GetPrev(pos);
		hPrevItem = (pos) ? m_listVisible.GetPrev(pos) : 0;
	}

	return hPrevItem;
}

// return next visible header item
HCONTEXTITEM CContextCtrl::GetNextVisibleHeaderItem(HCONTEXTITEM hItem)
{
	if (!m_listVisible.GetCount())
		return 0;

	if (!hItem || !hItem->GetParent())
		return m_RootItem.GetFirst();

	// find header for given item
	if (!m_RootItem.IsChild(hItem))
	{
		while (hItem && hItem->GetParent() != GetRootItem())
			hItem = hItem->GetParent();
	}

	// now try to get next child for root
	HCONTEXTITEM hNext = m_RootItem.GetNext(hItem);
	if (!hNext)
		hNext = m_listVisible.GetTail();

	return hNext;
}

// return Prev visible header item
HCONTEXTITEM CContextCtrl::GetPrevVisibleHeaderItem(HCONTEXTITEM hItem)
{
	if (!m_listVisible.GetCount())
		return 0;

	if (!hItem || !hItem->GetParent())
		return m_RootItem.GetFirst();

	// find header for given item
	if (!m_RootItem.IsChild(hItem))
	{
		while (hItem && hItem->GetParent() != GetRootItem())
			hItem = hItem->GetParent();
		return hItem;
	}

	// now try to get next child for root
	HCONTEXTITEM hPrev = m_RootItem.GetPrev(hItem);
	if (!hPrev)
		hPrev = m_listVisible.GetHead();

	return hPrev;
}

// return count of visible items
UINT CContextCtrl::GetVisibleCount()
{
	int nCount = 0;
	HCONTEXTITEM hLastVis = GetLastVisibleItem();
	for (POSITION pos = m_listVisible.Find(GetFirstVisibleItem()); pos != NULL; )
	{
		HCONTEXTITEM hItem = m_listVisible.GetNext(pos);
		nCount++;

		if (hLastVis == hItem)
			break;
	}
	return nCount;
}

// fill array-parameter from visible items list
UINT CContextCtrl::GetVisibleItems(CItemList & rList, LPRECT lpRect)
{
	rList.RemoveAll();
	int nHeight = 0;

	if (lpRect)
		lpRect->left = lpRect->right = lpRect->top = lpRect->bottom = 0;

	HCONTEXTITEM hLastVis = GetLastVisibleItem();
	for (POSITION pos = m_listVisible.Find(GetFirstVisibleItem()); pos != NULL; )
	{
		HCONTEXTITEM hItem = m_listVisible.GetNext(pos);
		rList.AddTail(hItem);

		nHeight += GetItemHeight(hItem);
		if (hLastVis == hItem)
			break;
	}

	if (lpRect && ::IsWindow(GetSafeHwnd()) && rList.GetCount())
	{
		CRect rc;
		GetClientRect(rc);

		CPoint pt(0, 0);
		if (GetItemOrigin(rList.GetHead(), &pt))
			lpRect->top = min(pt.y, rc.top); 

		lpRect->left = rc.left; 
		lpRect->right = rc.right; 
		lpRect->bottom = lpRect->top + nHeight;

	}

	return rList.GetCount();
}

// get array of visible items;
// return -1 if we have any problem
UINT CContextCtrl::GetVisibleItems(HCONTEXTITEM * phItems, int nCount, LPRECT lpRect)
{
	bool bArr = phItems != NULL;
	// check params is valid
	if (!phItems || !nCount || (nCount && !AfxIsValidAddress((const void *)phItems, sizeof(HCONTEXTITEM) * nCount, true)))
		bArr = false;

	if (lpRect)
		lpRect->left = lpRect->right = lpRect->top = lpRect->bottom = 0;

	if (!m_listVisible.GetCount())
		return 0;

	if (bArr)
		memset((LPVOID)phItems, 0, sizeof(HCONTEXTITEM) * nCount);

	int nHeight = 0;
	int nCnt = 0;
	HCONTEXTITEM hLastVis = GetLastVisibleItem();
	
	for (POSITION pos = m_listVisible.Find(GetFirstVisibleItem()); pos != NULL; )
	{
		HCONTEXTITEM hItem = m_listVisible.GetNext(pos);
		if (bArr && nCnt < nCount)
			phItems[nCnt++] = hItem;

		nHeight += GetItemHeight(hItem);

		if (hLastVis == hItem)
			break;
	}

	if (lpRect && ::IsWindow(GetSafeHwnd()))
	{
		CRect rc;
		GetClientRect(rc);

		CPoint pt(0, 0);
		if (bArr && GetItemOrigin(phItems[0], &pt))
			lpRect->top = min(pt.y, rc.top); 
		else
			lpRect->top = rc.top; 

		lpRect->left = rc.left; 
		lpRect->right = rc.right; 
		lpRect->bottom = lpRect->top + nHeight;
	}

	return nCnt;
}

int CContextCtrl::GetVirtualHeight()
{
	if (!m_listVisible.GetCount())
		return 0;

	int nHeight = 0;
	for (POSITION pos = m_listVisible.GetHeadPosition(); pos != NULL; )
	{
		HCONTEXTITEM hItem = m_listVisible.GetNext(pos);
		if (hItem)
			nHeight += GetItemHeight(hItem);
	}
	return nHeight;
}

////////////////////////////////////////////////////////////////////////////////
// items selection

// select given item
BOOL  CContextCtrl::SelectItem(HCONTEXTITEM hItem, BOOL bSelect)
{
	ASSERT(hItem);
	if (!hItem)
		return false;

	bool bRedraw = false;
	POSITION pos = m_listSelected.Find(hItem);

	if (bSelect && !pos)
	{
		pos = m_listSelected.AddTail(hItem);
		hItem->SetState(hItem->GetState() | CVIS_SELECTED);

		bRedraw = true;
	}
	else if (!bSelect && pos)// && hItem != m_FocusItem)
	{
		m_listSelected.RemoveAt(pos);
		hItem->SetState(hItem->GetState() & ~CVIS_SELECTED);
//		if (hItem->IsKindOf(RUNTIME_CLASS(CContextItem)))
//		{
//			CContextItem * pItem = (CContextItem *)hItem;
//			pItem->SetNumber(-1);
//		}
		
		bRedraw = true;
	}
	
/*	if (bRedraw)
	{
		int nCount = 0;
		for (pos = m_listSelected.GetHeadPosition(); pos != NULL; )
		{
			HCONTEXTITEM hNextItem = m_listSelected.GetNext(pos);
			if (hNextItem && hNextItem->IsKindOf(RUNTIME_CLASS(CContextItem)))
			{
				CContextItem * pItem = (CContextItem *)hNextItem;
				pItem->SetNumber(++nCount);
				InvalidateItemRect(hNextItem);
			}
		}
	}

*/
	if (bRedraw)
		InvalidateItemRect(hItem);

	return true;
}
	
// return count of selected items
UINT CContextCtrl::GetSelectedCount()
{
	return m_listSelected.GetCount();
}

// get next selected item if it's exists
// if param is null -> return first selected
HCONTEXTITEM CContextCtrl::GetSelectedItem(HCONTEXTITEM hItem)
{
	if (!m_listSelected.GetCount())
		return 0;

	HCONTEXTITEM hNextItem = m_listSelected.GetHead();
	// try to find item in selected list
	POSITION pos = m_listSelected.Find(hItem);
	// if item is founded
	if (pos)
		// get next 
		hNextItem = m_listSelected.GetNext(pos);

	return hNextItem;
}

// get array of selected items;
// return -1 if we have any problem
UINT CContextCtrl::GetSelectedItems(HCONTEXTITEM * phItems, int nCount)
{
	// check params is valid
	if (!nCount || !AfxIsValidAddress((const void *)phItems, sizeof(HCONTEXTITEM) * nCount, true))
		return -1;

	int nCnt = (int)m_listSelected.GetCount();
	if (!nCnt)
		return 0;
	
	ASSERT (nCnt <= nCount);

	nCnt = 0;
	for (POSITION pos = m_listSelected.GetHeadPosition(); pos != NULL && nCnt < nCount; nCnt++)
		phItems[nCnt] = m_listSelected.GetNext(pos);

	return nCnt;
}

// fill array-parameter form selected items list
UINT CContextCtrl::GetSelectedItems(CItemList & rList)
{
	rList.RemoveAll();
	for (POSITION pos = m_listSelected.GetHeadPosition(); pos != NULL; )
		rList.AddTail(m_listSelected.GetNext(pos));
	return rList.GetCount();
}

// numeric

BOOL CContextCtrl::SetItemNumber(HCONTEXTITEM hItem, int nNumber)
{
	ASSERT(hItem);
	if (!hItem || !hItem->IsKindOf(RUNTIME_CLASS(CContextItem)))
		return false;

	CContextItem * pItem = (CContextItem*)hItem;
	if (!pItem)
		return false;

	DWORD dwState = hItem->GetState();
	pItem->SetNumber(nNumber);

	if (nNumber <= 0)
		dwState &= ~CVIS_NUMBERED;
	else
		dwState |= CVIS_NUMBERED;

	hItem->SetState(dwState);

	InvalidateItemRect(hItem);
	return true;
}

int CContextCtrl::GetItemNumber(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	if (!hItem || !hItem->IsKindOf(RUNTIME_CLASS(CContextItem)))
		return -1;

	if ((hItem->GetState() & CVIS_NUMBERED) != CVIS_NUMBERED)
		return -1;

	CContextItem * pItem = (CContextItem*)hItem;
	if (!pItem)
		return -1;

	return pItem->GetNumber();
}

BOOL CContextCtrl::ItemHasNumber(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	if (!hItem || !hItem->IsKindOf(RUNTIME_CLASS(CContextItem)))
		return false;
	
	if ((hItem->GetState() & CVIS_NUMBERED) == CVIS_NUMBERED)
		return false;

	CContextItem * pItem = (CContextItem*)hItem;
	return (!pItem || pItem->GetNumber() < 0) ? false : true;
}

/*
// GetFirst numeric item
HCONTEXTITEM CContextCtrl::GetFirstNumericItem()
{
	return m_listNumeric.GetHead();
}

// Get last numeric item
HCONTEXTITEM CContextCtrl::GetLastNumericItem()
{
	return m_listNumeric.GetTail();
}

// get next numeric item
HCONTEXTITEM CContextCtrl::GetNextNumericItem(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	if (!hItem || !m_listNumeric.GetCount())
		return 0;

	HCONTEXTITEM hNextItem = m_listNumeric.GetTail();
	// try to find item in selected list
	POSITION pos = m_listNumeric.Find(hItem);
	// if item is founded
	if (pos)
	{
		// get next 
		m_listNumeric.GetNext(pos);
		hNextItem = pos ? m_listNumeric.GetNext(pos) : 0;
	}

	return hNextItem;
}

// get prev numeric item
HCONTEXTITEM CContextCtrl::GetPrevNumericItem(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	if (!hItem || !m_listNumeric.GetCount())
		return 0;

	HCONTEXTITEM hNextItem = m_listNumeric.GetHead();
	// try to find item in selected list
	POSITION pos = m_listNumeric.Find(hItem);
	// if item is founded
	if (pos)
	{
		// get next 
		m_listNumeric.GetPrev(pos);
		hNextItem = pos ? m_listNumeric.GetNext(pos) : 0;
	}

	return hNextItem;
}

// add item to numeric items
BOOL CContextCtrl::SetItemNumber(HCONTEXTITEM hItem, BOOL bNumber, BOOL bFirst)
{
	ASSERT(hItem);
	if (!hItem || !hItem->IsKindOf(RUNTIME_CLASS(CContextItem)))
		return false;

	if (bNumber)
	{
		POSITION pos = m_listNumeric.Find(hItem);
		if (pos)
			m_listNumeric.RemoveAt(pos);

		if (bFirst)
			m_listNumeric.AddHead(hItem);
		else
			m_listNumeric.AddTail(hItem);

		RecalcNumber();
	}
	else
	{
		POSITION pos = m_listNumeric.Find(hItem);
		if (pos)
		{
			CContextItem * pItem = (CContextItem*)hItem;
			if (pItem)
				pItem->SetNumber(-1);
			m_listNumeric.RemoveAt(pos);
			InvalidateItemRect(hItem);

			RecalcNumber();
		}
	}
	return true;
}

// get item number or -1
int	CContextCtrl::GetItemNumber(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	if (!hItem || !hItem->IsKindOf(RUNTIME_CLASS(CContextItem)))
		return -1;

	int nCount = 1;
	for (POSITION pos = m_listNumeric.GetHeadPosition(); pos != NULL; )
	{
		HCONTEXTITEM hNextItem = m_listNumeric.GetNext(pos);
		if (hItem == hNextItem)
			return nCount;

		nCount++;
	}
	return -1;
}

BOOL CContextCtrl::ItemHasNumber(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	if (!hItem || !hItem->IsKindOf(RUNTIME_CLASS(CContextItem)))
		return false;
	
	return m_listNumeric.Find(hItem) != NULL;
}

// get count of numeric items
UINT CContextCtrl::GetNumericCount()
{
	return m_listNumeric.GetCount();
}

// get array of numeric items;
// return -1 if we have any problem
UINT CContextCtrl::GetNumericItems(HCONTEXTITEM * phItems, int nCount)
{
	// check params is valid
	if (!nCount || !AfxIsValidAddress((const void *)phItems, sizeof(HCONTEXTITEM) * nCount, true))
		return -1;

	int nCnt = m_listSelected.GetCount();
	if (!nCnt)
		return 0;
	
	ASSERT (nCnt <= nCount);

	nCnt = 0;
	for (POSITION pos = m_listNumeric.GetHeadPosition(); pos != NULL && nCnt < nCount; nCnt++)
		phItems[nCnt] = m_listNumeric.GetNext(pos);

	return nCnt;
}

// fill array-parameter from numeric items list
UINT CContextCtrl::GetNumericItems(CItemList & rList)
{
	rList.RemoveAll();
	for (POSITION pos = m_listNumeric.GetHeadPosition(); pos != NULL; )
		rList.AddTail(m_listNumeric.GetNext(pos));
	return rList.GetCount();
}

// Remove all numeric items
void CContextCtrl::RemoveAllNumeric()
{
	for (POSITION pos = m_listNumeric.GetHeadPosition(); pos != NULL; )
	{
		POSITION prevPos = pos;
		HCONTEXTITEM hItem = m_listNumeric.GetNext(pos);
		if (hItem && hItem->IsKindOf(RUNTIME_CLASS(CContextItem)) && hItem != m_FocusItem)
		{
			CContextItem * pItem = (CContextItem*)hItem;
			pItem->SetNumber(-1);
			m_listNumeric.RemoveAt(prevPos);
		}
	}
	RecalcNumber();
	if (::IsWindow(GetSafeHwnd()))
		InvalidateRect(NULL);
}

void CContextCtrl::RecalcNumber()
{
	int nCount = 1;
	CRect rect(0, 0, 0, 0);
	for (POSITION pos = m_listNumeric.GetHeadPosition(); pos != NULL; )
	{
		HCONTEXTITEM hItem = m_listNumeric.GetNext(pos);
		if (hItem && hItem->IsKindOf(RUNTIME_CLASS(CContextItem)))
		{
			CContextItem * pItem = (CContextItem*)hItem;
			pItem->SetNumber(nCount);
			nCount++;
			CRect itemRect;
			if (GetItemRect(hItem, itemRect))
				rect.UnionRect(rect, itemRect);
		}
	}
	if (::IsWindow(GetSafeHwnd()))
		InvalidateRect(rect);
}
*/


// ensure visible
BOOL CContextCtrl::EnsureVisible(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	if (!hItem)
		return false;

	if (!m_listVisible.Find(hItem))
		MakeVisible(hItem);

	HCONTEXTITEM hFirstVisItem = GetFirstVisibleItem();
	HCONTEXTITEM hLastVisItem = GetLastVisibleItem();
	CPoint ptItem, ptLast, ptFirst = CPoint(0 , 0);

	if (!GetItemOrigin(hItem, &ptItem))
		return false;

	if (!GetItemOrigin(hLastVisItem, &ptLast))
		return false;
	
	int nOffset = 0;
	if (ptItem.y < ptFirst.y)
	{
		nOffset = ptItem.y - ptFirst.y;
	}
	else if (ptItem.y >= ptLast.y)
	{
		CRect rectWindow;
		GetClientRect(rectWindow);
		nOffset = ptItem.y + GetItemHeight(hItem) - rectWindow.bottom;
	}
	// else nothing
	int nCurPos = GetScrollPos(SB_VERT);
	int nNewPos = nCurPos + nOffset;
	int nMin, nMax;

	GetScrollRange(SB_VERT, &nMin, &nMax);

	if (nNewPos < nMin)
		nNewPos = nMin;

	if (nNewPos > nMax)
		nNewPos = nMax;

	if (nNewPos != nCurPos)
	{
		SetScrollPos(SB_VERT, nNewPos, true);

		CRect rect, rectWindow;
		GetItemRect(hItem, rect);
		GetClientRect(rectWindow);

		if (rect.Height() < rectWindow.Height())
		{
//			nMax += rectWindow.Height() - rect.Height();
//			SetScrollRange(SB_VERT, nMin, nMax, true);

			sbBreak = false;
			while (rect.bottom > rectWindow.bottom && !sbBreak)
			{
	/*			nCurPos = GetScrollPos(SB_VERT);
				nNewPos = nCurPos + GetItemHeight(hItem);

				if (nNewPos > nMax)
					nNewPos = nMax;
			
				SetScrollPos(SB_VERT, nNewPos, true);
				InvalidateRect(NULL);
	*/
				SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
				GetItemRect(hItem, rect);
			}
			sbBreak = false;

		}
		InvalidateRect(NULL);
		UpdateWindow();
	}
	
	return true;
}

void CContextCtrl::CancelTracking()
{
	if (IsValid(m_TrackedItem))
		m_TrackedItem->OnMouseLeave(CPoint(-1, -1));

	m_TrackedItem = 0;
}

// focus item
BOOL CContextCtrl::SetItemFocus(HCONTEXTITEM hItem)
{
    if (hItem == m_FocusItem)
        return true;

    HCONTEXTITEM hPrev = m_FocusItem;
    m_FocusItem = hItem;

    if (IsValid(hPrev))
    {
        SendMessageToParent(m_FocusItem, hPrev, CVN_SELCHANGING);

		UINT nState = GetItemState(hPrev);
		nState &= ~CVIS_FOCUSED;

        SetItemState(hPrev, nState);
//        RedrawItem(hPrev); // comment to reduce flicker
		InvalidateItemRect(hPrev);
    }

    if (IsValid(m_FocusItem))
    {
        SetItemState(m_FocusItem, GetItemState(m_FocusItem) | CVIS_FOCUSED);
//        RedrawItem(m_FocusItem); // comment to reduce flicker
		InvalidateItemRect(m_FocusItem);

        SendMessageToParent(m_FocusItem, hPrev, CVN_SELCHANGED);
    }

	return true;
}

HCONTEXTITEM CContextCtrl::GetItemFocus()
{
	return m_FocusItem;
}

// hit test (point in client coordinates)
HCONTEXTITEM CContextCtrl::HitTest(CPoint pt, UINT * pFlags)
{
	HCONTEXTITEM hItem = GetItemFromPt(pt);

	CRect rect;
	if (pFlags && hItem && GetItemRect(hItem, rect))
		*pFlags = hItem->HitTest(rect, pt);

	return hItem;
}

/*
HCONTEXTITEM CContextCtrl::HitTest(CVHITTESTINFO* pHitTestInfo) const
{
	return 0;
}
*/

// sort
BOOL CContextCtrl::SortChildren(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	if (!hItem)
		return false;
	
	bool bRet = hItem->SortChildren();
	InvalidateRect(NULL);
	return bRet;
}

BOOL CContextCtrl::SortChildrenCB(LPCVSORTCB pSort)
{
	if (!pSort || !pSort->hParent || !pSort->lpfnCompare)
		return false;

	bool bRet = pSort->hParent->SortChildrenCB(pSort);
	InvalidateRect(NULL);
	return bRet;
}

/*
// change
BOOL CContextCtrl::SetItemType(HCONTEXTITEM hItem, CRuntimeClass* pRuntimeClass)
{
	ASSERT(hItem);
	// chack params is valid
	if (!hItem || !pRuntimeClass || hItem == GetRootItem())
		return false;

	// check item has same runtime-class as we need to set
	if (hItem->IsKindOf(pRuntimeClass))
		return true;

	

	return true;
}
*/

BOOL CContextCtrl::InvalidateItemRect(HCONTEXTITEM hItem)
{
	if (!m_bAllowDraw || !::IsWindow(GetSafeHwnd()))
		return true;
	CRect rect;
	if (!GetItemRect(hItem, &rect))
		return false;

	rect.InflateRect(1, 1);
	InvalidateRect(rect);

	return true;
}

// get item's origin point in client coordinates
BOOL CContextCtrl::GetItemOrigin(HCONTEXTITEM hItem, LPPOINT p)
{
	ASSERT(hItem);
	ASSERT(p);

	if (!hItem || !p)
		return false;

	p->x = 0;
	p->y = 0;

	// be sure item is visible
	// if not so we need replace item to item's parent and try again
	while (!m_listVisible.Find(hItem) && hItem != GetRootItem())
		hItem = hItem->GetParent();

	// if item is root_item -> return false
	// because root_item always invisible
	if (GetRootItem() == hItem)
		return false;

	// from first item in visible_item_list to first_visible_item
	bool bFound = false;
	HCONTEXTITEM hFirstVisItem = GetFirstVisibleItem();

	POSITION pos = 0;
	for (pos = m_listVisible.GetHeadPosition(); pos != NULL; )
	{
		// get item 
		HCONTEXTITEM hCmpItem = m_listVisible.GetNext(pos);
		// compare w/ given item
		if (hCmpItem == hItem)
		{
			bFound = true;
			p->y = 0;
		}
		// compare w/ first visible item
		if (hCmpItem == hFirstVisItem)
			break;

		p->y -= GetItemHeight(hCmpItem);
	}
	// if found -> return 
	if (bFound)
		return true;

	// if not found
	// try to find given item further from next pos 
	
	p->y = GetItemHeight(hFirstVisItem);
	for (/*pos = m_list.Find(hFirstVisItem)*/; pos != NULL; )
	{
		HCONTEXTITEM hCmpItem = m_listVisible.GetNext(pos);

		if (hCmpItem == hItem)
		{
			bFound = true;
			break;
		}

		p->y += GetItemHeight(hCmpItem);
	}

	return bFound ? true : false;
}

// return item from pt in client rect
HCONTEXTITEM CContextCtrl::GetItemFromPt(CPoint point)
{
	HCONTEXTITEM hItem = 0;

	CRect rect;
	GetClientRect(rect);

	if (!rect.PtInRect(point))
		return 0;

    int xpos = 0;
	// for all item in visible list from First visible
    for (POSITION pos = m_listVisible.Find(GetFirstVisibleItem()); pos != NULL; )
    {
		hItem = m_listVisible.GetNext(pos);
        xpos += GetItemHeight(hItem);
        if (xpos > point.y)
            break;
    }

	if (xpos < point.y)
		hItem = 0;
//    if (!hItem && m_listVisible.GetCount()) // after last
//        hItem = m_listVisible.GetTail();

	return hItem;
}

// reset selected item's list
void CContextCtrl::ResetSelected()
{
	CRect rect;
	GetClientRect(rect);

	// for all item in selected list
	for (POSITION pos = m_listSelected.GetHeadPosition(); pos != NULL; )
	{
		// get next item
		HCONTEXTITEM hItem = m_listSelected.GetNext(pos);
		CRect ItemRect;
		// union item's rect for update later 
		if (GetItemRect(hItem, ItemRect))
			rect.UnionRect(rect, ItemRect);

		// checnge state
		hItem->SetState(hItem->GetState() & ~CVIS_SELECTED);
	}
	m_listSelected.RemoveAll();

	InvalidateRect(rect);
}

// scroll bar functionality
void CContextCtrl::ResetScrollBars()
{
    if (!m_bAllowDraw || !::IsWindow(GetSafeHwnd())) 
        return;
    
    CRect rect;
    
    EnableScrollBars(SB_HORZ, false); 
    
    GetClientRect(rect);
    
    if (rect.top >= rect.bottom)
    {
        EnableScrollBarCtrl(SB_BOTH, FALSE);
        return;
    }
    
    int nVirtualHeight = GetVirtualHeight();
  
    if (rect.Height() < nVirtualHeight)
    {
//        EnableScrollBars(SB_VERT, TRUE); 
        m_nVScrollMax = nVirtualHeight + GetDefItemHeight();
    }
    else
    {
//        EnableScrollBars(SB_VERT, FALSE); 
        m_nVScrollMax = 0;
    }

    ASSERT(m_nVScrollMax < INT_MAX); // This should be fine

    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_PAGE | SIF_RANGE;
    si.nPage = (m_nVScrollMax > 0) ? rect.Height() : 0;
	si.nMin = 0;
	si.nMax = m_nVScrollMax;

    SetScrollInfo(SB_VERT, &si, true);
/*
	int nMin, nMax;
	GetScrollRange(SB_VERT, &nMin, &nMax);
	if (nMin != 0 || nMax != m_nVScrollMax)
		SetScrollRange(SB_VERT, 0, m_nVScrollMax, TRUE);
*/
    EnableScrollBars(SB_VERT, m_nVScrollMax != 0);
}

void CContextCtrl::EnableScrollBars(int nBar, BOOL bEnable)
{
    CWnd::EnableScrollBarCtrl(SB_HORZ, false);
    if (nBar == SB_VERT || nBar == SB_BOTH)
        CWnd::EnableScrollBarCtrl(SB_VERT, bEnable);
}

int  CContextCtrl::GetScrollPos32(int nBar, BOOL bGetTrackPos)
{
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);

    if (bGetTrackPos)
    {
        if (GetScrollInfo(nBar, &si, SIF_TRACKPOS))
            return si.nTrackPos;
    }
    else
    {
        if (GetScrollInfo(nBar, &si, SIF_POS))
            return si.nPos;
    }

    return 0;

}

BOOL CContextCtrl::SetScrollPos32(int nBar, int nPos, BOOL bRedraw)
{
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask  = SIF_POS;
    si.nPos   = nPos;
    return SetScrollInfo(nBar, &si, bRedraw);

}

BOOL CContextCtrl::IsVisibleVScroll()
{
	int nMin, nMax;
	GetScrollRange(SB_VERT, &nMin, &nMax);
	return nMax != 0;
}


// Editing
void  CContextCtrl::OnEndEditItem(HCONTEXTITEM hItem, CString str)
{
	ASSERT(hItem);
	if (hItem)
	{
		CString strCurrent = GetItemText(hItem);
		if (strCurrent != str)
			SetItemText(hItem, str);

		hItem->OnEndEdit();
	}
}

void  CContextCtrl::OnEditItem(HCONTEXTITEM hItem, CPoint point, UINT nChar)
{
    m_TitleTip.Hide();  // hide any titletips
//	CancelToolTips();
	CancelTracking();

    // Can we do it?
    if (!IsValid(hItem) || !IsItemEditable(hItem))
        return;

    // Can we see what we are doing?
    EnsureVisible(hItem);
    if (!IsItemVisible(hItem))
        return;

    // Where, exactly, are we gonna do it??
    CRect rect;
    if (!GetItemRect(hItem, rect))
        return;

    // Tell Mum and Dad what we are doing
	CVITEM cvItem;
	if (!hItem->ToCVITEM(&cvItem))
		return;

    if (!SendMessageToParentDisp(&cvItem, CVN_BEGINLABELEDIT))
		return;

    // Let's do it...
    hItem->Edit(rect, point, IDC_INPLACE_CONTROL, nChar);
}

void  CContextCtrl::EndEditing()
{
    if (!IsValid(m_FocusItem)) 
		return;

    if (m_FocusItem)
        m_FocusItem->EndEdit();
}

// Drawing
void  CContextCtrl::OnDraw(CDC* pDC)
{
    if (!m_bAllowDraw)
        return;

    CRect clipRect;
    if (pDC->GetClipBox(&clipRect) == ERROR)
        return;

    EraseBkgnd(pDC);            // OnEraseBkgnd does nothing, so erase bkgnd here.
    // This necessary since we may be using a Memory DC.

    CRect rect;
	CRect ClientRect, VisRect;
	GetClientRect(&ClientRect);

	CItemList VisList;
	if (!GetVisibleItems(VisList, VisRect))
		return;

	CContextItemBase * pItem = 0;
    rect.bottom = -1;
	rect.left = 0;//max(clipRect.left, VisRect.left);
	rect.right = ClientRect.right - 1;//min(clipRect.right, VisRect.right);

    for (POSITION pos = VisList.GetHeadPosition(); pos != NULL; )
    {
		pItem = VisList.GetNext(pos);

        rect.top = rect.bottom + 1;
        rect.bottom = rect.top + GetItemHeight(pItem) - 1;

        if (rect.top > clipRect.bottom)
            continue;                // Gone past cliprect
        if (rect.bottom < clipRect.top)
            continue;             // Reached cliprect yet?

        pItem->Draw(pDC, rect, FALSE);
    }

	// empty visible list
	VisList.RemoveAll();
}

void CContextCtrl::EraseBkgnd(CDC* pDC)
{
    CRect  VisRect(0, 0, 0, 0), ClipRect, rect;
    CBrush Back(GetBkColor());

    if (pDC->GetClipBox(ClipRect) == ERROR)
        return;

	UINT nVisCount = GetVisibleCount();
	if (nVisCount != 0)
		GetVisibleItems(NULL, 0, &VisRect);

	if (VisRect.IsRectEmpty())
		VisRect = ClipRect;

	rect.left	= min(VisRect.left, ClipRect.left); //???
	rect.right	= min(VisRect.right, ClipRect.right);//???
	rect.top	= max(VisRect.top, ClipRect.top);//???
	rect.bottom	= max(VisRect.bottom, ClipRect.bottom);//???

    // Draw Fixed columns background
    pDC->FillRect(rect, &Back);
}

BOOL CContextCtrl::SetRedraw(BOOL bAllowDraw, BOOL bResetScrollBars)
{
	BOOL bPrevAllowDraw = m_bAllowDraw;
    m_bAllowDraw = bAllowDraw;

    if (m_bAllowDraw != bPrevAllowDraw && m_bAllowDraw)
	{
		RecalcVisible();
        InvalidateRect(NULL, false);
	}

	if (bResetScrollBars)
		ResetScrollBars();

	return bPrevAllowDraw;
}

void CContextCtrl::Invalidate(BOOL bErase)
{
	if (!::IsWindow(GetSafeHwnd()) || !m_bAllowDraw)
		return;
	CWnd::Invalidate(bErase);
}


void CContextCtrl::InvalidateRect(LPCRECT lpRect, BOOL bErase)
{
	if (!::IsWindow(GetSafeHwnd()) || !m_bAllowDraw)
		return;

	CWnd::InvalidateRect(lpRect, bErase);
}

BOOL CContextCtrl::RedrawItem(HCONTEXTITEM hItem, CDC * pDC)
{
    BOOL bResult = TRUE;
    BOOL bMustReleaseDC = FALSE;

    if (!m_bAllowDraw || !IsItemVisible(hItem))
        return FALSE;

    CRect rect;
    if (!GetItemRect(hItem, rect))
        return FALSE;

    if (!pDC)
    {
        pDC = GetDC();
        if (pDC)
            bMustReleaseDC = TRUE;
    }

    if (pDC)
		bResult = hItem->Draw(pDC, rect, TRUE);
    else
        InvalidateRect(rect, TRUE);     // Could not get a DC - invalidate it anyway
    // and hope that OnPaint manages to get one

    if (bMustReleaseDC)
        ReleaseDC(pDC);

    return bResult;
}

void	CContextCtrl::CreateLightBrush()
{
	CWindowDC dc(NULL);
	int nBitsPerPixel = dc.GetDeviceCaps(BITSPIXEL);
	m_brLight.DeleteObject();

	if (nBitsPerPixel > 8)
	{
		COLORREF clrBk = GetBkColor();
		COLORREF clrHighlight = GetHighlightColor();

		COLORREF clrLight = RGB (GetRValue(clrBk) + ((GetRValue(clrHighlight) - GetRValue(clrBk)) / 2),
								 GetGValue(clrBk) + ((GetGValue(clrHighlight) - GetGValue(clrBk)) / 2),
								 GetBValue(clrBk) + ((GetBValue(clrHighlight) - GetBValue(clrBk)) / 2));

		m_brLight.CreateSolidBrush(clrLight);
	}
	else
	{
		HBITMAP hbmGray = CreateDitherBitmap(dc.GetSafeHdc());
		ASSERT(hbmGray != NULL);
		CBitmap bmp;
		bmp.Attach(hbmGray);
		m_brLight.CreatePatternBrush(&bmp);
	}
}

HBITMAP	CContextCtrl::CreateDitherBitmap(HDC hDC)
{
	COLORREF clrBk = GetBkColor();
	COLORREF clrHighlight = GetHighlightColor();
	struct  // BITMAPINFO with 16 colors
	{
		BITMAPINFOHEADER	bmiHeader;
		RGBQUAD				bmiColors[16];
	} 
	bmi;
	memset(&bmi, 0, sizeof(bmi));

	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = 8;
	bmi.bmiHeader.biHeight = 8;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 1;
	bmi.bmiHeader.biCompression = BI_RGB;

	bmi.bmiColors[0].rgbBlue = GetBValue(clrBk);
	bmi.bmiColors[0].rgbGreen = GetGValue(clrBk);
	bmi.bmiColors[0].rgbRed = GetRValue(clrBk);

	bmi.bmiColors[1].rgbBlue = GetBValue(clrHighlight);
	bmi.bmiColors[1].rgbGreen = GetGValue(clrHighlight);
	bmi.bmiColors[1].rgbRed = GetRValue(clrHighlight);

	// initialize the brushes
	long patGray[8];
	for (int i = 0; i < 8; i++)
	   patGray[i] = (i & 1) ? 0xAAAA5555L : 0x5555AAAAL;

	HBITMAP hbm = CreateDIBitmap(hDC, &bmi.bmiHeader, CBM_INIT,
		(LPBYTE)patGray, (LPBITMAPINFO)&bmi, DIB_RGB_COLORS);
	return hbm;
}

COLORREF CContextCtrl::CreateDitherColor()
{
	COLORREF clrBk = GetBkColor();
	COLORREF clrHighlight = GetHighlightColor();
	return RGB(GetRValue(clrBk) + ((GetRValue(clrHighlight) - GetRValue(clrBk)) / 2),
			   GetGValue(clrBk) + ((GetGValue(clrHighlight) - GetGValue(clrBk)) / 2),
			   GetBValue(clrBk) + ((GetBValue(clrHighlight) - GetBValue(clrBk)) / 2));
}


///////////////////////////////////////////////////////////////////////////////
// message handlers
void CContextCtrl::OnPaint()
{
    CPaintDC dc(this);      // device context for painting

    if (m_bDoubleBuffer)    // Use a memory DC to remove flicker
    {
		CRect rc;
		GetClientRect(rc);
        CMemDC MemDC(&dc, rc);
        OnDraw(&MemDC);
    }
    else                    // Draw raw - this helps in debugging vis problems.
        OnDraw(&dc);
}


BOOL CContextCtrl::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;    // Don't erase the background.
}

void CContextCtrl::OnSize(UINT nType, int cx, int cy)
{
    static BOOL bAlreadyInsideThisProcedure = FALSE;
    if (bAlreadyInsideThisProcedure)
        return;

    if (!::IsWindow(m_hWnd))
        return;

    // Start re-entry blocking
    bAlreadyInsideThisProcedure = TRUE;

    EndEditing();        // destroy any InPlaceEdit's

    CWnd::OnSize(nType, cx, cy);
    ResetScrollBars();

    // End re-entry blocking
    bAlreadyInsideThisProcedure = FALSE;
}

void CContextCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    EndEditing();
    m_TitleTip.Hide();  // hide any titletips
//	CancelToolTips();
	CancelTracking();


    // Get the scroll position ourselves to ensure we get a 32 bit value
    int scrollPos = GetScrollPos32(SB_VERT);

	HCONTEXTITEM hFirst = GetFirstVisibleItem();

    CRect rect;
    GetClientRect(rect);

    switch (nSBCode)
    {
    case SB_LINEDOWN:
        if (scrollPos < m_nVScrollMax)
        {
			HCONTEXTITEM hLast = GetLastVisibleItem();
            
			int yScroll = GetItemHeight(hLast);//hFirst);
			yScroll += scrollPos;
			yScroll = (yScroll < m_nVScrollMax) ? yScroll : m_nVScrollMax;

            SetScrollPos32(SB_VERT, yScroll);
            if (GetScrollPos32(SB_VERT) == scrollPos)
			{
//				sbBreak = true;
                break;          // didn't work
			}

            //rect.top = GetFixedRowHeight() + yScroll;
            //ScrollWindow(0, -yScroll, rect);
            //rect.top = rect.bottom - yScroll;
            InvalidateRect(rect);
        }
        break;
        
    case SB_LINEUP:
        if (scrollPos > 0)
        {
			POSITION pos = m_listVisible.Find(hFirst);
			if (pos)
				m_listVisible.GetPrev(pos);
			
			HCONTEXTITEM hToUse = 0;
			if (pos)
				hToUse = m_listVisible.GetPrev(pos);

            int yScroll = hToUse ? GetItemHeight(hToUse) : 0;
            SetScrollPos32(SB_VERT, max(0, scrollPos - yScroll));
            //ScrollWindow(0, yScroll, rect);
            //rect.bottom = rect.top + yScroll;
            InvalidateRect(rect);
        }
        break;
        
    case SB_PAGEDOWN:
        if (scrollPos < m_nVScrollMax)
        {
            scrollPos = min(m_nVScrollMax, scrollPos + rect.Height());
            SetScrollPos32(SB_VERT, scrollPos);
            InvalidateRect(rect);
        }
        break;
        
    case SB_PAGEUP:
        if (scrollPos > 0)
        {
            int offset = -rect.Height();
            int pos = max(0, scrollPos + offset);
            SetScrollPos32(SB_VERT, pos);
            InvalidateRect(rect);
        }
        break;
        
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        {
			int nPos = GetScrollPos32(SB_VERT, TRUE);
            SetScrollPos32(SB_VERT, nPos);
//			HCONTEXTITEM hNewFirst = GetFirstVisibleItem();
//            if (hNewFirst != hFirst)
                InvalidateRect(rect);
        }
        break;
        
    case SB_TOP:
        if (scrollPos > 0)
        {
            SetScrollPos32(SB_VERT, 0);
            InvalidateRect(NULL);
        }
        break;
        
    case SB_BOTTOM:
        if (scrollPos < m_nVScrollMax)
        {
            SetScrollPos32(SB_VERT, m_nVScrollMax);
            InvalidateRect(NULL);
        }
        
    default: 
        break;
    }
}

void CContextCtrl::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent != WM_VSCROLL && nIDEvent != WM_MOUSEMOVE)
 		return;

    CPoint pt, origPt;
    if (!GetCursorPos(&origPt))
        return;

    ScreenToClient(&origPt);

    CRect rect;
    GetClientRect(rect);
	if (nIDEvent == WM_MOUSEMOVE)
	{
		if (m_bMouseButtonDown == FALSE && m_nVTimerID != 0)
		{
//			ClipCursor(NULL);

			if (GetCapture()->GetSafeHwnd() == GetSafeHwnd())
			{
				ReleaseCapture();
				KillTimer(m_nVTimerID);
				m_nVTimerID = 0;
			}
			m_MouseMode = MOUSE_NOTHING;

			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		}

		if (!rect.PtInRect(origPt))
			CancelTracking();

		return;
	}

	HCONTEXTITEM FocusItem = GetItemFocus();
	pt = origPt;

    if (nIDEvent == WM_VSCROLL)
	{
		if (pt.y > rect.bottom)
		{
			SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);

			if (pt.x < rect.left)
				pt.x = rect.left;
			if (pt.x > rect.right)
				pt.x = rect.right;
			pt.y = rect.bottom - 1;
			
			HCONTEXTITEM hItem = GetItemFromPt(pt);
			if (FocusItem != hItem)
				SetItemFocus(hItem);

			/// tracking
			if (IsTracking())
			{
				CRect ItemRect;
				if (hItem && /*hItem->GetTextRect(ItemRect))*/GetItemRect(hItem, ItemRect))
				{
					bool bPtrInRect = ItemRect.PtInRect(origPt) == TRUE;
					if (m_TrackedItem != hItem || !bPtrInRect)
					{
						if (IsValid(m_TrackedItem))
							m_TrackedItem->OnMouseLeave(origPt);

						m_TrackedItem = bPtrInRect ? hItem : 0;
						if (m_TrackedItem && bPtrInRect)
							m_TrackedItem->OnMouseEnter(origPt);
					}
					if (hItem && bPtrInRect)
						hItem->OnMouseOver(origPt);
				}
			}
		}
		else if (pt.y < rect.top)
		{
			SendMessage(WM_VSCROLL, SB_LINEUP, 0);

			if (pt.x < rect.left)
				pt.x = rect.left;
			if (pt.x > rect.right)
				pt.x = rect.right;
			pt.y = 1;
			
			HCONTEXTITEM hItem = GetItemFromPt(pt);
			if (FocusItem != hItem)
				SetItemFocus(hItem);

			/// tracking
			if (IsTracking())
			{
				CRect ItemRect;
				if (hItem && /*hItem->GetTextRect(ItemRect))//*/GetItemRect(hItem, ItemRect))
				{
					bool bPtrInRect = ItemRect.PtInRect(origPt) == TRUE;
					if (m_TrackedItem != hItem || !bPtrInRect)
					{
						if (IsValid(m_TrackedItem))
							m_TrackedItem->OnMouseLeave(origPt);

						m_TrackedItem = bPtrInRect ? hItem : 0;
						if (m_TrackedItem && bPtrInRect)
							m_TrackedItem->OnMouseEnter(origPt);
					}
					if (hItem && bPtrInRect)
						hItem->OnMouseOver(origPt);
				}
			}
		}
	}
}

UINT CContextCtrl::OnGetDlgCode()
{
    UINT nCode = DLGC_WANTARROWS | DLGC_WANTCHARS|DLGC_DEFPUSHBUTTON;// | DLGC_WANTALLKEYS;
    return nCode;
}

CPoint CContextCtrl::GetPointClicked(HCONTEXTITEM hItem, const CPoint & point)
{
    CPoint PointItemOrigin;
    if (!hItem || !GetItemOrigin(hItem, &PointItemOrigin))
        return CPoint(0, 0);

    CPoint PointClickedItemRelative(point);
    PointClickedItemRelative -= PointItemOrigin;
    return PointClickedItemRelative;
}

// mouse functionality
void CContextCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
    m_TitleTip.Hide();  // hide any titletips
//	CancelToolTips();
	CancelTracking();

    SetFocus();

    m_bMouseButtonDown   = TRUE;
    m_LeftClickDownPoint = point;

	// get hittest result
	UINT uFlag = 0;
    m_LeftClickDownItem  = HitTest(point, &uFlag);

    if (!IsValid(m_LeftClickDownItem))
        return;

    EndEditing();

//	TRACE("lButtonDown : %s\n", m_LeftClickDownItem->GetText());
    // tell the item about it -- only things like a btn embedded in a item
    //  would look at these msgs
    if (m_LeftClickDownItem)
        m_LeftClickDownItem->OnClickDown(point);

	// for non header items only!!!
	if (!ItemIsHeader(m_LeftClickDownItem))
	{
		// If the user clicks on a number field of nonheader item, then prepare to select this item
		// (If the user moves the mouse, then cancel)
		if ((uFlag & CVHT_ONITEMNUMBER) == CVHT_ONITEMNUMBER)
		{
			m_MouseMode = MOUSE_PREPARE_NUMBER_ITEM;
			m_LastMousePoint = point;
			return;
		}

		// If the user clicks on the current item, then prepare to edit it.
		// (If the user moves the mouse, then dragging occurs)
		if (m_LeftClickDownItem == m_FocusItem && (uFlag & CVHT_ONITEMLABEL) == CVHT_ONITEMLABEL)
		{
			if (IsItemEditable(m_LeftClickDownItem))
			{
				m_MouseMode = MOUSE_PREPARE_EDIT;
				m_LastMousePoint = point;
				return;
			}
		}
	}
/*  
	else // for header items only 
	{
		if ((uFlag & CVHT_ONITEMBUTTON) == CVHT_ONITEMBUTTON)
		{
			m_MouseMode = MOUSE_PREPARE_EDIT;
			m_LastMousePoint = point;
		}
	}
*/
    SetCapture();

	if ((uFlag & CVHT_ONITEMICON) == CVHT_ONITEMICON)
	{
		SetItemFocus(m_LeftClickDownItem);
	}
	else if ((uFlag & CVHT_ONITEMLABEL) == CVHT_ONITEMLABEL ||
			(uFlag & CVHT_ONITEM) == CVHT_ONITEM)
	{
		m_bFocusOnSelected = ((m_LeftClickDownItem->GetState() & CVIS_SELECTED) == CVIS_SELECTED) && m_LeftClickDownItem != m_FocusItem;
		SetItemFocus(m_LeftClickDownItem);
	}

    if (m_MouseMode != MOUSE_PREPARE_NUMBER_ITEM && (uFlag & CVHT_NOWHERE) != CVHT_NOWHERE) // not sizing or editing -- selecting
    {
        m_MouseMode = MOUSE_SELECT;
		m_nVTimerID = SetTimer(WM_VSCROLL, m_nTimerInterval, 0);
    }   

    m_LastMousePoint = point;
}

/*
CVHT_ONITEMLABEL
CVHT_ONITEMICON         0x0002
CVHT_ONITEMLABEL        0x0004
CVHT_ONITEM             (TVHT_ONITEMICON | TVHT_ONITEMLABEL)
CVHT_ONITEMINDENT       0x0008
CVHT_ONITEMBUTTON       0x0010
CVHT_ONITEMNUMBER		0x0020
*/


void CContextCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
    CWnd::OnLButtonUp(nFlags, point);

    m_bMouseButtonDown = FALSE;

    ClipCursor(NULL);

	if (GetCapture()->GetSafeHwnd() == GetSafeHwnd())
        ReleaseCapture();

    if (m_nVTimerID)
    {
        KillTimer(m_nVTimerID);
        m_nVTimerID = 0;
    }

	UINT uFlag = 0;
	HCONTEXTITEM hHitItem = HitTest(point, &uFlag);
    if (!IsValid(hHitItem))
	{
		m_MouseMode = MOUSE_NOTHING;
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		m_bMouseLDblClick = false;
        return;
	}

//	TRACE("lButtonUp : %s\n", hHitItem->GetText());

	if (!m_bMouseLDblClick)
	{
		// for non header item only
		if (!ItemIsHeader(hHitItem))
		{
			// m_MouseMode == MOUSE_PREPARE_EDIT only if user clicked down on current item
			// and then didn't move mouse before clicking up (releasing button)
			if (m_MouseMode == MOUSE_PREPARE_EDIT && 
				(uFlag & CVHT_ONITEMLABEL) == CVHT_ONITEMLABEL && 
				m_FocusItem == m_LeftClickDownItem && m_FocusItem == hHitItem)
			{
				OnEditItem(m_FocusItem, point, VK_LBUTTON);
			}
			else if ((uFlag & CVHT_ONITEMICON) == CVHT_ONITEMICON)
			{
				if (hHitItem == m_LeftClickDownItem)
				{
					if (m_FocusItem != hHitItem)
					{
						SetItemFocus(hHitItem);
					}
					else
					{
						hHitItem->OnClick(point);
						SendMessageToParent(hHitItem, 0, NM_CLICK);
						SendMessageToParent(hHitItem, 0, CVN_ACTIVATE);
					}
				}
			}
			else if ((uFlag & CVHT_ONITEMLABEL) == CVHT_ONITEMLABEL ||
					 (uFlag & CVHT_ONITEM) == CVHT_ONITEM)
			{
				if (hHitItem == m_LeftClickDownItem)
				{
					if (m_FocusItem != hHitItem)
					{
						SetItemFocus(hHitItem);
					}
					else if (!m_bFocusOnSelected)
					{
						hHitItem->OnClick(point);
						SendMessageToParent(hHitItem, 0, NM_CLICK);
						SendMessageToParent(hHitItem, 0, CVN_ACTIVATE);
					}
				}
			}
			else if (m_MouseMode == MOUSE_PREPARE_NUMBER_ITEM && 
					 (uFlag & CVHT_ONITEMNUMBER) == CVHT_ONITEMNUMBER) 
			{
				hHitItem->OnSetCursor(point);
				m_MouseMode = MOUSE_NOTHING;
				if (hHitItem == m_LeftClickDownItem)
				{
					if (IsItemSelected(hHitItem))
					{
						SendMessageToParent(hHitItem, 0, CVN_ACTIVATE);
					}
					else
					{
						SendMessageToParent(hHitItem, 0, CVN_NUMBER);
					}
				}
				return;
			}
		}
		else // for header item
		{
			if (m_LeftClickDownItem == hHitItem && (uFlag & CVHT_ONITEMBUTTON) == CVHT_ONITEMBUTTON)
				Expand(hHitItem, CVE_TOGGLE);
			else if (m_FocusItem == hHitItem)
			{
				m_FocusItem->OnClick(point);
				SendMessageToParent(m_FocusItem, 0, NM_CLICK);
			}
		}
	}
	m_bMouseLDblClick = false;

    m_MouseMode = MOUSE_NOTHING;
    
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
    
    if (!IsValid(m_LeftClickDownItem))
        return;
    
    CWnd *pOwner = GetOwner();
    if (pOwner && IsWindow(pOwner->m_hWnd))
        pOwner->PostMessage(WM_COMMAND, MAKELONG(GetDlgCtrlID(), BN_CLICKED),
        (LPARAM) GetSafeHwnd());
}

void CContextCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	m_bMouseLDblClick = true;
	UINT uFlag = 0;
	HCONTEXTITEM hHitItem = HitTest(point, &uFlag);
    if (!IsValid(hHitItem))
    {
		m_MouseMode = MOUSE_NOTHING;
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
        return;
    }

//	TRACE("lButtonDblClick", hHitItem->GetText());
	// for non header item only
/*	if (!ItemIsHeader(hHitItem))
	{
		// m_MouseMode == MOUSE_PREPARE_EDIT only if user clicked down on current item
		// and then didn't move mouse before clicking up (releasing button)
		if (m_MouseMode == MOUSE_PREPARE_EDIT && 
			(uFlag & CVHT_ONITEMLABEL) == CVHT_ONITEMLABEL && 
			m_FocusItem == m_LeftClickDownItem && m_FocusItem == hHitItem)
		{
			OnEditItem(m_FocusItem, point, VK_LBUTTON);
		}
		else if ((uFlag & CVHT_ONITEMICON) == CVHT_ONITEMICON ||
				 (uFlag & CVHT_ONITEMLABEL) == CVHT_ONITEMLABEL)
		{
			if (hHitItem == m_LeftClickDownItem)
			{
				if (m_FocusItem != hHitItem)
				{
					SetItemFocus(hHitItem);
				}
				else 
				{
					hHitItem->OnDblClick(point);
					SendMessageToParent(hHitItem, 0, NM_DBLCLK);
					SendMessageToParent(hHitItem, 0, CVN_ACTIVATE);
				}
			}
		}
		else if ((uFlag & CVHT_ONITEM) == CVHT_ONITEM)
		{
			if (m_FocusItem == hHitItem)
			{
				hHitItem->OnDblClick(point);
				SendMessageToParent(hHitItem, 0, NM_DBLCLK);
				SendMessageToParent(hHitItem, 0, CVN_ACTIVATE);
			}
		}
		else if (m_MouseMode == MOUSE_PREPARE_NUMBER_ITEM && 
				 (uFlag & CVHT_ONITEMNUMBER) == CVHT_ONITEMNUMBER) 
		{
			hHitItem->OnSetCursor(point);
			m_MouseMode = MOUSE_NOTHING;
			if (hHitItem == m_LeftClickDownItem)
			{
				if (IsItemSelected(hHitItem))
				{
					SendMessageToParent(hHitItem, 0, CVN_ACTIVATE);
				}
				else
				{
					SendMessageToParent(hHitItem, 0, CVN_NUMBER);
				}
			}
			return;
		}
	}
	else // for header item
	{
*/		if (m_MouseMode == MOUSE_NOTHING)
		{
			if ((uFlag & CVHT_ONITEMBUTTON) != CVHT_ONITEMBUTTON)
				Expand(hHitItem, CVE_TOGGLE);
			hHitItem->OnDblClick(point);
	        SendMessageToParent(hHitItem, 0, NM_DBLCLK);
		}
//	}
	

    CWnd::OnLButtonDblClk(nFlags, point);
}

void CContextCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
    CWnd::OnRButtonUp(nFlags, point);
    m_TitleTip.Hide();  // hide any titletips
//	CancelToolTips();
	CancelTracking();

    EndEditing();        // Auto-destroy any InPlaceEdit's
    ClipCursor(NULL);

    if (GetCapture()->GetSafeHwnd() == GetSafeHwnd())
    {
        ReleaseCapture();
        if (!m_nVTimerID)
		{
			KillTimer(m_nVTimerID);
	        m_nVTimerID = 0;
		}
    }

	UINT uFlag = 0;
	HCONTEXTITEM hHitItem = HitTest(point, &uFlag);
    if (!IsValid(hHitItem))
    {
		m_MouseMode = MOUSE_NOTHING;
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
        return;
    }

	SetItemFocus(hHitItem);
}

void CContextCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
    CWnd::OnRButtonDown(nFlags, point);

    m_TitleTip.Hide();  // hide any titletips
//	CancelToolTips();

    EndEditing();        // Auto-destroy any InPlaceEdit's
	CancelTracking();
    ClipCursor(NULL);
/*
    if (!m_nVTimerID)
    {
        KillTimer(m_nVTimerID);
        m_nVTimerID = 0;
    }
*/
	UINT uFlag = 0;
	HCONTEXTITEM hHitItem = HitTest(point, &uFlag);
    if (!IsValid(hHitItem))
    {
		m_MouseMode = MOUSE_NOTHING;
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
        return;
    }

	SetItemFocus(hHitItem);
}

void CContextCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
    CRect rect;
    GetClientRect(rect);

    // If outside client area, return (unless we are drag n dropping)
    if (m_MouseMode != MOUSE_DRAGGING && !rect.PtInRect(point))
        return;

	UINT uFlag = 0;
	HCONTEXTITEM hItem = HitTest(point, &uFlag);

	if (m_MouseMode == MOUSE_PREPARE_EDIT)
	{
		int nDelta = (int)sqrt(double((m_LeftClickDownPoint.x - point.x) * (m_LeftClickDownPoint.x - point.x) + 
							   (m_LeftClickDownPoint.y - point.y) * (m_LeftClickDownPoint.y - point.y)));
		if (nDelta > m_nMinMoveDistance)
		{
			if (!m_nVTimerID)
				m_nVTimerID = SetTimer(WM_VSCROLL, m_nTimerInterval, 0);
			m_MouseMode = MOUSE_SELECT;
		}
	}
    // Sometimes a MOUSEMOVE message can come after the left buttons
    // has been let go, but before the BUTTONUP message hs been processed.
    // We'll keep track of mouse buttons manually to avoid this.
    // All bMouseButtonDown's have been replaced with the member m_bMouseButtonDown
    // BOOL bMouseButtonDown = ((nFlags & MK_LBUTTON) == MK_LBUTTON);


    // If the left mouse button is up, then test to see if row/column sizing is imminent
    if (!m_bMouseButtonDown ||
        (m_bMouseButtonDown && m_MouseMode == MOUSE_NOTHING))
    {
		if (m_MouseMode != MOUSE_NOTHING)
        {
            SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
            m_MouseMode = MOUSE_NOTHING;
        }

        if (m_MouseMode == MOUSE_NOTHING)
        {
			CRect ItemRect;
			ItemRect.SetRectEmpty();
			if (hItem)
				GetItemRect(hItem, &ItemRect);

            // Let the item know that a big fat cursor is currently hovering
            // over it.
			/// tracking
			if (IsTracking())
			{
//				CRect rectText;
//				rectText.SetRectEmpty();
//
//				if (hItem)
//					hItem->GetTextRect(rectText);

				bool bPtrInRect = ItemRect.PtInRect(point) == TRUE;
				if (m_TrackedItem != hItem || !bPtrInRect)
				{
					if (IsValid(m_TrackedItem))
						m_TrackedItem->OnMouseLeave(point);

					m_TrackedItem = bPtrInRect ? hItem : 0;
					if (m_TrackedItem && bPtrInRect)
						m_TrackedItem->OnMouseEnter(point);
				}
				if (hItem && bPtrInRect)
					hItem->OnMouseOver(point);
			}

            // Titletips anyone? anyone?
            if (m_bTitleTips)
            {
                CRect TextRect = ItemRect;
                if (hItem)
                {
                    LPCTSTR szTipText = hItem->GetTipText();
                    if (szTipText && szTipText[0]
                        && !hItem->IsEditing()
                        && hItem->GetTextRect(TextRect)
						&& !TextRect.IsRectEmpty())
                    {
						int nOffset = 0;
                        m_TitleTip.Show(TextRect, hItem->GetTipText(), nOffset, GetMargin(), ItemRect,
                                        hItem->GetFont(),  GetTitleTipTextClr(), GetTitleTipBackClr());
                    }
                }
            }
        }

        m_LastMousePoint = point;
        return;
    }

    if (!IsValid(m_LeftClickDownItem))
    {
        m_LastMousePoint = point;
        return;
    }

    // If the left mouse button is down, then process appropriately
    if (m_bMouseButtonDown)
    {
        switch (m_MouseMode)
        {
        case MOUSE_SELECT:
            {
                if (!IsValid(hItem))
                    return;

				HCONTEXTITEM FocusItem = GetItemFocus();

                if (hItem != FocusItem)// && !ItemIsHeader(hItem))
                {
                    SetItemFocus(hItem);
                }

                break;
            }

            break;
            
        case MOUSE_PREPARE_DRAG:
            m_MouseMode = MOUSE_PREPARE_DRAG;
            break;
        }    
    }
    
    m_LastMousePoint = point;
}

BOOL CContextCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // A m_nRowsPerWheelNotch value less than 0 indicates that the mouse
    // wheel scrolls whole pages, not just lines.
    if (m_nRowsPerWheelNotch == -1)
    {
        int nPagesScrolled = zDelta / 120;

        if (nPagesScrolled > 0)
            for (int i = 0; i < nPagesScrolled; i++)
                PostMessage(WM_VSCROLL, SB_PAGEUP, 0);
        else
            for (int i = 0; i > nPagesScrolled; i--)
                PostMessage(WM_VSCROLL, SB_PAGEDOWN, 0);
    }
    else
    {
        int nRowsScrolled = m_nRowsPerWheelNotch * zDelta / 120;

        if (nRowsScrolled > 0)
            for (int i = 0; i < nRowsScrolled; i++)
                PostMessage(WM_VSCROLL, SB_LINEUP, 0);
        else
            for (int i = 0; i > nRowsScrolled; i--)
                PostMessage(WM_VSCROLL, SB_LINEDOWN, 0);
    }

    return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CContextCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    if (nHitTest == HTCLIENT)
    {
		CPoint pt(GetMessagePos());
		ScreenToClient(&pt);
		HCONTEXTITEM hItem = GetItemFromPt(pt);

        switch (m_MouseMode)
        {
/*
        case MOUSE_OVER_COL_DIVIDE:
			{
				UINT uCursor = IDC_CURSOR_WIDTH;

				CPoint start;
				if (GetCellOrigin(0, cell.col, &start) && 
					(pt.x - start.x <= m_nResizeCaptureRange))
				{
					if (GetCellOrigin(0, cell.col - 1, &start))
					{
						if (GetColumnWidth(cell.col - 1) <= (2 * m_nResizeCaptureRange + 1))
							uCursor = IDC_CURSOR_COLUMN_SPLIT;
					}
				}
				SetCursor(AfxGetApp()->LoadCursor(uCursor));
			}
            break;
        case MOUSE_OVER_ROW_DIVIDE:
			{
				UINT uCursor = IDC_CURSOR_HEIGHT;
				CPoint start;
				if (GetCellOrigin(cell.row, 0, &start) && 
					(pt.y - start.y <= m_nResizeCaptureRange))
				{
					if (GetCellOrigin(cell.row - 1, 0, &start))
					{
						if (GetRowHeight(cell.row - 1) <= (2 * m_nResizeCaptureRange + 1))
							uCursor = IDC_CURSOR_ROW_SPLIT;
					}
				}
				SetCursor(AfxGetApp()->LoadCursor(uCursor));
			}
            break;
*/
        case MOUSE_DRAGGING:
            break;

        default:
            if (IsValid(hItem))
            {
                if (hItem && hItem->OnSetCursor(pt))
					return TRUE;
            }

            SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
        }
        return TRUE;
    }

    return CWnd::OnSetCursor(pWnd, nHitTest, message);

}

void CContextCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
/*
	if (nChar == VK_ESCAPE && m_MouseMode == MOUSE_COLUMN_DRAGGING)
	{
		m_bMouseButtonDown = FALSE;
		ClipCursor(NULL);

		if (GetCapture()->GetSafeHwnd() == GetSafeHwnd())
		{
			ReleaseCapture();
			KillTimer(m_nVTimerID);
			m_nVTimerID = 0;
		}
		m_MouseMode = MOUSE_NOTHING;

		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		return;
	}
*/
	m_TitleTip.Hide();
//	CancelToolTips();
	CancelTracking();
    if (!IsValid(m_FocusItem))
    {
        CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
        return;
    }

    HCONTEXTITEM hNext = m_FocusItem;
    BOOL bChangeLine = FALSE;
	BOOL bShift = IsSHIFTpressed();
	BOOL bCtrl = IsCTRLpressed();


/*
    if (IsCTRLpressed())
    {
        switch (nChar)
        {
        case 'A':
            OnEditSelectAll();
            break;
        case 'X':
            OnEditCut();
            break;
        case VK_INSERT:
        case 'C':
            OnEditCopy();
            break;
        case 'V':
            OnEditPaste();
            break;
        }
    }
*/
/*
    if (IsSHIFTpressed() &&(nChar == VK_INSERT))
        OnEditPaste();
*/
    switch (nChar)
    {
/*    case VK_DELETE: 
        if (IsItemEditable(m_FocusItem))
        {
			CVITEM cvItem;
			if (m_FocusItem->ToCVITEM(&cvItem) && SendMessageToParentDisp(&cvItem, CVN_BEGINLABELEDIT))
			{
				cvItem.strText = _T("");
				if (SendMessageToParentDisp(&cvItem, CVN_ENDLABELEDIT))
					SetItemText(m_FocusItem, cvItem.strText);

				RedrawItem(m_FocusItem);
			}
        }
        break;
        
    case VK_TAB:    
        if (IsSHIFTpressed())
        {
            if (next.col > m_nFixedCols) 
                next.col--;
            else if (next.col == m_nFixedCols && next.row > m_nFixedRows) 
            {
                next.row--; 
                next.col = GetColumnCount() - 1; 
                bChangeLine = TRUE;
            }
            else
                CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
        }
        else
        {
            if (next.col <(GetColumnCount() - 1)) 
                next.col++;
            else if (next.col == (GetColumnCount() - 1) && 
                next.row <(GetRowCount() - 1))
            {
                next.row++; 
                next.col = m_nFixedCols; 
                bChangeLine = TRUE;
            }
            else
                CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
        } 
        break;
*/        
    case VK_DOWN:   
            // don't let user go to a hidden row
			hNext = (IsCTRLpressed()) ? GetNextVisibleHeaderItem(m_FocusItem) : GetNextVisibleItem(m_FocusItem);

			if (!hNext)
				hNext = m_FocusItem;
            break;
        
    case VK_UP:     
            // don't let user go to a hidden row
			hNext = (IsCTRLpressed()) ? GetPrevVisibleHeaderItem(m_FocusItem) : GetPrevVisibleItem(m_FocusItem);
			if (!hNext)
				hNext = m_FocusItem;
            break;
        
    case VK_RIGHT:  
            // don't let user go to a hidden column
			if (m_FocusItem && !m_FocusItem->IsExpanded())
			{
				hNext = m_FocusItem;
				Expand(m_FocusItem, CVE_EXPAND);
			}
/*			else
			{
				hNext = GetNextVisibleItem(m_FocusItem);
				if (!hNext)
					hNext = m_FocusItem;
			}
*/
            break;
        
    case VK_LEFT:   
            // don't let user go to a hidden column
			if (m_FocusItem && m_FocusItem->IsExpanded())
			{
				hNext = m_FocusItem;
				Expand(m_FocusItem, CVE_COLLAPSE);
			}
/*			else
			{
				hNext = GetPrevVisibleItem(m_FocusItem);
				if (!hNext)
					hNext = m_FocusItem;
			}
*/
            break;
        
    case VK_NEXT:   
        {
			if (IsCTRLpressed() || !IsVisibleVScroll())
			{
				SendMessage(WM_VSCROLL, SB_BOTTOM, 0);
				hNext = m_listVisible.GetTail();
			}
			else
			{
				SendMessage(WM_VSCROLL, SB_PAGEDOWN, 0);

				SCROLLINFO sf;
				sf.cbSize = sizeof(SCROLLINFO);
				sf.fMask = SIF_PAGE;

				CRect rect;
				GetClientRect(rect);
				
				CPoint ptFocus;
				hNext = m_FocusItem;
				CPoint ptLast;
				GetItemOrigin(GetLastVisibleItem(), &ptLast);
				if (GetItemOrigin(m_FocusItem, &ptFocus) && GetScrollInfo(SB_VERT, &sf, SIF_PAGE))
				{
					int nsize = ptFocus.y + sf.nPage;
					if (nsize > rect.Height())
						nsize = rect.Height() - 1;

					if (nsize > ptLast.y)
						nsize = ptLast.y;

					if (nsize < rect.top)
						nsize = rect.top;
					hNext = GetItemFromPt(CPoint(1, nsize));
				}
			}

            break;
        }

    case VK_PRIOR:  
        {
			if (IsCTRLpressed() || !IsVisibleVScroll())
			{
				SendMessage(WM_VSCROLL, SB_TOP, 0);
				hNext = m_listVisible.GetHead();
			}
			else
			{
				SendMessage(WM_VSCROLL, SB_PAGEUP, 0);
				
				SCROLLINFO sf;
				sf.cbSize = sizeof(SCROLLINFO);
				sf.fMask = SIF_PAGE;

				CPoint ptFocus;
				hNext = m_FocusItem;
				if (GetItemOrigin(m_FocusItem, &ptFocus) && GetScrollInfo(SB_VERT, &sf, SIF_PAGE))
				{
					int nsize = ptFocus.y - sf.nPage;
					if (nsize < 0)
						nsize = 1;

					hNext = GetItemFromPt(CPoint(1, nsize));
				}
			}
            break;
        }
        
    case VK_HOME:  
		{
			// Home and Ctrl-Home work more like Excel
			//  and don't let user go to a hidden item
			if (IsCTRLpressed())
			{
				SendMessage(WM_VSCROLL, SB_TOP, 0);
				hNext = m_listVisible.GetHead();
			}

			break;
		}
        
    case VK_END:    
		{
			// End and Ctrl-End work more like Excel
			//  and don't let user go to a hidden item
			if (IsCTRLpressed())
			{
				SendMessage(WM_VSCROLL, SB_BOTTOM, 0);
				hNext = m_listVisible.GetTail();
			}

			break;
		}
        
    case VK_F2:
        OnEditItem(m_FocusItem, CPoint( -1, -1), VK_LBUTTON);
        break;

    case VK_SPACE:
		if (m_FocusItem && !ItemIsHeader(m_FocusItem))
		{
			SendMessageToParent(m_FocusItem, 0, CVN_NUMBER);
		}
        break;

	case VK_RETURN:
		if (m_FocusItem)
		{
			if (ItemIsHeader(m_FocusItem))
				Expand(m_FocusItem, CVE_TOGGLE);
			else
				SendMessageToParent(m_FocusItem, 0, CVN_ACTIVATE);
		}
        return;
    default:
        CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
        return;
    }

    if (hNext != m_FocusItem && hNext)
    {
        SetItemFocus(hNext);

		if (!ItemIsHeader(m_FocusItem) && bShift && (nChar == VK_DOWN || nChar == VK_UP))
		{
			if ((m_FocusItem->GetState() & CVIS_SELECTED) != CVIS_SELECTED)
				SendMessageToParent(m_FocusItem, 0, CVN_ACTIVATE);
		}

        if (!IsItemVisible(hNext))
        {
            switch (nChar)
            {
            case VK_RIGHT:  
            case VK_DOWN:   
                SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);  
                break;
                
            case VK_LEFT:   
            case VK_UP:     
                SendMessage(WM_VSCROLL, SB_LINEUP, 0);    
                break;

            }
            EnsureVisible(hNext); // Make sure item is visible
        }
        EnsureVisible(hNext); // Make sure item is visible
    }
}


void CContextCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!IsCTRLpressed() && m_MouseMode == MOUSE_NOTHING && nChar != VK_ESCAPE)
	{
		if (nChar == VK_RETURN && m_FocusItem)
		{
			if (ItemIsHeader(m_FocusItem))
				Expand(m_FocusItem, CVE_TOGGLE);
			else
				SendMessageToParent(m_FocusItem, 0, CVN_ACTIVATE);
		}
//		else if (nChar != VK_TAB && IsItemEditable(m_FocusItem))
//		OnEditItem(m_FocusItem, CPoint(-1, -1), nChar);

	}

	CWnd::OnChar(nChar, nRepCnt, nFlags);
}

void CContextCtrl::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    m_TitleTip.Hide();  // hide any titletips
//	CancelToolTips();
	CancelTracking();

	CWnd::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CContextCtrl::OnCancelMode()
{
	if (m_MouseMode == MOUSE_DRAGGING)
	{
		m_bMouseButtonDown = FALSE;
		ClipCursor(NULL);

		if (GetCapture()->GetSafeHwnd() == GetSafeHwnd())
		{
			ReleaseCapture();
			KillTimer(m_nVTimerID);
			m_nVTimerID = 0;
		}
		m_MouseMode = MOUSE_NOTHING;

		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	}
	CWnd::OnCancelMode();
}

void CContextCtrl::OnCaptureChanged(CWnd *pWnd)
{
    if (pWnd->GetSafeHwnd() == GetSafeHwnd())
        return;

    // kill timer if active
    if (m_nVTimerID != 0)
    {
        KillTimer(m_nVTimerID);
        m_nVTimerID = 0;
    }

    // Kill drag and drop if active
    if (m_MouseMode == MOUSE_DRAGGING)
        m_MouseMode = MOUSE_NOTHING;
}

void CContextCtrl::OnSysColorChange()
{
	Invalidate();

    CWnd::OnSysColorChange();
}

void CContextCtrl::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	Invalidate();

    m_nRowsPerWheelNotch = GetMouseScrollLines(); // Get the number of lines

    CWnd::OnSettingChange(uFlags, lpszSection);
}

LRESULT CContextCtrl::OnSetFont(WPARAM hFont, LPARAM lParam)
{
    LRESULT result = Default();

    // Get the logical font
    LOGFONT lf;
    if (!GetObject((HFONT) hFont, sizeof(LOGFONT), &lf))
        return result;

    // Store font as the global default
    memcpy(&m_Logfont, &lf, sizeof(LOGFONT));
	CSize OldCharSize(0, 0);
	CSize NewCharSize(0, 0);

    // Get the font size and hence the default item size
    CDC* pDC = GetDC();
    if (pDC)
    {
		CFont * pOldFont = pDC->SelectObject(&m_BoldFont);
		OldCharSize = pDC->GetTextExtent(_T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSATUVWXYZ"),52);
		pDC->SelectObject(pOldFont);

        m_Font.DeleteObject();
        m_Font.CreateFontIndirect(&m_Logfont);

		lf.lfWeight = SELECTED_ITEM_FONT_WEIGHT;
		m_BoldFont.DeleteObject();
        m_BoldFont.CreateFontIndirect(&lf);

        pOldFont = pDC->SelectObject(&m_Font);

        TEXTMETRIC tm;
        pDC->GetTextMetrics(&tm);

        m_nMargin = pDC->GetTextExtent(_T(" "), 1).cx;
		if (m_nMargin < 3)
			m_nMargin = 3;

        pDC->SelectObject(pOldFont);

		pDC->SelectObject(&m_BoldFont);
		NewCharSize = pDC->GetTextExtent(_T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSATUVWXYZ"),52);
		pDC->SelectObject(pOldFont);

        ReleaseDC(pDC);

        m_nDefHeaderHeight = tm.tmHeight + tm.tmExternalLeading + (tm.tmHeight / 3) + m_nExtSize;// + 2 * m_nMargin;
        m_nDefItemHeight   = m_nIconSize + 2*m_nExtSize + 2*m_nMargin + 2 * m_nBorder;
    }

	if (OldCharSize != NewCharSize)
	{
		double fRatioH = (double)NewCharSize.cy / (double)OldCharSize.cy;
		for (POSITION pos = m_list.GetHeadPosition(); pos != NULL; )
		{
			HCONTEXTITEM hItem = m_list.GetNext(pos);
			int nNewHeight = (int)(hItem->GetHeight() * fRatioH);
			if (ItemIsHeader(hItem))
			{
				if (nNewHeight < m_nDefHeaderHeight)
					nNewHeight = m_nDefHeaderHeight;
			}
			else
			{
				if (nNewHeight < m_nDefItemHeight)
					nNewHeight = m_nDefItemHeight;
			}
			hItem->SetHeight(nNewHeight);
		}
	}

    if (::IsWindow(GetSafeHwnd()))
        InvalidateRect(NULL);

    return result;
}

LRESULT CContextCtrl::OnGetFont(WPARAM hFont, LPARAM lParam)
{
    return (LRESULT) (HFONT) m_Font;
}

BOOL CContextCtrl::OnNeedToolTipText(UINT id, NMHDR * pTTTStruct, LRESULT * pResult)
{
    TOOLTIPTEXT * pTTT = (TOOLTIPTEXT*)pTTTStruct;
    HCONTEXTITEM hItem = (HCONTEXTITEM)pTTTStruct->idFrom;
	if (!hItem)
		return false;

    pTTT->hinst = 0;
	pTTT->lpszText = _tcsdup(hItem->GetTipText());
	return true;
}


void CContextCtrl::OnEndInPlaceEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMCVDISPINFO *	pcvDispInfo = (NMCVDISPINFO *)pNMHDR;
    CVITEM *		pcvItem = &pcvDispInfo->item;

    *pResult = 0;
    // In case OnEndInPlaceEdit called as window is being destroyed
    if (!IsWindow(GetSafeHwnd()))
        return;

	CString strOld = pcvItem->hItem->GetText();
    OnEndEditItem(pcvItem->hItem, pcvItem->strText);

	HCONTEXTITEM hItem = pcvItem->hItem;
    if (!SendMessageToParentDisp(pcvItem, CVN_ENDLABELEDIT))
	{
		if (m_list.Find(hItem) && m_FocusItem == hItem)
		{
			SetItemText(hItem, strOld);
			OnEditItem(m_FocusItem, CPoint( -1, -1), pcvItem->lParam);
		}
	}
	*pResult = 0;
}

void CContextCtrl::UpdateWindow()
{
	if (m_bAllowDraw && ::IsWindow(GetSafeHwnd()))
		CWnd::UpdateWindow();
}

void CContextCtrl::OnDestroy() 
{
//	DestroyUpDownBmp();
	if (m_nMTimerID)
	{
		KillTimer(m_nMTimerID);
		m_nMTimerID = 0;
	}
	if (m_nVTimerID)
	{
		KillTimer(m_nVTimerID);
		m_nVTimerID = 0;
	}
	DeleteAllItems();

	CWnd::OnDestroy();
}

void CContextCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);

	HCONTEXTITEM hItem = m_FocusItem ? m_FocusItem : m_hPrevFocusItem;
	if (pOldWnd != this && !hItem)
	{
		hItem = GetFirstVisibleItem();
	}
	if (hItem && m_list.Find(hItem))
		SetItemFocus(hItem);
}

bool CContextCtrl::DrawIcon(CDC * pDC, CRect ImageRect, HCONTEXTITEM hItem)
{
	if (!hItem || !pDC)
		return false;

	DRAWITEMSTRUCT dis;
	memset(&dis, 0, sizeof(DRAWITEMSTRUCT));
	dis.CtlType = ODT_CONTEXTVIEW;
	dis.CtlID = GetDlgCtrlID();
	dis.itemID = (UINT)hItem;
	dis.itemData = hItem->GetData();

	dis.rcItem.left = ImageRect.left;
	dis.rcItem.top = ImageRect.top;
	dis.rcItem.right = ImageRect.right;
	dis.rcItem.bottom = ImageRect.bottom;

	dis.itemAction = ODA_DRAWENTIRE;

	dis.itemState = ODS_DEFAULT;
	
	if ((hItem->GetState() & CVIS_FOCUSED) == CVIS_FOCUSED)
		dis.itemState = ODS_FOCUS;

	if ((hItem->GetState() & CVIS_SELECTED) == CVIS_SELECTED)
		dis.itemState |= ODS_SELECTED;

	if ((hItem->GetState() & CVIS_DISABLED) == CVIS_DISABLED)
		dis.itemState |= ODS_DISABLED;
	
	dis.hDC = *pDC;

	CWnd * pOwner = GetOwner();
	if (pOwner && IsWindow(pOwner->m_hWnd))
		return pOwner->SendMessage(WM_DRAWITEM, 0, LPARAM(&dis)) != FALSE;

	return false;
}

#if _MSC_VER >= 1300
	void CContextCtrl::OnActivateApp(BOOL bActive, DWORD hTask) 
#else
	void CContextCtrl::OnActivateApp(BOOL bActive, HTASK hTask) 
#endif
{
	CWnd::OnActivateApp(bActive, hTask);
	InvalidateRect(NULL);	
}

void CContextCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);
	m_hPrevFocusItem = m_FocusItem;
	SetItemFocus(0);
}
