/*
 *      H T N O R M A L I S E  --  Normalise obj + compress top +
 *									bottom n% of grey range
 *
 *
 *  Written:
 *           Image Recognition Systems
 *           720 Birchwood Boulevard
 *           Birchwood Science Park
 *           Warrington
 *           Cheshire
 *           WA3 7PX
 *
 *	Copyright (c) and intellectual property rights Image Recognition Systems
 *
 *  Date:    14th January 1987
 *
 *  Modifications
 *
 *	18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	05 Nov 1987		BDP		Check input params will prevent /0s
 *	07 May 1987		BDP		protection against null or empty objs
 */

#include <stdio.h>
#include <wstruct.h>

/*
 * Protection against null and empty objects added. if either detected then
 * early return will a status indicator will result. bdp 7/5/87
 *
 */
htnormalise (obj,toppc,botpc)
struct object *obj;
float toppc,botpc;
{
	struct object *h, *histo();
	struct histogramdomain *hdom;
	register int mini,diff,i,*gv;
	register GREY *g;
	int maxi,tsum,bsum;
	long hsum;
	struct iwspace iwsp;
	struct gwspace gwsp;

	if (woolz_check_obj(obj, "htnormalise") != 0)
		return(1);
	if ( toppc + botpc >= 100 )
		return(1);		/* this would be stupid and could cause problems albeit
						highly unlikely	*/
	h = histo(obj);
	hdom = (struct histogramdomain *) h->idom;
	if (hdom->npoints <= 0)
		return(1);
	gv = hdom->hv;
	hsum = 0;
	for (i=0; i<hdom->npoints; i++)
		hsum += gv[i];
	tsum = hsum*toppc/100;
	bsum = hsum*botpc/100;
	for (mini = 0; mini < hdom->npoints-1; mini++) {
		bsum -= gv[mini];
		if (bsum < 0)
			break;
	}
	for (maxi = hdom->npoints-1; maxi > 0; maxi--) {
		tsum -= gv[maxi];
		if (tsum < 0)
			break;
	}
	freeobj(h);

/*	fprintf(stderr,"truncated histogram min %d max %d\n",mini,maxi); */
	mini--;
	diff = maxi - mini;
	initgreyscan(obj,&iwsp,&gwsp);
	while (nextgreyinterval(&iwsp) == 0) {
		g = gwsp.grintptr;
		for (i=iwsp.lftpos; i<=iwsp.rgtpos; i++) {
			*g = (*g - mini)*255/diff;
			if (*g > 255)
				*g = 255;
			else if (*g < 0)
				*g = 0;
			g++;
		}
	}
	return(0);
}

