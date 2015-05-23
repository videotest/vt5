#include "StdAfx.h"
#include "Timer.h"

CTimer *CTimer::m_pTimer = NULL;

void CALLBACK EXPORT TimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	if (CTimer::m_pTimer)
		CTimer::m_pTimer->OnTimer();
}

CTimer::CTimer()
{
	m_pTimer = this;
}

CTimer::~CTimer()
{
	m_pTimer = NULL;
}

bool CTimer::_SetTimer(DWORD dwElapse)
{
	m_nID = SetTimer(NULL, 1, dwElapse, TimerProc);
	return m_nID >= 0;
}

void CTimer::_KillTimer()
{
	KillTimer(NULL, m_nID);
}
