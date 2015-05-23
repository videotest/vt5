/* 
 * showobjbox.c
 *
 * display the enclosing right rectangle of an object 
 * colour: tint ( negative means erase );
 * frame : f
 *
 * Modifications:
 *
 *	 7 Feb 1991		CAS		voids, mizar defines
 *  18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	06 May 1987		BDP		protection against null or empty objs
 */
#ifdef OSK
#include <wstruct.h>
#endif
#ifdef WIN32
#include <wstruct.h>
#endif
#ifdef VAX
#include "/mnt1/woolz/mizar/WOOLZ/h/wstruct.h"
#endif
#define OVERLAY1	1
#define OVERLAY2	2
#define OVERLAYCOLS	3

static expand=6;
static	utintb, uxb , uyb , uwb , uhb ;
static	tintb, xb , yb , wb , hb ;

/*	boxobj
	protection here provided by local routine bothbox.
	boxobj is a public level function. bdp 6/5/87 .  */

boxobj(obj,f,tint)
struct object *obj; 
struct pframe *f;
{
	 return(bothbox(obj,f,tint,0));
}


/*	backlight
	display a pixel in colour   tint   in the rectangle surround
  	of an object                obj
  	using the frame             fr
  	(but only at 'background' points - that is, only at points
    having no representation in the planes corresponding to
    OVERLAYCOL)
    Note: if tint negative, delete.
	Protection provided by bothbox. backlight is a public function */

backlight(obj,fr,tint)
struct pframe *fr; 
struct object *obj; 
{
	return(bothbox(obj,fr,tint,1));
 }


/*  bothbox
	mode 0 : disprect
	mode 1 : backlight
	Protection provided to higher level routines above against null or 
	empty objects.  If either a null or empty object is found then return
	occurs with no output . bdp 6/5/87  */

bothbox(obj,fr,tint,mode)
struct pframe *fr;
struct object *obj;
{
	register int k1,k2,l1,l2;
	register struct intervaldomain *mydom;
	int kwide,lwide,xv,yv;

	if (woolz_check_obj(obj, "bothbox") != 0)
		return(1);		/* problem indicator to anyone interested */
	if (wzemptyidom(obj->idom) > 0)
		return(1);
		
	mydom = obj->idom;
	k1 = mydom->kol1;
	k2 = mydom->lastkl;
	
	if (k2-k1 < 8) { k2 += 4; k1 -= 4; }
	l1 = mydom->line1;
	l2 = mydom->lastln;
	if (l2-l1 < 8) { l2 += 4; l1 -= 4; }
	
	if ( tint <0 ) { intens(0); tint = -tint; } else intens(1);
	
	kwide=frkdis(k2-k1+expand,fr); /* get frame-transformed kol dis*/
	lwide = 2*expand + frldis(l2-l1+expand,fr);

	/* get digs coordinates after frame transformation */
	xv = frdigsxpos(k1-expand/2,fr);
	yv = frdigsypos(l1-expand/2,fr);

	/* caution negative scales ! */
	if (lwide < 0) { lwide = -lwide; yv -= lwide ; }
	if (kwide < 0) { kwide = -kwide; xv -= kwide; }
	moveto(xv,yv);
	
	/* we are now in Digs coordinates */
	if (mode) {
	pixelmode(3);
		colour(OVERLAYCOLS);
		pixbegin(8,1,1,kwide,lwide);
		pixel(tint); 
		pixend(); 

	/* record for erasure */
	pixelmode(1);
	utintb = tint;
	uxb = xv; uyb =yv; uwb = kwide; uhb = lwide;
	} else {
		colour(tint);
		lineby (kwide,0);
		lineby (0,lwide);
		lineby (-kwide,0);
		lineto (xv,yv);

	/* record for erasure */
	tintb = tint;
	xb = xv; yb =yv; wb = kwide; hb = lwide;
		
	}
	return(0);		/* show everything been done */
}


/* erase a previously back-lighted area . This routine protected by its
own static variables . bdp 6/5/87 */
unbacklight() {
		if (!utintb) return(0);
		colour(OVERLAYCOLS);
/*		intens(0); */
		moveto(uxb,uyb);
		pixelmode(3);
		pixbegin(8,1,1,uwb,uhb);
		pixel(0); 
		pixend(); 
		pixelmode(1);
		intens(1);
		utintb=0;
}


/* erase a box drawn by bothbox. protected as above. bdp 6/5/87  */
unbox() {
		if (!tintb) return(0);
		intens(0);
		moveto(xb,yb);
		colour(tintb);
		lineby (wb,0);
		lineby (0,hb);
		lineby (-wb,0);
		lineto (xb,yb);
		intens(1);
		tintb=0;
}
