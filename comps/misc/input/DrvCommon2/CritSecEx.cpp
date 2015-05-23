#include "StdAfx.h"
#include "CritSecEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool CCriticalSectionEx::s_bMultythreaded = false;

CCriticalSectionEx::CCriticalSectionEx()
{
	m_bMultithreaded = true;
	InitializeCriticalSection(&m_cs);
}

void CCriticalSectionEx::Enter()
{
	if (s_bMultythreaded && m_bMultithreaded)
		EnterCriticalSection(&m_cs);
}

void CCriticalSectionEx::Leave()
{
	if (s_bMultythreaded && m_bMultithreaded)
		LeaveCriticalSection(&m_cs);
}


