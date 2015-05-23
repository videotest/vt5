// VersionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "shell.h"
#include "VersionDlg.h"
#include "tlhelp32.h"

class CActionShowVersion : public CActionBase
{
	DECLARE_DYNCREATE(CActionShowVersion)
	GUARD_DECLARE_OLECREATE(CActionShowVersion)

public:
	CActionShowVersion(){}
	virtual ~CActionShowVersion(){}

public:
	virtual bool Invoke();
};


IMPLEMENT_DYNCREATE(CActionShowVersion, CCmdTargetEx);
// {625242BB-A913-45da-9245-2531ED82B04E}
GUARD_IMPLEMENT_OLECREATE(CActionShowVersion, "Shell.ShowVersion",
0x625242bb, 0xa913, 0x45da, 0x92, 0x45, 0x25, 0x31, 0xed, 0x82, 0xb0, 0x4e);


// {60A7F5C2-0A3B-44b6-AEB3-BDAF55887999}
static const GUID guidShowVersion = 
{ 0x60a7f5c2, 0xa3b, 0x44b6, { 0xae, 0xb3, 0xbd, 0xaf, 0x55, 0x88, 0x79, 0x99 } };


ACTION_INFO_FULL(CActionShowVersion, IDS_ACTION_SHOW_VERSION, -1, -1, guidShowVersion);

bool CActionShowVersion::Invoke()
{
	CVersionDlg dlg;
	dlg.DoModal();
	return true;
}


#define VERSION_INFO_SECTION	"\\VersionInfo"
#define VERSION_INFO_ONLY_VT5	"ShowVT5FilesOnly"
/////////////////////////////////////////////////////////////////////////////
// CVersionDlg dialog


CVersionDlg::CVersionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVersionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVersionDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CVersionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVersionDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVersionDlg, CDialog)
	//{{AFX_MSG_MAP(CVersionDlg)
	ON_BN_CLICKED(IDC_SHOW_ONLY_OUR, OnShowOnlyOur)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVersionDlg message handlers

BOOL CVersionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	HWND hwnd = ::GetDlgItem( GetSafeHwnd(), IDC_LIST );
	if( !hwnd )	return false;

	ListView_SetExtendedListViewStyle( hwnd, LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT );

	RECT rc;
	memset( &rc, 1, sizeof(RECT) );
	::GetClientRect( hwnd, &rc );

	LVCOLUMN col;
	memset( &col, 0, sizeof(LVCOLUMN) );

	col.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM|LVCF_FMT;
	col.cx = rc.right - rc.left - 120 - 100 - 100;
	col.pszText = "File Name";
	col.iSubItem = 0;
	col.fmt = LVCFMT_LEFT;
	ListView_InsertColumn( hwnd, col.iSubItem, &col );
	
	col.cx = 100;
	col.pszText = "Version";
	col.iSubItem++;
	col.fmt = LVCFMT_RIGHT;
	ListView_InsertColumn( hwnd, col.iSubItem, &col );

	col.cx = 100;
	col.pszText = "Date";
	col.iSubItem++;
	col.fmt = LVCFMT_RIGHT;
	ListView_InsertColumn( hwnd, col.iSubItem, &col );

	col.cx = 100;
	col.pszText = "Size";
	col.iSubItem++;
	col.fmt = LVCFMT_RIGHT;
	ListView_InsertColumn( hwnd, col.iSubItem, &col );

	if( 1L == ::GetValueInt( ::GetAppUnknown(), VERSION_INFO_SECTION, VERSION_INFO_ONLY_VT5, 1 ) )
		::CheckDlgButton( GetSafeHwnd(), IDC_SHOW_ONLY_OUR, BST_CHECKED );
	else
		::CheckDlgButton( GetSafeHwnd(), IDC_SHOW_ONLY_OUR, BST_UNCHECKED );

	fill_list();
	
	return true;
}

bool CVersionDlg::fill_list()
{	
	CWaitCursor wait;

	HWND hwnd = ::GetDlgItem( GetSafeHwnd(), IDC_LIST );
	if( !hwnd )	return false;

	ListView_DeleteAllItems( hwnd );
	char sz_exe_path[MAX_PATH];	sz_exe_path[0] = 0;
	::GetModuleFileName( 0, sz_exe_path, sizeof(sz_exe_path) );
	{
		char* psz = strrchr( sz_exe_path, '\\' );
		if( psz )
			*psz = 0;
	}

	bool bour_files = ( 1L == ::GetValueInt( ::GetAppUnknown(), VERSION_INFO_SECTION, VERSION_INFO_ONLY_VT5, 1 ) );

	int idx = 0;
	LV_ITEM	item = {0};
	
	MODULEENTRY32 me32	= {0}; 

	DWORD dw_pid = ::GetCurrentProcessId();
	
	me32.dwSize = sizeof(MODULEENTRY32);
	me32.th32ProcessID = dw_pid;
	
	DWORD dw_size_all = 0;
	
	HANDLE hModuleSnap	= 0; 
	hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dw_pid ); 
    if( hModuleSnap != INVALID_HANDLE_VALUE )
	{
		
		if( Module32First( hModuleSnap, &me32 ) )
		{			
			do
			{				
				char sz_file_name[MAX_PATH] = {0};
				GetLongPathName( me32.szExePath, sz_file_name, sizeof(sz_file_name) );

				strlwr( sz_file_name );
				strlwr( sz_exe_path );

				if( !bour_files || ( bour_files && strstr( sz_file_name, sz_exe_path ) ) )
				{
					//insert file name
					item.mask = LVIF_TEXT|LVIF_IMAGE;
					item.iItem = idx++;
					item.iSubItem = 0;
					item.pszText = sz_file_name;
					ListView_InsertItem( hwnd, &item );
					
					//insert vi
					item.iSubItem++;				
					char sz_buf[256];	sz_buf[0] = 0;
					DWORD dw_size = GetFileVersionInfoSize( me32.szExePath, 0 );
					if( dw_size )
					{
						BYTE* pbuf = new BYTE[dw_size];
						if( pbuf )
						{
							if( GetFileVersionInfo( me32.szExePath, 0, dw_size, pbuf ) ) 
							{
								VS_FIXEDFILEINFO* pffi = 0;
								UINT     uFfiLen = 0;
								if( VerQueryValue( pbuf, "\\", (void**)&pffi, &uFfiLen ) )
								{
									WORD w0 = HIWORD( pffi->dwFileVersionMS );
									WORD w1 = LOWORD( pffi->dwFileVersionMS );
									WORD w2 = HIWORD( pffi->dwFileVersionLS );
									WORD w3 = LOWORD( pffi->dwFileVersionLS );
									
									sprintf( sz_buf, "%d.%d.%d.%d", w0, w1, w2, w3 );								
								}
							}
							delete pbuf;
						}					
					}
					item.pszText = sz_buf;
					ListView_SetItem( hwnd, &item );					
					
					char	sz_time[100] = "--";
					char	sz_size[100] = "--";

					HANDLE	hFile = ::CreateFile( sz_file_name, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0 );
					if( hFile != INVALID_HANDLE_VALUE )
					{
						//insert date
						FILETIME	filetime, ft;
						::GetFileTime( hFile, 0, 0, &filetime );
						::FileTimeToLocalFileTime( &filetime, &ft );
						SYSTEMTIME	systime;
						::FileTimeToSystemTime( &ft, &systime );
						sprintf( sz_time, "%d.%d.%d %d:%d", systime.wDay, systime.wMonth, systime.wYear, systime.wHour, systime.wMinute );

						item.pszText = sz_time;
						item.iSubItem++;
						ListView_SetItem( hwnd, &item );

						//insert size
						DWORD	dw_size = ::GetFileSize( hFile, 0 );
						::sprintf( sz_size, "%d Bytes", dw_size );
						item.pszText = sz_size;
						item.iSubItem++;
						ListView_SetItem( hwnd, &item );

						::CloseHandle( hFile );
						dw_size_all += dw_size;
					}										
				}

			}while( Module32Next( hModuleSnap, &me32 ) );
		}		

		CloseHandle( hModuleSnap );
	}

	item.pszText = "All size";
	item.iItem = idx++;
	item.iSubItem = 0;
	ListView_InsertItem( hwnd, &item );

	char sz_size[100];
	::sprintf( sz_size, "%d Bytes", dw_size_all );
	item.pszText = sz_size;
	item.iSubItem = 3;
	ListView_SetItem( hwnd, &item );


	return TRUE;
}

void CVersionDlg::OnShowOnlyOur() 
{
	if( ::IsDlgButtonChecked( GetSafeHwnd(), IDC_SHOW_ONLY_OUR ) == BST_CHECKED )
		::SetValue( ::GetAppUnknown(), VERSION_INFO_SECTION, VERSION_INFO_ONLY_VT5, (long)1 );
	else
		::SetValue( ::GetAppUnknown(), VERSION_INFO_SECTION, VERSION_INFO_ONLY_VT5, (long)0 );


	fill_list();	
}
