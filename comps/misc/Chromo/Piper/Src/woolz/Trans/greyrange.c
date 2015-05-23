/*
 * greyrange.c	Jim Piper	July 1983
 *
 * Modifications
 *
 *	27 Mar 1987		jimp	Registers etc.
 *	07 May 1987		BDP		protection against null or empty objs
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

/*
 * compute greyrange of pixel object
 * No extra protection required , in this module beyond the protection
 * in scanning routines , which will cope with null or empty objects
 * bdp 7/5/87 
 */
greyrange(obj,min,max)
struct object *obj;
GREY *min, *max;
{
	register GREY *g;
	register int i, v, lmin, lmax;
	struct iwspace iwsp;
	struct gwspace gwsp;

	lmin = 1;
	lmax = 0;
	initgreyscan(obj,&iwsp,&gwsp);
	while (nextgreyinterval(&iwsp) == 0) {
		g = gwsp.grintptr;
		for (i=0; i<iwsp.colrmn; i++) {
			v = *g;
			g++;
			if (lmin > lmax){
				lmin = lmax = v;
			}
			else if (v > lmax)
				lmax = v;
			else if (v < lmin)
				lmin = v;
		}
	}
	*min = lmin;
	*max = lmax;
}
