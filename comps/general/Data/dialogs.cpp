// dialogs.cpp : implementation file
//

#include "stdafx.h"
#include "Data.h"
#include "dialogs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCreateObjectDlg dialog


CCreateObjectDlg::CCreateObjectDlg(IUnknown *punkData, CWnd* pParent /*=NULL*/)
	: CDialog(CCreateObjectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreateObjectDlg)
	m_strName = _T("");
	//}}AFX_DATA_INIT
	m_sptrD = punkData;
}


void CCreateObjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateObjectDlg)
	DDX_Control(pDX, IDC_KIND, m_comboKinds);
	DDX_Text(pDX, IDC_NAME, m_strName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCreateObjectDlg, CDialog)
	//{{AFX_MSG_MAP(CCreateObjectDlg)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreateObjectDlg message handlers

BOOL CCreateObjectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	FillTypesCombo();

	m_strName = m_strObjectName;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCreateObjectDlg::OnOK() 
{
	if (!UpdateData())
		return;
//store name
	m_strObjectName = m_strName;
//store kind
	long	nPos = m_comboKinds.GetCurSel();
	INamedDataInfo *pTypeInfo = (INamedDataInfo *)m_comboKinds.GetItemData( nPos );
	m_strObjectType = ::GetObjectName( pTypeInfo );

	//without addref

	if (m_sptrD != NULL)
	{
		long NameExists = 0;
		
		if (!m_strName.IsEmpty())
		{// check for unique name 
			_bstr_t btsrObjName(m_strObjectName);
			m_sptrD->NameExists(btsrObjName, &NameExists);
		}
		if (NameExists)
		{
			CString message, str;
			str.LoadString(IDS_ERR_OBJECTNAME_EXISTS_S);
			message.Format(str, m_strObjectName);
			MessageBox(message);
		}
		else
			CDialog::OnOK();
	}
	else
		CDialog::OnOK();
}


void CCreateObjectDlg::FillTypesCombo()
{
	IUnknown	*punkApp = GetAppUnknown();

	sptrIDataTypeInfoManager	sptrT( punkApp );

	long	nTypesCount = 0;
	sptrT->GetTypesCount( &nTypesCount );
	INamedDataInfo	*pTypeInfo = 0;

	CString	strSel;

	for( long nType = 0; nType < nTypesCount; nType++ )
	{
		BSTR	bstr = 0;
		sptrT->GetTypeInfo( nType, &pTypeInfo );

		INamedObject2Ptr	ptrNamed( pTypeInfo );
		
		BSTR	bstrC = 0;
		pTypeInfo->GetContainerType( &bstrC );
		CString	strCType = bstrC;
		bool	bEmpty = strCType.GetLength()==0;
		::SysFreeString( bstrC );

		pTypeInfo->Release();

		if( !bEmpty )continue;

		// [vanek] BT2000:3553 - 24.12.2003
		INamedDataInfo2Ptr sptrTypeInfo2 = ptrNamed;
		if( sptrTypeInfo2 == 0 )
			continue;

		DWORD dwContState = 0;
		sptrTypeInfo2->GetContainerState( &dwContState );
		if( !(dwContState & cntsEnabled) )
			continue;
		//
        
		ptrNamed->GetUserName( &bstr );

		CString	strType = bstr; 
		::SysFreeString( bstr );

		int nIdx = m_comboKinds.AddString( strType );
		m_comboKinds.SetItemData( nIdx, (DWORD)pTypeInfo );

		if( ::GetObjectName( pTypeInfo ) == m_strObjectType )
			strSel = strType;
	}

	if( strSel.IsEmpty() )
		m_comboKinds.SetCurSel( 0 );
	else
		m_comboKinds.SelectString( 0, strSel );
}

/////////////////////////////////////////////////////////////////////////////
// CActivateObjectDlg dialog


CActivateObjectDlg::CActivateObjectDlg( IUnknown *punkContext, CWnd* pParent )
	: CDialog(CActivateObjectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CActivateObjectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_sptrObjToActivate = 0;

	m_tree.AttachData(punkContext, 0);

	m_strDlgTitle.LoadString( IDS_ACTIVATE_OBJECT );
	m_strBoundTitle.LoadString( IDS_BOUND_TITLE );
	m_strObjectTitle.LoadString( IDS_OBJECT_TITLE );
}


void CActivateObjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CActivateObjectDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CActivateObjectDlg, CDialog)
	//{{AFX_MSG_MAP(CActivateObjectDlg)
	ON_NOTIFY(NM_DBLCLK, ID_INNER_TREE, OnDblclkTree)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CActivateObjectDlg message handlers


BOOL CActivateObjectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowText(m_strDlgTitle);

	if( GetDlgItem(IDC_BOUND_STRING) )
		((CWnd*)GetDlgItem(IDC_BOUND_STRING))->SetWindowText(m_strBoundTitle);

	if( GetDlgItem(IDC_OBJECT_STRING) )
		((CWnd*)GetDlgItem(IDC_OBJECT_STRING))->SetWindowText(m_strObjectTitle);

	if (!m_tree.Attach(WS_BORDER, this, IDC_TREE))
		return false;

	m_tree.FillAllTypes(true);
	
	return TRUE;
}

void CActivateObjectDlg::OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnOK();
	
	*pResult = 0;
}
void CActivateObjectDlg::OnOK() 
{
	m_sptrObjToActivate = m_tree.GetSelectedObject();

	if (m_sptrObjToActivate == 0)
	{
		AfxMessageBox(IDS_OBJECT_REQUIRED);
		return;
	}
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CDeleteObjectDlg dialog


CDeleteObjectDlg::CDeleteObjectDlg( IUnknown *pDD, CWnd* pParent )
	: CDialog(CDeleteObjectDlg::IDD, pParent)
{
	m_tree.AttachData(0, pDD);
	m_punkSelected = 0;
	//{{AFX_DATA_INIT(CDeleteObjectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDeleteObjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeleteObjectDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeleteObjectDlg, CDialog)
	//{{AFX_MSG_MAP(CDeleteObjectDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE, OnDblclkTree)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDeleteObjectDlg::OnOK() 
{
	m_punkSelected = m_tree.GetSelectedObject();

	if (!m_punkSelected)
	{
		AfxMessageBox(IDS_OBJECT_REQUIRED);
		return;
	}
	
	CDialog::OnOK();
}

void CDeleteObjectDlg::OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnOK();
	
	*pResult = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CDeleteObjectDlg message handlers

BOOL CDeleteObjectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (!m_tree.Attach(WS_BORDER, this, IDC_TREE))
		return false;

	m_tree.FillAllTypes(true);
	
	return TRUE; 
}


//should be called before DoModal()
void CDeleteObjectDlg::SetSelectedObject(LPCTSTR szName)
{
	m_strSelectedObject = szName;
}

//with AddRef
IUnknown *CDeleteObjectDlg::GetSelectedObject()
{
	m_punkSelected->AddRef();
	return m_punkSelected;
}


/////////////////////////////////////////////////////////////////////////////
// CRenameDlg dialog

CRenameObjectDlg::CRenameObjectDlg(IUnknown* punkTarget, CWnd* pParent /*=NULL*/)
	: CDialog(CRenameObjectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRenameObjectDlg)
	m_strName = _T("");
	//}}AFX_DATA_INIT
	m_tree.AttachData(0, punkTarget);
	m_sptrD = punkTarget;
	m_punkRenameObject = 0;
}


void CRenameObjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRenameObjectDlg)
	DDX_Text(pDX, IDC_EDIT, m_strName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRenameObjectDlg, CDialog)
	//{{AFX_MSG_MAP(CRenameObjectDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE, OnDblclkTree)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRenameObjectDlg message handlers

BOOL CRenameObjectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (!m_tree.Attach(WS_BORDER, this, IDC_TREE))
		return false;

	m_tree.FillAllTypes(true);
	HTREEITEM hti = m_tree.FindObjectItem( m_strOldName );
	if( hti != 0 )m_tree.SetObjectActive( hti, true );

	
	return TRUE;
}

void CRenameObjectDlg::OnOK() 
{
	m_punkRenameObject = m_tree.GetSelectedObject();

	if (!m_punkRenameObject)
	{
		AfxMessageBox(IDS_OBJECT_REQUIRED);
		return;
	}

	m_strOldName = ::GetObjectName(m_punkRenameObject);   

	if (!UpdateData())
		return;

	if (m_strName.IsEmpty())
	{
		AfxMessageBox(IDS_OBJECT_NEWNAME_REQUIRED);
		return;
	}

	long	NameExists = 0;
	_bstr_t bstrName(m_strName);

	// if new name already exists
	m_sptrD->NameExists(bstrName, &NameExists);
	if (NameExists)
	{
		CString message, str;
		str.LoadString(IDS_ERR_OBJECTNAME_EXISTS_S);
		message.Format(str, m_strName);
		MessageBox(message);
	}

	m_strNewName = m_strName;

	CDialog::OnOK();
}

void CRenameObjectDlg::OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
}

IUnknown* CRenameObjectDlg::GetRenamedObject()
{	
	if (!m_punkRenameObject)
		return NULL;

	m_punkRenameObject->AddRef();
	
	return m_punkRenameObject;
}
/////////////////////////////////////////////////////////////////////////////
// COrganizerDlg dialog


COrganizerDlg::COrganizerDlg(IUnknown *punkDoc, CWnd* pParent /*=NULL*/)
	: CDialog(COrganizerDlg::IDD, pParent)
{
	m_punkDoc = punkDoc;
	m_punkApp = GetAppUnknown();

	m_nUndoCounter = 1;
	//{{AFX_DATA_INIT(COrganizerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

COrganizerDlg::~COrganizerDlg()
{
	m_contextApp->AttachData( 0 );
	m_contextDoc->AttachData( 0 );
}

void COrganizerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COrganizerDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COrganizerDlg, CDialog)
	//{{AFX_MSG_MAP(COrganizerDlg)
	ON_BN_CLICKED(IDC_TOAPP, OnToapp)
	ON_BN_CLICKED(IDC_TODOC, OnTodoc)
	ON_BN_CLICKED(IDC_UNDO, OnUndo)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COrganizerDlg message handlers


BOOL COrganizerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_contextApp.CreateInstance( szContextProgID );
	m_contextDoc.CreateInstance( szContextProgID );

	m_contextApp->AttachData( m_punkApp );
	m_contextDoc->AttachData( m_punkDoc );


	m_treeApp.SetHilightActiveItems( false );
	m_treeApp.AttachData( m_contextApp, m_punkApp );
	m_treeDoc.SetHilightActiveItems( false );
	m_treeDoc.AttachData( m_contextDoc, m_punkDoc );
	
	if (!m_treeApp.Attach(WS_CHILD|WS_VISIBLE|WS_BORDER, this, IDC_TREE1))
		return false;
	if (!m_treeDoc.Attach(WS_CHILD|WS_VISIBLE|WS_BORDER, this, IDC_TREE2))
		return false;							

	m_treeApp.FillAllTypes( true );
	m_treeDoc.FillAllTypes( true );

	ChangeUndoButtonState( false );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COrganizerDlg::OnToapp() 
{																	  
	IUnknown *punkToMove = m_treeDoc.GetSelectedObject();

	if( !punkToMove )
		return;

	CString	strExecuteParams;
	strExecuteParams.Format( "\"%s\"", (const char *)::GetObjectName( punkToMove ) );

	::ExecuteAction( _T("ObjectMoveToApp"), strExecuteParams );

	UpdateTrees();

	ChangeUndoButtonState( true );
}

void COrganizerDlg::OnTodoc() 
{																	  
	IUnknown *punkToMove = m_treeApp.GetSelectedObject();

	if( !punkToMove )
		return;

	CString	strExecuteParams;
	strExecuteParams.Format( "\"%s\"", (const char *)::GetObjectName( punkToMove ) );
	//punkToMove->Release();

	::ExecuteAction( _T("ObjectMoveToDoc"), strExecuteParams );

	UpdateTrees();
	ChangeUndoButtonState( true );
}

void COrganizerDlg::OnUndo() 
{
	::ExecuteAction( _T("Undo") );

	UpdateTrees();
	ChangeUndoButtonState( false );
}

void COrganizerDlg::ChangeUndoButtonState( bool bAddAction )
{
	if( bAddAction )
		m_nUndoCounter++;
	else
		m_nUndoCounter--;

	GetDlgItem( IDC_UNDO )->EnableWindow( m_nUndoCounter != 0 );
}


void COrganizerDlg::UpdateTrees()
{
	m_treeApp.ClearContens();
	m_treeDoc.ClearContens();
	m_treeApp.FillAllTypes( true );
	m_treeDoc.FillAllTypes( true );
}
void CCreateObjectDlg::OnHelp() 
{
	HelpDisplay( "CreateObjectDlg" );
}

void CActivateObjectDlg::OnHelp() 
{
	HelpDisplay( "ActivateObjectDlg" );
}

void COrganizerDlg::OnHelp() 
{
	HelpDisplay( "OrganizerDlg" );
}

void CRenameObjectDlg::OnHelp() 
{
	HelpDisplay( "RenameObjectDlg" );
}

void CDeleteObjectDlg::OnHelp() 
{
	HelpDisplay( "DeleteObjectDlg" );
	
}
