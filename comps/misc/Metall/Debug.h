#if !defined(__Debug_H__)
#define __Debug_H__

#if !defined(__PROJECTNAME__)
#define __PROJECTNAME__ "metall"
#endif

#if !defined(__LOGFILE__)
#define __LOGFILE__ "c:\\log.log"
#endif

#if !defined(__DEBUG_SECTION__)
#define __DEBUG_SECTION__ "debug"
#endif

void dprintf(char * szFormat, ...);
void dflush();

//#define __TIME_TRACE
#if defined(__TIME_TRACE)

#define TimeTrace(p) CTimeTrace tt(p)

class CTimeTrace
{
	const char *m_pszFuncName;
	DWORD m_dwBeg;
public:
	CTimeTrace(const char *pszFuncName)
	{
		m_dwBeg = GetTickCount();
		m_pszFuncName = pszFuncName;
	}
	~CTimeTrace()
	{
		DWORD dw = GetTickCount();
		char szBuff[256];
		sprintf(szBuff, "%s : %d\n", m_pszFuncName, dw-m_dwBeg);
		OutputDebugString(szBuff);
	}
};

class CTracePoint
{
	friend class CTracePointsArray;
public:
	const char *m_pszFuncName;
	DWORD m_dwTotal;
	DWORD m_dwCur;
	DWORD m_dwEnd;
	CTracePoint(const char *pszFuncName = NULL)
	{
		m_pszFuncName = pszFuncName;
		m_dwTotal = m_dwCur = 0;
	}
	void start()
	{
		m_dwCur = GetTickCount();
	}
	void stop()
	{
		m_dwEnd = GetTickCount();
		m_dwTotal += m_dwEnd - m_dwCur;
	}
};

class CTracePoint1 : public CTracePoint
{
	friend class CTracePointsArray;
public:
	CTracePoint1 *m_pPrev;
	CTracePoint1(const char *pszFuncName = NULL, CTracePoint1 *pPrev = NULL) : 
		CTracePoint(pszFuncName)
	{
		m_dwTotal = m_dwCur = 0;
		m_pPrev = pPrev;
	}
	void start()
	{
		if (m_pPrev)
		{
			m_pPrev->stop();
			m_dwCur = m_pPrev->m_dwEnd;
		}
		else
			m_dwCur = GetTickCount();
	}
	void init()
	{
		for (CTracePoint1 *tp = this; tp; tp = tp->m_pPrev)
			tp->m_dwTotal = 0;
	}
	void make_string(char *psz)
	{
		*psz = 0;
		for (CTracePoint1 *tp = this; tp; tp = tp->m_pPrev)
		{
			char szBuff[256];
			sprintf(szBuff, "%s:%d,", tp->m_pszFuncName, tp->m_dwTotal);
			strcat(psz, szBuff);
			tp->m_dwTotal = 0;
		}
	}
	void print()
	{
		for (CTracePoint1 *tp = this; tp; tp = tp->m_pPrev)
		{
			char szBuff[256];
			sprintf(szBuff, "%s : %d\n", tp->m_pszFuncName, tp->m_dwTotal);
			OutputDebugString(szBuff);
			tp->m_dwTotal = 0;
		}
	}
	void msgbox()
	{
		char szBuff[1024];
		strcpy(szBuff,"\\");
		strcat(szBuff,__PROJECTNAME__);
		strcat(szBuff,"\\TimeTrace\\MsgBox");
		if (GetValueInt(GetAppUnknown(), szBuff, m_pszFuncName, 1))
		{
			make_string(szBuff);
			VTMessageBox(szBuff, "Debug", MB_OK|MB_ICONEXCLAMATION);
		}
	}
};

class CTracePointsArray
{
	CTracePoint1 *m_pTP;
	char *m_pNames;
	int m_nSize;
public:
	CTracePointsArray()
	{
		m_pTP = NULL;
		m_pNames = NULL;
		m_nSize = 0;
	}
	void init(const char *pszFuncName, int nSize)
	{
		delete m_pTP;
		delete m_pNames;
		m_nSize = nSize;
		m_pTP = new CTracePoint1[nSize];
		m_pNames = new char[50*nSize];
		memset(m_pNames, 0, 50*nSize);
		for (int i = 0; i < nSize; i++)
		{
			sprintf(m_pNames+50*i,"%s:%d",pszFuncName,i);
			m_pTP[i].m_pszFuncName = m_pNames+50*i;
			if (i > 0) m_pTP[i].m_pPrev = &m_pTP[i-1];
		}
	}
	void init_name(const char *pszFuncName, int i, IUnknown *punk)
	{
		INamedObject2Ptr NamedObj(punk);
		BSTR bstr;
		NamedObj->GetName(&bstr);
		_bstr_t bstr1(bstr,false);
		sprintf(m_pNames+50*i,"%s %s",pszFuncName,(const char*)bstr1);

	}
	void start(int i)
	{
		m_pTP[i].start();
	}
	void stop()
	{
		m_pTP[m_nSize-1].stop();
	}
	void msgbox()
	{
		m_pTP[m_nSize-1].msgbox();
	}
	~CTracePointsArray()
	{
		delete m_pTP;
		delete m_pNames;
	}
};

class CTimeTrace2
{
	CTracePoint *m_ptp;
public:
	CTimeTrace2(CTracePoint *ptp)
	{
		m_ptp = ptp;
		m_ptp->start();
	}
	~CTimeTrace2()
	{
		m_ptp->stop();
	}
};

#define STracePoint1(tp,x,y) static CTracePoint1 tp(x,y)
#define STracePoint1_(tp,x)  static CTracePoint1 tp(x)
#define StartTracePoint(tp) tp.start()
#define StopTracePoint(tp) tp.stop()
#define PrintTracePoint(tp) tp.print()
#define MsgBoxTracePoint(tp) tp.msgbox()

#define MTracePointsArray(tp) CTracePointsArray tp
#define TPA_InitName(tpa,ArrName,i,punk) tpa.init_name(ArrName,i,punk)
#define InitTracePointsArray(tpa,Name,Size) tpa.init(Name,Size)
#define StartTracePointInArray(tpa,i) tpa.start(i)

#else
#define STracePoint1(tp,x,y)
#define STracePoint1_(tp,x)
#define TimeTrace(p)
#define StartTracePoint(tp)
#define StopTracePoint(tp)
#define PrintTracePoint(tp)
#define MsgBoxTracePoint(tp)
#define MTracePointsArray(tp)
#define TPA_InitName(tpa,ArrName,i,punk)
#define InitTracePointsArray(tpa,Name,Size)
#define StartTracePointInArray(tpa,i)
#endif


#endif
