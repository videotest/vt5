#include "StdAfx.h"
#include "AbacusWork.h"


static HINSTANCE s_hLib;

static ABACUSCONNECT s_AbacusConnect;
static ABACUSDISCONNECT s_AbacusDisconnect;
static ABACUSGETSTATE s_AbacusGetState;
static ABACUSGETINIT s_AbacusGetInit;
static ABACUSGETFIRSTRECORDPOSITION s_AbacusGetFirstRecord;
static ABACUSGETNEXTRECORD s_AbacusGetNextRecord;
static ABACUSREMOVERECORD s_AbacusRemoveRecord;
static ABACUSGETSETUP s_AbacusGetSetup;
static ABACUSSETSETUP s_AbacusSetSetup;
static ABACUSGETCOMMSETUP s_AbacusGetCommSetup;
static ABACUSSETCOMMSETUP s_AbacusSetCommSetup;
static ABACUSGETLASTRECORDNUM s_AbacusGetLastRecordNum;

bool _AbacusConnect(ABACUSCONTROLSETUP *pAbacusSetup, COMMSETUP *pCommSetup)
{
	char szPath[_MAX_PATH],szDrive[_MAX_DRIVE],szDir[_MAX_DIR];
	GetModuleFileName(AfxGetInstanceHandle(), szPath, _MAX_PATH);
	_splitpath(szPath, szDrive, szDir, NULL, NULL);
	_makepath(szPath, szDrive, szDir, "AbacusCtrl", ".dll");
	s_hLib = LoadLibrary(szPath);
	if (s_hLib == NULL)
	{
		CString s;
		s.Format("Can not load %s", szPath);
		AfxMessageBox(s, MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	s_AbacusConnect = (ABACUSCONNECT)GetProcAddress(s_hLib, "AbacusConnect");
	s_AbacusDisconnect = (ABACUSDISCONNECT)GetProcAddress(s_hLib, "AbacusDisconnect");
	s_AbacusGetState = (ABACUSGETSTATE)GetProcAddress(s_hLib, "AbacusGetState");
	s_AbacusGetInit = (ABACUSGETINIT)GetProcAddress(s_hLib, "AbacusGetInit");
	s_AbacusGetFirstRecord = (ABACUSGETFIRSTRECORDPOSITION)GetProcAddress(s_hLib,
		"AbacusGetFirstRecordPosition");
	s_AbacusGetNextRecord = (ABACUSGETNEXTRECORD)GetProcAddress(s_hLib, "AbacusGetNextRecord");
	s_AbacusRemoveRecord = (ABACUSREMOVERECORD)GetProcAddress(s_hLib, "AbacusRemoveRecord");
	s_AbacusGetSetup = (ABACUSGETSETUP)GetProcAddress(s_hLib, "AbacusGetSetup");
	s_AbacusSetSetup = (ABACUSSETSETUP)GetProcAddress(s_hLib, "AbacusSetSetup");
	s_AbacusGetCommSetup = (ABACUSGETCOMMSETUP)GetProcAddress(s_hLib, "AbacusGetCommSetup");
	s_AbacusSetCommSetup = (ABACUSSETCOMMSETUP)GetProcAddress(s_hLib, "AbacusSetCommSetup");
	s_AbacusGetLastRecordNum = (ABACUSGETLASTRECORDNUM)GetProcAddress(s_hLib, "AbacusGetLastRecordNum");
	if (s_AbacusConnect == 0 || s_AbacusDisconnect == 0 || s_AbacusGetState == 0 ||
		s_AbacusGetInit == 0 || s_AbacusGetFirstRecord == 0 ||
		s_AbacusGetNextRecord == 0 || s_AbacusRemoveRecord == 0 ||
		s_AbacusGetSetup == 0 || s_AbacusSetSetup == 0 ||
		s_AbacusGetCommSetup == 0 || s_AbacusSetCommSetup == 0 ||
		s_AbacusGetLastRecordNum == 0)
	{
		CString s;
		s.Format("%s is invalid", szPath);
		AfxMessageBox(s, MB_OK|MB_ICONEXCLAMATION);
		FreeLibrary(s_hLib);
		s_hLib = NULL;
		return false;
	}
	s_AbacusSetSetup(pAbacusSetup);
	s_AbacusSetCommSetup(pCommSetup);
	if (!s_AbacusConnect())
	{
		FreeLibrary(s_hLib);
		s_hLib = NULL;
		return false;
	}
	return true;
}

void _AbacusDisconnect()
{
	if (s_hLib)
	{
		s_AbacusDisconnect();
		FreeLibrary(s_hLib);
		s_hLib = NULL;
	}
}

int  _AbacusGetState()
{
	if (s_hLib == NULL)
		return AbacusState_NotConnected;
	else
		return s_AbacusGetState();
}

bool _AbacusGetInit(ABACUSINIT **ppAbacusInit)
{
	if (s_hLib == NULL)
		return false;
	else return s_AbacusGetInit(ppAbacusInit);
}

bool _AbacusGetFirstRecordPosition(TPOS *plPos)
{
	if (s_hLib == NULL)
		return false;
	else return s_AbacusGetFirstRecord(plPos);
}

bool _AbacusGetNextRecord(TPOS *plPos, ABACUSRECORD **ppRecords)
{
	if (s_hLib == NULL)
		return false;
	else return s_AbacusGetNextRecord(plPos, ppRecords);
}

void _AbacusRemoveRecord(TPOS lPos)
{
	if (s_hLib != NULL)
		return s_AbacusRemoveRecord(lPos);
}

bool _AbacusGetSetup(ABACUSCONTROLSETUP *pSetup)
{
	if (s_hLib == NULL)
		return false;
	else
	{
		s_AbacusGetSetup(pSetup);
		return true;
	}
}

void _AbacusSetSetup(ABACUSCONTROLSETUP *pSetup)
{
	if (s_hLib != NULL)
		return s_AbacusSetSetup(pSetup);
}

bool _AbacusGetCommSetup(COMMSETUP *pCommSetup)
{
	if (s_hLib == NULL)
		return false;
	else
	{
		s_AbacusGetCommSetup(pCommSetup);
		return true;
	}
}

void _AbacusSetCommSetup(COMMSETUP *pCommSetup)
{
	if (s_hLib != NULL)
		s_AbacusSetCommSetup(pCommSetup);
}

int _AbacusGetLastRecordNum()
{
	if (s_hLib == NULL)
		return 0;
	else
		return s_AbacusGetLastRecordNum();
}
