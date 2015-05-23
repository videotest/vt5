/*
 * multiprofiles.c		Jim Piper		20/1/84
 *
 * Modifications
 *
 *	 7 Feb 1991		CAS		voids
 *	17 Nov 1987		SCG		Safeguard against finding
 *							zero width on long thin objects
 *							resulting in zero profile and
 *							prevent use of poly with no of vertices zero
 *	11 Sep 1986		CAS		Includes
 */
#include <stdio.h>
#include <wstruct.h>
#include <chromanal.h>

/*
 * make multiple profiles as specified in array "moments", which should be
 * monotonic increasing, minimum value >= -1.
 * This saves considerable time in the extraction of slices perpendicular
 * to medial axis.  The maximum number of simultaneous profiles is MAXSPROF.
 *
 * "moment" selects profile type :
 *	-2	max density
 *	-1	mean density (width defined as sum of non-zero points)
 *	 0	integrated density
 *	 1	first moment (sigma |md| / sigma m) (cf. centroid)
 *	 2	second moment (sigma mdd / sigma m) (radius of gyration)
 *
 * The mid-points polygon is assumed to be integer, scaled up by a factor
 * of 8, to avoid floating point wherever possible.  Similarly, sintheta
 * and costheta are represented by numbers in [-256,256].
 */
#define MAXSPROF	5
void multiprofiles(obj, mpol, prof, moments, nmoments)
struct chromosome *obj;
struct object *mpol, **prof;
int *moments;
{
	register int d,g,i,sum,maxg;
	int j, k, mlines, sline, sumd[MAXSPROF], sump, sumq, mom;
	int halfwidth, *hv[MAXSPROF];
	GREY rbuf[2*MAXHALFWIDTH+1];
	int x,y,sintheta,costheta;
	struct object *makemain();
	struct histogramdomain *hdom;
	struct polygondomain *p;

	p = (struct polygondomain *) mpol->idom;
	if (mpol->type != 10 || p->type != 1) {
		fprintf(stderr,"not a type 1 polygon\n");
		exit(91);
	}

	/*
	 * approximate 1.5 mean width of object
	 * to provide a limiting distance from axis for profile extraction
	 * Safeguard against a duff polygon
	 */
	if((mlines = p->nvertices) != 0) {
	
		halfwidth = 2*obj->plist->area;
		halfwidth /= (3*mlines);
		if (halfwidth > MAXHALFWIDTH)
			halfwidth = MAXHALFWIDTH;
		/*
	 	 * a long narrow object could obtain halfwidth of zero.  This will
	 	 * subsequently give a zero profile of this object.
	 	 */
		if (halfwidth == 0)
			halfwidth = 1;
	}
	for (i=0; i<nmoments; i++) {
		hdom = (struct histogramdomain *) Calloc(sizeof(struct histogramdomain) + mlines , sizeof(int));
		hdom->type = 1;
		hdom->r = 0;	/* display with vertical base */
		hdom->l = obj->idom->line1;
		hdom->k = obj->idom->kol1;
		hdom->npoints= mlines;
		hv[i] = hdom->hv = (int *) (hdom + 1);
		prof[i] = makemain(13,hdom,NULL,NULL,NULL);
	}
	for (sline=1; sline<=mlines; sline++) {
		/*
	 	* get next column, line (x,y) in unit-spaced steps
	 	* along smoothed mid-points polygon, and local orientation.
	 	* sline is line number in straightened object
	 	*/
		setnextpoint(p,&x,&y,&sintheta,&costheta,sline);
		/*
		 * rotate into buffer.
		 */
		profslice(obj,x,y,sintheta,costheta,halfwidth,rbuf);
		/*
		 * compute density profile value
		 */
		maxg = 0;
		sum = 0;
		sump = 0;
		for (i=0; i<nmoments; i++)
			sumd[i] = 0;
		for (k=0; k<=halfwidth*2; k++) {
			g = rbuf[k];
			if (g > maxg)
				maxg = g;
			sum += g;
			if (g > 0)
				sump++;
			mom = g;
			d = halfwidth - k;
			if (d < 0)
				d = -d;
			i = 0;
			for (j=0; j<nmoments; j++) {
				for (; i<moments[j]; i++)
					mom *= d;
				sumd[j] += mom;
			}
		}
		sumq = sum;
		if (moments[0] == -1 && sump > 0)
			sum /= sump;
		for (i=0; i<nmoments; i++) {
			*hv[i] = (moments[i] == -2? maxg: (moments[i] <= 0? sum: (sumq==0? 0: 10*sumd[i]/sumq)));
			hv[i]++;
		}
	}
	return;
}
