// ConvDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DBaseDoc.h"
#include "ConvDlg.h"
#include "constant.h"
#include "DBStructDlg.h"

#include "carioint.h"
#include "misc_utils.h"


/////////////////////////////////////////////////////////////////////////////
//
// CDBaseActionConvert
//
/////////////////////////////////////////////////////////////////////////////
class CDBaseActionConvertVT4 : public CActionBase
{
	DECLARE_DYNCREATE(CDBaseActionConvertVT4)
	GUARD_DECLARE_OLECREATE(CDBaseActionConvertVT4)

public:
	CDBaseActionConvertVT4();
	virtual ~CDBaseActionConvertVT4();

public:
	virtual bool Invoke();
};

IMPLEMENT_DYNCREATE(CDBaseActionConvertVT4, CCmdTargetEx);

// {3D986393-0A94-4d44-B415-14D73074FD6F}
GUARD_IMPLEMENT_OLECREATE(CDBaseActionConvertVT4, "DBaseDoc.ConvertVT4", 
0x3d986393, 0xa94, 0x4d44, 0xb4, 0x15, 0x14, 0xd7, 0x30, 0x74, 0xfd, 0x6f);

// {5C6144C6-6F70-4c1d-9DCB-C441FAB36D5F}
static const GUID guidConvertVT4 = 
{ 0x5c6144c6, 0x6f70, 0x4c1d, { 0x9d, 0xcb, 0xc4, 0x41, 0xfa, 0xb3, 0x6d, 0x5f } };

ACTION_INFO_FULL(CDBaseActionConvertVT4, IDS_CONVERT_VT4, -1, -1, guidConvertVT4);
ACTION_TARGET(CDBaseActionConvertVT4, szTargetApplication );

/////////////////////////////////////////////////////////////////////////////
CDBaseActionConvertVT4::CDBaseActionConvertVT4()
{}

/////////////////////////////////////////////////////////////////////////////
CDBaseActionConvertVT4::~CDBaseActionConvertVT4()
{}

bool CDBaseActionConvertVT4::Invoke()
{
	CConvDlg dlg;
	dlg.DoModal();
	return true;
}

/////////////////////////////////////////////////////////////////////////////
//
// CConvDlg dialog
//
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CConvDlg, CDialog)
CConvDlg::CConvDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConvDlg::IDD, pParent)
{
}

/////////////////////////////////////////////////////////////////////////////
CConvDlg::~CConvDlg()
{
	clear_src_ar_fi();
}

/////////////////////////////////////////////////////////////////////////////
void CConvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConvDlg, CDialog)
	ON_BN_CLICKED(IDC_ANALIZE, OnBnClickedAnalize)
	ON_BN_CLICKED(IDC_START, OnBnClickedStart)
	ON_BN_CLICKED(IDC_VT4_BROWSE, OnBnClickedVt4Browse)
	ON_BN_CLICKED(IDC_VT5_BROWSE, OnBnClickedVt5Browse)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDHELP, OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CConvDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	HICON hicon = LoadIcon( AfxGetInstanceHandle( ), "IDI_ICON_LOAD" );
	if( hicon )
	{
		HWND hwnd = ::GetDlgItem( GetSafeHwnd(), IDC_VT4_BROWSE );
		if( hwnd )
			::SendMessage( hwnd, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hicon );
		
		hwnd = ::GetDlgItem( GetSafeHwnd(), IDC_VT5_BROWSE );
		if( hwnd )
			::SendMessage( hwnd, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hicon );

		::DestroyIcon( hicon );	hicon = 0;
	}

	

	HWND hwnd_list = ::GetDlgItem( GetSafeHwnd(), IDC_LIST );
	if( hwnd_list )
		ListView_SetExtendedListViewStyle( hwnd_list, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP );


	//insert columns to list
	{
		CRect rc;
		::ZeroMemory( &rc, sizeof(RECT) );
		::GetClientRect( hwnd_list, &rc );

		CString str_caption;
		str_caption.LoadString( IDS_CAPTION_COL_USER );
		CString str_type;
		str_type.LoadString( IDS_CAPTION_COL_FIELD_TYPE );

		LVCOLUMN column;
		memset( &column, 0, sizeof(LVCOLUMN) );
		column.mask		= LVCF_TEXT | LVCF_WIDTH;
		column.pszText	= (char*)(const char*)str_caption;		
		column.cx		= rc.Width() / 2;
		ListView_InsertColumn( hwnd_list, 0, &column );	

		column.cx		= rc.Width() / 2 - 20;		
		column.pszText	= (char*)(const char*)str_type;	
		ListView_InsertColumn( hwnd_list, 1, &column );
	}

	//::SetDlgItemText( GetSafeHwnd(), IDC_VT4_PATH, 
	//	::GetValueString( ::GetAppUnknown(), "\\DatabaseConvertor", "SourcePath", "" ) );

	//::SetDlgItemText( GetSafeHwnd(), IDC_VT5_PATH, 
	//	::GetValueString( ::GetAppUnknown(), "\\DatabaseConvertor", "TargetPath", "" ) );

	//analize_target_is_empty();

	set_ui_enable();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CConvDlg::OnDestroy()
{
	::SetValue( ::GetAppUnknown(), "\\DatabaseConvertor", "SourcePath", get_src_db() );
	::SetValue( ::GetAppUnknown(), "\\DatabaseConvertor", "TargetPath", get_target_db() );
}

/////////////////////////////////////////////////////////////////////////////
void CConvDlg::clear_src_ar_fi()
{
	for( int i=0; i<m_arr_src_fi.GetSize(); i++ )
		delete m_arr_src_fi[i];

	m_arr_src_fi.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
CString CConvDlg::get_src_db()
{
	CWnd* pwnd = GetDlgItem( IDC_VT4_PATH );
	if( !pwnd )	return "";

	CString str;
	pwnd->GetWindowText( str );
	return str;	
}

/////////////////////////////////////////////////////////////////////////////
CString CConvDlg::get_target_db()
{
	CWnd* pwnd = GetDlgItem( IDC_VT5_PATH );
	if( !pwnd )	return "";

	CString str;
	pwnd->GetWindowText( str );
	return str;	
}

/////////////////////////////////////////////////////////////////////////////
void CConvDlg::set_ui_enable()
{
	CString str_src		= get_src_db();
	CString str_target	= get_target_db();

	HWND hwnd_analize	= ::GetDlgItem( GetSafeHwnd(), IDC_ANALIZE );
	HWND hwnd_start		= ::GetDlgItem( GetSafeHwnd(), IDC_START );

	::EnableWindow( hwnd_analize, str_src.GetLength() );
	::EnableWindow( hwnd_start, str_target.GetLength() && m_arr_src_fi.GetSize() );
}

/////////////////////////////////////////////////////////////////////////////
bool CConvDlg::open_src_table()
{
	try
	{
		CString src_db = get_src_db();
		if( !src_db.GetLength() )
			return false;

		m_conn_ptr		= 0;
		m_catalog_ptr	= 0;
		m_ptr_rs		= 0;

		clear_src_ar_fi();

		if( !SUCCEEDED( m_conn_ptr.CreateInstance( __uuidof(ADO::Connection) ) ) )
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			AfxMessageBox( IDS_WARNING_CONNECTION_OBJECT_FAILED, MB_ICONSTOP | MB_OK );
			return false;
		}
		if( m_catalog_ptr == 0 )
		{
			if( S_OK != m_catalog_ptr.CreateInstance(__uuidof (ADOX::Catalog) ) )
			{
				AFX_MANAGE_STATE(AfxGetStaticModuleState());
				AfxMessageBox( IDS_WARNING_CATALOG_OBJECT_FAILED, MB_ICONSTOP | MB_OK );		 
				return false;
			}					
		}
								    
		CString strConnectionString = DB_DEFAULT_CONNECTION_STRING;
		strConnectionString += ";";
		strConnectionString += DB_DATASOURCE;
		strConnectionString += src_db;	

		_bstr_t ConnectionString( strConnectionString );
		_bstr_t UserID("Admin");
		_bstr_t Password;

		m_conn_ptr->Open( ConnectionString, UserID, Password, 0 );

		m_catalog_ptr->PutActiveConnection( _variant_t( (IDispatch*)m_conn_ptr ) );

		ADOX::TablesPtr ptrTables = m_catalog_ptr->Tables;
		if( ptrTables == 0 )
			return false;

		int nTablesCount = ptrTables->Count;

		CString str_table;
		clear_src_ar_fi();		

		for( long i=0;i<nTablesCount;i++ )
		{
			ADOX::_TablePtr ptrTable = ptrTables->GetItem( i );
			if( ptrTable == 0 )
				continue;
			
			CString strType = (LPCSTR)ptrTable->Type;
			strType.MakeLower();
			if( strType != "table" )
				continue;			

			ADOX::ColumnsPtr ptrColumns = ptrTable->Columns;
			if( ptrColumns == 0 )
				continue;			

			long nColCount = ptrColumns->Count;
			if( !nColCount )
				continue;

			str_table = (LPCSTR)ptrTable->Name;

			for( long j=0;j<nColCount;j++ )
			{				
				ADOX::_ColumnPtr ptrColumn = ptrColumns->GetItem( j );
				
				if( ptrColumn == 0 )
					continue;

				CString str_field_name = (LPCSTR)ptrColumn->Name;
				if( !str_field_name.GetLength() )
					continue;

				if( str_field_name[0] == '%' || str_field_name[0] == '^' )
					continue;

				_CFieldInfo* pfi = new _CFieldInfo;				
				
				pfi->m_strUserName			= str_field_name;
				ADOX::DataTypeEnum	dt		= ptrColumn->Type;				

				if( dt == ADOX::adDouble )
				{
					pfi->m_FieldType	= ftDigit;
				}				
				else if( dt == ADOX::adBSTR || dt == ADOX::adVarChar || dt == ADOX::adLongVarWChar || 
					dt == ADOX::adWChar || dt == ADOX::adVarWChar || dt == ADOX::adLongVarWChar )
				{
					pfi->m_FieldType	= ftString;
				}
				else if( dt == ADOX::adDate || dt == ADOX::adDBDate || dt == ADOX::adDBTime || dt == ADOX::adDBTimeStamp )
				{
					pfi->m_FieldType	= ftDateTime;
				}
				else if( dt == ADOX::adLongVarBinary || dt == ADOX::adVarBinary )
				{
					pfi->m_FieldType	= ftVTObject;
				}
				else
				{
					delete pfi;
					continue;
				}

				m_arr_src_fi.Add( pfi );
				
			}
			break;//need only one table
		}		

		if( !m_arr_src_fi.GetSize() )
			return false;

		CString str_sql;
		str_sql.Format( "select * from [%s]", (const char*)str_table );

		m_ptr_rs.CreateInstance( __uuidof(ADO::Recordset) );
		m_ptr_rs->CursorLocation = ADO::adUseServer;

		m_ptr_rs->Open( _bstr_t( str_sql ), _variant_t( (IDispatch*)m_conn_ptr ), 
						ADO::adOpenKeyset, ADO::adLockOptimistic, ADO::adCmdText );


		return true;
	}
	catch (_com_error &e)
	{
		dump_com_error( e );
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////
bool CConvDlg::analize_src_table()
{
	try
	{
		if( m_ptr_rs == 0 )	return false;

		m_ptr_rs->MoveFirst();
		long lcount = m_ptr_rs->RecordCount;

		HWND hwnd = ::GetDlgItem( GetSafeHwnd(), IDC_PROGRESS );
		if( hwnd )
			::SendMessage( hwnd, PBM_SETRANGE32, (WPARAM)0, (LPARAM)lcount );

		int nrecord = 0;
		while( VARIANT_FALSE == m_ptr_rs->ADOEOF )
		{  			
			if( is_aborted() )	return false;

			if( hwnd )
			{
				::SendMessage( hwnd, PBM_SETPOS, (WPARAM)nrecord+1, 0 );
				::UpdateWindow( hwnd );
			}

			bool ball_fields_complete = true;
			for( int idx=0; idx<m_arr_src_fi.GetSize(); idx++ )
			{
				_CFieldInfo* pfi = m_arr_src_fi[idx];
				if( pfi->m_FieldType == ftVTObject && pfi->m_strVTObjectType.IsEmpty() )
				{
					ball_fields_complete = false;
					break;
				}
			}

			if( ball_fields_complete )
			{
				::SendMessage( hwnd, PBM_SETPOS, (WPARAM)lcount, 0 );
				::UpdateWindow( hwnd );

				return true;
			}

			nrecord++;
			ADO::FieldsPtr spFields = m_ptr_rs->Fields;
			
			if( spFields )
			{
				int nFieldsCount = spFields->Count;
				
				for( long i=0;i<nFieldsCount;i++ )
				{						
					ADO::FieldPtr fieldPtr = spFields->Item[_variant_t(i)];					
					_bstr_t bstr_field = fieldPtr->Name;

					_CFieldInfo* pfi = 0;
					for( int k=0; k<m_arr_src_fi.GetSize(); k++ )
					{
						_CFieldInfo* pfi_test = m_arr_src_fi[k];
						if( pfi_test->m_strUserName == (char*)bstr_field )
						{
							pfi = pfi_test;
							break;
						}
					}

					if( !pfi )
						continue;
					
					if( pfi->m_FieldType != ftVTObject )
						continue;

					if( pfi->m_strVTObjectType.GetLength() )
						continue;

					_variant_t var;
					if( S_OK == fieldPtr->get_Value( &var ) )						
					{
						if( fieldPtr->Type == ADO::adLongVarBinary || fieldPtr->Type == ADO::adVarBinary )
						{
							if( var.vt == (VT_ARRAY|VT_UI1) )
							{
								
								int nsize = var.parray->rgsabound[0].cElements;
								BYTE* pSrc = (BYTE*)var.parray->pvData;

								if( is_jpeg( pSrc, nsize ) || is_raw_bmp( pSrc, nsize ) )
									pfi->m_strVTObjectType = szTypeImage;
								else if( -1 != find_vt4_prefix( pSrc, nsize ) )
									pfi->m_strVTObjectType = szTypeObjectList;
							}
						}
					}
				}
			}
			m_ptr_rs->MoveNext();
		}

		return true;
	}
	catch (_com_error &e)
	{
		dump_com_error( e );
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CConvDlg::pack_field_info()
{
	for( int i=m_arr_src_fi.GetSize()-1; i>=0; i-- )
	{
		_CFieldInfo* pfi = m_arr_src_fi[i];
		if( pfi->m_FieldType == ftNotSupported || 
			( pfi->m_FieldType == ftVTObject && pfi->m_strVTObjectType.IsEmpty() ) || 
			pfi->m_strUserName.IsEmpty()
			)
		{
			m_arr_src_fi.RemoveAt( i );
			delete pfi;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
bool CConvDlg::fill_list()
{
	HWND hwnd_list = ::GetDlgItem( GetSafeHwnd(), IDC_LIST );

	if( !hwnd_list )
		return false;

	ListView_DeleteAllItems( hwnd_list );

	for( int i=0; i<m_arr_src_fi.GetSize(); i++ )
	{
		_CFieldInfo* pfi = m_arr_src_fi[i];
		
		LVITEM item;
		::ZeroMemory( &item, sizeof(LVITEM) );
		//check box
		item.mask		= LVIF_TEXT | LVIF_IMAGE;
		item.iSubItem	= 0;
		item.iItem		= i;
		item.pszText	= (char*)(const char*)pfi->m_strUserName;

		ListView_InsertItem( hwnd_list, &item );

		CString str_type = CDBStructGrid::GetFieldTypeAsString( pfi );
		//caption
		item.mask		= LVIF_TEXT;
		item.iSubItem	= 1;		
		item.pszText	= (char*)(const char*)str_type;

		ListView_SetItem( hwnd_list, &item );
	}

	return true;
}

/*
IStream* ps = 0;
if( S_OK == ::CreateStreamOnHGlobal( 0, TRUE, &ps ) )
{
	ULONG uWrite = 0;
	if( S_OK == ps->Write( pSrc, (ULONG)nsize, &uWrite ) )
	{
		LARGE_INTEGER	uiPos = {0};										
		ps->Seek( uiPos, 0, 0 );

		char sz_buf[256];
		sprintf( sz_buf, "f:\\vti%d_%d.dat", nrecord, i );

		CFile file( sz_buf, CFile::modeCreate|CFile::modeWrite );
		file.Write( pSrc, nsize );
		file.Close();

	}

	ps->Release();	ps = 0;
}
*/

/////////////////////////////////////////////////////////////////////////////
bool CConvDlg::is_jpeg( BYTE* pbuf, int nsize )
{
	if( !pbuf ||  nsize < 10 )
		return false;

	char* psz_buf = (char*)pbuf;
	if( psz_buf[6] == 'J' && psz_buf[7] == 'F' &&
		psz_buf[8] == 'I' && psz_buf[9] == 'F' )
	{
		return true;
	}
	
	return false;
}


bool CConvDlg::is_raw_bmp( BYTE* pbuf, int nsize )
{	
	if( nsize <= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) )
		return false;

	return ( pbuf[0] == 40 );//'B' && pbuf[1] == 'M' );
}
/////////////////////////////////////////////////////////////////////////////
long CConvDlg::find_vt4_prefix( BYTE* pbuf, int nsize )
{
	if( !pbuf )
		return -1;

	char* psz_buf = (char*)pbuf;

	const char*	psz_prefix = "VT4File";
	int prefix_len = strlen(psz_prefix);
	for( int i = 0; i < nsize - prefix_len; i++ )
	{
		if( !strncmp( psz_buf, psz_prefix, prefix_len )	)
			return i-1;

		psz_buf++;
	}

	return -1;
}

/////////////////////////////////////////////////////////////////////////////
bool CConvDlg::is_aborted()
{
	MSG	msg;

	while( ::PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
	{
		if( msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE )
		{
			AfxMessageBox( IDS_WAS_ABORT, MB_ICONERROR );
			return true;
		}

		if( msg.message == WM_PAINT || msg.message == WM_NCPAINT )
		{
			::TranslateMessage( &msg );
			::DispatchMessage( &msg );
		}
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////////
IUnknownPtr CConvDlg::create_jpeg_filter()
{
	CLSID clsid = {0};
	//Create Jpeg File Filter
	if( S_OK != ::CLSIDFromProgID( _bstr_t("FileFilters2.JPGFilter"), &clsid ) )
		return 0;

	IUnknown* punk = 0;
	if( S_OK != ::CoCreateInstance( clsid, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&punk ) )
		return 0;

	IUnknownPtr ptr_jpeg = punk;
	punk->Release();	punk = 0;

	return ptr_jpeg;
}


/////////////////////////////////////////////////////////////////////////////
IUnknownPtr CConvDlg::create_vti_filter()
{
	CLSID clsid = {0};
	//Create VTI File Filter
	if( S_OK != ::CLSIDFromProgID( _bstr_t("FileFilters.VTIFileFilter"), &clsid ) )
		return false;

	IUnknown* punk = 0;
	if( S_OK != ::CoCreateInstance( clsid, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&punk ) )
		return false;

	IUnknownPtr ptr_vti = punk;
	punk->Release();	punk = 0;

	return ptr_vti;
}

/////////////////////////////////////////////////////////////////////////////
IUnknownPtr CConvDlg::create_bmp_filter()
{
	CLSID clsid = {0};
	//Create VTI File Filter
	if( S_OK != ::CLSIDFromProgID( _bstr_t("ImageDoc.BMPFileFilter"), &clsid ) )
		return false;

	IUnknown* punk = 0;
	if( S_OK != ::CoCreateInstance( clsid, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&punk ) )
		return false;

	IUnknownPtr ptr_vti = punk;
	punk->Release();	punk = 0;

	return ptr_vti;
}

/////////////////////////////////////////////////////////////////////////////
CString CConvDlg::get_jpeg_tmp_path()
{
	char sz_file[MAX_PATH];
	::GetModuleFileName( 0, sz_file, sizeof(sz_file) );
	char* psz_token = strrchr( sz_file, '\\' );
	*(psz_token++) = 0;
	strcat( sz_file, "\\vt4_2_vt5.jpg" );

	return sz_file;
}

/////////////////////////////////////////////////////////////////////////////
CString CConvDlg::get_vti_tmp_path()
{
	char sz_file[MAX_PATH];
	::GetModuleFileName( 0, sz_file, sizeof(sz_file) );
	char* psz_token = strrchr( sz_file, '\\' );
	*(psz_token++) = 0;
	strcat( sz_file, "\\vt4_2_vt5.vti" );

	return sz_file;
}

/////////////////////////////////////////////////////////////////////////////
void CConvDlg::analize_target_is_empty()
{
	CString str_src = get_src_db();
	CString str_target = get_target_db();

	if( str_src.GetLength() /*&& !str_target.GetLength() */ )
	{
		char* psz_buf = new char[str_src.GetLength() + 10];
		if( psz_buf )
		{
			strcpy( psz_buf, str_src );
			char* psz_token = strrchr( psz_buf, '.' );
			if( psz_token )
			{
				*psz_token = 0;
				strcat( psz_buf, "_exp5.mdb" );
				::SetDlgItemText( GetSafeHwnd(), IDC_VT5_PATH, psz_buf );		
			}

			delete psz_buf;	psz_buf = 0;
		}
	}
}

bool g_inside_converter = false;
/////////////////////////////////////////////////////////////////////////////
bool CConvDlg::start_convert()
{
	try
	{
		
		bool bkaryo_build		= ( 1L == ::GetValueInt( ::GetAppUnknown(), "\\DatabaseConvertor", "RunBuildKaryo", 1L ) );
		bool bcalc_measresult	= ( 1L == ::GetValueInt( ::GetAppUnknown(), "\\DatabaseConvertor", "RunCalcMeasResult", 1L ) );

		CString str_error;

		if( is_aborted() )	return false;

		HWND hwnd_progress = ::GetDlgItem( GetSafeHwnd(), IDC_PROGRESS );
		::SendMessage( hwnd_progress, PBM_SETPOS, (WPARAM)0, 0 );
		::UpdateWindow( hwnd_progress );
		
		const char* psz_table = "tbl";
		
		pack_field_info();

		if( m_ptr_rs == 0 )
			return false;

		//test if file exist
		CString str_mdb_file = get_target_db();
		if( !str_mdb_file.GetLength() )
			return false;

		CString str_dbd_file = str_mdb_file;
		str_dbd_file.MakeLower();
		int nidx = str_dbd_file.Find( ".mdb" );
		if( nidx == -1 )
			return false;

		{
			CString str_file1 = get_src_db();
			CString str_file2 = get_target_db();

			str_file1.MakeLower();
			str_file2.MakeLower();

			if( str_file1 == str_file2 )
			{
				AfxMessageBox( IDS_EXPORT_FILE_EQUAL, MB_ICONERROR );
				return false;
			}
		}

		

		str_dbd_file.SetAt( nidx, '.' );
		str_dbd_file.SetAt( nidx+1, 'd' );
		str_dbd_file.SetAt( nidx+2, 'b' );
		str_dbd_file.SetAt( nidx+3, 'd' );

		{
			CFileFind ff;
			if( ff.FindFile( str_mdb_file ) )
			{
				CString strMessage;
				strMessage.Format( IDS_WARNING_FILE_EXIST, str_mdb_file );
				if( AfxMessageBox( strMessage, MB_YESNO ) != IDYES )
					return false;

				::DeleteFile( str_mdb_file );
			}

		}

		{
			CFileFind ff;
			if( ff.FindFile( str_dbd_file ) )
			{
				CString strMessage;
				strMessage.Format( IDS_WARNING_FILE_EXIST, str_dbd_file );
				if( AfxMessageBox( strMessage, MB_YESNO ) != IDYES )
					return false;
			}
		}

		if( is_aborted() )	return false;

		ADOX::_CatalogPtr	ptrCat;
		ptrCat.CreateInstance(__uuidof (ADOX::Catalog) );
		CString strConn("Provider=Microsoft.JET.OLEDB.4.0;Data source = ");
		strConn += str_mdb_file;			
		ptrCat->Create( _bstr_t( (LPCSTR)strConn ) );

		if( is_aborted() )	return false;
		//create new document
		g_inside_converter = true;

		CString strArg;
		strArg.Format( "\"%s\"", (LPCTSTR)"Database" );
		::ExecuteAction( "FileNew", strArg, 0 );

		g_inside_converter = false;


		//Get Active Database
		IApplicationPtr ptr_app( ::GetAppUnknown() );
		if( ptr_app == 0 )
			return false;

		IDBConnectionPtr	ptrDBConn;
		IDBaseStructPtr		ptrDBStruct;
		IDBaseDocumentPtr	ptrDBaseDoc;
		{
			IUnknown* punk = 0;
			ptr_app->GetActiveDocument( &punk );
			if( punk )
			{
				ptrDBConn		= punk;
				ptrDBStruct		= punk;
				ptrDBaseDoc		= punk;
				punk->Release();	punk = 0;
			}
		}

		if( ptrDBConn == 0 || ptrDBStruct == 0 || ptrDBaseDoc == 0 ) 
			return false;
		
		if( is_aborted() )	return false;

		if( S_OK != ptrDBConn->LoadAccessDBase( _bstr_t( (LPCSTR)str_mdb_file ) ) )
			return false;

		if( is_aborted() )	return false;

		if( S_OK != ptrDBStruct->AddTable( _bstr_t( (LPCSTR)psz_table ) ) != S_OK )
			return false;

		if( is_aborted() )	return false;

		//generate field name
		int ndate_count		= 0;
		int ndig_count		= 0;
		int nstring_count	= 0;
		int nimage_count	= 0;
		int nol_count		= 0;

		for( int i=0; i<m_arr_src_fi.GetSize(); i++ )
		{
			char sz_field[256];	sz_field[0] = 0;
			_CFieldInfo* pfi = m_arr_src_fi[i];
			if( pfi->m_FieldType == ftDateTime )
				sprintf( sz_field, "DateTime%d", ++ndate_count );
			else if( pfi->m_FieldType == ftDigit )
				sprintf( sz_field, "Number%d", ++ndig_count );
			else if( pfi->m_FieldType == ftString )
				sprintf( sz_field, "String%d", ++nstring_count );
			else if( pfi->m_FieldType == ftVTObject )
			{
				if( pfi->m_strVTObjectType == szTypeImage )
					sprintf( sz_field, "Image%d", ++nimage_count );
				else if( pfi->m_strVTObjectType == szTypeObjectList )
					sprintf( sz_field, "ObjList%d", ++nol_count );
			}

			pfi->m_strFieldName = sz_field;			

			pfi->m_strFieldName.MakeLower();
		}

		//add primary key
		{
			_CFieldInfo* pfi = new _CFieldInfo;
			pfi->m_FieldType = ftPrimaryKey;
			pfi->m_strFieldName = "id_tbl";
			pfi->m_strUserName	= "id_tbl";
			m_arr_src_fi.Add( pfi );
		}


		//add fields to database
		for( i=0; i<m_arr_src_fi.GetSize(); i++ )
		{
			_CFieldInfo* pfi = m_arr_src_fi[i];
			if( S_OK != ptrDBStruct->AddField(	_bstr_t( (LPCSTR)psz_table ),
										_bstr_t( (LPCSTR)pfi->m_strFieldName ), 
										pfi->m_FieldType, 
										0, 0 ) )
				return false;

			ptrDBStruct->SetFieldInfo( 
								_bstr_t( (LPCSTR)psz_table ),
								_bstr_t( (LPCSTR)pfi->m_strFieldName ), 
								_bstr_t( (LPCSTR)pfi->m_strUserName ),
								FALSE,
								FALSE,
								_bstr_t( (LPCSTR)"" ),
								_bstr_t( (LPCSTR)pfi->m_strVTObjectType ) );

		}

		IUnknown* punkQuery = ::CreateTypedObject( szTypeQueryObject );
		if( !punkQuery )
			return false;

		ISelectQueryPtr ptr_sq = punkQuery;
		IQueryObjectPtr ptr_qo = punkQuery;
		punkQuery->Release();	punkQuery = 0;

		if( ptr_sq == 0 || ptr_qo == 0 )
			return false;

		//set to document
		INamedDataPtr ptr_nd( ptrDBStruct );
		if( ptr_nd == 0 )
			return false;
		
		ptr_nd->SetValue( 0, _variant_t( (IUnknown*)ptr_sq ) );

		ptr_qo->SetBaseTable( _bstr_t( psz_table ) );
		ptr_qo->SetSimpleQuery( true );

		
		//set caption & thumbnail
		{
			bool bset_caption	= false;
			bool bset_thumbnail	= false;

			for( i=0; i<m_arr_src_fi.GetSize(); i++ )
			{
				_CFieldInfo* pfi = m_arr_src_fi[i];
				if( pfi->m_FieldType == ftString )
				{
					bset_caption = true;
					ptr_sq->SetTumbnailCaptionLocation( _bstr_t(psz_table), bstr_t(pfi->m_strFieldName) ); 
				}
				else if( pfi->m_FieldType == ftVTObject && pfi->m_strVTObjectType == szTypeImage )
				{
					bset_thumbnail = true;
					ptr_sq->SetTumbnailLocation( _bstr_t(psz_table), bstr_t(pfi->m_strFieldName) ); 
				}

				if( bset_caption && bset_thumbnail )
					break;
			}
		}
		

		if( is_aborted() )	return false;

		ptrDBaseDoc->SetActiveQuery( ptr_sq );

		if( S_OK != ptr_sq->Open() )
			return false;
		

		if( is_aborted() )	return false;

		//save document file
		IDocumentSitePtr ptr_ds( ptrDBStruct );
		IDataContext2Ptr  ptr_active_dc;
		if( ptr_ds )
		{
			if( is_aborted() )	return false;
			ptr_ds->SaveDocument( _bstr_t( (LPCSTR)str_dbd_file ) );
			if( is_aborted() )	return false;

			//get active context
			IUnknown* punk_view = 0;
			ptr_ds->GetActiveView( &punk_view );
			if( punk_view )
			{
				ptr_active_dc = punk_view;
				punk_view->Release();	punk_view = 0;
			}
		}			

		if( ptr_active_dc )
			ptr_active_dc->SetObjectSelect( ptr_sq, 0 );

		//start copy record
		m_ptr_rs->MoveFirst();
		long lcount = m_ptr_rs->RecordCount;

		HWND hwnd = ::GetDlgItem( GetSafeHwnd(), IDC_PROGRESS );
		if( hwnd )
			::SendMessage( hwnd, PBM_SETRANGE32, (WPARAM)0, (LPARAM)lcount );

		CString str_jpeg	= get_jpeg_tmp_path();
		CString str_ol		= get_vti_tmp_path();



		int nrecord = 0;
		while( VARIANT_FALSE == m_ptr_rs->ADOEOF )
		{  				
			if( is_aborted() )
			{
				if( ptr_active_dc )
					ptr_active_dc->SetObjectSelect( ptr_sq, 1 );
				return false;
			}

			if( S_OK != ptr_sq->Insert() )
				return false;

			if( hwnd )
			{
				::SendMessage( hwnd, PBM_SETPOS, (WPARAM)nrecord+1, 0 );
				::UpdateWindow( hwnd );
			}

			nrecord++;
			ADO::FieldsPtr spFields = m_ptr_rs->Fields;
			
			int nFieldsCount = spFields->Count;
			
			for( long i=0;i<nFieldsCount;i++ )
			{						
				ADO::FieldPtr fieldPtr = spFields->Item[_variant_t(i)];					
				_bstr_t bstr_field = fieldPtr->Name;

				_CFieldInfo* pfi = 0;
				for( int k=0; k<m_arr_src_fi.GetSize(); k++ )
				{
					_CFieldInfo* pfi_test = m_arr_src_fi[k];
					if( pfi_test->m_strUserName == (char*)bstr_field )
					{
						pfi = pfi_test;
						break;
					}
				}

				if( !pfi )
					continue;
				
				_variant_t var;
				if( S_OK != fieldPtr->get_Value( &var ) )						
					continue;

				CString str_path;
				str_path.Format( "%s\\%s.%s", SECTION_DBASEFIELDS, psz_table, pfi->m_strFieldName );

				if( pfi->m_FieldType != ftVTObject )
				{
					ptr_nd->SetValue( _bstr_t(str_path), var );
				}
				else
				{
					if( !( fieldPtr->Type == ADO::adLongVarBinary || fieldPtr->Type == ADO::adVarBinary ) )
						continue;

					if( var.vt != (VT_ARRAY|VT_UI1) )
						continue;

					int nsize = var.parray->rgsabound[0].cElements;
					BYTE* pSrc = (BYTE*)var.parray->pvData;

					if( !nsize || !pSrc )
						continue;

					if( is_aborted() )
					{
						if( ptr_active_dc )
							ptr_active_dc->SetObjectSelect( ptr_sq, 1 );
						return false;
					}


					try
					{
						if( pfi->m_strVTObjectType == szTypeImage )
						{
							bool braw_bmp = is_raw_bmp( pSrc, nsize );
							CFile file( str_jpeg, CFile::modeCreate|CFile::modeWrite );

							if( braw_bmp )
							{
								BITMAPFILEHEADER bfh = {0};
								BITMAPINFOHEADER* pbi = (BITMAPINFOHEADER*)pSrc;
								bfh.bfType = ((int( 'M' )<<8)|int( 'B' ));
								bfh.bfSize = sizeof( bfh )+pbi->biSizeImage;
								bfh.bfOffBits = sizeof( BITMAPFILEHEADER )+sizeof( BITMAPINFOHEADER )+ ((pbi->biBitCount == 24)?0:(1<<pbi->biBitCount<<2));
								file.Write( &bfh, sizeof( bfh ) );
							}

							file.Write( pSrc, nsize );
							file.Close();

							/*
							{
								char sz_file[MAX_PATH];
								strcpy( sz_file, str_jpeg );
								char* psz_token = strrchr( sz_file, '.' );
								if( psz_token )
								{
									char sz_count[MAX_PATH];
									sprintf( sz_count, "%d", nrecord );

									*psz_token = 0;
									strcat( sz_file, sz_count );
									strcat( sz_file, ".jpg" );
									CopyFile( str_jpeg, sz_file, false );
								}
							}
							*/

							
							if( !braw_bmp && !is_jpeg( pSrc, nsize ) )
								continue;


							IFileFilter2Ptr		ptr_jpeg		= braw_bmp ? create_bmp_filter() : create_jpeg_filter();
							IFileDataObjectPtr	ptr_jpeg_file	= ptr_jpeg;
							if( ptr_jpeg == 0 || ptr_jpeg_file == 0 )
								continue;

							if( S_OK != ptr_jpeg_file->LoadFile( _bstr_t( str_jpeg ) ) )
							{
								CString str_format;							
								str_format.Format( IDS_RECORD_FORMAT, nrecord );

								str_error += str_format;
								str_error += ". Unspecified error.";
								str_error += "\n";

								continue;
							}

							IUnknown* punk = 0;
							ptr_jpeg->GetCreatedObject( 0, &punk );
							if( punk )
							{
								ptr_nd->SetValue( _bstr_t( str_path ), _variant_t( punk ) );
								punk->Release();
							}
						}
						else if( pfi->m_strVTObjectType == szTypeObjectList )
						{
							LPOS lpos = find_vt4_prefix( pSrc, nsize );
							if( lpos == -1 )
								continue;
							
							BYTE* pbuf_start = pSrc + lpos;
							CFile file( str_ol, CFile::modeCreate|CFile::modeWrite );
							file.Write( pbuf_start, nsize - lpos );
							file.Close();

							/*
							{
								char sz_file[MAX_PATH];
								strcpy( sz_file, str_jpeg );
								char* psz_token = strrchr( sz_file, '.' );
								if( psz_token )
								{
									char sz_count[MAX_PATH];
									sprintf( sz_count, "%d", nrecord );

									*psz_token = 0;
									strcat( sz_file, sz_count );
									strcat( sz_file, ".vti" );
									CopyFile( str_jpeg, sz_file, false );
								}
							}
							*/
							

							IFileFilter2Ptr		ptr_vti			= create_vti_filter();
							IFileDataObjectPtr	ptr_vti_file	= ptr_vti;
							if( ptr_vti == 0 || ptr_vti_file == 0 )
								continue;

							if( S_OK != ptr_vti_file->LoadFile( _bstr_t( str_ol ) ) )
							{
								CString str_format;							
								str_format.Format( IDS_RECORD_FORMAT, nrecord );

								str_error += str_format;
								str_error += ". Unspecified error.";
								str_error += "\n";

								continue;
							}

							int ncount = 0;
							ptr_vti->GetCreatedObjectCount( &ncount );
							for( int idx=0;idx<ncount; idx++ )
							{
								IUnknown* punk = 0;
								ptr_vti->GetCreatedObject( idx, &punk );
								if( punk )
								{
									if( ::GetObjectKind( punk ) == szTypeObjectList )
									{
										ptr_nd->SetValue( _bstr_t( str_path ), _variant_t( punk ) );

										{
											variant_t var_obj;
											ptr_nd->GetValue( _bstr_t( str_path ), &var_obj );
											if( var_obj.vt == VT_UNKNOWN )
											{
												CString str_name = ::GetObjectName( var_obj.punkVal );

												CString strArg;
												strArg.Format( "\"%s\"", (LPCTSTR)str_name );

												if( bcalc_measresult )
													::ExecuteAction( "CalcMeasResult", strArg, 0 );	

												if( bkaryo_build )
													::ExecuteAction( "KaryoBuild", strArg, 0 );	
											}
										}
									}

									punk->Release();
								}
							}
						}
					}
					catch(CException* e)
					{
						CString str_format;							
						str_format.Format( IDS_RECORD_FORMAT, nrecord );

						char sz_error[1024];	sz_error[0] = 0;
						e->GetErrorMessage( sz_error, sizeof(sz_error) );
						e->Delete();

						str_error += str_format;
						str_error += ". ";
						str_error += sz_error;
						str_error += "\n";
					}
					catch(...)
					{
						CString str_format;							
						str_format.Format( IDS_RECORD_FORMAT, nrecord );

						str_error += str_format;
						str_error += ". Unspecified error.";
						str_error += "\n";
					}

				}
			}
			m_ptr_rs->MoveNext();
		}


		ptr_sq->Update();

		if( ptr_active_dc )
			ptr_active_dc->SetObjectSelect( ptr_sq, 1 );

		if( str_error.GetLength() )
		{
			CString str_dump;
			str_dump.LoadString( IDS_ERROR_CONVERT );
			str_dump += "\n";
			str_dump += str_error;

			AfxMessageBox( str_dump, MB_ICONERROR );
		}
		else
			AfxMessageBox( IDS_CONVERTION_DONE, MB_ICONWARNING );			


		return true;
	}
	catch (_com_error &e)
	{
		dump_com_error( e );
		return false;
	}
	catch(CException* e)
	{
		e->ReportError();
		e->Delete();
	}
	catch(...)
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
void CConvDlg::OnBnClickedAnalize()
{
	CWaitCursor wait;

	open_src_table();
	analize_src_table();
	pack_field_info();
	fill_list();
	set_ui_enable();
}

/////////////////////////////////////////////////////////////////////////////
void CConvDlg::OnBnClickedVt4Browse()
{
	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, 0, _FILE_OPEN_SIZE_ );
	strcpy( dlg.m_ofn.lpstrFile, get_src_db() );
	dlg.m_ofn.lpstrFilter = "MSAccess files(*.mdb)\0*.mdb\0\0";
	if( dlg.DoModal() == IDOK )
	{
		::SetDlgItemText( GetSafeHwnd(), IDC_VT4_PATH, dlg.GetPathName() );		
		analize_target_is_empty();

		OnBnClickedAnalize();

	}

	set_ui_enable();


}

/////////////////////////////////////////////////////////////////////////////
void CConvDlg::OnBnClickedVt5Browse()
{
	CFileDialog dlg(FALSE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, 0, _FILE_OPEN_SIZE_ );
	strcpy( dlg.m_ofn.lpstrFile, get_target_db() );
	dlg.m_ofn.lpstrFilter = "MSAccess files(*.mdb)\0*.mdb\0\0";
	if( dlg.DoModal() == IDOK )
	{
		CString str_file_new	= dlg.GetPathName();
		CString str_file		= str_file_new;
		str_file.MakeLower();
		if( str_file.Find( ".mdb") == -1 )
			str_file_new += ".mdb";
	
		::SetDlgItemText( GetSafeHwnd(), IDC_VT5_PATH, str_file_new );		
	}

	set_ui_enable();
}

/////////////////////////////////////////////////////////////////////////////
void CConvDlg::OnBnClickedStart()
{
	CWaitCursor wait;

	start_convert();    

	::DeleteFile( get_jpeg_tmp_path() );
	::DeleteFile( get_vti_tmp_path() );

}



