#include "StdAfx.h"
#include "CamThread.h"
#include "CritSecEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DWORD WINAPI ThreadRoutine(LPVOID lpParam)
{
	CCamThread *p = (CCamThread *)lpParam;
	p->ThreadRoutine();
	return 0;
}


CCamThread::CCamThread()
{
	m_hEventStop = NULL;
	m_hThread = NULL;
	CCriticalSectionEx::s_bMultythreaded = true;
}

CCamThread::~CCamThread()
{
	Stop();
}

void CCamThread::Start()
{
	if (m_hThread == NULL)
	{
		m_hEventStop = CreateEvent(NULL, TRUE, FALSE, NULL);
		DWORD dwId;
		m_hThread = CreateThread(NULL, 0, ::ThreadRoutine, this, 0, &dwId);
	}
}

void CCamThread::Stop()
{
	if (m_hThread != NULL)
	{
		SetEvent(m_hEventStop);
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hEventStop);
		m_hEventStop = NULL;
		m_hThread = NULL;
	}
}

bool CCamThread::IsStarted()
{
	return m_hThread != NULL;
}

bool CCamThread::WaitForEvent(HANDLE hEvent)
{
	HANDLE aEvents[2];
	aEvents[0] = m_hEventStop;
	aEvents[1] = hEvent;
	DWORD dw = WaitForMultipleObjects(2, aEvents, FALSE, INFINITE);
	return dw == WAIT_OBJECT_0;
}



CCamThread2::CCamThread2()
{
	m_hThread = NULL;
	CCriticalSectionEx::s_bMultythreaded = true;
	m_lStopFlag = 0;
}

CCamThread2::~CCamThread2()
{
	Stop();
}

void CCamThread2::Start()
{
	if (m_hThread == NULL)
	{
		DWORD dwId;
		m_hThread = CreateThread(NULL, 0, ::ThreadRoutine, this, 0, &dwId);
		//sergey 03.04.06
		//SetThreadPriority(m_hThread, THREAD_PRIORITY_TIME_CRITICAL);
		//end
	}
}

void CCamThread2::Stop()
{
	if (m_hThread != NULL)
	{
		InterlockedIncrement(&m_lStopFlag);
		WaitForSingleObject(m_hThread, INFINITE);
		m_hThread = NULL;
		m_lStopFlag = 0;
	}
}

bool CCamThread2::IsStarted()
{
	return m_hThread != NULL;
}
