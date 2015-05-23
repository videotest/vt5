/*
 * mirrorprof.c	Jim Piper	23/1/84
 *
 * Make a histogram by mirroring the input histogram about points
 * near the end of the input histogram.  The output parameters
 * "*o" and "*e" provide pointers into the output histogram which
 * indicate the relative end-point positions of the original histogram.
 *
 * Modifications
 *
 *	11 Sep 1986		CAS		Includes
 */
#include <stdio.h>
#include <wstruct.h>

struct object *mirrorprof(p,o,e)
struct object *p;
register int *o;
int *e;
{
	struct object *np, *makemain();
	struct histogramdomain *hdom, *ndom, *makehistodmn();
	register int i,ovlap,n,*hv,*nv;

	hdom = (struct histogramdomain *) p->idom;
	ovlap = hdom->npoints >> 2;
	if (ovlap > 8)
		ovlap = 8;
	*o = hdom->npoints - ovlap;
	*e = *o + hdom->npoints - 1;
	ndom = makehistodmn(1,3*hdom->npoints - 2*ovlap);
	ndom->l = hdom->l - *o;
	ndom->k = hdom->k;
	ndom->r = hdom->r;
	np = makemain(13,ndom,0,0,p);
	n = hdom->npoints;
	hv = hdom->hv;
	nv = ndom->hv + *o;
	for (i=0; i<n; i++)
		*nv++ = *hv++;
	hv = hdom->hv;
	nv = ndom->hv + *o + ovlap;
	for (i=0; i<n; i++)
		*nv-- += *hv++;
	hv = hdom->hv;
	nv = ndom->hv + ndom->npoints - 1;
	for (i=0; i<n; i++)
		*nv-- += *hv++;
	/*
	 * increment the profile outside original bounds
	 * to ensure that minima near ends are within original bounds
	 */
	ovlap >>= 1;
	n = *o+ovlap;
	nv = ndom->hv;
	for (i=0; i<n; i++)
		(*(nv++))++;
	n = ndom->npoints;
	nv = ndom->hv + *e - ovlap;
	for (i= *e - ovlap; i<n; i++)
		(*(nv++))++;

	return(np);
}
