
/*
 * shorten.c		Jim Piper		20-05-86
 *
 * Modifications
 *
 *	11 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>
#include <chromanal.h>


/*
 * using density profile, shorten tips of mid points polygon,
 * density profile and moment profile
 * until within chromosome body (density at least 1% of peak).
 */
shorten(mpol,prof0,prof2)
struct object *mpol, *prof0, *prof2;
{
	register int i,l,*g,maxh;
	register struct histogramdomain *hdom0, *hdom2;
	register struct polygondomain *p;

	hdom0 = (struct histogramdomain *)prof0->idom;
	hdom2 = (struct histogramdomain *)prof2->idom;
	p = (struct polygondomain *)mpol->idom;
	l = hdom0->npoints;
	g = hdom0->hv;
	maxh = 0;
	for (i=0; i<l; i++) {
		if (*g > maxh)
			maxh = *g;
		g++;
	}
	maxh /= 100;
	while (*--g < maxh) {
		hdom0->npoints--;
		hdom2->npoints--;
		p->nvertices--;
	}
	while (*hdom0->hv < maxh) {
		hdom0->hv++;
		hdom0->npoints--;
		hdom2->hv++;
		hdom2->npoints--;
		p->vtx++;
		p->nvertices--;
	}
}
