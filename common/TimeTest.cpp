#include "stdafx.h"
#include "utils.h"
#include "Timetest.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////cpp
 
CTimeTest::CTimeTest( bool bStart, CString s, bool bModal )
{
 m_str = s;
 m_bModalReport = bModal;
 m_dwTotal = 0;
 m_bRunning = false;
 m_bEnableFileOutput = false;
 
 if( bStart )
  Start();
}
 
CTimeTest::~CTimeTest()
{
 Report();
}
 
void CTimeTest::Init(bool stop)
{
 if (stop)
		Stop();
	else
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

 if (m_bEnableFileOutput)
 {
	 if (::GetValueInt(GetAppUnknown(), "General", "TimeLogging", 0) == 1)
	 {
		char* pFileName = "timelogging.dat";
		CString strFileName = ::GetValueString(GetAppUnknown(), "Paths", "LogPath", "");
		if(strFileName.IsEmpty())
			strFileName = "c:\\";
		strFileName += pFileName;
		CStdioFile f;
		CFileException e;
		if(f.Open( strFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText | CFile::modeNoTruncate, &e ) != 0)
		{
			f.SeekToEnd();
			f.WriteString(s);
			f.Close();
		}
		else
		{
			OutputDebugString( "Writing to time log FAULT!" );
		}
	 }
 }
}

