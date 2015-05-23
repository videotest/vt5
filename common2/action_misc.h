#pragma once

#include "action_main.h"

class CActionShowView: public CAction
{
protected:
	com_call DoInvoke();
	com_call GetActionState( DWORD *pdwState );
	
	virtual _bstr_t GetViewProgID() = 0;
};


