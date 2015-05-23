/*
 *      U S P A X I S 8 . C -- Unit Space Axis
 *
 *
 *  Written: Clive A Stubbings & Sarah E. A. Jones
 *           Image Recognition Systems
 *           720 Birchwood Boulevard
 *           Birchwood Science Park
 *           Warrington
 *           Cheshire
 *           WA3 7PX
 *
 *	Copyright (c) and intellectual property rights Image Recognition Systems
 *
 *  Date:    29 April 1988.
 *
 *  Modifications
 *
 *   5 Dec 1988		SEAJ	Added new variable input2 to allow for points
 *							being < 64 appart.
 *	24 Nov 1988		SEAJ	Added test to length to stop dividing by zero.
 *
 */
#include <stdio.h>
#include <wstruct.h>

struct polygondomain *uspaxis8(poly1)
register struct polygondomain *poly1;
{
	register struct ivertex *fv, *gv;
	struct polygondomain *poly2, *makepolydmn();
	int i, j, xdist, ydist, space, done, input, input2, output;
	int xleft, yleft, left, length;

	fv = poly1->vtx;
	space = 31;
	for (i = 1, j = 0; i<poly1->nvertices; i++, j++) {
		xdist = fv[i].vtX - fv[j].vtX;
		if (xdist < 0)
			xdist = -xdist;
		ydist = fv[i].vtY - fv[j].vtY;
		if (ydist < 0)
			ydist = -ydist;
		space += (xdist + ydist);
	}
	poly2 = makepolydmn(1,NULL,0,space/8,1);
	
	gv = poly2->vtx;
	if (poly1->nvertices == 1) {
		gv->vtX = fv->vtX;	/* Set up single point */
		gv->vtY = fv->vtY;
		poly2->nvertices = 1;
		return(poly2);
	}
	
	done = 0;
	input = 0;
	input2 = 1;
	output = 0;

   	while (1) {
   		xdist = fv[input2].vtX - fv[input].vtX;
   		ydist = fv[input2].vtY - fv[input].vtY;
   		length = hypot8( xdist, ydist);

		if ((length >= done) && (length > 0)) {
	   		do {
   				/* values calculated are *8 to compensate for hypot8 and +32 
   				   to round the value up */
   				gv[output].vtX = fv[input].vtX + ((xdist*done + 32)/length);
	   			gv[output].vtY = fv[input].vtY + ((ydist*done + 32)/length);
   				output++;
   				done += 64;
	   		} while (done < length);
   		
	   		xleft = fv[input2].vtX - gv[output-1].vtX;
   			yleft = fv[input2].vtY - gv[output-1].vtY;
   			left = hypot8(xleft, yleft);
	   		done = 64 - left;
	   		input = input2;
	   	}
   		if (input2 >= poly1->nvertices - 1) {
   			gv[output].vtX = fv[input2].vtX;
   			gv[output].vtY = fv[input2].vtY;
   			poly2->nvertices = output + 1;
   			return(poly2);
   		}
   		input2++;
   	}
 }
