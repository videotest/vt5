/*
 * freespace.c		Jim Piper	August 1983.
 * 
 * Free space from structured objects.
 * If space was not allocated by "malloc" system then you are in trouble !
 *
 * Modifications
 *
 *  23 May 1991		ih			Null checks in freeboundlist
 *	 7 Feb 1991		CAS		voids
 *	29 Jan 1991	jimp@MRC	Free polygon's plist
 *	18 Jan 1989		CAS			Added free stuff for text object (type 70)
 *	 5 Oct 1988		CAS			Freeframe..
 *	20 Jan 1988		CAS			fixed boobytraps in freehisto, freepoly
 *								 + freedomain.  Also collected + massaged below comment
 * 								20/11/87 bdp, proper routine to free rectdomain
 *	20 Nov 1987		BDP			Free rdom must use freedomain, no free ptr !
 *	13 Nov 1987		BDP			Frees from domains must check whether associated 
 *								arrays, eg histo points are in their own space 
 *								or part of same space before freeing
 *	 7 Jan 1987		CAS			free for type 110 objects
 *	13 Nov 1986		jimp		freeobj(NULL) now does nothing !!
 *	17 Oct 1986		CAS			fflush(debug_channel)
 *	15 Oct 1986		CAS			Un static freevaluetable
 *	13 Oct 1986		CAS			Changed some free's to freexxxx's
 *	 6 Oct 1986		CAS			Test linkcount in intvdomain and valuetb
 *								before freeing - decrement count - but only
 *								free if <= 0
 *								Added debug facility
 *  24 Sep 1986		GJP			Big Free not little free
 *	12 Sep 1986		CAS			Includes
 */

#include <stdio.h>
#include <wstruct.h>

static FILE *debug_channel;

/*
 *	F R E E O B J _ D E B U G
 *
 */
freeobj_debug(chan)
FILE *chan;
{
	debug_channel = chan;
}

/*
 *	F R E E O B J
 *
 */
freeobj (obj)
struct object *obj;
{
	if (obj == NULL)
		return(0);
	switch (obj->type) {
	case 1:
		if (debug_channel) {
			fprintf(debug_channel,"Freeobj: obj 0x%x type 1, intv 0x%x, lnk %d, valt 0x%x, lnk %d plist 0x%x\n",
					obj, obj->idom, (obj->idom?obj->idom->linkcount: 0),
					obj->vdom, (obj->vdom?obj->vdom->linkcount: 0),
					obj->plist);
			fflush(debug_channel);
		}
		freedomain(obj->idom);
		freevaluetable(obj->vdom);
 		freeplist(obj->plist);
		break;
	case 10:
		if (debug_channel) {
			fprintf(debug_channel,"Freeobj: obj 0x%x type %d, poly 0x%x\n",
				obj, obj->type,obj->idom);
			fflush(debug_channel);
		}
		freepolydmn(obj->idom);
 		freeplist(obj->plist);
		break;
	case 11:
		if (debug_channel) {
			fprintf(debug_channel,"Freeobj: obj 0x%x type %d, bound 0x%x\n",
			 obj, obj->type,obj->idom);
			fflush(debug_channel);
		}
		freeboundlist(obj->idom);
		break;
	case 12:
		if (debug_channel) {
			fprintf(debug_channel,"Freeobj: obj 0x%x type %d, convh 0x%x, chord 0x%x\n",
			 obj, obj->type,obj->idom,  obj->vdom);
			fflush(debug_channel);
		}
		freepolydmn(obj->idom);
		freeconvhull(obj->vdom);
		break;
	case 13:
		if (debug_channel) {
			fprintf(debug_channel,"Freeobj: obj 0x%x type %d, histo 0x%x\n",
					obj, obj->type,obj->idom);
			fflush(debug_channel);
		}
		freehistodmn(obj->idom);
		break;
	case 20:
		if (debug_channel) {
			fprintf(debug_channel,"Freeobj: obj 0x%x type %d, rect 0x%x\n",
					obj, obj->type,obj->idom);
			fflush(debug_channel);
		}
		freerectdmn(obj->idom);
		break;
	case 40:
		if (debug_channel) {
			fprintf(debug_channel,"Freeobj: obj 0x%x type %d\n",obj, obj->type);
			fflush(debug_channel);
		}
		break;
	case 60:
		if (debug_channel) {
			fprintf(debug_channel,"Freeobj: obj 0x%x type %d\n",obj, obj->type);
			fflush(debug_channel);
		}
		break;
	case 70:
		if (debug_channel) {
			fprintf(debug_channel,"Freeobj: obj 0x%x type %d tdom 0x%x text 0x%x\n",obj, obj->type,obj->idom,obj->vdom);
			fflush(debug_channel);
		}
		freetextdomain(((struct text *)obj)->tdom);
		if (((struct text *)obj)->text)
			Free(((struct text *)obj)->text);
		break;
	case 110:
		if (debug_channel) {
			fprintf(debug_channel,"Freeobj: obj 0x%x type %d\n",obj, obj->type);
			fflush(debug_channel);
		}
		freeplist(obj->plist);
		break;
	default :
		if (debug_channel) {
			fprintf(debug_channel,"Freeobj: obj 0x%x type %d, can't free\n",
				obj, obj->type);
			fflush(debug_channel);
		}
		break;
	}
	Free(obj);
}


/*
 *	F R E E D O M A I N  --
 *
 */
freedomain (idom)
struct intervaldomain *idom;
{
	if (idom != NULL) {
		if ((--(idom->linkcount)) <= 0) {
			switch (idom->type) {
				case (1) :
					if (idom->freeptr != NULL)
						Free(idom->freeptr);
					break;
				default :
					break;
			}
			Free(idom);
		}
	}
}

/*
 *	F R E E V A L U E T A B L E  --
 *
 */
freevaluetable( vdom)
struct intervaldomain *vdom;
{
	if (vdom != NULL) {
		if ((--(vdom->linkcount)) <= 0) {
			if (vdom->freeptr != NULL)
				Free(vdom->freeptr);
			Free(vdom);
		}
	}
}


/*
 *	F R E E P L I S T  --
 *
 */
freeplist (plist)
struct propertylist *plist;
{
	if (plist != NULL)
		Free(plist);
}

/*
 *	F R E E H I S T O D M N  --
 *
 */
freehistodmn (h)
struct histogramdomain *h;
{
	if (h != NULL) {
		Free(h);
	}
}

/*
 *	F R E E C O N V H U L L  --
 *
 */
freeconvhull(c)
struct cvhdom *c;
{
	if (c != NULL)
		Free(c);
}

/*
 *	F R E E R E C T D M N  --
 *
 */
freerectdmn(r)
struct irect *r;
{
	if (r != NULL)
		Free(r);		/* should be ok for type 1 or 2 rectangles */
}

/*
 *	F R E E P O L Y D M N  --
 *
 */
freepolydmn(poly)
struct polygondomain *poly;
{
	if (poly != NULL)
		Free(poly);
}


/*
 *  F R E E B O U N D L I S T  --
 *
 */
freeboundlist(b)
struct boundlist *b;
{
	if (b) {
		if (b->next) {
			freeboundlist(b->next);
		}
		if (b->down) {
			freeboundlist(b->down);
		}
		if (b->poly) freepolydmn(b->poly);
		Free(b);
	}
}

/*
 *	F R E E T E X T D O M A I N  --  Free text domain
 *
 */
freetextdomain(tdom)
struct textdomain *tdom;
{
	if (tdom != NULL) {
		Free(tdom);
	}
}
