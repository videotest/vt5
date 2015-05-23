#ifndef __histint_h__
#define __histint_h__

#include "defs.h"


interface IProvideHistColors : public IUnknown
{
	com_call GetHistColors( int nPaneNo, COLORREF *pcrArray, COLORREF *pcrCurve ) = 0;	//sizeof( cr ) = 256
};

declare_interface( IProvideHistColors, "A64E2937-CA76-4527-97C3-CBD0778DC09E" );

#endif //__histint_h__