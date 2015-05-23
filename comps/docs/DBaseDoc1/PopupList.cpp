// PopupList.cpp : implementation file
//

#include "stdafx.h"
#include "PopupList.h"

/////////////////////////////////////////////////////////////////////////////
// CPopupList

CPopupList::CPopupList() : m_pReciever(NULL)
{
}

CPopupList::~CPopupList()
{
}

CRect CPopupList::GetInitRect( CPoint ptStartPoint, int nRecomendedWidth, int nRecomendedHeight,
							  bool bCorrectCtrlPos )
{
	CRect rc;

	int nCount = GetCount();
	if( nCount < 1 )
	{
		rc = CRect(	ptStartPoint.x,
						ptStartPoint.y,
						ptStartPoint.x + nRecomendedWidth, 
						ptStartPoint.y + 150 
						);

		int nHeight = rc.Height();

		rc.top -= nHeight;
		rc.bottom -= nHeight;

		MoveWindow( &rc );
	}

	int nItemHeight = GetItemHeight( 0 );
	

	rc.left = ptStartPoint.x;
	rc.top  = ptStartPoint.y;

	int nWidth, nHeight;

	int nAdd = 4 * ::GetSystemMetrics(SM_CYEDGE);	

	nWidth = nRecomendedWidth;
	nHeight = ( (nCount * nItemHeight + nAdd ) > nRecomendedHeight ? 
											nRecomendedHeight
											: (nCount * nItemHeight + nAdd ) );
	
	rc.right  = rc.left + nWidth;
	rc.bottom = rc.top  + nHeight;

	rc.top -= nHeight;
	rc.bottom -= nHeight;

	if( bCorrectCtrlPos )
		MoveWindow( &rc );

	return rc;
}

BEGIN_MESSAGE_MAP(CPopupList, CListBox)
	//{{AFX_MSG_MAP(CPopupList)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPopupList message handlers
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool CPopupList::XPopupCtrlEvent::OnInit ()
{
    METHOD_PROLOGUE_(CPopupList, PopupCtrlEvent);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
void CPopupList::XPopupCtrlEvent::OnShow ()
{
    METHOD_PROLOGUE_(CPopupList, PopupCtrlEvent);
    pThis->SetFocus();
}

///////////////////////////////////////////////////////////////////////////////
void CPopupList::XPopupCtrlEvent::OnHide (bool bAbort)
{
    METHOD_PROLOGUE_(CPopupList, PopupCtrlEvent);
}

///////////////////////////////////////////////////////////////////////////////
int CPopupList::XPopupCtrlEvent::OnKeyDown (UINT nChar, UINT, UINT)
{
    if ( nChar == VK_RETURN )
    {
        return CPopup::end;
    }
    return CPopup::doNothing;
}

///////////////////////////////////////////////////////////////////////////////
int CPopupList::XPopupCtrlEvent::OnLButtonDown (UINT, CPoint pt)
{
    METHOD_PROLOGUE_(CPopupList, PopupCtrlEvent);
    return CPopup::doNothing;
}

///////////////////////////////////////////////////////////////////////////////
int CPopupList::XPopupCtrlEvent::OnLButtonUp (UINT, CPoint pt)
{
    METHOD_PROLOGUE_(CPopupList, PopupCtrlEvent);
    return CPopup::doNothing;
}

///////////////////////////////////////////////////////////////////////////////
IPopupCtrlEvent* CPopupList::XPopupCtrlEvent::GetInterfaceOf (HWND)
{
    // Not used here because this control doesn't include other controls
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
void CPopupList::XPopupCtrlEvent::SetReciever (IPopupCtrlReciever* pReciever)
{
	METHOD_PROLOGUE_(CPopupList, PopupCtrlEvent);
    pThis->m_pReciever = pReciever;    
}

///////////////////////////////////////////////////////////////////////////////
void CPopupList::OnDestroy() 
{
	CListBox::OnDestroy();		
	delete this;
}

int CPopupList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	LOGFONT lf;
	::ZeroMemory( &lf, sizeof(lf) );
	CFont* pFont = CFont::FromHandle( (HFONT)GetStockObject(ANSI_VAR_FONT) );	
	if( pFont )
	{
		pFont->GetLogFont( &lf );
		m_Font.CreatePointFontIndirect( &lf );
		SetFont( &m_Font );
	}

	
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CPopupList::PreCreateWindow(CREATESTRUCT& cs) 
{	
	//cs.dwExStyle |= WS_EX_CLIENTEDGE;		
	return CListBox::PreCreateWindow(cs);
}

void CPopupList::OnSelchange() 
{

	if( m_pReciever )
	{
		int nIndex = GetCurSel();
		CString strText;
		this->GetText( nIndex, strText );

		if( nIndex != -1 )		
		{
			m_pReciever->OnNotify( szChooserEventStringValueChange, strText );	
			m_pReciever->OnNotify( szChooserEventUINTValueChange, (UINT)nIndex );	
		}
	}

	CPopup* pParent = (CPopup*)GetParent();
	pParent->ChildWannaDead();

}
