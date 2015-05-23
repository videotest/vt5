#ifndef __aliase_int_h__
#define __aliase_int_h__

#include "defs.h"


interface IAliaseMan : public IUnknown
{
	com_call LoadFile( BSTR bstrFileName ) = 0;
	com_call StoreFile( BSTR bstrFileName ) = 0;

	com_call GetActionString( BSTR bstrAction, BSTR *pbstrString ) = 0;
	com_call GetObjectString( BSTR bstrObject, BSTR *pbstrString ) = 0;

	com_call IsReadOnly() = 0;
};

declare_interface(IAliaseMan, "038D09F7-A29F-4a12-81D5-8224FEE5386F")

interface IAliaseMan2 : public IAliaseMan
{
	com_call GetActionArgumentString( BSTR bstrAction, BSTR bstrArgument, BSTR *pbstrString ) = 0;
};

declare_interface(IAliaseMan2, "DCC508D1-3619-4560-ACB5-6E6448E611A5")


#endif  //__aliase_int_h__