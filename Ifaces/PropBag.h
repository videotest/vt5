#ifndef __PropBag_h__
#define __PropBag_h__

#include "defs.h"

interface INamedPropBag : public IUnknown
{
	com_call SetProperty( BSTR bstrName, VARIANT var ) = 0;
	com_call GetProperty( BSTR bstrName, VARIANT* pvar ) = 0;
	com_call GetFirstPropertyPos( long* plos ) = 0;
	com_call GetNextProperty (BSTR* pbstrName, VARIANT* pvar, long* plos ) = 0;
	com_call DeleteProperty (long lPos) = 0;
};

declare_interface( INamedPropBag, "B730E264-09C4-4262-A80F-8D6598F086B4" )

interface INamedPropBagSer : public INamedPropBag
{
	com_call Load( IStream *pStream ) = 0;
	com_call Store( IStream *pStream ) = 0;
};

declare_interface( INamedPropBagSer, "0126B162-B90F-49a3-83AF-FDBC2A5396BD" )

#endif //__PropBag_h__
