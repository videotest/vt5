/*
 * wdd.c		Jim Piper		February 1984
 *
 * compute the Weighted Density Distribution of banded chromosome profiles
 * (see e.g. Erik Granum's thesis).  We compute here the functions 1-4 and 6.
 * wdd5 computed here is the same as wdd2 but phase shifted; the aim is
 * to replace wdd1 with something more robust.
 *
 *	Wdd coefficients are computed in range -255 to 255 
 *	(permitting the computation of odd-valued distributions in
 *	opposite direction to be obtained simply by negating the result).
 *	The computation is performed in integer arithmetic, and the
 *	profile is treated symmetrically.
 *
 *
 * Modifications
 *
 *	18 Nov 1987		SCG		Safeguard against a zero profile
 *						which would cause zero divisions
 *						in a number of functions
 *	11 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <math.h>
#include <wstruct.h>


/* v returns the wdd values */

wdd (histo, v)
struct object *histo;
register short *v;
{
	struct histogramdomain *hdom;
	register int p,i,l,u,*h,*g;
	int ll,td,w;
	int wdd1,wdd2,wdd3,wdd4,wdd5,wdd6;
	hdom = (struct histogramdomain *) histo->idom;

	l = hdom->npoints;
	ll = l*2;
	/*
	 * wdd2 - take care with central point if odd number of points
	 * wdd5 - derive from wdd4 computation
	 */
	h = hdom->hv;
	g = h+l-1;
	td = wdd1 = wdd2 = wdd3 = wdd4 = wdd5 = wdd6 = 0;

	for (i=1; i<l; i+=2) {
		w = *h - *g;
		u = *h++ + *g--;
		td += u;
		p = 3072*i/l;
		wdd1 += w * (p - 3072);
		wdd2 += u * (p - 1536);
		if (i*3 <= ll) {
			wdd3 += w * (p - 1024);
		} else {
			wdd3 += w * (3072 - p);
		}
		if (i*4 <= ll) {
			wdd4 += u * (p - 768);
			wdd5 += w * (p - 768);
		} else {
			wdd4 += u * (2304 - p);
			wdd5 += w * (2304 - p);
		}
		if (i*6 <= ll)
			wdd6 += u * (p - 512);
		else if (i*3 > ll)
			wdd6 += u * (p - 2560);
		else
			wdd6 += u * (1536 - p);
	}
	/* convert back to range [-256,256] */
	wdd1 /= 12;
	wdd2 /= 6;
	wdd3 /= 4;
	wdd4 /= 3;
	wdd5 /= 3;
	wdd6 /= 2;
	/* If an odd number of points in histogram, deal suitable with middle */
	if (l&1 == 1) {
		td += *h;
		u = *h * 256;
		wdd2 += u;
		wdd4 -= u;
		wdd6 += u;
	}

	/*
	 * normalise by profile total density
	 * it is possible for 1 pixel wide objects that the profile is
	 * zero everywhere.  Safeguard here against zero division
	 *
	 */
	td /= 10;
	if(td == 0) {
		*v = 0;
		*++v = 0;
		*++v = 0;
		*++v = 0;
		*++v = 0;
		*++v = 0;
	} else {
		*v = wdd1/td;
		*++v = wdd2/td;
		*++v = wdd3/td;
		*++v = wdd4/td;
		*++v = wdd5/td;
		*++v = wdd6/td;
	}
}



/*
 * coefficient of variation (standard deviation / mean) of profile.
 */
cvdd(histo)
struct object *histo;
{
	register struct histogramdomain *hdom;
	register int l,s,ss,*h;
	hdom = (struct histogramdomain *) histo->idom;
	l = hdom->npoints;
	h = hdom->hv;
	s = 0;
	ss = 0;
	for (; l>0; l--) {
		s += *h;
		ss += *h * *h;
		h++;
	}
	ss >>= 8;  s >>= 4;		/* prevent overflow */
	if(s == 0)			/* safeguard against zero profile */
		return(0);
	else
		return(1024 * isqrt(hdom->npoints*ss - s*s) / s);
}


/*
 * Granum's NSSD of profile -
 * "normalised root of the sum of squared density differences"
 */
nssd(histo)
struct object *histo;
{
	register struct histogramdomain *hdom;
	register int l,s,ss,sd,*h;
	hdom = (struct histogramdomain *) histo->idom;
	l = hdom->npoints;
	h = hdom->hv;
	s = 0;
	sd = 0;
	for (; l>0; l--) {
		s += *h;
		ss = *h++;
		ss -= *h;
		sd += ss*ss;
	}
	if(s == 0)			/* Safeguard against zero profile */ 
		return(0);
	else
		return(1024 * isqrt8(sd) / s);
}


/*
 * return the profile of absolute differences between points of a histogram
 */
struct object *gradprof (histo)
struct object *histo;
{
	struct object *makemain();
	struct histogramdomain *hdom, *hnew, *makehistodmn();
	register int i, v, *h, *g;
	hdom = (struct histogramdomain *)histo->idom;
	hnew = makehistodmn(1,hdom->npoints - 1);
	h = hdom->hv;
	g = hnew->hv;
	hnew->r = hdom->r;
	hnew->k = hdom->k;
	hnew->l = hdom->l;
	for (i=0; i<hnew->npoints; i++) {
		v = *h++;
		v -= *h;
		if (v<0)
			v = -v;
		*g++ = v;
	}
	return(makemain(13,hnew,NULL,NULL,NULL));
}



/*
 * compute center-centroid distance and squared radius of gyration
 * from center of a profile, both normalised to profile length.
 * THIS IS SO SIMILAR TO WDD, WDD2 THAT IT IS NO LONGER USED !!
ddmoments (histo, m1, m2)
struct object *histo;
short *m1, *m2;
{
	register struct histogramdomain *hdom;
	register int i,*h;
	double d,s1,s2,sm;
	hdom = (struct histogramdomain *)histo->idom;
	s1 = 0;
	s2 = 0;
	sm = 0;
	h = hdom->hv;
	d = (double)(hdom->npoints - 1);
	for (i=0; i<hdom->npoints; i++) {
		s1 += d * *h;
		s2 += d * d * *h;
		sm += *h++;
		d -= 2.0;
	}
	if(sm == 0) {
		*m1 = 0;
		*m2 = 0;
	} else {
		sm *= hdom->npoints;
		*m1 = 1000*s1/sm;
		sm *= hdom->npoints;
		*m2 = 1000*s2/sm;
	}
}
*/
