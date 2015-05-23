#include "StdAfx.h"
#include "Comm.h"
#include "resource.h"
#include "misc_utils.h"


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
	_bstr_t bstrCom = ::GetValueString(::GetAppUnknown(), "Comm", "Port", "Com1");
	m_hFile = CreateFile(bstrCom, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		_bstr_t s = _bstr_t("Cannot open ") + bstrCom;
		throw new CMessageException(s);
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

bool CComm::DoRead(InfoFieldData &Data)
{
	DWORD dwRead;
	BYTE  bVoltage;
	DWORD dwZoom = 0, dwMarkerNm = 0;
	BYTE b;
	if (::GetValueInt(GetAppUnknown(), "CommWork", "SendRequest", 0))
	{
		BYTE b = 0x55;
		if (!WriteFile(m_hFile, &b, 1, &dwRead, NULL) || dwRead != 1)
			return false;
	}
	for (int i = 0; i < 4; i++)
	{
		if (!ReadFile(m_hFile, &b, 1, &dwRead, NULL) || dwRead != 1)
			return false;
		dwZoom <<= 8;
		dwZoom |= ((DWORD)b);
	}
	if (!ReadFile(m_hFile, &bVoltage, 1, &dwRead, NULL) || dwRead != 1)
		return false;
	for (i = 0; i < 4; i++)
	{
		if (!ReadFile(m_hFile, &b, 1, &dwRead, NULL) || dwRead != 1)
			return false;
		dwMarkerNm <<= 8;
		dwMarkerNm |= ((DWORD)b);
	}
	Data.nZoom = dwZoom;
	Data.nVoltageKV = bVoltage;
	Data.dMarkerNm = double(dwMarkerNm)/10.;
	return true;
}

bool CComm::ReadInfoField(InfoFieldData &Data)
{
	_bstr_t bstr(::GetValueString(::GetAppUnknown(), "CommWork", "MicroName", "SEO TEM"));
	strncpy(Data.szMicroName, (const char *)bstr, MAX_MICRO_NAME);
	GetLocalTime(&Data.localTime);
	bool bRead = false;;
	if (::GetValueInt(GetAppUnknown(), "CommWork", "TestMode", 0) == 0)
	{
		bRead = DoRead(Data);
		if (!bRead)
			throw new CMessageException(IDS_CANNOT_READ);
	}
	if (!bRead)
	{
		Data.nVoltageKV = 100;
		Data.nZoom = 2000000;
		Data.dMarkerNm = 1.;
	}
	return true;
}

