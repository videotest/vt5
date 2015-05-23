#pragma once

#include <atlstr.h>
#include <math.h>
inline void split( CString strSource, CString strSplitter, CString **Dest, long *lSz )
{
	int nLen = strSource.GetLength();
	int nLen2 = strSplitter.GetLength();

	*lSz = 0;
	*Dest = new CString[nLen];

	int nPrev = 0;
	// [vanek] - 24.08.2004
	int nSplitter = -1;
	while( (nSplitter  = strSource.Find( strSplitter, nPrev )) != -1 )
	{
		(*Dest)[*lSz] = strSource.Mid( nPrev, nSplitter - nPrev );
		nPrev = nSplitter + nLen2;
		(*lSz)++;
		if( nPrev >= nLen )
			break;
	}

	if( nPrev != nLen )
	{
		(*Dest)[*lSz] = strSource.Mid( nPrev, nLen - nPrev );
		(*lSz)++;
	}
}

#define DIGITS_COUNT 10

inline int get_num_count( double fVal, int iCounter = 0 )
{
	double f_abs = fVal > 0 ? 0.5 : -0.5;
	long nVal = long( fVal + f_abs );
	if( iCounter >= DIGITS_COUNT )
		return iCounter;
	double fDelta = fabs( fVal - nVal );
	if( fDelta > 0.00000000001 )
		return get_num_count( fVal * 10.0, iCounter + 1 );
	return iCounter;
}

inline CString getFormatString(double value)
{
	CString str_xp;
	if(value < 1e-9){
		str_xp="%g";
	}else{
		int ncol = get_num_count( value );
		char format[MAX_PATH], _format[MAX_PATH];
		sprintf( _format, "%d", ncol );
		strcpy( format, "%." );
		strcat( format, _format );
		strcat( format, "lf" );
		str_xp = format;
	}
	return str_xp;
}
