// ActionAbacusToDB.cpp : Realization of Abacus interface.
//

#include "stdafx.h"
#include "AbacusUI.h"
#include "ActionAbacusToDB.h"
#include "AbacusWork.h"
#include "Utils.h"
#include "dbase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static int s_nLastAbacusRecord = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////
//CActionAbacusConnect
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionAbacusConnect, CCmdTargetEx);

// {7CCEE61A-A14B-4403-8FA0-50C736C5F2CA}
GUARD_IMPLEMENT_OLECREATE(CActionAbacusConnect, _T("AbacusUI.ActionAbacusConnect"), 
0x7ccee61a, 0xa14b, 0x4403, 0x8f, 0xa0, 0x50, 0xc7, 0x36, 0xc5, 0xf2, 0xca);


// {71CC0475-342F-4085-B72F-F5D508BD32F5}
static const GUID guidActionAbacusConnect = 
{ 0x71cc0475, 0x342f, 0x4085, { 0xb7, 0x2f, 0xf5, 0xd5, 0x8, 0xbd, 0x32, 0xf5 } };


ACTION_INFO_FULL(CActionAbacusConnect, ID_ACTION_ABACUS_CONNECT, -1, -1, guidActionAbacusConnect);
ACTION_TARGET(CActionAbacusConnect, szTargetApplication);
ACTION_ARG_LIST(CActionAbacusConnect)
	ARG_INT("Connect", 1)
END_ACTION_ARG_LIST();

bool CActionAbacusConnect::s_bConnected = false;
bool CActionAbacusConnect::s_bAutoExecute = true;
static int s_nTimerId = 0;

void CALLBACK EXPORT TimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	DO_LOCK();
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IUnknownPtr punkApp(GetAppUnknown());
	IApplicationPtr sptrApp(punkApp);
	if (punkApp == 0)
		return;
	IUnknownPtr punkAM;
	sptrApp->GetActionManager(&punkAM);
	IActionManagerPtr sptrAM(punkAM);
	if (sptrAM == 0)
		return;
	DWORD dwFlags = 0;
	sptrAM->GetCurrentExecuteFlags(&dwFlags);
	if (dwFlags&aefRunning)
		return;
	IUnknownPtr punkDoc;
	sptrApp->GetActiveDocument(&punkDoc);
	if (punkDoc == 0)
		return;
	if (!::CheckInterface(punkDoc, IID_IDBaseDocument))
		return;
	int nAbacusRecord = _AbacusGetLastRecordNum();
	if (nAbacusRecord > s_nLastAbacusRecord)
	{
		s_nLastAbacusRecord = nAbacusRecord;
		if (AfxMessageBox(IDS_ASK_SEND, MB_YESNO|MB_ICONQUESTION) == IDYES)
			::ExecuteAction("AbacusToDB", NULL, 0);
	}
}


class _CAbacusFree
{
public:
	~_CAbacusFree()
	{
		if (CActionAbacusConnect::s_bConnected)
		{
			if (s_nTimerId > 0)
				KillTimer(NULL, s_nTimerId);
			_AbacusDisconnect();
		}
	}
};


CActionAbacusConnect::CActionAbacusConnect()
{
}

CActionAbacusConnect::~CActionAbacusConnect()
{
}

bool CActionAbacusConnect::Invoke()
{
	if (s_bConnected)
	{
		_AbacusDisconnect();
		if (s_nTimerId > 0)
		{
			KillTimer(NULL, s_nTimerId);
			s_nTimerId = 0;
		}
		s_bConnected = false;
	}
	else
	{
		CWaitCursor wc;
		s_bAutoExecute = ::GetValueInt(::GetAppUnknown(), "\\Abacus", "AutoExecuteSendToDB", true)?true:false;
		ABACUSCONTROLSETUP AbacusSetup = {0};
		AbacusSetup.nPackageBuffSize = ::GetValueInt(::GetAppUnknown(), "\\Abacus", "MaxPackageSize", 16384);
		AbacusSetup.nConnectTimeoutMs = ::GetValueInt(::GetAppUnknown(), "\\Abacus", "ConnectTimeout", 10000);
		AbacusSetup.nDebugLog = ::GetValueInt(::GetAppUnknown(), "\\Abacus", "DebugLog", 0);
		AbacusSetup.bRequestPLT = ::GetValueInt(::GetAppUnknown(), "\\Abacus", "RequestPLT", FALSE)?true:false;
		AbacusSetup.bRequestRBC = ::GetValueInt(::GetAppUnknown(), "\\Abacus", "RequestRBC", FALSE)?true:false;
		AbacusSetup.bRequestWBC = ::GetValueInt(::GetAppUnknown(), "\\Abacus", "RequestWBC", FALSE)?true:false;
		COMMSETUP CommSetup = {0};
		CommSetup.nBaud = ::GetValueInt(::GetAppUnknown(), "\\Abacus\\Comm", "Baud", CBR_9600);
		CString sCom = ::GetValueString(::GetAppUnknown(), "\\Abacus\\Comm", "Port", "Com1");
		strncpy(CommSetup.szComm, sCom, min(sCom.GetLength(),50));
		CommSetup.nDataBits = ::GetValueInt(::GetAppUnknown(), "\\Abacus\\Comm", "DataBits", 8);
		CommSetup.nParity = ::GetValueInt(::GetAppUnknown(), "\\Abacus\\Comm", "Parity", NOPARITY);
		CommSetup.nReadTotalTimeoutConstant = ::GetValueInt(::GetAppUnknown(), "\\Abacus\\Comm", "Parity", 1000);
		CommSetup.nStopBits = ::GetValueInt(::GetAppUnknown(), "\\Abacus\\Comm", "StopBits", ONESTOPBIT);
		bool bRes = _AbacusConnect(&AbacusSetup, &CommSetup);
		if (bRes)
		{
			DWORD dwStart = GetTickCount();
			int nState = _AbacusGetState();
			DWORD dwNow = dwStart;
			while ((nState == AbacusState_Connecting || nState == AbacusState_NotConnected)&&
				dwNow < dwStart+2*AbacusSetup.nConnectTimeoutMs)
			{
				Sleep(200);
				dwNow = GetTickCount();
				nState = _AbacusGetState();
			}
			if (nState == AbacusState_Connected)
			{
				if (s_bAutoExecute)
					s_nTimerId = SetTimer(NULL, 2, 1000, TimerProc);
				s_bConnected = true;
			}
			else
				AfxMessageBox(IDS_CAN_NOT_CONNECT_TO_ABACUS, MB_OK|MB_ICONEXCLAMATION);
		}
		else
			AfxMessageBox(IDS_CAN_NOT_CONNECT_TO_ABACUS, MB_OK|MB_ICONEXCLAMATION);
	}
	return true;
}

bool CActionAbacusConnect::IsChecked()
{
	return s_bConnected;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//CActionAbacusToDB
////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CActionAbacusToDB, CCmdTargetEx);

// {7EEE95B7-A9B9-4427-9FAA-A79B07CF06B6}
GUARD_IMPLEMENT_OLECREATE(CActionAbacusToDB, _T("AbacusUI.ActionAbacusToDB"), 
0x7eee95b7, 0xa9b9, 0x4427, 0x9f, 0xaa, 0xa7, 0x9b, 0x7, 0xcf, 0x6, 0xb6);

// {B3F9D303-E199-448c-B1C7-D46B22ED40A9}
static const GUID guidActionAbacusToDB = 
{ 0xb3f9d303, 0xe199, 0x448c, { 0xb1, 0xc7, 0xd4, 0x6b, 0x22, 0xed, 0x40, 0xa9 } };


ACTION_INFO_FULL(CActionAbacusToDB, ID_ACTION_ABACUS_TO_DB, -1, -1, guidActionAbacusToDB);
ACTION_TARGET(CActionAbacusToDB, szTargetAnydoc);
ACTION_ARG_LIST(CActionAbacusToDB)
END_ACTION_ARG_LIST();

CActionAbacusToDB::CActionAbacusToDB()
{
}

CActionAbacusToDB::~CActionAbacusToDB()
{
}

#define FLDI_SNO			0
#define FLDI_DATE			1
#define FLDI_SID			2
#define FLDI_PID			3
#define FLDI_NAME			4
#define FLDI_BIRTH			5
#define FLDI_SEX			6
#define FLDI_DOC			7
#define FLDI_OPID			8
#define FLDI_MODE			9
#define FLDI_WRN			10
#define FLDI_WBC			18
#define FLDI_RBC			19
#define FLDI_HGB			20
#define FLDI_HCT			21
#define FLDI_MCV			22
#define FLDI_MCH			23
#define FLDI_MCHC			24
#define FLDI_PLT			25
#define FLDI_PCT			26
#define FLDI_MPV			27
#define FLDI_PDWsd			28
#define FLDI_PDWcv			29
#define FLDI_RDWsd			30
#define FLDI_RDWcv			31
#define FLDI_LYM			32
#define FLDI_MID			33
#define FLDI_GRA			34
#define FLDI_LYMPR			35
#define FLDI_MIDPR			36
#define FLDI_GRAPR			37
#define FLDI_RBCTime		38
#define FLDI_WBCTime		39

CString CActionAbacusToDB::MakeSexString(int nSex)
{
	CString s;
	s.LoadString(nSex?IDS_FEMALE:IDS_MALE);
	return s;
}

void CActionAbacusToDB::AddWarningText(CString &s, LPCTSTR lpstrWarn, LPCTSTR lpstrDef)
{
	CString sWarn = ::GetValueString(GetAppUnknown(), _T("\\Abacus\\Warnings"),
		lpstrWarn, lpstrDef);
	if (sWarn.GetLength() > 0)
	{
		sWarn += "\n";
		s += sWarn;
	}
}


CString CActionAbacusToDB::MakeWarningString(DWORD dwWarn)
{
	CString s;
	if (dwWarn & 0x0001)
		AddWarningText(s, _T("c"), _T("RBC/PLT clogging."));
	if (dwWarn & 0x0002)
		AddWarningText(s, _T("s"), _T("RBC/PLT time error."));
	if (dwWarn & 0x0004)
		AddWarningText(s, _T("d"), _T("RBC/PLT data package error."));
	if (dwWarn & 0x0008)
		AddWarningText(s, _T("m"), _T("RBC/PLT coincidence too high. Linearity error."));
	if (dwWarn & 0x0010)
		AddWarningText(s, _T("k"), _T("RBC peak warning."));
	if (dwWarn & 0x0020)
		AddWarningText(s, _T("l"), _T("RBC/PLT limit warning."));
	if (dwWarn & 0x0040)
		AddWarningText(s, _T("b"), _T("RBC blank is high, or no RBC blank."));
	if (dwWarn & 0x0080)
		AddWarningText(s, _T("p"), _T("PLT blank too high, or no PLT blank."));
	if (dwWarn & 0x0100)
		AddWarningText(s, _T("C"), _T("WBC clogging."));
	if (dwWarn & 0x0200)
		AddWarningText(s, _T("S"), _T("WBC time error."));
	if (dwWarn & 0x0400)
		AddWarningText(s, _T("D"), _T("WBC data package errors."));
	if (dwWarn & 0x0800)
		AddWarningText(s, _T("M"), _T("WBC coincidence is too high. Linearity error."));
	if (dwWarn & 0x1000)
		AddWarningText(s, _T("R"), _T("Too many RBC cut from WBC"));
	if (dwWarn & 0x2000)
		AddWarningText(s, _T("L"), _T("WBC/RBC limit warning"));
	if (dwWarn & 0x4000)
		AddWarningText(s, _T("B"), _T("WBC blank too high, or no WBC blank"));
	if (dwWarn & 0x8000)
		AddWarningText(s, _T("H"), _T("HGB blank is high, or no HGB blank"));
	if (dwWarn & 0x10000)
		AddWarningText(s, _T("E"), _T("No WBC three part"));
	if (dwWarn & 0x20000)
		AddWarningText(s, _T("W"), _T("WBC three part warning or WBC three part diff. unsuccessful."));
	return s;
}

CString CActionAbacusToDB::MakeModeString(int nMode)
{
	CString sEntry;
	sEntry.Format(_T("%d"), nMode);
	CString s = GetValueString(GetAppUnknown(), "\\Abacus\\Mode", sEntry, sEntry);
	return s;
}

void CActionAbacusToDB::DoSendToDB(const char *pszName, _variant_t varValue)
{
	CString sFieldName = GetValueString(GetAppUnknown(), "\\Abacus\\SendToDB", pszName, NULL);
	if (sFieldName.IsEmpty())
		return;
	sptrINamedData spND(m_punkTarget);
	if (spND != 0)
	{
		CString sKey = CString(SECTION_DBASEFIELDS) + CString(_T("\\")) + sFieldName;
		spND->SetValue(_bstr_t((LPCTSTR)sKey), varValue);
	}
}

void CActionAbacusToDB::SendToDB(const char *pszName, int nValue)
{
	_variant_t var((long)nValue);
	DoSendToDB(pszName, var);
}

void CActionAbacusToDB::SendToDB(const char *pszName, double dValue)
{
	_variant_t var(dValue);
	DoSendToDB(pszName, var);
}

void CActionAbacusToDB::SendToDB(const char *pszName, struct tm tm1)
{
	COleDateTime dt(tm1.tm_year, tm1.tm_mon, tm1.tm_mday, tm1.tm_hour,
		tm1.tm_min, tm1.tm_sec);
	_variant_t var(dt, VT_DATE);
	DoSendToDB(pszName, var);
}

void CActionAbacusToDB::SendToDB(const char *pszName, const char *pszValue)
{
	_variant_t var(pszValue);
	DoSendToDB(pszName, var);
}

void CActionAbacusToDB::SendParameterToDB(const char *pszName, double dCoef, ABACUSPARAMETER *pParam)
{
	if (pParam->nParFlag < 4)
		SendToDB(pszName, dCoef*pParam->dParValue);
}

bool CActionAbacusToDB::Invoke()
{
	CWaitCursor wc;
	IDBaseDocumentPtr ptrDB(m_punkTarget);
	if (ptrDB == NULL)
		return false;
	IUnknownPtr punkQuery;
	ptrDB->GetActiveQuery(&punkQuery);
	if (punkQuery==0)
		return false;
	ISelectQueryPtr ptrQ(punkQuery);
	if (ptrQ == NULL)
		return false;
	BOOL bOpen = FALSE;
	ptrQ->IsOpen( &bOpen );
	if (!bOpen)
		return false;

	POSITION lPos = 0;
	_AbacusGetFirstRecordPosition(&lPos);
	while(lPos)
	{
		POSITION lPrevPos = lPos;
		ABACUSRECORD *p;
		_AbacusGetNextRecord(&lPos, &p);
		if (ptrQ->Insert() != S_OK)
			return false;
		SendToDB("SNO", p->Data.nSNO);
		SendToDB("DATE", p->Data.tmMeasurement);
		SendToDB("SID", p->Data.pszSID);
		SendToDB("PID", p->Data.pszPID);
		SendToDB("NAME", p->Data.pszName);
		SendToDB("BIRTH", p->Data.dtBirth);
		CString sSex = MakeSexString(p->Data.nSex);
		SendToDB("SEX", (const char *)sSex);
		SendToDB("DOC", p->Data.pszDoc);
		SendToDB("OPID", p->Data.pszOPID);
		CString sMode = MakeModeString(p->Data.nMode);
		SendToDB("MODE", (const char *)sMode);
		CString sWarn = MakeWarningString(p->Data.dwWarn);
		SendToDB("WARN", (const char *)sWarn);
		if (p->Data.nParn >= 1)
			SendParameterToDB("P01", 1000000000., &p->Data.padParams[0]);
		if (p->Data.nParn >= 2)
			SendParameterToDB("P02", 1000000000000., &p->Data.padParams[1]);
		if (p->Data.nParn >= 3)
			SendParameterToDB("P03", 1., &p->Data.padParams[2]);
		if (p->Data.nParn >= 4)
			SendParameterToDB("P04", 1., &p->Data.padParams[3]);
		if (p->Data.nParn >= 5)
			SendParameterToDB("P05", 1., &p->Data.padParams[4]);
		if (p->Data.nParn >= 6)
			SendParameterToDB("P06", 1., &p->Data.padParams[5]);
		if (p->Data.nParn >= 7)
			SendParameterToDB("P07", 1., &p->Data.padParams[6]);
		if (p->Data.nParn >= 8)
			SendParameterToDB("P08", 1000000000., &p->Data.padParams[7]);
		if (p->Data.nParn >= 9)
			SendParameterToDB("P09", 1., &p->Data.padParams[8]);
		if (p->Data.nParn >= 10)
			SendParameterToDB("P10", 1., &p->Data.padParams[9]);
		if (p->Data.nParn >= 11)
			SendParameterToDB("P11", 1., &p->Data.padParams[10]);
		if (p->Data.nParn >= 12)
			SendParameterToDB("P12", 1., &p->Data.padParams[11]);
		if (p->Data.nParn >= 13)
			SendParameterToDB("P13", 1., &p->Data.padParams[12]);
		if (p->Data.nParn >= 14)
			SendParameterToDB("P14", 1., &p->Data.padParams[13]);
		if (p->Data.nParn >= 15)
			SendParameterToDB("P15", 1000000000., &p->Data.padParams[14]);
		if (p->Data.nParn >= 16)
			SendParameterToDB("P16", 1000000000., &p->Data.padParams[15]);
		if (p->Data.nParn >= 17)
			SendParameterToDB("P17", 1000000000., &p->Data.padParams[16]);
		if (p->Data.nParn >= 18)
			SendParameterToDB("P18", 1., &p->Data.padParams[17]);
		if (p->Data.nParn >= 19)
			SendParameterToDB("P19", 1., &p->Data.padParams[18]);
		if (p->Data.nParn >= 20)
			SendParameterToDB("P20", 1., &p->Data.padParams[19]);
		if (p->Data.nParn >= 21)
			SendParameterToDB("P21", 1., &p->Data.padParams[20]);
		if (p->Data.nParn >= 22)
			SendParameterToDB("P22", 1., &p->Data.padParams[21]);
		if (ptrQ->Update() != S_OK)
			return false;
		_AbacusRemoveRecord(lPrevPos);
	}
	s_nLastAbacusRecord = ::_AbacusGetLastRecordNum();


	return true;
}

bool CActionAbacusToDB::IsAvaible()
{
	return CActionAbacusConnect::IsConnected() &&
		::CheckInterface(m_punkTarget, IID_IDBaseDocument);
}



