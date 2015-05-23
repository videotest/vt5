#include "stdafx.h"
#include "Timetest.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////cpp
 
CTimeTest::CTimeTest( bool bStart, CString s, bool bModal )
{
 m_str = s;
 m_bModalReport = bModal;
 m_dwTotal = 0;
 m_bRunning = false;
 
 if( bStart )
  Start();
}
 
CTimeTest::~CTimeTest()
{
 Report();
}
 
void CTimeTest::Init()
{
 Stop();
 m_dwTotal = 0;
}
void CTimeTest::Start()
{
 if( m_bRunning )
  return;
 m_bRunning = true;
 
 m_dwStart = GetTickCount();
}
 
void CTimeTest::Stop()
{
 if( !m_bRunning )
  return;
 
 DWORD dw = GetTickCount()-m_dwStart;
 m_bRunning = false;
 
 m_dwTotal += dw;
}
 
void CTimeTest::Report(bool stop)
{
	if (stop)
		Stop();
	else
	{
		DWORD dw = GetTickCount()-m_dwStart;
		m_dwTotal += dw;
	}
 
 CString s;
 
 s.Format( "%s - time %d\n", (LPCTSTR)m_str, m_dwTotal );
 
 if( m_bModalReport )
  AfxMessageBox( s );
 else
  OutputDebugString( s );
}

