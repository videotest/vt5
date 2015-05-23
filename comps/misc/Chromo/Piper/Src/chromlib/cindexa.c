/*
 * cindexa.c	Jim Piper	23/1/84
 */
#include <stdio.h>
#include <wstruct.h>

/*
 * return the area centromeric index (percentage) defined as percentage
 * of object area below given centromere line position.  Object is assumed
 * to be rotated to be vertical.
 */
cindexa(obj, l)
struct object * obj;
{
	register int s1, s2, s3;
	struct iwspace iwsp;
	initrasterscan(obj,&iwsp,0);

	s1 = s2 = s3 = 0;
	while (nextinterval(&iwsp) == 0)
		if (iwsp.linpos < l)
			s1 += iwsp.colrmn;
		else if (iwsp.linpos == l)
			s2 += iwsp.colrmn;
		else
			s3 += iwsp.colrmn;
	return ((100*s3 + 50*s2)/(s1+s2+s3));
}
