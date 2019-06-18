// PeriodicDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Periodic.h"
#include "PeriodicDlg.h"
#include "Input.h"
#include "Comm.h"
#include "misc5.h"
#include <direct.h>
#include "InfoField.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void DoExecute(LPCTSTR pActionName, LPCTSTR pParams)
{
	IUnknownPtr punkAM(_GetOtherComponent(GetAppUnknown(), IID_IActionManager),false);
	IActionManagerPtr sptrAM(punkAM);
	CString sActionName(pActionName);
	CString sParams(pParams);
	BSTR bstrActionName = sActionName.AllocSysString();
	BSTR bstrParams = sParams.AllocSysString();
	sptrAM->ExecuteAction(bstrActionName, bstrParams, 0);
};


CString SavePath()
{
	_bstr_t bstr = GetValueString(GetAppUnknown(), "Periodic", "Path", NULL);
	CString s((const wchar_t *)bstr);
	if (s.IsEmpty())
	{
		TCHAR szPath[_MAX_PATH];
		GetModuleFileName(NULL, szPath, _MAX_PATH);
		TCHAR szDrive[_MAX_DRIVE],szDir[_MAX_DIR];
		_tsplitpath(szPath, szDrive, szDir, NULL, NULL);
		_tmakepath(szPath, szDrive, szDir, NULL, NULL);
		s = szPath;
		if (s[s.GetLength()-1] != _T('\\') && s[s.GetLength()-1] != _T('/'))
			s += _T("/");
		s += _T("Images");
	}
	if (s[s.GetLength()-1] != _T('\\') && s[s.GetLength()-1] != _T('/'))
		s += _T("/");
	_tmkdir((LPCTSTR)s);
	return s;
}


CDeviceRecord::CDeviceRecord()
{
	m_nDevice = -1;
}

CDeviceRecord::CDeviceRecord(CDeviceRecord &s)
{
	m_sptrDrv = s.m_sptrDrv;
	m_nDevice = s.m_nDevice;
	m_dwFrameSize = s.m_dwFrameSize;
}

CDeviceRecord& CDeviceRecord::operator=(CDeviceRecord &s)
{
	m_sptrDrv = s.m_sptrDrv;
	m_nDevice = s.m_nDevice;
	m_dwFrameSize = s.m_dwFrameSize;
	return *this;
}

bool CDevices::Init()
{
	RemoveAll();
	CStringArray sa;
	for (int i = 0; ; i++)
	{
		CString sEntry;
		sEntry.Format(_T("Device%d"), i);
		CString s = GetValueString(GetAppUnknown(), "\\Periodic", sEntry, NULL);
		if (s.IsEmpty())
			break;
		sa.Add(s);
	}
	if (sa.GetSize() == 0)
	{
		if( GetValueInt( GetAppUnknown(), "\\Periodic", "WarningNoDevice", 1L ) == 1L )
			AfxMessageBox(IDS_NO_DEVICES, MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	IUnknownPtr punkCM(_GetOtherComponent(GetAppUnknown(),IID_IDriverManager), false);
	sptrIDriverManager sptrDM(punkCM);
	ICompManagerPtr sptrCM(punkCM);
	int nDevices;
	sptrDM->GetDevicesCount(&nDevices);
	for (i = 0; i < sa.GetSize(); i++)
	{
		// Find device
		CString sFind = sa[i];
		int nDriver,nDevInDriver;
		for (int j = 0; j < nDevices; j++)
		{
			BSTR bstrSName;
			sptrDM->GetDeviceInfo(j, &bstrSName, NULL, NULL, &nDriver, &nDevInDriver);
			CString sCurDev(bstrSName);
			::SysFreeString(bstrSName);
			if (sCurDev == sFind)
				break;
		}
		if (j == nDevices) // Device not fond
		{
			CString s;
			s.Format(IDS_DEVICE_NOT_FOUND, (LPCTSTR)sFind);
			AfxMessageBox(s, MB_OK|MB_ICONEXCLAMATION);
			RemoveAll();
			return false;
		}
		// Device found in list, initialize record
		CDeviceRecord DevRec;
		IUnknownPtr punkDrv;
		sptrCM->GetComponentUnknownByIdx(nDriver, &punkDrv);
		IDriver3Ptr sptrDrv3(punkDrv);
		if (sptrDrv3 == 0)
		{
			CString s;
			s.Format(IDS_INVALID_DEVICE, (LPCTSTR)sFind);
			AfxMessageBox(s, MB_OK|MB_ICONEXCLAMATION);
			RemoveAll();
			return false;
		}
		DevRec.m_sptrDrv = sptrDrv3;
		DevRec.m_nDevice = nDevInDriver;
		Add(DevRec);
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// CPeriodicDlg dialog

IMPLEMENT_DYNCREATE(CPeriodicDlg, CDialog)

BEGIN_INTERFACE_MAP(CPeriodicDlg, CDialog)
	INTERFACE_PART(CPeriodicDlg, IID_IWindow, Wnd)
	INTERFACE_PART(CPeriodicDlg, IID_IDockableWindow, Dock)
	INTERFACE_PART(CPeriodicDlg, IID_INamedObject2, Name)
	INTERFACE_PART(CPeriodicDlg, IID_IEventListener, EvList)
	INTERFACE_PART(CPeriodicDlg, IID_IDrawObject, Draw)
	INTERFACE_PART(CPeriodicDlg, IID_IDrawObject2, Draw)
END_INTERFACE_MAP()

// {568177EA-4217-45E1-889B-99936A6CBFF9}
GUARD_IMPLEMENT_OLECREATE(CPeriodicDlg, _T("Periodic.PeriodicDlg"), 
0x568177ea, 0x4217, 0x45e1, 0x88, 0x9b, 0x99, 0x93, 0x6a, 0x6c, 0xbf, 0xf9);


CPeriodicDlg::CPeriodicDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPeriodicDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPeriodicDlg)
	m_nFrames = GetValueInt(GetAppUnknown(), "Periodic", "Frames", 1);
	m_nInterval = GetValueInt(GetAppUnknown(), "Periodic", "Interval", 1);
	m_nType = GetValueInt(GetAppUnknown(), "Periodic", "Type", 0);
	//}}AFX_DATA_INIT
//	m_nComPeriod = GetValueInt(GetAppUnknown(), "Periodic", "CommQuerryPeriod", 1000);
	m_sName = _T("PeriodicDlg");
	m_sUserName.LoadString(IDS_DLGNAME);
	_OleLockApp( this );
	InitializeCriticalSection(&g_cs);
	m_nTimerCap = m_nTimerComm =0;
	m_lpBuffer = NULL;
}

CPeriodicDlg::~CPeriodicDlg()
{
	delete m_lpBuffer;
	UnRegister(GetAppUnknown());
	_OleUnlockApp( this );
}


CWnd *CPeriodicDlg::GetWindow()
{
	return this;
}

bool CPeriodicDlg::DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	Create(IDD, pparent);
	CString s;
	s.LoadString(IDS_DLGNAME);
	SetWindowText(s);
	CRect	rect = NORECT;
	GetClientRect(&rect);
	m_size = CSize(rect.Width(), rect.Height());
	m_bFixed = true;
	return true;
}

// {C0D5CB20-6ABF-4DCC-ADBA-9E9C998965A8}
static const IID IID_IIPreviewView =
{ 0xc0d5cb20, 0x6abf, 0x4dcc, { 0xad, 0xba, 0x9e, 0x9c, 0x99, 0x89, 0x65, 0xa8 } };
void CPeriodicDlg::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if (strcmp(pszEvent, szAppCreateView) == 0)
	{
		if (punkFrom && ::CheckInterface(punkFrom, IID_IIPreviewView))
		{
			IWindow2Ptr sptrWnd2(punkFrom);
			if (sptrWnd2 != 0)
			{
				sptrWnd2->AttachDrawing(GetControllingUnknown());
			}
		}
	}
	if (strcmp(pszEvent, szAppDestroyView) == 0)
	{
		if (punkFrom && ::CheckInterface(punkFrom, IID_IIPreviewView))
		{
			IWindow2Ptr sptrWnd2(punkFrom);
			if (sptrWnd2 != 0)
			{
				sptrWnd2->DetachDrawing(GetControllingUnknown());
			}
		}
	}
}

void CPeriodicDlg::DoDraw(CDC &dc)
{
	sptrIScrollZoomSite	sptrS(m_punkDrawTarget);
	CSize sizeNZ;
	sptrS->GetClientSize(&sizeNZ);
	CPoint ptPos;
	sptrS->GetScrollPos(&ptPos);
	double dZoom;
	sptrS->GetZoom(&dZoom);
	CRect rcDst(-ptPos.x,-ptPos.y,int(sizeNZ.cx*dZoom)-ptPos.x,int(sizeNZ.cy*dZoom)-ptPos.y);
	CString s = FormatCoordinates();
	dc.SetTextColor(m_clrText);
	dc.DrawText(s, -1, rcDst, DT_SINGLELINE|DT_RIGHT|DT_BOTTOM);
}

void CPeriodicDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPeriodicDlg)
	DDX_Text(pDX, IDC_E_FRAMES, m_nFrames);
	DDX_Text(pDX, IDC_E_INTERVAL, m_nInterval);
	DDX_Radio(pDX, IDC_TIME, m_nType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPeriodicDlg, CDialog)
	//{{AFX_MSG_MAP(CPeriodicDlg)
	ON_EN_CHANGE(IDC_E_FRAMES, OnChangeEFrames)
	ON_EN_CHANGE(IDC_E_INTERVAL, OnChangeEInterval)
	ON_BN_CLICKED(IDC_TIME, OnTime)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_STOP1, OnStop1)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BOTH, OnBoth)
	ON_BN_CLICKED(IDC_SIGNAL, OnSignal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPeriodicDlg message handlers

BOOL CPeriodicDlg::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDialog::OnInitDialog();
	
	if (m_Devices.Init())
	{
		g_pThread = new CComThread;
		s_bRun = 1;
		g_pThread->CreateThread();
	}
	else
	{
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_INTERVAL), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_FRAMES), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_TIME), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_SIGNAL), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_BOTH), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_START), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_STOP1), FALSE);
	}
	Register(GetAppUnknown(), NULL);
	m_clrText = ::GetValueColor(GetAppUnknown(), "Editor", "CommentText_Fore", RGB(0,255,0));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CPeriodicDlg::OnChangeEFrames() 
{
	UpdateData();
	SetValue(GetAppUnknown(), "Periodic", "Frames", (long)m_nFrames);
}

void CPeriodicDlg::OnChangeEInterval() 
{
	UpdateData();
	SetValue(GetAppUnknown(), "Periodic", "Interval", (long)m_nInterval);
}

void CPeriodicDlg::OnTime() 
{
	UpdateData();
	SetValue(GetAppUnknown(), "Periodic", "Type", (long)m_nType);
}

void CPeriodicDlg::OnBoth() 
{
	UpdateData();
	SetValue(GetAppUnknown(), "Periodic", "Type", (long)m_nType);
}

void CPeriodicDlg::OnSignal() 
{
	UpdateData();
	SetValue(GetAppUnknown(), "Periodic", "Type", (long)m_nType);
}

void CPeriodicDlg::OnStart() 
{
	UpdateData();
	m_nCur = 0;
	m_nAllFramesSize = 0;
	for (int i = 0; i < m_Devices.GetSize(); i++)
	{
		CDeviceRecord &DevRec = m_Devices[i];
		IInputPreview2Ptr sptrPrv2(DevRec.m_sptrDrv);
		if (sptrPrv2 != 0)
			sptrPrv2->SetPreviewMode(DevRec.m_nDevice, Preview_Full);
		DevRec.m_sptrDrv->StartImageAquisition(DevRec.m_nDevice, TRUE);
		DevRec.m_sptrDrv->GetImage(DevRec.m_nDevice, NULL, &DevRec.m_dwFrameSize);
		m_nAllFramesSize += DevRec.m_dwFrameSize+sizeof(CommData);
	}
	m_lpBuffer = new BYTE[m_nAllFramesSize*m_nFrames];
	if (m_nType == 0 || m_nType == 2)
		m_nTimerCap = SetTimer(1, m_nInterval*1000, NULL);
	if (m_nType == 1 || m_nType == 2)
		m_nTimerComm = SetTimer(2, 250, NULL); 
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_INTERVAL), FALSE);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_FRAMES), FALSE);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_TIME), FALSE);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_SIGNAL), FALSE);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_BOTH), FALSE);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_START), FALSE);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_STOP1), TRUE);
}

void CPeriodicDlg::OnStop1() 
{
	if (m_nTimerCap) KillTimer(m_nTimerCap);
	if (m_nTimerComm) KillTimer(m_nTimerComm);
	::SetWindowText(::GetDlgItem(m_hWnd, IDC_PROGRESS), _T("Saving  images"));
	m_nTimerCap = m_nTimerComm =0;
	OpenImages();
	delete m_lpBuffer;
	m_lpBuffer = NULL;
	for (int i = 0; i < m_Devices.GetSize(); i++)
	{
		CDeviceRecord &DevRec = m_Devices[i];
		IInputPreview2Ptr sptrPrv2(DevRec.m_sptrDrv);
		DevRec.m_sptrDrv->StartImageAquisition(DevRec.m_nDevice, FALSE);
		if (sptrPrv2 != 0)
			sptrPrv2->SetPreviewMode(DevRec.m_nDevice, Preview_Fast);
	}
	::SetWindowText(::GetDlgItem(m_hWnd, IDC_PROGRESS), _T(""));
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_INTERVAL), TRUE);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_FRAMES), TRUE);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_TIME), TRUE);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_SIGNAL), TRUE);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_BOTH), TRUE);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_START), TRUE);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_STOP1), FALSE);
}


void CPeriodicDlg::OnDestroy() 
{
	if (g_pThread)
	{
		HANDLE h;
		ASSERT(g_pThread->m_bAutoDelete);
		BOOL b = DuplicateHandle(GetCurrentProcess(), g_pThread->m_hThread, GetCurrentProcess(), &h, SYNCHRONIZE, FALSE, 0);
		s_bRun = 0;
		if (b)
			WaitForSingleObject(h, 30000);
	}
	UnRegister(GetAppUnknown(), NULL);
//	KillTimer(m_nTimerComm);
	CDialog::OnDestroy();
}

void CPeriodicDlg::OnTimer(UINT_PTR nIDEvent) 
{
	if (nIDEvent == m_nTimerCap)
	{
		InputImages();
	}
	if (nIDEvent == m_nTimerComm)
	{
		if (s_bCapture)
		{
			InputImages();
			s_bCapture = 0;
		}
	}
	if (m_nCur >= m_nFrames)
		OnStop1();

	CDialog::OnTimer(nIDEvent);
}

void CPeriodicDlg::InputImages()
{
	LPBYTE lpFrames = m_lpBuffer+m_nCur*m_nAllFramesSize;
	for (int i = 0; i < m_Devices.GetSize(); i++)
	{
		*((CommData *)lpFrames) = g_CommData;
		lpFrames += sizeof(CommData);
		CDeviceRecord &DevRec = m_Devices[i];
		LPBYTE lpData;
		DWORD dwSize;
		DevRec.m_sptrDrv->GetImage(DevRec.m_nDevice, (void**)&lpData, &dwSize);
		memcpy(lpFrames, lpData, min(dwSize,DevRec.m_dwFrameSize));
		lpFrames += DevRec.m_dwFrameSize;
	}
	m_nCur++;
	TCHAR szBuff[20];
	_stprintf(szBuff, _T("%d/%d"), m_nCur, m_nFrames);
	::SetWindowText(::GetDlgItem(m_hWnd, IDC_PROGRESS), szBuff);
}

void CPeriodicDlg::OpenImages()
{
	CString sPath = SavePath();
	CString sExt = GetValueString(GetAppUnknown(), "Periodic", "Extension", ".image");
	if (!sExt.IsEmpty() && sExt[0] != '.')
		sExt = CString(_T("."))+sExt;
	for (int i = 0; i < m_nCur; i++)
	{
		// For each device set saved image
		LPBYTE lpFrames = m_lpBuffer+i*m_nAllFramesSize;
		for (int j = 0; j < m_Devices.GetSize(); j++)
		{
			// Save coordinates for image
			CommData Data = *((CommData *)lpFrames);
			lpFrames += sizeof(CommData);
			// Create new image document
			DoExecute(_T("FileNew"), _T("\"Image\""));
			IApplicationPtr sptrApp(GetAppUnknown());
			IUnknownPtr punkDoc;
			sptrApp->GetActiveDocument(&punkDoc);
			// Set saved image into document
			CDeviceRecord &DevRec = m_Devices[j];
			IUnknownPtr punkImg(CreateTypedObject("Image"),false);
			DevRec.m_sptrDrv->ConvertToNative(DevRec.m_nDevice, lpFrames, DevRec.m_dwFrameSize, punkImg);
			_variant_t var((IUnknown *)punkImg);
			::SetValue(punkDoc, 0, 0, var);
			lpFrames += DevRec.m_dwFrameSize;
			// Mark document as modified
			IFileDataObject2Ptr	ptrFileObject(punkDoc);
			if (ptrFileObject != 0)
				ptrFileObject->SetModifiedFlag(TRUE);
			// Select image in all views (usually only one view)
			IDocumentSitePtr sptrDS = punkDoc;
			if (sptrDS == 0)
				continue;
			long lPos = 0;
			sptrDS->GetFirstViewPosition(&lPos);
			while (lPos)
			{
				IUnknownPtr sptr;
				sptrDS->GetNextView(&sptr, &lPos);
				sptrIDataContext sptrDC(sptr);
				sptrDC->SetActiveObject(_bstr_t(szTypeImage), punkImg, 0);
				IViewPtr sptrV(sptr);
				sptrV->OnUpdate(_bstr_t(szEventActiveObjectChange), NULL);
			}
			// Add commentary object
			CString s;
			s.Format(_T("%d:%d"), (int)Data.x, (int)Data.y);
			AddObject(punkDoc, s);
			// Save document in disk file
			CString sName;
			sName.Format(_T("Image%dCam%d"), i, j);
			sName = CString(_T("\""))+sPath+sName+sExt+CString(_T("\""));
			DoExecute(_T("FileSave"), sName);
		}
	}
}


void CPeriodicDlg::PostNcDestroy() 
{
	delete this;	
	CDialog::PostNcDestroy();
}

