#include "stdafx.h"
#include "resource.h"
#include "InputMsg.h"
#include "mtd_utils.h"

/////////////////////////////////////////////////////////////
/// CNewMetoddDlg class implementation
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
CInputMsgDlg::CInputMsgDlg() : dlg_impl( IDD_DLG_INPUT_MSG )
{
	m_str_caption = _T("");
	m_str_msg = _T("");
}

/////////////////////////////////////////////////////////////
CInputMsgDlg::~CInputMsgDlg()
{
}

/////////////////////////////////////////////////////////////
long	CInputMsgDlg::on_initdialog()
{
    long lres = dlg_impl::on_initdialog();

	if( !m_str_caption.IsEmpty() )
		::SetWindowText( handle(), m_str_caption );
	
	if( !m_str_msg.IsEmpty() )
	{
		HWND hwnd_edit = 0;
		hwnd_edit = ::GetDlgItem( handle(), IDC_EDIT_MSG );
		if( hwnd_edit )
			::SetWindowText( hwnd_edit, m_str_msg );
	}

	return lres;	
}

/////////////////////////////////////////////////////////////
void CInputMsgDlg::on_ok()
{
	HWND hwnd_edit = 0;
	hwnd_edit = ::GetDlgItem( handle(), IDC_EDIT_MSG );
	if( hwnd_edit )
	{
		int ntext_length = 0;
		ntext_length = ::GetWindowTextLength( hwnd_edit );
		if( ntext_length )
		{
			ntext_length ++;
			m_str_msg.Empty();
			::GetWindowText( hwnd_edit, m_str_msg.GetBufferSetLength(ntext_length), ntext_length );
			m_str_msg.ReleaseBuffer( );           
		}
	}

    return __super::on_ok( );
}

/////////////////////////////////////////////////////////////
LRESULT CInputMsgDlg::on_command(uint cmd)
{
    if( LOWORD(cmd) == IDC_EDIT_MSG && HIWORD(cmd) == EN_CHANGE  )
	{        
		//update_ok( );
		return 1L;
    }

	return __super::on_command( cmd );
}
