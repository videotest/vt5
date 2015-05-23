/*
 * axissmooth.c		Jim Piper		4 September 1984
 *
 * Smooth a polygonal chromosome axis by applying a low pass filter to its
 * x and y coordinate lists independently, but preserve the end points.
 *
 * Routine to be used for integer polygons only, "iteratons" must be >= 2.
 *
 * Leave the resulting polygon scaled up by 8, for further fixed-point
 * sub-integral processing.
 *
 * Multiplication and division is done by shifting here, so beware of
 * effect on negative-valued polygon vertices.
 *
 *	Modifications
 *
 *	 7 Feb 1991		CAS		voids
 */

#include <stdio.h>
#include <wstruct.h>

void axissmooth(poly, iterations)
register struct polygondomain *poly;
{
	register k, i;
	register struct ivertex *iv;
	register int kwas, l;
	int lwas, n, id;
	/*
	 * iterate 1-2-1 smoothing, leaving tips alone
	 */
	if (iterations < 2)
		iterations = 2;
	for (n=0; n<iterations; n++)
		switch (poly->type) {
		case 1:
			iv = poly->vtx;
			kwas = iv->vtX;
			lwas = iv->vtY;
			iv->vtX <<= 2;
			iv->vtY <<= 2;
			iv++;
			for (i=2; i<poly->nvertices; i++) {
				k = iv->vtX;
				iv->vtX = kwas + (k<<1) + (iv+1)->vtX;
				kwas = k;
				l = iv->vtY;
				iv->vtY = lwas + (l<<1) + (iv+1)->vtY;
				lwas = l;
				iv++;
			}
			iv->vtX <<= 2;
			iv->vtY <<= 2;
			break;
		default:
			break;
		}
	/*
	 * divide vertex values by (4 ** iterations)/8
	 */
	switch (poly->type) {
	case 1:
		id = iterations*2 - 3;
		iv = poly->vtx;
		for (i=0; i<poly->nvertices; i++) {
			iv->vtX >>= id;
			iv->vtY >>= id;
			iv++;
		}
		break;
	default:
		break;
	}
	return;
}
