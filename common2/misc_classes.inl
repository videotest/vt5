//misc_classes.inl
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))


/////////////////////////////////////////////////////////////////////////////////////////
//_point
inline _point::_point()
{	}

inline _point::_point( long nx, long ny )
{	x = nx;	y = ny;	}

inline _point &_point::operator =( const POINT &point )
{	x = point.x; y = point.y; return *this;	}

inline void _point::from_lparam( LPARAM lparam )
{	x = GET_X_LPARAM(lparam);	y = GET_Y_LPARAM(lparam);	}

inline _point _point::delta( const _point &pt )
{	return _point( x-pt.x, y-pt.y );	}

inline _point _point::offset( const _point &pt )
{	return _point( x+pt.x, y+pt.y );	}

inline bool	operator ==( const _point &point1, const POINT &point2 )
{	return point1.x == point2.x && point1.y == point2.y;	}

inline _point &operator +=( _point &point1, const POINT &point2 )
{	point1.x += point2.x;point1.y += point2.y;return point1;	}

inline _point &operator -=( _point &point1, const POINT &point2 )
{	point1.x -= point2.x;point1.y -= point2.y;return point1;	}

inline _point operator +( const _point &point1, const POINT &point2 )
{	return _point( point1.x+point2.x, point1.y+point2.y );	}

inline _point operator -( const _point &point1, const POINT &point2 )
{	return _point( point1.x-point2.x, point1.y-point2.y );	}



/////////////////////////////////////////////////////////////////////////////////////////
//_size
inline _size::_size()
{	}

inline _size::_size( int cxx, int cyy )
{	cx = cxx; cy = cyy;	}


/////////////////////////////////////////////////////////////////////////////////////////
//_rect
inline _rect::_rect()
{	left = top = bottom = right = 0;	}

inline _rect::_rect( long nleft, long ntop, long nright, long nbottom )
{	left = nleft; top = ntop; right = nright; bottom = nbottom;	}

inline _rect::_rect( const _point &point, const _size &size )
{	left = point.x; top = point.y; right = point.x+size.cx; bottom = point.y+size.cy;	}

inline _point	_rect::top_left()
{	return _point( left, top );	}

inline _point	_rect::bottom_right()
{	return _point( right, bottom );	}

inline _point	_rect::center()
{	return _point( (left+right)>>1, (top+bottom)>>1 );	}

inline void _rect::set_offset( const _point &point )
{	right = point.x+width();	bottom = point.y+height();	left = point.x;	top = point.y;	}

inline void _rect::set_offset( int x, int y )
{	right = x+width();	bottom = y+height();	left = x;	top = y;	}

inline void _rect::delta( int x, int y )
{
	long nWidth  = width();
	long nHeight = height();
	left += x;	top += y;	right = left + nWidth; bottom = top + nHeight;
}

inline long	_rect::width()
{	return right-left;	}

inline long	_rect::height()
{	return bottom-top;	}

inline _size _rect::size()
{	return _size( right-left, bottom-top );	}

inline void	_rect::set( int x, int y, int x1, int y1 )
{	left = x; top = y; right = x1; bottom = y1;}

inline void	_rect::merge( const _rect &rect )
{	left = __min( rect.left, left ); top = __min( rect.top, top ); 
	right = __max( rect.right, right ); bottom = __max( rect.bottom, bottom );	}

inline void	_rect::inflate( long n )
{	left -= n, right += n; top -= n; bottom += n;	}

inline void	_rect::inflate( long dx, long dy )
{	left -= dx, right += dx; top -= dy; bottom += dy;	}

inline void	_rect::inflate( long dx, long dy, long dx1, long dy1 )
{	left -= dx1, right += dx1; top -= dy1; bottom += dy1;	}

inline bool	operator ==( const _rect &rc1, const RECT &rc2 )
{	return rc1.left == rc2.left && rc1.top == rc2.top && rc1.right == rc2.right && rc1.bottom == rc2.bottom; }
