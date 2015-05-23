/*
 *	S K E L E T O N . C
 *
 *
 */

#define register	/* take out all registers until V2.0 compiler fixed */
#define NO_4CON_INT_POINTS 	/* ignore 4-connected interior points */

/*
 * skeleton.c	Jim Piper	22 October 1984
 *
 ******************************************************************************
 * A proper interval-domain skeletonisation by Hilditch's method.
 *
 * This algorithm iterates a two-stage process with three interval domain
 * objects as follows:
 *	 Stage 1 - remove points according to algorithm:
 *		(i) is current input object
 *		(ii) is current subset of (i) where a point deletion is possible
 *		(iii) is the set of points deleted during this pass
 *	 Stage 2 - tidy up for next pass:
 *		(a) check for termination - object (iii) empty
 *		(b) construct new, thinned input object (i) by subtracting
 *			object (iii) from old object (i)
 *		(c) construct new object (ii) by taking dilation of (iii) and
 *			intersecting with new (i)
 * The method can be further improved by reducing the number of points scanned
 * in the first pass to edge points.  These are extracted as the difference
 * between the original object and its erosion.
 *
 ******************************************************************************
 * IMPLEMENT HILDITCH'S TESTS 1-6 BY TABLE LOOK UP (48 words, 32 bits)
 *
 *
 * Neighbour order is as follows:
 *			3   2*  1 	asterisk marks neighbours
 *			4*     0=8	where "removed this pass" (RTP)
 *			5   6   7	is relevant to algorithm
 *
 * Use neighbours 0 - 4  and RTP 2 to generate an address in range 0 - 47
 *	(neighbour 2 has three effective values: not in object previously,
 *	in object now, removed this pass), bit values as follows:
 *		8      16/32*      4
 *		2        -         1
 *		-        -         -
 * OR IF "NO_4CON_INT_POINTS"
 *		2      16/32*      1
 *		8        -         4
 *		-        -         -
 *
 * Use neighbours 5-7, RTP 4, and the smoothing criterion to generate a
 *	5 bit bit-address, look at corresponding bit in addressed word
 *	in look-up table.  If zero, retain point.  Address values as follows:
 *		-        -         -
 *		8*       -         -		(smoothing criterion 16)
 *		2        4         1
 *
 * Thus lookup table is 48 words long, 32 bit integer values.
 *
 * Look-up table constructed by separate program "newskelsetup.c"
 ******************************************************************************
 *
 * Modifications
 *
 *	18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	11 May 1987		BDP		protection against null or empty objs
 *	15 Oct 1986		CAS			Fixed linkcount - more for consistancy than
 *								anything else
 *	30 Sep 1986		CAS			Change malloc to Malloc
 *	13 Sep 1986		CAS			Includes
 */


/*
 ******************************************************************************
 * The following defines are left over from the development
 * and testing of this and other algorithms
 */
/*#define MONITOR		/* monitoring statements */
/*#define P_DEL_OBJ 1		/* construct potentially_deletable object */
/*#define P_DILATION 1		/* potentially deletable object is dilation */
/*#define EROSION 1		/* first p-d-object is orig - erosion_4 */
/*
 * NOTE THAT WE MUST INTERSECT POTDEL AND SKELOBJ ON EACH ITERATION
 * OR ALTERNATIVELY EXPLICITLY LOOK AT EACH ELEMENT OF POTDEL AND
 * SEE IF A MEMBER OF SKELOBJ OTHERWISE WE MAY DECIDE TO DELETE
 * POINTS OUTSIDE SKELOBJ AND THEN MUCK UP POINTS WITHIN SKELOBJ.
 ******************************************************************************
 */

#include <stdio.h>
#include <wstruct.h>

#define MIN(a,b) (a<b? a: b)

/* a line of intervals plus current position information */
struct skintvline {
	struct interval *intp;
	SMALL intcount;
	SMALL ijntpos;
	COORD ijleft;
	COORD ijright;
	COORD prevright;
};

struct extintv {
	COORD jjleft;
	COORD jjright;
	COORD nextleft;
};

static unsigned long lut[48] = {
#ifdef NO_4CON_INT_POINTS
	000075400340, 000000200000, 000000200000, 000000000000, 
	000074600362, 000074600363, 000000000000, 000000000000, 
	036475372364, 000000000000, 000175200765, 000000000000, 
	036074170360, 036074170360, 000074000360, 000074000360, 
	000000200000, 000000200001, 000000200001, 000000200001, 
	000074600363, 000074600363, 000074600363, 000074600363, 
	000175200765, 000175200765, 000175200765, 000175200765, 
	000303601417, 000303601417, 000303601417, 000303601417, 
	000000000000, 000000200001, 000000200001, 000000000000, 
	000074600363, 000074600363, 000000000000, 000000000000, 
	000175200765, 000000000000, 000175200765, 000000000000, 
	000000000000, 000000000000, 000000000000, 000000000000
#else NO_4CON_INT_POINTS
	000075400340, 000074600362, 036475372364, 036074170360,
	000000200000, 000074600363, 000000000000, 036074170360,
	000000200000, 000000000000, 000175200765, 000074000360,
	000000000000, 000000000000, 000000000000, 000074000360,
	000000200000, 000074600363, 000175200765, 000303601417,
	000000200001, 000074600363, 000175200765, 000303601417,
	000000200001, 000074600363, 000175200765, 000303601417,
	000000200001, 000074600363, 000175200765, 000303601417,
	000000000000, 000074600363, 000175200765, 000000000000,
	000000200001, 000074600363, 000000000000, 000000000000,
	000000200001, 000000000000, 000175200765, 000000000000,
	000000000000, 000000000000, 000000000000, 000000000000
#endif NO_4CON_INT_POINTS
};
static struct interval *spareitvl = 0;
static int sparecount = 0, maxkols = 0;
/*
 * protection added will return null if null objec tsupplied, and a copy of
 * an empty object if supplied.
 */
struct object *
skeleton (obj,smoothpasses)
struct object *obj;
{
	struct object *skelobj, *potdel, *makemain(), *diffdom();
#ifdef NO_4CON_INT_POINTS
	struct intervaldomain *deldom, *altdeldom;
	struct intervaldomain *makedomain(), *newidomain();
#else NO_4CON_INT_POINTS
	struct object *ob1, *delobj, *intersect2(), *dilation(), *erosion4();
	struct intervaldomain *s;
#endif NO_4CON_INT_POINTS
	int passno, i, delarea;
#ifdef MONITOR
	int skelarea;
#endif MONITOR

	/* first check input object */
	
	if (woolz_check_obj(obj, "skeleton") != 0)
		return(NULL);
	if (wzemptyidom(obj->idom) > 0)
		return(makemain(1,newidomain(obj->idom), NULL, NULL, NULL));
		
	/*
	 * copy input object, and make plenty of space for intermediate
	 * intervals in the deleted-this-pass object
	 */
#ifndef NO_4CON_INT_POINTS
	delobj = makemain(1,makedomain(1,obj->idom->line1,obj->idom->lastln,
			obj->idom->kol1,obj->idom->lastkl),NULL,NULL,obj);
	delobj->idom->freeptr = (char *) Malloc((100+8*intcount(obj->idom))*sizeof(struct interval));
	delobj->idom->linkcount++;
#else NO_4CON_INT_POINTS
	deldom = makedomain(1,obj->idom->line1,obj->idom->lastln,
			obj->idom->kol1,obj->idom->lastkl);
	deldom->freeptr = (char *) Malloc((100+8*intcount(obj->idom))*sizeof(struct interval));
	deldom->linkcount = 1;
	altdeldom = makedomain(1,obj->idom->line1,obj->idom->lastln,
			obj->idom->kol1,obj->idom->lastkl);
	altdeldom->freeptr = (char *) Malloc((100+8*intcount(obj->idom))*sizeof(struct interval));
	altdeldom->linkcount = 1;
	skelobj = makemain(1,NULL,NULL,NULL,obj);
#endif NO_4CON_INT_POINTS

#ifdef NO_4CON_INT_POINTS
	/*
	 * first pass - output thinned object in delobj
	 */
	strip(obj,deldom,&delarea,smoothpasses--);
	/*
	 * subsequent passes - use output of previous pass as input,
	 * swap between the two temporary objects.
	 */
	while (delarea > 0) {
		skelobj->idom = deldom;
		deldom = altdeldom;
		altdeldom = skelobj->idom;
		strip(skelobj,deldom,&delarea,smoothpasses--);
	}
	/*
	 * now need to standardise the interval domain, copy for
	 * output object, and free workspace
	 */
/*
	standardidom(deldom);
*/
	skelobj->idom = newidomain(deldom);
	freedomain(deldom);
	freedomain(altdeldom);
	return(skelobj);
#else NO_4CON_INT_POINTS
	/*
	 * first pass - the potentially deletable object is input object
	 */
#ifdef EROSION
	ob1 = erosion4(obj);
	ob1->vdom = ob1->plist = NULL;
	potdel = diffdom(obj,ob1);
	freeobj(ob1);
#else EROSION
	potdel = obj;
#endif EROSION
	skelobj = obj;
#ifdef MONITOR
	skelarea = area(skelobj);
	fprintf(stderr,"original area %d potarea %d\n",skelarea,area(potdel));
#endif MONITOR
	strip(skelobj,potdel,delobj,&delarea,smoothpasses--);
	passno = 0;
	/*
	 * subsequent passes
	 */
	while (delarea > 0) {
		ob1 = skelobj;
		skelobj = diffdom(ob1,delobj);
		if (++passno > 1) {
			freeobj(ob1);
#ifdef P_DILATION
			freeobj(potdel);
#endif P_DILATION
		}
#ifdef P_DILATION
		ob1 = dilation(delobj);
		potdel = intersect2(ob1,skelobj);
		freeobj(ob1);
#else P_DILATION
		potdel = skelobj;
#endif P_DILATION
#ifdef MONITOR
	skelarea = area(skelobj);
	fprintf(stderr,"delarea %4d skelarea %4d potarea %4d\n",delarea,skelarea,area(potdel));
#endif MONITOR
		strip(skelobj,potdel,delobj,&delarea,smoothpasses--);
	}
	skelobj->vdom = NULL;
	return(skelobj);
#endif NO_4CON_INT_POINTS
}


/*
 * The following macros return bit values according to whether points
 * lies within an interval of a specified line of intervals,
 * in the following patterns with corresponding bit values returned :
 *	POINTS1()		x		16
 *	POINTS2()		x     x		2     1
 *	POINTS3()		x  x  x		2  4  1
 * In each case, "intl.ijntpos" provides a memory of where the previous call
 * to the macro on this line got to, and is set to 0 on empty lines or -1
 * when beyond end of last interval.
 * "Intl.intp" provides the memory of the interval position.
 */

#ifdef NO_4CON_INT_POINTS
strip(skelobj,deldom,delarea,smoothp)
struct object *skelobj;
struct intervaldomain *deldom;
#else NO_4CON_INT_POINTS
strip(skelobj,potdel,delobj,delarea,smoothp)
struct object *skelobj, *potdel, *delobj;
#endif NO_4CON_INT_POINTS
int *delarea;
{ 
	struct intervaldomain *skeldom;
#ifndef NO_4CON_INT_POINTS
	struct intervaldomain *deldom;
#endif NO_4CON_INT_POINTS
	register struct interval *intp;
	struct interval *jntp;
	register struct intervalline *intl;
	struct intervalline *lintl;
	struct iwspace iwsp;
	struct skintvline dm1lint, lm1lint, lp1lint;
/*#ifdef P_DEL_OBJ*/
	struct skintvline llint;
/*#endif P_DEL_OBJ*/
	register int k, b, w, delflipflop;
	int lintc, lastrem, k1, kl;
	int rtp, smoothbits, l, skl1, skll, pdl1, v, km1, wa;


	smoothbits = (smoothp>=0? 16: 0);
	*delarea = 0;
	skeldom = skelobj->idom;
	skl1 = skeldom->line1;
	skll = skeldom->lastln;
#ifdef NO_4CON_INT_POINTS
	pdl1 = skl1;
#else NO_4CON_INT_POINTS
	pdl1 = potdel->idom->line1;
#endif NO_4CON_INT_POINTS

	/* set up interval table for delobj */
#ifndef NO_4CON_INT_POINTS
	deldom = delobj->idom;
#endif NO_4CON_INT_POINTS
	jntp = (struct interval *) deldom->freeptr;
	intl = deldom->intvlines;
	l = skll - skl1;
	for (k=0; k<=l; k++)
		(intl++)->nintvs = 0;

	/* start scanning potdel */
#ifdef NO_4CON_INT_POINTS
	initrasterscan(skelobj,&iwsp,0);
#else NO_4CON_INT_POINTS
	initrasterscan(potdel,&iwsp,0);
#endif NO_4CON_INT_POINTS
	while (nextinterval(&iwsp) == 0) {
		/*
		 * if beginning of line, much to set up
		 */
		if (iwsp.nwlpos != 0) {
			l = iwsp.linpos - skl1;
			/* we know current line has intervals */
			lintl = skeldom->intvlines + l;
#ifdef P_DEL_OBJ
			if (l >= 0) {
				llint.intcount = lintl->nintvs;
				llint.intp = intp = lintl->intvs;
				llint.ijleft = intp->ileft;
				llint.ijright = intp->iright;
				llint.ijntpos = 0;
			} else
				llint.intcount = 0;
#endif P_DEL_OBJ
			if (l > 0 ) {
				intl = lintl - 1;
				if ((lm1lint.intcount = (intl)->nintvs) > 0) {;
					lm1lint.intp = intp = intl->intvs;
					lm1lint.ijleft = intp->ileft;
#ifdef NO_4CON_INT_POINTS
					lm1lint.prevright =
#endif NO_4CON_INT_POINTS
					lm1lint.ijright = intp->iright;
#ifndef NO_4CON_INT_POINTS
					lm1lint.ijntpos = 0;
#endif NO_4CON_INT_POINTS
				}
			} else {
				lm1lint.intcount = 0;
			}
			if (iwsp.linpos > pdl1) {
				intl = deldom->intvlines + l - 1;
				if ((dm1lint.intcount = intl->nintvs) > 0) {
					dm1lint.intp = intp = intl->intvs;
					dm1lint.ijleft = intp->ileft;
#ifdef NO_4CON_INT_POINTS
					dm1lint.prevright =
#endif NO_4CON_INT_POINTS
					dm1lint.ijright = intp->iright;
#ifndef NO_4CON_INT_POINTS
					dm1lint.ijntpos = 0;
#endif NO_4CON_INT_POINTS
				}
			} else {
				dm1lint.intcount = 0;
			}
			if (iwsp.linpos < skll) {
				if ((lp1lint.intcount = (++lintl)->nintvs) > 0) {;
					lp1lint.intp = intp = lintl->intvs;
					lp1lint.ijleft = intp->ileft;
#ifdef NO_4CON_INT_POINTS
					lp1lint.prevright =
#endif NO_4CON_INT_POINTS
					lp1lint.ijright = intp->iright;
#ifndef NO_4CON_INT_POINTS
					lp1lint.ijntpos = 0;
#endif NO_4CON_INT_POINTS
				}
			} else
				lp1lint.intcount = 0;
			lintc = 0;
			intp = jntp;
		}
		/*
		 * compute deletable points in this interval
		 */
		lastrem = 0;
		delflipflop = 0;
		k1 = iwsp.lftpos - skeldom->kol1; 
		kl = iwsp.rgtpos - skeldom->kol1;
		for (k=k1; k<=kl; k++) {
#ifdef NO_4CON_INT_POINTS
	{if (lm1lint.intcount <= 0 || k < lm1lint.ijleft) { 
		w = 0; 
		goto exitP1a; 
	} 
	while (k > lm1lint.ijright) { 
		if (--lm1lint.intcount <= 0) { 
			lm1lint.intcount = -1;
			w = 0; 
			goto exitP1a; 
		} 
		else { 
			lm1lint.prevright = lm1lint.ijright; 
			lm1lint.ijleft = (++lm1lint.intp)->ileft; 
			lm1lint.ijright = lm1lint.intp->iright; 
			if (k < lm1lint.ijleft) { 
				w = 0; 
				goto exitP1a; 
			} 
		} 
	} 
	w = 16; 
exitP1a: ; 
}
			if (k > k1)
				w += 8;
			if (k < kl)
				w += 4;
	{if (lp1lint.intcount <= 0 || k < lp1lint.ijleft) { 
		b = 0; 
		goto exitP1b; 
	} ;
	while (k > lp1lint.ijright) { 
		if (--lp1lint.intcount <= 0) { 
			lp1lint.intcount = -1;
			b = 0; 
			goto exitP1b; 
		} 
		else { 
			lp1lint.prevright = lp1lint.ijright; 
			lp1lint.ijleft = (++lp1lint.intp)->ileft; 
			lp1lint.ijright = lp1lint.intp->iright; 
			if (k < lp1lint.ijleft) { 
				b = 0; 
				goto exitP1b; 
			} 
		} 
	} 
	b = 16; 
exitP1b: ; 
}
			if (b > 0) {	/* (actually b==16) */
				/* not a 4-connected boundary point ? */
				if (w == 28) {		/* RETAIN POINT */
					lastrem = 0;	/* for next point */
					if (delflipflop == 0) {
						lintc++;
						delflipflop = 1;
						intp->ileft = k;
					}
					/* skip over further interior points */
					k = MIN((kl-1),lm1lint.ijright);
					k = MIN(k,lp1lint.ijright);
					intp->iright = k;
					continue;	/* WITH LOOP */
				}
				b = 4;
			}
			/*
			 * If neighbour 2 present, see if it still present
			 * in thinned object "delobj"
			 */
			if (w >= 16) {
	{if (dm1lint.intcount <= 0 || k < dm1lint.ijleft) { 
		rtp = 0; 
		goto exitP1c; 
	} 
	while (k > dm1lint.ijright) { 
		if (--dm1lint.intcount <= 0) { 
			dm1lint.intcount = -1;
			rtp = 0; 
			goto exitP1c; 
		} 
		else { 
			dm1lint.prevright = dm1lint.ijright; 
			dm1lint.ijleft = (++dm1lint.intp)->ileft; 
			dm1lint.ijright = dm1lint.intp->iright; 
			if (k < dm1lint.ijleft) { 
				rtp = 0; 
				goto exitP1c; 
			} 
		} 
	} 
	rtp = 16; 
exitP1c: ; 
}
				w += (16-rtp);
			}
/* here "k" refers to the middle position in the run of three */
/* also, interwal stepping has already been performed by POINTS1 */
{ /* first check if any interwals in line */ 
	if (lm1lint.intcount != 0) { 
		/* past end of last interwal in line ? */ 
		if (lm1lint.intcount < 0) { 
			if (k-1 == lm1lint.ijright) 
				w += 2; 
		/* otherwise not yet past last interwal, check cases */ 
		} else { 
			if (k > lm1lint.ijleft || k-1 == lm1lint.prevright) 
				w += 2; 
			if (k < lm1lint.ijright && k+1 >= lm1lint.ijleft) 
				w++; 
		} 
	} 
}
			w = lut[w];
			if (w != 0) {
/* here "k" refers to the middle position in the run of three */
/* also, interbal stepping has already been performed by POINTS1 */
{ /* first check if any interbals in line */ 
	if (lp1lint.intcount != 0) { 
		/* past end of last interbal in line ? */ 
		if (lp1lint.intcount < 0) { 
			if (k-1 == lp1lint.ijright) 
				b += 2; 
		/* otherbise not yet past last interbal, check cases */ 
		} else { 
			if (k > lp1lint.ijleft || k-1 == lp1lint.prevright) 
				b += 2; 
			if (k < lp1lint.ijright && k+1 >= lp1lint.ijleft) 
				b++; 
		} 
	} 
}
				b += (smoothbits + lastrem);
				w &= (1 << b);
			}
#else NO_4CON_INT_POINTS
			km1 = k-1;
/* here "k" refers to the left-most position in the run of three */
	{
	if (lm1lint.intcount <= 0) { 
		w = 0; 
		goto exitP3a; 
	} 
	while (km1 > lm1lint.ijright) { 
		if (++(lm1lint.ijntpos) >= lm1lint.intcount) { 
			lm1lint.intcount = -1; 
			w = 0; 
			goto exitP3a; 
		} 
		else {			/* increment interwal pointer */ 
			lm1lint.ijleft = (++lm1lint.intp)->ileft; 
			lm1lint.ijright = lm1lint.intp->iright; 
		} 
	} 
	if (km1+2 < lm1lint.ijleft) {				/* 0 0 0 */ 
		w = 0; 
		goto exitP3a; 
	} else if (km1 < lm1lint.ijleft) { 
		if (km1+2 == lm1lint.ijleft) {		/* 0 0 1 */ 
			w = 1; 
			goto exitP3a; 
		} else { 
			if (km1+2 <= lm1lint.ijright) {	/* 0 1 1 */ 
				w = 5; 
				goto exitP3a; 
			} else {			/* 0 1 0 */ 
				w = 4; 
				goto exitP3a; 
			}
		} 
	} else { 
		/* km1 now lies within an interwal */ 
		if (km1+2 <= lm1lint.ijright) {		/* 1 1 1 */ 
			w = 7; 
			goto exitP3a; 
		} else if (km1+1 == lm1lint.ijright) {	/* 1 1 0 */ 
			w = 6; 
			goto exitP3a; 
		} else if (lm1lint.ijntpos < lm1lint.intcount && 
		    	km1+2 == ((struct extintv *)(lm1lint.intp))->nextleft) { 
			w = 3;				/* 1 0 1 */ 
			goto exitP3a; 
		} else {				/* 1 0 0 */ 
			w = 2; 
		} 
	} 
exitP3a: ; 
}
			w <<= 2;
/* here "km1" refers to the left-most position in the run of three */
{	if (llint.intcount <= 0) { 
		goto exitP2; 
	} 
	while (km1 > llint.ijright) { 
		if (++llint.ijntpos >= llint.intcount) { 
			llint.intcount = -1; 
			goto exitP2; 
		} 
		else {			/* increment interwal pointer */ 
			llint.ijleft = (++llint.intp)->ileft; 
			llint.ijright = llint.intp->iright; 
		} 
	} 
	if (km1+2 < llint.ijleft) {				/* 0 0 0 */ 
		goto exitP2; 
	} else if (km1 < llint.ijleft) { 
		if (km1+2 <= llint.ijright) {		/* 0 0 1, 0 1 1 */ 
			w += 1; 
			goto exitP2; 
		} else {				/* 0 1 0 */ 
			goto exitP2; 
		} 
	} else { 
		/* km1 now lies within an interwal */ 
		if (km1+2 <= llint.ijright) {		/* 1 1 1 */ 
			w += 3; 
			goto exitP2; 
		} else if (km1+1 == llint.ijright) {	/* 1 1 0 */ 
			w += 2; 
			goto exitP2; 
		} else if (llint.ijntpos < llint.intcount && 
		    	km1+2 == ((struct extintv *)(llint.intp))->nextleft) { 
			w += 3;				/* 1 0 1 */ 
			goto exitP2; 
		} else					/* 1 0 0 */ 
			w += 2; 
	} 
exitP2: ; 
}
{	if (dm1lint.intcount <= 0 || k < dm1lint.ijleft) 
		goto exitP1; 
	while (k > dm1lint.ijright) { 
		if (++dm1lint.ijntpos >= dm1lint.intcount) { 
			dm1lint.intcount = -1; 
			goto exitP1; 
		} 
		else { 
			dm1lint.prevright = dm1lint.ijright; 
			dm1lint.ijleft = (++dm1lint.intp)->ileft; 
			dm1lint.ijright = dm1lint.intp->iright; 
			if (k < dm1lint.ijleft) { 
				goto exitP1; 
			} 
		} 
	} 
	w += 16; 
exitP1: ; 
}
			w = lut[w];
			if (w != 0) {
/* here "k" refers to the left-most position in the run of three */
	{
	if (lp1lint.intcount <= 0) { 
		b = 0; 
		goto exitP3b; 
	} 
	while (km1 > lp1lint.ijright) { 
		if (++(lp1lint.ijntpos) >= lp1lint.intcount) { 
			lp1lint.intcount = -1; 
			b = 0; 
			goto exitP3b; 
		} 
		else {			/* increment interbal pointer */ 
			lp1lint.ijleft = (++lp1lint.intp)->ileft; 
			lp1lint.ijright = lp1lint.intp->iright; 
		} 
	} 
	if (km1+2 < lp1lint.ijleft) {				/* 0 0 0 */ 
		b = 0; 
		goto exitP3b; 
	} else if (km1 < lp1lint.ijleft) { 
		if (km1+2 == lp1lint.ijleft) {		/* 0 0 1 */ 
			b = 1; 
			goto exitP3b; 
		} else { 
			if (km1+2 <= lp1lint.ijright) {	/* 0 1 1 */ 
				b = 5; 
				goto exitP3b; 
			} else {			/* 0 1 0 */ 
				b = 4; 
				goto exitP3b; 
			}
		} 
	} else { 
		/* km1 nob lies bithin an interbal */ 
		if (km1+2 <= lp1lint.ijright) {		/* 1 1 1 */ 
			b = 7; 
			goto exitP3b; 
		} else if (km1+1 == lp1lint.ijright) {	/* 1 1 0 */ 
			b = 6; 
			goto exitP3b; 
		} else if (lp1lint.ijntpos < lp1lint.intcount && 
		    	km1+2 == ((struct extintv *)(lp1lint.intp))->nextleft) { 
			b = 3;				/* 1 0 1 */ 
			goto exitP3b; 
		} else {				/* 1 0 0 */ 
			b = 2; 
		} 
	} 
exitP3b: ; 
}
				b += (smoothbits + lastrem);
				w &= (1 << b);
			}
#endif NO_4CON_INT_POINTS
			if (w != 0) {		/* REMOVE POINT */
				(*delarea)++;
				lastrem = 8;	/* for next point */
#ifdef NO_4CON_INT_POINTS
				if (delflipflop == 1) {
					delflipflop = 0;
					intp++;
				}
#else NO_4CON_INT_POINTS
				if (delflipflop == 0) {
					lintc++;
					delflipflop = 1;
					intp->ileft = k;
				}
				intp->iright = k;
#endif NO_4CON_INT_POINTS
			} else {			/* RETAIN POINT */
				lastrem = 0;	/* for next point */
#ifdef NO_4CON_INT_POINTS
				if (delflipflop == 0) {
					lintc++;
					delflipflop = 1;
					intp->ileft = k;
				}
				intp->iright = k;
#else NO_4CON_INT_POINTS
				if (delflipflop == 1) {
					delflipflop = 0;
					intp++;
				}
#endif NO_4CON_INT_POINTS
			}
		}
		/*
		 * clear up an open interval end
		 */
		if (delflipflop == 1)
			intp++;
		/*
		 * tidy up at end of line
		 */
		if (iwsp.intrmn == 0) {
			makeinterval(iwsp.linpos,deldom,lintc,jntp);
			jntp = intp;
		}
	}
}
