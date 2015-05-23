#ifndef __misc_classes_h__
#define __misc_classes_h__

class _point : public POINT
{
public:
	_point();
	_point( long nx, long ny );
	_point(const POINT point)
	{
		x = point.x;
		y = point.y;
	}
	_point( long l )	{y = l>>16; x = l&0xFFFF;}
	_point( const POINTS &p) {x = p.x; y = p.y;}

	inline _point &operator =(const POINT &point );

	void from_lparam( LPARAM lparam );
	_point delta( const _point &pt );
	_point offset( const _point &pt );
};

class _size : public SIZE
{
public:
	_size();
	_size( int cx, int cy );
};

class _rect : public RECT
{
public:
	_rect();
	_rect( long nleft, long ntop, long nright, long nbottom );
	_rect( const _point &point, const _size &size );

	void	set_offset( const _point &point );
	void	set_offset( int x, int y );
	void	set( int x, int y, int x1, int y1 );
	void	merge( const _rect &rect );
	void	delta( int x, int y );
	void	inflate( long n );
	void	inflate( long dx, long dy );
	void	inflate( long dx, long dy, long dx1, long dy1 );

	long	width();
	long	height();
	_size	size();
	_point	top_left();
	_point	bottom_right();
	_point	center();

	_rect& operator =(const _rect& rc)
	{left = rc.left; top = rc.top; right = rc.right; bottom = rc.bottom; return *this;}
};


#ifndef __AFX_H__
#define NOPOINT	_point( 0, 0 )
#define NORECT	_rect( 0, 0, 0, 0 )
#define RECT_SIZE_ARG( rect )	rect.left, rect.top, rect.width(), rect.height()
#define RECT_COORDS_ARG( rect )	rect.left, rect.top, rect.right, rect.bottom
#endif //__AFX_H__

#include "misc_classes.inl"

#endif //__misc_classes_h__