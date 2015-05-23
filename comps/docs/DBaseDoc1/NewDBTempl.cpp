// CNewDBTempl.cpp : implementation file
//

#include "stdafx.h"
#include "DBaseDoc.h"
#include "NewDBTempl.h"
#include "constant.h"


CString get_mdb_path( const char* psz_dbd )
{
	char sz_buf[2*MAX_PATH];
	strcpy( sz_buf, psz_dbd );
	strlwr( sz_buf );

	char* psz_find = strstr( sz_buf, ".dbd" );
	if( !psz_find )
		return "";

	*psz_find = 0;
	strcat( sz_buf, ".mdb" );
	
	return sz_buf;
}

CString get_mdb_save_name( const char* psz_mdb )
{
	if (GetValueInt(GetAppUnknown(), "\\Database", "SaveRelativePath", FALSE))
	{
		TCHAR szPath[_MAX_PATH], szFName[_MAX_FNAME], szExt[_MAX_EXT];
		_tsplitpath(psz_mdb, NULL, NULL, szFName, szExt);
		_tmakepath(szPath, NULL, NULL, szFName, szExt);
		return szPath;
	}
	return psz_mdb;
}

CString get_dbd_path( const char* psz_dbd )
{
	char sz_buf[2*MAX_PATH];
	strcpy( sz_buf, psz_dbd );
	strlwr( sz_buf );

	if( !strchr( sz_buf, ':' ) || !strchr( sz_buf, '\\' ) || !strstr( sz_buf, ".dbd" ) )
		return "";
	
	return psz_dbd;
}

/////////////////////////////////////////////////////////////////////////////
//
// CNewDBTemplAction
//
/////////////////////////////////////////////////////////////////////////////
class CNewDBTemplAction : public CActionBase
{
	DECLARE_DYNCREATE(CNewDBTemplAction)
	GUARD_DECLARE_OLECREATE(CNewDBTemplAction)

public:
	CNewDBTemplAction();
	virtual ~CNewDBTemplAction();

public:
	virtual bool	Invoke();
	virtual bool	IsAvaible();
	bool			copy_files( CString str_src, CString str_target );
};

IMPLEMENT_DYNCREATE(CNewDBTemplAction, CCmdTargetEx);

// {EE5FEA05-9A2F-4f6f-8059-02135A8D1516}
GUARD_IMPLEMENT_OLECREATE(CNewDBTemplAction, "DBaseDoc.NewDBTempl", 
0xee5fea05, 0x9a2f, 0x4f6f, 0x80, 0x59, 0x2, 0x13, 0x5a, 0x8d, 0x15, 0x16);

// {F9DEC7DD-BB8F-4d8b-A64D-249BAD2E00F3}
static const GUID guidNewDBTempl = 
{ 0xf9dec7dd, 0xbb8f, 0x4d8b, { 0xa6, 0x4d, 0x24, 0x9b, 0xad, 0x2e, 0x0, 0xf3 } };

ACTION_INFO_FULL(CNewDBTemplAction, IDS_DB_FROM_TEMPLATE, -1, -1, guidNewDBTempl);
ACTION_TARGET(CNewDBTemplAction, "anydoc" );

/////////////////////////////////////////////////////////////////////////////
CNewDBTemplAction::CNewDBTemplAction(){}

/////////////////////////////////////////////////////////////////////////////
CNewDBTemplAction::~CNewDBTemplAction(){}

/////////////////////////////////////////////////////////////////////////////
bool CNewDBTemplAction::Invoke()
{
	CWaitCursor wait;

	CNewDBTempl dlg;

	dlg.m_ptr_document = m_punkTarget;

	if( dlg.DoModal() != IDOK )
		return false;
	//get src file name & save
	CString str_src = "";
	IDocumentSitePtr ptr_ds( m_punkTarget );
	BSTR bstr_name = 0;
	ptr_ds->GetPathName( &bstr_name );
	char sz_buf[MAX_PATH];	sz_buf[0] = 0;
	if( bstr_name )
	{
		str_src = bstr_name;
		::SysFreeString( bstr_name );	bstr_name = 0;
		ptr_ds->SaveDocument( _bstr_t( str_src ) );
	}

	CString str_target = dlg.m_dbd_path;
	//copy file name
	if( !::CopyFile( str_src, str_target, TRUE ) )
	{
		CString str_error = GetWin32ErrorDescr( ::GetLastError() );
		AfxMessageBox( str_error, MB_ICONERROR );
		return false;
	}

	//set mdb file name
	CString str_mdb_target = get_mdb_path( str_target );
	CString str_mdb_save = get_mdb_save_name( str_mdb_target );
	IFileDataObjectPtr	ptr_new_db( "Data.NamedData", 0, CLSCTX_INPROC_SERVER );
	if( ptr_new_db )
	{
		if( S_OK == ptr_new_db->LoadFile( _bstr_t( str_target ) ) )
		{
			INamedDataPtr ptr_nd( ptr_new_db );
			if( ptr_nd )
			{
				CString str_conn = DB_DEFAULT_CONNECTION_STRING;
				str_conn += ";";
				str_conn += DB_DATASOURCE;
				str_conn += str_mdb_target;				
				::SetValue( ptr_nd, SECTION_SETTINGS, CONNECTION_STRING, str_conn );
				
			}
			ptr_new_db->SaveFile( _bstr_t( str_target ) );
		}
	}

	CString str_mdb_src = "";
	IDBConnectionPtr ptr_db_conn( m_punkTarget );
	if( ptr_db_conn )
	{
		BSTR bstr = 0;
		ptr_db_conn->GetMDBFileName( &bstr );
		if( bstr )
		{
			str_mdb_src = bstr;
			::SysFreeString( bstr );	bstr = 0;
		}
	}

	copy_files( str_mdb_src, str_mdb_target );

	if( dlg.m_bopen )
		ExecuteAction( "FileOpen", CString( "\"" ) + str_target + CString( "\"" ) );

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CNewDBTemplAction::IsAvaible()
{
	IDBaseDocumentPtr	ptr_db( m_punkTarget );
	if( ptr_db == 0 )
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CNewDBTemplAction::copy_files( CString str_src, CString str_target )
{
	if( !str_src.GetLength() || !str_target.GetLength() )
		return false;

	//copy file name
	if( !::CopyFile( str_src, str_target, TRUE ) )
	{
		CString str_error = GetWin32ErrorDescr( ::GetLastError() );
		AfxMessageBox( str_error, MB_ICONERROR );
		return false;
	}

	//empty table
	try
	{
		ADO::_ConnectionPtr		conn_ptr;
		ADOX::_CatalogPtr		catalog_ptr;

		if( !SUCCEEDED( conn_ptr.CreateInstance( __uuidof(ADO::Connection) ) ) )
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			AfxMessageBox( IDS_WARNING_CONNECTION_OBJECT_FAILED, MB_ICONSTOP | MB_OK );
			return false;
		}
		if( catalog_ptr == 0 )
		{
			if( S_OK != catalog_ptr.CreateInstance(__uuidof (ADOX::Catalog) ) )
			{
				AFX_MANAGE_STATE(AfxGetStaticModuleState());
				AfxMessageBox( IDS_WARNING_CATALOG_OBJECT_FAILED, MB_ICONSTOP | MB_OK );		 
				return false;
			}					
		}
								    
		CString strConnectionString = DB_DEFAULT_CONNECTION_STRING;
		strConnectionString += ";";
		strConnectionString += DB_DATASOURCE;
		strConnectionString += str_target;	

		_bstr_t ConnectionString( strConnectionString );
		_bstr_t UserID("Admin");
		_bstr_t Password;

		conn_ptr->Open( ConnectionString, UserID, Password, 0 );

		catalog_ptr->PutActiveConnection( _variant_t( (IDispatch*)conn_ptr ) );

		ADOX::TablesPtr ptrTables = catalog_ptr->Tables;
		if( ptrTables == 0 )
			return false;

		int nTablesCount = ptrTables->Count;

		CString str_table;

		for( long i=0;i<nTablesCount;i++ )
		{
			ADOX::_TablePtr ptrTable = ptrTables->GetItem( i );
			if( ptrTable == 0 )
				continue;
			
			CString strType = (LPCSTR)ptrTable->Type;
			strType.MakeLower();
			if( strType != "table" )
				continue;			

			str_table = (LPCSTR)ptrTable->Name;

			CString str_sql = "delete * from ";
			str_sql += str_table;

			ptrTable = 0;

			conn_ptr->Execute( _bstr_t( (LPCSTR)str_sql ), NULL, ADO::adCmdText );
		}
		
	}
	catch (_com_error &e)
	{
		dump_com_error( e );
		return false;
	}


	//compact DB
	CompactMDB( str_target );


	return true;
}




// CNewDBTempl dialog
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CNewDBTempl, CDialog)
CNewDBTempl::CNewDBTempl(CWnd* pParent /*=NULL*/)
	: CDialog(CNewDBTempl::IDD, pParent)	
{
	m_dbd_path	= "";
	m_bopen		= false;
}

/////////////////////////////////////////////////////////////////////////////
CNewDBTempl::~CNewDBTempl()
{
}

/////////////////////////////////////////////////////////////////////////////
void CNewDBTempl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNewDBTempl, CDialog)
	ON_BN_CLICKED(IDC_BROWSE, OnBnClickedBrowse)
END_MESSAGE_MAP()


// CNewDBTempl message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CNewDBTempl::OnInitDialog()
{
	__super::OnInitDialog();
	HICON hicon = LoadIcon( AfxGetInstanceHandle( ), "IDI_ICON_LOAD" );
	if( hicon )
	{
		HWND hwnd = ::GetDlgItem( GetSafeHwnd(), IDC_BROWSE );
		if( hwnd )
			::SendMessage( hwnd, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hicon );
	}

	//set default path
	IDocumentSitePtr ptr_ds( m_ptr_document );
	BSTR bstr_name = 0;
	ptr_ds->GetPathName( &bstr_name );
	char sz_buf[MAX_PATH];	sz_buf[0] = 0;
	if( bstr_name )
	{
		strcpy( sz_buf, (char*)_bstr_t( bstr_name ) );
		::SysFreeString( bstr_name );	bstr_name = 0;
		//kill extension
		char* psz_ext = strrchr( sz_buf, '.' );		
		if( psz_ext )
			*psz_ext = 0;
		//parse dir & file name
		char* psz_dir = sz_buf;
		char* psz_file = strrchr( sz_buf, '\\' );		
		if( psz_file )
		{
			*psz_file = 0;	psz_file++;
		}

		if( psz_dir && psz_file )
		{
			//detect num start from end
			int nlen = strlen( psz_file );
			char* psz_digit = psz_file + nlen;
			for( int i=nlen-1;i>=0;i-- )
			{
				if( isdigit_ru( *(psz_file + i) ) )
					psz_digit--;
				else
					break;
			}

			int nbase = 1;
			if( psz_digit && strlen( psz_digit ) )
				nbase = atoi(psz_digit);

			char sz_tmp[10];
			sprintf( sz_tmp, "%d", nbase );

			//construct new path
			CString str_path = psz_dir;
			str_path += "\\";
			str_path += psz_file;
			str_path += sz_tmp;
			str_path += ".dbd";

			::SetDlgItemText( GetSafeHwnd(), IDC_NEW_PATH, str_path );		
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CNewDBTempl::OnOK()
{
	char sz_path[255];	sz_path[0] = 0;
	::GetDlgItemText( GetSafeHwnd(), IDC_NEW_PATH, sz_path, sizeof(sz_path)-1 );
	m_dbd_path = sz_path;
	
	m_bopen = ( 1L == ::IsDlgButtonChecked( GetSafeHwnd(), IDC_OPEN ) );

	if( !strlen( sz_path ) || !strchr( sz_path, '.' ) || !strchr( sz_path, '\\' ) )
	{
		::SetFocus( ::GetDlgItem( GetSafeHwnd(), IDC_NEW_PATH ) );
		return;
	}

	CString str_dbd = get_dbd_path( m_dbd_path );
	CString str_mdb = get_mdb_path( m_dbd_path );
	if( !str_dbd.GetLength() || !str_dbd.GetLength() )
	{
		::SetFocus( ::GetDlgItem( GetSafeHwnd(), IDC_NEW_PATH ) );
		return;
	}

	{
		CFileFind ff;
		if( ff.FindFile( str_dbd ) )
		{
			CString str;
			str.Format( (UINT)IDS_FILE_EXIST, (const char*)str_dbd );
			AfxMessageBox( str, MB_ICONERROR );
			::SetFocus( ::GetDlgItem( GetSafeHwnd(), IDC_NEW_PATH ) );
			return;
		}
	}

	{
		CFileFind ff;
		if( ff.FindFile( str_mdb ) )
		{
			CString str;
			str.Format( IDS_FILE_EXIST, (const char*)str_mdb );
			AfxMessageBox( str, MB_ICONERROR );
			::SetFocus( ::GetDlgItem( GetSafeHwnd(), IDC_NEW_PATH ) );
			return;
		}
	}

	__super::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
void CNewDBTempl::OnBnClickedBrowse()
{
	char sz_path[255];	sz_path[0] = 0;
	::GetDlgItemText( GetSafeHwnd(), IDC_NEW_PATH, sz_path, sizeof(sz_path)-1 );

	CFileDialog dlg(FALSE, 0, 0, OFN_HIDEREADONLY, 0, 0, _FILE_OPEN_SIZE_ );
	strcpy( dlg.m_ofn.lpstrFile, sz_path );
	char sz_buf[255];	::ZeroMemory( sz_buf, sizeof(sz_buf) );
	::LoadString( AfxGetApp()->m_hInstance, IDS_DBASE_FILE_FILTER, sz_buf, sizeof(sz_buf) );
	
	for( int i=0;i<sizeof(sz_buf); i++ )
		if( sz_buf[i] == '\n' )	sz_buf[i] = 0;

	dlg.m_ofn.lpstrFilter = sz_buf;
	if( dlg.DoModal() == IDOK )
	{
		CString str_path = dlg.GetPathName();
		if( !strrchr( str_path, '.' ) )
			str_path += ".dbd";

		::SetDlgItemText( GetSafeHwnd(), IDC_NEW_PATH, str_path );		
	}
}
