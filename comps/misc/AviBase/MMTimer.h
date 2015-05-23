// MMTimer.h: interface for the CMMTimer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MMTIMER_H__958B18E9_3EEC_498E_B73E_47779F3872C1__INCLUDED_)
#define AFX_MMTIMER_H__958B18E9_3EEC_498E_B73E_47779F3872C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMMTimer  
{
public:
	CMMTimer();
	virtual ~CMMTimer();

	void StartTimer( long lPeriod, DWORD dwData, LPTIMECALLBACK lpf );
	void StopTimer( );
	UINT GetTimerID(){ return m_uiTimer;}

protected:	
	DWORD		m_dwUser;
	long		m_lPeriod;
	UINT		m_uiTimer;


};

#endif // !defined(AFX_MMTIMER_H__958B18E9_3EEC_498E_B73E_47779F3872C1__INCLUDED_)
