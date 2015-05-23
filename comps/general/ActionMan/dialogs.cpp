// dialogs.cpp : implementation file
//

#include "stdafx.h"
#include "ActionMan.h"
#include "dialogs.h"
#include "actionmanager.h"


/////////////////////////////////////////////////////////////////////////////
// CEnumDialog dialog

CEnumDialog::CEnumDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CEnumDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEnumDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEnumDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEnumDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEnumDialog, CDialog)
	//{{AFX_MSG_MAP(CEnumDialog)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnumDialog message handlers

class action_ex_info
{
public:
	action_ex_info()
	{
		m_clsid = INVALID_KEY;
	}
	virtual ~action_ex_info()
	{

	}

	CString	m_str_dll_name;
	CString	m_str_name;
	CLSID	m_clsid;
};

BOOL CEnumDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	ShowWindow( SW_SHOW );
	UpdateWindow();
	
	CWaitCursor wait;

	long nActionsCount = CActionManager::s_pActionManager->GetComponentCount();

	CArray<action_ex_info*,action_ex_info*>	ar_info;

	IVTApplicationPtr ptr_guard( ::GetAppUnknown() );
	if( ptr_guard )
	{
		CString str_prefix;
		{
			BSTR bstr_prefix = 0; 
			ptr_guard->GetData( 0, 0, &bstr_prefix, 0);
			str_prefix = bstr_prefix;
			if( bstr_prefix )	::SysFreeString( bstr_prefix );	bstr_prefix = 0;
		}
		
		
		CCompRegistrator registrator( "Actions" );
			DWORD	dwSize = 255;
			char	sz[255];

		for( int i = 0; ::RegEnumValue( registrator.m_hKey, i, sz, &dwSize, 0, 0, 0, 0 )==0; i++ )
		{	
			dwSize = 255;

			CLSID	clsid = INVALID_KEY;

			if (::CLSIDFromProgID( _bstr_t( sz ), &clsid) != S_OK)
				continue;

			IUnknown	*punk = 0;
			HRESULT hresult = ::CoCreateInstance(clsid, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (void **)&punk);
			
			if (!TraceIfFailed(hresult))
				continue;
			
			IActionInfoPtr ptr_ai = punk;
			punk->Release();

			if( ptr_ai == 0 )	continue;

			
			BSTR bstr = 0;
			ptr_ai->GetCommandInfo( &bstr, 0, 0, 0 );
			_bstr_t bstr_action_name = bstr;
			if( bstr )	::SysFreeString( bstr );	bstr = 0;

			CString str_entry = sz;
			//str_entry += "_";
			str_entry += str_prefix;				
			str_entry += "\\CLSID";

			CVTRegKey key( HKEY_CLASSES_ROOT, str_entry );
			key.Open();
			CVTRegValue val;
			key.GetValue( val );
			

			char* psz_value = val.GetString();
			if( !psz_value )	continue;


			str_entry = "CLSID\\";
			str_entry += psz_value;
			str_entry += "\\InprocServer32";
			
			CVTRegKey key_file( HKEY_CLASSES_ROOT, str_entry );
			key_file.Open();
			CVTRegValue val_file;
			key_file.GetValue( val_file );

			psz_value = val_file.GetString();
			if( !psz_value )	continue;


			char sz_file_name[MAX_PATH];	sz_file_name[0] = 0;
			GetLongPathName( psz_value, sz_file_name, sizeof(sz_file_name) );

			action_ex_info* pai = new action_ex_info;
			pai->m_str_name		= (char*)bstr_action_name;
			pai->m_clsid		= clsid;
			pai->m_str_dll_name	= sz_file_name;

			ar_info.Add( pai );


			CString str_txt;
			str_txt.Format( "Stage 1/2. Enum files %d/%d", i , nActionsCount );
			
			SetWindowText( str_txt );
			UpdateWindow();
		}
	}



	CEdit *pedit = (CEdit *)GetDlgItem( IDC_EDIT1 );		

	CString	str;

	str = "ActionName\tUserName\tGroupName\tHelp\tType\tFileName\tArgIn\tArgOut\r\n";

	

	for( long nAction = 0; nAction < nActionsCount; nAction++ )
	{
		sptrIActionInfo	sptrI( CActionManager::s_pActionManager->GetComponent( nAction, false ) );
		IUnknown	*punkA = 0;
		sptrI->ConstructAction( &punkA );

		bool	bFilter = CheckInterface( punkA, IID_IFilterAction );
		bool	bInteractive = CheckInterface( punkA, IID_IInteractiveAction );
		punkA->Release();


		BSTR	bstrName = 0, bstrUserName = 0, bstrGroupName = 0, bstrHelp = 0;
		sptrI->GetCommandInfo( &bstrName, &bstrUserName, &bstrHelp, &bstrGroupName );

		CString	strGroup( bstrGroupName );
		CString	strUserName( bstrUserName );
		CString	strName( bstrName );
		CString	strHelp( bstrHelp );
		CString strType;
		CString strFileName;
		CString strArgIn;
		CString strArgOut;

		::SysFreeString( bstrGroupName );
		::SysFreeString( bstrUserName );
		::SysFreeString( bstrName );
		::SysFreeString( bstrHelp );
		

		int	nInArgCount = 0;
		int	nOutArgCount = 0;

		if( bFilter )
			strType =" (filter)";
		if( bInteractive )
			strType =" (interactive)";

		for( int i=0;i<ar_info.GetSize(); i++ )
		{
			action_ex_info* p = ar_info[i];
			if( p->m_str_name == strName )
			{
				strFileName = p->m_str_dll_name;
				delete p;
				ar_info.RemoveAt( i );
				break;
			}
		}

		sptrI->GetArgsCount( &nInArgCount );
		sptrI->GetOutArgsCount( &nOutArgCount );


		if( nInArgCount != 0 )
		{
			for( int nArg = 0; nArg < nInArgCount; nArg++ )
			{
				BSTR	bstrName = 0;
				BSTR	bstrKind = 0;
				BSTR	bstrDefVal = 0;
				sptrI->GetArgInfo( nArg, &bstrName, &bstrKind, &bstrDefVal );

				CString	strArg = bstrName;
				CString	strKind = bstrKind;
				CString	strVal = bstrDefVal;

				::SysFreeString( bstrName );
				::SysFreeString( bstrKind );
				::SysFreeString( bstrDefVal );

				
				strArgIn += strArg;
				strArgIn += ":";
				strArgIn += strKind;
				
				if( !strVal.IsEmpty() )
					strArgIn += "="+strVal;

				if( nArg != nInArgCount - 1 )
					strArgIn += "; ";
			}
		}

		if( nOutArgCount != 0 )
		{
			for( int nArg = 0; nArg < nOutArgCount; nArg++ )
			{
				BSTR	bstrName = 0;
				BSTR	bstrKind = 0;
			
				sptrI->GetOutArgInfo( nArg, &bstrName, &bstrKind );

				CString	strArg = bstrName;
				CString	strKind = bstrKind;
			
				::SysFreeString( bstrName );
				::SysFreeString( bstrKind );
			
				strArgOut += strArg;
				strArgOut += ":";
				strArgOut += strKind;

				if( nArg != nOutArgCount - 1 )
					strArgOut += "; ";				
			}
		}

		//str = "ActionName\tUserName\tGroupName\tHelp\tType\tFileName\tArgIn\tArgOut\r\n";

		int nlen = strHelp.GetLength();
		for( int idx=0;idx<nlen;idx++ )
			if( strHelp[idx] == '\n' )	strHelp.SetAt( idx, ' ' );

		str += strName + "\t";
		str += strUserName + "\t";
		str += strGroup + "\t";
		str += strHelp + "\t";
		str += strType + "\t";
		str += strFileName + "\t";
		str += strArgIn + "\t";
		str += strArgOut + "\t";

		str += "\r\n";

		CString str_txt;
		str_txt.Format( "Stage 2/2. Enum actions %d/%d", nAction, nActionsCount );
		
		SetWindowText( str_txt );
		UpdateWindow();

	}


	pedit->SetWindowText( str );

	for( int i=0;i<ar_info.GetSize(); i++ )
	{
		action_ex_info* p = ar_info[i];
		delete p;
	}
	ar_info.RemoveAll();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
// CScriptDlg dialog


CScriptDlg::CScriptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScriptDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScriptDlg)
	m_strScriptText = _T("");
	//}}AFX_DATA_INIT
}


void CScriptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScriptDlg)
	DDX_Text(pDX, IDC_SCRIPT, m_strScriptText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScriptDlg, CDialog)
	//{{AFX_MSG_MAP(CScriptDlg)
	ON_BN_CLICKED(IDC_PREV, OnPrev)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScriptDlg message handlers

void CScriptDlg::OnPrev() 
{
	if( m_scripts.GetSize() < 0 )
		return;

	CMenu	menu;
	menu.CreatePopupMenu();

	for( int i = 0; i < m_scripts.GetSize(); i++ )
	{
		CString	strScriptText = m_scripts[i];

		int nPos = strScriptText.Find( "\r" );
		if( nPos == -1 )
			nPos = strScriptText.Find( "\n" );
		
		if( nPos != -1 )
			strScriptText = strScriptText.Left( nPos );

		if( nPos != -1 )
		{
			HMENU hmenu = ::CreatePopupMenu();

			char sz_buf[1024];
			::ZeroMemory( sz_buf, 0 );
			CString s1=m_scripts[i];
			s1.Remove('\r');
			strncpy( sz_buf, s1, sizeof(sz_buf) - 1 );

			char* psz = strtok( sz_buf, "\n" );
			while( psz )
			{
				::AppendMenu( hmenu, MF_STRING, i + 1, psz ); 
				psz = strtok( 0, "\n" );
			}

			menu.AppendMenu( MF_POPUP, (UINT)hmenu, strScriptText );
		}
		else
			menu.AppendMenu( MF_STRING, i+1, strScriptText );
			
	}

	CPoint	pt;
	CRect	rect;

	GetDlgItem( IDC_PREV )->GetWindowRect( rect );
	//ScreenToClient( rect );

	pt.x = rect.left;
	pt.y = rect.bottom;

	int nPos = menu.TrackPopupMenu( TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RETURNCMD, pt.x, pt.y, this )-1;

	GetDlgItem( IDC_SCRIPT )->SetFocus();

	if( nPos == -1 )
		return;

	m_strScriptText = m_scripts[nPos];

	m_strScriptText.Remove('\r');
	m_strScriptText.Replace("\n","\r\n");
	//int cr = m_strScriptText.Find( '\n' );
	//while( cr != -1 )
	//{
	//	m_strScriptText.Insert( cr, '\r' );
	//	cr = m_strScriptText.Find( '\n', cr + 2 );
	//}

	UpdateData( false );
}

BOOL CScriptDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	RestoreScripts();

	if( m_scripts.GetSize() > 0 )
		m_strScriptText = m_scripts[0];

	UpdateData( false );
	// TODO: Add extra initialization here

	//AAM
	m_ScriptEdit.SubclassDlgItem(IDC_SCRIPT, this);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CScriptDlg::OnOK() 
{
	if( !UpdateData() )
		return;

	if( m_scripts.GetSize() == 0 || m_scripts[0] != m_strScriptText && !m_strScriptText.IsEmpty() )
		m_scripts.InsertAt( 0, m_strScriptText );

	for(int i=1; i<m_scripts.GetSize(); i++)
	{
		if(m_scripts[i] == m_strScriptText)
		{
			m_scripts.RemoveAt(i);
			break;
		}
	}

	StoreScripts();

	CDialog::OnOK();
}

const char szScriptSection[] = "CommandLines";
const char szScriptCount[] = "Count";
const char szScript[] = "Script";

void CScriptDlg::RestoreScripts()
{
	int	nCount = min( 25, ::GetValueInt( GetAppUnknown(), szScriptSection, szScriptCount, 0 ) );

	for( int nScript = 0; nScript < nCount; nScript++ )
	{
		CString	strEntry;
		strEntry.Format( "%s%d", szScript, nScript );
		CString	str = ::GetValueString( GetAppUnknown(), szScriptSection, strEntry, 0 );

		str.Remove('\r');
		str.Replace("\n","\r\n");

		if( str.IsEmpty() )
			continue;



		m_scripts.Add( str );
	}
}
void CScriptDlg::StoreScripts()
{
	int	nCount = m_scripts.GetSize();
	::SetValue( GetAppUnknown(), szScriptSection, szScriptCount, _variant_t((long)nCount) );

	for( int nScript = 0; nScript < nCount; nScript++ )
	{
		CString	strEntry;
		strEntry.Format( "%s%d", szScript, nScript );
		::SetValue( GetAppUnknown(), szScriptSection, strEntry, m_scripts[nScript] );
	}
}
void CEnumDialog::OnHelp() 
{
	HelpDisplay( "EnumDialog" );
}

void CScriptDlg::OnHelp() 
{
	HelpDisplay( "ScriptDlg" );
}
