// EventDlg.cpp : implementation file
//

#include "stdafx.h"
#include "axeditor.h"
#include "EventDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEventDlg dialog


CEventDlg::CEventDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEventDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEventDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CEventDlg::~CEventDlg()
{
	m_arrEvents.RemoveAll();
}

void CEventDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventDlg)
	DDX_Control(pDX, IDC_LIST_EVENTS, m_listEvents);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEventDlg, CDialog)
	//{{AFX_MSG_MAP(CEventDlg)
	ON_BN_CLICKED(ID_BUTTON_EDIT_CODE, OnButtonEditCode)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_EVENTS, OnItemchangedListEvents)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventDlg message handlers

BOOL CEventDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here

	/*m_plbEvents = new CCheckListBox();
	CRect rc = NORECT;
	m_lb.GetClientRect(rc);
	m_plbEvents->Create(m_lb.GetStyle()|LBS_OWNERDRAWFIXED, rc, this, m_lb.GetDlgCtrlID());
	m_plbEvents->SetCheckStyle(BS_CHECKBOX);
	*/

	CRect	rcList = NORECT;
	m_listEvents.GetClientRect(rcList);
	m_listEvents.InsertColumn(1, "", LVCFMT_LEFT, rcList.Width());
	
	m_listEvents.SetExtendedStyle(LVS_EX_CHECKBOXES);

	int nSize = m_arrEvents.GetSize();
	for(int i = 0; i < nSize; i++)
	{
		CString strEvent = m_arrEvents[i];
		bool bCheck = strEvent[0] == 'Y';
		strEvent.Delete(0);
		m_listEvents.InsertItem(i, strEvent);
		m_listEvents.SetCheck(i, bCheck);
	}

	if(!m_strTitle.IsEmpty())
		SetWindowText(m_strTitle);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEventDlg::_OnOK()
{
	m_bAnyKilled = false;
	int nSize = m_listEvents.GetItemCount();	
	for(int i = 0; i < nSize; i++)
	{
		if(m_listEvents.GetCheck(i) == 1)
		{
			if(m_arrEvents[i][0] == 'Y')
				m_arrEvents[i] = "";
			else
				m_arrEvents[i].SetAt(0, 'A');
		}
		else
		{
			if(m_arrEvents[i][0] == 'Y')
			{
				m_arrEvents[i].SetAt(0, 'K');
				m_bAnyKilled = true;
			}
			else
				m_arrEvents[i] = "";
		}
	}
}

void CEventDlg::OnOK() 
{
	// TODO: Add extra validation here
	_OnOK();
	m_bEditCode = false;
	CDialog::OnOK();
}


void CEventDlg::OnButtonEditCode() 
{
	// TODO: Add your control notification handler code here
	m_bEditCode = true;
	_OnOK();
	CDialog::OnOK();
}


void CEventDlg::OnItemchangedListEvents(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	if(pNMListView->uChanged == LVIF_STATE) 
	{
		if(pNMListView->uNewState == 8192 || pNMListView->uNewState == 4096)
		{
			UINT nItem = pNMListView->iItem;
			int nSize = m_listEvents.GetItemCount();	
			bool bCheck = m_listEvents.GetCheck(nItem) == 1;
			if(m_listEvents.GetItemState(nItem, 0xffffffff) & LVIS_SELECTED )
			{
				for(int i = 0; i < nSize; i++)
				{	
					if(m_listEvents.GetItemState(i, 0xffffffff) & LVIS_SELECTED )
						m_listEvents.SetCheck(i, bCheck);	
				}
			}
		}
		
	}
	*pResult = 0;
}

void CEventDlg::OnHelp() 
{
	HelpDisplay( "EventDlg" );
}
