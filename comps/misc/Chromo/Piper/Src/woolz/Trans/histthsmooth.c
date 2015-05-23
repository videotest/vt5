/*
 * histthsmooth.c	Jim Piper
 *
 * Modifications
 *
 *	18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	07 May 1987		BDP		protection against null or empty objs
 *	24 Sep 1986		GJP		Big Free not little free
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>


/*
 * smooth histogram - for thresholding
 * this is a very brutal smooth
 * Protection added. check on supplied object will cause immediate return if
 * object is erroneus. Also will return imm. if empty to  prevent null malloc
 * request. bdp 7/5/87 
 */
histothreshsmooth (histo)
struct object *histo;
{
	struct histogramdomain *hdom;
	register *h,*ht,j,k;
	int i,np;
	if (woolz_check_obj(histo, "histothreshsmooth") != 0)
		return(1);
	hdom = (struct histogramdomain *) histo->idom;
	np = hdom->npoints;
	if ( np <= 0 )
		return(1);

	h = hdom->hv;
	ht = (int *) Malloc(np * sizeof(int));
	for (j=0; j<np; j++)
		ht[j] = h[j];
	for (i=0; i<np; i++) {
		for (j=i-5; j<=i+5; j++) {
			k=j;
			if (k<0)
				k=0;
			else if (k>np-1)
				k=np-1;
			h[i] += ht[k];
		}
	}
	for (j=0; j<np; j++)
		h[j] /= 11;
	Free(ht);
	return(0);
}
