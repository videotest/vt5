// SetupATDlg.cpp : implementation file
//

#include "stdafx.h"
#include "audittrail.h"
#include "SetupATDlg.h"
#include "auditint.h"


/////////////////////////////////////////////////////////////////////////////
// CSetupATDlg dialog


CSetupATDlg::CSetupATDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetupATDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetupATDlg)
	//}}AFX_DATA_INIT
	m_nAuditedCount = 0;
	m_nActionsCount = 0;
}


void CSetupATDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupATDlg)
	DDX_Control(pDX, IDC_LIST_AVAIL, m_listActions);
	DDX_Control(pDX, IDC_LIST_AUDITED, m_listAudited);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetupATDlg, CDialog)
	//{{AFX_MSG_MAP(CSetupATDlg)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnButtonRemove)
	ON_LBN_SETFOCUS(IDC_LIST_AUDITED, OnSetfocusListAudited)
	ON_LBN_SETFOCUS(IDC_LIST_AVAIL, OnSetfocusListAvail)
	ON_LBN_SELCHANGE(IDC_LIST_AUDITED, OnSelchangeListAudited)
	ON_LBN_SELCHANGE(IDC_LIST_AVAIL, OnSelchangeListAvail)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupATDlg message handlers

BOOL CSetupATDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	CMap<CString, LPCSTR, bool, bool> mapAudited;
	
	while(true)
	{
		CString strParam;
		strParam.Format("%s%d", "Action_", m_nAuditedCount);
		CString strAction = ::GetValueString(GetAppUnknown(), "AuditTrail", strParam, "");
		if(strAction.IsEmpty())
			break;

		m_listAudited.AddString(strAction);

		mapAudited.SetAt(strAction, true);

		m_nAuditedCount++;
	}
	
	IUnknown	*punk = _GetOtherComponent(GetAppUnknown(), IID_IActionManager);
	ICompManagerPtr sptrCM(punk);
	if( punk )
		punk->Release();
	
	int nActionsCount = 0;
	sptrCM->GetCount(&nActionsCount);
	CString	str;

	for( long nAction = 0; nAction < nActionsCount; nAction++ )
	{
		IUnknown* punkAI = 0;
		sptrCM->GetComponentUnknownByIdx(nAction, &punkAI);
		sptrIActionInfo	sptrI(punkAI);
		if(punkAI)
			punkAI->Release();
		if(sptrI == 0)
			continue;

		BSTR	bstrName = 0, bstrUserName = 0, bstrGroupName = 0, bstrHelp = 0;
		sptrI->GetCommandInfo( &bstrName, &bstrUserName, &bstrHelp, &bstrGroupName );

		CString	strName( bstrName );
	
		::SysFreeString( bstrGroupName );
		::SysFreeString( bstrUserName );
		::SysFreeString( bstrName );
		::SysFreeString( bstrHelp );

		bool b = false;
		if((mapAudited.Lookup(strName, b) != TRUE))
		{
			m_listActions.AddString(strName);
			m_nActionsCount++;
		}
	}
	
	_SetCounters();

	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_REMOVE)->EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupATDlg::OnButtonAdd() 
{
	// TODO: Add your control notification handler code here
	long nCount = m_listActions.GetSelCount();
	if(nCount <= 0)
		return;

	int* pItems = new int[nCount];
	VERIFY(m_listActions.GetSelItems(nCount, pItems) == nCount);
	for(long i = nCount-1; i >= 0 ; i--)
	{
		CString strAction;
		m_listActions.GetText(pItems[i], strAction);
		m_listActions.DeleteString(pItems[i]);

		m_listAudited.AddString(strAction);
	}
	delete pItems;
	m_listActions.SetSel(-1, FALSE);

	m_nActionsCount -= nCount;
	m_nAuditedCount += nCount;
	_SetCounters();

	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(FALSE);
}

void CSetupATDlg::OnButtonRemove() 
{
	// TODO: Add your control notification handler code here
	long nCount = m_listAudited.GetSelCount();
	if(nCount <= 0)
		return;

	int* pItems = new int[nCount];
	VERIFY(m_listAudited.GetSelItems(nCount, pItems) == nCount);
	for(long i = nCount-1; i >= 0 ; i--)
	{
		CString strAction;
		m_listAudited.GetText(pItems[i], strAction);
		m_listAudited.DeleteString(pItems[i]);

		m_listActions.AddString(strAction);
	}
	delete pItems;
	m_listAudited.SetSel(-1, FALSE);

	m_nActionsCount += nCount;
	m_nAuditedCount -= nCount;
	_SetCounters();

	GetDlgItem(IDC_BUTTON_REMOVE)->EnableWindow(FALSE);
}

void CSetupATDlg::_SetCounters()
{
	CString strActionsCount;
	strActionsCount.Format("%d", m_nActionsCount);
	GetDlgItem(IDC_ACTIONS_COUNT)->SetWindowText(strActionsCount);
	CString strAuditedCount;
	strAuditedCount.Format("%d", m_nAuditedCount);
	GetDlgItem(IDC_AUDITED_COUNT)->SetWindowText(strAuditedCount);
}

void CSetupATDlg::OnOK() 
{
	// TODO: Add extra validation here
	int nCount = m_listAudited.GetCount();
	for(int i = 0; i <= nCount; i++)
	{
		CString strAction = "";
		if(i < nCount)
			m_listAudited.GetText(i, strAction);
		CString strParam;
		strParam.Format("%s%d", "Action_", i);
		::SetValue(GetAppUnknown(), "AuditTrail", strParam, strAction);
	}

	IUnknown* punkATM = _GetOtherComponent(GetAppUnknown(), IID_IAuditTrailMan);
	IAuditTrailManPtr sptrATM(punkATM);
	if(punkATM)
		punkATM->Release();

	if(sptrATM != 0)
	{
		sptrATM->RescanActionsList();
	}
	
	CDialog::OnOK();
}

void CSetupATDlg::OnSetfocusListAudited() 
{
	// TODO: Add your control notification handler code here
	m_listActions.SetSel(-1, FALSE);	
	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(FALSE);
}

void CSetupATDlg::OnSetfocusListAvail() 
{
	// TODO: Add your control notification handler code here
	m_listAudited.SetSel(-1, FALSE); 
	GetDlgItem(IDC_BUTTON_REMOVE)->EnableWindow(FALSE);
}

void CSetupATDlg::OnSelchangeListAudited() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_BUTTON_REMOVE)->EnableWindow(m_listAudited.GetSelCount() > 0 ? TRUE : FALSE);
}

void CSetupATDlg::OnSelchangeListAvail() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(m_listActions.GetSelCount() > 0 ? TRUE : FALSE);
}

void CSetupATDlg::OnHelp() 
{
	HelpDisplay( "SetupATDlg" );
}
