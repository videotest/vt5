/*
 * massdiff.c		Jim Piper		12 September 1984
 *
 * Compute 100 * (difference in mass between top half and bottom half of
 * chromosome [by area])/chromosome area.
 *
 * beware, may not be able to use propertylist values of area, mass
 */

#include <stdio.h>
#include <wstruct.h>
#include <chromanal.h>

massdiff(obj)
struct chromosome *obj;
{
	register int k,massdiff,areaby2,areaobj;
	struct iwspace iwsp;
	struct gwspace gwsp;
	register GREY *g;

	massdiff = 0;
	areaobj = obj->plist->area;
	areaby2 = areaobj/2;
	initgreyscan(obj,&iwsp,&gwsp);
	while (nextgreyinterval(&iwsp) == 0) {
		g = gwsp.grintptr;
		for (k=0; k<iwsp.colrmn; k++) {
			if (areaby2 >= 0) {
				areaby2--;
				massdiff += *g++;
			} else
				massdiff -= *g++;
		}
	}
	return (100*massdiff/areaobj);
}
