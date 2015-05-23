/*
 * mass.c	Jim Piper	July 1983
 *
 * Modifications
 *
 *	08 May 1987		BDP		protection against null or empty objs
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

/*
 * return the mass (sum of grey-values) of an object
 * Protection against null or empty objects provided here by scanning routines
 * no extra changes necessary . bdp 8/5/87
 */
mass(obj)
struct object * obj;
{
	register int k, mass;
	register GREY *g;
	struct iwspace iwsp;
	struct gwspace gwsp;
	initgreyscan(obj,&iwsp,&gwsp);

	mass = 0;
	while (nextgreyinterval(&iwsp) == 0) {
		g = gwsp.grintptr;
		for (k=iwsp.lftpos; k<=iwsp.rgtpos; k++)
			mass += *g++;
	}
	return (mass);
}
