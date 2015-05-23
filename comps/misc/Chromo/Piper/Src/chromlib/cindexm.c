/*
 * cindexm.c	Jim Piper	23/1/84
 */
#include <stdio.h>
#include <wstruct.h>

/*
 * return the mass centromeric index (percentage) defined as percentage
 * of object mass below given centromere line position.  Object is assumed
 * to be rotated to be vertical.
 */
cindexm(obj, l)
struct object * obj;
{
	register int k;
	register GREY *g;
	register long s1, s2, s3;
	struct iwspace iwsp;
	struct gwspace gwsp;
	initgreyscan(obj,&iwsp,&gwsp);

	s1 = s2 = s3 = 0;
	while (nextgreyinterval(&iwsp) == 0) {
		g = gwsp.grintptr;
		for (k=iwsp.lftpos; k<= iwsp.rgtpos; k++) {
			if (iwsp.linpos < l)
				s1 += *g++;
			else if (iwsp.linpos == l)
				s2 += *g++;
			else
				s3 += *g++;
		}
	}
	return ((100*s3 + 50*s2)/(s1+s2+s3));
}
