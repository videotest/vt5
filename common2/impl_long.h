#ifndef __impl_long_h__
#define __impl_long_h__

#include "action5.h"



class TerminateLongException
{
public:
	TerminateLongException(){};
	
	void Delete()
	{
		delete this;
	};
};


class CLongOperationImpl : public ILongOperation
{
public:
	CLongOperationImpl();
	virtual ~CLongOperationImpl();

protected:
	com_call AttachNotifyWindow( HWND hWndNotifyWindow );
	com_call GetCurrentPosition( int *pnStage, int *pnPosition, int *pnPercent );
	com_call GetNotifyRanges( int *pnStageCount, int *pnNotifyCount );
	com_call Abort();
	
public:
	virtual bool StartNotification( int nCYCount, int nStageCount = 1, int nNotifyFreq = 10 );
	virtual bool Notify( int nPos );
	virtual bool NextNotificationStage();
	virtual bool FinishNotification();
	virtual bool TerminateNotification();
	void	_Abort();

private:
	int		m_nStagesCount;
	int		m_nCurrentStage;
	int		m_nNotifyCount;
	int		m_nCurrentNotifyPos;
	int		m_nPercrent;
	HWND	m_hwndNotify;
	long	m_nNotifyFreq;
	bool	m_bNeedToTerminate;
};

#endif