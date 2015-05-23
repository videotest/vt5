// AbacusViewerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AbacusViewer.h"
#include "AbacusViewerDlg.h"
#include "AbacusWork.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString _GetProfileString(const char *pSect, const char *pEntry, const char *pDef)
{
	CString s = AfxGetApp()->GetProfileString(pSect, pEntry);
	if (s.IsEmpty())
	{
		AfxGetApp()->WriteProfileString(pSect, pEntry, pDef);
		s = pDef;
	}
	return s;
}

int _GetProfileInt(const char *pSect, const char *pEntry, int nDef)
{
	int n = AfxGetApp()->GetProfileInt(pSect, pEntry, -1);
	if (n == -1)
	{
		if (nDef >= 0)
			AfxGetApp()->WriteProfileInt(pSect, pEntry, nDef);
		n = nDef;
	}
	return n;
}


// CAbacusViewerDlg dialog

BEGIN_DHTML_EVENT_MAP(CAbacusViewerDlg)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()


CAbacusViewerDlg::CAbacusViewerDlg(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(CAbacusViewerDlg::IDD, CAbacusViewerDlg::IDH, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAbacusViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
}

BEGIN_MESSAGE_MAP(CAbacusViewerDlg, CDHtmlDialog)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_VIEW, OnBnClickedButtonView)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CAbacusViewerDlg message handlers

BOOL CAbacusViewerDlg::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	ABACUSCONTROLSETUP AbacusSetup = {0};
	AbacusSetup.nPackageBuffSize = _GetProfileInt("Abacus", "MaxPackageSize", 16384);
	AbacusSetup.nConnectTimeoutMs = _GetProfileInt("Abacus", "ConnectTimeout", 10000);
	AbacusSetup.nDebugLog = _GetProfileInt("Abacus", "DebugLog", 0);
	AbacusSetup.bRequestPLT = _GetProfileInt("Abacus", "RequestPLT", FALSE)?true:false;
	AbacusSetup.bRequestRBC = _GetProfileInt("Abacus", "RequestRBC", FALSE)?true:false;
	AbacusSetup.bRequestWBC = _GetProfileInt("Abacus", "RequestWBC", FALSE)?true:false;
	COMMSETUP CommSetup = {0};
	CommSetup.nBaud = _GetProfileInt("Comm", "Baud", CBR_9600);
	CString sCom = _GetProfileString("Comm", "Port", "Com1");
	strncpy(CommSetup.szComm, sCom, min(sCom.GetLength(),50));
	CommSetup.nDataBits = _GetProfileInt("Comm", "DataBits", 8);
	CommSetup.nParity = _GetProfileInt("Comm", "Parity", NOPARITY);
	CommSetup.nReadTotalTimeoutConstant = _GetProfileInt("Comm", "Parity", 1000);
	CommSetup.nStopBits = _GetProfileInt("Comm", "StopBits", ONESTOPBIT);
	if (!_AbacusConnect(&AbacusSetup,&CommSetup))
		AfxMessageBox("Can not connect", MB_OK|MB_ICONEXCLAMATION);

	int nWidth = 80;
	m_List.InsertColumn(0, "SNO", 0, nWidth);
	m_List.InsertColumn(1, "Date", 0, nWidth);
	m_List.InsertColumn(2, "Time", 0, nWidth);
	m_List.InsertColumn(3, "SID", 0, nWidth);
	m_List.InsertColumn(4, "PID", 0, nWidth);
	m_List.InsertColumn(5, "Name", 0, nWidth);
	m_List.InsertColumn(6, "Birth", 0, nWidth);
	m_List.InsertColumn(7, "Sex", 0, nWidth);
	m_List.InsertColumn(8, "Doc", 0, nWidth);
	m_List.InsertColumn(9, "Opid", 0, nWidth);
	m_List.InsertColumn(10, "Mode", 0, nWidth);
	m_List.InsertColumn(11, "Wrn", 0, nWidth);
	m_List.InsertColumn(12, "PM1", 0, nWidth);
	m_List.InsertColumn(13, "PM2", 0, nWidth);
	m_List.InsertColumn(14, "RM1", 0, nWidth);
	m_List.InsertColumn(15, "WM1", 0, nWidth);
	m_List.InsertColumn(16, "WM2", 0, nWidth);
	m_List.InsertColumn(17, "WM3", 0, nWidth);
	for (int i = 0; i < 25; i++)
	{
		CString s;
		s.Format("Par%d", i+1);
		m_List.InsertColumn(18+i, s, 0, nWidth);
	}

	SetTimer(1, 500, NULL);

//	m_List.InsertItem(0, "Item 1");
//	m_List.SetItemText(0, 1, "Subitem 1");
//	m_List.SetItemText(0, 2, "Subitem 2");
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAbacusViewerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
		CDHtmlDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAbacusViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

HRESULT CAbacusViewerDlg::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;
}

HRESULT CAbacusViewerDlg::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;
}

static int s_nLastRecordAdded = 0;
void CAbacusViewerDlg::AddAbacusData()
{
	char szBuff[256];
	long lPos = 0;
	_AbacusGetFirstRecordPosition(&lPos);
	while(lPos)
	{
		long lPrevPos = lPos;
		ABACUSRECORD *p;
		_AbacusGetNextRecord(&lPos, &p);
		int nItem = m_List.GetItemCount();
		itoa(p->Data.nSNO, szBuff, 10);
		m_List.InsertItem(nItem, szBuff);
		sprintf(szBuff, "%d-%d-%d",  p->Data.tmMeasurement.tm_mday,
			p->Data.tmMeasurement.tm_mon, p->Data.tmMeasurement.tm_year);
		m_List.SetItemText(nItem, 1, szBuff);
		sprintf(szBuff, "%d:%d:%d",  p->Data.tmMeasurement.tm_hour,
			p->Data.tmMeasurement.tm_min, p->Data.tmMeasurement.tm_sec);
		m_List.SetItemText(nItem, 2, szBuff);
		m_List.SetItemText(nItem, 3, p->Data.pszSID);
		m_List.SetItemText(nItem, 4, p->Data.pszPID);
		m_List.SetItemText(nItem, 5, p->Data.pszName);
		sprintf(szBuff, "%d-%d-%d",  p->Data.dtBirth.tm_mday,
			p->Data.dtBirth.tm_mon, p->Data.dtBirth.tm_year);
		m_List.SetItemText(nItem, 6, szBuff);
		m_List.SetItemText(nItem, 7, p->Data.nSex?"female":"male");
		m_List.SetItemText(nItem, 8, p->Data.pszDoc);
		m_List.SetItemText(nItem, 9, p->Data.pszOPID);
		m_List.SetItemText(nItem, 10, itoa(p->Data.nMode, szBuff, 10));
		m_List.SetItemText(nItem, 11, itoa(p->Data.dwWarn, szBuff, 16));
		m_List.SetItemText(nItem, 12, itoa(p->Data.nPM1, szBuff, 10));
		m_List.SetItemText(nItem, 13, itoa(p->Data.nPM2, szBuff, 10));
		m_List.SetItemText(nItem, 14, itoa(p->Data.nRM1, szBuff, 10));
		m_List.SetItemText(nItem, 15, itoa(p->Data.nWM1, szBuff, 10));
		m_List.SetItemText(nItem, 16, itoa(p->Data.nWM2, szBuff, 10));
		m_List.SetItemText(nItem, 17, itoa(p->Data.nWM3, szBuff, 10));
		for (int i = 0; i < p->Data.nParn; i++)
		{
			CString s;
			int n = p->Data.padParams[i].nParFlag;
			if (n == 0)
				s.Format("%g", p->Data.padParams[i].dParValue);
			else if (n == 1)
			{
				s.Format("%g", p->Data.padParams[i].dParValue);
				s += _T("+");
			}
			else if (n == 2)
			{
				s.Format("%g", p->Data.padParams[i].dParValue);
				s += _T("-");
			}
			else if (n == 3)
			{
				s.Format("%g", p->Data.padParams[i].dParValue);
				s += _T("*");
			}
			else if (n == 4)
				s = _T("---E");
			m_List.SetItemText(nItem, 18+i, s);			
		}
		_AbacusRemoveRecord(lPrevPos);
	}
	s_nLastRecordAdded = _AbacusGetLastRecordNum();
}

void CAbacusViewerDlg::SaveAbacusData()
{
	char szPath[_MAX_PATH],szDrive[_MAX_DRIVE],szDir[_MAX_DIR];
	if (GetModuleFileName(AfxGetInstanceHandle(), szPath, _MAX_PATH) == 0)
		return;
	_splitpath(szPath, szDrive, szDir, NULL, NULL);
	_makepath(szPath, szDrive, szDir, "AbacusData", ".txt");
	CStdioFile File;
	if (!File.Open(szPath, CFile::modeCreate|CFile::modeWrite|CFile::typeText))
		return;
	int nColumns = 43;
	char szText[200];
	for (int i = 0; i < nColumns; i++)
	{
		LVCOLUMN Col = {0};
		Col.mask = LVCF_TEXT;
		Col.cchTextMax = 200;
		Col.pszText = szText;
		if (m_List.GetColumn(i, &Col))
			File.WriteString(Col.pszText);
		else
			File.WriteString("??");
		if (i < nColumns-1)
			File.WriteString("\t");
	}
	File.WriteString("\n");
	int nItems = m_List.GetItemCount();
	for (i = 0; i < nItems; i++)
	{
		for (int j = 0; j < nColumns; j++)
		{
			CString s = m_List.GetItemText(i, j);
			if (j < nColumns-1)
				s += "\t";
			File.WriteString(s);
		}
		if (i < nItems-1)
			File.WriteString("\n");
	}
}


void CAbacusViewerDlg::PostNcDestroy()
{
	KillTimer(1);
	_AbacusDisconnect();

	CDHtmlDialog::PostNcDestroy();
}

void CAbacusViewerDlg::OnBnClickedButtonView()
{
	AddAbacusData();
	SaveAbacusData();
}

void CAbacusViewerDlg::OnTimer(UINT nIDEvent)
{
	static int nPrevState = -1;
	int nState = _AbacusGetState();
	if (nState != nPrevState)
	{
		nPrevState = nState;
		CString s("?");
		if (nState == AbacusState_NotConnected)
			s = "Not connected";
		else if (nState == AbacusState_Connecting)
			s = "Connecting";
		else if (nState == AbacusState_Connected)
			s = "Connected";
		else if (nState == AbacusState_Error)
			s = "Error";
		::SetWindowText(::GetDlgItem(m_hWnd, IDC_STATIC_STATE), s);
	}

	int nRecords = 0;
	long lPos = 0;
	_AbacusGetFirstRecordPosition(&lPos);
	while (lPos)
	{
		nRecords++;
		ABACUSRECORD *p;
		if (!_AbacusGetNextRecord(&lPos, &p))
			break;
	}
	int nLastRecNum = _AbacusGetLastRecordNum();
	static int nPrevRecords = 0;
	static int nPrevLastRecNum = 0;
	if (nRecords != nPrevRecords || nLastRecNum != nPrevLastRecNum)
	{
		nPrevRecords = nRecords;
		nPrevLastRecNum = nLastRecNum;
		char szBuff[256];
		sprintf(szBuff, "%d - %d (%d)", s_nLastRecordAdded, nLastRecNum, nRecords);
		::SetDlgItemText(m_hWnd, IDC_STATIC_RECORDS, szBuff);
		ABACUSINIT *pInit;
		if (_AbacusGetInit(&pInit))
		{
			CString s;
			s.Format("%s %s %d/%d/%d %d:%d:%d", pInit->pszDevice,
				pInit->pszVer, (int)pInit->tmMsg.tm_mday, (int)pInit->tmMsg.tm_mon,
				(int)pInit->tmMsg.tm_year, (int)pInit->tmMsg.tm_hour,
				(int)pInit->tmMsg.tm_min, (int)pInit->tmMsg.tm_sec);
			::SetDlgItemText(m_hWnd, IDC_STATIC_INIT, s);
		}
		else
			::SetDlgItemText(m_hWnd, IDC_STATIC_INIT, "??");
	}

	CDHtmlDialog::OnTimer(nIDEvent);
}
