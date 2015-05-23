// AbacusCtrl.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "AbacusCtrl.h"
#include "Comm.h"
#include "debug.h"
#include <memory>
#include <iostream>
#include <vector>

#define LOG_FILE szLogPath
char szLogPath[_MAX_PATH] = "\0";

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	char szDrive[_MAX_DRIVE],szDir[_MAX_DIR],szFName[_MAX_FNAME];
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (GetModuleFileName((HMODULE)hModule, szLogPath, _MAX_PATH))
		{
			_splitpath(szLogPath, szDrive,szDir, szFName, NULL);
			_makepath(szLogPath, szDrive, szDir, szFName, ".log");
		}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}


struct AbacusRecordHolder
{
	AbacusRecordHolder *pNext;
	AbacusRecordHolder *pPrev;
	ABACUSRECORD ar;
};
AbacusRecordHolder *s_pFirstAbacusRecord = NULL;
AbacusRecordHolder *s_pLastAbacusRecord = NULL;
ABACUSINIT s_AbacusInit = {0, 0};

static void FreeInitRecord()
{
	if (s_AbacusInit.pszDevice)
		delete s_AbacusInit.pszDevice;
	s_AbacusInit.pszDevice = 0;
	if (s_AbacusInit.pszVer)
		delete s_AbacusInit.pszVer;
	s_AbacusInit.pszVer = 0;
}

static void FreeDataRecord(AbacusRecordHolder *p)
{
	delete p->ar.Data.padParams;
	delete p->ar.Data.pszDoc;
	delete p->ar.Data.pszName;
	delete p->ar.Data.pszOPID;
	delete p->ar.Data.pszPID;
	delete p->ar.Data.pszSID;
	for (int i = 0; i < 3; i++)
	{
		delete p->ar.Histo[i].pszPID;
		delete p->ar.Histo[i].pszSID;
	}
}

static void FreeAllDataRecords()
{
	for (AbacusRecordHolder *p = s_pFirstAbacusRecord; p != NULL;)
	{
		FreeDataRecord(p);
		AbacusRecordHolder *p1 = p;
		p = p->pNext;
		delete p1;
	}
	s_pFirstAbacusRecord = NULL;
	s_pLastAbacusRecord = NULL;
}

static char *MakePSZ(const void *p, int nSize)
{
	static char szBuff[256];
	if (nSize > 256) nSize = 256;
	memcpy(szBuff, p, nSize);
	szBuff[nSize] = 0;
	return szBuff;
}

ABACUSCONTROLSETUP s_AbacusSetup = {16384, 0, 0, 0, 0, 10000};
static bool s_bInitialized = false;
static int s_nErrorCode = 0;
static LONG s_dwStop = 0;
static ABACUSSTATE s_AbacusState = AbacusState_NotConnected;
static CRITICAL_SECTION s_cs;
static bool s_bIncorrectPackage = false;
static HANDLE s_hThread = NULL;
static DWORD s_dwThreadId = 0;
static int s_nLastRecordNum = 0;

static bool ReadPackage(unsigned char *pBuffer, unsigned nBufferSize, unsigned *pnPackageSize)
{
	unsigned nPos = 0;
	DWORD nRead = 0;
	*pnPackageSize = 0;
	const unsigned nRBuffSize = 128;
	unsigned char buff[nRBuffSize];
	while (g_Comm.ReadEx(nRBuffSize, buff, &nRead) && !s_dwStop)
	{
		if (nRead == 0)
			continue;
/*		{
			if (s_AbacusSetup.nDebugLog)
				dprintf(LOG_FILE, "Package reading error, 0 bytes read (%d)\n",
					GetLastError());
			return false;
		}*/
		int nEOTPos = -1;
		for (unsigned i = 0; i < nRead; i++)
		{
			if (buff[i] == 4) // EOT
			{
				nEOTPos = i;
				break;
			}
		}
		if (nEOTPos >= 0 && nEOTPos != nRead-1)
		{
			if (s_AbacusSetup.nDebugLog)
				dprintf(LOG_FILE, "Package reading error: data after EOT\n");
			return false;
		}
		if (nEOTPos >= 0)
		{
			memcpy(pBuffer+nPos, buff, nEOTPos+1);
			*pnPackageSize = nPos+(unsigned)nEOTPos+1;
			s_bIncorrectPackage = false;
			return true;
		}
		else
		{
			memcpy(pBuffer+nPos, buff, nRead);
			nPos += nRead;
			if (nPos + nRead > nBufferSize)
			{
				if (s_AbacusSetup.nDebugLog)
					dprintf(LOG_FILE, "Package buffer too small\n");
				return false;
			}
		}
	}
	if (!s_dwStop && s_AbacusSetup.nDebugLog)
		dprintf(LOG_FILE, "Package reading error, ReadFile failed (%d)\n",
			GetLastError());
	return false;
}

static void LogPackage(unsigned char *pBuffer, unsigned nBufferSize)
{
	char szBuff[1026];
	unsigned nPos = 0;
	dprintf(LOG_FILE, "Package:\n");
	for (unsigned i = 0; i < nBufferSize; i++)
	{
		if (pBuffer[i] >= 32)
			szBuff[nPos++] = pBuffer[i];
		else
		{
			szBuff[nPos++] = '<';
			szBuff[nPos++] = '0'+pBuffer[i]/10;
			szBuff[nPos++] = '0'+pBuffer[i]%10;
			szBuff[nPos++] = '>';
		}
		if (i > 0 && i%32 == 0)
		{
			szBuff[nPos++] = '\n';
			szBuff[nPos++] = 0;
			dprintf(LOG_FILE, "%s", szBuff);
			nPos = 0;
		}
	}
	if (nPos > 0)
	{
		szBuff[nPos++] = '\n';
		szBuff[nPos++] = 0;
		dprintf(LOG_FILE, "%s", szBuff);
	}
}

static int FieldAtoi(const unsigned char *pField, int nSize)
{
	char szBuff[64];
	memcpy(szBuff, pField, nSize);
	szBuff[nSize] = 0;
	return atoi(szBuff);
}

static int FieldHexAtoi(const unsigned char *pField, int nSize)
{
	char szBuff[64];
	memcpy(szBuff, pField, nSize);
	szBuff[nSize] = 0;
	int nValue = 0;
	sscanf(szBuff, "%x", &nValue);
	return nValue;
}

static double FieldAtof(const unsigned char *pField, int nSize)
{
	char szBuff[64];
	memcpy(szBuff, pField, nSize);
	for (int i = 0; i < nSize; i++)
	{
		if (szBuff[i] == ',')
			szBuff[i] = '.';
	}
	szBuff[nSize] = 0;
	return atof(szBuff);
}

static char *StringFieldCopy(unsigned char *pField, unsigned nSize)
{
	char *p = new char[nSize+1];
	memcpy(p, pField, nSize);
	p[nSize] = 0;
	return p;
}

static bool FieldToDate(struct tm *tm1, unsigned char *pField, unsigned nSize)
{
	if (nSize != 8)
	{
		if (s_AbacusSetup.nDebugLog)
		{
			dprintf(LOG_FILE, "Invalid date format: %s (%d chars)"
				" Date ignored\n", MakePSZ((const char*)pField, nSize), nSize);
			s_bIncorrectPackage = true;
		}
		return false;
	}
	tm1->tm_year = FieldAtoi(pField, 4);
	tm1->tm_mon = FieldAtoi(pField+4, 2);
	tm1->tm_mday = FieldAtoi(pField+6, 2);
	return true;
}

static bool FieldToTime(struct tm *tm1, unsigned char *pField, unsigned nSize)
{
	if (nSize != 6)
	{
		if (s_AbacusSetup.nDebugLog)
		{
			dprintf(LOG_FILE, "Invalid time format: %s (%d chars)"
				" Date ignored\n", MakePSZ((const char*)pField, nSize), nSize);
			s_bIncorrectPackage = true;
		}
		return false;
	}
	tm1->tm_hour = FieldAtoi(pField, 2);
	tm1->tm_min = FieldAtoi(pField+2, 2);
	tm1->tm_sec = FieldAtoi(pField+4, 2);
	return true;
}

static bool ParseInitPackage(unsigned char *pBuffer, unsigned nSize)
{
	FreeInitRecord();
	memset(&s_AbacusInit.tmMsg, 0, sizeof(s_AbacusInit.tmMsg));
	int nStage = 0;
	unsigned nPrev = 0;
	for (unsigned i = 0; i < nSize; i++)
	{
		if (pBuffer[i] == 9) //HT
		{
			if (nStage == 0)
			{
				if (i > nPrev)
				{
					s_AbacusInit.pszDevice = new char[i-nPrev+1];
					strncpy(s_AbacusInit.pszDevice, (const char *)pBuffer+nPrev, i-nPrev);
					s_AbacusInit.pszDevice[i-nPrev] = 0;
				}
			}
			else if (nStage == 1)
			{
				if (i > nPrev)
				{
					s_AbacusInit.pszVer = new char[i-nPrev+1];
					strncpy(s_AbacusInit.pszVer, (const char *)pBuffer+nPrev, i-nPrev);
					s_AbacusInit.pszVer[i-nPrev] = 0;
				}
			}
			else if (nStage == 2)
				FieldToDate(&s_AbacusInit.tmMsg, pBuffer+nPrev, i-nPrev);
			else if (s_AbacusSetup.nDebugLog)
				dprintf(LOG_FILE, "Extra field in init package - ignored\n");
			nStage++;
			nPrev = i+1;
		}
	}
	FieldToTime(&s_AbacusInit.tmMsg, pBuffer+nPrev, nSize-nPrev);
	if (nStage != 3)
		s_bIncorrectPackage = true;
	return true;
}

static bool InitDataElement(AbacusRecordHolder *pRecHld, unsigned char *pName,
	unsigned nNameSize, unsigned char *pValue, unsigned nValueSize, 
	unsigned char *pValue2 = NULL, unsigned nValue2 = 0)
{
	if (!strncmp((char *)pName, "SNO", nNameSize))
		pRecHld->ar.Data.nSNO = FieldAtoi(pValue, nValueSize);
	else if (!strncmp((char *)pName, "DATE", nNameSize))
		FieldToDate(&pRecHld->ar.Data.tmMeasurement, pValue, nValueSize);
	else if (!strncmp((char *)pName, "TIME", nNameSize))
		FieldToTime(&pRecHld->ar.Data.tmMeasurement, pValue, nValueSize);
	else if (!strncmp((char *)pName, "SID", nNameSize))
		pRecHld->ar.Data.pszSID = StringFieldCopy(pValue, nValueSize);
	else if (!strncmp((char *)pName, "PID", nNameSize))
		pRecHld->ar.Data.pszPID = StringFieldCopy(pValue, nValueSize);
	else if (!strncmp((char *)pName, "NAME", nNameSize))
		pRecHld->ar.Data.pszName = StringFieldCopy(pValue, nValueSize);
	else if (!strncmp((char *)pName, "BIRTH", nNameSize))
		FieldToDate(&pRecHld->ar.Data.dtBirth, pValue, nValueSize);
	else if (!strncmp((char *)pName, "SEX", nNameSize))
		pRecHld->ar.Data.nSex = FieldAtoi(pValue, nValueSize);
	else if (!strncmp((char *)pName, "DOC", nNameSize))
		pRecHld->ar.Data.pszDoc = StringFieldCopy(pValue, nValueSize);
	else if (!strncmp((char *)pName, "OPID", nNameSize))
		pRecHld->ar.Data.pszOPID = StringFieldCopy(pValue, nValueSize);
	else if (!strncmp((char *)pName, "MODE", nNameSize))
		pRecHld->ar.Data.nMode = FieldAtoi(pValue, nValueSize);
	else if (!strncmp((char *)pName, "WRN", nNameSize))
		pRecHld->ar.Data.dwWarn = FieldHexAtoi(pValue, nValueSize);
	else if (!strncmp((char *)pName, "PM1", nNameSize))
		pRecHld->ar.Data.nPM1 = FieldAtoi(pValue, nValueSize);
	else if (!strncmp((char *)pName, "PM2", nNameSize))
		pRecHld->ar.Data.nPM2 = FieldAtoi(pValue, nValueSize);
	else if (!strncmp((char *)pName, "RM1", nNameSize))
		pRecHld->ar.Data.nRM1 = FieldAtoi(pValue, nValueSize);
	else if (!strncmp((char *)pName, "WM1", nNameSize))
		pRecHld->ar.Data.nWM1 = FieldAtoi(pValue, nValueSize);
	else if (!strncmp((char *)pName, "WM2", nNameSize))
		pRecHld->ar.Data.nWM2 = FieldAtoi(pValue, nValueSize);
	else if (!strncmp((char *)pName, "WM3", nNameSize))
		pRecHld->ar.Data.nWM3 = FieldAtoi(pValue, nValueSize);
	else if (!strncmp((char *)pName, "PARN", nNameSize))
	{
		pRecHld->ar.Data.nParn = FieldAtoi(pValue, nValueSize);
		if (pRecHld->ar.Data.nParn > 0)
		{
			pRecHld->ar.Data.padParams = new ABACUSPARAMETER[pRecHld->ar.Data.nParn];
			for (int i = 0; i < pRecHld->ar.Data.nParn; i++)
			{
				pRecHld->ar.Data.padParams[i].dParValue = 0.;
				pRecHld->ar.Data.padParams[i].pszParID = NULL;
				pRecHld->ar.Data.padParams[i].nParFlag = 0;
			}
		}
	}
	else if (pName[0] == 'P' && nNameSize == 3 && isdigit(pName[1]) &&
		isdigit(pName[2]))
	{
		int n = FieldAtoi(pName+1, 2)-1;
		if (n < 0 || n >= pRecHld->ar.Data.nParn)
		{
			if (s_AbacusSetup.nDebugLog > 0)
				dprintf(LOG_FILE, "Invalid parameter number %d\n", n);
			s_bIncorrectPackage = true;
		}
		else
		{
			pRecHld->ar.Data.padParams[n].dParValue = FieldAtof(pValue, nValueSize);
			if (pValue2 != NULL)
				pRecHld->ar.Data.padParams[n].nParFlag = FieldAtoi(pValue2, nValue2);
		}
	}
	else
	{
		if (s_AbacusSetup.nDebugLog > 0)
			dprintf(LOG_FILE, "Unknown data element %s\n", 
				MakePSZ((const char*)pName, nNameSize));
		s_bIncorrectPackage = true;
		return false;
	}
	return true;
}

static bool ParseDataPackage(unsigned char *pBuffer, unsigned nSize)
{
	int nField = 0;
	unsigned anFieldPos[3],anFieldSize[3];
	anFieldPos[0] = 0;
	AbacusRecordHolder *pRecHld = new AbacusRecordHolder;
	memset(pRecHld, 0, sizeof(*pRecHld));
	for (unsigned i = 0; i < nSize+1; i++)
	{
		if (pBuffer[i] == 9) //HT
		{
			anFieldSize[nField] = i-anFieldPos[nField];
			nField++;
			anFieldPos[nField] = i+1;
		}
		else if (pBuffer[i] == 10 || i == nSize)
		{
			anFieldSize[nField] = i-anFieldPos[nField];
			if (nField < 1)
			{
				if (i-anFieldPos[nField] > 0)
				{
					if (s_AbacusSetup.nDebugLog)
					{
						dprintf(LOG_FILE, "Error in data package: no HT in field %s\n",
							MakePSZ(pBuffer+anFieldPos[nField], i-anFieldPos[nField]));
						s_bIncorrectPackage = true;
					}
				}
			}
			else
			{
				if (nField == 1)
					InitDataElement(pRecHld, pBuffer+anFieldPos[0], anFieldSize[0],
						pBuffer+anFieldPos[1], anFieldSize[1]);
				else
					InitDataElement(pRecHld, pBuffer+anFieldPos[0], anFieldSize[0],
						pBuffer+anFieldPos[1], anFieldSize[1], pBuffer+anFieldPos[2],
						anFieldSize[2]);
			}

			anFieldPos[0] = i+1;
			nField = 0;
		}
	}
	if (s_AbacusSetup.nDebugLog) dprintf(LOG_FILE, "Data package parsed, adding int list.. ");
	EnterCriticalSection(&s_cs);
	if (s_pLastAbacusRecord)
		s_pLastAbacusRecord->pNext = pRecHld;
	pRecHld->pPrev = s_pLastAbacusRecord;
	s_pLastAbacusRecord = pRecHld;
	if (s_pFirstAbacusRecord == NULL)
		s_pFirstAbacusRecord = pRecHld;
	LeaveCriticalSection(&s_cs);
	if (s_AbacusSetup.nDebugLog) dprintf(LOG_FILE, "added\n");
	return true;
}

static bool ParseRBCPackage(unsigned char *pBuffer, unsigned nSize)
{
	return true;
}

static bool ParseWBCPackage(unsigned char *pBuffer, unsigned nSize)
{
	return true;
}

static bool ParsePLTPackage(unsigned char *pBuffer, unsigned nSize)
{
	return true;
}

static void AcknowledgePackage(unsigned char *pReply, int *pnReplySize,
							   unsigned char chReq, unsigned char chMID)
{
	pReply[0] = 6; // Ack
	pReply[1] = chReq;
	pReply[2] = chMID;
	*pnReplySize = 3;
}

static bool ParsePackage(unsigned char *pBuffer, unsigned nSize)
{
	if (pBuffer[0] != 1) // SOH
	{
		dprintf(LOG_FILE, "Invalid package: no SOH byte\n");
		return false;
	}
	unsigned char chMID,chCMD;
	chMID = pBuffer[1];
	chCMD = pBuffer[2];
	if (chCMD != 'I' && chCMD != 'D' && chCMD != 'R' && chCMD != 'W' &&
		chCMD != 'P')
	{
		dprintf(LOG_FILE, "Invalid package type %c\n", chCMD);
		return false;
	}
	if (pBuffer[3] != 2) // STX
	{
		dprintf(LOG_FILE, "Invalid package: no STX byte\n");
		return false;
	}
	unsigned nETXPos = 0;
	for (unsigned i = 4; i < nSize; i++)
	{
		if (pBuffer[i] == 3) // ETX
		{
			nETXPos = i;
			break;
		}
	}
	if (nETXPos == 0)
	{
		dprintf(LOG_FILE, "Invalid package: no ETX byte\n");
		return false;
	}
	unsigned char szReply[10];
	int nReplySize;
	switch( chCMD )
	{
	case 'I':
		if (s_AbacusSetup.nDebugLog) dprintf(LOG_FILE, "init package\n");
		ParseInitPackage(pBuffer+4, nETXPos-4);
		if (s_AbacusSetup.nDebugLog) dprintf(LOG_FILE, "Acknowledging.. ");
		AcknowledgePackage(szReply, &nReplySize, ' ', chMID);
		if (!g_Comm.Write(nReplySize, szReply))
		{
			dprintf(LOG_FILE, "Error during reply to init package: WriteFile"
				" failed(%d)\n", GetLastError());
			return false;
		}
		if (s_AbacusSetup.nDebugLog) dprintf(LOG_FILE, "OK\n");
		break;
	case 'D':
		if (s_AbacusSetup.nDebugLog) dprintf(LOG_FILE, "data package\n");
		ParseDataPackage(pBuffer+4, nETXPos-4);
		if (s_AbacusSetup.nDebugLog) dprintf(LOG_FILE, "Acknowledging.. ");
		AcknowledgePackage(szReply, &nReplySize, s_AbacusSetup.bRequestRBC?'R':' ', chMID);
		if (!g_Comm.Write(nReplySize, szReply))
		{
			dprintf(LOG_FILE, "Error during reply to data package: WriteFile"
				" failed(%d)\n", GetLastError());
			return false;
		}
		if (s_AbacusSetup.nDebugLog) dprintf(LOG_FILE, "OK\n");
		break;
	case 'R':
		ParseRBCPackage(pBuffer+4, nETXPos-4);
		if (s_AbacusSetup.nDebugLog) dprintf(LOG_FILE, "Acknowledging.. ");
		AcknowledgePackage(szReply, &nReplySize, s_AbacusSetup.bRequestWBC?'W':' ', chMID);
		if (!g_Comm.Write(nReplySize, szReply))
		{
			dprintf(LOG_FILE, "Error during reply to RBC package: WriteFile"
				" failed(%d)\n", GetLastError());
			return false;
		}
		if (s_AbacusSetup.nDebugLog) dprintf(LOG_FILE, "OK\n");
		break;
	case 'W':
		ParseWBCPackage(pBuffer+4, nETXPos-4);
		if (s_AbacusSetup.nDebugLog) dprintf(LOG_FILE, "Acknowledging.. ");
		AcknowledgePackage(szReply, &nReplySize, s_AbacusSetup.bRequestPLT?'P':' ', chMID);
		if (!g_Comm.Write(nReplySize, szReply))
		{
			dprintf(LOG_FILE, "Error during reply to WBC package: WriteFile"
				" failed(%d)\n", GetLastError());
			return false;
		}
		if (s_AbacusSetup.nDebugLog) dprintf(LOG_FILE, "OK\n");
		break;
	case 'P':
		ParsePLTPackage(pBuffer+4, nETXPos-4);
		if (s_AbacusSetup.nDebugLog) dprintf(LOG_FILE, "Acknowledging.. ");
		AcknowledgePackage(szReply, &nReplySize, ' ', chMID);
		if (!g_Comm.Write(nReplySize, szReply))
		{
			dprintf(LOG_FILE, "Error during reply to PLT package: WriteFile"
				" failed(%d)\n", GetLastError());
			return false;
		}
		if (s_AbacusSetup.nDebugLog) dprintf(LOG_FILE, "OK\n");
		break;
	};
	return true;
}

static DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	if (s_AbacusSetup.nDebugLog)
	{
		time_t t;
		t = time(&t);
		struct tm *tm1 = localtime(&t);
		dprintf(LOG_FILE, "\nAbacus controller started, %d/%d/%d %d:%d:%d\n",
			(int)tm1->tm_mday, (int)tm1->tm_mon+1, (int)tm1->tm_year+1900,
			(int)tm1->tm_hour, (int)tm1->tm_min, (int)tm1->tm_sec);
	}
	s_AbacusState = AbacusState_Connecting;
	g_Comm.Open();
	if (!g_Comm.IsOpened())
	{
		if (s_AbacusSetup.nDebugLog)
			dprintf(LOG_FILE, "Com port not opened, error %d\n", GetLastError());
		s_nErrorCode = -1;
		s_AbacusState = AbacusState_Error;
		return s_nErrorCode;
	}
	if (s_AbacusSetup.nDebugLog)
		dprintf(LOG_FILE, "Port opened. Connecting.\n");
	BYTE by = 5; //ENQ
	if (!g_Comm.Write(1, &by))
	{
		if (s_AbacusSetup.nDebugLog)
			dprintf(LOG_FILE, "Error writing ENQ, error %d\n", GetLastError());
		s_nErrorCode = -2;
		s_AbacusState = AbacusState_Error;
		return s_nErrorCode;
	}
	DWORD dwStart = GetTickCount();
	BOOL bSuccess = FALSE;
	do
	{
		bSuccess = g_Comm.Read(1, &by);
	}
	while (!bSuccess && GetTickCount()-dwStart < (unsigned)s_AbacusSetup.nConnectTimeoutMs);
	if (!bSuccess)
	{
		if (s_AbacusSetup.nDebugLog)
			dprintf(LOG_FILE, "Other side not responding after ENQ, error %d\n", GetLastError());
		s_nErrorCode = -3;
		s_AbacusState = AbacusState_Error;
		return s_nErrorCode;
	}
	if (by != 6) // ACQ
	{
		if (s_AbacusSetup.nDebugLog)
			dprintf(LOG_FILE, "Other side responded %d after ENQ\n", (int)by);
		s_nErrorCode = -4;
		s_AbacusState = AbacusState_Error;
		return s_nErrorCode;
	}
	if (s_AbacusSetup.nDebugLog)
		dprintf(LOG_FILE, "Initialization succeeded, waiting for packages\n");

	s_AbacusState = AbacusState_Connected;
	std::auto_ptr<unsigned char> PackageBuffer(new unsigned char[
		s_AbacusSetup.nPackageBuffSize]);
	while (!s_dwStop)
	{
		unsigned nPackageSize;
		if (s_AbacusSetup.nDebugLog)
			dprintf(LOG_FILE, "Waiting for next package\n");
		if (!ReadPackage(PackageBuffer.get(), s_AbacusSetup.nPackageBuffSize, &nPackageSize))
			break; // Error already logged
		if (s_AbacusSetup.nDebugLog)
		{
			LogPackage(PackageBuffer.get(), nPackageSize);
			dprintf(LOG_FILE, "Parsing.. ");
		}
		bool bValidPackage = ParsePackage(PackageBuffer.get(), nPackageSize);
//		if ((!bValidPackage || s_bIncorrectPackage) && s_AbacusSetup.nDebugLog)
//			LogPackage(PackageBuffer.get(), nPackageSize);
		if (!bValidPackage)
		{
			if (s_AbacusSetup.nDebugLog)
				dprintf(LOG_FILE, "Session terminated with error\n");
			s_AbacusState = AbacusState_Error;
			break;
		}
		s_nLastRecordNum++;
	}
	s_AbacusState = AbacusState_NotConnected;
	g_Comm.Close();
	return 0;
};


bool AbacusInitialize()
{
	if (!s_bInitialized)
	{
		InitializeCriticalSection(&s_cs);
		s_bInitialized = true;
	}
	return true;
}

ABACUSCTRL_API bool AbacusConnect()
{
	if (s_hThread != NULL)
		return true;
	AbacusInitialize();
	s_dwStop = 0;
	s_hThread = CreateThread(NULL, 0, ThreadProc, NULL, 0, &s_dwThreadId);
	return true;
}

ABACUSCTRL_API void AbacusDisconnect()
{
	if (s_hThread == NULL)
		return;
	InterlockedIncrement(&s_dwStop);
	WaitForSingleObject(s_hThread, INFINITE);
	s_hThread = NULL;
	FreeAllDataRecords();
	FreeInitRecord();
}

ABACUSCTRL_API int  AbacusGetState()
{
	return s_AbacusState;
}

ABACUSCTRL_API bool AbacusGetInit(ABACUSINIT **ppAbacusInit)
{
	*ppAbacusInit = &s_AbacusInit;
	return true;
}

ABACUSCTRL_API bool AbacusGetFirstRecordPosition(POSITION *plPos)
{
	*plPos = (POSITION)s_pFirstAbacusRecord;
	return true;
}

ABACUSCTRL_API bool AbacusGetNextRecord(POSITION *plPos, ABACUSRECORD **ppRecord)
{
	AbacusRecordHolder *p = (AbacusRecordHolder *)*plPos;
	*ppRecord = &p->ar;
	*plPos = (POSITION)p->pNext;
	return true;
}

ABACUSCTRL_API void AbacusRemoveRecord(POSITION lPos)
{
	EnterCriticalSection(&s_cs);
	AbacusRecordHolder *p = (AbacusRecordHolder *)lPos;
	if (p->pPrev)
		p->pPrev->pNext = p->pNext;
	if (p->pNext)
		p->pNext->pPrev = p->pPrev;
	if (p == s_pLastAbacusRecord)
		s_pLastAbacusRecord = p->pPrev;
	if (p == s_pFirstAbacusRecord)
		s_pFirstAbacusRecord = p->pNext;
	FreeDataRecord(p);
	delete p;
	LeaveCriticalSection(&s_cs);
}

ABACUSCTRL_API void AbacusGetSetup(ABACUSCONTROLSETUP *pSetup)
{
	*pSetup = s_AbacusSetup;
}

ABACUSCTRL_API void AbacusSetSetup(ABACUSCONTROLSETUP *pSetup)
{
	s_AbacusSetup = *pSetup;
}

extern COMMSETUP g_CommSetup;

ABACUSCTRL_API void AbacusGetCommSetup(COMMSETUP *pCommSetup)
{
	*pCommSetup = g_CommSetup;
}

ABACUSCTRL_API void AbacusSetCommSetup(COMMSETUP *pCommSetup)
{
	g_CommSetup = *pCommSetup;
}

ABACUSCTRL_API int AbacusGetLastRecordNum()
{
	return s_nLastRecordNum;
}
