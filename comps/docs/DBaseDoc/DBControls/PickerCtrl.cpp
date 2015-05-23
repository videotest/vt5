// PickerCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "DBControls.h"
#include "PickerCtrl.h"
#include "dbcontrol.h"

//#include "msado15.tlh"
//#include "msado15.tli"
#import "msado15.dll" rename_namespace("ADO") rename("EOF", "ADOEOF")

#include "EditCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CPickerCtrl

CPickerCtrl::CPickerCtrl()
{
	m_pickerState = psUnpressed;
	m_pPopupWnd = NULL;
	m_bDisabled = FALSE;
	m_bClickedFromPopup = false;
}

CPickerCtrl::~CPickerCtrl()
{
}


BEGIN_MESSAGE_MAP(CPickerCtrl, CWnd)
	//{{AFX_MSG_MAP(CPickerCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_WM_SETFOCUS()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPickerCtrl message handlers

void CPickerCtrl::OnPaint() 
{
	CRect rc;
	GetClientRect( &rc );
	CPaintDC dc(this);

	UINT uType = 0;
	UINT uState = 0;



	if( GetState() ==  psUnpressed )
	{
		uType	= DFC_SCROLL;
		uState	= DFCS_SCROLLDOWN | DFCS_SCROLLCOMBOBOX;		
	}
	else
	{
		uType	= DFC_SCROLL;
		uState	= DFCS_SCROLLDOWN | DFCS_PUSHED | DFCS_SCROLLCOMBOBOX;		
	}

	// [vanek] BT2000: 3510 - 26.01.2004
	if( m_bDisabled )
		uState |= DFCS_INACTIVE;

	dc.DrawFrameControl( &rc, uType, uState );

}

PickerState CPickerCtrl::GetState()
{
	return m_pickerState;	
}

void CPickerCtrl::SetState( PickerState state )
{
	if( m_pickerState == state )
		return;

	m_pickerState = state;

	if( m_pickerState == psUnpressed )
	{		
		//m_ctrlListBox.ShowWindow( SW_HIDE );
	}
	else if (!m_bClickedFromPopup)
	{

		CRect rcInit = m_rcPopupInit;

		//Need manual set
		rcInit.bottom = rcInit.top + 200;



		CRect rcNewRect;
		GetClientRect( &rcNewRect );
		ClientToScreen( &rcNewRect );		

		rcInit.left -= ( m_PickerOldRect.left - rcNewRect.left );
		rcInit.right -= ( m_PickerOldRect.right - rcNewRect.right );

		rcInit.top -= ( m_PickerOldRect.top - rcNewRect.top );
		rcInit.bottom -= ( m_PickerOldRect.bottom - rcNewRect.bottom );

		m_pPopupWnd = new CPickerPopupWnd();
		LPCTSTR lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW);

		 //WS_EX_WINDOWEDGE || WS_EX_TRANSPARENT
		BOOL b = m_pPopupWnd->CreateEx( WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_PALETTEWINDOW,
								lpszClass, 
								_T(""),
								WS_POPUP | WS_VISIBLE,
								rcInit.left,rcInit.top, 
								rcInit.Width(),rcInit.Height(),
								GetSafeHwnd(),
								NULL,
								NULL);
		m_pPopupWnd->m_pPickerCtrl = this;
	}
	m_bClickedFromPopup = false;

	Invalidate();

}


void CPickerCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//CDBControlImpl* pDBControl = (CDBControlImpl*)GetParent();	
	//if( pDBControl->GetBlankViewMode() != bvmPreview )
	//	return;

	if( m_bDisabled )
		return;

	SetState( psPressed );
	SetCapture();			

	CWnd::OnLButtonDown(nFlags, point);
}

void CPickerCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( m_bDisabled )
		return;
	
	ReleaseCapture();
	SetState( psUnpressed );	
	Invalidate();		
	
	
	CWnd::OnLButtonUp(nFlags, point);
}

void CPickerCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect rcClient;
	GetClientRect( &rcClient );
	if( m_pickerState == psPressed && !rcClient.PtInRect(point) )
	{
		ReleaseCapture();
		m_pickerState = psUnpressed;	
		Invalidate();		
	}
	
	CWnd::OnMouseMove(nFlags, point);
}

int CPickerCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
// CPickerPopupWnd

CPickerPopupWnd::CPickerPopupWnd()
{
	m_pPickerCtrl = NULL;
}

CPickerPopupWnd::~CPickerPopupWnd()
{
}


BEGIN_MESSAGE_MAP(CPickerPopupWnd, CWnd)
	//{{AFX_MSG_MAP(CPickerPopupWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPickerPopupWnd message handlers
/////////////////////////////////////////////////////////////////////////////
// CPickerListBox

CPickerListBox::CPickerListBox()
{
	m_hWndReciever = NULL;
}

CPickerListBox::~CPickerListBox()
{
}


BEGIN_MESSAGE_MAP(CPickerListBox, CListBox)
	//{{AFX_MSG_MAP(CPickerListBox)
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPickerListBox message handlers

BOOL CPickerPopupWnd::DestroyWindow() 
{			
	BOOL bResult = CWnd::DestroyWindow();
	delete this;
	return bResult;
}

int CPickerPopupWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	

	LOGFONT lf;
	::ZeroMemory( &lf, sizeof(lf) );
	CFont* pFont = CFont::FromHandle( (HFONT)GetStockObject(ANSI_VAR_FONT) );	
	if( pFont )
	{
		pFont->GetLogFont( &lf );
		m_Font.CreatePointFontIndirect( &lf );
	}



	CRect rcClient;
	GetClientRect( &rcClient );
	m_ctrlListBox.Create(	WS_BORDER | WS_HSCROLL | WS_VSCROLL |
							LBS_NOTIFY | WS_CHILD | WS_VISIBLE, 
							rcClient, this, ID_LIST_BOX_PICKER );

	m_ctrlListBox.SetFont( &m_Font );

	CWnd* pWndReciever;
	pWndReciever = (CWnd::FromHandle(lpCreateStruct->hwndParent))->GetParent();
	
	m_ctrlListBox.SetReciver( pWndReciever->GetSafeHwnd() );

	sptrIDBControl spDBCtrl( pWndReciever->GetControllingUnknown() );
	if( spDBCtrl )
	{
		IUnknown* pUnkDoc = NULL;
		spDBCtrl->GetDBaseDocument( &pUnkDoc );

		BSTR bstrTable, bstrField;
		spDBCtrl->GetTableName( &bstrTable );
		spDBCtrl->GetFieldName( &bstrField );

		CString strTable = bstrTable;
		CString strField = bstrField;

		if( bstrTable )
			::SysFreeString( bstrTable );

		if( bstrField )
			::SysFreeString( bstrField );
		
		CString strSQL;
		strSQL.Format( "SELECT Distinct( CStr(%s)) FROM %s where %s <> null",
						(LPCTSTR)strField, (LPCTSTR)strTable, (LPCTSTR)strField );


		if( pUnkDoc )
		{
			sptrIDBConnection spDBDoc( pUnkDoc );
			if( spDBDoc )
			{
				IUnknown* pUnkConnection = NULL;
				spDBDoc->GetConnection( &pUnkConnection );
				if( pUnkConnection )
				{
					ADO::_ConnectionPtr ptrConn( pUnkConnection );
					if( ptrConn )
					{
						if( ptrConn->State == ADO::adStateOpen )
						{
							ADO::_RecordsetPtr spRecordset;
							try{
								_variant_t vConn = (IDispatch*)ptrConn;
								spRecordset.CreateInstance( __uuidof(ADO::Recordset) );
								spRecordset->CursorLocation = ADO::adUseClient;
								spRecordset->Open( _bstr_t(strSQL), vConn, 
									ADO::adOpenStatic, ADO::adLockReadOnly, ADO::adCmdText);

								if( VARIANT_FALSE == spRecordset->ADOEOF )
									spRecordset->MoveFirst();
									
								while( VARIANT_FALSE == spRecordset->ADOEOF )
								{

									ADO::FieldsPtr spFields = spRecordset->Fields;
									if( spFields != NULL )
									{
										ADO::FieldPtr spField = spFields->GetItem( (long)0 );
										if( spField )
										{
											_variant_t var;
											var = spField->GetValue();		
											if( !(var.vt == VT_EMPTY || var.vt == VT_NULL) )
											{
												var.ChangeType( VT_BSTR );
												CString strValue = var.bstrVal;
												m_ctrlListBox.AddString( strValue );
											}
										}
									}
									spRecordset->MoveNext();
								}			
								
							}
							catch(...)
							{

							}
						}
					}
					pUnkConnection->Release();
				}
			}
			pUnkDoc->Release();
		}
	}


	/*
	m_ctrlListBox.AddString("1");
	m_ctrlListBox.AddString("2");
	m_ctrlListBox.AddString("3");
	m_ctrlListBox.AddString("4");
	m_ctrlListBox.AddString("1");
	m_ctrlListBox.AddString("2");
	m_ctrlListBox.AddString("3");
	m_ctrlListBox.AddString("4");
	m_ctrlListBox.AddString("1");
	m_ctrlListBox.AddString("2");
	m_ctrlListBox.AddString("3");
	m_ctrlListBox.AddString("4");
	m_ctrlListBox.AddString("1");
	m_ctrlListBox.AddString("2");
	m_ctrlListBox.AddString("3");
	m_ctrlListBox.AddString("4sddddddddddddddddddddddddddddddddddddddddddd");
	m_ctrlListBox.AddString("1");
	m_ctrlListBox.AddString("2");
	m_ctrlListBox.AddString("3");
	m_ctrlListBox.AddString("4");
	m_ctrlListBox.AddString("1");
	m_ctrlListBox.AddString("2");
	m_ctrlListBox.AddString("3");
	m_ctrlListBox.AddString("4");
	*/

	m_ctrlListBox.SetFocus();

	return 0;
}

void CPickerPopupWnd::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);
	//m_ctrlListBox.SetFocus();
}

void CPickerListBox::OnKillFocus(CWnd* pNewWnd) 
{
 	CListBox::OnKillFocus(pNewWnd);	
	CPoint pt;
	GetCursorPos(&pt);
	CWnd *pCursWnd = WindowFromPoint(pt);
	CPickerPopupWnd *pParent = (CPickerPopupWnd *)GetParent();
	if (pParent != NULL && pParent->m_pPickerCtrl != NULL && pParent->m_pPickerCtrl == pCursWnd)
	{
		CPickerCtrl *pPickerCtrl = pParent->m_pPickerCtrl;
		pPickerCtrl->m_bClickedFromPopup = true;
	}
	GetParent()->DestroyWindow();
}

BOOL CPickerListBox::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle |= WS_EX_WINDOWEDGE;
	return CListBox::PreCreateWindow(cs);
}

void CPickerListBox::OnSelchange() 
{
	CString strText;
	int nIndex;

	nIndex = GetCurSel();

	if( nIndex != -1 )
	{
		GetText( nIndex, strText );
		TCHAR szBuf[256];
		lstrcpy( szBuf, strText );

		ASSERT(m_hWndReciever);

		CWnd* pWnd = CWnd::FromHandle(m_hWndReciever);

		//::SendMessage( m_hWndReciever, WM_SETTEXT, 0, (LPARAM)szBuf );
		pWnd->SendMessage( WM_SET_CONTROL_TEXT, 0, (LPARAM)szBuf );		
		
		pWnd->SetFocus( );
		
	}
	
	
}

void CPickerCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here
}

void CPickerCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	if( GetParent() )
		VERIFY( GetParent()->SendMessage( WM_SET_FOCUS_TO_CONTROL, 0, 0 ) );

	CWnd::OnRButtonDown(nFlags, point);
}
