/*
 * setval.c	Jim Piper	July 1983
 *
 * Modifications
 *
 *	06 May 1987		BDP		protection against null or empty objs
 *	12 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

/*
 * set all pixels in an object to same value "val"
 */
setval (obj, val)
struct object *obj;
{
	struct iwspace i1;
	struct gwspace g1;
	register int i;

	/* protection here provided by the scanning routines which should cope
	with null and empty objects. no other changes. bdp 6/5/87.  */
	
	initgreyscan(obj,&i1,&g1);
	while (nextgreyinterval(&i1) == 0)
		for (i=0; i<i1.colrmn; i++)
			*g1.grintptr++ = val;
}
