/*
 * axisslope.c		Jim Piper		20/1/84
 *
 *	modifications
 *
 *	11 Jan 1991		Cas		Fix possible divide by zero
 */
#include <stdio.h>
#include <wstruct.h>

/*
 * compute estimate of slope of polygon vertices between lf and lr
 * using a gradient-finding convolution independently in X and Y.
 * Express as 256*sin, 256*cos of slope.
 */
axisslope(p,lf,lr,sinsl,cossl)
struct polygondomain *p;
int *sinsl,*cossl;
{
	register struct ivertex *fv, *gv;
	register int i,s,c,sscc;
	s = 0;
	c = 0;
	fv = p->vtx + lf - 1;
	gv = p->vtx + lr - 1;
	lf = (lr-lf+1)>>1;
	for (i=0; i<lf; i++) {
		c += gv->vtY - fv->vtY;
		s += fv->vtX - gv->vtX;
		gv--;
		fv++;
	}
	sscc = isqrt8(s*s + c*c);
	if (sscc == 0) {
		if (s > c) {
			*sinsl = 256;
			*cossl = 0;
		} else {
			*sinsl = 0;
			*cossl = 256;
		}
	} else {
		*sinsl = (s<<11)/sscc;
		*cossl = (c<<11)/sscc;
	}
}
