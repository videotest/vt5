#ifndef __impl_event_h__
#define __impl_event_h__

#include "defs.h"
#include "misc5.h"

class CEventListenerImpl : public IEventListener
{
public:
	CEventListenerImpl();
	virtual ~CEventListenerImpl();

	void Register( IUnknown *punkNC, bool bRegister, char *pszReserved );
public:
	com_call Notify( BSTR szEventDesc, IUnknown *pHint, IUnknown *pFrom, void *pdata, long cbSize );
	com_call IsInNotify(long  *pbFlag);
protected:
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize ){}
protected:
	bool	m_bInsideNotify;
	int		m_nRegisterCount;
};


#endif //__impl_event_h__