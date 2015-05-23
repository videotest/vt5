/*
 * skelsupport.c	Jim Piper	1983
 *
 * Support routines for skeletonisation, particularly
 * applied to chromosome analysis.
 *
 *
 * Modifications
 *
 *	11 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>
#include <chromanal.h>


/*
 * Detect branch in skeleton.  A branch implies a 3 by 3 neighbourhood
 * with at least 4 skeleton points.
 * If no branches, mark one tip with value 2
 * and dump its coordinates somewhere.
 */
int tipx = 0;
int tipy = 0;
findtipandbranch(sk)
register struct object *sk;
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
					tipx = k;
					tipy = l;
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
disttrans(sk,k,l)
register struct object *sk;
register k,l;
{
	register i,j,nn;
	GREY g, *greyval();
	g = 2;
	do {
		nn = 0;
		for (i=l+1; i>=l-1; i--)
			for (j=k-1; j<=k+1; j++) {
				if (i==l && j==k)
					j++;
				if (*greyval(sk,i,j) == 1) {
					*greyval(sk,i,j) = ++g;
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


/*
 * make a polygon from a skeleton,
 * and extend its tips a little.
 */
struct object *polyfromskel(skobj)
struct object *skobj;
{
	struct object *makemain();
	struct polygondomain *poly, *makepolydmn();
	struct iwspace iwsp;
	struct gwspace gwsp;
	register struct ivertex *vtx;
	register int nvtx,raster,k,nbackup;
	register GREY *g;
	
	disttrans(skobj,tipx,tipy);

	/* convert distance-transformed skeleton to a polygon. */
	/* leave space for extending the tips */
	poly = makepolydmn(1,NULL,0,2+area(skobj),1);
	nvtx = poly->nvertices = poly->maxvertices;
	/* remember that the distance transform gives values >= 2 */
	vtx = poly->vtx - 1;
	initgreyscan(skobj,&iwsp,&gwsp);
	while (nextgreyinterval(&iwsp) == 0) {
		g = gwsp.grintptr;
		for (k=iwsp.lftpos; k<=iwsp.rgtpos; k++) {
			vtx[*g].vtX = k;
			vtx[*g].vtY = iwsp.linpos;
			g++;
		}
	}

	/* alter position of atypical, isolated points which lie in pattern, e.g.
	           x
	    	   x                         y                     y xxxx
	           ny         or         xxxxnxxxx      or       xxnx
	           x
	           x
	 this may not be order independent, of course, see third pattern.
	 in above, n is new position of y, x is unchanged */
	vtx = poly->vtx;
	for (k=1; k<nvtx-3; k++) {
		/* look at x coordinates
		/* if x coordinates of next but adjoining points the same, force
		/* x coordinate of middle point to be same
		/* (cheaper than further checks) */
		if (vtx[k].vtX == vtx[k+2].vtX)
			vtx[k+1].vtX = vtx[k].vtX;
		/* alternatively, look at y coordinates */
		else if (vtx[k].vtY == vtx[k+2].vtY)
			vtx[k+1].vtY = vtx[k].vtY;
	}

	/* set end points by fitting to line between penultimate point and point
	/* 5 along (unless too short). then produce polygon by about 5 points.*/
	nbackup=EXTENDTIP;
	if (nvtx < EXTENDTIP + 3)
		nbackup=nvtx-3;
	vtx->vtX = 2*vtx[1].vtX - vtx[1+nbackup].vtX;
	vtx->vtY = 2*vtx[1].vtY - vtx[1+nbackup].vtY;
	vtx[nvtx-1].vtX = 2*vtx[nvtx-2].vtX - vtx[nvtx-nbackup-2].vtX;
	vtx[nvtx-1].vtY = 2*vtx[nvtx-2].vtY - vtx[nvtx-nbackup-2].vtY;

	/* make polygon object and return */
	return(makemain(10,poly,NULL,NULL,skobj));
}
