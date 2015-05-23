#include "defs.h"

#ifndef __timetest_h__
#define __timetest_h__
/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////header
class std_dll CTimeTest
{
 bool m_bModalReport;
 bool m_bRunning;
  
 DWORD m_dwTotal;
 DWORD m_dwStart;
 CString m_str;
 
public:
 CTimeTest( bool bStart = true, CString s = "Time Test", bool bModal = false );
 ~CTimeTest();

 bool m_bEnableFileOutput;

public:
 void Init(bool stop = true);
 void Start();
 void Stop();
 void Report(bool stop = true);
};

#endif //__timetest_h__