/*
 *   greyscan.c
 *   routines to traverse a grey-table object,
 *   in conjunction with "intdomscan.c"
 *
 * Modifications
 *
 *	22 Nov 1988		dcb		Added param to woolz_exit to say where called from
 *  18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	28 May 1987		BDP		Added NULL iwsp->gwsp test as check on vdom
 *	05 May 1987		BDP		protection against null or empty objs
 *	2  Mar 1987		GJP		Woolz_exit
 *	12 Sep 1986		CAS		Includes
 */

#include <stdio.h>
#include <wstruct.h>


/*
initialise interval and grey scanning of standard object in standard direction
This is the main public level initialisation routine , protection is supplied
by the routines it calls */

initgreyscan(obj,iwsp,gwsp)		/* raster and tranpl of 0 provided */
struct object	*obj;
struct iwspace	*iwsp;
struct gwspace	*gwsp;
{
	initgreyrasterscan(obj,iwsp,gwsp,0,0);
}


/*
as initgreyscan, but with choice of raster direction and transplanting
this is a subsidiaary entry point designed for use by seqpar . as of
5/5/87, this was the only external routine directly using this routine .
protection is supplied at a lower level . bdp 5/5/87 */

initgreyrasterscan(obj,iwsp,gwsp,raster,tranpl)
struct object	*obj;
struct iwspace	*iwsp;
struct gwspace	*gwsp;
{
	initrasterscan(obj,iwsp,raster);
	initgreywsp(obj,iwsp,gwsp,tranpl);
}


/*
 * attach grey workspace to interval scanning workspace
 * local function.
 * protection included here against both null objects and null domains , if
 * a problem is found then an error indication is returned. this can be used
 * as a success test if required. bdp 5/5/87 
 * The iwsp->gryptr is now initialised to NULL, so that the initialisation
 * routine here has a means of indicating to the grey scanning routine that
 * a problem was detected , and that it should return 1(not found) on its first
 * invocation. bdp 28/5/87
 */
initgreywsp(obj,iwsp,gwsp,tranpl)
struct object	*obj;
register struct iwspace	*iwsp;
register struct gwspace	*gwsp;
{
	register struct valuetable *vdmn;
	iwsp->gryptr = NULL;	/* default indicator of error, changed later if ok */

	if (woolz_check_obj(obj, "initgreywsp") != 0)
		return(1);
	if (obj->vdom != NULL) {
		/* set up gwsp */
		iwsp->gryptr = gwsp;
		gwsp->gtable = obj->vdom;
		vdmn = gwsp->gtable;
		switch (vdmn->type) {
		case 1:
		case 2:
		/* synchronise grey table line pointer to interval domain
			- pointer when incremented points to first line */
			gwsp->gline = vdmn->vtblines + iwsp->linpos - vdmn->line1;
			break;
		case 21:
		case 22:
			/* synchronise grey table line pointer to interval domain
				- pointer when incremented points to first line */
			gwsp->gline = (struct valueline **) (((struct ivtable *)vdmn)->vil + iwsp->linpos - vdmn->line1);
			break;
		default:
			break;
		}
		gwsp->intptr = iwsp;
		gwsp->tranpl = tranpl;
	}
	return(0);	/* no problems found */
}


/*
obtain next interval and its grey table.
if tranpl and gvio=0 and scan already started (check line number)
	then must collect previous grey interval
if tranpl and gvio=1 then must output this interval
	(unless scan finished).
No explicit extra checks have been put into this routine. If it is used
as intended, ie only after a next interval call then this will protect it.
the alternative is to use an indicator set by next interval which is then
checked by greyinterval before operation to ensure the call is legal. 
however , as these two routines are already highly coupled the introduction
of more in the form of a static flag has been avioded for the present
bdp 5/5/87 */

nextgreyinterval(iwsp)
register struct iwspace	*iwsp;
{
	register struct gwspace *gwsp = iwsp->gryptr;

	/* in the following test it should be noted that initraster scan sets
	linpos = (linbot - lineraster), so first time through this is never
	going to be true. This means that protection intorduced into that
	routine and nextinterval are going to cause this routine to return
	1 first time through if an error was detected, in the object or idom,
	if theres a problem with the vdom that must be detected and handled here
	bdp 5/5/87  */
	
	if (iwsp->gryptr == NULL)
		return(1);		/* cant do a grey scan if theres no greytable */
	if (gwsp->tranpl && gwsp->gvio == 0 && iwsp->linpos != (iwsp->linbot - iwsp->lineraster))
		greyinterval(iwsp);
	if (nextinterval(iwsp) != 0)
		return(1);
	if (!gwsp->tranpl || gwsp->gvio == 1 )
		greyinterval(iwsp);
	return(0);
}


/*
handle grey table for an interval.
This must follow a call to "nextinterval"
*/
greyinterval(iwsp)
register struct iwspace	*iwsp;
{
	register GREY *g;
	struct rectvaltb *vdmn;
	register struct gwspace *gwsp = iwsp->gryptr;
	register int i;
	struct valintline *vil;
	struct valueline *val;
	vdmn = (struct rectvaltb *) gwsp->gtable;

	switch (vdmn->type) {
	case 1:
	case 2:
		if (iwsp->nwlpos)				/* new line signal, move to */
			gwsp->gline += iwsp->nwlpos*iwsp->lineraster;	/* next line of greytable */
		/*
	 	* pointer to grey values
	 	*/
		g = (*(gwsp->gline))->values + iwsp->lftpos - (*(gwsp->gline))->vkol1;
		break;
	case 11:
	case 12:
		g = vdmn->values + (iwsp->linpos - vdmn->line1) * vdmn->width +
			iwsp->lftpos - vdmn->kol1;
		break;
	case 21:
	case 22:
		if (iwsp->nwlpos)				/* new line signal, move to */
			gwsp->gline = (struct valueline **) (((struct valintline *) gwsp->gline) + iwsp->nwlpos*iwsp->lineraster);	/* next line of greytable */
		/*
	 	* pointer to grey values
	 	*/
		vil = (struct valintline *) gwsp->gline;
		val = vil->vtbint;
		for (i=0; i<vil->nintvs; i++) {
			if (val->vkol1 <= iwsp->lftpos && val->vlastkl >= iwsp->rgtpos) {
				g = val->values + iwsp->lftpos - val->vkol1;
				goto G_OK;
			}
			val++;
		}
		fprintf(stderr,"GREY-TABLE type 21 does not cover domain\n");
		woolz_exit(10, "greyinterval");
G_OK:
		break;
	default:
		break;
	}
	if (gwsp->tranpl != 0)		/* transplant mode */
		switch (gwsp->gvio) {
		case 0 :		/* input to object */
			for (i=0; i<iwsp->colrmn; i++)
				*g++ = gwsp->grintptr[i];
			break;
		case 1 :		/* output from object */
			for (i=0; i<iwsp->colrmn; i++)
				gwsp->grintptr[i] = *g++;
			break;
		default :
			fprintf(stderr,"gvio %d not allowed\n",gwsp->gvio);
			break;
		}
	else
		gwsp->grintptr = g;	/* direct mode */
}
