/*
 * edgegv.c	Jim Piper	July 1983
 *
 * Modifications
 *
 *	18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	06 May 1987		BDP		protection against null or empty objs
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

/*
 * compute mean grey value of edge points.
 * protection against null objects added, this will just cause zero to 
 * be returned. For an empty object the same result will occur, previously
 * an empty object would give a divide by zero error. An extra variable
 * has been used allowing easier handling of the empty case an giving
 * just one point of return
 */
edgegv (obj)
struct object *obj;
{
	struct iwspace iwsp;
	struct gwspace gwsp;
	register count,gtot,gv;

	gv = 0;
	if (woolz_check_obj(obj, "edgegv") == 0)
		if ( ( wzemptyidom(obj->idom) == 0 ) && (obj->type == 1) )
		{
			initgreyscan(obj,&iwsp,&gwsp);
			gtot = 0;
			count = 0;
			while (nextgreyinterval(&iwsp) == 0) {
				count++;
				gtot += *gwsp.grintptr;
				if (iwsp.colrmn > 1) {
					count++;
					gtot += *(gwsp.grintptr + iwsp.colrmn -1);
				}
			}
			if (count > 0)
				gv = gtot/count;
		}
	return(gv);
}
