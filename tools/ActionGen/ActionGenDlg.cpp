// ActionGenDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ActionGen.h"
#include "ActionGenDlg.h"

#include "objbase.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CActionGenDlg dialog

CActionGenDlg::CActionGenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CActionGenDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CActionGenDlg)
	//}}AFX_DATA_INIT

	m_strAction = AfxGetApp()->GetProfileString( "CActionGenDlg", "m_strAction" );
	m_strFileName  = AfxGetApp()->GetProfileString( "CActionGenDlg", "m_strFileName" );
	m_strProjectName  = AfxGetApp()->GetProfileString( "CActionGenDlg", "m_strProjectName" );
	m_nSettings  = AfxGetApp()->GetProfileInt( "CActionGenDlg", "m_nSettings", FALSE );
	m_strTarget  = AfxGetApp()->GetProfileString( "CActionGenDlg", "m_strTarget" );
	m_bTarget  = AfxGetApp()->GetProfileInt( "CActionGenDlg", "m_bTarget", m_bTarget );
	m_bInteractive = AfxGetApp()->GetProfileInt( "CActionGenDlg", "m_bInteractive", m_bInteractive );
	m_bUndo = AfxGetApp()->GetProfileInt( "CActionGenDlg", "m_bUndo", m_bUndo );
	m_bUpdate = AfxGetApp()->GetProfileInt( "CActionGenDlg", "m_bUpdate", m_bUpdate );


	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CActionGenDlg::~CActionGenDlg()
{
	AfxGetApp()->WriteProfileString( "CActionGenDlg", "m_strAction", m_strAction );
	AfxGetApp()->WriteProfileString( "CActionGenDlg", "m_strFileName", m_strFileName );
	AfxGetApp()->WriteProfileString( "CActionGenDlg", "m_strProjectName", m_strProjectName );
	AfxGetApp()->WriteProfileInt( "CActionGenDlg", "m_nSettings", m_nSettings );
	AfxGetApp()->WriteProfileString( "CActionGenDlg", "m_strTarget", m_strTarget );
	AfxGetApp()->WriteProfileInt( "CActionGenDlg", "m_bTarget", m_bTarget );
	AfxGetApp()->WriteProfileInt( "CActionGenDlg", "m_bInteractive", m_bInteractive );
	AfxGetApp()->WriteProfileInt( "CActionGenDlg", "m_bUndo", m_bUndo );
	AfxGetApp()->WriteProfileInt( "CActionGenDlg", "m_bUpdate", m_bUpdate );
}

void CActionGenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CActionGenDlg)
	DDX_Text(pDX, IDC_ACTION, m_strAction);
	DDX_Text(pDX, IDC_FILENAME, m_strFileName);
	DDX_Text(pDX, IDC_PROJECT, m_strProjectName);
	DDX_Check(pDX, IDC_SETTINGS, m_nSettings);
	DDX_Text(pDX, IDC_STR_TARGET, m_strTarget);
	DDX_Check(pDX, IDC_TARGET, m_bTarget);
	DDX_Check(pDX, IDC_INTERACTIVE, m_bInteractive);
	DDX_Check(pDX, IDC_UNDONEABLE, m_bUndo);
	DDX_Check(pDX, IDC_UPDATE, m_bUpdate);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CActionGenDlg, CDialog)
	//{{AFX_MSG_MAP(CActionGenDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_GENERATE, OnGenerate)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CActionGenDlg message handlers

BOOL CActionGenDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CActionGenDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CActionGenDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


class CStringArrayEx : public CStringArray
{
public:
	int	Find( const char *szString, int iPos = 0 )
	{
		for( int i = iPos; i < GetSize(); i++ )
		{
			if( GetAt( i ) == szString )
				return i;
		}

		return Add( szString );
	}

	void ReadFile( const char *szFileName )
	{
		try
		{
			CStdioFile	file( szFileName, CFile::modeRead|CFile::typeText );

			RemoveAll();

			CString	s;

			for( ;; )
			{
				if( !file.ReadString( s ) )
					break;
				Add( s );
			}

		}
		catch( CException *pe )
		{
			pe->Delete();
		}
	}
	void WriteFile( const char *szFileName )
	{
		try
		{
			CStdioFile	file( szFileName, CFile::modeCreate|CFile::modeWrite|CFile::typeText );

			for( int i = 0; i < GetSize(); i++ )
				file.WriteString( GetAt( i )+"\n" );

		}
		catch( CException *pe )
		{
			pe->ReportError();
			pe->Delete();
		}
	}

};

void CActionGenDlg::OnGenerate() 
{
	if( !UpdateData() )
		return;

	if( m_strFileName.IsEmpty() || m_strAction.IsEmpty() ) 
		return;

	char	szDrv[_MAX_DRIVE], szPath[_MAX_PATH], szFileName[_MAX_PATH];

	::_splitpath( m_strFileName, szDrv, szPath, szFileName, 0 );

	char	szPathCPP[_MAX_PATH], szPathH[_MAX_PATH];

	::_makepath( szPathCPP, szDrv, szPath, szFileName, ".cpp" );
	::_makepath( szPathH, szDrv, szPath, szFileName, ".h" );

	CString	strClassName = CString( "CAction" )+m_strAction;
	CString	strProgID = m_strProjectName+"."+m_strAction;
	CString	strInfoName = "guid"+m_strAction;

	/*GUID	GUID_NULL;
	ZeroMemory( &GUID_NULL, sizeof( GUID_NULL  ) );*/

	GUID	guidInfo1= GUID_NULL, guidInfo2= GUID_NULL, guidA1= GUID_NULL, guidA2 = GUID_NULL;

	::CoCreateGuid( &guidInfo1 );
	::CoCreateGuid( &guidInfo2 );
	::CoCreateGuid( &guidA1 );
	::CoCreateGuid( &guidA2 );

	if( guidInfo1 == GUID_NULL ||
		guidInfo2 == GUID_NULL ||
		guidA1 == GUID_NULL ||
		guidA2 == GUID_NULL )
	{
		AfxMessageBox( "Can't create guid" );
		return;
	}

	CString	strFormatComment, strFormatInfo, strFormatA;


	strFormatComment = "// {%08lX-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}";
	strFormatA = "0x%lx, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x);";
	strFormatInfo = "{ 0x%lx, 0x%x, 0x%x, { 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x } };";

	CString	strGinfo1C, strGinfo1, 
			strGA1C, strGA1,
			strGinfo2C, strGinfo2, 
			strGA2, strGA2C;

	strGinfo1C.Format( strFormatComment, 
		guidInfo1.Data1, guidInfo1.Data2, guidInfo1.Data3,
		guidInfo1.Data4[0], guidInfo1.Data4[1], guidInfo1.Data4[2], guidInfo1.Data4[3],
		guidInfo1.Data4[4], guidInfo1.Data4[5], guidInfo1.Data4[6], guidInfo1.Data4[7] );
	strGinfo1.Format( strFormatInfo, 
		guidInfo1.Data1, guidInfo1.Data2, guidInfo1.Data3,
		guidInfo1.Data4[0], guidInfo1.Data4[1], guidInfo1.Data4[2], guidInfo1.Data4[3],
		guidInfo1.Data4[4], guidInfo1.Data4[5], guidInfo1.Data4[6], guidInfo1.Data4[7] );

	strGinfo2C.Format( strFormatComment, 
		guidInfo2.Data1, guidInfo2.Data2, guidInfo2.Data3,
		guidInfo2.Data4[0], guidInfo2.Data4[1], guidInfo2.Data4[2], guidInfo2.Data4[3],
		guidInfo2.Data4[4], guidInfo2.Data4[5], guidInfo2.Data4[6], guidInfo2.Data4[7] );
	strGinfo2.Format( strFormatInfo, 
		guidInfo2.Data1, guidInfo2.Data2, guidInfo2.Data3,
		guidInfo2.Data4[0], guidInfo2.Data4[1], guidInfo2.Data4[2], guidInfo2.Data4[3],
		guidInfo2.Data4[4], guidInfo2.Data4[5], guidInfo2.Data4[6], guidInfo2.Data4[7] );


	strGA1C.Format( strFormatComment,
		guidA1.Data1, guidA1.Data2, guidA1.Data3,
		guidA1.Data4[0], guidA1.Data4[1], guidA1.Data4[2], guidA1.Data4[3],
		guidA1.Data4[4], guidA1.Data4[5], guidA1.Data4[6], guidA1.Data4[7] );
	strGA1.Format( strFormatA,
		guidA1.Data1, guidA1.Data2, guidA1.Data3,
		guidA1.Data4[0], guidA1.Data4[1], guidA1.Data4[2], guidA1.Data4[3],
		guidA1.Data4[4], guidA1.Data4[5], guidA1.Data4[6], guidA1.Data4[7] );

	strGA2C.Format( strFormatComment,
		guidA2.Data1, guidA2.Data2, guidA2.Data3,
		guidA2.Data4[0], guidA2.Data4[1], guidA2.Data4[2], guidA2.Data4[3],
		guidA2.Data4[4], guidA2.Data4[5], guidA2.Data4[6], guidA2.Data4[7] );
	strGA2.Format( strFormatA,
		guidA2.Data1, guidA2.Data2, guidA2.Data3,
		guidA2.Data4[0], guidA2.Data4[1], guidA2.Data4[2], guidA2.Data4[3],
		guidA2.Data4[4], guidA2.Data4[5], guidA2.Data4[6], guidA2.Data4[7] );


	//default .h file

	CStringArrayEx	strsCpp, strsH;

	strsH.Add( CString()+"#ifndef __"+szFileName+"_h__" );
	strsH.Add( CString()+"#define __"+szFileName+"_h__" );
	strsH.Add( CString()+"" );
	strsH.Add( CString()+"//[ag]1. classes" );
	strsH.Add( CString()+"" );
	strsH.Add( CString()+"" );
	strsH.Add( CString()+"#endif //__"+szFileName+"_h__" );


	strsCpp.Add( CString()+"#include \"stdafx.h\"" );
	strsCpp.Add( CString()+"#include \""+szFileName+".h\"" );
	strsCpp.Add( CString()+"" );
	strsCpp.Add( CString()+"" );
	strsCpp.Add( CString()+"//[ag]1. dyncreate" );
	strsCpp.Add( CString()+"" );
	strsCpp.Add( CString()+"" );
	strsCpp.Add( CString()+"#ifdef _DEBUG" );
	strsCpp.Add( CString()+"//[ag]2. olecreate debug" );
	strsCpp.Add( CString()+"" );
	strsCpp.Add( CString()+"" );
	strsCpp.Add( CString()+"//[ag]3. guidinfo debug" );
	strsCpp.Add( CString()+"" );
	strsCpp.Add( CString()+"" );
	strsCpp.Add( CString()+"#else" );
	strsCpp.Add( CString()+"//[ag]4. olecreate release" );
	strsCpp.Add( CString()+"" );
	strsCpp.Add( CString()+"" );
	strsCpp.Add( CString()+"//[ag]5. guidinfo release" );
	strsCpp.Add( CString()+"" );
	strsCpp.Add( CString()+"" );
	strsCpp.Add( CString()+"#endif //_DEBUG" );
	strsCpp.Add( CString()+"" );
	strsCpp.Add( CString()+"" );
	strsCpp.Add( CString()+"//[ag]6. action info" );
	strsCpp.Add( CString()+"" );
	strsCpp.Add( CString()+"" );
	strsCpp.Add( CString()+"//[ag]7. targets" );
	strsCpp.Add( CString()+"" );
	strsCpp.Add( CString()+"" );
	strsCpp.Add( CString()+"//[ag]8. arguments" );
	strsCpp.Add( CString()+"" );
	strsCpp.Add( CString()+"" );
	strsCpp.Add( CString()+"//[ag]9. implementation" );
	strsCpp.Add( CString()+"" );
	strsCpp.Add( CString()+"" );


	strsH.ReadFile( szPathH );
	strsCpp.ReadFile( szPathCPP );

	int	iPos = strsH.Find( "//[ag]1. classes" );
	iPos++;

	if( m_bInteractive )
		strsH.InsertAt( ++iPos, CString()+"class "+strClassName+" : public CInteractiveActionBase" );
	else
		strsH.InsertAt( ++iPos, CString()+"class "+strClassName+" : public CActionBase" );
	strsH.InsertAt( ++iPos, CString()+"{" );
	strsH.InsertAt( ++iPos, CString()+"\tDECLARE_DYNCREATE("+strClassName+")" );
	strsH.InsertAt( ++iPos, CString()+"\tDECLARE_OLECREATE("+strClassName+")" );
	strsH.InsertAt( ++iPos, CString()+"" );
	if( m_nSettings )
		strsH.InsertAt( ++iPos, CString()+"ENABLE_SETTINGS();" );
	if( m_bUndo )
		strsH.InsertAt( ++iPos, CString()+"ENABLE_UNDO();" );

	strsH.InsertAt( ++iPos, CString()+"public:" );
	strsH.InsertAt( ++iPos, CString()+"\t"+strClassName+"();" );
	strsH.InsertAt( ++iPos, CString()+"\tvirtual ~"+strClassName+"();" );
	strsH.InsertAt( ++iPos, CString()+"" );
	strsH.InsertAt( ++iPos, CString()+"public:" );
	strsH.InsertAt( ++iPos, CString()+"\tvirtual bool Invoke();" );
	if( m_nSettings )
		strsH.InsertAt( ++iPos, CString()+"\tvirtual bool ExecuteSettings( CWnd *pwndParent );" );
	if( m_bUndo )
	{
		strsH.InsertAt( ++iPos, CString()+"//undo interface" );
		strsH.InsertAt( ++iPos, CString()+"\tvirtual bool DoUndo();" );
		strsH.InsertAt( ++iPos, CString()+"\tvirtual bool DoRedo();" );
	}

	if( m_bUpdate )
	{
		strsH.InsertAt( ++iPos, CString()+"//update interface" );
		strsH.InsertAt( ++iPos, CString()+"\tvirtual bool IsAvaible();" );
		strsH.InsertAt( ++iPos, CString()+"\tvirtual bool IsChecked();" );
	}


	strsH.InsertAt( ++iPos, CString()+"};" );
	strsH.InsertAt( ++iPos, "" );

	

	iPos = strsCpp.Find( "//[ag]1. dyncreate" );
	iPos++;
	strsCpp.InsertAt( ++iPos, CString()+"IMPLEMENT_DYNCREATE("+strClassName+", CCmdTargetEx);" );

	iPos = strsCpp.Find( "//[ag]2. olecreate debug" );
	iPos++;

	strsCpp.InsertAt( ++iPos, strGA1C );
	strsCpp.InsertAt( ++iPos, CString()+"IMPLEMENT_OLECREATE("+strClassName+", \""+strProgID+"D\"," );
	strsCpp.InsertAt( ++iPos, strGA1 );
	
	iPos = strsCpp.Find( "//[ag]3. guidinfo debug" );
	iPos++;

	strsCpp.InsertAt( ++iPos, strGinfo1C );
	strsCpp.InsertAt( ++iPos, "static const GUID "+strInfoName +" =" );
	strsCpp.InsertAt( ++iPos, strGinfo1 );

	iPos = strsCpp.Find( "//[ag]4. olecreate release" );
	iPos++;

	strsCpp.InsertAt( ++iPos, strGA2C );
	strsCpp.InsertAt( ++iPos, CString()+"IMPLEMENT_OLECREATE("+strClassName+", \""+strProgID+"\"," );
	strsCpp.InsertAt( ++iPos, strGA2 );

	iPos = strsCpp.Find( "//[ag]5. guidinfo release" );
	iPos++;

	strsCpp.InsertAt( ++iPos, strGinfo2C );
	strsCpp.InsertAt( ++iPos, "static const GUID "+strInfoName +" =" );
	strsCpp.InsertAt( ++iPos, strGinfo2 );

	iPos = strsCpp.Find( "//[ag]6. action info" );
	iPos++;
	strsCpp.InsertAt( ++iPos, "ACTION_INFO_FULL(" + strClassName + ", -1, -1, -1, " + strInfoName + ");" );
	

	if( m_bTarget )
	{
		iPos = strsCpp.Find( "//[ag]7. targets" );
		iPos++;
		strsCpp.InsertAt( ++iPos, "ACTION_TARGET(" + strClassName + ", \"" +m_strTarget+ "\");" );
	}

	
	iPos = strsCpp.Find( "//[ag]9. implementation" );
	iPos++;

	strsCpp.InsertAt( ++iPos, "//////////////////////////////////////////////////////////////////////" );
	strsCpp.InsertAt( ++iPos, "//"+strClassName+" implementation" );

	strsCpp.InsertAt( ++iPos, strClassName+"::"+strClassName+"()" );
	strsCpp.InsertAt( ++iPos, "{" );
	strsCpp.InsertAt( ++iPos, "}" );
	strsCpp.InsertAt( ++iPos, "" );
	strsCpp.InsertAt( ++iPos, strClassName+"::~"+strClassName+"()" );
	strsCpp.InsertAt( ++iPos, "{" );
	strsCpp.InsertAt( ++iPos, "}" );
	strsCpp.InsertAt( ++iPos, "" );

	if( m_nSettings )
	{
		
		strsCpp.InsertAt( ++iPos, "bool "+strClassName+"::ExecuteSettings( CWnd *pwndParent )" );
		strsCpp.InsertAt( ++iPos, "{" );
		strsCpp.InsertAt( ++iPos, "\treturn true;" );
		strsCpp.InsertAt( ++iPos, "}" );
		strsCpp.InsertAt( ++iPos, "" );
	}

	strsCpp.InsertAt( ++iPos, "bool "+strClassName+"::Invoke()" );
	strsCpp.InsertAt( ++iPos, "{" );
	strsCpp.InsertAt( ++iPos, "\treturn true;" );
	strsCpp.InsertAt( ++iPos, "}" );
	strsCpp.InsertAt( ++iPos, "" );

	if( m_bUndo )
	{
		strsCpp.InsertAt( ++iPos, "//undo/redo" );
		strsCpp.InsertAt( ++iPos, "bool "+strClassName+"::DoUndo()" );
		strsCpp.InsertAt( ++iPos, "{" );
		strsCpp.InsertAt( ++iPos, "\treturn true;" );
		strsCpp.InsertAt( ++iPos, "}" );
		strsCpp.InsertAt( ++iPos, "" );
		strsCpp.InsertAt( ++iPos, "bool "+strClassName+"::DoRedo()" );
		strsCpp.InsertAt( ++iPos, "{" );
		strsCpp.InsertAt( ++iPos, "\treturn true;" );
		strsCpp.InsertAt( ++iPos, "}" );
		strsCpp.InsertAt( ++iPos, "" );
	}

	if( m_bUpdate )
	{
		strsCpp.InsertAt( ++iPos, "//extended UI" );
		strsCpp.InsertAt( ++iPos, "bool "+strClassName+"::IsAvaible()" );
		strsCpp.InsertAt( ++iPos, "{" );
		strsCpp.InsertAt( ++iPos, "\treturn true;" );
		strsCpp.InsertAt( ++iPos, "}" );
		strsCpp.InsertAt( ++iPos, "" );
		strsCpp.InsertAt( ++iPos, "bool "+strClassName+"::IsChecked()" );
		strsCpp.InsertAt( ++iPos, "{" );
		strsCpp.InsertAt( ++iPos, "\treturn false;" );
		strsCpp.InsertAt( ++iPos, "}" );
		strsCpp.InsertAt( ++iPos, "" );
	}

	

	strsH.WriteFile( szPathH );
	strsCpp.WriteFile( szPathCPP );

}

void CActionGenDlg::OnBrowse() 
{
	UpdateData();

	CFileDialog	dlg( true, "*.cpp", m_strFileName );

	if( dlg.DoModal() == IDOK )
		m_strFileName = dlg.GetPathName();

	UpdateData( FALSE );
}

void CActionGenDlg::OnCancel() 
{
	UpdateData();
	
	CDialog::OnCancel();
}

void CActionGenDlg::OnOK() 
{
	OnGenerate();
	
	CDialog::OnOK();
}
