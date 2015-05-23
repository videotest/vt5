#pragma once


class CCriticalSectionEx
{
public:
	static bool s_bMultythreaded;
	bool m_bMultithreaded;
	CRITICAL_SECTION m_cs;

	CCriticalSectionEx();
	void Enter();
	void Leave();
};

class CCritSectionAccess
{
	CCriticalSectionEx *m_pcs;
public:
	CCritSectionAccess(CCriticalSectionEx *pcs)
	{
		m_pcs = pcs;
		m_pcs->Enter();
	}
	~CCritSectionAccess()
	{
		m_pcs->Leave();
	}
};