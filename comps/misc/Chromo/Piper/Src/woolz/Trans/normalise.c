/*
 * normalise.c	Jim Piper	July 1983
 *
 * Modifications
 *
 *	08 May 1987		BDP		protection against null or empty objs
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

/*
 * normalise grey-table of object to lie in range 0-255
 * Protection against null and empty objects is given to this module by
 * protection in greyrange and the scanning routines. If any problem exists
 * in the supplied object then the while loop will fail at its first attempt.
 * no other changes have been added therefore.  bdp 8/5/87
 */
normalise(obj)
struct object *obj;
{
	GREY min,max,range;
	register GREY *g;
	register int i;
	struct iwspace iwsp;
	struct gwspace gwsp;

	greyrange(obj,&min,&max);
	range = max - min + 1;
	initgreyscan(obj,&iwsp,&gwsp);
	while (nextgreyinterval(&iwsp) == 0) {
		g = gwsp.grintptr;
/*
		for (i=0; i<iwsp.colrmn; i++) {
			*g = 256 * (*g - min) / range;
			g++;
		}
*/
		gstretch(g,iwsp.colrmn,256,min,range);
	}
}
