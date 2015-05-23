/*
 * frameobjnum.c	Jim Piper	10 August 1983
 *
 * Modifications
 *
 *	 7 Feb 1991		CAS		voids
 *  18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *   8 Aug 1988		SEAJ	Added new function frameobjstr.
 *	 2 Nov 1987		CAS		Added some useful comments
 *	05 May 1987		BDP		protection against null or empty objs
 *	19 Nov 1986		CAS		Added routine frameobjnnum (from kyy/countsubs)
 *	12 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>


#define VERTOFFS 64

/*
 *	F R A M E O B J N U M 
 *
 * display object number immediately below object first interval
 * object protection is largely performed by protection in initrasterscan
 * and nextinterval, the object has to be checked prior to its use in the 
 * case statement however */

frameobjnum(num,obj,f)
struct object *obj;
struct pframe *f;
{
	struct iwspace iwsp;

if ( woolz_check_obj(obj, "frameobjnum") == 0 )
	/* switch on object type */
	switch(obj->type / 10) {

	case 0 :		/* interval/grey-table object */
		initrasterscan(obj,&iwsp,0);
		if (nextinterval(&iwsp) == 0) {
			dispnum(num,f->dx+f->scale*f->ix*(iwsp.colpos-f->ox),
				f->dy+f->scale*f->iy*(iwsp.linpos-f->oy)-VERTOFFS);
			update();
		}
		break;
	default :
		break;
	}
}
/*
 *	F R A M E O B J S T R
 *
 * display object number immediately below object first interval
 * object protection is largely performed by protection in initrasterscan
 * and nextinterval, the object has to be checked prior to its use in the 
 * case statement however */

frameobjstr(str,obj,f)
char *str;
struct object *obj;
struct pframe *f;
{
	struct iwspace iwsp;

if ( woolz_check_obj(obj, "frameobjstr") == 0 )
	/* switch on object type */
	switch(obj->type / 10) {

	case 0 :		/* interval/grey-table object */
		initrasterscan(obj,&iwsp,0);
		if (nextinterval(&iwsp) == 0) {
			dispstring(str,f->dx+f->scale*f->ix*(iwsp.colpos-f->ox),
				f->dy+f->scale*f->iy*(iwsp.linpos-f->oy)-VERTOFFS);
			update();
		}
		break;
	default :
		break;
	}
}


/*
 *	F R A M E O B J N N U M
 *
 * display number immediately below object
 *
 *	position  1 - low left
 *			  3 - low right
 *
 *	This routine displays numbers around an object.  Each is stored
 *	so that it can be undisplayed.  The scheme assumes only one box/number
 *	set displayed at a time and all of the previous are erased before
 *	the next set are drawn.
 *	Input of null obj will cause undisplay.
 *
 * Object protection also added. The check for obj existing which already
 * existed has been changed to use woolz_check_obj, standard woolz check
 */
frameobjnnum(num,obj,f,position)
int		num,position;
struct object *obj;
struct pframe *f;
{
	static	int	lastnum[12],lastx[12],lasty[12],
				havedisp[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
	int	objx,objy;
	
	/* n.b. there is no formal checking on pframe structure integrity */
	if (position > 12) return(0);
	if (havedisp[position-1]) {
		intens(0);
		dispnum(lastnum[position-1],lastx[position-1],lasty[position-1]);
		havedisp[position-1] = 0;
		intens(1);
	}
	/*
	 * this function should have an independent flag for display/undisplay
	 * rather than test the object pointer.  If this is implemented then
	 * full object checking will be done, and below (obj != NULL) can then
	 * be removed.
	 */
	if ((obj != NULL) && (woolz_check_obj(obj, "frameobjnnum") == 0)) {
		switch (obj->type) {
		
		case 1:
			if (obj->idom) {
				switch (position) {
				case 1:
				default:
					objx = obj->idom->kol1;
					objy = obj->idom->line1;
					break;
				case 3:
					objx = obj->idom->lastkl - 16;
					objy = obj->idom->line1 - 16;
					break;
				}
				lastnum[position-1] = num;
				lastx[position-1] = f->dx + f->scale * f->ix * (objx - f->ox);
				lasty[position-1] = f->dy + f->scale * f->iy * (objy - f->oy);
				dispnum(num, lastx[position-1], lasty[position-1]);
			}
			havedisp[position-1] = 1;
		default:
			;
		}
	}
}


