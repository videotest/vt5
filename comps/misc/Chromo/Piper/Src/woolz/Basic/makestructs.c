/*
 *
 *	M A K E S T R U C T S . C --
 *
 *           Image Recognition Systems
 *           720 Birchwood Boulevard
 *           Birchwood Science Park
 *           Warrington
 *           Cheshire
 *           WA3 7PX
 *
 *	Written: J. Piper
 *           Medical Research Council
 *           Clinical and Population Cytogenetics Unit
 *           Edinburgh.
 *
 *	Copyright (c) and intellectual property rights Image Recognition Systems
 *
 *  Date:    July 1983.
 *
 *	Purpose: Allocate space and make structures.
 *
 *	Description:
 *
 *	Modifications:
 *
 *	31 Jan 1991		JimP		Initialised debug_channel to NULL
 *	05 Dec 1990		JimP		makepframe() sets ix=iy=1.
 *	30 Nov 1990		JimP		Added makeirect()
 *	30 Jun 1989		CAS			Wrong type of type -> domain 1/11/21 should
 *								have been obj type 1
 *	17 Jan 1989		CAS			Only incr vdom linkcount if type 1,11,21
 *	28 Nov 1988		dcb		set invlines to NULL for type 2 domain (previously
 *							unset)
 *  18 Nov 1988		dcb		woolz_check_obj() instead of wzcheckobj()
 *	16 Jun 1987		CAS			Only incr linkcount in makemain for interval
 *								struct objects (1 11 21)
 *	15 May 1987		BDP	    	protected greys against empty objs to stop zero callocs
 *	05 May 1987		BDP		    protection against null or empty objs
 *	10 Mar 1987		CAS			Fix makerectvaltb 
 *	 9 Mar 1987		CAS			Fix makerectvaltb 
 *	 6 Mar 1987		GJP			Fix makerectvaltb 
 *	 5 mar 1987		CAS/GJP		Reorganise type 11 grey tables
 *	10 Dec 1986		CAS			New routine makeplist
 *	15 Oct 1986		CAS			Linkcount into newidomain
 *	 9 Oct 1986		CAS			Looking for lost space - added debug facility
 *	 6 Oct 1986		CAS (+GJP)	Added linkcount to idom and vtb structs
 *	12 Sep 1986		CAS         Combined this with vax long name version
 *							    and fixed includes
 *	21 Aug 1986		CAS+        Removed init of origonal field in newgrey
 *					JIMP		because of type 21 grey tables.
 *	20 Aug 1986     JP	    	Changes to "newgrey" and "newvaluetb" to cope with
 *							    object with input objects with rectangular
 *						    	grey-tables.
 */

#include <stdio.h>
#include <wstruct.h>

static FILE *debug_channel = NULL;

/*
 *	M A K E S T R U C T _ D E B U G
 *
 */
Makestruct_debug(chan)
FILE	*chan;
{
	debug_channel = chan;
}

/*
 *	M A K E D O M A I N  --  Make interval domain
 *
 */
struct intervaldomain *makedomain(type,l1,ll,k1,kl)
{
	struct intervaldomain *idom;
	int nlines, intervallinespace;

#ifdef WTRACE
	fprintf(stderr,"makedomain called, type %d, l1 %d, ll %d, k1 %d, kl %d\n",type,l1,ll,k1,kl);
#endif WTRACE

	nlines = ll - l1 + 1;
	
	/* following check protects the calloc when either an empty domain is
	being made, or a spurious request is received */
	
	if (nlines > 0)
		intervallinespace = nlines * sizeof(struct intervalline);
	else
		intervallinespace = 0;
	
	switch(type) {
	case 1:
		idom = (struct intervaldomain *)
				 Calloc(sizeof(struct intervaldomain) + intervallinespace ,1 );
		break;
	default:
	case 2:
		idom = (struct intervaldomain *) Malloc(sizeof(struct intervaldomain));
		break;
	}
	idom->type = type;
	idom->line1 = l1;
	idom->lastln = ll;
	idom->kol1 = k1;
	idom->lastkl = kl;
	idom->freeptr = NULL;
	idom->linkcount = 0;
	switch (type) {
	case 1:
		idom->intvlines = (struct intervalline *) (idom + 1);
		break;
	default:
	case 2:
		idom->intvlines = NULL;	/* Was previously unset */
		break;
	}
	return(idom);
}


/*
 *	M A K E M A I N  --  Make an object
 *
 */
struct object *makemain(type,domain,values,plist,assoc)
int type;
struct intervaldomain *domain;
struct valuetable *values;
struct propertylist *plist;
struct object *assoc;
{
	struct object *obj;
#ifdef WTRACE
	fprintf(stderr,"makemain called \n");
#endif WTRACE
	obj = (struct object *) Malloc(sizeof(struct object));
	obj->type = type;
	obj->idom = domain;
	if (domain && (type == 1))
		domain->linkcount++;
	obj->vdom = values;
	if (values && (type == 1))
		values->linkcount++;
	obj->plist = plist;
	obj->assoc = assoc;
	if (debug_channel) {
		switch (obj->type) {
		case 1:
		fprintf(debug_channel,"Makestructs - Obj 0x%x type %d idom 0x%x dl %d val 0x%x vl %d plist 0x%x ass 0x%x\n",
			obj, obj->type, obj->idom, (obj->idom ? obj->idom->linkcount: 0),
			obj->vdom, (obj->vdom ? obj->vdom->linkcount: 0),obj->plist,
			obj->assoc);
			break;
		case 10:
		fprintf(debug_channel,"Makestructs - Obj 0x%x type %d pdom 0x%x ass 0x%x\n",
			obj, obj->type, obj->idom, obj->assoc);
		}
	}
	return(obj);
}


/*
 *	M A K E V A L U E T B  --  Make type 1 or 2 value / grey table
 *
 */
struct valuetable *
makevaluetb(type,l1,ll,bckgrnd,orig)
struct object *orig;
{
	register int nlines,i;
	struct valuetable *vtb;
	struct valueline *vln;
	int valuelinespace;

	nlines = ll - l1 + 1;
	
	/* following check protects the calloc when either an empty domain is
	being made, or a spurious request is received */
	
	if (nlines > 0)
		valuelinespace = nlines * (sizeof(struct valueline *) + 
									sizeof(struct valueline) );
	else
		valuelinespace = 0;
	
	vtb = (struct valuetable *)
				 Calloc(sizeof(struct valuetable) + valuelinespace, 1);
	vtb->type = type;
	vtb->original = orig;
	vtb->line1 = l1;
	vtb->lastln = ll;
	vtb->bckgrnd = bckgrnd;
	vtb->freeptr = NULL;
	vtb->linkcount = 0;
	vtb->vtblines = (struct valueline **) (vtb + 1);

	vln = (struct valueline *) (vtb->vtblines + nlines);

	for (i=0; i<nlines; i++)
		vtb->vtblines[i] = vln++;
	return(vtb);
}

/*
 *	M A K E R E C T V A L T B  --  Make type 11 or 12 value/grey table
 *
 */
struct rectvaltb *
makerectvaltb(type, line1, lastln, kol1, width, bckgrnd, values)
int type, line1, lastln, kol1, width;
GREY *values, bckgrnd;
{
	register struct rectvaltb *vtb;

	vtb = (struct rectvaltb *) Malloc(sizeof(struct rectvaltb));
	vtb->type = type;
	vtb->freeptr = NULL;
	vtb->linkcount = 0;
	vtb->line1 = line1;
	vtb->lastln = lastln;
	vtb->kol1 = kol1;
	vtb->width = width;
	vtb->bckgrnd = bckgrnd;
	vtb->values = values;
	return(vtb);
}


makeinterval(line,domain,nints,intptr)
struct intervaldomain *domain;
struct interval *intptr;
{
	struct intervalline *ivln;
	
	/* protection added here to check the existence of the supplied
	domain before its used. Also a good point to check that this isnt
	an empty domain .  bdp 5/5/87 */
	
	if ( domain != NULL )
		if (line >= domain->line1)
		{
			ivln = domain->intvlines + line - domain->line1;
			ivln->nintvs = nints;
			ivln->intvs = intptr;
		}
}


makevalueline(vtb,line,k1,kl,greyptr)
struct valuetable *vtb;
GREY *greyptr;
{
	struct valueline *vlln ;
	
	/* protection added just as above . bdp 5/5/87  */
	
	if ( vtb != NULL )
		if (line >= vtb->line1)
		{
			vlln = vtb->vtblines[line-vtb->line1];
			vlln->vkol1 = k1;
			vlln->vlastkl = kl;
			vlln->values = greyptr;
		}
}



/*
 * Make a type 1 (integer) polygon domain from an array of vertices.
 * If "copy" non-zero then allocate space and make a copy of the vertices
 * else just plant a pointer to original data.
 * Only protection needed here are addition of default statements bdp 5/5/87 
 */
struct polygondomain *makepolydmn (type, vertices, n, maxv, copy)
COORD *vertices;
{
	register struct polygondomain *p;
	register COORD *v;
	register int i;
	register float *f, *fvertices;
	int vertexsize;

	if (copy == 0)
		vertexsize = 0;
	else
		switch (type) {
		case 1:
			vertexsize = sizeof(struct ivertex);
			break;
		case 2:
			vertexsize = sizeof(struct fvertex);
			break;
		default:
			break;
		}

	p = (struct polygondomain *) Calloc(sizeof(struct polygondomain) + maxv * vertexsize, 1);
	p->type = type;
	p->nvertices = n;
	p->maxvertices = maxv;
	if (copy == 0) {
		p->vtx = (struct ivertex *) vertices;
		return(p);
	} else {
		p->vtx = (struct ivertex *) (p + 1);
		switch (type) {
		case 1:
			v = (COORD *) p->vtx;
			for (i = 0; i < 2*n; i++) {
				*v++ = *vertices++;
			}
			break;

		case 2:
			f = (float *) p->vtx;
			fvertices = (float *) vertices;
			for (i = 0; i < 2*n; i++) {
				*f++ = *fvertices++;
			}
			break;
		default:
			break;
		}

		return(p);
	}
}




struct object * makerect(line1, lastln, kol1, lastkl, values, bckgrnd, prop, assoc_obj)
int line1, lastln, kol1, lastkl;
GREY *values, bckgrnd;
struct object *assoc_obj;
struct propertylist *prop;

/* construct a rectangular object */

{
	struct intervaldomain *idom, *makedomain();
	register struct rectvaltb *vtb;
	struct object *obj,*makemain();

	idom = makedomain(2,line1,lastln,kol1,lastkl);
	vtb = makerectvaltb(11,line1, lastln, kol1, lastkl - kol1 + 1, bckgrnd, values);
	obj = makemain(1,idom,vtb,prop,assoc_obj);
	vtb->original = obj;
	return(obj);
}

/*
 * make an empty histogram structure
 */
struct histogramdomain *makehistodmn(type, n)
{
	struct histogramdomain *h;

	h = (struct histogramdomain *) Calloc (sizeof(struct histogramdomain) + n * (type == 1? sizeof(int): sizeof(float)), 1);
	h->type = type;
	h->r= 1;		/* default - display horizontally */
	h->npoints = n;
	h->hv = (int *) (h+1);
	return(h);
}

/*
 * make a new object with same domain as input object,
 * but with a new grey-table of same size as domain,
 * with values COPIED from original object.
 * Protection added to cope with null object. In this case a null is
 * returned. If a null value domain is found in the supplied object
 * then a null object is still returned. If supplied object is empty
 * then protection in the scanning routines should ensure that effect
 * is just to create and return an empty object here. bdp 5/5/87
 */
struct object *
newgrey(iobj)
struct object * iobj;
{
	struct object *jobj, *makemain();
	struct valuetable *v, *newvaluetb();
	register GREY *g1,*g2;
	register int i;
	struct iwspace iwsp1,iwsp2;
	struct gwspace gwsp1,gwsp2;

	/*
	 * just check the interval domain
	 */
	if ((iobj == NULL) || (woolz_check_idom(iobj->idom, "newgrey.1") != 0))
		return(NULL);
		
	if (iobj->vdom != NULL) {
		if (woolz_check_vdom(iobj, "newgrey.2") != 0)
			return(NULL);
		v = newvaluetb(iobj,iobj->vdom->type,iobj->vdom->bckgrnd);
	} else
		v = NULL;
		
	jobj = makemain(iobj->type,iobj->idom,v,iobj->plist,iobj);

	if (v != NULL) {
		initgreyscan(iobj,&iwsp1,&gwsp1);
		initgreyscan(jobj,&iwsp2,&gwsp2);
		while (nextgreyinterval(&iwsp1) == 0) {
			nextgreyinterval(&iwsp2);
			g1 = gwsp1.grintptr;
			g2 = gwsp2.grintptr;
			for (i=iwsp1.lftpos; i<=iwsp1.rgtpos; i++)
				*g2++ = *g1++;
		}
	}
	return(jobj);
}



/*
 * make a valuetable for object obj, sized by its interval domain
 */
struct valuetable *
newvaluetb(obj, type, bckgrnd)
struct object * obj;
{
	register struct valuetable *v;
	struct valuetable *makevaluetb(), *makeivtable();
	struct iwspace iwsp;
	register GREY *greyptr;
	struct valueline *vlln,*w;
	register int k1, nvtlinearea;
	
	/* 
	 * protection added as for newgrey, except no vdom check necessary
	 * bdp 5/5/87 
	 * since object not finished yet, check whether idom is OK
	 */
	if ((obj == NULL) || woolz_check_idom(obj->idom, "newvaluetb") != 0)
		return(NULL);
		
	switch (type) {
	default:
		type = 1;		/* in case it was a rectangle (=11) originally */
	case 1:
		v = makevaluetb(type,obj->idom->line1,obj->idom->lastln,bckgrnd,obj);
		if ((nvtlinearea = linearea(obj)) > 0)
		{
			greyptr = (GREY *) Calloc(linearea(obj), sizeof(GREY));
			v->freeptr = (char *) greyptr;
			initrasterscan(obj,&iwsp,0);
			while (nextinterval(&iwsp) == 0) {
				if (iwsp.nwlpos)
					k1 = iwsp.lftpos;
				if (iwsp.intrmn == 0) {
					makevalueline(v,iwsp.linpos,k1,iwsp.rgtpos,greyptr);
					greyptr += iwsp.rgtpos - k1 +1;
				}
			}
		}
		break;
	case 21:
		v = makeivtable(type,obj,bckgrnd);
		break;
	}
	return(v);
}



/*
 *	N E W I D O M A I N  --  copy an interval domain
 * Protection added to check the supplied domain for existence and for
 * it being empty, inthe former case a null is returned , inthe latter an
 * empty domain- as supplied.  bdp 6/5/87.
 * Also added check for empty object to prevent attemmpt to malloc zero
 * intervals space, as this causes malloc to fall over. bdp 15/5/87 
 */
struct intervaldomain *
newidomain(idom)
struct intervaldomain *idom;
{
	struct intervaldomain *jdom, *makedomain();
	register struct interval *itvl, *jtvl, *ktvl;
	struct intervalline *ivln;
	register l,n;

	if (woolz_check_idom(idom, "newidomain") != 0)
		return(NULL);
		
	jdom = makedomain(idom->type,idom->line1,idom->lastln,idom->kol1,idom->lastkl);
	if (idom->type == 1) {
		ivln = idom->intvlines;
		n = 0;
		for (l=idom->line1; l<=idom->lastln; l++) {
			n += ivln->nintvs;
			ivln++;
		}
		if ( n > 0 )	/* prevent zero malloc request */
		{
			jtvl = itvl = (struct interval *) Malloc(n*sizeof(struct interval));
			jdom->freeptr = (char *) itvl;
			jdom->linkcount = 0;
			ivln = idom->intvlines;
			for (l=idom->line1; l<=idom->lastln; l++) {
				ktvl = ivln->intvs;
				for (n=0; n<ivln->nintvs; n++) {
					jtvl->ileft = ktvl->ileft;
					jtvl->iright = ktvl->iright;
					jtvl++;
					ktvl++;
				}
				makeinterval(l,jdom,ivln->nintvs,itvl);
				itvl = jtvl;
				ivln++;
			}
		}
	}
	return(jdom);
}

/*
 *	M A K E P L I S T  --  Get space for a plist
 *
 */
struct propertylist *makeplist(size)
int		size;
{
	register struct propertylist *pl = (struct propertylist *) Calloc(1,size);
	pl->size = size;
	return(pl);
}


struct pframe *makepframe (scale,dx,dy,ox,oy)
{
	register struct pframe *f = (struct pframe *) Calloc(sizeof(struct pframe),1);
	f->type = 60;
	f->scale = scale;
	f->dx = dx;
	f->dy = dy;
	f->ox = ox;
	f->oy = oy;
	f->ix = f->iy = 1;
	return(f);
};

struct text *maketext(string,l,k,stringlen)
char *string;
{
	struct textdomain *tdom, *maketdomain();
	struct text *textobj;
	
	tdom = maketdomain(l,k,stringlen);
	textobj = (struct text *) makemain(70,tdom,string,NULL,NULL);
	return(textobj);
}

struct textdomain *maketdomain(l,k,stringlen)
{
	struct textdomain *tdom;
	tdom = (struct textdomain *) Calloc(sizeof(struct textdomain),1);
	tdom->type = 70;
	tdom->l = l;
	tdom->k = k;
	tdom->orientation = 0;
	tdom->spacing = 0;
	tdom->stringdir = 0;
	tdom->boldness = 0;
	tdom->font = 0;
	tdom->stringlen = stringlen;
	return(tdom);
}



struct object *makeirect()
{
	struct object *makemain();
	struct irect *r;
	r = (struct irect *) Calloc(1,sizeof(struct irect));
	r->type = 1;
	r->rangle = 0.0;
	return(makemain(20,r,NULL,NULL,NULL));
}


