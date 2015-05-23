/*
 * invert.c	Jim Piper	July 1983
 *
 * Modifications
 *
 *	10 Dec 1987		BDP		Fixed a stray printf with a missing parameter
 *	08 May 1987		BDP		protection against null or empty objs
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

/*
 * invert grey-table of object.
 * protection in this case in fact provided by protection in greyrange
 * and the interval scanning routines themselves, no additional changes
 * necessary here.  bdp 8/5/87
 */
invert(obj)
struct object *obj;
{
	GREY min,max,range;
	register GREY *g;
	register int i;
	struct iwspace iwsp;
	struct gwspace gwsp;

#ifdef WTRACE
	fprintf(stderr,"invert object\n");
#endif WTRACE

	greyrange(obj,&min,&max);
	range = max + min;
#ifdef DEBUG
	fprintf(stderr,"invert: greyrange aggregate is %d\n",range);
#endif DEBUG
	initgreyscan(obj,&iwsp,&gwsp);
	while (nextgreyinterval(&iwsp) == 0) {
		g = gwsp.grintptr;
		for (i=0; i<iwsp.colrmn; i++) {
			*g = range - *g;
			g++;
		}
	}
}
