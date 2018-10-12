#include "stdafx.h"
#include "resource.h"
#include "InputMtdName.h"
#include "mtd_utils.h"

/////////////////////////////////////////////////////////////
/// CNewMetoddDlg class implementation
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
CInputMtdNameDlg::CInputMtdNameDlg() : dlg_impl( IDD_DLG_INPUT_MTD_NAME )
{
	m_str_method_name = _T("");
	m_str_caption = _T("");
	m_bcheck_unique = FALSE;
	m_ballow_overwrite = FALSE;
	m_balready_exists = FALSE;
	m_bhide_static_text = FALSE;
}

/////////////////////////////////////////////////////////////
CInputMtdNameDlg::~CInputMtdNameDlg()
{
}

/////////////////////////////////////////////////////////////
LRESULT	CInputMtdNameDlg::on_initdialog()
{
    LRESULT lres = dlg_impl::on_initdialog();

	if( !m_str_caption.IsEmpty() )
		::SetWindowText( handle(), m_str_caption );
	
	if( !m_str_method_name.IsEmpty() )
	{
		HWND hwnd_mtd_edit = 0;
		hwnd_mtd_edit = ::GetDlgItem( handle(), IDC_EDIT_MTD_NAME );
		if( hwnd_mtd_edit )
			::SetWindowText( hwnd_mtd_edit, m_str_method_name );
	}

	if( m_bhide_static_text )
	{
		HWND hwnd_edit = 0;
		hwnd_edit = ::GetDlgItem( handle(), IDC_EDIT_MTD_NAME );

		HWND hwnd_static = 0;
		hwnd_static = ::GetDlgItem( handle(), IDC_STATIC );
		if( hwnd_static && hwnd_edit )
		{
			RECT rc_static = {0}, rc_edit = {0};
            GetWindowRect( hwnd_edit, &rc_edit );
			GetWindowRect( hwnd_static, &rc_static );
			rc_edit.left = rc_static.left;
			::ScreenToClient( handle(), (POINT *)(&rc_edit) );
			::ScreenToClient( handle(), ((POINT *)(&rc_edit) + 1) );
			::MoveWindow( hwnd_edit, rc_edit.left, rc_edit.top, rc_edit.right - rc_edit.left, 
				rc_edit.bottom - rc_edit.top, TRUE );

			::ShowWindow( hwnd_static, SW_HIDE );            			
		}
	}

	update_ok( );
	return lres;	
}

/////////////////////////////////////////////////////////////
void CInputMtdNameDlg::on_ok()
{
	HWND hwnd_mtd_edit = 0;
	hwnd_mtd_edit = ::GetDlgItem( handle(), IDC_EDIT_MTD_NAME );
	if( hwnd_mtd_edit )
	{
		LRESULT ntext_length = 0;
		ntext_length = ::SendMessage( hwnd_mtd_edit, EM_LINELENGTH, (WPARAM)0, (LPARAM) 0);
		if( ntext_length )
		{
			ntext_length ++;
			m_str_method_name.Empty();
			::GetWindowText( hwnd_mtd_edit, m_str_method_name.GetBufferSetLength(ntext_length), ntext_length );
			m_str_method_name.ReleaseBuffer( );           
			m_str_method_name.TrimLeft( );
			m_str_method_name.TrimRight( );
		}
	}

	if( m_bcheck_unique && !m_str_method_name.IsEmpty() )
	{
        IUnknown *punk_mtd = 0;
		get_method_by_name( _bstr_t( m_str_method_name ), 0, &punk_mtd );

		m_balready_exists = (punk_mtd!=0);

		if( punk_mtd )
		{
			punk_mtd->Release(); 
			punk_mtd = 0;

            CString str_caption( _T("") );
			int ntext_length = 0;
			ntext_length = ::GetWindowTextLength( handle() );
			::GetWindowText( handle(), str_caption.GetBufferSetLength( ntext_length + 1 ), ntext_length + 1);
			str_caption.ReleaseBuffer();		

			if(m_ballow_overwrite)
			{ // можно перезаписывать существующие, если юзер разрешит
				CString str_msg( _T("") );
				str_msg.Format( (UINT)(IDS_METHOD_EXIST_OVERWRITE), m_str_method_name );
				if(IDYES != ::MessageBox( handle(), str_msg, str_caption, MB_YESNO | MB_ICONWARNING ) )
					return; // если не сказали "да" - то вернемся обратно в диалог
			}
			else
			{ // нельзя
				CString str_msg( _T("") );
				str_msg.Format( (UINT)(IDS_METHOD_EXIST), m_str_method_name );
				::MessageBox( handle(), str_msg, str_caption, MB_OK | MB_ICONWARNING );
				return;
			}
		}
	}

    return __super::on_ok( );
}

/////////////////////////////////////////////////////////////
LRESULT CInputMtdNameDlg::on_command(uint cmd)
{
    if( LOWORD(cmd) == IDC_EDIT_MTD_NAME && HIWORD(cmd) == EN_CHANGE  )
	{        
		update_ok( );
		return 1L;
    }

	return __super::on_command( cmd );
}

/////////////////////////////////////////////////////////////
void	CInputMtdNameDlg::update_ok( )
{
    CString str( _T("") ); 
	LRESULT llength = 0;
	llength = ::SendMessage( get_dlg_item( IDC_EDIT_MTD_NAME ), EM_LINELENGTH, (WPARAM)(0), (LPARAM)(0) );
	llength = llength + 1;
	get_dlg_item_text( IDC_EDIT_MTD_NAME, str.GetBufferSetLength( llength ), llength);
	str.ReleaseBuffer( );
	str.TrimLeft();
	::EnableWindow( get_dlg_item( IDOK ), !str.IsEmpty() );
}

