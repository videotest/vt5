#ifndef __impl_message_h__
#define __impl_message_h__

#include "window5.h"


class CMessageFilterImpl : public IMsgListener
{
public:
	virtual LRESULT	DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );
	com_call OnMessage( MSG *pmsg, LRESULT *plReturn );
};

#endif //__impl_message_h__