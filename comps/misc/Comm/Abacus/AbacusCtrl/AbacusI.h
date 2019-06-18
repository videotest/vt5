#pragma once

#include <time.h>


#ifdef ABACUSCTRL_EXPORTS
#define ABACUSCTRL_API __declspec(dllexport)
#else
#define ABACUSCTRL_API __declspec(dllimport)
#endif

// ��������� ��� ������ AbacusCtrl.dll
struct ABACUSCONTROLSETUP
{
	int nPackageBuffSize; // ������ ������� ��� ������.
	int nDebugLog; // �������� �� ���������� ����������
	bool bRequestRBC; // ����������� �� ��������� RBC
	bool bRequestWBC; // ����������� �� ��������� WBC
	bool bRequestPLT; // ����������� �� ��������� PLT
	int nConnectTimeoutMs; // ������� ������� (� ��) ����� ������ �������
	                       // �� ������ <ENQ> ��� ����������
};


// ��������� COMM-�����
struct COMMSETUP
{
	int nBaud; //CBR_2400, CBR_4800, CBR_9600 � �.�.
	int nStopBits; // ONESTOPBIT, ONE5STOPBITS, TWOSTOPBITS
	int nParity; // NOPARITY, EVENPARITY, ODDPARITY, MARKPARITY ��� SPACEPARITY
	int nDataBits; // ����� ��� ������
	int nReadTotalTimeoutConstant; // ������� ��� ������ ������ �������
	char szComm[50]; // ��������, �������� "Com1"
};

// ��������� AbacusCtrl.dll
enum ABACUSSTATE
{
	AbacusState_NotConnected = 0,
	AbacusState_Connecting = 1,
	AbacusState_Connected = 5,
	AbacusState_Error = 6,
};

// ������ ������ INIT � �������
struct ABACUSINIT
{
	char *pszDevice;// �������� "Abacus Junior Human"
	char *pszVer;// �������� "2.66"
	tm   tmMsg;// ���� � �����
};

// ���������� � ���������
struct ABACUSPARAMETER
{
	const char *pszParID; // ��������, �������� P01, P02 � �.�.
	double dParValue; // ��������
	int nParFlag; // ���������� �������� ��������������
};

// ������ ������ DATA � ������� - ����� ���������� �� ����� �������.
// ��� ������� ���� � ������� ��� ����������� �� �������� ���������
// (http://www.diatronltd.com/downloads/serpro17.pdf)
struct ABACUSDATA
{
	int nSNO; // ���� SNO (internal identifier), ����������� � ��������� ����
	tm  tmMeasurement; // ������ ����� DATE � TIME
	const char *pszSID; // ���� SID (sample identifier (alphanumeric))
	const char *pszPID; // ���� PID (patient identifier (alphanumeric))
	const char *pszName;// ���� NAME (patient name (alphanumeric))
	tm dtBirth;// ���� BIRTH. ��������� �������� ������ ����, �� �����.
	int nSex; // ���� SEX(0 -male, 1- female)
	const char *pszDoc;// ���� DOC (doctor's name)
	const char *pszOPID;// ���� OPID (operator identifier (alphanumeric))
	int nMode;// ���� MODE (patient type)
	DWORD dwWarn;// ���� WARN (warning bits)
	int nPM1; // ���� PM1 (PLT lower marker)
	int nPM2; // ���� PM2 (PLT upper marker)
	int nRM1; // ���� RM1 (RBC lower marker)
	int nWM1; // ���� WM1 (WBC upper marker)
	int nWM2; // ���� WM2 (WBC LYM-MID marker)
	int nWM3; // ���� WM3 (WBC MID-GRA marker)
	int nParn; // ���� PARN (Number of parameters)
	ABACUSPARAMETER *padParams; // ������ ���������� �������� nParn
};

// ������ ������� RBC, WBC, PLT � ������� - �����������.
// ��� ������� ���� � ������� ��� ����������� �� �������� ���������
// (http://www.diatronltd.com/downloads/serpro17.pdf)
struct ABACUSHISTOGRAM
{
	int nSNO;// ���� SNO (internal identifier), ����������� � ��������� ����
	tm tmMeasurement;// ������ ����� DATE � TIME
	const char *pszSID;// ���� SID (sample identifier (alphanumeric))
	const char *pszPID;// ���� PID (patient identifier (alphanumeric))
	int nCHN;// ���� CHN (number of histogram channels (256))
	int nData[256]; // ������ �����������
};

// �����, ������������ ������������ �� ��������������� �����������
enum AbacusHistoFlags
{
	ABACUS_RBC = 1, // ��������� RBC
	ABACUS_WBC = 2, // ��������� WBC
	ABACUS_PLT = 4, // ��������� PLT
};

// ���������� � ���������� �������
struct ABACUSRECORD
{
	ABACUSDATA Data; // ���������� ������ DATA
	DWORD nHistoFlags; // ����� �� AbacusHistoFlags
	ABACUSHISTOGRAM Histo[3];// ���������� ������� RBC, WBC, PLT
};

// ���������� ����������. ���������� true ���� ���������� �����������
extern "C" ABACUSCTRL_API bool AbacusConnect();
typedef ABACUSCTRL_API bool (*ABACUSCONNECT)();
// ��������� ���������� � ���������� COM-����
extern "C" ABACUSCTRL_API void AbacusDisconnect();
typedef ABACUSCTRL_API void (*ABACUSDISCONNECT)();
// ��������� �����. ���������� ���� �� �������� ABACUSSTATE.
extern "C" ABACUSCTRL_API int  AbacusGetState();
typedef ABACUSCTRL_API int  (*ABACUSGETSTATE)();
// �������� ���������� �� ������ INIT. ���������� false ���� ���������� �� �����������
// ��� ����� INIT �� ��������. 
// �������� ppAbacusInit ��������� �� ���������� ���� ABACUSINIT *, ����
// ��������� ��������� �� ��������� � ������� �� ������ INIT. ��� ���������
// ������ ��� ������, �� ������ ��������������.
extern "C" ABACUSCTRL_API bool AbacusGetInit(ABACUSINIT **ppAbacusInit);
typedef ABACUSCTRL_API bool (*ABACUSGETINIT)(ABACUSINIT **ppAbacusInit);
// �������� ������� ������ ������ � ����������� �� ������� �� �����������
// ������. ���������� false, ���� � ������ ��� �� ����� ����� ������.
extern "C" ABACUSCTRL_API bool AbacusGetFirstRecordPosition(long *plPos);
typedef ABACUSCTRL_API bool (*ABACUSGETFIRSTRECORDPOSITION)(long *plPos);
// �������� ������� ��������� ������ � ����������� �� ������� �� �����������
// ������, � �� ������� ������� - ���������� �� �������.
// �������� plPos �������� ��������� �� ������� ������, �� ������� ���� ��������
// ����������. �� ������ � ���� ��������� ������� ��������� ������.
// �������� ppRecords ��������� �� ���������� ���� ABACUSRECORD *, ����
// ��������� ��������� �� ��������� � ������� �� �������. ��� ���������
// ������ ��� ������, �� ������ ��������������.
// ������ ������������ true.
extern "C" ABACUSCTRL_API bool AbacusGetNextRecord(long *plPos, ABACUSRECORD **ppRecords);
typedef ABACUSCTRL_API bool (*ABACUSGETNEXTRECORD)(long *plPos, ABACUSRECORD **ppRecords);
// ������� ������ �� �������.
extern "C" ABACUSCTRL_API void AbacusRemoveRecord(long lPos);
typedef ABACUSCTRL_API void (*ABACUSREMOVERECORD)(long lPos);
// �������� ������� ��������� ��� ������ AbacusCtrl.dll
extern "C" ABACUSCTRL_API void AbacusGetSetup(ABACUSCONTROLSETUP *pSetup);
typedef ABACUSCTRL_API void (*ABACUSGETSETUP)(ABACUSCONTROLSETUP *pSetup);
// ���������� ��������� ��� ������ AbacusCtrl.dll.
extern "C" ABACUSCTRL_API void AbacusSetSetup(ABACUSCONTROLSETUP *pSetup);
typedef ABACUSCTRL_API void (*ABACUSSETSETUP)(ABACUSCONTROLSETUP *pSetup);
// �������� ������� ��������� COM-�����
extern "C" ABACUSCTRL_API void AbacusGetCommSetup(COMMSETUP *pCommSetup);
typedef ABACUSCTRL_API void (*ABACUSGETCOMMSETUP)(COMMSETUP *pCommSetup);
// ���������� ��������� COM-�����
extern "C" ABACUSCTRL_API void AbacusSetCommSetup(COMMSETUP *pCommSetup);
typedef ABACUSCTRL_API void (*ABACUSSETCOMMSETUP)(COMMSETUP *pCommSetup);
// �������� ����� �������, �������� � ������� �� ����� ������ ����� ��������� ����������.
extern "C" ABACUSCTRL_API int AbacusGetLastRecordNum();
typedef ABACUSCTRL_API int (*ABACUSGETLASTRECORDNUM)();
