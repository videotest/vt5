/*
 * histo.c	Jim Piper
 *
 * Modifications
 *
 *	18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	29 Jul 1987		CAS		Dont use fshisto - copy relevant stuff to here
 *	07 May 1987		BDP		protection against null or empty objs
 *	13 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>

static struct object *dohisto();

/*
 * extract histogram of type 1 object
 * negative values are ignored, histogram starts at zero
 * Protection added jsut checks object for Null and returns null if in error
 * previous type check left alone, is another possible cause of null,. the 
 * routine is protected otherwise by the scanning routines. bdp 7/5/87
 */
struct object *
histo(obj)
struct object *obj;
{
	struct object *fshisto();
	struct iwspace iwsp;
	struct gwspace gwsp;
	register GREY *g;
	register int k,maxg;

	if (woolz_check_obj(obj, "histo") != 0)
		return(NULL);
	if (obj->type != 1)
		return(NULL);
	/* find maximum value */
	maxg = 0;
	initgreyscan(obj,&iwsp,&gwsp);
	while (nextgreyinterval(&iwsp) == 0) {
		g = gwsp.grintptr;
		for (k=iwsp.lftpos; k<=iwsp.rgtpos; k++) {
			if (*g > maxg)
				maxg = *g;
			g++;
		}
	}
	return(dohisto(obj,maxg));
}


static struct object *dohisto(obj,maxg)
struct object *obj;
{
	struct object *hist, *makemain();
	struct histogramdomain *hdom, *makehistodmn();
	struct iwspace iwsp;
	struct gwspace gwsp;
	register GREY *g;
	register int k, *h;
	/* make empty histogram */
	hdom = makehistodmn(1,maxg+1);
	hist = makemain(13,hdom,NULL,NULL,obj);
	/* fill histogram */
	h = hdom->hv;
	initgreyscan(obj,&iwsp,&gwsp);
	while (nextgreyinterval(&iwsp) == 0) {
		g = gwsp.grintptr;
		for (k=iwsp.lftpos; k<=iwsp.rgtpos; k++) {
			if (*g >= 0)
				h[*g] ++;
			g++;
		}
	}
	return(hist);
}



