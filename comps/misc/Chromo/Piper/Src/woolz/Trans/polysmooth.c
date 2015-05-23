/*
 * polysmooth.c		Jim Piper	November 30 1983
 * polygon domain operations
 *
 * Modifications
 *
 *	13 Nov 1987		BDP		Change iterations loops to ensure divisions happen 
 *							every loop to minimise chance of overflow of 16 bit coords
 *	12 Nov 1987		BDP		Check against single point polygons
 *	08 May 1987		BDP		protection against null or empty objs
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

/*
 * smooth a polygon by applying a low pass filter to its
 * x and y coordinate lists independently.
 * But preserve the end points.
 * Protection added just checks for domain existing. routine is secure against
 * an iterations of zero parameter, and now protected against polygons with
 * less than three points, the minimum number required for filtering
 */

/*
 *  !!  N.B. !!  This routine will now operate as safely on a large number of
 *  iterations as for one iteration.  However in all cases it is necessary to
 *	ensure that COORD vertices do not exceed 8192 otherwise an overflow error
 *  possibly giving a negative result will occur.  bdp 13/11/87
 */
polysmooth(poly, iterations)
register struct polygondomain *poly;
{
	register int i;
	register struct ivertex *iv;
	register struct fvertex *fv;
	register int k, kwas, l;
	int lwas,n;
	double x, xwas, y, ywas;

	if (poly == NULL)
		return(1);
	
	if ( poly->nvertices < 3)
		return(0);			/* not an error, but cant do anything, also < 2 will
							crash it !  bdp 12/11/87  */
	/*
	 * iterate 1-2-1 smoothing, leaving tips alone
	 * now (13/11/87) do both mult and div for each loop. Also no point in
	 * multiplying first and last points, just dont divide them later !
	 */
	for (n=0; n<iterations; n++)
	{
		switch (poly->type) {
		case 1:
			iv = poly->vtx;
			kwas = iv->vtX;
			lwas = iv->vtY;
			iv++;
			for (i=2; i<poly->nvertices; i++) {
				k = iv->vtX;
				iv->vtX = kwas + 2 * k + (iv+1)->vtX;
				kwas = k;
				l = iv->vtY;
				iv->vtY = lwas + 2 * l + (iv+1)->vtY;
				lwas = l;
				iv++;
			}
			iv = poly->vtx;
			iv++;			/* skip first vertex as weve not scaled that */
			for (i=2; i<poly->nvertices; i++) {
				iv->vtX /= 4;
				iv->vtY /= 4;
				iv++;
			}
			break;
		case 2:
			fv = (struct fvertex *) poly->vtx;
			xwas = fv->vtX;
			ywas = fv->vtY;
			fv++;
			for (i=2; i<poly->nvertices; i++) {
				x = fv->vtX;
				fv->vtX = xwas + 2 * x + (fv+1)->vtX;
				xwas = x;
				y = fv->vtY;
				fv->vtY = ywas + 2 * y + (fv+1)->vtY;
				ywas = y;
				fv++;
			}
			fv = (struct fvertex *) poly->vtx;
			fv++;
			for (i=2; i<poly->nvertices; i++) {
				fv->vtX /= 4.0;
				fv->vtY /= 4.0;
				fv++;
			}
			break;
		}
	}
	return(0);
}
