/*
 * histosmooth.c	Jim Piper
 *
 * Modifications
 *
 *	18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	07 May 1987		BDP		protection against null or empty objs
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>


/*
 * smooth a histogram by applying a low pass filter.
 * But preserve the end points.
 * protection added to check supplied object. if probelm found then this will
 * casue an immediate return. The routine should cope with the effects of
 * an empty domain or iterations of <1 .  bdp 7/5/87
 */
histosmooth(histo, iterations)
struct object *histo;
{
	register int i;
	register int *iv;
	float	 *fv;
	register int k, kwas, n;
	register struct histogramdomain *hdom;
	double x, xwas, fd;

	if (woolz_check_obj(histo, "histosmooth") != 0 )
		return(1);
	hdom = (struct histogramdomain *) histo->idom;
	/*
	 * iterate 1-2-1 smoothing, leaving tips alone
	 */
	for (n=0; n<iterations; n++)
		switch (hdom->type) {
		case 1:
			iv = hdom->hv;
			kwas = *iv;
			*iv = kwas * 4;
			iv++;
			for (i=2; i<hdom->npoints; i++) {
				k = *iv;
				*iv = kwas + 2 * k + *(iv+1);
				iv++;
				kwas = k;
			}
			*iv = *iv * 4;
			break;
		case 2:
			fv = (float*) hdom->hv;
			xwas = *fv;
			*fv = xwas*4.0;
			fv++;
			for (i=2; i<hdom->npoints; i++) {
				x = *fv;
				*fv = (xwas + 2 * x + *(fv+1))/4.0;
				fv++;
				xwas = x;
			}
			*fv = *fv * 4.0;
			break;
		}
	/*
	 * divide vertex values by 4 ** iterations
	 */
	switch (hdom->type) {
	case 1:
		k = 4;
		for (n=1; n<iterations; n++)
			k = k*4;
		iv = hdom->hv;
		for (i=0; i<hdom->npoints; i++) {
			*iv = *iv/k;
			iv++;
		}
		break;
	case 2:
		fd = 4.0;
		for (n=1; n<iterations; n++)
			fd = fd * 4.0;
		fv = (float *) hdom->hv;
		for (i=0; i<hdom->npoints; i++) {
			*fv = *fv / fd;
			fv++;
		}
		break;
	}
	return(0);
}
