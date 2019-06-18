#include "stdafx.h"
#include "resource.h"
#include "manual.h"
#include "nameconsts.h"
#include "image5.h"
#include "data5.h"
#include "misc_utils.h"
#include "misc_templ.h"
#include "core5.h"
#include "Classes5.h"
#include "docview5.h"
#include "calibrint.h"
#include "class_utils.h"
#include "stdio.h"
#include "fcntl.h"
#include "sys/stat.h"
#include "LearningInfo.h"
#include "StatFile.h"
#include "MessageException.h"
#include "PaintDC.h"
#include "win_main.h"
#include "../../Chromo/ChromosEx/consts.h"
#include "CarioInt.h"
#include "ChObjUtils.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
//CManualWnd
////////////////////////////////////////////////////////////////////////////////////////////////////

//const int m_nAppend = 50;
class CManualWnd : public ComObjectBase, public CWinImpl, public IScrollZoomSite
{
	int m_nResultCode; // Set in EndDialog
	int m_nClassified,m_nAll; // For information below caryogram
//	_point m_pt; // Point of click
	_rect m_rcObj; // rectangle of object in screen coordinates
	bool m_bLButDown;
	_bstr_t m_bstrTextCls;   // Text 1 in left part - "Classified %d"
	_bstr_t m_bstrTextAll;   // Text 2 in left part - "Total %d"
	_bstr_t m_bstrComment1;  // Comment 1 in right part - "1 chromosome"
	_bstr_t m_bstrComment2;  // Comment 2 in right part - "2 chromosomes"
	_bstr_t m_bstrComment3;  // Comment 3 in right part - ">2 chromosomes"
	_size m_szText;     // Size of one text item in left part of cellar
	_size m_szComment;  // Size of one text item in right part of cellar
	int m_nColorWidth;  // Width of color rectangle in right part of cellar (height is m_szComment.cy)
	int m_nMarg;
	int m_nSBHeight,m_nCellarHeight;
	_size m_szMin;//,m_szMax;
	IUnknownPtr m_punkBaseView;
	IUnknownPtr m_punkDoc;
	IUnknownPtr m_punkCarioView;
	HWND m_hwndStatic,m_hwndStatusBar;
	bool CreateChildView();
	double m_dZoom;
	double GetZoom() {return m_dZoom;}
	void CalcZoom(int &cx, int &cy);
	void CalcZoomOnSize(int cxWnd, int cyWnd);
	_size CorrectSizesHeight(int nHeight);
	_size CorrectSizesWidth(int nWidth);
	_point CalculatePosition(_size sz);
	int m_nAppend,m_nWidth1,m_nWidth2,m_nMinX;
	void CalculateAppendAndMinPos();
	void EnableOwner();
	_size ClientSizeByCarioSize(_size szCario);
public:
	void SetHwnd(HWND hwnd) {m_hwnd = hwnd;}
	CManualWnd(_rect rc, IUnknown *punkBaseView, int nClassified, int nAll)
	{
		m_dZoom = 1.;
		m_nResultCode = -1;
		m_rcObj = rc;
		m_nClassified = nClassified;
		m_nAll = nAll;
		m_punkBaseView = punkBaseView;
		m_bLButDown = false;
	}
	~CManualWnd()
	{
#if defined(_DEBUG)
		m_nRefCounter = 0;
#endif
	}
	route_unknown();
	virtual LRESULT	DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );
	virtual void DoPaint();
	virtual UINT idd() {return IDD_MENU;}
	virtual bool DoInitDialog();
	virtual HRESULT DoQueryAggregates(const IID &iid, void **ppRet);
	virtual IUnknown *DoGetInterface( const IID &iid );
	virtual void OnSize(WORD fwSizeType, unsigned cx, unsigned cy);
	virtual void OnSizing(unsigned fwSide, LPRECT lpRect);
	virtual void OnGetMinMaxInfo( LPMINMAXINFO lpMinMaxInfo );
	virtual void OnLButtonDown(WORD fwKeys, unsigned x, unsigned y);
	virtual void OnLButtonUp(WORD fwKeys, unsigned x, unsigned y);
	virtual void OnRButtonUp(WORD fwKeys, unsigned x, unsigned y);
	virtual void OnMouseMove(WORD fwKeys, unsigned x, unsigned y);
	virtual void OnKeyDown(WORD wVirtKey, ULONG lKeyData);
	virtual void OnKeyUp(WORD wVirtKey, ULONG lKeyData);
	virtual void OnDestroy();
	virtual void OnClose();
	// Implementing dialog-style behavior with simple window
	int DoModal();
	void EndDialog(int nResult);
	//IScrollZoomSite
	com_call GetScrollBarCtrl( DWORD sbCode, HANDLE *phCtrl ) {*phCtrl = NULL; return S_OK;};
	com_call GetClientSize( SIZE *psize )
	{
		psize->cx = psize->cy = 0;
		return S_OK;
	}
	com_call SetClientSize( SIZE sizeNew ) {return S_OK;}
	com_call GetScrollPos( POINT *pptPos ) {pptPos->x = pptPos->y = 0; return S_OK;}
	com_call SetScrollPos( POINT ptPos ) {return S_OK;}
	com_call GetVisibleRect( RECT *pRect )
	{
		::GetClientRect(hwnd(), pRect);
		return S_OK;
	}
	com_call EnsureVisible( RECT rect ) {return S_OK;}
	com_call GetZoom( double *pfZoom ) {*pfZoom = m_dZoom; return S_OK;}
	com_call SetZoom( double fZoom ) {return S_OK;}
	com_call SetScrollOffsets( RECT rcOffsets ) {return S_OK;}
	com_call SetAutoScrollMode( BOOL bSet ) {return S_OK;}
	com_call GetAutoScrollMode( BOOL* pbSet ) {*pbSet = FALSE; return S_OK;}
	com_call LockScroll( BOOL bLock ) {return S_OK;}
	com_call UpdateScroll( WORD sbCode ) {return S_OK;}
};

LRESULT CALLBACK ManualWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CWinImpl *p;
	if (nMsg == WM_NCCREATE)
	{
		CREATESTRUCT *pcs = (CREATESTRUCT *)lParam;
		p = (CWinImpl *)pcs->lpCreateParams;
		((CManualWnd *)p)->SetHwnd(hWnd);
		::SetWindowLong(hWnd, GWLP_USERDATA, (LONG)p);
	}
	p = (CWinImpl *)::GetWindowLong(hWnd, GWLP_USERDATA);
	return p->DoMessage(nMsg, wParam, lParam);
}

bool CManualWnd::CreateChildView()
{
	// Create static (as parent)
	_bstr_t bstrProgId(szCarioViewProgID);
	CLSID	clsid;
	::CLSIDFromProgID(bstrProgId, &clsid);
	HRESULT hr = ::CoCreateInstance(clsid, Unknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void **)&m_punkCarioView);
	if (FAILED(hr))
		return false;
	IViewPtr sptrBaseView(m_punkBaseView);
	sptrBaseView->GetDocument(&m_punkDoc);
	IViewPtr sptrCarioView(m_punkCarioView);
	{
		ICarioViewPtr ptrCario( sptrCarioView );
//MessageBox(hwnd(), "111", "111", MB_OK);
		if( ptrCario )
			ptrCario->SetMode( cvmMenu );
//MessageBox(hwnd(), "222", "222", MB_OK);

	}
	sptrCarioView->Init(m_punkDoc, NULL);
	IWindowPtr sptrWndView(m_punkCarioView);
	_rect rcClient;
	::GetClientRect(hwnd(), &rcClient);
	sptrWndView->CreateWnd(hwnd(), rcClient, WS_CHILD|WS_VISIBLE|WS_DISABLED, 1);
	HWND hwnd;
	sptrWndView->GetHandle((HANDLE*)&hwnd);
	SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_CLIENTEDGE);
	return true;
}

IUnknown *CManualWnd::DoGetInterface( const IID &iid )
{
	if (iid == IID_IScrollZoomSite)
		return (IScrollZoomSite *)this;
	else
	{
		IUnknown *punk = DoGetWinInterface(iid);
		if (punk) return punk;
		return ComObjectBase::DoGetInterface(iid);
	}
}

HRESULT CManualWnd::DoQueryAggregates(const IID &iid, void **ppRet)
{
	if (iid == IID_IDataContext)
		return m_punkBaseView->QueryInterface(iid, ppRet);
	return m_punkCarioView->QueryInterface(iid, ppRet);
}

void CManualWnd::CalcZoomOnSize(int cxWnd, int cyWnd)
{
	int cxCario,cyCario;
	ICarioViewPtr sptrCario(m_punkCarioView);
	sptrCario->GetCarioSizes(&cxCario, &cyCario);
	double dZoomX = double(cxWnd)/double(cxCario);
	double dZoomY = double(cyWnd)/double(cyCario);
	m_dZoom = min(dZoomX,dZoomY);
}

void CManualWnd::CalcZoom(int &cx, int &cy)
{
	int cxScreen = ::GetSystemMetrics(SM_CXFULLSCREEN);
	int cyScreen = ::GetSystemMetrics(SM_CYFULLSCREEN);
	if (cx > cxScreen / 2 || cy > cyScreen / 2)
	{
		double dZoomX = double(cx)/double(cxScreen)/2.;
		double dZoomY = double(cy)/double(cyScreen)/2.;
		m_dZoom = max(dZoomX,dZoomY);
		cx = int(cx/m_dZoom);
		cy = int(cy/m_dZoom);
	}
}

_size CManualWnd::ClientSizeByCarioSize(_size szCario)
{
	return _size(szCario.cx+2*m_nMarg,szCario.cy+m_nAppend+2*m_nMarg);
}

const int nMinSize = 20; // Sizes can't be less than some minimal value
_size CManualWnd::CorrectSizesHeight(int cy)
{
	// Calculate default size
	int cxCario,cyCario;
	ICarioViewPtr sptrCario(m_punkCarioView);
	sptrCario->GetCarioSizes(&cxCario, &cyCario);
	_rect rcDef(_point(0,0), ClientSizeByCarioSize(_size(cxCario,cyCario)));
	_rect rcDefClient = rcDef;
	::AdjustWindowRectEx(&rcDef, ::GetWindowLong(hwnd(), GWL_STYLE), FALSE, ::GetWindowLong(hwnd(), GWL_EXSTYLE));
	_rect rcWnd = rcDef;
	// apply limits
/*	int cyMax = ::GetValueInt(::GetAppUnknown(), "Classification\\ManualWnd", "HeightMax", 0);
	if (cyMax == 0)
		cyMax = GetSystemMetrics(SM_CYFULLSCREEN);
	cy = min(cy,cyMax);*/
	// now calculate size by saved settings
	int dx = rcDef.width()-rcDefClient.width();
	int dy = rcDef.height()-rcDefClient.height();
	int cyCarioNow = cy - dy - m_nAppend - m_nMarg;
	if (cyCarioNow < nMinSize)
		cyCarioNow = nMinSize;
	int cxCarioNow = cxCario*cyCarioNow/cyCario;
	_rect rc(_point(0,0), ClientSizeByCarioSize(_size(cxCarioNow,cyCarioNow)));
	::AdjustWindowRectEx(&rc, ::GetWindowLong(hwnd(), GWL_STYLE), FALSE, ::GetWindowLong(hwnd(), GWL_EXSTYLE));
	return rc.size();
}

_size CManualWnd::CorrectSizesWidth(int cx)
{
	// Calculate default size
	int cxCario,cyCario;
	ICarioViewPtr sptrCario(m_punkCarioView);
	sptrCario->GetCarioSizes(&cxCario, &cyCario);
	_rect rcDef(_point(0,0), ClientSizeByCarioSize(_size(cxCario,cyCario)));
	_rect rcDefClient = rcDef;
	::AdjustWindowRectEx(&rcDef, ::GetWindowLong(hwnd(), GWL_STYLE), FALSE, ::GetWindowLong(hwnd(), GWL_EXSTYLE));
	_rect rcWnd = rcDef;
	// now calculate size by saved settings
	int dx = rcDef.width()-rcDefClient.width();
	int dy = rcDef.height()-rcDefClient.height();
	int cxCarioNow = cx - dx - 2*m_nMarg;
//	if (cxCarioNow > nMinSize)
	{
		int cyCarioNow = cyCario*cxCarioNow/cxCario;
		_rect rc(_point(0,0), ClientSizeByCarioSize(_size(cxCarioNow,cyCarioNow)));
//		_rect rc(0,0,cxCarioNow,cyCarioNow+m_nAppend);
		::AdjustWindowRectEx(&rc, ::GetWindowLong(hwnd(), GWL_STYLE), FALSE, ::GetWindowLong(hwnd(), GWL_EXSTYLE));
	return rc.size();
	}
//	else
//		return nMinSize;
}

_point CManualWnd::CalculatePosition(_size sz)
{
	IWindowPtr sptrView(m_punkBaseView);
	HWND hwnd;
	sptrView->GetHandle((HANDLE *)&hwnd);
	RECT rcWnd;
	GetWindowRect(hwnd, &rcWnd);
	_point ptMax(::GetSystemMetrics(SM_CXFULLSCREEN), ::GetSystemMetrics(SM_CYFULLSCREEN));
	_point pt;
	if (ptMax.x-m_rcObj.right >= sz.cx)
	{ // Window suite on the right of object
		if (ptMax.y-m_rcObj.bottom >= sz.cy) // Suite on the bottom
			pt = _point(m_rcObj.right, m_rcObj.bottom);
		else if (m_rcObj.top > sz.cy) // Suite on top
			pt = _point(m_rcObj.right, m_rcObj.top-sz.cy);
		else // Not suite neither on top nor on bottom
			pt = _point(m_rcObj.right, (sz.cy-m_rcObj.height())/2);
	}
	else if (m_rcObj.left >= sz.cx) // Suite on the left
	{
		if (ptMax.y-m_rcObj.bottom >= sz.cy) // Suite on the bottom
			pt = _point(m_rcObj.left-sz.cx, m_rcObj.bottom);
		else if (m_rcObj.top >= sz.cy) // Suite on the top
			pt = _point(m_rcObj.left-sz.cx, m_rcObj.top-sz.cy);
		else // Not suite neither on top nor on bottom
			pt = _point(m_rcObj.left-sz.cx, (sz.cy-m_rcObj.height())/2);
	}
	else // Not suite neither on the left nor on the right
	{
		if (ptMax.y-m_rcObj.bottom >= sz.cy) // Suite on the bottom
			pt = _point((sz.cx-m_rcObj.width())/2, m_rcObj.bottom);
		else if (m_rcObj.top >= sz.cy) // Suite on the top
			pt = _point((sz.cx-m_rcObj.width())/2, m_rcObj.top-sz.cy);
		else // Not suite neither on top nor on bottom
			pt = _point(m_rcObj.right, m_rcObj.bottom);
	}
	int nRightMinSpace = GetValueInt(GetAppUnknown(), "Classification\\ManualWnd", "MinRightMargin", 100);
	if (sz.cx < nRightMinSpace) nRightMinSpace = sz.cx;
	int nBottomMinSpace = GetValueInt(GetAppUnknown(), "Classification\\ManualWnd", "MinBottomMargin", 50);
	if (sz.cy < nBottomMinSpace) nBottomMinSpace = sz.cy;
	if (pt.x > ptMax.x-nRightMinSpace) pt.x = ptMax.x-nRightMinSpace;
	if (pt.y > ptMax.y-nBottomMinSpace) pt.y = ptMax.y-nBottomMinSpace;
	if (pt.x < 0) pt.x = 0;
	if (pt.y < 0) pt.y = 0;
//	char szBuff[200];
//	sprintf(szBuff,"%d-%d,%d-%d", m_rcObj.left, m_rcObj.right, m_rcObj.top, m_rcObj.bottom);
//	MessageBox(NULL, szBuff, "test", MB_OK);
	return pt;
}

void CManualWnd::CalculateAppendAndMinPos()
{
	HDC hdc = GetDC(hwnd());
	HFONT hf = (HFONT)GetStockObject(SYSTEM_FONT);
	HFONT hfOld = (HFONT)SelectObject(hdc, hf);
	char szText[200];
	// Left part of comment
	_rect rc1(0,0,0,0),rc2(0,0,0,0);
	sprintf(szText, (const char *)m_bstrTextCls, 100);
	DrawText(hdc, szText, -1, &rc1, DT_CALCRECT|DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	sprintf(szText, (const char *)m_bstrTextAll, 100);
	DrawText(hdc, szText, -1, &rc2, DT_CALCRECT|DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	m_szText = _size(max(rc1.width(),rc2.width()), max(rc1.height(),rc2.height()));
	// Right part of comment
	_rect rc3(0,0,0,0),rc4(0,0,0,0),rc5(0,0,0,0);
	DrawText(hdc, (const char *)m_bstrComment1, -1, &rc3, DT_CALCRECT|DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	DrawText(hdc, (const char *)m_bstrComment2, -1, &rc4, DT_CALCRECT|DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	DrawText(hdc, (const char *)m_bstrComment3, -1, &rc5, DT_CALCRECT|DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	m_szComment = _size(max(max(rc3.width(),rc4.width()),rc5.width()), max(max(rc3.height(),rc4.height()),rc5.height()));
	m_nColorWidth = m_szComment.cy;
	m_nMarg = 4;
	m_nCellarHeight = 2*max(2*m_szText.cy,3*m_szComment.cy);
	m_nMinX = m_szText.cx+m_szComment.cx+m_nColorWidth+3*m_nMarg;
	SelectObject(hdc, hfOld);
	ReleaseDC(hwnd(),hdc);
}

bool CManualWnd::DoInitDialog()
{
	// Load string
	SetWindowText(hwnd(), _LoadString(IDS_TITLE));
	m_bstrTextCls = _LoadString(IDS_TEXT1);
	m_bstrTextAll = _LoadString(IDS_TEXT2);
	m_bstrComment1 = _LoadString(IDS_1CHROMOSOME);
	m_bstrComment2 = _LoadString(IDS_2CHROMOSOME);
	m_bstrComment3 = _LoadString(IDS_3CHROMOSOME);
	CalculateAppendAndMinPos(); // Calculate "cellar"
	// Init status bar
	m_hwndStatusBar = CreateWindowEx( 
		0,                       // no extended styles 
		STATUSCLASSNAME,         // name of status bar class 
		(LPCTSTR) NULL,          // no text when first created 
		SBARS_SIZEGRIP |         // includes a sizing grip 
		WS_CHILD|WS_VISIBLE,                // creates a child window 
		0, 0, 0, 0,              // ignores size and position 
		hwnd(),              // handle to parent window 
		(HMENU)1,       // child window identifier 
		app::instance()->handle(),                   // handle to application instance 
		NULL);                  // no window creation data 
	RECT rcSB; 
	::GetWindowRect(m_hwndStatusBar, &rcSB);
	m_nSBHeight = rcSB.bottom-rcSB.top;
	m_nAppend = m_nCellarHeight+m_nSBHeight;
	// Calculate initial size and position
	const int n = 4;
	bool b = CreateChildView();
	// Determine the size using saved values
	// Calculate default size
	int cxCario,cyCario;
	ICarioViewPtr sptrCario(m_punkCarioView);
	sptrCario->GetCarioSizes(&cxCario, &cyCario);
	_rect rcDef(_point(0,0), ClientSizeByCarioSize(_size(cxCario,cyCario)));
	_rect rcDefClient = rcDef;
	::AdjustWindowRectEx(&rcDef, ::GetWindowLong(hwnd(), GWL_STYLE), FALSE, ::GetWindowLong(hwnd(), GWL_EXSTYLE));
	_rect rcWnd = rcDef;
	// now calculate size by saved settings
	int cy = ::GetValueInt(::GetAppUnknown(), "Classification\\ManualWnd", "Height", 0);
	int cyMax = ::GetValueInt(::GetAppUnknown(), "Classification\\ManualWnd", "HeightMax", 0);
	if (cyMax == 0)
		cyMax = GetSystemMetrics(SM_CYFULLSCREEN);
	cy = min(cy,cyMax);
	int dx = rcDef.width()-rcDefClient.width();
	int dy = rcDef.height()-rcDefClient.height();
	if (cy > 0 || cyCario > GetSystemMetrics(SM_CYFULLSCREEN))
	{
		if (cy == 0) cy = GetSystemMetrics(SM_CYFULLSCREEN);
		if (cy <= dy+m_nAppend+2*m_nMarg+nMinSize)
			cy = dy+m_nAppend+2*m_nMarg+nMinSize+1;
		int cyCarioNow = cy - dy - m_nAppend - 2*m_nMarg;
		if (cyCarioNow > nMinSize)
		{
			int cxCarioNow = cxCario*cyCarioNow/cyCario;
			_rect rc(_point(0,0), ClientSizeByCarioSize(_size(cxCarioNow,cyCarioNow)));
			::AdjustWindowRectEx(&rc, ::GetWindowLong(hwnd(), GWL_STYLE), FALSE, ::GetWindowLong(hwnd(), GWL_EXSTYLE));
			rcWnd = rc;
		}
	}
	m_szMin = _size(m_nMinX+dx,dy+m_nAppend+2*m_nMarg+nMinSize+1);
	_point pt = CalculatePosition(_size(rcWnd.width(), rcWnd.height()));
	::SetWindowPos(hwnd(), NULL, pt.x, pt.y, rcWnd.width(), rcWnd.height(), SWP_NOZORDER);
	return b;
}

void CManualWnd::OnGetMinMaxInfo(LPMINMAXINFO lpMinMaxInfo)
{
	lpMinMaxInfo->ptMaxSize.x = lpMinMaxInfo->ptMaxTrackSize.x = GetSystemMetrics(SM_CXFULLSCREEN);
	lpMinMaxInfo->ptMaxSize.y = lpMinMaxInfo->ptMaxTrackSize.y = GetSystemMetrics(SM_CYFULLSCREEN);
	lpMinMaxInfo->ptMinTrackSize.x = min(m_szMin.cx,lpMinMaxInfo->ptMaxSize.x);
	lpMinMaxInfo->ptMinTrackSize.y = min(CorrectSizesWidth(m_nMinX).cy,lpMinMaxInfo->ptMaxSize.y);
}

void CManualWnd::OnSize(WORD fwSizeType, unsigned cx, unsigned cy)
{
	IWindowPtr sptrWnd(m_punkCarioView);
	HWND hwndV;
	sptrWnd->GetHandle((HANDLE *)&hwndV);
	int cy1 = cy-m_nAppend-2*m_nMarg;
	if (cy1 < 0) cy1 = 0;
	int cx1 = cx-2*m_nMarg;
	if (cx1 < 0) cx1 = 0;
	CalcZoomOnSize(cx1,cy1);
	::SetWindowPos(hwndV, NULL, m_nMarg, m_nMarg, cx1, cy1, SWP_NOZORDER);
	::InvalidateRect(hwnd(), NULL, FALSE);
	::SetWindowPos(m_hwndStatusBar, NULL, 0, cy-m_nSBHeight, cx, m_nSBHeight, SWP_NOZORDER|SWP_NOMOVE);
}

void CManualWnd::OnSizing(unsigned fwSide, LPRECT lpRect)
{
	_size sz;
	if (fwSide == WMSZ_RIGHT || fwSide == WMSZ_LEFT)
		sz = CorrectSizesWidth(lpRect->right-lpRect->left);
	else
		sz = CorrectSizesHeight(lpRect->bottom-lpRect->top);
	if (fwSide == WMSZ_BOTTOM || fwSide == WMSZ_BOTTOMRIGHT || fwSide == WMSZ_RIGHT)
	{
		lpRect->right = lpRect->left+sz.cx;
		lpRect->bottom = lpRect->top+sz.cy;
	}
	else if (fwSide == WMSZ_TOP || fwSide == WMSZ_TOPRIGHT)
	{
		lpRect->right = lpRect->left+sz.cx;
		lpRect->top = lpRect->bottom-sz.cy;
	}
	else if (fwSide == WMSZ_TOPLEFT || fwSide == WMSZ_LEFT)
	{
		lpRect->left = lpRect->right-sz.cx;
		lpRect->top = lpRect->bottom-sz.cy;
	}
	else if (fwSide == WMSZ_BOTTOMLEFT)
	{
		lpRect->left = lpRect->right-sz.cx;
		lpRect->bottom = lpRect->top+sz.cy;
	}
}

void CManualWnd::OnLButtonDown(WORD fwKeys, unsigned x, unsigned y)
{
	m_bLButDown = true;
}

void CManualWnd::OnLButtonUp(WORD fwKeys, unsigned x, unsigned y)
{
	if (!m_bLButDown) return;
	IWindowPtr sptrWnd(m_punkCarioView);
	HWND hwndCario;
	sptrWnd->GetHandle((HANDLE *)&hwndCario);
	RECT rcClient;
	::GetClientRect(hwndCario, &rcClient);
	if (int(x) < rcClient.right && int(y) < rcClient.bottom)
	{
		double dZoom = GetZoom();
		_point pt(unsigned(x/dZoom), unsigned(y/dZoom));
		ICarioViewPtr sptrCario(m_punkCarioView);
		int nLine,nCell;
		sptrCario->GetCellByPoint(pt,&nLine,&nCell,NULL,FALSE);
		int nClass;
		sptrCario->GetCellClass(nLine,nCell,&nClass);
		EndDialog(nClass+1); // 0 - unknown, 1,2,... - classes
	}
	else
		EndDialog(-1); // Save current class
}

void CManualWnd::OnRButtonUp(WORD fwKeys, unsigned x, unsigned y)
{
	EndDialog(-1); // Save current class
}

void CManualWnd::OnMouseMove(WORD fwKeys, unsigned x, unsigned y)
{
	IWindowPtr sptrWnd(m_punkCarioView);
	HWND hwndCario;
	sptrWnd->GetHandle((HANDLE *)&hwndCario);
	RECT rcClient;
	::GetClientRect(hwndCario, &rcClient);
	if (int(x) < rcClient.right && int(y) < rcClient.bottom)
	{
		double dZoom = GetZoom();
		_point pt(unsigned(x/dZoom), unsigned(y/dZoom));
		ICarioViewPtr sptrCario(m_punkCarioView);
		int nLine,nCell;
		sptrCario->GetCellByPoint(pt,&nLine,&nCell,NULL,FALSE);
		if (nLine >= 0 && nCell >= 0)
		{
			sptrCario->SetActiveLine(nLine);
			sptrCario->SetActiveCell(nCell);
		}
	}
}

void CManualWnd::OnKeyDown(WORD wVirtKey, ULONG lKeyData)
{
	if (wVirtKey == VK_UP || wVirtKey == VK_DOWN || wVirtKey == VK_LEFT || wVirtKey == VK_RIGHT)
	{
		ICarioViewPtr sptrCario(m_punkCarioView);
		int nLine,nCell;
		sptrCario->GetActiveLine(&nLine);
		sptrCario->GetActiveCell(&nCell);
		if (nLine < 0 || nCell < 0)
			nLine = nCell = 0;
		else
		{
			int nLinesCount,nCellsCount;
			switch(wVirtKey)
			{
			case VK_UP:
				if (nLine == 0)
					sptrCario->GetLineCount(&nLine);
				sptrCario->SetActiveLine(--nLine);
				break;
			case VK_DOWN:
				sptrCario->GetLineCount(&nLinesCount);
				if (nLine >= nLinesCount-1)
					sptrCario->SetActiveLine(nLine = 0);
				else
					sptrCario->SetActiveLine(++nLine);
				break;
			case VK_LEFT:
				if (nCell == 0)
					sptrCario->GetCellCount(nLine, &nCell);
				sptrCario->SetActiveCell(--nCell);
				break;
			case VK_RIGHT:
				sptrCario->GetCellCount(nLine, &nCellsCount);
				if (nCell >= nCellsCount-1)
					sptrCario->SetActiveCell(0);
				else
					sptrCario->SetActiveCell(++nCell);
				break;
			}
			if (wVirtKey == VK_UP || wVirtKey == VK_DOWN)
			{
				sptrCario->GetCellCount(nLine, &nCellsCount);
				if (nCell >= nCellsCount)
					sptrCario->SetActiveCell(max(--nCell,0));
			}
		}
	}
}

void CManualWnd::OnKeyUp(WORD wVirtKey, ULONG lKeyData)
{
	if (wVirtKey == VK_ESCAPE)
		EndDialog(-1);
	else if (wVirtKey == VK_RETURN)
	{
		ICarioViewPtr sptrCario(m_punkCarioView);
		int nLine,nCell;
		sptrCario->GetActiveLine(&nLine);
		sptrCario->GetActiveCell(&nCell);
		if (nLine >= 0 && nCell >= 0)
		{
			int nClass;
			sptrCario->GetCellClass(nLine,nCell,&nClass);
			EndDialog(nClass+1);
		}
		else
			EndDialog(-1);
	}
}

LRESULT	CManualWnd::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	switch( nMsg )
	{
	case WM_INITDIALOG:     DoInitDialog(); return FALSE;
	case WM_CREATE:         DoInitDialog(); return 0;
	case WM_PAINT:          DoPaint(); return TRUE;
	case WM_COMMAND:        DoCommand( wParam ); return 1;
	case WM_NOTIFY:         DoNotify( wParam, (NMHDR*)lParam ); return TRUE;
	case WM_GETINTERFACE:   return (LRESULT)(CWinImpl*)this;
	case WM_SIZE:           OnSize(wParam,LOWORD(lParam),HIWORD(lParam)); return TRUE;
	case WM_SIZING:         OnSizing(wParam,(LPRECT)lParam); return TRUE;
	case WM_GETMINMAXINFO:	OnGetMinMaxInfo((LPMINMAXINFO)lParam); return TRUE;
	case WM_LBUTTONDOWN:    OnLButtonDown(wParam,LOWORD(lParam),HIWORD(lParam)); return TRUE;
	case WM_LBUTTONUP:      OnLButtonUp(wParam,LOWORD(lParam),HIWORD(lParam)); return TRUE;
	case WM_RBUTTONUP:      OnRButtonUp(wParam,LOWORD(lParam),HIWORD(lParam)); return TRUE;
	case WM_MOUSEMOVE:      OnMouseMove(wParam,LOWORD(lParam),HIWORD(lParam)); return TRUE;
	case WM_KEYDOWN:        OnKeyDown(wParam, lParam); return TRUE;
	case WM_KEYUP:          OnKeyUp(wParam, lParam); return TRUE;
	case WM_CLOSE:          OnClose(); return CWinImpl::DoMessage(nMsg, wParam, lParam);
	case WM_DESTROY:        OnDestroy(); return FALSE;
	default:
		return CWinImpl::DoMessage(nMsg, wParam, lParam);
	}
}

void CManualWnd::DoPaint()
{
	paint_dc dc(hwnd());
	HFONT hf = (HFONT)GetStockObject(SYSTEM_FONT);
	HFONT hfOld = (HFONT)SelectObject(dc.hdc, hf);
	_brush br(::CreateSolidBrush(::GetSysColor(COLOR_BTNFACE)));
	FillRect(dc.hdc, &dc.ps.rcPaint, br);
	RECT rcClient;
	GetClientRect(hwnd(), &rcClient);
	SetBkMode(dc.hdc, TRANSPARENT);
	char szText[200];
	// Draw left text strings
	int y1l = rcClient.bottom-m_nAppend;
	int y2l = y1l+m_nCellarHeight/2;
	int y3l = y1l+m_nCellarHeight;
	_rect rc1(rcClient.left+m_nMarg, y1l, rcClient.left+m_nMarg+m_szText.cx, y2l);
	sprintf(szText, (const char *)m_bstrTextCls, m_nClassified);
	DrawText(dc.hdc, szText, -1, &rc1, DT_SINGLELINE|DT_LEFT|DT_VCENTER);
	_rect rc2(rcClient.left+m_nMarg, y2l, rcClient.left+m_nMarg+m_szText.cx, y3l);
	sprintf(szText, (const char *)m_bstrTextAll, m_nAll);
	DrawText(dc.hdc, szText, -1, &rc2, DT_SINGLELINE|DT_LEFT|DT_VCENTER);
	// Draw right text strings
	int y1r = rcClient.bottom-m_nAppend;
	int y2r = y1r+m_nCellarHeight/3;
	int y3r = y1r+m_nCellarHeight*2/3;
	int y4r = y1r+m_nCellarHeight;
	_rect rc3(rcClient.right-m_szComment.cx-m_nMarg, y1r, rcClient.right-m_nMarg, y2r);
	DrawText(dc.hdc, (const char *)m_bstrComment1, -1, &rc3, DT_SINGLELINE|DT_LEFT|DT_VCENTER);
	_rect rc4(rcClient.right-m_szComment.cx-m_nMarg, y2r, rcClient.right-m_nMarg, y3r);
	DrawText(dc.hdc, (const char *)m_bstrComment2, -1, &rc4, DT_SINGLELINE|DT_LEFT|DT_VCENTER);
	_rect rc5(rcClient.right-m_szComment.cx-m_nMarg, y3r, rcClient.right-m_nMarg, y4r);
	DrawText(dc.hdc, (const char *)m_bstrComment3, -1, &rc5, DT_SINGLELINE|DT_LEFT|DT_VCENTER);
	// Draw color rectangles
	int x1r = rcClient.right-m_szComment.cx-m_nColorWidth-2*m_nMarg;
	int x2r = rcClient.right-m_szComment.cx-2*m_nMarg;
	COLORREF clr = ::GetValueColor(GetAppUnknown(), CARIO_VIEW_SHARED, "CellObject1", RGB( 140, 198, 198 ));
	_brush br1(::CreateSolidBrush(clr));
	_rect rc6(x1r, y1r+1, x2r, y2r-1);
	FillRect(dc.hdc, &rc6, br1);
	clr = ::GetValueColor(GetAppUnknown(), CARIO_VIEW_SHARED, "CellObject2", RGB( 148, 222, 116 ) );
	_brush br2(::CreateSolidBrush(clr));
	_rect rc7(x1r, y2r+1, x2r, y3r-1);
	FillRect(dc.hdc, &rc7, br2);
	clr = ::GetValueColor(GetAppUnknown(), CARIO_VIEW_SHARED, "CellObject3", RGB( 227, 129, 111 ) );
	_brush br3(::CreateSolidBrush(clr));
	_rect rc8(x1r, y3r+1, x2r, y4r-1);
	FillRect(dc.hdc, &rc8, br3);
	SelectObject(dc.hdc, hfOld);
}

void CManualWnd::OnClose()
{
	EnableOwner();
	m_nResultCode = -1;
}

void CManualWnd::OnDestroy()
{
	_rect rc;
	::GetWindowRect(hwnd(), &rc);
	::SetValue(::GetAppUnknown(), "Classification\\ManualWnd", "Height", rc.height());
	PostQuitMessage(m_nResultCode);
}

class _CDisableWnd
{
	HWND m_hwnd;
public:
	_CDisableWnd(HWND hwnd)
	{
		m_hwnd = hwnd;
		EnableWindow(m_hwnd, FALSE);
	};
	~_CDisableWnd()
	{
		EnableWindow(m_hwnd, TRUE);
	}
};

int CManualWnd::DoModal()
{
	HWND hwndOwner = 0;
	IApplicationPtr sptrApp(GetAppUnknown());
	if(sptrApp != 0)
		sptrApp->GetMainWindowHandle(&hwndOwner);
	DWORD dwStyle = WS_POPUP|WS_VISIBLE;
	BOOL bResizing = ::GetValueInt(::GetAppUnknown(), "Classification\\ManualWnd", "EnableResize", TRUE);
	if (bResizing) dwStyle |= WS_THICKFRAME; else dwStyle |= WS_BORDER;
	BOOL bCaption = ::GetValueInt(::GetAppUnknown(), "Classification\\ManualWnd", "EnableCaption", FALSE);
	if (bCaption) dwStyle |= WS_CAPTION|WS_SYSMENU;
	if (FAILED(CreateWnd(hwndOwner, NORECT, dwStyle, 0)))
		return -1;
	EnableWindow(hwndOwner, FALSE);
//	_CDisableWnd DisableOwner(hwndOwner);
	SetForegroundWindow(hwnd());
	MSG msg;
	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	SetForegroundWindow(hwndOwner);
	return (short)msg.wParam;
}

void CManualWnd::EnableOwner()
{
	HWND hwndOwner = GetWindow(hwnd(), GW_OWNER);
	EnableWindow(hwndOwner, TRUE);
}

void CManualWnd::EndDialog(int nResult)
{
	EnableOwner();
	m_nResultCode = nResult;
	DestroyWindow();
}


static void _CalcClassifiedAndAll(IUnknown *punkObjList, int &nClassified, int &nAll)
{
	nClassified = nAll = 0;
	sptrINamedDataObject2 sptrNDO2Obj(punkObjList);
	if (sptrNDO2Obj == 0)
		return;
	long pos = 0;
	sptrNDO2Obj->GetFirstChildPosition((long*)&pos);
	while (pos)
	{
		IUnknownPtr punk;
		sptrNDO2Obj->GetNextChild((long*)&pos, &punk);
		ICalcObjectPtr sptrCO(punk);
		long lClass = get_object_class(sptrCO);
		if (lClass >= 0)
			nClassified++;
		nAll++;
	}
}

void set_manual_class(ICalcObject *ptrObj, int lClass)
{
	IUnknownPtr punkChr = CreateChildObjects(ptrObj);
	IChromosomePtr sptrChr(punkChr);
	if (sptrChr != 0)
	{
		sptrChr->SetManualClass(lClass);
	}
}

void set_class(ICalcObject *ptrObj, long lClass)
{
	set_object_class(ptrObj, lClass);
	set_manual_class(ptrObj, lClass>=0?1:0);
}

long get_class(ICalcObject *ptrObj, int *plManualClass)
{
	if (plManualClass)
	{
		IUnknownPtr ptrFound = find_child_by_interface(ptrObj, IID_IChromosome);
		IChromosomePtr sptrChr(ptrFound);
		if (ptrFound != 0)
			sptrChr->GetManualClass(plManualClass);
		else
			*plManualClass = 0;
	}
	return get_object_class(ptrObj);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//CActionManClassify
////////////////////////////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CActionManClassifyInfo::s_pargs[] = 
{
	{0, 0, 0, false, false },
};


CActionManClassify::CActionManClassify()
{
	m_hcurDefault = LoadCursor(app::handle(), MAKEINTRESOURCE(IDC_MANUALCLASSIFY));
}

void CActionManClassify::ChangeClass()
{
	long lClass = m_lClass;
	int nManualClass = m_nManualClass;
	m_lClass = get_class(m_sptrObj,&m_nManualClass);
	set_object_class(m_sptrObj, lClass);
	set_manual_class(m_sptrObj, nManualClass);
	RedrawObjectList(m_ptrObjList);
}

_point CActionManClassify::ToApplication(_point pt)
{
	IScrollZoomSitePtr sptrScroll(m_ptrTarget);
	if (sptrScroll == 0) return pt;
	_point pt1 = _window(pt, sptrScroll);
	IWindowPtr sptrView(m_ptrTarget);
	if (sptrView == 0) return pt;
	HWND hwnd;
	sptrView->GetHandle((HANDLE *)&hwnd);
	::ClientToScreen(hwnd, &pt1);
	return pt1;
}

_rect CActionManClassify::ToApplication(_rect rc)
{
	IScrollZoomSitePtr sptrScroll(m_ptrTarget);
	if (sptrScroll == 0) return rc;
	_rect rc1 = _window(rc, sptrScroll);
	IWindowPtr sptrView(m_ptrTarget);
	if (sptrView == 0) return rc;
	HWND hwnd;
	sptrView->GetHandle((HANDLE *)&hwnd);
	_point pttl(rc1.top_left());
	_point ptbr(rc1.bottom_right());
	::ClientToScreen(hwnd, &pttl);
	::ClientToScreen(hwnd, &ptbr);
	rc1 = _rect(pttl.x, pttl.y, ptbr.x, ptbr.y);
	return rc1;
}


HRESULT CActionManClassify::DoInvoke()
{
	//get the target document
	IUnknownPtr ptrDoc;
	IViewPtr sptrV(m_ptrTarget);
	sptrV->GetDocument(&ptrDoc);
	sptrIDocument	sptrD(ptrDoc);
	//change the target - now it is document
	m_guidTarget = ::GetKey(sptrD);
	SetModified(true);
	return S_OK;
}

bool CActionManClassify::DoLButtonDownOnObject(_point point, IUnknown *punkObj, IUnknown *punkObjList)
{
	ICalcObjectPtr sptrCO(punkObj);
	m_sptrObj = punkObj;
	m_ptrObjList = punkObjList;
	m_lClass = get_class(sptrCO, &m_nManualClass);
	int nClassified,nAll;
	_CalcClassifiedAndAll(punkObjList,nClassified,nAll);
	// Calculate start point
	IRectObjectPtr sptrROObj(punkObj);
	_rect rcObj;
	sptrROObj->GetRect(&rcObj);
	_rect rcObj1 = ToApplication(rcObj);
	// Show image
	CManualWnd ManualWnd(rcObj1,m_ptrTarget,nClassified,nAll);
	long lNewClass = ManualWnd.DoModal()-1;
	if (lNewClass >= -1)
		set_class(sptrCO, lNewClass);
	return true;
}

IUnknown *CActionManClassify::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else return CInteractiveAction::DoGetInterface( iid );
}


HRESULT CActionManClassify::DoUndo()
{
	ChangeClass();
	SetModified(false);
	return S_OK;
}

HRESULT CActionManClassify::DoRedo()
{
	ChangeClass();
	SetModified(true);
	return S_OK;
}

