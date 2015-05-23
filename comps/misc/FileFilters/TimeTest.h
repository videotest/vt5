
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////header
class CTimeTest
{
 bool m_bModalReport;
 bool m_bRunning;
 
 DWORD m_dwTotal;
 DWORD m_dwStart;
 CString m_str;
 
public:
 CTimeTest( bool bStart = true, CString s = "Time Test", bool bModal = false );
 ~CTimeTest();
 
public:
 void Init();
 void Start();
 void Stop();
 void Report(bool stop = true);
};

 