// UserInfo.cpp : implementation file
//

#include "stdafx.h"
#include "DBaseDoc.h"
#include "UserInfo.h"


// CUserInfoDlg dialog

IMPLEMENT_DYNAMIC(CUserInfoDlg, CDialog)
CUserInfoDlg::CUserInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUserInfoDlg::IDD, pParent)
{
}

CUserInfoDlg::~CUserInfoDlg()
{
}

void CUserInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CUserInfoDlg::OnInitDialog()
{
	BOOL bres = __super::OnInitDialog();
	
	CArray<user_info,user_info&> arr_users;
	GetUsersInfo( &arr_users );

	HWND hwnd_list = ::GetDlgItem( m_hWnd, IDC_LIST );
	if( hwnd_list )
	{
		ListView_SetExtendedListViewStyle( hwnd_list, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP );
		//insert columns
		{
			char sz_user[255];		sz_user[0] = 0;
			char sz_machine[255];	sz_machine[0] = 0;
			::LoadString( AfxGetApp()->m_hInstance, IDS_USER, sz_user, sizeof(sz_user) );
			::LoadString( AfxGetApp()->m_hInstance, IDS_MACHINE, sz_machine, sizeof(sz_machine) );

			RECT rc = {0};
			::GetClientRect( hwnd_list, &rc );
			int nwidth = rc.right - rc.left;

			LVCOLUMN column;
			memset( &column, 0, sizeof(LVCOLUMN) );

			column.mask		= LVCF_TEXT | LVCF_WIDTH;
			column.cx		= nwidth / 2 - 20;
			column.pszText	= sz_user;

			ListView_InsertColumn( hwnd_list, 0, &column );	

			column.cx		= nwidth / 2;
			column.pszText	= sz_machine;	
			ListView_InsertColumn( hwnd_list, 1, &column );
		}

		for( int idx=0; idx<arr_users.GetSize(); idx++ )
		{
			user_info& ui = arr_users[idx];

			LVITEM item = {0};
			//check box
			item.mask		= LVIF_TEXT;
			item.iSubItem	= 0;
			item.iItem		= 0;
			item.pszText	= (LPSTR)(const char*)ui.m_str_user;

			ListView_InsertItem( hwnd_list, &item );

			//caption
			item.mask		= LVIF_TEXT;
			item.iSubItem	= 1;		
			item.pszText	= (LPSTR)(const char*)ui.m_str_machine;

			ListView_SetItem( hwnd_list, &item );
		}
	}

	return bres;
}






BEGIN_MESSAGE_MAP(CUserInfoDlg, CDialog)
END_MESSAGE_MAP()


// CDDLErrorDlg dialog

IMPLEMENT_DYNAMIC(CDDLErrorDlg, CDialog)
CDDLErrorDlg::CDDLErrorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDDLErrorDlg::IDD, pParent)
{
}

CDDLErrorDlg::~CDDLErrorDlg()
{
}

void CDDLErrorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDDLErrorDlg, CDialog)
	ON_COMMAND(IDC_USER_INFO,OnUserInfo)
END_MESSAGE_MAP()


// CDDLErrorDlg message handlers

void CDDLErrorDlg::OnUserInfo()
{
	CUserInfoDlg dlg;
	dlg.DoModal();
}