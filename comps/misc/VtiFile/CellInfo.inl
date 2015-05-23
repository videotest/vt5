#ifndef __CellInfo_inl__
#define __CellInfo_inl__

void ClientToWnd( CRect &rect, int iZoom, CPoint ptScroll = CPoint( 0, 0 ) );
void ClientToWnd( CSize &size, int iZoom, CPoint ptScroll = CPoint( 0, 0 ) );
void ClientToWnd( CPoint &point, int iZoom, CPoint ptScroll = CPoint( 0, 0 ) );

void ClientToWnd( CRect &rect, double fZoom, CPoint ptScroll = CPoint( 0, 0 ) );

void WndToClient( CRect &rect, int iZoom, CPoint ptScroll = CPoint( 0, 0 ) );
void WndToClient( CSize &size, int iZoom, CPoint ptScroll = CPoint( 0, 0 ) );
void WndToClient( CPoint &point, int iZoom, CPoint ptScroll = CPoint( 0, 0 ) );

inline void ClientToWnd( CRect &rect, double fZoom, CPoint ptScroll )
{
	rect.left = int( (rect.left+0.5)*fZoom );
	rect.right = int( (rect.right+0.5)*fZoom );
	rect.top = int( (rect.top+0.5)*fZoom );
	rect.bottom = int( (rect.bottom+0.5)*fZoom );

	rect -= ptScroll;
}

inline void ClientToWnd( CRect &rect, int iZoom, CPoint ptScroll )
{
	ClientToWnd( rect, iZoom>0?iZoom:-1./iZoom, ptScroll );
}

inline void ClientToWnd( CSize &size, int iZoom, CPoint ptScroll )
{
	double fZoom = iZoom>0?iZoom:-1./iZoom;

	size.cx = int( (size.cx+0.5)*fZoom ) - ptScroll.x;
	size.cy = int( (size.cy+0.5)*fZoom ) - ptScroll.y;
}

inline void ClientToWnd( CPoint &point, int iZoom, CPoint ptScroll )
{
	double fZoom = iZoom>0?iZoom:-1./iZoom;

	point.x = int( (point.x+0.5)*fZoom ) - ptScroll.x;
	point.y = int( (point.y+0.5)*fZoom ) - ptScroll.y;
}

inline void WndToClient( CRect &rect, int iZoom, CPoint ptScroll )
{
	double fZoom = iZoom>0?iZoom:-1./iZoom;

	rect += ptScroll;

	rect.left = int( (rect.left)/fZoom );
	rect.right = int( (rect.right)/fZoom );
	rect.top = int( (rect.top)/fZoom );
	rect.bottom = int( (rect.bottom)/fZoom );
}

inline void WndToClient( CSize &size, int iZoom, CPoint ptScroll )
{
	double fZoom = iZoom>0?iZoom:-1./iZoom;

	size.cx = int( (size.cx+ptScroll.x)/fZoom );
	size.cy = int( (size.cy+ptScroll.y)/fZoom );
}

inline void WndToClient( CPoint &point, int iZoom, CPoint ptScroll )
{
	double fZoom = iZoom>0?iZoom:-1./iZoom;

	point.x = int( (point.x+ptScroll.x)/fZoom );
	point.y = int( (point.y+ptScroll.y)/fZoom );
}


#endif //__CellInfo_inl__