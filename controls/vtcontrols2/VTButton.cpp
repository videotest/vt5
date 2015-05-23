// VTButton.cpp : implementation file
//

#include "stdafx.h"
#include "vtcontrols2.h"
#include "VTButton.h"
#include "VTWidthCtl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVTButton

CVTButton::CVTButton()
{
	m_Checked=FALSE;
	m_IsMouseOver=FALSE;
	m_IsBitmap=FALSE;
	m_IsAutoRepeat=FALSE;
	m_IsMovable=FALSE;
//	m_IsDragged=FALSE;
	m_IsUpButton=FALSE;
	m_IsArrow=FALSE;

	CClientDC	dc( 0 );

	m_Font.CreateFont(
    MulDiv(10, GetDeviceCaps(dc, LOGPIXELSY), 72),                        // nHeight
    0,                         // nWidth
    0,                         // nEscapement
    0,                         // nOrientation
    FW_NORMAL,                 // nWeight
    FALSE,                     // bItalic
    FALSE,                     // bUnderline
    0,                         // cStrikeOut
    ANSI_CHARSET,              // nCharSet
    OUT_DEFAULT_PRECIS,        // nOutPrecision
    CLIP_DEFAULT_PRECIS,       // nClipPrecision
    DEFAULT_QUALITY,           // nQuality
    DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
    _T("Arial"));
}

CVTButton::~CVTButton()
{
}


BEGIN_MESSAGE_MAP(CVTButton, CButton)
	//{{AFX_MSG_MAP(CVTButton)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVTButton message handlers

void CVTButton::OnPaint() 
{
	CPaintDC pdc(this); // device context for painting
	CRect rc;
	GetClientRect(rc);

    CDC memDC;
	memDC.CreateCompatibleDC(&pdc);

	CBitmap bmp;
	CBitmap *oldBitmap;
	CBrush *oldBrush;
	CPen *oldPen;
	CFont *oldFont;

	bmp.CreateCompatibleBitmap(&pdc,rc.Width(),rc.Height());
	oldBitmap=(CBitmap*)memDC.SelectObject(&bmp);

	CVTWidthCtrl *parent=(CVTWidthCtrl*)GetParent();	
	CBrush brush;
	brush.CreateSolidBrush(parent->m_Brush);
	memDC.FillRect(&rc,&brush);

	if(m_IsArrow)
	{
		CRect rect=rc;
		rect.left+=1;
		rect.top+=1;
		rect.right-=2;
		rect.bottom-=2;
		memDC.SetPolyFillMode(ALTERNATE);
		CBrush brush;
		CPen pen;
		if(!m_IsMouseOver)
		{
			brush.CreateSolidBrush(parent->GetColorSliderArrows());
			pen.CreatePen(PS_SOLID,1,parent->GetColorSliderArrows());
		}
		else
		{
			brush.CreateSolidBrush(parent->GetColorSliderArrowsHighlight());
			pen.CreatePen(PS_SOLID,1,parent->GetColorSliderArrowsHighlight());
		}

        oldBrush = (CBrush*)memDC.SelectObject(&brush);
		oldPen=(CPen*)memDC.SelectObject(&pen);

		rect.DeflateRect(1,1);


		if(parent->m_Orientation)
		{
			if(m_IsUpButton)
			{
				CPoint points[3]={CPoint(rect.left,rect.top+(rect.bottom-rect.top)/2),CPoint(rect.right,rect.top),CPoint(rect.right,rect.bottom)};
				memDC.Polygon(points,3);
			}
			else
			{
				CPoint points[3]={CPoint(rect.right,rect.top+(rect.bottom-rect.top)/2),CPoint(rect.left,rect.bottom),CPoint(rect.left,rect.top)};
				memDC.Polygon(points,3);
			}
		}
		else
		{
			if(m_IsUpButton)
			{
				CPoint points[3]={CPoint(rect.left+(rect.right-rect.left)/2,rect.top),CPoint(rect.right,rect.bottom),CPoint(rect.left,rect.bottom)};
    			memDC.Polygon(points,3);
			}
			else
			{
				CPoint points[3]={CPoint(rect.left+(rect.right-rect.left)/2,rect.bottom),CPoint(rect.left,rect.top),CPoint(rect.right,rect.top)};
    			memDC.Polygon(points,3);
			}

		}

		rect.InflateRect(1,1);
		memDC.SelectObject(oldBrush);
		memDC.SelectObject(oldPen);
	}
		
	else
	{
		if(!m_IsBitmap)
		{
			CString str;
			str.FormatMessage(_T("%1!d!"),parent->m_Width);
			memDC.SetBkColor(parent->m_Brush);
			memDC.SetTextColor(parent->GetColorTextScale());
			oldFont=(CFont*)memDC.SelectObject(&m_Font);
			memDC.DrawText(str,&rc,DT_CENTER | DT_SINGLELINE | DT_VCENTER );
			memDC.SelectObject(oldFont);
		}
		else
		{
			CBrush brush(parent->GetColorSliderArrows());
			CPen pen;
			pen.CreatePen(PS_SOLID,1,parent->GetColorSliderArrows());
			//oldBrush = (CBrush*)pdc.SelectObject(&brush);
			//oldPen= ( CPen*)pdc.SelectObject(&pen);

			CRect rect;
			GetClientRect(rect);

			memDC.FillRect(rect,&brush);

			//memDC.SelectObject(oldBrush);
			//memDC.SelectObject(oldPen);

		}
	}

	if(!m_IsMovable)
		if(!m_Checked)
		{
			if(m_IsMouseOver) 
				memDC.DrawEdge(&rc,BDR_RAISEDINNER,BF_RECT);
				//else pdc.DrawEdge(&rc,EDGE_ETCHED,BF_RECT);
		}
		else
		{
			memDC.DrawEdge(&rc,BDR_SUNKENINNER,BF_RECT);
		}

		pdc.BitBlt(rc.left,rc.top,rc.Width(),rc.Height(),&memDC,0,0,SRCCOPY);

		memDC.SelectObject(oldBitmap);
			
}

void CVTButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(!m_IsMouseOver)
	{
	 m_IsMouseOver=TRUE;
	 InvalidateRect(NULL);
	 SetTimer(1,55,NULL);
	}

	if(m_IsMovable && nFlags==MK_LBUTTON) 
	{
		CRect rect;
		CRect parent;
		CVTWidthCtrl* pParent = (CVTWidthCtrl*)GetParent();
		pParent->GetWindowRect(parent);
		GetWindowRect(rect);
		POINT offset;
		offset.x=0;
		offset.y=0;
		rect.left-=parent.left;         // OLE Container bug?  to fix +3
		rect.top-=parent.top;
		rect.right-=parent.left;
		rect.bottom-=parent.top;
				
		if(pParent->m_Orientation) offset.x=point.x-m_oldpoint.x;
		else offset.y=point.y-m_oldpoint.y;
		rect.OffsetRect(offset);

		if(pParent->m_Orientation)
		{	
			if(rect.left>=m_SliderFrom && rect.right<=m_SliderTo) 
			{
				MoveWindow(rect);
				m_SliderCurrent=rect.left;
				::SendMessage(pParent->GetSafeHwnd(),WM_COMMAND,MAKEWPARAM(GetDlgCtrlID(),BN_CLICKED),(DWORD)GetSafeHwnd());
			}
		}
		else
		  if(rect.top>=m_SliderFrom && rect.bottom<=m_SliderTo)
		  {
			  MoveWindow(rect);
			  m_SliderCurrent=rect.top;
			  ::SendMessage(pParent->GetSafeHwnd(),WM_COMMAND,MAKEWPARAM(GetDlgCtrlID(),BN_CLICKED),(DWORD)GetSafeHwnd());
		  }

	}
	else
	{
	//		SetTimer(1,55,NULL);
	}
//	m_oldpoint=point;

}

void CVTButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CVTWidthCtrl* pParent = (CVTWidthCtrl*) GetParent();
	pParent->Check(true);
	::SendMessage(pParent->GetSafeHwnd(),WM_SETFOCUS,NULL,NULL);

	if(m_IsMovable)
	{
		SetCapture();
//		m_IsDragged=TRUE;
		m_oldpoint=point;
	}	

	if(!m_Checked && !m_IsMovable)
	 {
		m_Checked=TRUE;
		InvalidateRect(NULL);
	 }

	if(!m_IsAutoRepeat)
	{
		HWND parent=GetParent()->GetSafeHwnd();
		HWND hWnd=GetSafeHwnd();
		int id=GetDlgCtrlID();

		::SendMessage(parent,WM_COMMAND,MAKEWPARAM(id,BN_CLICKED),(DWORD)hWnd);

		m_Checked=false;
		InvalidateRect(NULL);



//	  CButton::OnLButtonDown(nFlags, point);
	}
	else
	{
		//CButton::OnLButtonDown(nFlags, point);
		SetCapture();
		int nbiter=0;
		HWND parent=GetParent()->GetSafeHwnd();
		HWND hWnd=GetSafeHwnd();
		int id=GetDlgCtrlID();
		MSG mess;

		while( !PeekMessage( &mess, hWnd, WM_LBUTTONUP, WM_LBUTTONUP, PM_REMOVE ) )
		{
			 if( nbiter < 10 )
			 {
				 Sleep( 150 );
			 } 
		     else
				 if( nbiter < 20 )
				 {
					 Sleep( 150 );
				 } 
		         else
					 if( nbiter < 30 )
					 { 
						 Sleep( 50 );
					 }
	                 else 
					 { 
						 Sleep( 25 );
					 }
		
    		nbiter++;

				
	    	::SendMessage(parent,WM_COMMAND,MAKEWPARAM(id,BN_CLICKED),(DWORD)hWnd);
		}

    ReleaseCapture();
	SendMessage( WM_LBUTTONUP );
	}

}

void CVTButton::OnTimer(UINT_PTR nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	POINT pt;
	GetCursorPos(&pt);
	CRect rect;
    GetWindowRect (rect);

	if(!rect.PtInRect(pt))
	{
		m_IsMouseOver=FALSE;
		KillTimer(1);
		InvalidateRect(NULL);
	}

//	CButton::OnTimer(nIDEvent);
}

void CVTButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_Checked=FALSE;
	if(m_IsMovable)
	{
		ReleaseCapture();
//		m_IsDragged=FALSE;
	}
	InvalidateRect(NULL);
	
//	CButton::OnLButtonUp(nFlags, point);
}

BOOL CVTButton::SetBmp(UINT nIDResource)
{
  m_IsBitmap=TRUE;
  return TRUE;
}

BOOL CVTButton::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class

	return CButton::PreCreateWindow(cs);
}

void CVTButton::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnLButtonDown(nFlags, point);
}
