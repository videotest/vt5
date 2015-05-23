#include "StdAfx.h"
#include "Reposition.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CRepositionNode::RecalcRect()
{
	bool bFirst = true;
	CRect rcNode;
	for (int i = 0; i < GetSize(); i++)
	{
		if (!ElementAt(i).m_bTemp)
		{
			CRect rcWnd = ElementAt(i).m_rcWnd;
			if (bFirst)
				rcNode = rcWnd;
			else
				rcNode = CRect(min(rcNode.left,rcWnd.left),min(rcNode.top,rcWnd.top),
					max(rcNode.right,rcWnd.right),max(rcNode.bottom,rcWnd.bottom));
			bFirst = false;
		}
	}
	if (bFirst)
		rcNode = CRect(0,0,5,5);
	else
	{
		rcNode.right += rcNode.left;
		rcNode.bottom += rcNode.top;
		rcNode.top = rcNode.left = 0;
	}
	for (int i = 0; i < GetSize(); i++)
	{
		if (ElementAt(i).m_bTemp)
		{
			CRect rcWnd = ElementAt(i).m_rcWnd;
			int n = 2;
			int nWidth = max(rcNode.Width(),rcWnd.Width()+n*2);
			rcNode.right = rcNode.left+nWidth;
			ElementAt(i).m_rcWnd = CRect(n,rcNode.bottom+n,rcWnd.Width()+n,rcNode.bottom+rcWnd.Height()+n);
			rcNode.bottom += rcWnd.Height()+n*2;
		}
	}
	m_rcNode = CRect(m_rcNode.left,m_rcNode.top,m_rcNode.left+rcNode.right,m_rcNode.top+rcNode.bottom);
}

void CRepositionNode::Add(HWND hwnd, CRect rcWnd)
{
	ASSERT(rcWnd.left>=m_rcNode.left&&rcWnd.right<=m_rcNode.right);
	ASSERT(rcWnd.top>=m_rcNode.top&&rcWnd.bottom<=m_rcNode.bottom);
	CRect rc(rcWnd.left-m_rcNode.left,rcWnd.top-m_rcNode.top,rcWnd.right-m_rcNode.left,
		rcWnd.bottom-m_rcNode.top);
	CArray<CCtrlInfo,CCtrlInfo&>::Add(CCtrlInfo(hwnd,rc));
}

void CRepositionNode::Reposition(CPoint ptPos)
{
	for (int i = 0; i < GetSize(); i++)
	{
		SetWindowPos(ElementAt(i).m_hwnd, NULL, ptPos.x+ElementAt(i).m_rcWnd.left, ptPos.y+ElementAt(i).m_rcWnd.top,
			0, 0, SWP_NOSIZE|SWP_NOZORDER);
	}
	m_rcNode = CRect(ptPos.x, ptPos.y, ptPos.x+m_rcNode.Width(), ptPos.y+m_rcNode.Height());
}

void CRepositionNode::InsertTempControl(CCtrlInfo &CtrlInfo, int nPos)
{
	for (int i = 0; i < GetSize(); i++)
	{
		if (ElementAt(i).m_bTemp && nPos == 0)
		{
			CArray<CCtrlInfo,CCtrlInfo&>::InsertAt(i-1, CtrlInfo);
			return;
		}
	}
	CArray<CCtrlInfo,CCtrlInfo&>::Add(CtrlInfo);
}

void CRepositionNode::AddTempControl(HWND hwnd, CRect rc, int nPos)
{
	CCtrlInfo CtrlInfo(hwnd,rc,true);
//	CArray<CCtrlInfo,CCtrlInfo&>::Add(CtrlInfo);
	InsertTempControl(CtrlInfo, nPos);
	RecalcRect();
	Reposition(m_rcNode.TopLeft());
}

void CRepositionNode::RemoveTempControl(HWND hwnd)
{
	for (int i = 0; i < GetSize(); i++)
	{
		if (ElementAt(i).m_bTemp && ElementAt(i).m_hwnd == hwnd)
			RemoveAt(i);
	}
	RecalcRect();
	Reposition(m_rcNode.TopLeft());
}

CRepositionManager::CRepositionManager()
{
	m_hwndParent = NULL;
	m_idBase = -1;
	m_szMaxClient = CSize(0,0);
	m_rcBase = CRect(0,0,0,0);
	m_rcMinMargins = CRect(0,0,0,0);
};

void CRepositionManager::Init(HWND hwndParent, int idBase, CSize szMaxClient)
{
	m_hwndParent = hwndParent;
	m_idBase = idBase;
	m_szMaxClient = szMaxClient;
	HWND hwndBase = ::GetDlgItem(hwndParent, idBase);
	CRect rcBase,rcParent;
	GetClientRect(hwndParent, rcParent);
	CWnd::FromHandle(hwndParent)->ClientToScreen(rcParent);
	GetWindowRect(hwndBase, &rcBase);
	ASSERT(rcBase.left>=rcParent.left&&rcBase.right<=rcParent.right);
	ASSERT(rcBase.top>=rcParent.top&&rcBase.bottom<=rcParent.bottom);
	m_LeftTop.InitRect(CRect(rcParent.left,rcParent.top,rcBase.left,rcBase.top));
	m_Left.InitRect(CRect(rcParent.left,rcBase.top,rcBase.left,rcBase.bottom));
	m_LeftBottom.InitRect(CRect(rcParent.left,rcBase.bottom,rcBase.left,rcParent.bottom));
	m_RightTop.InitRect(CRect(rcBase.right,rcParent.top,rcParent.right,rcBase.top));
	m_Right.InitRect(CRect(rcBase.right,rcBase.top,rcParent.right,rcBase.bottom));
	m_RightBottom.InitRect(CRect(rcBase.right,rcBase.bottom,rcParent.right,rcParent.bottom));
	m_Top.InitRect(CRect(rcBase.left,rcParent.top,rcBase.right,rcBase.top));
	m_Bottom.InitRect(CRect(rcBase.left,rcBase.bottom,rcBase.right,rcParent.bottom));
	for (HWND hwnd = ::GetWindow(m_hwndParent, GW_CHILD); hwnd; hwnd = ::GetWindow(hwnd, GW_HWNDNEXT))
	{
		if (hwnd == hwndBase) continue;
		CRect rc;
		GetWindowRect(hwnd, rc);
		if (rc.right < rcBase.left) // On the left of the base
		{
			if (rc.bottom < rcBase.top)
				m_LeftTop.Add(hwnd, rc);
			else if (rc.top > rcBase.bottom)
				m_LeftBottom.Add(hwnd, rc);
			else if (rc.top >= rcBase.top && rc.bottom <= rcBase.bottom)
				m_Left.Add(hwnd, rc);
			else
				ShowWindow(hwnd, SW_HIDE);
		}
		else if (rc.left > rcBase.right)
		{
			if (rc.bottom < rcBase.top)
				m_RightTop.Add(hwnd, rc);
			else if (rc.top > rcBase.bottom)
				m_RightBottom.Add(hwnd, rc);
			else if (rc.top >= rcBase.top && rc.bottom <= rcBase.bottom)
				m_Right.Add(hwnd, rc);
			else
				ShowWindow(hwnd, SW_HIDE);
		}
		else if (rc.left >= rcBase.left && rc.right <= rcBase.right)
		{
			if (rc.bottom < rcBase.top)
				m_Top.Add(hwnd, rc);
			else if (rc.top > rcBase.bottom)
				m_Bottom.Add(hwnd, rc);
			else
				ShowWindow(hwnd, SW_HIDE);
		}
		else
			ShowWindow(hwnd, SW_HIDE);
	}
	m_LeftTop.RecalcRect();
	m_Left.RecalcRect();
	m_LeftBottom.RecalcRect();
	m_RightTop.RecalcRect();
	m_Right.RecalcRect();
	m_RightBottom.RecalcRect();
	m_Top.RecalcRect();
	m_Bottom.RecalcRect();
}

void CRepositionManager::Reposition(CSize szBase, CRect &rcBase, CSize &szTotal)
{
	const int nMargin = 0;
	int nLeft = max(max(m_LeftTop.GetWndSize().cx,m_LeftBottom.GetWndSize().cx),m_Left.GetWndSize().cx);
	int nRight = max(max(m_RightTop.GetWndSize().cx,m_RightBottom.GetWndSize().cx),m_Right.GetWndSize().cx);
	int nCenMinX = max(m_Top.GetWndSize().cx,m_Bottom.GetWndSize().cx);
	int nTop = max(max(m_LeftTop.GetWndSize().cy,m_RightTop.GetWndSize().cy),m_Top.GetWndSize().cy);
	int nBottom = max(max(m_LeftBottom.GetWndSize().cy,m_RightBottom.GetWndSize().cy),m_Bottom.GetWndSize().cy);
	int nCenMinY = max(m_Left.GetWndSize().cy,m_Right.GetWndSize().cy);
	CSize szMin(nLeft+nRight+nCenMinX,nTop+nBottom+nCenMinY);
	int nCenterX = max(nCenMinX,szBase.cx);
	int nCenterY = max(nCenMinY,szBase.cy);
	if (szMin.cx <= m_szMaxClient.cx && szMin.cy <= m_szMaxClient.cy)
	{
		if (nLeft+nCenterX+nRight>m_szMaxClient.cx)
			nCenterX = m_szMaxClient.cx-nLeft-nRight;
		if (nTop+nCenterY+nBottom>m_szMaxClient.cy)
			nCenterY = m_szMaxClient.cy-nTop-nBottom;
		ASSERT(nCenterX>=nCenMinX&&nCenterY>=nCenMinY);
	}
	m_LeftTop.Reposition(CPoint(0,0));
	m_Left.Reposition(CPoint(0,nTop));
	m_LeftBottom.Reposition(CPoint(0,nTop+nCenterY));
	m_Top.Reposition(CPoint(nLeft,0));
	m_Bottom.Reposition(CPoint(nLeft,nTop+nCenterY));
	m_RightTop.Reposition(CPoint(nLeft+nCenterX,0));
	m_Right.Reposition(CPoint(nLeft+nCenterX,nTop));
	m_RightBottom.Reposition(CPoint(nLeft+nCenterX,nTop+nCenterY));
	rcBase = CRect(nLeft,nTop,nLeft+nCenterX,nTop+nCenterY);
	szTotal = CSize(nLeft+nCenterX+nRight,nTop+nCenterY+nBottom);
}

CRect CRepositionManager::CalcMargins()
{
	int nLeft = max(m_LeftTop.GetWndSize().cx,max(m_Left.GetWndSize().cx,m_LeftBottom.GetWndSize().cx));
	int nTop = max(m_LeftTop.GetWndSize().cy,max(m_Top.GetWndSize().cy,m_RightTop.GetWndSize().cy));
	int nRight = max(m_RightTop.GetWndSize().cx,max(m_Right.GetWndSize().cx,m_RightBottom.GetWndSize().cx));
	int nBottom = max(m_LeftBottom.GetWndSize().cy, max(m_Bottom.GetWndSize().cy, m_RightBottom.GetWndSize().cy));
	nLeft = max(nLeft,m_rcMinMargins.left);
	nTop = max(nTop,m_rcMinMargins.top);
	nRight = max(nRight,m_rcMinMargins.right);
	nBottom = max(nBottom,m_rcMinMargins.bottom);
	return CRect(nLeft,nTop,nRight,nBottom);
}

void CRepositionManager::RepositionByTotal()
{
	CRect rcClient;
	GetClientRect(m_hwndParent, rcClient);
	CSize szTotal(rcClient.Size());
	CRect rcMargs(CalcMargins());
	m_LeftTop.Reposition(CPoint(0,0));
	m_Left.Reposition(CPoint(0,rcMargs.top));
	m_LeftBottom.Reposition(CPoint(0,szTotal.cy-rcMargs.bottom));
	m_Top.Reposition(CPoint(rcMargs.left,0));
	m_Bottom.Reposition(CPoint(rcMargs.left,szTotal.cy-rcMargs.bottom));
	m_RightTop.Reposition(CPoint(szTotal.cx-rcMargs.right,0));
	m_Right.Reposition(CPoint(szTotal.cx-rcMargs.right,rcMargs.top));
	m_RightBottom.Reposition(CPoint(szTotal.cx-rcMargs.right,szTotal.cy-rcMargs.bottom));
	HWND hwndBase = ::GetDlgItem(m_hwndParent, m_idBase);
	SetWindowPos(hwndBase, NULL, rcMargs.left, rcMargs.top, szTotal.cx-rcMargs.left-rcMargs.right, szTotal.cy-rcMargs.top-rcMargs.bottom, SWP_NOZORDER);
	m_rcBase = CRect(rcMargs.left, rcMargs.top, szTotal.cx-rcMargs.left-rcMargs.right, szTotal.cy-rcMargs.top-rcMargs.bottom);
}

CSize CRepositionManager::GetMinSize()
{
	int nMinSizeY1 = m_LeftTop.GetWndSize().cy+m_Left.GetWndSize().cy+m_LeftBottom.GetWndSize().cy;
	int nMinSizeY2 = m_Top.GetWndSize().cy+m_Bottom.GetWndSize().cy;
	int nMinSizeY3 = m_RightTop.GetWndSize().cy+m_Right.GetWndSize().cy+m_RightBottom.GetWndSize().cy;
	int nMinSizeX1 = m_LeftTop.GetWndSize().cx+m_Top.GetWndSize().cx+m_RightTop.GetWndSize().cx;
	int nMinSizeX2 = m_Left.GetWndSize().cx+m_Right.GetWndSize().cx;
	int nMinSizeX3 = m_LeftBottom.GetWndSize().cx+m_Bottom.GetWndSize().cx+m_RightBottom.GetWndSize().cx;
	CSize sz(max(nMinSizeX1,max(nMinSizeX2,nMinSizeX3)),max(nMinSizeY1,max(nMinSizeY2,nMinSizeY3)));
	return sz;
}

CSize CRepositionManager::CalcSizeByCx(CSize szPrvOrig, int cx, bool bWndSize)
{
	if (bWndSize)
	{
		CSize szMarg(GetParentWindowMargins());
		ASSERT(cx-szMarg.cx>0);
		CSize sz = CalcSizeByCx(szPrvOrig, cx-szMarg.cx);
		return CSize(sz.cx+szMarg.cx,sz.cy+szMarg.cy);
	}
	CSize szMin(GetMinSize());
	if (cx < szMin.cx) cx = szMin.cx;
	CRect rcMargs(CalcMargins());
	int cxPrev = cx-rcMargs.left-rcMargs.right;
	ASSERT(cxPrev >= 0);
	int cyPrev = cxPrev*szPrvOrig.cy/szPrvOrig.cx;
	int cy = cyPrev+rcMargs.top+rcMargs.bottom;
	if (cy < szMin.cy)
		return CalcSizeByCy(szPrvOrig,szMin.cy);
	else
		return CSize(cx,cy);
}

CSize CRepositionManager::CalcSizeByCy(CSize szPrvOrig, int cy, bool bWndSize)
{
	if (bWndSize)
	{
		CSize szMarg(GetParentWindowMargins());
		ASSERT(cy-szMarg.cy>0);
		CSize sz = CalcSizeByCy(szPrvOrig, cy-szMarg.cy);
		return CSize(sz.cx+szMarg.cx,sz.cy+szMarg.cy);
	}
	CSize szMin(GetMinSize());
	if (cy < szMin.cy) cy = szMin.cy;
	CRect rcMargs(CalcMargins());
	int cyPrev = cy-rcMargs.top-rcMargs.bottom;
	ASSERT(cyPrev >= 0);
	int cxPrev = cyPrev*szPrvOrig.cx/szPrvOrig.cy;
	int cx = cxPrev+rcMargs.left+rcMargs.right;
	if (cx < szMin.cx)
		return CalcSizeByCx(szPrvOrig,szMin.cx);
	else
		return CSize(cx,cy);
}

CSize CRepositionManager::GetParentWindowMargins()
{
	CRect rcWindow,rcClient;
	GetWindowRect(m_hwndParent, rcWindow);
	GetClientRect(m_hwndParent, rcClient);
	return CSize(rcWindow.Size().cx-rcClient.Size().cx,rcWindow.Size().cy-rcClient.Size().cy);
}

void CRepositionManager::SetMaximalSize(CSize szPrvOrig, bool bOrig)
{
	int cxFullScreen = ::GetSystemMetrics(SM_CXFULLSCREEN);
	int cyFullScreen = ::GetSystemMetrics(SM_CYFULLSCREEN);
	CSize sz = CalcSizeByCx(szPrvOrig, cxFullScreen, true);
	if (sz.cy > cyFullScreen)
		sz = CalcSizeByCy(szPrvOrig, cyFullScreen, true);
	if (bOrig)
		::SetWindowPos(m_hwndParent, 0, (cxFullScreen-sz.cx)/2, (cyFullScreen-sz.cy)/2, sz.cx, sz.cy, SWP_NOZORDER);
	else
		::SetWindowPos(m_hwndParent, 0, 0, 0, sz.cx, sz.cy, SWP_NOZORDER|SWP_NOMOVE);
	RepositionByTotal();
}

void CRepositionManager::SetSizeByPreview(CSize szPreview, bool bOrig)
{
	CRect rcBase;
	CSize sz;
	Reposition(szPreview, rcBase, sz);
	int cxPreview = min(szPreview.cx, rcBase.Width());
	int cyPreview = min(szPreview.cy, rcBase.Height());
	HWND hwndBase = ::GetDlgItem(m_hwndParent, m_idBase);
	SetWindowPos(hwndBase, NULL, rcBase.left, rcBase.top, cxPreview, cyPreview, SWP_NOZORDER);
	CRect rcTotal(0, 0, sz.cx, sz.cy);
	AdjustWindowRect(&rcTotal, ::GetWindowLong(m_hwndParent, GWL_STYLE), FALSE);
	sz = rcTotal.Size();
	if (bOrig)
	{
		int cxFullScreen = ::GetSystemMetrics(SM_CXFULLSCREEN);
		int cyFullScreen = ::GetSystemMetrics(SM_CYFULLSCREEN);
		::SetWindowPos(m_hwndParent, 0, (cxFullScreen-sz.cx)/2, (cyFullScreen-sz.cy)/2, sz.cx, sz.cy, SWP_NOZORDER);
	}
	else
		::SetWindowPos(m_hwndParent, 0, 0, 0, sz.cx, sz.cy, SWP_NOZORDER|SWP_NOMOVE);
}


void CRepositionManager::HandleSizing(CSize szPrvOrig, UINT nSide, LPRECT lpRect)
{
	CRect rc(lpRect);
	CSize szMargins(GetParentWindowMargins());
	CSize szClient(rc.Size().cx-szMargins.cx,rc.Size().cy-szMargins.cy);
	if (nSide==WMSZ_TOP||nSide==WMSZ_BOTTOM)
		szClient = CalcSizeByCy(szPrvOrig,szClient.cy);
	else
		szClient = CalcSizeByCx(szPrvOrig,szClient.cx);
	szClient = CSize(szClient.cx+szMargins.cx,szClient.cy+szMargins.cy);
	if (nSide==WMSZ_TOPLEFT||nSide==WMSZ_LEFT||nSide==WMSZ_BOTTOMLEFT)
		lpRect->left = lpRect->right-szClient.cx;
	else
		lpRect->right = lpRect->left+szClient.cx;
	if (nSide==WMSZ_TOPLEFT||nSide==WMSZ_TOP||nSide==WMSZ_TOPRIGHT)
		lpRect->top = lpRect->bottom-szClient.cy;
	else
		lpRect->bottom = lpRect->top+szClient.cy;
}

void CRepositionManager::AddTempControl(HWND hwnd, int nNode, int iPos)
{
	CRect rcWnd;
	::GetWindowRect(hwnd, &rcWnd);
	if (m_Left.GetSize())
		m_Left.AddTempControl(hwnd, CRect(0,0,rcWnd.Width(), rcWnd.Height()), iPos);
	else
		m_Right.AddTempControl(hwnd, CRect(0,0,rcWnd.Width(), rcWnd.Height()), iPos);
}

void CRepositionManager::RemoveTempControl(HWND hwnd)
{
	m_Left.RemoveTempControl(hwnd);
	m_Right.RemoveTempControl(hwnd);
}


