#if !defined(AFX_DPOINT_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
#define AFX_DPOINT_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "misc_utils.h"

typedef struct tagDPOINT
{
    double  x;
    double  y;
} DPOINT, *PDPOINT, NEAR *NPDPOINT, FAR *LPDPOINT;

class _dpoint : public DPOINT
{
public:
	_dpoint();
	_dpoint(double nx, double ny);
	_dpoint(const DPOINT dpoint)
	{
		x = dpoint.x;
		y = dpoint.y;
	}
	_dpoint(const POINT point)
	{
		x = point.x;
		y = point.y;
	}

	inline _dpoint &operator =(const DPOINT &dpoint );

	void from_lparam( LPARAM lparam );
	_dpoint delta( const _dpoint &pt );
	_dpoint offset( const _dpoint &pt );
};

///////////////////////////////////
inline _dpoint::_dpoint()
{	}

inline _dpoint::_dpoint(double nx, double ny)
{	x = nx;	y = ny;	}

inline _dpoint &_dpoint::operator =( const DPOINT &dpoint )
{	x = dpoint.x; y = dpoint.y; return *this;	}

inline void _dpoint::from_lparam( LPARAM lparam )
{	x = GET_X_LPARAM(lparam);	y = GET_Y_LPARAM(lparam);	}

inline bool	operator ==( const _dpoint &dpoint1, const DPOINT &dpoint2 )
{	return dpoint1.x == dpoint2.x && dpoint1.y == dpoint2.y;	}

inline _dpoint &operator +=( _dpoint &dpoint1, const DPOINT &dpoint2 )
{	dpoint1.x += dpoint2.x;dpoint1.y += dpoint2.y;return dpoint1;	}

inline _dpoint &operator -=( _dpoint &dpoint1, const DPOINT &dpoint2 )
{	dpoint1.x -= dpoint2.x;dpoint1.y -= dpoint2.y;return dpoint1;	}

inline _dpoint operator +( const _dpoint &dpoint1, const DPOINT &dpoint2 )
{	return _dpoint( dpoint1.x+dpoint2.x, dpoint1.y+dpoint2.y );	}

inline _dpoint operator -( const _dpoint &dpoint1, const DPOINT &dpoint2 )
{	return _dpoint( dpoint1.x-dpoint2.x, dpoint1.y-dpoint2.y );	}



#endif // !defined(AFX_DPOINT_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
