#include "stdafx.h"

#include "core5.h"
#include "data5.h"

#include "impl_long.h"
#include "nameconsts.h"
#include "com_main.h"
#include "misc_utils.h"



int	g_nNotify = 1;

CLongOperationImpl::CLongOperationImpl()
{
	m_nStagesCount = 0;
	m_nCurrentStage = 0;
	m_nNotifyCount = 0;
	m_nCurrentNotifyPos = 0;
	m_nPercrent = 0;
	m_hwndNotify = 0;
	m_bNeedToTerminate  = false;

}

CLongOperationImpl::~CLongOperationImpl()
{
}

bool CLongOperationImpl::StartNotification( int nCYCount, int nStageCount, int nNotifyFreq )
{
	g_nNotify = GetValueInt( App::application(), "\\General", "EnableFilterNotify", g_nNotify );

	m_bNeedToTerminate  = false;
	if( !m_hwndNotify )
	{
		IApplicationPtr	ptrA( App::application() );
		ptrA->GetMainWindowHandle( &m_hwndNotify );
	}

	m_nNotifyCount = nCYCount;
	m_nStagesCount = nStageCount;
	m_nNotifyFreq = nNotifyFreq;

	dbg_assert(m_nStagesCount>=1);
	dbg_assert(m_nNotifyCount>=0);
	
	if( m_nNotifyFreq == -1 )
		m_nNotifyFreq = 30;//m_nNotifyCount/20;

	m_nCurrentStage = 0;
	m_nCurrentNotifyPos = 0;
	m_nPercrent = 0;

	::SendMessage( m_hwndNotify, WM_NOTIFYLONGOPERATION, loaStart, (LPARAM)(IUnknown*)this) ;

	return true;
}

bool CLongOperationImpl::Notify( int nPos )
{
	m_nCurrentNotifyPos = nPos;

	dbg_assert( m_nCurrentNotifyPos < m_nNotifyCount );

	m_nPercrent  = 100*(m_nCurrentStage*m_nNotifyCount+m_nCurrentNotifyPos)/max( m_nStagesCount*m_nNotifyCount, 1);

	if( !g_nNotify )
		return true;

	if( (nPos % m_nNotifyFreq) == 0 )
		::SendMessage( m_hwndNotify, WM_NOTIFYLONGOPERATION, loaSetPos, (LPARAM)(IUnknown*)this);

	if( m_bNeedToTerminate )
	{
		::SendMessage( m_hwndNotify, WM_NOTIFYLONGOPERATION, loaTerminate, (LPARAM)(IUnknown*)this);
		throw new TerminateLongException();
	}
	return !m_bNeedToTerminate;
}

bool CLongOperationImpl::NextNotificationStage()
{
	m_nCurrentNotifyPos = 0;
	m_nCurrentStage++;

	m_nCurrentStage = min( m_nCurrentStage, m_nStagesCount-1 );

	m_nPercrent = 100*(m_nCurrentStage*m_nNotifyCount+m_nCurrentNotifyPos)/max( m_nStagesCount*m_nNotifyCount, 1);

	::SendMessage( m_hwndNotify, WM_NOTIFYLONGOPERATION, loaSetPos, (LPARAM)(IUnknown*)this);

	return true;
}

bool CLongOperationImpl::FinishNotification()
{
	m_nCurrentNotifyPos = m_nNotifyCount-1;
	m_nCurrentStage = m_nStagesCount-1;
	m_nPercrent = 100;

	::SendMessage( m_hwndNotify, WM_NOTIFYLONGOPERATION, loaComplete, (LPARAM)(IUnknown*)this);

	return true;
}

bool CLongOperationImpl::TerminateNotification()
{
	::SendMessage( m_hwndNotify, WM_NOTIFYLONGOPERATION, loaTerminate, (LPARAM)(IUnknown*)this);
	return true;
}

void CLongOperationImpl::_Abort()
{
	::SendMessage( m_hwndNotify, WM_NOTIFYLONGOPERATION, loaTerminate, (LPARAM)(IUnknown*)this);
	throw new TerminateLongException();
}

HRESULT CLongOperationImpl::AttachNotifyWindow( HWND hWndNotifyWindow )
{
	m_hwndNotify = hWndNotifyWindow;
	return S_OK;
}

HRESULT CLongOperationImpl::GetCurrentPosition( int *pnStage, int *pnPosition, int *pnPercent )
{
	if( pnStage )
		*pnStage = m_nCurrentStage;
	if( pnPosition )
		*pnPosition = m_nCurrentNotifyPos;
	if( pnPercent )
		*pnPercent = m_nPercrent;

		return S_OK;
}

HRESULT CLongOperationImpl::GetNotifyRanges( int *pnStageCount, int *pnNotifyCount )
{
	if( pnStageCount )
		*pnStageCount = m_nStagesCount;
	if( pnNotifyCount )
		*pnNotifyCount = m_nNotifyCount;
	return S_OK;
}

HRESULT CLongOperationImpl::Abort()
{
	m_bNeedToTerminate = true;
	//_Abort();
	return S_OK;
}
