// MMTimer.cpp: implementation of the CMMTimer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MMTimer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMMTimer::CMMTimer()
{	
	m_lPeriod	= 1;
	m_dwUser	= -1;
	m_uiTimer	= -1;
}

//////////////////////////////////////////////////////////////////////
CMMTimer::~CMMTimer()
{
	StopTimer( );
}

//////////////////////////////////////////////////////////////////////
void CMMTimer::StartTimer( long lPeriod, DWORD dwData, LPTIMECALLBACK lpf )
{
	if( lPeriod < 1 ) lPeriod = 1;
	
	if( S_OK != timeBeginPeriod( lPeriod ) )
		return;

	m_lPeriod = lPeriod;

	m_dwUser = dwData;

	m_uiTimer = timeSetEvent( m_lPeriod, 1, lpf, m_dwUser, TIME_PERIODIC );
	
}


//////////////////////////////////////////////////////////////////////
void CMMTimer::StopTimer( )
{
	if( m_uiTimer == -1 )
		return;

	timeKillEvent( m_uiTimer );

	timeEndPeriod( m_lPeriod );	

	m_dwUser	= -1;

	m_lPeriod	= -1;
	m_uiTimer	= -1;
}