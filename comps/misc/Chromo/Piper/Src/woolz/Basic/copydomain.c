/*
 * copydomain.c
 *
 * Modifications
 *
 *	01 May 1987		BDP		protection against null or empty objs
 *	15 Oct 1986		CAS		Linkcount
 *	12 Sep 1986		CAS		Includes
 */
#include <stdio.h>
#include <wstruct.h>


struct intervaldomain *copydomain(idom,spareitv,spc)
struct intervaldomain *idom;
struct interval **spareitv;
int *spc;
{
	struct intervaldomain *jdom, *makedomain();
	register struct interval *itvl, *jtvl, *ktvl;
	struct intervalline *ivln;
	register l,n;

	/* protection added checks firstly that supplied domain exists, if its
	NULL then NULL is returned. If an empty object is supplied then its still
	copied, but returned before any further operations. In both these cases
	NULL is returned to the spare itv pointer.  bdp 1/5/87 */
	
	*spareitv = NULL;	/* initialise in case of error return */
	if (idom == NULL)
		return(NULL);

	jdom = makedomain(1,idom->line1,idom->lastln,idom->kol1,idom->lastkl);
	if ( wzemptyidom(idom) > 0 )
		return(jdom);	/* return copy of the empty domain */
		
	ivln = idom->intvlines;
	n = 0;
	for (l=idom->line1; l<=idom->lastln; l++) {
		n += ivln->nintvs;
		ivln++;
	}
	jtvl = itvl = (struct interval *) Malloc(2*n*sizeof(struct interval));
	jdom->freeptr = (char *) itvl;
	jdom->linkcount = 0;
	*spc = n;
	ivln = idom->intvlines;
	for (l=idom->line1; l<=idom->lastln; l++) {
		ktvl = ivln->intvs;
		for (n=0; n<ivln->nintvs; n++) {
			jtvl->ileft = ktvl->ileft;
			jtvl->iright = ktvl->iright;
			jtvl++;
			ktvl++;
		}
		makeinterval(l,jdom,ivln->nintvs,itvl);
		itvl = jtvl;
		ivln++;
	}
	*spareitv = itvl;
	return(jdom);
}
