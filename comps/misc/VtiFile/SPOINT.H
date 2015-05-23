#ifndef __SPoint_h__
#define __SPoint_h__

class CshortPoint
{
public:
	__int16	x, y;
public:
	CshortPoint();
	CshortPoint( int, int );
	CshortPoint( __int16, __int16 );
	CshortPoint( const CshortPoint & );
	CshortPoint( const CPoint & );
	CshortPoint( const POINT & );
	CshortPoint( const POINTS & );
public:	//internal support
	CshortPoint operator += ( const CshortPoint &spt );
	CshortPoint operator -= ( const CshortPoint &spt );
	CshortPoint operator  + ( const CshortPoint &spt );
	CshortPoint operator  - ( const CshortPoint &spt );
	operator CPoint();
	BOOL operator ==( const CshortPoint & );
	BOOL operator ==( const CPoint & );
	BOOL operator !=( const CshortPoint & );
	BOOL operator !=( const CPoint & );
	
};

inline CshortPoint::CshortPoint()
{
	x = y = 0;
}

inline CshortPoint::CshortPoint( int xx, int yy )
{
	x = (__int16)xx;
	y = (__int16)yy;
}


inline CshortPoint::CshortPoint( const POINT &pt )
{
	x = (__int16)pt.x;
	y = (__int16)pt.y;
}

inline CshortPoint::CshortPoint( const POINTS &pts )
{
	x = pts.x;
	y = pts.y;
}

inline CshortPoint::CshortPoint( __int16 xx, __int16 yy )
{
	x = xx;
	y = yy;
}

inline CshortPoint::CshortPoint( const CshortPoint &spt )
{
	x = spt.x;
	y = spt.y;
}

inline CshortPoint::CshortPoint( const CPoint &pt )
{
	x = (__int16)pt.x;
	y = (__int16)pt.y;
}

inline CshortPoint CshortPoint::operator+=( const CshortPoint &spt )
{
	x+=spt.x;
	y+=spt.y;
	return *this;
}

inline CshortPoint CshortPoint::operator-=( const CshortPoint &spt )
{
	x-=spt.x;
	y-=spt.y;
	return *this;
}

inline CshortPoint::operator CPoint()
{
	return CPoint( x, y );
}

inline BOOL CshortPoint::operator ==( const CshortPoint &spt )
{
	return spt.x == x && spt.y == y;
}

inline BOOL CshortPoint::operator ==( const CPoint &pt )
{
	return pt.x == (int)x && pt.y == (int)y;
}

inline BOOL CshortPoint::operator !=( const CshortPoint &spt )
{
	return spt.x != x || spt.y != y;
}

inline BOOL CshortPoint::operator !=( const CPoint &pt )
{
	return pt.x != (int)x || pt.y != (int)y;
}

inline CshortPoint CshortPoint::operator +( const CshortPoint &spt )
{
	return CshortPoint( x+spt.x, y+spt.y );
}

inline CshortPoint CshortPoint::operator -( const CshortPoint &spt )
{
	return CshortPoint( x-spt.x, y-spt.y );
}

#endif //__SPoint_h__

