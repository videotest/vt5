/*
 * writeobj.c		Jim Piper		August 1983
 *
 * Routines to convert object structures to sequential data streams.
 * When disc files are intended,
 * any necessary "fseek"-ing must be done elsewhere.
 *
 * The basic philosophy of the sequential form of an object is that
 * sub-objects are ordered in such a way that pointers are unnecessary.
 * See ../doc/filestructs.d for details.
 *************************************************************************
 * #ifdef OTV - for writing files produced by OS9 Woolz into VAX Woolz.
 * This is very restricted - only handles type 1 objects without plists.
 * A problem is that 16-bit words are byte-swapped between VAX and
 * MC68000 but anything written a byte at a time is in the correct order.
 * So we have to ensure that "putword()" not only writes 32 bits, but in
 * the correct byte order.
 *************************************************************************
 *
 * Modifications
 *
 *	28 Mar 1991		CAS		OTV support of vwriteobj
 *	7 Feb 1991		CAS		Added write stus checking to write compound object
 *							Mizar/OSK defines
 * 29-01-91		JimP@MRC	put in a writepropertlist() for polygons, to match the
 *							change in readobj.c.  ALL appropriate objects should have
 *							propertylist read/written.
 * 23-10-90		Jimp@MRC	put in writecompounda for compound objects
 *	 1 Jun 1989		CAS		Global def of errno for V2.2
 *	22 Feb 1989		CAS		Rest of mputc checking
 *	 1 Feb 1989		CAS		More status checking (full for type 1 + 110)
 *	30 Jan 1989		CAS		Start to do somthing about returning a status from writeobj
 *	22 Nov 1988		dcb		Added param to woolz_exit to say where called from
 *  2  Oct 1988		dcb		Add type 60
 *	25 Nov 1987		BDP		Protect write polygon against NULL domain
 *	20 Nov 1987		BDP		Write of type 20 done from wrong field !
 *	16 Nov 1987		BDP		Added type 12 object capability in same style as the rest
 *	2  Mar 1987		GJP		Woolz_exit
 *	17 Dec 1986		CAS		Moved mfwrite into gplib + renamed fbwrite
 *	03 Dec 1986		jimp	Use fast macro instead of putc()
 *	27 Nov 1986		jimp	Don't write type if incorrect, don't exit,
 *							instead print error and return.
 *	12 Sep 1986		CAS		Includes
 *************************************************************************
 */

#include <stdio.h>
//#include <fbio.h>
#include <wstruct.h>
#ifdef OTV
typedef	int	SGREY;
typedef	int	SCOORD;
typedef	int	SSMALL;
#else OTV
typedef	short	SGREY;
typedef	short	SCOORD;
typedef	short	SSMALL;
#endif OTV
#define mputc putc
#define fbwrite fwrite

extern int errno;

#ifdef OTV
/* "getw()" on Vax is 32 bits, least significant 16 first */
static putword(w,fp)
register unsigned w;
FILE *fp;
{
	if (mputc(w&255, fp) != -1) {
		w >>= 8;
		if (mputc(w&255, fp) != -1) {
			w >>= 8;
			if (mputc(w&255, fp) != -1) {
				w >>= 8;
				if (mputc(w&255, fp) != -1)
					return(0);
			}
		}
	}
	return(-1);
}
#else VTO
static putword(w,fp)
register unsigned w;
FILE *fp;
{
	if (mputc((w>>8)&255, fp) != -1)
		if (mputc(w&255, fp) != -1)
			return(0);
	return(-1);
}
#endif OTV


int packcheck = 1;


#ifdef OTV
vwriteobj(fp,obj)
#else OTV
writeobj(fp,obj)
#endif OTV
FILE *fp;
struct object *obj;
{
	int		stat = 1;
	if (obj == NULL) {
		fprintf(stderr,"Writeobj: NULL object ignored\n");
		errno = 0;
		return(1);
	}
	switch (obj->type) {

	case 1:
		if (mputc((char) obj->type, fp) != -1)
			if (writeintervaldomain(fp,obj->idom) != -1)
				if (writevaluetable(fp,obj) != -1)
					if (writepropertylist(fp,obj->plist) != -1)
						stat = 0;
		break;

	case 10:
		/* better to ignore this rather than write a NULL polygon cos readobj
		would at best return a NULL and terminate an input stream */
		
		if (obj->idom == NULL){
			fprintf(stderr,"Writeobj: NULL polygon ignored\n");
			return(stat);
		}
		if (mputc((char) obj->type, fp) != -1)
			if (writepolygon(fp,obj->idom) != -1)
				if (writepropertylist(fp,obj->plist) != -1)
					stat = 0;
		break;

	case 11:
		if (mputc((char) obj->type, fp) != -1) {
			writeboundlist(fp,obj->idom);
			stat = 0;
		}
		break;

	case 12:
		if (mputc((char) obj->type, fp) != -1) {
			writepolygon(fp,obj->idom);
			writeconvhull(fp,obj->vdom);
			stat = 0;
		}
		break;
		
	case 13:
		if (mputc((char) obj->type, fp) != -1) {
			writehisto(fp, obj->idom);
			stat = 0;
		}
		break;

	case 20:
		if (mputc((char) obj->type, fp) != -1) {
			writerect(fp,obj->idom);
			stat = 0;
		}
		break;

	case 30:
	case 31:
		if (mputc((char) obj->type, fp) != -1) {
			writevector(fp,obj);
			stat = 0;
		}
		break;

	case 40:
	case 41:
		if (mputc((char) obj->type, fp) != -1) {
			writepoint(fp,obj);
			stat = 0;
		}
		break;

	case 60:	/* frame */
		if (mputc((char) obj->type, fp) != -1) {
			writeframe(fp,obj);
			stat = 0;
		}
		break;

	case 70:
		if (mputc((char) obj->type, fp) != -1) {
			writetext(fp,obj);
			stat = 0;
		}
		break;

	case 80:
	case 81:
		if (mputc((char) obj->type, fp) != -1) {
			if (writecompounda(fp,obj) != -1)
				stat = 0;
		}
		break;

	case 110:
		if (mputc((char) obj->type, fp) != -1)
			if (writepropertylist(fp,obj->plist) != -1)
				stat = 0;
		break;

	default:
		fprintf(stderr,"Can't write object type %d\n",obj->type);
		errno = 0;
	}
	return(stat);
}


static
writeintervaldomain(fp, idom)
register FILE *fp;
struct intervaldomain *idom;
{
	register int i, j, nlines;
	register struct intervalline *ivln;
	register struct interval *jtvl;

	if (idom == NULL) {
		return(mputc(0,fp));
	}
	if (mputc((char) idom->type, fp) == -1) return(-1);
	if (putword(idom->line1, fp) == -1) return(-1);
	if (putword(idom->lastln, fp) == -1) return(-1);
	if (putword(idom->kol1, fp) == -1) return(-1);
	if (putword(idom->lastkl, fp) == -1) return(-1);

	switch (idom->type) {

	case 1:
		nlines = idom->lastln - idom->line1;
		for (i=0; i<=nlines; i++)
			if (putword(idom->intvlines[i].nintvs, fp) == -1)
				return(-1);
		ivln = idom->intvlines;
		for (i=0; i<=nlines; i++) {
#ifdef OTV
			jtvl = ivln->intvs;
			for (j=0; j<ivln->nintvs; j++,jtvl++) {
				putword(jtvl->ileft,fp);
				putword(jtvl->iright,fp);
			}
#else OTV
			/* much faster in OS9-68000 unfortunately */
			if (fbwrite(ivln->intvs, sizeof(struct interval), ivln->nintvs, fp) != ivln->nintvs)
				return(-1);
#endif OTV
			ivln++;
		}
		break;

	case 2:
		break;

	default:
		fprintf(stderr,"Can't write domain type %d\n",idom->type);
		woolz_exit(51, "writeintervaldomain");
		return(-1);
	}
	return(0);
}



static
writepropertylist(fp, plist)
register FILE *fp;
struct propertylist *plist;
{
	register char *pl;
	register int i, si;
#ifdef VTO
	static int warn=1;
#endif VTO
	if (plist == NULL) {
		return(mputc((char) 0, fp));
	} else {
#ifdef VTO
		if (warn) {
			warn = 0;
			fprintf(stderr,"Warning - propertylist may not be converted\n");
		}
#endif VTO
		if (mputc((char) 1, fp) == -1)
			return(-1);
		if (putword(plist->size, fp) == -1)
			return(-1);
		pl = (char *) plist;
		pl += sizeof(SMALL);
		si = plist->size - sizeof(SSMALL);
		if (fbwrite(pl,1,si,fp) != si)
			return(-1);
	}
}



static
writevaluetable(fp, obj)
register FILE *fp;
struct object *obj;
{
	struct iwspace iwsp;
	struct gwspace gwsp;
	int packing, type;
	register int i, min, max;
	register GREY *g, *gtop;
	short sht;

	if (obj->vdom == NULL) {
		return(mputc(0,fp));
	}

	type = obj->vdom->type;
	if (mputc((char) (type%10), fp) == -1)
		return(-1);
	switch (type) {
		case 1:
		case 11:
		case 21:
			if (packcheck != 0) {
				min = 0;
				max = 255;
				initgreyscan(obj,&iwsp,&gwsp);
				while (nextgreyinterval(&iwsp) == 0) {
					g = gwsp.grintptr;
					for (i=0; i<iwsp.colrmn; i++) {
						if (*g < min)
							min = *g;
						else if (*g > max)
							max = *g;
						g++;
					}
				}
				if (min == 0 && max == 255)
					packing = 3;
#ifndef OSK
				else if (min > -32768 && max < 32768)
					packing = 2;
#endif OSK
				else
					packing = 1;
			} else
				packing = 3;
			if (mputc((char) packing, fp) == -1) return(-1);
			if (putword(obj->vdom->bckgrnd, fp) == -1) return(-1);
			initgreyscan(obj,&iwsp,&gwsp);
			while(nextgreyinterval(&iwsp) == 0)
				switch (packing) {
#ifndef OTV
					case 1:
						if (fbwrite(gwsp.grintptr, sizeof(GREY), iwsp.colrmn, fp) != iwsp.colrmn)
							return(-1);
						break;
#ifndef OSK
					case 2:
						g = gwsp.grintptr;
						for (i=0; i<iwsp.colrmn; i++) {
							sht = (short) *g;
							if (fbwrite(&sht, sizeof(short), 1, fp) != 1)
								return(-1);
							g++;
						}
						break;
#endif OSK
#endif OTV
					case 3:
						/*
						 * byte packing.
						 */
						g = gwsp.grintptr;
						gtop = g + iwsp.colrmn;
						while (g < gtop)
							if (mputc(*g++, fp) == -1)
								return(-1);
						break;

					default:
						fprintf(stderr,"Can't write packing type %d\n",packing);
						woolz_exit(53, "writevaluetable.1");
						return(-1);
					}
				break;
		case 2:
		case 12:
		case 22:
			fprintf(stderr,"Can't write float grey tables\n");
			woolz_exit(53, "writevaluetable.2");
			return(-1);
		default:
			fprintf(stderr,"Unknown grey table type %d\n",type);
			woolz_exit(53, "writevaluetable.3");
			return(-1);
	}
}

/*
 *	W R I T E P O L Y G O N  --
 *
 */
static
writepolygon(fp, poly)
FILE *fp;
struct polygondomain *poly;
{
	register int nvertices;

	nvertices = poly->nvertices;
	if (mputc((char) poly->type, fp) == -1) return(-1);
	if (putword(nvertices, fp) == -1) return(-1);
	switch (poly->type) {

	case 1:
		if (fbwrite(poly->vtx, sizeof(struct ivertex), nvertices, fp) != nvertices)
			return(-1);
		break;

	case 2:
		if (fbwrite(poly->vtx, sizeof(struct fvertex), nvertices, fp) != nvertices)
			return(-1);
		break;
	
	default:
		fprintf(stderr,"Illegal polygon type %d\n",poly->type);
		woolz_exit(51, "writepolygon");
	}
	return(0);
}


/*
 *	W R I T E B O U N D L I S T  --
 *
 */
static
writeboundlist (fp,blist)
FILE *fp;
struct boundlist *blist;
{
	if (blist == NULL) {
		return(mputc(0,fp));
	} else {
		if (mputc(1,fp) == -1) return(-1);
		if (mputc((char)blist->type,fp) == -1) return(-1);
		if (writeboundlist(fp,blist->next) == -1) return(-1);
		if (writeboundlist(fp,blist->down) == -1) return(-1);
		if (putword(blist->wrap,fp) == -1) return(-1);
		if (writepolygon(fp,blist->poly) == -1) return(-1);
	}
	return(0);
}

/*
 *	W R I T E C O N V H U L L  --
 *
 */
static writeconvhull(fp, cdom)
register FILE *fp;
struct cvhdom *cdom;
{
	if ( cdom == NULL ) {
		return(mputc((char) 0, fp));
	} else {
#ifdef CVTEST
		fprintf(stderr,"writeobj: %d chords being written \n",cdom->nchords);
#endif CVTEST
		if (mputc((char) cdom->type ,fp) == -1) return(-1);
		if (putword(cdom->nchords, fp) == -1) return(-1);
		if (putword(cdom->nsigchords, fp) == -1) return(-1);
		if (putword(cdom->mdlin, fp) == -1) return(-1);
		if (putword(cdom->mdkol, fp) == -1) return(-1);
		if (fbwrite(cdom->ch, sizeof(struct chord), cdom->nchords, fp) != cdom->nchords) return(-1);
	}
	return(0);	
}

/*
 *	W R I T E R E C T  --
 *
 */
static
writerect(fp, rdom)
FILE *fp;
struct irect *rdom;
{
	if (rdom == NULL) {
		return(mputc(0,fp));
	} else {
		if (mputc(rdom->type,fp) == -1) return(-1);
		switch (rdom->type) {
			case 1:
				if (fbwrite(&(rdom->irk[0]), sizeof(struct irect) - sizeof(SMALL), 1, fp) != 1) return(-1);
				break;
			case 2:
				if (fbwrite(&(rdom->irk[0]), sizeof(struct frect) - sizeof(SMALL), 1, fp) != 1) return(-1);
				break;
		}
	}
	return(0);
}
			
/*
 *	W R I T E V E C T O R  --
 *
 */
static writevector(fp, vec)
struct ivector *vec;
{
	switch (vec->type) {
		case 30:
			if (fbwrite(&vec->style, sizeof(struct ivector) - sizeof(SMALL), 1, fp) != 1) return(-1);
			break;
		case 31:
			if (fbwrite(&vec->style, sizeof(struct fvector) - sizeof(SMALL), 1, fp) != 1) return(-1);
			break;
	}
	return(0);
}
			
/*
 *	W R I T E P O I N T  --
 *
 */
static writepoint(fp, pnt)
struct ipoint *pnt;
{
	switch (pnt->type) {
		case 40:
			if (fbwrite(&pnt->style, sizeof(struct ipoint) - sizeof(SMALL), 1, fp) != 1) return(-1);
			break;
		case 41:
			if (fbwrite(&pnt->style, sizeof(struct fpoint) - sizeof(SMALL), 1, fp) != 1) return(-1);
			break;
	}
	return(0);
}

/*
 *	W R I T E H I S T O  --
 *
 */
static writehisto(fp, h)
FILE *fp;
struct histogramdomain *h;
{
	if (mputc(h->type, fp) == -1) return(-1);
	if (fbwrite(&h->r, sizeof(struct histogramdomain) - sizeof(SMALL),1,fp) != 1) return(-1);
	if (fbwrite(h->hv, (h->type == 1? sizeof(int): sizeof(float)), h->npoints, fp) != h->npoints) return(-1);
	return(0);
}

/*
 *	W R I T E F R A M E  --
 *
 */
static writeframe(fp, frame)
FILE *fp;
struct pframe *frame;
{
	if (fbwrite(&(frame->scale), sizeof(struct pframe) - sizeof(SMALL), 1, fp) != 1) return(-1);
	return(0);
}

/*
 *	W R I T E T E X T  --
 *
 */
static writetext(fp,tobj)
FILE *fp;
struct text *tobj;
{
	struct textdomain *tdom;
	char *tstring,c;
	int i;

	if (fbwrite(tobj->tdom,sizeof(struct textdomain),1,fp) != 1) return (-1);
	tstring = tobj->text;
	tdom = tobj->tdom;
	for(i = 0;i <= tdom->stringlen; i++) {
		c = tstring[i];
		if (mputc(c,fp) == -1)
			return(-1);
	}
	return(0);
}

static writecompounda(fp,c)
FILE *fp;
struct compounda *c;
{
	int i;
	if (mputc(c->otype, fp) == -1)
		return(-1);
	if (putword(c->n, fp) == -1)
		return(-1);
	for (i=0; i<c->n; i++) {
		if (c->o[i] == NULL) {
			if (mputc(0, fp) == -1)
				return(-1);
		} else {
#ifdef OTV
			if (vwriteobj(fp,c->o[i]) != 0)
#else
			if (writeobj(fp,c->o[i]) != 0)
#endif
				return(-1);
		}
	}
	return(writepropertylist(fp,c->p));
}
	

