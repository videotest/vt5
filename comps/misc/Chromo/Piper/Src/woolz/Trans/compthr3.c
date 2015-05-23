/*
 * compthr3.c	Jim Piper
 *
 * Modifications
 *
 *	18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	04 Dec 1987		BDP		fixed a 0/0 problem for ybar1 
 *	05 Nov 1987		BDP		Fix added to ensure result <=255
 *  11 Aug 1987		BDP		protection against divide by zero on slope calculation
 *	06 May 1987		BDP		protection against null or empty objs
 *	2  Mar 1987		GJP		Woolz_exit
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>


/*
 * compute threshold as first point above background peak that
 * gradient falls to zero.  Similar to finding a minimum.
 * protection against Null object added. If a problem is found then zero
 * is returned. Should cope with an empty object alright.
 * Woolz -exit temporarily removed.   bdp 6/5/87
 */
compthr3 (histo)
struct object *histo;
{ 
	register int i, histmax, *pf;
	int maxh, maxi, ll, lr, lrmllp1, thr;
	double imxbar, xbar1,ybar1,cprod,xsqsum,slope;
	struct histogramdomain *hdom;

#ifdef DEBUG
	fprintf(stderr,"compthr3: entered\n");
#endif DEBUG
	if (woolz_check_obj(histo, "compthr3") != 0)
		return(0);

#ifdef DEBUG
	fprintf(stderr,"compthr3: histo checked \n");
#endif DEBUG
	hdom = (struct histogramdomain *) histo->idom;
	pf = hdom->hv;
	histmax = hdom->npoints;
	/*
	 * FIND MAXIMUM POINT OF HISTOGRAM.
	 */
#ifdef DEBUG
	fprintf(stderr,"compthr3: hdom.npoints = %d\n",histmax);
#endif DEBUG
	maxh = 0;
	for (i=0; i<histmax; i++)	{
		if (pf[i] > maxh) { 
			maxh = pf[i] ;	
			maxi = i ;	
		}
	}
#ifdef DEBUG
	fprintf(stderr,"compthr3: maxi = %d, maxh = %d \n",maxi,maxh);
#endif DEBUG
	/*
	 * search profile for zero slope point
	 * first drop to 33% of peak, as peak area is often noisy.
	 */
	for (ll=maxi; ll<histmax; ll++)
		if (3*pf[ll] < maxh)
			break;
	lr=ll+10;
#ifdef DEBUG
	fprintf(stderr,"compthr3: ll = %d, lr = %d	 \n",ll,lr);
#endif DEBUG
	for ( ; lr<histmax; ll++) {
		/*
		 * COMPUTE LEAST SQUARES FIT OF LINE TO HISTOGRAM
		 * VALUES BETWEEN LL AND LR
		 */
		xbar1 = (ll+lr)/2 ;
		ybar1 = 0.0 ;
		lrmllp1 = lr-ll+1 ;
#ifdef DEBUG
	fprintf(stderr,"compthr3: xbar1 = %f \n",xbar1);
#endif DEBUG
		for (i=(ll); i<=(lr); i++)	{
			if (pf[i] == 0) { 
				lrmllp1-- ;	
			}
			ybar1 += pf[i] ;
		}
		/* 4/12/87 possible to get a 0 /= 0; here, bad news so check first bdp */
		
		if (lrmllp1 == 0)
			ybar1 = 0;
		else
			ybar1 /= (lrmllp1) ;
		
		cprod = 0.0 ;
		xsqsum = 0.0 ;
#ifdef DEBUG
	fprintf(stderr,"compthr3: lrmllp1 = %d, ybar1 = %f \n",lrmllp1,ybar1);
#endif DEBUG
		for (i=ll; i<=lr; i++)	{
			if (pf[i] != 0) { 
				imxbar = i-xbar1 ;
				cprod += imxbar*(pf[i]-ybar1) ;
				xsqsum += imxbar*imxbar ;
			}
		}
		/* SLOPE */
#ifdef DEBUG
	fprintf(stderr,"compthr3: cprod = %f, xsqsum = %f \n",cprod,xsqsum);
#endif DEBUG
		if (xsqsum > 0.0)
			slope = cprod/xsqsum ;
		else
			break;	/* this must also be a suitable place to terminate */
		if (slope > 0.0)
			break;	/* out of "for" loop */
		lr++;
	}
	thr = ( ll + lr )/2;
#ifdef DEBUG
	fprintf(stderr,"compthr3: completed thr = %d\n",thr);
#endif DEBUG
	return( thr <= 255 ? thr : 255 ); /* ensure return value in range */
}
