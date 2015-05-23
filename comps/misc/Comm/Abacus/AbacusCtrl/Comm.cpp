#include "StdAfx.h"
#include "Comm.h"
#include "AbacusI.h"
#include <stdio.h>
//#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CComm g_Comm;
COMMSETUP g_CommSetup = {CBR_9600, ONESTOPBIT, NOPARITY, 8, 1, "Com1"};


CComm::CComm()
{
	m_hFile = INVALID_HANDLE_VALUE;
}

CComm::~CComm()
{
	Close();
}

void CComm::Open()
{
	if (m_hFile != INVALID_HANDLE_VALUE)
		return;
	m_hFile = CreateFile(g_CommSetup.szComm, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		char szMessage[256];
		sprintf(szMessage, "Can not open %s", g_CommSetup.szComm);
		MessageBox(NULL, szMessage, "Abacus", MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	SetupComm(m_hFile, 4096, 4096);
	PurgeComm(m_hFile, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
	COMMTIMEOUTS  CommTimeOuts ;
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF ;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0 ;
	CommTimeOuts.ReadTotalTimeoutConstant = g_CommSetup.nReadTotalTimeoutConstant;
	// CBR_9600 is approximately 1byte/ms. For our purposes, allow
	// double the expected time per character for a fudge factor.
	CommTimeOuts.WriteTotalTimeoutMultiplier = 2*CBR_9600/g_CommSetup.nBaud;
	CommTimeOuts.WriteTotalTimeoutConstant = 0 ;
	SetCommTimeouts( m_hFile, &CommTimeOuts ) ;
	DCB        dcb ;
	dcb.DCBlength = sizeof( DCB ) ;
	GetCommState( m_hFile, &dcb ) ;
	dcb.BaudRate = g_CommSetup.nBaud;
	dcb.ByteSize = g_CommSetup.nDataBits;
	dcb.Parity = g_CommSetup.nParity;
	dcb.StopBits = g_CommSetup.nStopBits;
	// setup hardware flow control
	dcb.fBinary = TRUE;
	dcb.fParity = TRUE;
	SetCommState(m_hFile, &dcb);
}

void CComm::Close()
{
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		PurgeComm(m_hFile, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

bool CComm::Read(unsigned nBytes, LPBYTE lpByte)
{
	DWORD dwRead;
	return ReadFile(m_hFile, lpByte, nBytes, &dwRead, NULL) && dwRead == nBytes;
}

bool CComm::ReadEx(unsigned nBytes, LPBYTE lpByte, DWORD *pnRead)
{
	return ReadFile(m_hFile, lpByte, nBytes, pnRead, NULL) == TRUE;
}

bool CComm::Write(unsigned nBytes, LPBYTE lpByte)
{
	DWORD dwRead;
	return WriteFile(m_hFile, lpByte, nBytes, &dwRead, NULL) && dwRead == nBytes;
}

void CComm::Purge()
{
	PurgeComm(m_hFile,PURGE_TXCLEAR|PURGE_RXCLEAR);
}

