// InsertControlDlg.Cpp : implementation file
//

#include "StdAfx.H"
#include "insctldg.H"
#include "Shlwapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma comment(lib, "Shlwapi.lib")

/////////////////////////////////////////////////////////////////////////////
// CInsertControlDlg dialog


CInsertControlDlg::CInsertControlDlg( CWnd* pParent ) :
   CDialog( CInsertControlDlg::IDD, pParent ),
   m_clsid( CLSID_NULL )
{
	//{{AFX_DATA_INIT(CInsertControlDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CInsertControlDlg::~CInsertControlDlg()
{
	m_arrCLSID.RemoveAll();
}

void CInsertControlDlg::DoDataExchange(CDataExchange* pDX)
{
   int iItem;
   POSITION posControl;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInsertControlDlg)
	DDX_Control(pDX, IDC_CONTROLS, m_lbControls);
	//}}AFX_DATA_MAP

   if( pDX->m_bSaveAndValidate )
   {
	  iItem = m_lbControls.GetCurSel();
	  if( iItem == LB_ERR )
	  {
		 m_clsid = CLSID_NULL;
	  }
	  else
	  {
		 posControl = POSITION( m_lbControls.GetItemDataPtr( iItem ) );
		 ASSERT( posControl != NULL );
		 m_clsid = m_lControls.GetAt( posControl );
	  }
   }
}


BEGIN_MESSAGE_MAP(CInsertControlDlg, CDialog)
	//{{AFX_MSG_MAP(CInsertControlDlg)
	ON_LBN_DBLCLK(IDC_CONTROLS, OnControlsDblClk)
	ON_LBN_SELCHANGE(IDC_CONTROLS, OnControlsSelChange)
	//}}AFX_MSG_MAP
	ON_COMMAND(IDHELP, OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInsertControlDlg message handlers

BOOL CInsertControlDlg::OnInitDialog()
{
   HRESULT hResult;
   CATID catid;

   hResult = m_pCatInfo.CreateInstance( CLSID_StdComponentCategoriesMgr, NULL,
	  CLSCTX_INPROC_SERVER );
   if( FAILED( hResult ) )
   {
	  TRACE( "Failed to create category manager\n" );
	  EndDialog( IDCANCEL );
	  return( TRUE );
   }

	CDialog::OnInitDialog();

   catid = CATID_Control;
   m_aImplementedCategories.Add( catid );

   m_lbControls.ModifyStyle( 0, WS_HSCROLL );

   RefreshControlList();

   m_lbControls.SetFocus();

	return( FALSE );
}

void GetClassServerPath( REFCLSID clsid, CString& strServerPath )
{
   HKEY hKey;
   HKEY hServerKey;
   OLECHAR szCLSID[64];
   LONG nResult;
   ULONG nBytes;
   DWORD dwType;
   LPTSTR pszServerPath;

   StringFromGUID2( clsid, szCLSID, 64 );

   hKey = NULL;
   hServerKey = NULL;
   try
   {
	  nResult = RegOpenKeyEx( HKEY_CLASSES_ROOT, CString( "CLSID\\" )+CString(
		 szCLSID ), 0, KEY_READ, &hKey );
	  if( nResult != ERROR_SUCCESS )
	  {
		 throw( E_FAIL );
	  }

	  nResult = RegOpenKeyEx( hKey, _T( "InprocServer32" ), 0, KEY_READ,
		 &hServerKey );
	  if( nResult != ERROR_SUCCESS )
	  {
		 nResult = RegOpenKeyEx( hKey, _T( "InprocHandler32" ), 0, KEY_READ,
			&hServerKey );
		 if( nResult != ERROR_SUCCESS )
		 {
			nResult = RegOpenKeyEx( hKey, _T( "LocalServer32" ), 0, KEY_READ,
			   &hServerKey );
			if( nResult != ERROR_SUCCESS )
			{
			   throw( E_FAIL );
			}
		 }
	  }

	  nBytes = 0;
	  nResult = RegQueryValueEx( hServerKey, NULL, NULL, &dwType, NULL,
		 &nBytes );
	  if( (nResult != ERROR_SUCCESS) || (dwType != REG_SZ) )
	  {
		 throw( E_FAIL );
	  }
	  pszServerPath = LPTSTR( _alloca( nBytes ) );
	  nResult = RegQueryValueEx( hServerKey, NULL, NULL, &dwType,
		 LPBYTE( pszServerPath ), &nBytes );
	  if( (nResult != ERROR_SUCCESS) || (dwType != REG_SZ) )
	  {
		 throw( E_FAIL );
	  }

	  strServerPath = pszServerPath;

	  RegCloseKey( hKey );
	  hKey = NULL;
	  RegCloseKey( hServerKey );
	  hServerKey = NULL;
   }
   catch( HRESULT )
   {
	  if( hKey != NULL )
	  {
		 RegCloseKey( hKey );
	  }
	  if( hServerKey != NULL )
	  {
		 RegCloseKey( hServerKey );
	  }

	  strServerPath.LoadString( IDS_SERVERNOTFOUND );

	  return;
   }
}

void CInsertControlDlg::RefreshControlList()
{
   BOOL tDone;
   HRESULT hResult;
   IEnumGUIDPtr pEnum;
   ULONG nImplementedCategories;
   CATID* pcatidImpl;
   ULONG nRequiredCategories;
   CATID* pcatidReq;
   CLSID clsid;
   LPOLESTR pszName;
   CString strName;
   ULONG iCategory;
   int iItem;
   POSITION posControl;
   CString strServerPath;
   CString strString;

   m_lbControls.ResetContent();
   m_lControls.RemoveAll();

   nImplementedCategories = m_aImplementedCategories.GetSize();
   if( nImplementedCategories == 0 )
   {
	  nImplementedCategories = ULONG( -1 );
	  pcatidImpl = NULL;
   }
   else
   {
	  pcatidImpl = (CATID*)_alloca( nImplementedCategories*sizeof( CATID ) );
	  for( iCategory = 0; iCategory < nImplementedCategories; iCategory++ )
	  {
		 pcatidImpl[iCategory] = m_aImplementedCategories[iCategory];
	  }
   }

  nRequiredCategories = ULONG( -1 );
  pcatidReq = NULL;

   hResult = m_pCatInfo->EnumClassesOfCategories( nImplementedCategories,
	  pcatidImpl, nRequiredCategories, pcatidReq, &pEnum );
   if( FAILED( hResult ) )
   {
	  return;
   }

   tDone = FALSE;
   while( !tDone )
   {
	  hResult = pEnum->Next( 1, &clsid, NULL );
	  if( hResult == S_OK )
	  {
		 pszName = NULL;
		 hResult = OleRegGetUserType( clsid, USERCLASSTYPE_FULL, &pszName );
		 if( SUCCEEDED( hResult ) )
		 {
			strName = pszName;
			CoTaskMemFree( pszName );
			pszName = NULL;
			iItem = m_lbControls.AddString( strName );
			posControl = m_lControls.AddTail( clsid );
			m_lbControls.SetItemDataPtr( iItem, posControl );
		 }
	  }
	  else
	  {
		 tDone = TRUE;
	  }
   }

   OnControlsSelChange();
}


void CInsertControlDlg::OnControlsDblClk()
{
   OnOK();
}



void CInsertControlDlg::OnControlsSelChange()
{
	int iItem = m_lbControls.GetCurSel();
	GetDlgItem( IDOK )->EnableWindow( iItem != LB_ERR );
}



void CInsertControlDlg::OnOK() 
{
	int iItem = m_lbControls.GetCurSel();

	ASSERT( iItem != -1 );
	POSITION posControl = POSITION( m_lbControls.GetItemDataPtr( iItem ) );
	m_clsid = m_lControls.GetAt( posControl );

	m_arrCLSID.RemoveAll();
	int nCount = m_lbControls.GetSelCount();
	m_arrCLSID.SetSize(nCount);
	int* pInt = new int[nCount];
	m_lbControls.GetSelItems( nCount, pInt );
	for(int i = 0; i < nCount; i++)
	{
		POSITION posControl = POSITION( m_lbControls.GetItemDataPtr( pInt[i] ) );
		m_arrCLSID[i] = m_lControls.GetAt( posControl );
	}

	delete pInt;
	CDialog::OnOK();
}

void CInsertControlDlg::OnHelp() 
{
	HelpDisplay( "InsertControlDlg" );
}