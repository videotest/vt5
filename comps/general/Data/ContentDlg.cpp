// ContentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "data.h"
#include "ContentDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CSaveSetupSection__
{ // класс, занимающийся исключительно тем, что сохраняет текущую секцию, а при смерти - восстанавливает ее
public:
	CSaveSetupSection__(IUnknown* punkND)
	{
		m_sptrND = punkND;
		if(m_sptrND!=0) m_sptrND->GetCurrentSection(&m_bstrSection.GetBSTR());
	}
	~CSaveSetupSection__()
	{
		if(m_sptrND!=0) m_sptrND->SetupSection(m_bstrSection);
	}
private:
	INamedDataPtr m_sptrND;
	_bstr_t m_bstrSection;
};

/////////////////////////////////////////////////////////////////////////////
// CContentDlg dialog


CContentDlg::CContentDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CContentDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CContentDlg)
	m_nDataSource = 0;
	m_sLastSelectedPath.Empty();
	//}}AFX_DATA_INIT
}


void CContentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CContentDlg)
	DDX_Control(pDX, IDC_TREE1, m_treeData);
	DDX_CBIndex(pDX, IDC_DATA, m_nDataSource);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CContentDlg, CDialog)
	//{{AFX_MSG_MAP(CContentDlg)
	ON_CBN_SELCHANGE(IDC_DATA, OnSelchangeData)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
	ON_NOTIFY(TVN_DELETEITEM, IDC_TREE1, OnTvnDeleteitemTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnTvnSelchangedTree1)
	ON_NOTIFY(TVN_KEYDOWN, IDC_TREE1, OnTvnKeydownTree1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CContentDlg message handlers

void CContentDlg::OnSelchangeData() 
{
	UpdateData();
	FillTree();
}

BOOL CContentDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_imageList.Create(IDB_CONTENT_BITMAP, 16, 5, RGB( 255, 255, 255 ) );
	m_treeData.SetImageList( &m_imageList, TVSIL_NORMAL );

	FillTree();
    
	return FALSE; 
}

void CContentDlg::FillTree()
{												
	m_treeData.DeleteAllItems();

	CString			strDataSourceName;

	if( m_nDataSource == 0 )
	{
		m_ptrData = GetAppUnknown();
		strDataSourceName = "Application Data";

	}
	else
	{
		IApplicationPtr	ptrApp = GetAppUnknown();
		IUnknown		*punkDoc = 0;

		ptrApp->GetActiveDocument( &punkDoc );

		m_ptrData = punkDoc;
		if( punkDoc )
		{
			IDocumentSitePtr	ptrDocSite( punkDoc );
			BSTR	bstrFileName;
			ptrDocSite->GetPathName( &bstrFileName );

			strDataSourceName = bstrFileName;
			::SysFreeString( bstrFileName );

			punkDoc->Release();
		}
	}

	if( m_ptrData==0 )
	{
		m_treeData.InsertItem( "no active documents...", 1, 1 );
	}
	else
	{
		m_ptrData->SetupSection( 0 );

		HTREEITEM	hti = m_treeData.InsertItem( strDataSourceName, 1, 1 );
		_FillDataBrunch( m_ptrData, hti, 0, 0 );

		m_treeData.Expand( hti, TVE_EXPAND );
	}

	// [vanek] SBT: 898 - 02.04.2004
	CWnd *pwnd_focus = 0;
	if( _SetCurrSelPath( m_sLastSelectedPath ) )
		pwnd_focus = (CWnd*)(&m_treeData);
	else
		pwnd_focus = GetDlgItem( IDC_DATA );

	if( pwnd_focus )
		GotoDlgCtrl( pwnd_focus ); pwnd_focus = 0;	
}

void CContentDlg::_FillDataBrunch( INamedData *pIData, HTREEITEM hti, const char *szPath, const char *szEntryName )
{
	_bstr_t	bstrCurrentSection = szPath;
	pIData->SetupSection( bstrCurrentSection );

	_variant_t	var;
	_bstr_t		bstrEntry = szEntryName;
	pIData->GetValue( bstrCurrentSection, &var );
	int	nImage = -1;

	CString	strValue = szEntryName;
	if( strValue.IsEmpty() )
		strValue = "no name";

	{
		if( var.vt == VT_EMPTY )
		{
			strValue += ":Empty";
			nImage = 1;
		}
		else if( var.vt == VT_NULL )
		{
			strValue += ":Null";
			nImage = 1;
		}
		else if( var.vt == VT_UNKNOWN||var.vt == VT_DISPATCH )
		{
			strValue += ":Data object=";
			strValue += GetObjectName( var.punkVal );
			nImage = 0;	
		}
		else if( var.vt == VT_BITS )
		{
			strValue += ":Pointer ";

			long *ptr = (long*)var.lVal;
			if( !ptr )strValue += "(empty)";
			else 
			{
				CString	s;
				s.Format( "(%d)", *ptr );
				strValue += s;
			}

		}
		else
		{
			CString srtValue;
			_variant_t	varText;

			try
			{
				//dummy copy
				varText = var;
				_VarChangeType( varText, VT_BSTR );
				srtValue = varText.bstrVal;
			}
			catch(...)
			{

			}

			strValue += ":Value=";
			strValue += srtValue;
			nImage = 2;
		}
	}

	
/*	if( !szPath || strcmp( szPath, "\\" ) )
		bstrCurrentSection+="\\";
	bstrCurrentSection+=szEntryName;*/

	

	long nCount = 0;
	pIData->GetEntriesCount( &nCount );

	CStringArray	strs;

	for( int nEntry = 0; nEntry < nCount; nEntry++ )
	{
		BSTR	bstr;
		pIData->GetEntryName( nEntry, &bstr );
		CString	str = bstr;

		::SysFreeString( bstr );
		strs.Add( str );
	}

	hti = m_treeData.InsertItem( strValue, nImage, nImage, hti );

	// [vanek]: add to lParam
	CItemData *pitem_data = 0;
	if( szEntryName )
	{
		// [vanek]: 
		pitem_data = new CItemData;	// free on delete notification
		pitem_data->m_pstr_entry_name = new CString( szEntryName ); 
		pitem_data->m_bobject = (var.vt == VT_UNKNOWN || var.vt == VT_DISPATCH);
	}


	if( hti )
		m_treeData.SetItemData( hti, (DWORD_PTR)pitem_data );


	if( var.vt == VT_UNKNOWN||var.vt == VT_DISPATCH )
	{
		//Private named data
		if( CheckInterface( var.punkVal, IID_INamedData) )
		{	
			INamedDataObject2Ptr ptrND2( var.punkVal );
			if( ptrND2 )
			{
				
				INamedDataPtr ptrNDPrivate( ptrND2 );
				if( ptrNDPrivate )
				{
					BSTR bstrSection;//Kir SBT2084 :Save & restore curent section
					ptrNDPrivate->GetCurrentSection(&bstrSection );
					ptrNDPrivate->SetupSection(0);
					_FillDataBrunch( ptrNDPrivate, hti, 0, 0 );
					ptrNDPrivate->SetupSection(bstrSection);
					::SysFreeString(bstrSection);
				}
			}
		}
	}



	for( nEntry = 0; nEntry < nCount; nEntry++ )
	{
		CString	strPath = szPath;
		strPath += "\\";
		strPath += strs[nEntry];

		_FillDataBrunch( pIData, hti, strPath, strs[nEntry] );
	}
	m_treeData.Expand( hti, TVE_EXPAND );
}

BOOL CContentDlg::_GetCurrSelPath( CString *pstr_path )
{
	if( !pstr_path )  
		return FALSE;

	pstr_path->Empty();

	HTREEITEM hti = m_treeData.GetSelectedItem();
	if( !hti )
		return FALSE;

	HTREEITEM hti_parent = hti;
//	pstr_path->Empty();
    while( hti_parent && hti_parent != TVI_ROOT )
    {
		CItemData *pitem_data = 0;
		pitem_data = (CItemData *)m_treeData.GetItemData( hti_parent );

		if( pitem_data )
		{
			if( pitem_data->m_bobject )
			{	// path in private named data
                if( pitem_data->m_pstr_entry_name )
					*pstr_path = _T("[") + *(pitem_data->m_pstr_entry_name) + _T("]") + *pstr_path;			

				break;
			}
			else
			{
				if( pitem_data->m_pstr_entry_name )
					*pstr_path = _T("\\") + *(pitem_data->m_pstr_entry_name) + *pstr_path;
			}
		}
		
//		pitem_data = 0;
		hti_parent = m_treeData.GetParentItem( hti_parent );
	}
	
	return TRUE;
}

BOOL CContentDlg::_SetCurrSelPath( CString str_path )
{
	if( str_path.IsEmpty() )
		return FALSE;

	int nopen_bracket = str_path.Find( '[' ),
		nclose_bracket = str_path.Find( ']' );

	HTREEITEM hti_parent = 0;
    if( nopen_bracket != -1 && nopen_bracket < nclose_bracket )
	{   // object's named data
        CString str_obj_name( _T("") );        
        str_obj_name = str_path.Mid( nopen_bracket + 1, nclose_bracket - (nopen_bracket + 1) );
		str_path = str_path.Mid( nclose_bracket + 1 );
		hti_parent = _FindItemByEntryName( TVI_ROOT, str_obj_name, TRUE);
	}
	else
		// application's or document's named data
		hti_parent = m_treeData.GetChildItem( TVI_ROOT );      

	if( !hti_parent )
		return FALSE;

    return _SelectPath( hti_parent, str_path );
}

BOOL	CContentDlg::_SelectPath( HTREEITEM hti_parent, CString str_path )
{
    if( !hti_parent )                
		return FALSE;

	if( str_path.IsEmpty() )
	{
		m_treeData.SelectItem( hti_parent );
		return TRUE;
	}

	if( str_path[0] == '\\' )
	{
		hti_parent = m_treeData.GetChildItem( hti_parent );      
		str_path.Delete( 0 );
	}

    HTREEITEM	hti = 0;
	hti = _GetItemByPath( hti_parent, str_path );
    if( hti )        
	{
		m_treeData.SelectItem( hti );
		return TRUE;
	}
	
	return FALSE;
}

HTREEITEM	CContentDlg::_GetItemByPath( HTREEITEM hti_parent, CString str_path )
{
	if( !hti_parent || str_path.IsEmpty() )
		return 0;

    HTREEITEM	hti_child = m_treeData.GetChildItem( hti_parent ),
				hti_found = 0;
	
	CString str_entry( _T("") ),
			str_sub_path( _T("") );

	int	idx = str_path.Find( '\\' );
	if( idx != -1 )
	{
		str_entry = str_path.Left( idx );
		str_sub_path = str_path.Right( str_path.GetLength()-idx-1 );
	}
	else
		str_entry = str_path;

	while( hti_child )
	{
		HTREEITEM hti_child_saved = hti_child; 
		hti_child = m_treeData.GetNextItem( hti_child, TVGN_NEXT );

		CItemData *pitem_data = 0;
        pitem_data = (CItemData *)m_treeData.GetItemData( hti_child_saved );
		if( !pitem_data )
			continue;

		if( pitem_data->m_pstr_entry_name && (str_entry == *pitem_data->m_pstr_entry_name) )                	
		{
			hti_found = hti_child_saved;
			break;
		}
	}

	if( hti_found && !str_sub_path.IsEmpty() )
		return _GetItemByPath( hti_found, str_sub_path );

    return hti_found;
}

HTREEITEM	CContentDlg::_FindItemByEntryName( HTREEITEM hti_parent, CString str_entry_name, BOOL bobject )
{
    if( !hti_parent )    
		return 0;

    HTREEITEM hti_child = m_treeData.GetChildItem( hti_parent );
	while( hti_child )
	{
		CItemData *pitem_data = 0;
		pitem_data = (CItemData *)m_treeData.GetItemData( hti_child );
		if( pitem_data && pitem_data->m_pstr_entry_name && (bobject == pitem_data->m_bobject) && 
			(str_entry_name == *(pitem_data->m_pstr_entry_name)) )
			return hti_child;
		
        HTREEITEM hti_find = 0;
		hti_find = _FindItemByEntryName( hti_child, str_entry_name, bobject );
		if( hti_find  )
			return hti_find;
        
		hti_child = m_treeData.GetNextItem( hti_child, TVGN_NEXT );
	}					

    return 0;
}

void	CContentDlg::_ExpandChilds( HTREEITEM hti_parent, UINT nCode )
{
    if( !hti_parent )    
		return;

    HTREEITEM hti_child = m_treeData.GetChildItem( hti_parent );
	while( hti_child )
	{
		_ExpandChilds( hti_child, nCode );
		m_treeData.Expand( hti_child, nCode );
		hti_child = m_treeData.GetNextItem( hti_child, TVGN_NEXT );		
	}					
        
}

void CContentDlg::OnHelp() 
{
	HelpDisplay( "ContentDlg" ); 
}

void CContentDlg::OnTvnDeleteitemTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	CItemData *pitem_data = 0;
	pitem_data = (CItemData*)pNMTreeView->itemOld.lParam;
	if( pitem_data )
		delete pitem_data; pitem_data = 0;

	*pResult = 0;
}

void CContentDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	// save path for selected item 
	_GetCurrSelPath( &m_sLastSelectedPath );

	*pResult = 0;
}

void CContentDlg::OnTvnKeydownTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVKEYDOWN lpKeyDown = reinterpret_cast<LPNMTVKEYDOWN>(pNMHDR);

	if( lpKeyDown->wVKey == 'C' && ( GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1)) ) != 0 )
	{	
		if( ( GetKeyState(VK_SHIFT) & (1 << (sizeof(SHORT)*8-1)) ) != 0 )
		{
			// [vanek] : <ctrl>+<shift>+<C> - collapse all childs - 26.11.2004
			HTREEITEM ht = 0;
			ht = m_treeData.GetSelectedItem();
			m_treeData.SetRedraw( FALSE );
			_ExpandChilds( ht, TVE_COLLAPSE );
			m_treeData.EnsureVisible( ht );
			m_treeData.SetRedraw( TRUE );
			return;
		}
            
		
		// [vanek] : copy selected path to clipboard - 31.05.2004
		CString str_path( _T("") );
		_GetCurrSelPath( &str_path );
		if( !str_path.IsEmpty( ) )
		{
			if( str_path[0] != '\\' )
				str_path = '\\' + str_path;

			if (OpenClipboard())
			{
				EmptyClipboard();
				HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(TCHAR) * (str_path.GetLength() + 1) );
				if (hData != NULL)
				{
					LPSTR pszData = (LPSTR) ::GlobalLock(hData);
					strcpy(pszData, str_path);
					GlobalUnlock(hData);
					SetClipboardData(CF_TEXT, hData);
				}
				CloseClipboard();
			}
		}
    }
	else if( lpKeyDown->wVKey == 'E' && ( GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1)) ) != 0 &&
		( GetKeyState(VK_SHIFT) & (1 << (sizeof(SHORT)*8-1)) ) != 0 )
	{
        // [vanek] : <ctrl>+<shift>+<E> - expand all childs - 26.11.2004
		HTREEITEM ht = 0;
		ht = m_treeData.GetSelectedItem();
		m_treeData.SetRedraw( FALSE );
		_ExpandChilds( ht, TVE_EXPAND );
		m_treeData.Expand( ht, TVE_EXPAND );
		m_treeData.EnsureVisible( ht );
		m_treeData.SetRedraw( TRUE );
	}

	*pResult = 0;
}

void	CContentDlg::SetLastSelectedPath( CString str_path )
{
    m_sLastSelectedPath = str_path;
}

BOOL CContentDlg::GetLastSelectedPath( CString *pstr_path )
{
	if( !pstr_path )
		return FALSE;

	*pstr_path = m_sLastSelectedPath;
	return TRUE;
}

void	CContentDlg::SetLastDataSource( int ndata_source )
{
    m_nDataSource = ndata_source;
}

int		CContentDlg::GetLastDataSource( )
{
	return m_nDataSource;
}

// CTreeMod

IMPLEMENT_DYNAMIC(CTreeMod, CTreeCtrl)
CTreeMod::CTreeMod()
{
}

CTreeMod::~CTreeMod()
{
}


BEGIN_MESSAGE_MAP(CTreeMod, CTreeCtrl)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnNMRclick)
	ON_WM_CONTEXTMENU()
//	ON_COMMAND(ID_MOD_NAMED_VAL, OnModNamedVal)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnTvnEndlabeledit)
END_MESSAGE_MAP()


// CTreeMod message handlers

void CTreeMod::OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	SendMessage(WM_CONTEXTMENU, (WPARAM) m_hWnd, GetMessagePos());
	*pResult = 1;
}

void CTreeMod::OnContextMenu(CWnd* /*pWnd*/, CPoint ptMousePos)
{
	if (ptMousePos.x == -1 && ptMousePos.y == -1)
		ptMousePos = (CPoint) GetMessagePos();

	ScreenToClient(&ptMousePos);

	UINT uFlags;
	HTREEITEM htItem;
	
	htItem = HitTest( ptMousePos, &uFlags );

	if( htItem == NULL )
		return;
	
	m_hActiveItem = htItem;

	//CMenu menu;
	//CMenu* pPopup;

	//menu.LoadMenu(IDR_TREEITEM_CONTEXTMENU);
	//pPopup = menu.GetSubMenu(0);
	//ClientToScreen(&ptMousePos);
	//pPopup->TrackPopupMenu( TPM_LEFTALIGN, ptMousePos.x, ptMousePos.y, this );
}

void CTreeMod::OnModNamedVal()
{
	// TODO: Add your command handler code here
}

void CTreeMod::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	CContentDlg* pContentDlg=(CContentDlg*)GetParent();

	// find object and path to param
	CItemData *pitem_data = 0;
	CString path;
	CString object;
	HTREEITEM hti_parent = pTVDispInfo->item.hItem;
	CString sElement=GetItemText(hti_parent);
  while( hti_parent && hti_parent != TVI_ROOT )
  {
		pitem_data = (CItemData *)pContentDlg->m_treeData.GetItemData( hti_parent );

		if( pitem_data )
		{
			if( pitem_data->m_bobject )
			{	// path in private named data
                if( pitem_data->m_pstr_entry_name )
					object = *(pitem_data->m_pstr_entry_name);			
				break;
			}
			else
			{
				if( pitem_data->m_pstr_entry_name )
					path = _T("\\") + *(pitem_data->m_pstr_entry_name) + path;
			}
		}
		hti_parent = pContentDlg->m_treeData.GetParentItem( hti_parent );
	}

	INamedDataPtr pNamedData;
	if( pContentDlg->m_nDataSource == 0 )
	{
		pNamedData=pContentDlg->m_ptrData;
	}
	else
	{
		//Private named data
		pNamedData= ::GetObjectByName(pContentDlg->m_ptrData, object);
	}

	if( pNamedData )
	{
		CSaveSetupSection__ s(pNamedData);
		_bstr_t	bstrCurrentSection = path;
		pNamedData->SetupSection(bstrCurrentSection);
		CString sNewValue=pTVDispInfo->item.pszText;
		int	pos=sNewValue.Find(_T(":Value="));
		if(pos>=0)
			sNewValue=sNewValue.Mid(pos+_tcslen(_T(":Value=")));
		_variant_t var, varDest=sNewValue;
		pNamedData->GetValue(bstrCurrentSection, &var );
		HRESULT hr=::VariantChangeType(&varDest,&varDest,VARIANT_NOVALUEPROP,var.vt);
		if(!hr && varDest != var){
			pNamedData->SetValue(bstrCurrentSection, varDest);
			*pResult = 1;
			COleDispatchDriver pMainWnd((LPDISPATCH)::_GetOtherComponent(GetAppUnknown(),IID_IMainWindow),TRUE);
			pMainWnd.InvokeHelper(62,DISPATCH_METHOD,0,0,0);
		}
	}
}


//// CNamedValueModDlg dialog
//
//IMPLEMENT_DYNAMIC(CNamedValueModDlg, CDialog)
//CNamedValueModDlg::CNamedValueModDlg(CWnd* pParent /*=NULL*/)
//	: CDialog(CNamedValueModDlg::IDD, pParent)
//	, Value(_T(""))
//	, NamedPath(_T(""))
//{
//}
//
//CNamedValueModDlg::~CNamedValueModDlg()
//{
//}
//
//void CNamedValueModDlg::DoDataExchange(CDataExchange* pDX)
//{
//	CDialog::DoDataExchange(pDX);
//	DDX_Text(pDX, IDC_NAMED_VAL, Value);
//	DDX_Text(pDX, IDC_NAMED_VAL_PATH, NamedPath);
//}
//
//BEGIN_MESSAGE_MAP(CNamedValueModDlg, CDialog)
//END_MESSAGE_MAP()

