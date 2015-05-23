/*
 * skelsup.c	Jim Piper	1983
 *
 * Support routines for skeletonisation, particularly
 * applied to chromosome analysis.
 *
 * Modified May 1986 for lower resolution sampling from skeleton,
 *
 * Modifications
 *
 *	13 Sep 1986		CAS		undefined EXTENDTIP - defined in chromanal
 *							to be different to here -- (why ?)
 *	11 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>
#include <chromanal.h>
#undef EXTENDTIP		/* Defined in chromanal.h to be 7... */


/*
 * Detect branch in skeleton.  A branch implies a 3 by 3 neighbourhood
 * with at least 4 skeleton points.
 * If no branches, mark one tip with value 2
 * and dump its coordinates somewhere.
 */
afindtipandbranch(sk,tipx,tipy)
register struct object *sk;
int *tipx,*tipy;
{
	register i,j,k,count;
	int l;
	GREY *greyval();
	int tipmarked;
	struct iwspace iwsp;
	tipmarked = 0;
	initrasterscan(sk,&iwsp,0);
	while (nextinterval(&iwsp) == 0) {
		l = iwsp.linpos;
		for (k=iwsp.lftpos; k<=iwsp.rgtpos; k++) {
			/*
			 * count neighbours in skeleton
			 */
			count = 0;
			for (i=l-1; i<=l+1; i++)
				for (j=k-1; j<=k+1; j++)
					if (*greyval(sk,i,j) > 0)
						count++;
			if (count>3 || count==1)	/* branch or isolated point */
				return(1);
			if (count==2 )  {		/* tip */
				if (tipmarked == 0) {
					*greyval(sk,l,k) = 2;
					*tipx = k;
					*tipy = l;
				} else if (tipmarked > 1)	/* >2 tips */
					return(1);
				tipmarked++;
			}
		}
	}
	if (tipmarked == 0)
		return(1);
	else
		return(0);
}


/*
 * distance transform.  First point is known and is assumed marked
 * with value 2.  Look at neighbours, mark with increasing values.
 */
adisttrans(sk,k,l)
register struct object *sk;
register k,l;
{
	register i,j;
	int g,nn;
	GREY *greyval();
	register GREY *gv;
	g = 2;
	do {
		nn = 0;
		for (i=l+1; i>=l-1; i--)
			for (j=k-1; j<=k+1; j++) {
				if (i==l && j==k)
					j++;
				gv = greyval(sk,i,j);
				if (*gv == 1) {
					*gv = ++g;
					l = i;
					k = j;
					nn++;
					goto LOOPEND;
				}
			}
LOOPEND:
		;
	} while (nn > 0);
}


#define SAMPLE	3
#define EXTENDTIP 9
/*
 * make a polygon by sampling a skeleton,
 * and extend its tips a little.
 */
struct object *apolyfromskel(skobj,tipx,tipy)
struct object *skobj;
{
	struct object *makemain();
	struct polygondomain *poly, *makepolydmn();
	struct iwspace iwsp;
	struct gwspace gwsp;
	register struct ivertex *vtx,*wtx;
	struct ivertex *xtx;
	register int gv,nvtx,k,nbackup;
	register GREY *g;
	int mult,areao,areas;
	
	/*
	 * distance transform the skeleton
	 */
	adisttrans(skobj,tipx,tipy);

	/*
	 * Convert distance-transformed skeleton to a polygon.
	 * Leave space for extending the tips
	 */
	areao = area(skobj);
	poly = makepolydmn(1,NULL,0,3+(areao-1)/SAMPLE,1);
	nvtx = poly->nvertices = poly->maxvertices;
	vtx = poly->vtx + 1;
	areas = (areao-1)%SAMPLE;
	areao /= 2;
	initgreyscan(skobj,&iwsp,&gwsp);
	while (nextgreyinterval(&iwsp) == 0) {
		g = gwsp.grintptr;
		for (k=iwsp.lftpos; k<=iwsp.rgtpos; k++) {
			/* remember that the distance transform gives values >= 2 */
			gv = *g - 2;
			/*
			 * take care that both first and last skeleton points
			 * are represented in final polygon.  This involves
			 * fiddling around with remainders when distance transform
			 * value is divided by sampling interval.
			 */
			if ((gv <= areao && gv%SAMPLE == 0) ||
				(gv > areao && gv%SAMPLE == areas)) {
				gv /= SAMPLE;
				vtx[gv].vtX = k;
				vtx[gv].vtY = iwsp.linpos;
			}
			g++;
		}
	}

	/*
	 * set end points by fitting to line between penultimate point and point
	 * EXTENDTIP/SAMPLE along (unless too short).
	 * Then produce polygon by about EXTENDTIP points.
	 */
	nbackup=EXTENDTIP/SAMPLE;
	if (nvtx < nbackup + 3)
		nbackup=nvtx-3;
	if (nbackup > 0) {
		mult = (2*EXTENDTIP) / (SAMPLE*nbackup);
		vtx = poly->vtx;
		wtx = vtx+1;
		xtx = wtx+nbackup;
		vtx->vtX = wtx->vtX + mult * (wtx->vtX - xtx->vtX);
		vtx->vtY = wtx->vtY + mult * (wtx->vtY - xtx->vtY);
		vtx += (nvtx-1);
		wtx = vtx-1;
		xtx = wtx-nbackup;
		vtx->vtX = wtx->vtX + mult * (wtx->vtX - xtx->vtX);
		vtx->vtY = wtx->vtY + mult * (wtx->vtY - xtx->vtY);
	}

	/* make polygon object and return */
	return(makemain(10,poly,NULL,NULL,skobj));
}
