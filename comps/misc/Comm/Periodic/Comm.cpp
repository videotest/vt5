#include "StdAfx.h"
#include "Comm.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CComm g_Comm;


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
	int iBaud = ::GetValueInt(::GetAppUnknown(), "Comm", "Baud", CBR_9600);
	int iStopBits = ::GetValueInt(::GetAppUnknown(), "Comm", "StopBits", ONESTOPBIT);
	int iParity = ::GetValueInt(::GetAppUnknown(), "Comm", "Parity", EVENPARITY);
	int iDataBits = ::GetValueInt(::GetAppUnknown(), "Comm", "DataBits", 8);
	m_bstrCom = ::GetValueString(::GetAppUnknown(), "Comm", "Port", "Com1");
	m_hFile = CreateFile(m_bstrCom, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		_bstr_t s = _bstr_t("Cannot open ") + m_bstrCom;
		AfxMessageBox(s, MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	SetupComm(m_hFile, 4096, 4096);
	PurgeComm(m_hFile, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
	COMMTIMEOUTS  CommTimeOuts ;
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF ;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0 ;
	CommTimeOuts.ReadTotalTimeoutConstant = ::GetValueInt(::GetAppUnknown(), "Comm", "Timeout", 1000);
	// CBR_9600 is approximately 1byte/ms. For our purposes, allow
	// double the expected time per character for a fudge factor.
	CommTimeOuts.WriteTotalTimeoutMultiplier = 2*CBR_9600/iBaud;
	CommTimeOuts.WriteTotalTimeoutConstant = 0 ;
	SetCommTimeouts( m_hFile, &CommTimeOuts ) ;
	DCB        dcb ;
	dcb.DCBlength = sizeof( DCB ) ;
	GetCommState( m_hFile, &dcb ) ;
	dcb.BaudRate = iBaud;
	dcb.ByteSize = iDataBits;
	dcb.Parity = iParity;
	dcb.StopBits = iStopBits;
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

bool CComm::Write(unsigned nBytes, LPBYTE lpByte)
{
	DWORD dwRead;
	return WriteFile(m_hFile, lpByte, nBytes, &dwRead, NULL) && dwRead == nBytes;
}

