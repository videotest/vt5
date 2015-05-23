// BatchInputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BatchInput.h"
#include "BatchInputDlg.h"
#include "comm.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CString SavePath()
{
	_bstr_t bstr = GetValueString(GetAppUnknown(), "BatchInput", "Path", NULL);
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
		s += _T("BatchInput");
	}
	if (s[s.GetLength()-1] != _T('\\') && s[s.GetLength()-1] != _T('/'))
		s += _T("/");
	_tmkdir((LPCTSTR)s);
	return s;
}

void DoExecute(IActionManager *pActionMan, LPCTSTR pActionName, LPCTSTR pParams)
{
	CString sActionName(pActionName);
	CString sParams(pParams);
	BSTR bstrActionName = sActionName.AllocSysString();
	BSTR bstrParams = sParams.AllocSysString();
	pActionMan->ExecuteAction(bstrActionName, bstrParams, 0);
};

CString CoordData::Format()
{
	if (nError == 0)
	{
		CString s;
		s.Format(_T("%04d %04d"), x, y);
		return s;
	}
	else if (nError == 10)
		return CString(_T("First byte doesn\'t equal FF"));
	else if (nError >= 1 && nError <= 9)
	{
		CString s;
		s.Format(_T("Cannot read byte %d"), nError);
		return s;
	}
	else
		return CString(_T("unknown error"));
}

DWORD g_dwStart;
CoordData ReadCoords()
{
	CoordData Data = {-1,0};
	BYTE b = 1;
	int iX = 0, iY = 0;
	Data.Tm1Beg = GetTickCount();
	if (g_Comm.Read(1, &b))
	{
		Data.Tm1End = GetTickCount();
		if( b != 255 )
		{
			Data.nError = 10;
			return Data;
		}
		b = 2;
		if (g_Comm.Read(1, &b))
		{
			Data.Tm2End = GetTickCount();
			iX = b;
			b = 3;
			if (g_Comm.Read(1, &b))
			{
				Data.Tm3End = GetTickCount();
				iX |= (b<<8);
				b = 4;
				if (g_Comm.Read(1, &b))
				{
					Data.Tm4End = GetTickCount();
					iY = b;
					b = 5;
					if (g_Comm.Read(1, &b))
					{
						Data.Tm5End = GetTickCount();
						iY |= (b<<8);
						Data.nError = 0;
						Data.x = iX;
						Data.y = iY;
						return Data;
					}
				}
			}
		}
	}
	Data.nError = b;
	return Data;
};

void CorrectIntervals(int &nIntervalType, unsigned &nInterval, LONG &nFrames)
{
	if (nIntervalType == 0)
	{
		if (nInterval < 100)
			nInterval = 100;
		if (nInterval > 36000000)
			nInterval = 36000000;
	}
	else
	{
		if (nInterval < 1)
			nInterval = 1;
		if (nInterval > 36000)
			nInterval = 36000;
	}
	if (nFrames < 1)
		nFrames = 1;
//	if (nFrames > 3000)
//		nFrames = 3000;
}

CImageRecord::CImageRecord()
{
	m_bInit = false;
	m_nFrame = -1;
}

CImageRecord::CImageRecord(CImageRecord& s)
{
	m_bInit = s.m_bInit;
	m_CoordData = s.m_CoordData;
	m_Time = s.m_Time;
	m_nFrame = s.m_nFrame;
}

CImageRecord& CImageRecord::operator=(CImageRecord &s)
{
	m_bInit = s.m_bInit;
	m_CoordData = s.m_CoordData;
	m_Time = s.m_Time;
	m_nFrame = s.m_nFrame;
	return *this;
}



BatchInputDlg *g_pBatchInputDlg = NULL;
LONG s_dwStop = 0;
DWORD s_dwNext = 0;
DWORD s_dwPause = 0;
CoordData g_CoordData = {1,0,0};
CRITICAL_SECTION g_cs;
static DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	int nSeqNum = 0;
	g_Comm.Open();
	while (!s_dwStop)
	{
		CoordData Data = ReadCoords();
		Data.nSeqNum = nSeqNum++;
		EnterCriticalSection(&g_cs);
		if (Data.nError == 0)
			g_CoordData = Data;
		if (g_pBatchInputDlg)
		{
			if (g_pBatchInputDlg->m_bUseTrigger)
				g_pBatchInputDlg->OnTryTrigger();
			else
			{
				DWORD dw = GetTickCount()-g_dwStart;
				if (dw >= s_dwNext)
				{
					g_pBatchInputDlg->OnExactTimer(-1);
					if (g_pBatchInputDlg) // will be NULL on last frame!
						s_dwNext += g_pBatchInputDlg->m_nRealInterval;
				}
			}
		}
		LeaveCriticalSection(&g_cs);
	}
	g_Comm.Close();
	return 0;
};

/////////////////////////////////////////////////////////////////////////////
// BatchInputDlg dialog

IMPLEMENT_DYNCREATE(BatchInputDlg, CDialog)

BatchInputDlg::BatchInputDlg()
	: CDialog(BatchInputDlg::IDD, NULL)
{
	m_nCur = 0;
	m_nTimer = 0;
	//{{AFX_DATA_INIT(BatchInputDlg)
	m_nFrames = ::GetValueInt(GetAppUnknown(), "\\BatchInput", "Frames", 1);
	m_nIntervalType = ::GetValueInt(GetAppUnknown(), "\\BatchInput", "IntervalType", 1);
	m_nInterval = ::GetValueInt(GetAppUnknown(), "\\BatchInput", "Interval", 1);
	//}}AFX_DATA_INIT
	m_sPath = SavePath();
	IUnknownPtr punkFF;
	CLSID clsid;
	CLSIDFromProgID(_bstr_t("ImageDoc.BMPFileFilter"),&clsid);
	CoCreateInstance(clsid,NULL,0,IID_IUnknown,(void **)(IUnknown **)&punkFF);
	m_ptrFilter = punkFF;
	m_sName = _T("BatchInputDlg");
	m_sUserName.LoadString(IDS_DLGNAME);
	_OleLockApp( this );
	m_nRealInterval = 0;
	g_dwStart = GetTickCount();
	InitializeCriticalSection(&g_cs);
	m_hThread = CreateThread(NULL, 0, ThreadProc, this, 0, &m_dwThreadId);
	m_Stage = Ready;
	m_bUseMMTimer = ::GetValueInt(GetAppUnknown(),"\\BatchInput","UseMMTimer",1)!=0;
	m_nBufferSize = ::GetValueInt(GetAppUnknown(),"\\BatchInput","BufferSize",10);
	m_nWinTimer = -1;
	m_bSaveImages = true;
	m_bUseTrigger = ::GetValueInt(GetAppUnknown(),"\\BatchInput","TriggeredInput",FALSE);
	m_lTriggerSeqNum = -1;
}

BatchInputDlg::~BatchInputDlg()
{
	InterlockedIncrement(&s_dwStop);
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteCriticalSection(&g_cs);
	_OleUnlockApp( this );
}

void BatchInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(BatchInputDlg)
	DDX_Text(pDX, IDC_E_FRAMES, m_nFrames);
	DDX_Text(pDX, IDC_E_INTERVAL, m_nInterval);
	DDX_CBIndex(pDX, IDC_UNIT, m_nIntervalType);
	DDX_Check(pDX, IDC_CAPTURE_BY_TRIGGER, m_bUseTrigger);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(BatchInputDlg, CDialog)
	//{{AFX_MSG_MAP(BatchInputDlg)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_STOP1, OnStop)
	ON_CBN_SELCHANGE(IDC_UNIT, OnSelchangeUnit)
	ON_EN_CHANGE(IDC_E_FRAMES, OnChangeEFrames)
	ON_EN_CHANGE(IDC_E_INTERVAL, OnChangeEInterval)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER,OnUserMsg)
	ON_MESSAGE(WM_GETINTERFACE, OnGetInterface)
	ON_BN_CLICKED(IDC_PAUSE, OnBnClickedPause)
	ON_BN_CLICKED(IDC_CAPTURE_BY_TRIGGER, OnBnClickedCaptureByTrigger)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()


BEGIN_INTERFACE_MAP(BatchInputDlg, CDialog)
	INTERFACE_PART(BatchInputDlg, IID_IWindow, Wnd)
	INTERFACE_PART(BatchInputDlg, IID_IDockableWindow, Dock)
//	INTERFACE_PART(BatchInputDlg, IID_IRootedObject, Rooted)
//	INTERFACE_PART(BatchInputDlg, IID_IEventListener, EvList)
	INTERFACE_PART(BatchInputDlg, IID_INamedObject2, Name)
	INTERFACE_PART(BatchInputDlg, IID_IHelpInfo, Help)
END_INTERFACE_MAP()

// {34B9715D-E0C4-4A09-B329-EEBD89826591}
GUARD_IMPLEMENT_OLECREATE(BatchInputDlg, _T("BatchInput.BatchInputDlg"), 
0x34b9715d, 0xe0c4, 0x4a09, 0xb3, 0x29, 0xee, 0xbd, 0x89, 0x82, 0x65, 0x91);


/////////////////////////////////////////////////////////////////////////////
// BatchInputDlg message handlers

CWnd *BatchInputDlg::GetWindow()
{
	return this;
}

bool BatchInputDlg::DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID )
{
	Create(IDD, pparent);
	CString s;
	s.LoadString(IDS_DLGNAME);
	SetWindowText(s);
	CRect rect = NORECT;
	GetClientRect(&rect);
	m_size = CSize(rect.Width(), rect.Height());
	m_bFixed = true;
	return true;
}


BOOL BatchInputDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	GetDlgItemText(IDC_PAUSE, m_sPauseTxt);
	m_sResumeTxt.LoadString(IDS_RESUME);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_INTERVAL), !m_bUseTrigger);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_UNIT), !m_bUseTrigger);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void BatchInputDlg::OnSelchangeUnit() 
{
	int n = m_nIntervalType;
	UpdateData();
	if (n == 0 && m_nIntervalType == 1)
	{
		m_nInterval /= 1000;
		if (m_nInterval <= 1) m_nInterval = 1;
		UpdateData(FALSE);
	}
	if (n == 1 && m_nIntervalType == 0)
	{
		m_nInterval *= 1000;
		UpdateData(FALSE);
	}
	SaveSettings();
}

void BatchInputDlg::OnChangeEFrames() 
{
	UpdateData();
	SaveSettings();
}

void BatchInputDlg::OnChangeEInterval() 
{
	UpdateData();
	SaveSettings();
}

void CALLBACK TimeFunc(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	BatchInputDlg *p = (BatchInputDlg *)dwUser;
	p->OnExactTimer(uTimerID);
}

bool BatchInputDlg::PrepareBuffers()
{
	UpdateData();
	CorrectIntervals(m_nIntervalType, m_nInterval, m_nFrames);
	UpdateData(FALSE);
	IUnknownPtr punkCM(_GetOtherComponent(GetAppUnknown(),IID_IDriverManager), false);
	sptrIDriverManager sptrDM(punkCM);
	IUnknownPtr punkDrv;
	TRACE("Starting sequence\n");
	sptrDM->GetCurrentDeviceAndDriver(NULL, &punkDrv, &m_nDev);
	m_Driver = punkDrv;
	m_Driver->GetTriggerMode(0,&m_bPrevTriggerMode);
	m_Driver->SetTriggerMode(0,m_bUseTrigger);
	m_Driver->GetTriggerNotifyMode(0,&m_dwTriggerNotifyMode);
	// Alloc buffer
	m_dwFrameSize = 0;
	HRESULT hr = m_Driver->GetImage(0, NULL, &m_dwFrameSize);
	if (FAILED(hr) || m_dwFrameSize == 0)
	{
		AfxMessageBox(IDS_OPEN_PREVIEW, MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	DWORD dwSize = m_dwFrameSize*min(m_nFrames,m_nBufferSize);
	m_lpData = (LPBYTE)VirtualAlloc(NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
	if (!m_lpData)
	{
		AfxMessageBox("Not enough memory", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	m_Images.SetSize(min(m_nFrames,m_nBufferSize));
	for (int i = 0; i < m_Images.GetSize(); i++)
		m_Images[i].m_bInit = false;
	m_nRealInterval = m_nIntervalType==0?m_nInterval:m_nInterval*1000;
	m_nCur = 0;
	m_bSaveImages = true;
	return true;
}

void BatchInputDlg::BeginCapture()
{
	// Init capturing images
	m_dwStart = GetTickCount()-g_dwStart;
	s_dwPause = 0;
	if (m_bUseTrigger)
		m_Driver->GetImageSequenceNumber(0, &m_lTriggerSeqNum);
	if (m_bUseMMTimer && !m_bUseTrigger)
	{
		InputImage(); // now m_nCur == 1
		if (m_nFrames > 1)
			m_nTimer = timeSetEvent(m_nRealInterval, 10, TimeFunc, (DWORD)this, TIME_PERIODIC|TIME_CALLBACK_FUNCTION);
	}
	else
	{
		EnterCriticalSection(&g_cs);
		s_dwNext = m_dwStart;
		g_pBatchInputDlg = this;
		LeaveCriticalSection(&g_cs);
	}
	if (m_nFrames > m_nBufferSize)
		m_nWinTimer = SetTimer(1, m_nRealInterval, NULL);
	if (m_bUseTrigger)
		m_Driver->GetImageSequenceNumber(0, &m_lTriggerSeqNum);
}

void BatchInputDlg::EndCapture()
{
	if (m_nWinTimer != -1)
	{
		KillTimer(m_nWinTimer);
		m_nWinTimer = -1;
	}
	if (m_bUseMMTimer && !m_bUseTrigger)
	{
		if (m_nTimer) // Stop capture images
			timeKillEvent(m_nTimer);
		m_nTimer = 0;
	}
	else
		g_pBatchInputDlg = NULL; // Called inside critical section.
}

void BatchInputDlg::UnprepareBuffers()
{
	m_Driver->SetTriggerMode(0,m_bPrevTriggerMode);
	m_Driver.Release();
	VirtualFree(m_lpData, 0, MEM_RELEASE);
	m_Images.RemoveAll();
}

void BatchInputDlg::DoStop(bool bSaveImages)
{
	if (bSaveImages)
	{
		CString sMsg;
		sMsg.LoadString(IDS_WRITING_IMAGES);
		SetDlgItemText(IDC_PROGRESS, sMsg);
		SaveImages();
	}
	m_nCur = 0;
	UnprepareBuffers();
	m_Stage = Ready;
	UpdateID();
	UpdateUIOnChangeState();
}

// Capture button pressed. Stage:Ready=>Ready||Ready=>Capturing
void BatchInputDlg::OnStart() 
{
	if (!PrepareBuffers())
		return;
	BeginCapture();
	if (m_nFrames == 1 && !m_bUseTrigger)
		SaveImages(); // Timer not set in this case!
	else
		m_Stage = Capturing;
	UpdateID();
	UpdateUIOnChangeState();
}

// Stop button pressed. Stage:Capturing=>Ready
void BatchInputDlg::OnStop() // Stop button pressed
{
	EndCapture();
	DoStop(m_bSaveImages);
}

// Pause pressed (allowed only on Capturing or Paused). Stage:Capturing=>Paused,Paused=>Capturing
void BatchInputDlg::OnBnClickedPause()
{
	static DWORD dwStartPause = 0;
	if (m_Stage==Capturing)
	{
		m_Stage = Paused;
		dwStartPause = GetTickCount();
	}
	else if (m_Stage==Paused)
	{
		m_Stage = Capturing;
		if (dwStartPause > 0)
		{
			s_dwPause += GetTickCount()-dwStartPause;
			dwStartPause = 0;
		}
		if (m_bUseTrigger)
			m_Driver->GetImageSequenceNumber(0, &m_lTriggerSeqNum);
	}
	UpdateID();
}


// Timer function. Stage:Capturing=>Capturing||Capturing=>Writing
void BatchInputDlg::OnExactTimer(UINT nIDEvent) 
{
	if (m_Stage != Capturing)
		return;
	if (m_nCur < m_nFrames)
		InputImage(); // internally increments m_nCur
	if (m_nCur >= m_nFrames)
	{
		EndCapture();
		m_Stage = Writing;
	}
	PostMessage(WM_USER);
}

void BatchInputDlg::InputImage()
{
	if (m_nCur > m_nFrames)
		return;
	int nCur = m_nCur%m_nBufferSize;
	if (m_Images[nCur].m_bInit == false)
	{
		LPBYTE lpData = NULL;
		DWORD dwSize = 0;
		HRESULT hr = m_Driver->GetImage(0, (void**)&lpData, &dwSize);
		if (hr == S_OK && lpData && dwSize > 0)
			memcpy(m_lpData+m_dwFrameSize*nCur, lpData, min(m_dwFrameSize,dwSize));
		EnterCriticalSection(&g_cs);
		m_Images[nCur].m_CoordData = g_CoordData;
		LeaveCriticalSection(&g_cs);
		m_Images[nCur].m_Time = GetTickCount();
		m_Images[nCur].m_bInit = true;
	}
	m_nCur++;
}

// WM_USER, 0 - used to redraw frames counter
// WM_USER, 1. Stage:Writing=>Ready
LRESULT BatchInputDlg::OnUserMsg(WPARAM wParam, LPARAM lParam)
{
	if (m_Stage == Writing)
	{
		DoStop(true);
		RestoreTriggerMode();
	}
	else
		UpdateID();
	return 0;
}


void BatchInputDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_nWinTimer && m_nFrames > m_nBufferSize)
		SaveImages();
}


void BatchInputDlg::UpdateID()
{
	char szBuf[40];
	if (m_Stage == Capturing || m_Stage == Paused) // If either capture or reading coordinates process still active
		wsprintf(szBuf, "%d / %d", m_nCur, m_nFrames);
	else
		szBuf[0] = 0;
	GetDlgItem(IDC_START)->EnableWindow(m_Stage==Ready);
	GetDlgItem(IDC_STOP1)->EnableWindow(m_Stage==Capturing);
	SetDlgItemText(IDC_PROGRESS, szBuf);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_PAUSE), m_Stage==Capturing||m_Stage==Paused);
	if (m_Stage==Paused)
		SetDlgItemText(IDC_PAUSE, m_sResumeTxt);
	else
		SetDlgItemText(IDC_PAUSE, m_sPauseTxt);
}

void BatchInputDlg::UpdateUIOnChangeState()
{
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_CAPTURE_BY_TRIGGER), m_Stage==Ready);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_INTERVAL), m_Stage==Ready && !m_bUseTrigger);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_UNIT), m_Stage==Ready && !m_bUseTrigger);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_FRAMES), m_Stage==Ready);
}


bool BatchInputDlg::SaveImage(IUnknown *punkRes, LPCTSTR lpstrName)
{
	CImageWrp	image(punkRes);
	BITMAPINFOHEADER	*pbi = image.ExtractDIBBits();
	BITMAPFILEHEADER		bfh;
	ZeroMemory( &bfh, sizeof( bfh ) );
	bfh.bfType = ((int( 'M' )<<8)|int( 'B' ));
	bfh.bfSize = sizeof( bfh )+pbi->biSizeImage;
	bfh.bfOffBits = sizeof( BITMAPFILEHEADER )+sizeof( BITMAPINFOHEADER )+ ((pbi->biBitCount == 24)?0:(1<<pbi->biBitCount<<2));
	CString sError;
	HANDLE hFile = CreateFile(lpstrName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL|FILE_FLAG_NO_BUFFERING, NULL);
	if (hFile == NULL)
	{
		sError.Format(IDS_ERROR_CREATING_FILE, lpstrName, GetLastError());
		AfxMessageBox(sError, MB_OK|MB_ICONEXCLAMATION);
		delete pbi;
		return false;
	}
	DWORD dwSectorsPerCluster, dwBytesPerSector, dwFreeClusters, dwClusters;
	int nError = 0;
	TCHAR szDrive[_MAX_DRIVE];
	_tsplitpath(lpstrName, szDrive, NULL, NULL, NULL);
	if (szDrive[_tcslen(szDrive)-1] != '\\' && szDrive[_tcslen(szDrive)-1] != '/')
		_tcscat(szDrive, _T("\\"));
	if (GetDiskFreeSpace(szDrive, &dwSectorsPerCluster, &dwBytesPerSector, &dwFreeClusters, &dwClusters))
	{
		DWORD dwSize = sizeof(bfh)+pbi->biSizeImage+pbi->biSize;
		dwSize = (dwSize+dwBytesPerSector-1)/dwBytesPerSector*dwBytesPerSector;
		DWORD dwFreeBytes = dwBytesPerSector*dwSectorsPerCluster*dwFreeClusters;
		if (dwSize <= dwFreeBytes)
		{
			LPBYTE lpData = (LPBYTE)VirtualAlloc(NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
			if (lpData)
			{
				memcpy(lpData, &bfh, sizeof(bfh));
				memcpy(lpData+sizeof(bfh),pbi,pbi->biSizeImage+pbi->biSize);
				DWORD dwWritten;
				if (!WriteFile(hFile, lpData, dwSize, &dwWritten, NULL) || dwWritten != dwSize)
					sError.Format(IDS_ERROR_DURING_WRITE_FILE, lpstrName, dwWritten, GetLastError());
				VirtualFree(lpData, 0, MEM_RELEASE);
			}
			else
				sError.LoadString(IDS_NOT_ENOGH_MEMORY);
		}
		else
			sError.Format(IDS_ERROR_NODISKFREESPACE, szDrive, dwFreeBytes, dwSize);
	}
	else
		sError.Format(IDS_ERROR_GETDISKFREESPACE, szDrive, GetLastError());
	CloseHandle(hFile);
	delete pbi;
	if (!sError.IsEmpty())
	{
		AfxMessageBox(sError, MB_OK|MB_ICONASTERISK);
		return false;
	}
	else
		return true;
}

void BatchInputDlg::SaveImages()
{
	static bool bInside = false;
	if (bInside) return;
	bInside = true;
	FILE *f;
	if (GetValueInt(GetAppUnknown(), "\\BatchInput", "Debug", 0))
		f = fopen("c:\\batchinput.log", "at");
	else
		f = NULL;
	for (int i = 0; i < m_Images.GetSize(); i++)
	{
		if (!m_Images[i].m_bInit) continue;
		CImageRecord rec;
		rec = m_Images[i];
		LPBYTE lpData = m_lpData+i*m_dwFrameSize;
		IUnknownPtr punkImg(CreateTypedObject("Image"),false);
		m_Driver->ConvertToNative(0, lpData, m_dwFrameSize, punkImg);
		CString sCoords = rec.m_CoordData.Format();
		if (sCoords.IsEmpty())
			sCoords.Format(_T("error%d"), i);
		CString s = m_sPath+sCoords+_T(".bmp");
		if (!SaveImage(punkImg, s))
		{
			KillTimer(m_nWinTimer);
			m_bSaveImages = false;
			break;
		}
		m_Images[i].m_bInit = false;
		if (f)
		{
			CoordData c = rec.m_CoordData;
			fprintf(f, " %s: byte1 start %d,", (const char *)sCoords, c.Tm1Beg);
			fprintf(f, "byte1 end %d,",	c.Tm1End);
			fprintf(f, "byte2 end %d,",	c.Tm2End);
			fprintf(f, "byte3 end %d,",	c.Tm3End);
			fprintf(f, "byte4 end %d,",	c.Tm4End);
			fprintf(f, "byte5 end %d,",	c.Tm5End);
			fprintf(f, "seq num %d, captured at %d\n", c.nSeqNum, rec.m_Time);
		}
	}
	if (f) fclose(f);
	bInside = false;
}


void BatchInputDlg::SaveSettings()
{
	::SetValue(GetAppUnknown(), "BatchInput", "Frames", (long)m_nFrames);
	::SetValue(GetAppUnknown(), "BatchInput", "IntervalType", (long)m_nIntervalType);
	::SetValue(GetAppUnknown(), "BatchInput", "Interval", (long)m_nInterval);
}

void BatchInputDlg::PostNcDestroy() 
{
	delete this;	
	CDialog::PostNcDestroy();
}

IMPLEMENT_UNKNOWN_BASE(BatchInputDlg, DrvSite);

HRESULT BatchInputDlg::XDrvSite::Invalidate()
{
	METHOD_PROLOGUE_EX(BatchInputDlg, DrvSite);
	if (pThis->m_dwTriggerNotifyMode != 2)
		pThis->OnExactTimer(0);
	return S_OK;
}

HRESULT BatchInputDlg::XDrvSite::OnChangeSize()
{
	return S_OK;
}

void BatchInputDlg::OnBnClickedCaptureByTrigger()
{
	UpdateData();
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_E_INTERVAL), !m_bUseTrigger);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_UNIT), !m_bUseTrigger);
	::SetValue(GetAppUnknown(), "\\BatchInput", "TriggeredInput", (long)m_bUseTrigger);
}

void BatchInputDlg::OnTryTrigger()
{
	if (m_dwTriggerNotifyMode == 2)
	{
		long lTrigSeqNum;
		m_Driver->GetImageSequenceNumber(0, &lTrigSeqNum);
		if (lTrigSeqNum > m_lTriggerSeqNum)
		{
			TRACE("BatchInputDlg::OnTryTrigger, lTrigSeqNum = %d\n", lTrigSeqNum);
			Sleep(40);
			if (m_Stage == Capturing)
				OnExactTimer(0);
			m_lTriggerSeqNum = lTrigSeqNum;
			m_Driver->ResetTrigger(0);
		}
	}
}

void BatchInputDlg::RestoreTriggerMode()
{
	IUnknownPtr punkCM(_GetOtherComponent(GetAppUnknown(),IID_IDriverManager), false);
	sptrIDriverManager sptrDM(punkCM);
	IUnknownPtr punkDrv;
	sptrDM->GetCurrentDeviceAndDriver(NULL, &punkDrv, &m_nDev);
	m_Driver = punkDrv;
	m_Driver->SetTriggerMode(0,0);
}

void BatchInputDlg::OnBnClickedButton1()
{
	IUnknownPtr punkCM(_GetOtherComponent(GetAppUnknown(),IID_IDriverManager), false);
	sptrIDriverManager sptrDM(punkCM);
	IUnknownPtr punkDrv;
	sptrDM->GetCurrentDeviceAndDriver(NULL, &punkDrv, &m_nDev);
	m_Driver = punkDrv;
	m_Driver->SetTriggerMode(0,0);
}
