#include "stdafx.h"
#include "impl_message.h"

//message map
HRESULT CMessageFilterImpl::OnMessage( MSG *pmsg, LRESULT *plReturn )
{
	*plReturn = DoMessage( pmsg->message, pmsg->wParam, pmsg->lParam );
	return *plReturn?S_FALSE:S_OK;
}

LRESULT	CMessageFilterImpl::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	return 0;
}
