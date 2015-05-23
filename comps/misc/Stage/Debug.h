#if !defined(__Debug_H__)
#define __Debug_H__

#if !defined(__LOGFILE__)
#define __LOGFILE__ "c:\\log.log"
#endif

#if !defined(__DEBUG_SECTION__)
#define __DEBUG_SECTION__ "debug"
#endif

void dprintf(char * szFormat, ...);
void dflush();

class CTracePoint
{
public:
	DWORD m_dwTotal;
	DWORD m_dwCur;
	DWORD m_dwEnd;
	CTracePoint()
	{
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
public:
	CTracePoint *m_pPrev;
	CTracePoint1(CTracePoint *pPrev = NULL)
	{
		m_dwTotal = m_dwCur = 0;
		m_pPrev = pPrev;
	}
	void start()
	{
		m_pPrev->stop();
		m_dwCur = m_pPrev->m_dwEnd;
	}
};

#endif
