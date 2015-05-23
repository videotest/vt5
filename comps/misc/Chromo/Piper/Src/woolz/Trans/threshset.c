/*
 * threshset.c
 *
 * set label topology and threshold under/over
 *
 * Modifications
 *
 *	 7 Feb 1991		CAS		voids
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

SMALL jdqt = 0, highlow = 1;
/*
 * set connection topology - 
 * diagonal if jdqt = 0,
 * square if jdqt = 1.
 */
void topset(jjdqt)
{ 
	jdqt = jjdqt;
	return;
} 

/*
 * set mode for selection of pixels - 
 * >= threshold if hi != 0,
 * < threshold if hi == 0;
 */
void hilow(hi)
{ 
	highlow = hi;
	return;
} 
