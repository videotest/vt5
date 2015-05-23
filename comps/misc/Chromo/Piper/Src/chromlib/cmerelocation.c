/*
 * cmerelocation.c	Jim Piper	23/1/84
 *
 * Find centromere from profile associated with a mid-points polygon,
 * with optional mirroring of profile at each end,
 * to pick up acrocentric centromeres.
 * Return as a position in mid-points polygon (-1 if none found).
 *
 * If used, the mirrored profile is free'ed at the end of this routine,
 * and the original profile is retained.
 *
 * The parameter "quantum" quantises the depth to which a local minimum
 * must sink below the adjacent maxima in order to be recognised as such.
 */
#include <stdio.h>
#include <wstruct.h>

#define PEMAX	30		/* maximum number of extrema in a profile */
struct profextremum {
	int pos;	/* position */
	int minmax;	/* 1 if max, -1 if min */
	int val;	/* profile value */
};


cmerelocation(prof,quantum,mirror)
struct object *prof;
{
	register int *hv;
	register i,m,ne,max;
	int curmin, curmax, pt, orig, end;
	struct object *mirrorprof();
	register struct histogramdomain *hdom;
	struct profextremum pe[PEMAX];
	register struct profextremum *PE;

	if (mirror) {
		prof = mirrorprof(prof,&orig,&end);
		hdom = (struct histogramdomain *)prof->idom;
	} else {
		orig = 0;
		hdom = (struct histogramdomain *)prof->idom;
		end = hdom->npoints;
	}
	/*
	 * find max value and use to determine quantisation of profile
	 */
	max = 0;
	hv = hdom->hv;
	for (i=0; i<hdom->npoints; i++) {
		if (*hv > max)
			max = *hv;
		hv++;
	}
	quantum = max/quantum;
	/*
	 * search profile for extrema
	 */
	m = 1;
	ne = 0;
	curmax = 0;
	hv = hdom->hv;
	PE = pe;
	for (i=0; i<hdom->npoints && ne < PEMAX; i++) {
		switch (m) {
		case 1:			/* looking for maximum */
			if (*hv > curmax) {
				curmax = *hv;
				pt = i;
			} else if (*hv < curmax - quantum) {
				PE->pos = pt;
				PE->minmax = m;
				PE->val = curmax;
				m = -m;
				ne++;
				PE++;
				curmin = *hv;
				pt = i;
			}
			break;
		case -1:		/* looking for minimum */
			if (*hv < curmin) {
				curmin = *hv;
				pt = i;
			} else if (*hv > curmin + quantum) {
				PE->pos = pt;
				PE->minmax = m;
				PE->val = curmin;
				m = -m;
				ne++;
				PE++;
				curmax = *hv;
				pt = i;
			}
			break;
		}
		hv++;
	}
	if (mirror)
		freeobj(prof);
	/*
	 * search extremum list for lowest minimum in original profile length,
	 * return -1 otherwise
	 */
	curmin = max;
	PE = pe;
	for (i=0; i<ne; i++) {
		if (PE->minmax < 0)
			if (PE->val < curmin && PE->pos >= orig && PE->pos <= end) {
				curmin = PE->val;
				pt = PE->pos;
			}
		PE++;
	}
	if (curmin == max)
		return(-1);
	else
		return(pt-orig);
}
