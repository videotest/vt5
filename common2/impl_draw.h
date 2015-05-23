#ifndef __impl_draw_h__
#define __impl_draw_h__

#include "object5.h"

class CDrawObjectImpl : public IDrawObject
{
public:
	CDrawObjectImpl();
	virtual ~CDrawObjectImpl();
public:
	com_call Paint( HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache );
	com_call GetRect( RECT *prect );
	com_call SetTargetWindow( IUnknown *punkTarget );
};


#endif //__impl_draw_h__