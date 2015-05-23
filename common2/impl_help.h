#ifndef __help_impl_h__
#define __help_impl_h__

#include "defs.h"
#include "misc5.h"

class CHelpInfoImpl : public IHelpInfo2
{
public:
	com_call GetHelpInfo( BSTR *pbstrHelpFile, BSTR *pbstrHelpTopic, DWORD *pdwFlags );
	com_call GetHelpInfo2( BSTR *pbstrHelpFile, BSTR *pbstrHelpTopic, DWORD *pdwFlags, BSTR *pbstrHelpPrefix );
protected:
	bstr_t	m_bstrHelpTopic;
};

#endif // __help_impl_h__