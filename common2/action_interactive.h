#ifndef __action_interactive__h__
#define __action_interactive__h__

#include "impl_draw.h"
#include "impl_mouse.h"
#include "impl_message.h"
#include "impl_event.h"
#include "action_main.h"

class CInteractiveAction : public CAction, 
							public CMessageFilterImpl,
							public CDrawObjectImpl,
							public CMouseImpl,
							public CEventListenerImpl,
							public IInteractiveAction
{
	route_unknown();

public:
	enum State
	{
		Inactive,
		Active,
		Ready,
		Terminated
	};
public:
	CInteractiveAction();
	virtual ~CInteractiveAction();

	virtual IUnknown *DoGetInterface( const IID &iid );
	virtual LRESULT	DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );
	virtual bool DoChar( int nChar, bool bKeyDown );
	virtual bool DoSetCursor();
protected:
	virtual bool Initialize();
	virtual bool Finalize();
	virtual void LeaveMode();
	virtual void DoActivateObjects( IUnknown *punkObject ){}
public:
	com_call BeginInteractiveMode();
	com_call TerminateInteractive();
	com_call IsComplete();

protected:
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
protected:
	State	m_state;
	HWND	m_hwnd;
	HCURSOR	m_hcurDefault;
};

#endif //__action_interactive__h__
