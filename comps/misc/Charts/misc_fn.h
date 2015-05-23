#pragma once

#define DIGITS_COUNT 10.0
inline int _get_num_count( double fVal, int iCounter = 0 )
{
	double f_abs = fVal > 0 ? 0.5 : -0.5;
	long nVal = long( fVal + f_abs );
	if( iCounter >= DIGITS_COUNT )
		return iCounter;
	double fDelta = fabs( fVal - nVal );
	if( fDelta > 0.00000000001 )
		return _get_num_count( fVal * 10.0, iCounter + 1 );
	return iCounter;
}
