/*
 * pmsaxis.c		Jim Piper @ AUC			11 May 1986
 *
 * Make an axis using a modification of the poor man's skeleton
 * procedure.  I.e. assume chromosome vertical, and find mid-points
 * of horizontal slices through chromosome, with the following
 * modifications:
 *	(1) Sample at vertical intervals separated by "step"
 *	(2) Start sampling at distance "tip" from top and bottom.
 *	(3) Force first "tipstraight" samples to lie on straight
 *		line, by skipping the intermediate sample points and
 *		then filling them in by linear interpolation.
 *	(4) make the tip values by linear interpolation from the
 *		straight tip section.
 * The resulting axis can be smoothed as usual by "axissmooth()".
 *
 * Modifications
 *
 *	 7 Dec 1988		CAS		Only copy back if there is a gap..
 *	05 Dec 1988		SCG/CAS	Dont interpolate intermediate undefined points; better
 *							to copy back and reduce pdom->nvertices
 *	02 Dec 1988		SCG/dcb	Rewritten the code in the main loop to cope
 *							with joined objects
 *	11 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

struct object *pmsaxis(obj,tip,step,tipstraight)
struct object *obj;
{
	struct polygondomain *pdom, *makepolydmn();
	register struct ivertex *vtx,*wtx,*xtx;
	struct intervaldomain *idom;
	struct interval *itvl;
	struct object *ax, *makemain();
	register int i, l, l1, l2, n;
	int nl, nl2, ts, dx, dy, count, gap;

	idom = obj->idom;
	nl = 3 + (idom->lastln - idom->line1 - 2*tip + step/2)/step;
	if (nl <= 3)
		nl = 4;
	/*
	 * if the object is rather short, don't permit a middle point
	 */
	if (nl < 2*(tipstraight+1))
		nl &= 0xfffe;
	nl &= 0xfffffffe;
	pdom = makepolydmn(1,NULL,0,nl,1);
	pdom->nvertices = nl;
	vtx = pdom->vtx + 1;
	ax = makemain(10,pdom,NULL,NULL,NULL);
	/*
	 * loop, filling axis polygon
	 */
	nl2 = nl/2 - 1;
	wtx = vtx + nl - 3;
	for (i=0; i<nl2; i++) {
		l1 = tip + i*step; 
		l2 = idom->lastln - idom->line1 - l1;
		/*
		 * If this interval is not empty define this vertex in the polygon
		 * and step forward
		 */
		if ((n = idom->intvlines[l1].nintvs) != 0) {
			itvl = idom->intvlines[l1].intvs;
			vtx->vtX = idom->kol1 + (itvl->ileft + itvl[n-1].iright)/2;
			vtx->vtY = l1 + idom->line1;
			vtx++;
		}
		/*
		 * If this interval is not empty define this vertex in the polygon
		 * and step back
		 */
		if ((n = idom->intvlines[l2].nintvs) != 0) {
			itvl = idom->intvlines[l2].intvs;
			wtx->vtX = idom->kol1 + (itvl->ileft + itvl[n-1].iright)/2;
			wtx->vtY = l2 + idom->line1;
			wtx--;
		}
		/*
		 * force skipping over the "tip-straight" section
		 */
		if (i == 0) {
			i += tipstraight-1;
			vtx += tipstraight-1;
			wtx -= tipstraight-1;
		}
	}
	/*
	 * remove intermediate undefined points by copying back 
	 * reduce pdom->nvertices accordingly
	 */
	count = pdom->vtx + pdom->nvertices - wtx - 1;
	gap = wtx - vtx + 1;
	if (gap > 0) {
		pdom->nvertices -= gap;
		wtx++;
		while (count--) {
			vtx->vtX = wtx->vtX;
			vtx->vtY = wtx->vtY;
			vtx++;
			wtx++;
		}
	}
	nl = pdom->nvertices;
	nl2 = nl/2 - 1;
	/*
	 * Fill in the end-points and the "tip-straight" section
	 * by linear interpolation.
	 */
	ts = tipstraight;
	if (nl2 < tipstraight)
		ts = nl2;
	vtx = pdom->vtx;
	wtx = vtx+1;
	xtx = wtx+tipstraight;
	if (nl2 <= tipstraight)
		xtx = wtx+nl-3;
	for (i = -1; i<ts; i++,vtx++) {
		l = i<0? idom->line1-2: wtx->vtY+i*step;
		vtx->vtX = wtx->vtX +
			(l-wtx->vtY) * (wtx->vtX-xtx->vtX) / (wtx->vtY-xtx->vtY);
		vtx->vtY = l;
	}
	vtx = pdom->vtx + nl - 1;
	wtx = vtx-1;
	xtx = wtx-tipstraight;
	if (nl2 <= tipstraight)
		xtx = wtx-nl+3;
	for (i = -1; i<ts; i++,vtx--) {
		l = i<0? idom->lastln+2: wtx->vtY-i*step;
		vtx->vtX = wtx->vtX +
			(l-wtx->vtY) * (wtx->vtX-xtx->vtX) / (wtx->vtY-xtx->vtY);
		vtx->vtY = l;
	}

	return(ax);
}
