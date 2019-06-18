#include "StdAfx.h"
#include "NewDoc.h"
#include "Trigger.h"
#include "Input.h"
#include "InputUtils.h"
#include <direct.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static IUnknownPtr __GetCurDriver(int *pnDev = NULL)
{
	IUnknownPtr punkCM(_GetOtherComponent(GetAppUnknown(),IID_IDriverManager), false);
	sptrIDriverManager sptrDM(punkCM);
	int nDev;
	IUnknownPtr punkD;
	sptrDM->GetCurrentDeviceAndDriver(NULL, &punkD, &nDev);
	if (pnDev) *pnDev = nDev;
	return punkD;
}

static DWORD __GetCurDriverFlags()
{
	sptrIDriver sptrD(__GetCurDriver());
	DWORD dwFlags;
	sptrD->GetFlags(&dwFlags);
	return dwFlags;
}

bool __GetTriggerMode()
{
	int nDev;
	IDriver3Ptr sptrDrv(__GetCurDriver(&nDev));
	if (sptrDrv == 0) return false;
	BOOL b;
	HRESULT hr = sptrDrv->GetTriggerMode(nDev,&b);
	return SUCCEEDED(hr)&&b;
}

void __SetTriggerMode(bool bSet)
{
	int nDev;
	IDriver3Ptr sptrDrv(__GetCurDriver(&nDev));
	if (sptrDrv == 0) return;
	sptrDrv->SetTriggerMode(nDev,bSet?TRUE:FALSE);
}


IMPLEMENT_DYNCREATE(CActionDriverDlg, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionTriggerMode, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionSoftwareTrigger, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionTriggerSequence, CCmdTargetEx);


// {0E5FDFDE-1BF8-4DEE-89F4-D3398C68CF6F}
GUARD_IMPLEMENT_OLECREATE(CActionDriverDlg, "input.DriverDlg", 
0xe5fdfde, 0x1bf8, 0x4dee, 0x89, 0xf4, 0xd3, 0x39, 0x8c, 0x68, 0xcf, 0x6f);

// {90CCAD42-7234-4C2F-B720-B79F9F078BCE}
GUARD_IMPLEMENT_OLECREATE(CActionTriggerMode, "input.TriggerMode", 
0x90ccad42, 0x7234, 0x4c2f, 0xb7, 0x20, 0xb7, 0x9f, 0x9f, 0x7, 0x8b, 0xce);

// {AFDFDBC2-875A-44B6-9CC4-36E4FDD8080E}
GUARD_IMPLEMENT_OLECREATE(CActionSoftwareTrigger, "input.SoftwareTrigger", 
0xafdfdbc2, 0x875a, 0x44b6, 0x9c, 0xc4, 0x36, 0xe4, 0xfd, 0xd8, 0x8, 0xe);

// {52E1B138-08F5-4D11-80C7-F75DC8F7C005}
GUARD_IMPLEMENT_OLECREATE(CActionTriggerSequence, "input.TriggerSequence", 
0x52e1b138, 0x8f5, 0x4d11, 0x80, 0xc7, 0xf7, 0x5d, 0xc8, 0xf7, 0xc0, 0x5);


// {B6104061-DFDE-4705-A67D-58E4AEC582EC}
static const GUID guidDriverDlg = 
{ 0xb6104061, 0xdfde, 0x4705, { 0xa6, 0x7d, 0x58, 0xe4, 0xae, 0xc5, 0x82, 0xec } };

// {1C1EF8C2-57DB-4537-9379-1A14346CF631}
static const GUID guidTriggerMode = 
{ 0x1c1ef8c2, 0x57db, 0x4537, { 0x93, 0x79, 0x1a, 0x14, 0x34, 0x6c, 0xf6, 0x31 } };

// {F023B1CA-94DE-49C4-87FC-A465211B9FAA}
static const GUID guidSoftwareTrigger = 
{ 0xf023b1ca, 0x94de, 0x49c4, { 0x87, 0xfc, 0xa4, 0x65, 0x21, 0x1b, 0x9f, 0xaa } };

// {FFE54773-8B39-479F-8762-3E75E06426AB}
static const GUID guidTriggerSequence = 
{ 0xffe54773, 0x8b39, 0x479f, { 0x87, 0x62, 0x3e, 0x75, 0xe0, 0x64, 0x26, 0xab } };

ACTION_INFO_FULL(CActionDriverDlg, ID_ACTION_DRIVERDLG, IDS_MENU_INPUT_IMAGE, IDS_TB_MAIN, guidDriverDlg);
ACTION_INFO_FULL(CActionTriggerMode, ID_ACTION_TRIGGERMODE, IDS_MENU_INPUT_IMAGE, IDS_TB_MAIN, guidTriggerMode);
ACTION_INFO_FULL(CActionSoftwareTrigger, ID_ACTION_DO_TRIGGER, IDS_MENU_INPUT_IMAGE, IDS_TB_MAIN, guidSoftwareTrigger);
ACTION_INFO_FULL(CActionTriggerSequence, ID_ACTION_TRIGGER_SEQUENCE, IDS_MENU_INPUT_IMAGE, IDS_TB_MAIN, guidTriggerSequence);

ACTION_TARGET(CActionDriverDlg, szTargetApplication);
ACTION_TARGET(CActionTriggerMode, szTargetApplication);
ACTION_TARGET(CActionSoftwareTrigger, szTargetApplication);
ACTION_TARGET(CActionTriggerSequence, szTargetApplication);

ACTION_ARG_LIST(CActionDriverDlg)
	ARG_STRING( _T("DialogName"), _T(""))
END_ACTION_ARG_LIST();
ACTION_ARG_LIST(CActionTriggerMode)
END_ACTION_ARG_LIST();
ACTION_ARG_LIST(CActionSoftwareTrigger)
END_ACTION_ARG_LIST();
ACTION_ARG_LIST(CActionTriggerSequence)
END_ACTION_ARG_LIST();

//////////////////////////////////////////////////////////////////////
//CActionDriverDlg implementation

CActionDriverDlg::CActionDriverDlg()
{
}

CActionDriverDlg::~CActionDriverDlg()
{
}

bool CActionDriverDlg::Invoke()
{
	CString s = GetArgumentString("DialogName");
	int nDev;
	IDriverPtr sptrDrv(__GetCurDriver(&nDev));
	HRESULT hr = sptrDrv->ExecuteDriverDialog(nDev, s);
	return SUCCEEDED(hr);
}

bool CActionDriverDlg::IsAvaible()
{
	IDriverPtr sptrDrv(__GetCurDriver());
	return sptrDrv!=0;
}


//////////////////////////////////////////////////////////////////////
//CActionTriggerMode implementation
CActionTriggerMode::CActionTriggerMode()
{
}

CActionTriggerMode::~CActionTriggerMode()
{
}

bool CActionTriggerMode::Invoke()
{
	bool bTM = __GetTriggerMode();
	__SetTriggerMode(!bTM);
	return true;
}

bool CActionTriggerMode::IsAvaible()
{
	return __GetCurDriverFlags()&FG_TRIGGERMODE?true:false;
}

bool CActionTriggerMode::IsChecked()
{
	return __GetTriggerMode();
}


//////////////////////////////////////////////////////////////////////
//CActionSoftwareTrigger implementation

CActionSoftwareTrigger::CActionSoftwareTrigger()
{
}

CActionSoftwareTrigger::~CActionSoftwareTrigger()
{
}

bool CActionSoftwareTrigger::Invoke()
{
	int nDev;
	IDriver3Ptr sptrDrv(__GetCurDriver(&nDev));
	if (sptrDrv == 0) return false;
	sptrDrv->DoSoftwareTrigger(nDev);
	return true;
}

bool CActionSoftwareTrigger::IsAvaible()
{
	return (__GetCurDriverFlags()&FG_TRIGGERMODE)!=0 && __GetTriggerMode();
}


//////////////////////////////////////////////////////////////////////
//CActionTriggerSequence implementation

CActionTriggerSequence::CActionTriggerSequence()
{
}

CActionTriggerSequence::~CActionTriggerSequence()
{
}


static void __GetActiveFrame(HWND &hwndMDI, HWND &hwndChild)
{
	hwndMDI = hwndChild = NULL;
	IApplicationPtr sptrApp(GetAppUnknown());
	IUnknownPtr punkDoc,punkView,punkFrame;
	sptrApp->GetActiveDocument(&punkDoc);
	IDocumentSitePtr sptrDS(punkDoc);
	if (sptrDS==0) return;
	sptrDS->GetActiveView(&punkView);
	IViewSitePtr sptrVS(punkView);
	if (sptrVS==0) return;
	sptrVS->GetFrameWindow(&punkFrame);
	IWindowPtr sptrW(punkFrame);
	if (sptrW==0) return;
	sptrW->GetHandle((HANDLE*)&hwndChild);
	hwndMDI = ::GetParent(hwndChild);
}

bool CActionTriggerSequence::Invoke()
{

	int nDev;
	IDriver3Ptr sptrDrv(__GetCurDriver(&nDev));
	BOOL bPrevTM,bPrevA;
	sptrDrv->GetTriggerMode(nDev,&bPrevTM);
	if (!bPrevTM) sptrDrv->SetTriggerMode(nDev,TRUE);
	sptrDrv->IsImageAquisitionStarted(nDev,&bPrevA);
	if (!bPrevA) sptrDrv->StartImageAquisition(nDev,TRUE);
	HWND hwndChild,hwndMDI;
	__GetActiveFrame(hwndMDI, hwndChild);
	CTriggerSeqDlg dlg(sptrDrv, nDev);
	dlg.DoModal();
	if (hwndMDI && hwndChild)
		::SendMessage(hwndMDI, WM_MDIACTIVATE, (WPARAM)hwndChild, 0);
	if (!bPrevA) sptrDrv->StartImageAquisition(nDev,bPrevA);
	if (!bPrevTM) sptrDrv->SetTriggerMode(nDev,bPrevTM);
	return true;
}

bool CActionTriggerSequence::IsAvaible()
{
	return __GetCurDriverFlags()&FG_TRIGGERMODE?true:false;
}


/////////////////////////////////////////////////////////////////////////////
// CTriggerSeqDlg dialog


CTriggerSeqDlg::CTriggerSeqDlg(IDriver3 *ptrDrv, int nDev, CWnd* pParent /*=NULL*/)
	: CDialog(CTriggerSeqDlg::IDD, pParent)
{
	m_ptrDrv = ptrDrv;
	m_nDev = nDev;
	InitializeCriticalSection(&m_cs);
	//{{AFX_DATA_INIT(CTriggerSeqDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTriggerSeqDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTriggerSeqDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTriggerSeqDlg, CDialog)
	//{{AFX_MSG_MAP(CTriggerSeqDlg)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTriggerSeqDlg message handlers

BOOL CTriggerSeqDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	DWORD dwTimeout = GetValueInt(GetAppUnknown(), "Input\\Trigger", "TriggerSequence", 3)*1000;
	m_dwTicksEnd = dwTimeout?GetTickCount()+dwTimeout:0;
	m_nImage = 0;
	m_nMaxImages = GetValueInt(GetAppUnknown(), "Input\\Trigger", "MaxImages", 15);
	SetTimer(1, 300, NULL);
	m_ptrDrv->AddDriverSite(m_nDev,this);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTriggerSeqDlg::OnDestroy() 
{
	m_ptrDrv->RemoveDriverSite(m_nDev,this);
	KillTimer(1);
	SaveImages();	
	CDialog::OnDestroy();
}

void CTriggerSeqDlg::OnTimer(UINT_PTR nIDEvent) 
{
	if (m_dwTicksEnd && GetTickCount() >= m_dwTicksEnd)
		PostMessage(WM_CLOSE);
	SaveImages();	
	CDialog::OnTimer(nIDEvent);
}

HRESULT CTriggerSeqDlg::Invalidate()
{
	if (m_nMaxImages == 0 || m_nImage++ < m_nMaxImages)
	{
		LPVOID lpData;
		DWORD dwSize;
		m_ptrDrv->GetImage(m_nDev,&lpData, &dwSize);
		LPVOID lpNew = malloc(dwSize);
		memcpy(lpNew, lpData, dwSize);
		EnterCriticalSection(&m_cs);
		m_arrFrames.Add(lpNew);
		LeaveCriticalSection(&m_cs);
	}
	return S_OK;
}

static CString SavePath()
{
	_bstr_t bstr = GetValueString(GetAppUnknown(), "Input\\Trigger", "SequencePath", NULL);
	CString s((const wchar_t *)bstr);
	if (s.IsEmpty())
	{
		TCHAR szPath[_MAX_PATH];
		GetModuleFileName(AfxGetInstanceHandle(), szPath, _MAX_PATH);
		TCHAR szDrive[_MAX_DRIVE],szDir[_MAX_DIR];
		_tsplitpath(szPath, szDrive, szDir, NULL, NULL);
		_tmakepath(szPath, szDrive, szDir, NULL, NULL);
		s = szPath;
		if (s[s.GetLength()-1] != _T('\\') && s[s.GetLength()-1] != _T('/'))
			s += _T("/");
		s += _T("TriggerSequence");
	}
	if (s[s.GetLength()-1] != _T('\\') && s[s.GetLength()-1] != _T('/'))
		s += _T("/");
	_tmkdir(s);
	return s;
}


void CTriggerSeqDlg::SaveImages()
{
	CString sPath = SavePath();
	while(1)
	{
		LPVOID lpData = NULL;
		EnterCriticalSection(&m_cs);
		if (m_arrFrames.GetSize() > 0)
		{
			lpData = m_arrFrames.GetAt(0);
			m_arrFrames.RemoveAt(0);
		}
		LeaveCriticalSection(&m_cs);
		if (lpData == NULL) break;
		IUnknownPtr punkImg(CreateTypedObject("Image"),false);
		m_ptrDrv->ConvertToNative(m_nDev,lpData, _msize(lpData), punkImg);
		free(lpData);
		IUnknownPtr punkAM(_GetOtherComponent(GetAppUnknown(), IID_IActionManager),false);
		IActionManagerPtr sptrAM(punkAM);
		DoExecute(sptrAM, _T("FileNew"), _T("\"Image\""));
		IApplicationPtr sptrApp(GetAppUnknown());
		IUnknownPtr punkDoc;
		sptrApp->GetActiveDocument(&punkDoc);
		_variant_t var((IUnknown *)punkImg);
		::SetValue(punkDoc, 0, 0, var);
		IFileDataObject2Ptr	ptrFileObject(punkDoc);
		if (ptrFileObject != 0)
			ptrFileObject->SetModifiedFlag(TRUE);
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
		if (GetValueInt(GetAppUnknown(), "Input\\Trigger", "AutomaticallySave", TRUE))
		{
			static int iNum=0;
			CString s;
			s.Format(_T("Shot%d.bmp"), ++iNum);
			s = CString(_T("\""))+sPath+s+CString(_T("\""));
			DoExecute(sptrAM, _T("FileSave"), s);
		}
	}
}
