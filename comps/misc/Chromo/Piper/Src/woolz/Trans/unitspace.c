/*
 * unitspace.c		Jim Piper	November 30 1983
 * polygon domain operations
 *
 * Modifications
 *
 *	25 Nov 1987		BDP		Fixed a problem with single pixel polygons
 *	11 May 1987		BDP		protection against null or empty objs
 *	24 Sep 1986		GJP		Big Free not little free
 *	13 Sep 1986		CAS		Includes
 */


#include <stdio.h>
#include <math.h>
#include <wstruct.h>
/* 
 * protection added just to check for none existence of polygon or it
 * being empty. in either case null will get returned. bdp 11/5/87
 */
 
struct polygondomain *unitspace(p1)
struct polygondomain *p1;
{
	register int i1,i2;
	double xwas,ywas,xd,yd,rd,rd1,rd2,fract,polylength();
	int freep;
	register struct fvertex *fv,*gv;
	struct polygondomain *p2, *makepolydmn(), *inttorealpoly();

	if (p1 == NULL)
		return(NULL);
	if (p1->nvertices <= 0)
		return(NULL);
		
	/*
	 * If input is an integer-valued polygon, convert to float
	 */
	freep = 0;
	if (p1->type == 1) {
		p1 = inttorealpoly(p1);
		freep = 1;
	}
	/* 25/11/87 bdp. check at this point for single vertex polygon otherwise
	end up going off into wild blue yonder below */
	
	if (p1->nvertices == 1)
	{
		p2 = makepolydmn(2,p1->vtx,1,1,1);
		if (freep)
			Free(p1);
		return(p2);
	}
	p2 = makepolydmn(2,NULL,0,2 + (int) polylength(p1),1);
	/*
	 * i1 is next vertex point in input polygon p1.
	 * i2 is next vertex point in unit-spaced polygon p2.
	 */

	fv = (struct fvertex *) p1->vtx;
	gv = (struct fvertex *) p2->vtx;
	xwas = fv->vtX;
	gv->vtX = xwas;
	ywas = fv->vtY;
	gv->vtY = ywas;
	i1 = 1;
	i2 = 0;

	/*
	 * in what follows, rd1 is the distance from the current output vertex
	 * to the next input vertex.
	 * rd is the distance from the last input vertex along the current
	 * input arc to the next output vertex.
	 */
	for (;;) {
		i2++;
		/*
		 * find distance to next point on mid-points polygon
		 */
		xd=fv[i1].vtX-xwas;
		yd=fv[i1].vtY-ywas;
		rd1=sqrt(xd*xd+yd*yd);
		/*
		 * if greater than 1, then next point is in same interval of polygon
		 */
		if (rd1 >= 1.0) {
			xd=xwas-fv[i1-1].vtX;
			yd=ywas-fv[i1-1].vtY;
			rd=1.0+sqrt(xd*xd+yd*yd);
		} 
		else if (i1 == p1->nvertices-1) {
			/*
			 * end of input polygon. construct last point
			 * unless rd1==0, in which case we already have it
			 */
			if (rd1 > 0.0) {
				gv[i2].vtX=xwas+xd/rd1;
				gv[i2].vtY=ywas+yd/rd1;
				i2++;
			}
			p2->nvertices = i2;
			if (freep)
				Free(p1);
			return(p2);
		} 
		/*
		 * next point not in this interval. Move to next and
		 * check if in that interval
		 */
		else 
			do {
				xwas += xd;
				ywas += yd;
				i1++;
				if (i1 == p1->nvertices) {
					/* end of input polygon */
					if (rd1 > 0.0) {
						gv[i2].vtX=xwas+xd/rd1;
						gv[i2].vtY=ywas+yd/rd1;
						i2++;
					}
					p2->nvertices = i2;
					if (freep)
						Free(p1);
					return(p2);
				}
				rd=1.0-rd1;
				xd = fv[i1].vtX-xwas;
				yd = fv[i1].vtY-ywas;
				rd2 = sqrt(xd*xd + yd*yd);
				if (rd2 < rd)
					rd1 += rd2;
			} while (rd2 < rd);

		xd=fv[i1].vtX-fv[i1-1].vtX;
		yd=fv[i1].vtY-fv[i1-1].vtY;
		fract=rd/sqrt(yd*yd+xd*xd);
		gv[i2].vtX=fv[i1-1].vtX+xd*fract;
		xwas=gv[i2].vtX;
		gv[i2].vtY=fv[i1-1].vtY+yd*fract;
		ywas=gv[i2].vtY;
	}
}
