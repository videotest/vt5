#ifndef __binimageint_h__
#define __binimageint_h__

#include "defs.h"

//serializible or text object impl
interface IBinaryImage : public IUnknown
{
	com_call CreateNew( int cx, int cy ) = 0;
	com_call GetRow( byte **prow, int nRow, BOOL bWithDWORDBorders ) = 0;
	com_call GetSizes( int *pcx, int *pcy ) = 0;
	com_call Free() = 0;
	com_call GetOffset( POINT *pt ) = 0;
	com_call SetOffset( POINT pt, BOOL bMoveImage ) = 0;
};
declare_interface( IBinaryImage, "BDE0A825-80BE-41ee-A394-03C162D436FF" );

#endif// __binimageint_h__