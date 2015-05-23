/*
 * area.c	Jim Piper	July 1983
 *
 * Modifications
 *
 *	01 May 1987		BDP		protection against null or empty objs
 *	12 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

/*
 * return the area of an object
 */
area(obj)
struct object * obj;
{
	register int size;
	struct iwspace iwsp;
	initrasterscan(obj,&iwsp,0);

	/*	init raster scan has been modified (1.2 intdomscan) such that if
	it is supplied with a NULL or faulty object, the first call to 
	nextinterval will return zero. In the case of area, this means a result 
	of zero is returned.  It should be noted that whilst this module has
	been protected, a detected error is not being reported to the calling
	function	*/
	
	size = 0;
	while (nextinterval(&iwsp) == 0)
		size += iwsp.colrmn;	/* colrmn = rtpos - lftpos + 1 */
	return (size);
}
