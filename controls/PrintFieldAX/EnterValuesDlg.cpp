// EnterValuesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PrintFieldAX.h"
#include "EnterValuesDlg.h"
#include "\vt5\awin\misc_utils.h"
#include "EditEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define DEFAULT_COMBO_SECTION "\\ValuesCombo\\Default\\"
#define DEFAULT_COMBO_SECTION "\\Default\\ValuesCombo\\"

CString CopyValueName(LPCTSTR lpstrSrc)
{
	int nLen = _tcslen(lpstrSrc);
	while (nLen > 0 && (lpstrSrc[nLen-1] == _T(' ') || lpstrSrc[nLen-1] == _T(':')))
		nLen--;
	CString s(lpstrSrc, nLen);
//	_tcsncpy(lpstrDest, lpstrSrc, nLen);
//	lpstrDest[nLen] = 0;
	return s;
}


/////////////////////////////////////////////////////////////////////////////
// CEnterValuesDlg dialog


CEnterValuesDlg::CEnterValuesDlg(request_impl *p, CWnd* pParent /*=NULL*/)
	: CDialog(CEnterValuesDlg::IDD, pParent)
{
	m_pdata = p;
	//{{AFX_DATA_INIT(CEnterValuesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nIdPrevActive = -1;
}

CEnterValuesDlg::~CEnterValuesDlg()
{
    clear_editex_list( );
}


void CEnterValuesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEnterValuesDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEnterValuesDlg, CDialog)
	//{{AFX_MSG_MAP(CEnterValuesDlg)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_COMBO_DEFAULT, OnBnClickedComboDefault)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnterValuesDlg message handlers

BOOL CEnterValuesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// vanek
	clear_editex_list( );
	
	if( !m_strTitle.IsEmpty() )
		SetWindowText( m_strTitle );

	m_pdata->layout( this );
	CRect	rect;
	GetWindowRect( &rect );

	CRect	rect_client;
	GetClientRect( &rect_client );

	int	delta = rect.Width()-rect_client.Width();

	int	dx = max( rect.Width(), m_pdata->m_size.cx+delta )-rect.Width();
	int	dy = m_pdata->m_size.cy;

	rect.bottom += dy;
	rect.right += dx;
	MoveWindow( &rect );

	GetDlgItem( IDOK )->GetWindowRect( &rect );
	ScreenToClient( rect );
	rect.OffsetRect( dx, dy );
	GetDlgItem( IDOK )->MoveWindow( &rect );

	GetDlgItem( IDCANCEL )->GetWindowRect( &rect );
	ScreenToClient( rect );
	rect.OffsetRect( dx, dy );
	GetDlgItem( IDCANCEL )->MoveWindow( &rect );

	GetDlgItem( IDC_COMBO_DEFAULT )->GetWindowRect( &rect );
	ScreenToClient( rect );
	rect.OffsetRect( dx, dy );
	GetDlgItem( IDC_COMBO_DEFAULT )->MoveWindow( &rect );

	GetDlgItem( IDHELP )->GetWindowRect( &rect );
	ScreenToClient( rect );
	rect.OffsetRect( dx, dy );
 	GetDlgItem( IDHELP )->MoveWindow( &rect );

	HFONT	hfont = (HFONT)SendMessage( WM_GETFONT );

	CClientDC	dc( 0 );
	dc.SelectObject( hfont );

	for( group_data *pg = m_pdata->first_group; pg; pg = pg->next )
	{
		HWND hwnd_group = ::CreateWindow( "button", pg->caption, WS_CHILD|WS_VISIBLE|BS_GROUPBOX, 
			pg->rect.left, pg->rect.top, pg->rect.Width(), pg->rect.Height(),
			GetSafeHwnd(), (HMENU)-1, AfxGetInstanceHandle(), 0 );

		::SendMessage( hwnd_group, WM_SETFONT, (WPARAM)hfont, 0 );

		for( control_data *p = pg->first_control; p; p = p->next )
		{
			bool	bMultyLine = 
				(p->params.flags & PFF_TEXT_STYLE_MULTYLINE) == PFF_TEXT_STYLE_MULTYLINE &&
				(p->params.flags & PFF_TYPE_MASKS) == PFF_TYPE_TEXT;
			CSize	size = dc.GetTextExtent( p->caption, p->caption.length() );

			RECT rcc = { 0 };
			dc.DrawText( (char *)p->caption, &rcc, DT_CALCRECT );

			size.cx = rcc.right - rcc.left;
			size.cy = rcc.bottom - rcc.top;


			if( bMultyLine )
			{
				p->rect_static.bottom = p->rect_static.top+size.cy;
				p->rect_edit.top = p->rect_static.bottom+4;
			}
			HWND hwnd_static = ::CreateWindow( "static", p->caption, WS_CHILD|WS_VISIBLE, 
				p->rect_static.left, p->rect_static.CenterPoint().y-size.cy/2, size.cx, size.cy, 
				GetSafeHwnd(), (HMENU)p->id_static, AfxGetInstanceHandle(), 0 );
			::SendMessage( hwnd_static, WM_SETFONT, (WPARAM)hfont, 0 );

			
			bool	bCreateSlider = false;
			CString	str_def;
			long	ldef = 0;

//			::strencode( sz_e, p->caption, ":\n\r\\", sizeof( sz_e ) );
//			::strcpy( sz_e, p->caption );
			CString sz_e = CopyValueName(p->caption);

			if( (p->params.flags & PFF_TYPE_MASKS) == PFF_TYPE_TEXT || 
				(p->params.flags & PFF_TYPE_MASKS) == PFF_TYPE_DATE )   // [vanek] BT2000: 3671 - 10.03.2004
				str_def = ::GetValueString( GetAppUnknown(), "\\Values", sz_e, "" );
			else if( (p->params.flags & PFF_TYPE_MASKS) == PFF_TYPE_NUMERIC )
			{
				if( (p->params.flags & PFF_NUMERIC_TYPE_MASK) == PFF_NUMERIC_TYPE_INTEGER )
				{
					ldef = ::GetValueInt( GetAppUnknown(), "\\Values", sz_e, p->params.ldef );
					str_def.Format( "%ld", ldef );
					bCreateSlider  = true;
				}
				else if( (p->params.flags & PFF_NUMERIC_TYPE_MASK) == PFF_NUMERIC_TYPE_DOUBLE )
				{
					double fdef = ::GetValueDouble( GetAppUnknown(), "\\Values", sz_e, p->params.fdef );
					if( p->data_format != -1 )
					{
						char format[MAX_PATH], _format[MAX_PATH];
						sprintf( _format, "%d", p->data_format );
						strcpy( format, "%." );
						strcat( format, _format );
						strcat( format, "lf" );

						str_def.Format( format, fdef );

					}
					else
						str_def = "";
				}
			}
			
/*			if( p->params.flags & PFF_TEXT_STYLE_INITBYDATE )
			{
				str_def =COleDateTime::GetCurrentTime().Format();
			}*/

			DWORD	dw_style = WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_BORDER;

			HWND	hwnd;

			if( (p->params.flags & PFF_TYPE_MASKS) == PFF_TYPE_DATE ||
				(p->params.flags & PFF_TYPE_MASKS) == PFF_TYPE_TEXT && 
				(p->params.flags & PFF_TEXT_STYLE_INITBYDATE)  )
			{
				dw_style |= DTS_SHORTDATECENTURYFORMAT;
				if ((p->params.flags & PFF_TEXT_STYLE_INITBYDATE) == 0)
					dw_style |= DTS_SHOWNONE;
				HWND hwnd = ::CreateWindowEx(0, DATETIMEPICK_CLASS, "DateTime", dw_style,
					p->rect_edit.left, p->rect_edit.top, p->rect_edit.Width(), p->rect_edit.Height(),
					GetSafeHwnd(), (HMENU)p->id, AfxGetInstanceHandle(), 0 );
				if ( !str_def.IsEmpty() )
				{
					COleDateTime DTime;
					DTime.ParseDateTime( str_def );
					SYSTEMTIME stime;
					DTime.GetAsSystemTime( stime );
					DateTime_SetSystemtime( hwnd, GDT_VALID, &stime);
				}
				else
					dw_style &= ~DTS_SHOWNONE;

			}
			else if( p->params.flags & PFF_TEXT_STYLE_COMBOBOX || p->params.flags & PFF_TEXT_STYLE_COMBOBOX2 )
			{
				if( p->params.flags & PFF_TEXT_STYLE_COMBOBOX )
					dw_style |= CBS_AUTOHSCROLL|CBS_DROPDOWN;
				else
					dw_style |= CBS_AUTOHSCROLL|CBS_DROPDOWNLIST;

				dw_style &= ~CBS_AUTOHSCROLL;

				HWND hwnd = 
				::CreateWindowEx( WS_EX_CLIENTEDGE, "combobox", str_def, dw_style, 
					p->rect_edit.left, p->rect_edit.top, p->rect_edit.Width(), p->rect_edit.Height(), 
					GetSafeHwnd(), (HMENU)p->id, AfxGetInstanceHandle(), 0 );
				::SendMessage( hwnd, WM_SETFONT, (WPARAM)hfont, 0 );


				INamedDataPtr	ptrNamedData( GetAppUnknown() );
				ptrNamedData->SetupSection( _bstr_t( "\\ValuesCombo\\" )+(const char *)sz_e );
				long	count;
				ptrNamedData->GetEntriesCount( &count );

				int nCurSel = -1;
				for( long i = 0; i < count; i++ )
				{
					BSTR	bstr;
					ptrNamedData->GetEntryName( i, &bstr );
					_bstr_t	bstr_entry = bstr;
					::SysFreeString( bstr );
					if (bstr_entry == _bstr_t(str_def))
						nCurSel = i;
					::SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)(const char*)bstr_entry );
				}

				// [vanek] BT2000: 3539. Глюк combobox: когд?установлен стил?CBS_DROPDOWN, список пуст ?
				// установлен?заведомо большая высота - не происходит снятия фокуса ?контрола (видимо на величину 
				// установленно?высоты), поэтом?увеличивае?высоту, только когд?добавили строки
				if(count > 0)
					::SetWindowPos( hwnd, 0, 0, 0,  p->rect_edit.Width(), p->rect_edit.Height() + 100, 
								SWP_NOMOVE|SWP_NOZORDER );

				if (p->params.flags & PFF_TEXT_STYLE_COMBOBOX2)
					::SendMessage( hwnd, CB_SETCURSEL, (WPARAM)nCurSel, 0);
				else
					::SetWindowText( hwnd, str_def );

			}
			else
			{
				if( p->params.flags & PFF_TEXT_STYLE_MULTYLINE )
					dw_style |= ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL;
				else
					dw_style |= ES_AUTOHSCROLL;

				if( p->params.flags & PFF_STYLE_ALIGN_L )
					dw_style |= ES_LEFT;
				else if( p->params.flags & PFF_STYLE_ALIGN_C )
					dw_style |= ES_CENTER;
				else if( p->params.flags & PFF_STYLE_ALIGN_R )
					dw_style |= ES_RIGHT;
				
				// vanek
				 
                hwnd = 
				::CreateWindowEx( WS_EX_CLIENTEDGE, "edit", str_def, dw_style, 
					p->rect_edit.left, p->rect_edit.top, p->rect_edit.Width(), p->rect_edit.Height(), 
					GetSafeHwnd(), (HMENU)p->id, AfxGetInstanceHandle(), 0 );

				if ( hwnd ) 
				{
					DWORD dwFlags = 0;
					
					if( (p->params.flags & PFF_TYPE_MASKS) == PFF_TYPE_NUMERIC )
					{
						dwFlags |= EDITEX_TYPE_NUMERIC;
						if( (p->params.flags & PFF_NUMERIC_TYPE_MASK) == PFF_NUMERIC_TYPE_INTEGER )
							dwFlags |= EDITEX_NUMERIC_TYPE_INTEGER;
						else if( (p->params.flags & PFF_NUMERIC_TYPE_MASK) == PFF_NUMERIC_TYPE_DOUBLE )
							dwFlags |= EDITEX_NUMERIC_TYPE_DOUBLE;
					}
					else if( (p->params.flags & PFF_TYPE_MASKS) == PFF_TYPE_TEXT )
						dwFlags |= EDITEX_TYPE_TEXT;

                    CEditEx *peditex = new CEditEx( dwFlags );
					if( peditex->SubclassWindow( hwnd ) )
					{
						::SendMessage( hwnd, WM_SETFONT, (WPARAM)hfont, 0 );
						m_EditExList.AddTail( peditex );
					}
					else
						delete peditex;	peditex = 0;
				}
				

				


			}
			if( bCreateSlider )
			{
				long	lmin = -10000;
				long	lmax = 10000;
			
				if( p->params.flags & PFF_NUMERIC_STYLE_DEFAULT )
					ldef = p->params.ldef;
				if( p->params.flags & PFF_NUMERIC_STYLE_RANGES )
				{
					lmin = p->params.lmin;
					lmax = p->params.lmax;
				}


				::CreateUpDownControl( WS_CHILD|WS_VISIBLE|UDS_SETBUDDYINT|UDS_ALIGNRIGHT|UDS_ARROWKEYS|UDS_NOTHOUSANDS, 
					0, 0, 0, 0, GetSafeHwnd(), p->id_spin, AfxGetInstanceHandle(),
					hwnd, lmax, lmin, ldef );
			}
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEnterValuesDlg::OnOK() 
{
	char	sz[4096];
	CString	sValCombo = "\\ValuesCombo\\";

	for( group_data *pg = m_pdata->first_group; pg; pg = pg->next )
	{
		for( control_data *p = pg->first_control; p; p = p->next )
		{
//			::strencode( sz_e, p->caption, ":\n\r\\", sizeof( sz_e ) );
//			::strcpy( sz_e, p->caption );
			CString sz_e = CopyValueName(p->caption);
			if( (p->params.flags & PFF_TYPE_MASKS) == PFF_TYPE_DATE ||
				p->params.flags & PFF_TEXT_STYLE_INITBYDATE )
			{
				HWND hwnd = ::GetDlgItem(m_hWnd, p->id);
				SYSTEMTIME SysTime;
				if (DateTime_GetSystemtime(hwnd,&SysTime) == GDT_VALID)
				{
					COleDateTime DateTime(SysTime);
					CString sDate = DateTime.Format(VAR_DATEVALUEONLY);
					SetValue( GetAppUnknown(), "\\Values", sz_e, sDate );
				}
				else
					SetValue( GetAppUnknown(), "\\Values", sz_e, (const char*)0 );
			}
			else
			{
				GetDlgItemText( p->id, sz, sizeof( sz ) );
				int ntext_len = strlen( sz );
				if( p->params.flags & PFF_TEXT_STYLE_COMBOBOX || p->params.flags & PFF_TEXT_STYLE_COMBOBOX2 )
					if( ntext_len )
						::SetValue( GetAppUnknown(), sValCombo+(const char*)sz_e, sz, 0l );
				if( (p->params.flags & PFF_TYPE_MASKS) == PFF_TYPE_NUMERIC )
				{
					if( (p->params.flags & PFF_NUMERIC_TYPE_MASK) == PFF_NUMERIC_TYPE_INTEGER )
					{
						if( ntext_len )
						{
							long	val;
							if( sscanf( sz, "%d", &val ) != 1 )
							{
								AfxMessageBox( IDS_NOINTEGER );
								return;
							}
							if( p->params.flags & PFF_NUMERIC_STYLE_RANGES )
							{
								if( val < p->params.lmin || val > p->params.lmax )
								{
									CString	str;
									str.Format( IDS_INT_NOTININTERVAL, val, p->params.lmin, p->params.lmax );
									AfxMessageBox( str );
									return;
								}
							}
							SetValue( GetAppUnknown(), "\\Values", sz_e, val );
						}
						else
							SetValue( GetAppUnknown(), "\\Values", sz_e, 0L );
					}
					else if( (p->params.flags & PFF_NUMERIC_TYPE_MASK) == PFF_NUMERIC_TYPE_DOUBLE )
					{
						if( ntext_len )
						{
							double	val;
							if( sscanf( sz, "%lf", &val ) != 1 )
							{
								AfxMessageBox( IDS_NODOUBLE );
								return;
							}
							if( p->params.flags & PFF_NUMERIC_STYLE_RANGES )
							{
								if( val < p->params.fmin || val > p->params.fmax )
								{
									CString	str;
									str.Format( IDS_DOUBLE_NOTININTERVAL, val, p->params.fmin, p->params.fmax );
									AfxMessageBox( str );
									return;
								}
							}
							SetValue( GetAppUnknown(), "\\Values", sz_e, val );
						}
						else
							SetValue( GetAppUnknown(), "\\Values", sz_e, 0L );
					}
				}
				else if( (p->params.flags & PFF_TYPE_MASKS) == PFF_TYPE_TEXT )
					SetValue( GetAppUnknown(), "\\Values", sz_e, sz );
			}
		}
	}
	
	CDialog::OnOK();
}

void CEnterValuesDlg::OnHelp() 
{
	HelpDisplay( "UserValues" );
}

BOOL CEnterValuesDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (nCode == CBN_SETFOCUS)
	{
		for( group_data *pg = m_pdata->first_group; pg; pg = pg->next )
		{
			for( control_data *p = pg->first_control; p; p = p->next )
			{
				if (p->id == nID)
				{
//					INamedDataPtr	ptrNamedData( GetAppUnknown() );
//					ptrNamedData->SetupSection( _bstr_t( DEFAULT_COMBO_SECTION )+p->caption);
//					long	count;
//					ptrNamedData->GetEntriesCount(&count);
//					::EnableWindow(::GetDlgItem(m_hWnd, IDC_COMBO_DEFAULT), count>0);
					::EnableWindow(::GetDlgItem(m_hWnd, IDC_COMBO_DEFAULT), TRUE);
					m_nIdPrevActive = nID;
					return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
				}
			}
		}
	}
	else if (nCode == CBN_KILLFOCUS)
	{
		CWnd *pWnd = GetFocus();
		if (pWnd == NULL || pWnd->GetDlgCtrlID() != IDC_COMBO_DEFAULT)
		{
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_COMBO_DEFAULT), FALSE);
			m_nIdPrevActive = -1;
		}
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CEnterValuesDlg::OnBnClickedComboDefault()
{
	CComboBox ComboBox;
	ComboBox.Attach(::GetDlgItem(m_hWnd,m_nIdPrevActive));
	if (ComboBox.m_hWnd != NULL)
	{
		control_data *pFocus = NULL;
		for( group_data *pg = m_pdata->first_group; pg; pg = pg->next )
		{
			for( control_data *p = pg->first_control; p; p = p->next )
			{
				if (p->id == m_nIdPrevActive)
				{
					pFocus = p;
					break;
				}
			}
		}
		if (pFocus)
		{
			long count;
			CArray<BSTR,BSTR&> saDefault;
			INamedDataPtr	ptrNamedData( GetAppUnknown() );
			_bstr_t sCap = CopyValueName(pFocus->caption);
			ptrNamedData->SetupSection(_bstr_t(DEFAULT_COMBO_SECTION)+sCap);
			ptrNamedData->GetEntriesCount(&count);
			BSTR bstr;
			for(long i = 0; i < count; i++)
			{
				ptrNamedData->GetEntryName(i, &bstr);
				saDefault.Add(bstr);
			}
			// Remove entries which are not contained in default section
			ptrNamedData->SetupSection( _bstr_t( "\\ValuesCombo\\" )+sCap);
			CArray<BSTR,BSTR&> sa;
			ptrNamedData->GetEntriesCount(&count);
			// Make array of entries to delete
			for(long i = 0; i < count; i++)
			{
				ptrNamedData->GetEntryName(i, &bstr);
				bool bFound = false;
				for(long j = 0; j < saDefault.GetSize(); j++)
				{
					if (_bstr_t(bstr) == _bstr_t(saDefault[j]))
					{
						bFound = true;
						break;
					}
				}
				if (!bFound)
					sa.Add(bstr);
				else
					::SysFreeString(bstr);
			}
			// Delete it
			for(i = 0; i < sa.GetSize(); i++)
			{
				ptrNamedData->DeleteEntry(sa[i]);
				::SysFreeString(sa[i]);
			}
			// Now remove values from combobox's list.
			CString sText;
			ComboBox.GetWindowText(sText);
			ComboBox.ResetContent();
			int nCurSel = 0;
			for(i = 0; i < saDefault.GetSize(); i++)
			{
				CString s(saDefault[i]);
				if (s == sText)
					nCurSel = i;
				ComboBox.AddString(s);
				::SysFreeString(saDefault[i]);
			}
			ComboBox.SetCurSel(nCurSel);
		}
	}
	ComboBox.SetFocus();
	ComboBox.Detach();
//	::EnableWindow(::GetDlgItem(m_hWnd, IDC_COMBO_DEFAULT), FALSE);
}
