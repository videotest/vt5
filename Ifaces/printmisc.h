#ifndef __printmisc_h__
#define __printmisc_h__

interface IPrintObjectSupport : public IUnknown
{	
	com_call GetMatchType( BSTR bstrObjectType, DWORD *pdwMatch ) = 0;
};


#endif //__printmisc_h__
