/*
 * readobj.c		Jim Piper		August 1983
 *
 * Routines to input object structures from sequential data streams.
 * When disc files are intended,
 * any necessary "fseek"-ing must be done elsewhere.
 *
 * The basic philosophy of the sequential form of an object is that
 * sub-objects are ordered in such a way that pointers are unnecessary.
 * See ../doc/filestructs.d for details.
 *
 *************************************************************************
 * #ifdef VTO - for reading files produced by VAX Woolz into OS9 Woolz.
 * This is very restricted - only handles type 1 objects without plists.
 * A problem is that 16-bit words are byte-swapped between VAX and
 * MC68000 but anything written a byte at a time is in the correct order.
 * So we have to ensure that "getword()" not only reads 32 bits, but in
 * the correct byte order.
 *************************************************************************
 *
 * Modifications
 * 
 *	 7 Feb 1991		CAS		Mizar/OSK defines
 * 29-01-91		JimP@MRC	put in a readpropertlist() for polygons, to match
 *							the change in writeobj.c.  ALL appropriate objects
 *							should have propertylist read/written.
 * 23-10-90		JimP@MRC	put in readcompounda for compound objects.  This
 *							also required the definition of a deliberate NULL
 *							object, obtained by type==0 and nothing else.
 *	17 Jan 1989		CAS		readtext used 1 char more than allocated for string
 *	22 Nov 1988		dcb		Added param to woolz_exit to say where called from
 *  2  Oct 1988		dcb		Add read frame
 *	23 Nov 1987		BDP		Changed convhull and histo to use only one block of memory
 *	20 Nov 1987		BDP		Fixed readrect to read into correct field
 *	16 Nov 1987		BDP		Added read convhull in the readobj 'style'.
 *	29 Oct 1987		BDP		Protected Mallocs in readintervaldomain and readvaluetb
 *							against zero requests.
 *	14 May 1987		BDP		protection against null or empty objs
 *	 9 Mar 1987		CAS/GJP		Return a signed value from getword...
 *	 2 Mar 1987		GJP			Woolz_exit
 *	17 Dec 1986		CAS		Moved mfread into gplib + renamed fbread
 *	02 Dec 1986		jimp	Use fast macro instead of getc()
 *	27 Nov 1986		jimp	Return NULL if data type incorrect
 *	15 Oct 1986		CAS		Get linkcount correct
 *	12 Sep 1986		CAS		Includes
 */

#include <stdio.h>
//#include <fbio.h>
#include <wstruct.h>

#ifdef VTO
typedef	int	SGREY;
typedef	int	SCOORD;
typedef	int	SSMALL;
#else VTO
typedef	short	SGREY;
typedef	short	SCOORD;
typedef	short	SSMALL;
#endif VTO

#ifdef VTO
/* "putw()" on Vax was 32 bits, least significant 16 first */
static getword(fp)
FILE *fp;
{
	register unsigned i,j,k;
	i = mgetc(fp);
	j = mgetc(fp);
	k = mgetc(fp);
	return((int)((mgetc(fp)<<24)+(k<<16)+(j<<8)+i));
}
#else
static getword(fp)
register FILE *fp;
{
	int	i;
	i = mgetc(fp);
	if (i > 127) i -= 256;
	i = i << 8;
	return(mgetc(fp)+i);
}
#endif VTO


#ifdef VTO
struct object *vreadobj (fp)
#else VTO
struct object *readobj (fp)
#endif VTO
FILE *fp;
{
	int type;
	struct object *obj,*makemain(),*readvector(),*readpoint(),*readtext();
	struct intervaldomain *idmn, *readintervaldomain();
	struct propertylist *ipp, *readpropertylist();
	struct polygondomain *poly, *readpolygon();
	struct boundlist *blist;
	struct cvhdom *cd, *readcvhdom();
	struct irect *rect, *readrect();
	struct histogramdomain *hdom, *readhisto();
	struct pframe *rframe, *readframe();
	struct object *readcompounda();
	type = mgetc(fp);
#ifdef WTRACE
	fprintf(stderr,"Readobj, reading type %d object\n",type);
#endif WTRACE

	switch (type) {
	case EOF:
	case 0:
		return(NULL);

	case 1:
		idmn = readintervaldomain(fp);
		obj = makemain( type, idmn, NULL, NULL, NULL);
		readvaluetable(fp,obj);
		obj->plist = readpropertylist(fp);
		if (feof(fp) != 0)
		{
#ifdef WTRACE
			fprintf(stderr,"readobj unexpected eof found\n");
#endif WTRACE
			return(NULL);
		}
		else
			return(obj);
		
#ifndef VTO
	case 10:
		poly = readpolygon(fp);
		if (poly == NULL)
			return(NULL);
		obj = makemain( type, poly, NULL, NULL, NULL);
		obj->plist = readpropertylist(fp);
		if (feof(fp) != 0)
		{
#ifdef WTRACE
			fprintf(stderr,"readobj unexpected eof found\n");
#endif WTRACE
			return(NULL);
		}
		else
			return(obj);

	case 11:
		if (readboundlist(fp, &blist) != 0)
			return(NULL);
		else
			return( makemain( type, blist, NULL, NULL, NULL));

	case 12:
#ifdef CVTEST
		fprintf(stderr,"readobj: type 12 object being read \n");
#endif CVTEST
		poly = readpolygon(fp);
		if (poly == NULL)
			return(NULL);
		else {
			cd = readcvhdom(fp);
			if (cd == NULL)
				return(NULL);
			else
				return( makemain( type, poly, cd, NULL, NULL));
		}

	case 13:
		hdom = readhisto(fp);
		if (hdom == NULL)
			return(NULL);
		else
			return(makemain(type, hdom, NULL, NULL, NULL));
	
	case 20:
		rect = readrect(fp);
		if (rect == NULL)
			return(NULL);
		else
			return( makemain( type, rect, NULL, NULL, NULL));

	case 30:
	case 31:
		return(readvector(fp, type));

	case 40:
	case 41:
		return(readpoint(fp, type));
	case 60: /* frame */
		if ((rframe = readframe(fp)) == NULL)
			return(NULL);
		else {
			rframe->type = type;
			return(rframe);
		}
	case 70:
		return(readtext(fp,type));
	case 80:
	case 81:
		return(readcompounda(fp,type));
	case 110:
		return(makemain(110,NULL,NULL,readpropertylist(fp),NULL));
#endif VTO

	default:
		fprintf(stderr,"Can't read object type %d\n",type);
		return(NULL);

	}
}

/* object protection against empty objects has been added to prevent zero
Malloc requests occuring. bdp 14/5/87  */

static
struct intervaldomain *
readintervaldomain(fp)
register FILE *fp;
{
	register i,l,ll;
	int type,l1,k1,kl,nints;
	struct intervaldomain *idmn, *makedomain();
	register struct intervalline *ivln;
	register struct interval *itvl;

	type = mgetc(fp);
	if (type == 0)
		return(NULL);
	l1 = getword(fp);
	ll = getword(fp);
	k1 = getword(fp);
	kl = getword(fp);
	if (feof(fp) != 0)
		return(NULL);
	idmn = makedomain(type,l1,ll,k1,kl);

	if (wzemptyidom(idmn) == 0)
		switch (type) {
		case 1:
			nints = 0;
			ivln = idmn->intvlines;
			for (l=l1; l<=ll; l++) {
				ivln->nintvs = getword(fp);
				nints += ivln->nintvs;
				ivln++;
			}
			if (feof(fp) != 0)
				return(NULL);
			itvl = NULL;
			if (nints > 0)
				itvl = (struct interval *) Malloc(nints * sizeof(struct interval));
			idmn->freeptr = (char *) itvl;
			idmn->linkcount = 0;
			ivln = idmn->intvlines;
#ifdef VTO
			for (i=0; i<nints; i++,itvl++) {
				itvl->ileft = getword(fp);
				itvl->iright = getword(fp);
			}
			itvl = (struct interval *) idmn->freeptr;
#else VTO
			fbread(itvl, sizeof(struct interval), nints, fp);
#endif VTO
			for (l=l1; l<=ll; l++) {
				nints = ivln->nintvs;
				makeinterval(l, idmn, nints, itvl);
				ivln++;
				itvl += nints;
			}
			break;
	
		case 2:
			break;
	
		default:
			fprintf(stderr,"illegal domain type %d\n",type);
			woolz_exit(52, "readintervaldomain");
	
		}

	if (feof(fp) != 0)
		return(NULL);
	else
		return(idmn);
}

static
readvaluetable (fp, obj)
register FILE *fp;
struct object *obj;
{
	struct iwspace iwsp;
	struct intervaldomain *idmn;
	struct valuetable *vtb, *makevaluetb();
	int packing, type, kstart, l1, ll, vtblinearea;
	register int i;
	short sht;
	GREY bckgrnd, *v;
	register GREY *g, *gtop;

	type = mgetc(fp);
	
	switch (type) {
	case 0:
	case EOF:
		obj->vdom = NULL;
		return(NULL);
	case 1:
	case 11:
	case 21:
		type = 1;	/* a fiddle for case 11 */
		packing = mgetc(fp);
		idmn = obj->idom;
		l1 = idmn->line1;
		ll = idmn->lastln;
		fbread(&bckgrnd, sizeof(SGREY), 1, fp);
		vtb = makevaluetb(1, l1, ll, bckgrnd, obj);
		obj->vdom = vtb;
		vtblinearea = linearea(obj);
		v = NULL;	/* default */
		vtb->linkcount = 0;
		if (vtblinearea > 0)
		{
			v = (GREY *) Malloc(vtblinearea * sizeof(GREY));
			vtb->linkcount = 1;
		}
		vtb->freeptr = (char *) v;
		initrasterscan(obj,&iwsp,0);
		while (nextinterval(&iwsp) == 0) {
			if (iwsp.nwlpos)
				kstart = iwsp.lftpos;
			switch (packing) {
#ifndef VTO
			case 1:
				fbread(v+iwsp.lftpos-kstart, sizeof(GREY), iwsp.colrmn, fp);
				break;
#ifndef OSK
			case 2:
				g = v+iwsp.lftpos-kstart;
				for (i=0; i<iwsp.colrmn; i++) {
					fread(&sht, sizeof(short), 1, fp);
					*g++ = (int) sht;
				}
				break;
#endif OSK
#endif VTO
			case 3:
				/*
				 * byte packing.
				 */
				g = v+iwsp.lftpos-kstart;
				gtop = g + iwsp.colrmn;
				while (g < gtop)
					*g++ = mgetc(fp);
				break;
			default:
				fprintf(stderr,"illegal grey-table packing %d\n",packing);
				woolz_exit(52, "readvaluetable.1");
			}
			if (iwsp.intrmn == 0) {
				makevalueline(vtb, iwsp.linpos, kstart, iwsp.rgtpos, v);
				v += (iwsp.rgtpos - kstart + 1);
			}
		}
		if (feof(fp) != 0)
			return(1);
		else
			return(0);

	case 2:
	case 12:
	case 22:
		fprintf(stderr,"Can't read float grey-table\n",type);
		woolz_exit(52, "readvaluetable.2");
	default:
		fprintf(stderr,"illegal grey-table type %d\n",type);
		woolz_exit(52, "readvaluetable.3");
	}
}





static
struct propertylist *readpropertylist (fp)
register FILE *fp;
{
	register int type, si, i;
	struct propertylist *pl;
	register char *ch;
#ifdef VTO
	static int warn=1;
#endif VTO

	type = mgetc(fp);
	switch (type) {
	case EOF:
	case 0:
	default:
		return(NULL);
	case 1:
#ifdef VTO
		if (warn) {
			warn = 0;
			fprintf(stderr,"Warning - propertylist may not be converted\n");
		}
#endif VTO
		si = getword(fp);
		pl = (struct propertylist *) Malloc(si);
		pl->size = si;
		ch = (char *)pl;
		ch += sizeof(SMALL);
		si -= sizeof(SSMALL);
		for (; si > 0; si--)
			*ch++ = mgetc(fp);
		if (feof(fp) != 0)
			return(NULL);
		else
			return(pl);
	}
}

#ifndef VTO

static
struct polygondomain *readpolygon (fp)
FILE *fp;
{
	int type;
	register int nvertices;
	struct polygondomain *poly, *makepolydmn();

	type = mgetc(fp);
	nvertices = getword(fp);
	if (feof(fp) != 0)
		return(NULL);
	if (type < 1 || type > 2)
		fprintf(stderr,"Illegal polygon domain type %d\n",type);
	poly = makepolydmn(type,NULL,0,nvertices,1);
	poly->nvertices = nvertices;
	switch (type) {

	case 1:
		fbread(poly->vtx, sizeof(struct ivertex), nvertices, fp);
		break;

	case 2:
		fbread(poly->vtx, sizeof(struct fvertex), nvertices, fp);
		break;
	}
	if (feof(fp) != 0)
		return(NULL);
	else
		return(poly);
}



static
readboundlist (fp, blist)
FILE *fp;
struct boundlist **blist;
{
	int type = mgetc(fp);
	switch (type) {
	case EOF :
		return(1);
	case 0:
		*blist = NULL;
		break;
	case 1:
		*blist = (struct boundlist *) Malloc (sizeof(struct boundlist));
		if (((*blist)->type = mgetc(fp)) == EOF)
			return(1);
		if (readboundlist(fp,&((*blist)->next)) != 0)
			return(1);
		if (readboundlist(fp,&((*blist)->down)) != 0)
			return(1);
		(*blist)->wrap = getword(fp);
		if (feof(fp))
			return(1);
		(*blist)->poly = readpolygon(fp);
		if ((*blist)->poly == NULL)
			return(1);
		break;
	}
	return(0);
}

void _strass(void *d, void *s, int sz)
{
	memcpy(d, s, sz);
}

static struct cvhdom *readcvhdom( fp )
FILE *fp;
{
	int type;
	struct cvhdom cdomst;
	struct cvhdom *c;
	
	type = mgetc(fp);
	switch(type){
	case EOF:
		return(NULL);
	case 1:
		c = (struct cvhdom *) Malloc (sizeof( struct cvhdom));
		cdomst.type = type;
		cdomst.nchords = getword(fp);
		cdomst.nsigchords = getword(fp);
		cdomst.mdlin = getword(fp);
		cdomst.mdkol = getword(fp);
		if (feof(fp) != 0)
			return(NULL);
#ifdef CVTEST
		fprintf(stderr,"readobj: %d chords being read \n",cdomst.nchords);
#endif CVTEST
		c = (struct cvhdom *) Malloc(sizeof( struct cvhdom) + 
			cdomst.nchords * sizeof( struct chord));
		_strass( c, &cdomst, sizeof(struct cvhdom));
		c->ch = (struct chord *) (c + 1);
		fbread(c->ch, sizeof(struct chord), c->nchords, fp);
		if (feof(fp) != 0)
			return(NULL);
		return(c);
	}
	return(NULL);
}


static
struct irect *readrect(fp)
FILE *fp;
{
	register type;
	struct irect *ir;
	struct frect *fr;
	type = mgetc(fp);
	if (feof(fp) != 0)
		return(NULL);
	/* 20/11/87 bdp . following reads now to k[0] not rangle, this is still
	a horrible way of doing this even it is quick ! */
	
	switch (type) {
	case 1:
		ir = (struct irect *) Malloc (sizeof(struct irect));
		ir->type = type;
		fbread(&(ir->irk[0]), sizeof(struct irect) - sizeof(SMALL), 1, fp);
		if (feof(fp) != 0)
			return(NULL);
		return(ir);
	case 2:
		fr = (struct frect *) Malloc (sizeof(struct frect));
		fr->type = type;
		fbread(&(fr->frk[0]), sizeof(struct frect) - sizeof(SMALL), 1, fp);
		if (feof(fp) != 0)
			return(NULL);
		return((struct irect *)fr);
	}
}


static
struct object *readvector(fp, type)
FILE *fp;
{
	struct ivector *iv;
	struct fvector *fv;
	switch (type) {
	case 30:
		iv = (struct ivector *) Malloc (sizeof(struct ivector));
		iv->type = type;
		fbread(&(iv->style), sizeof(struct ivector) - sizeof(SMALL), 1, fp);
		if (feof(fp) != 0)
			return(NULL);
		return((struct object *)iv);
	case 31:
		fv = (struct fvector *) Malloc (sizeof(struct fvector));
		fv->type = type;
		fbread(&(fv->style), sizeof(struct fvector) - sizeof(SMALL), 1, fp);
		if (feof(fp) != 0)
			return(NULL);
		return((struct object *)fv);
	}
}


static
struct object *readpoint(fp, type)
FILE *fp;
{
	struct ipoint *iv;
	struct fpoint *fv;
	switch (type) {
	case 40:
		iv = (struct ipoint *) Malloc (sizeof(struct ipoint));
		iv->type = type;
		fbread(&(iv->style), sizeof(struct ipoint) - sizeof(SMALL), 1, fp);
		if (feof(fp) != 0)
			return(NULL);
		return((struct object *)iv);
	case 41:
		fv = (struct fpoint *) Malloc (sizeof(struct fpoint));
		fv->type = type;
		fbread(&(fv->style), sizeof(struct fpoint) - sizeof(SMALL), 1, fp);
		if (feof(fp) != 0)
			return(NULL);
		return((struct object *)fv);
	}
}


static
struct histogramdomain *readhisto(fp)
FILE *fp;
{
	struct histogramdomain histost;
	struct histogramdomain *h;
	
	histost.type = mgetc(fp);
	fbread(&histost.r, sizeof(struct histogramdomain) - sizeof(SMALL), 1, fp);
	if (feof(fp) != 0)
		return(NULL);
	h = (struct histogramdomain *) Malloc (sizeof(struct histogramdomain) +
			histost.npoints * (histost.type == 1? sizeof(int): sizeof(float)));
	_strass( h, &histost , sizeof(struct histogramdomain));
	h->hv = (int *) (h + 1);
	fbread(h->hv, (h->type == 1? sizeof(int): sizeof(float)), h->npoints, fp);
	if (feof(fp) != 0)
		return(NULL);
	return(h);
}

/*
 *	R E A D F R A M E  --  
 *
 */
static
struct pframe *readframe(fp)
FILE *fp;
{
	struct pframe *rf;
	rf = (struct pframe *) Malloc(sizeof(struct pframe));
	fbread(&(rf->scale), sizeof(struct pframe)-sizeof(SMALL), 1, fp);
	if (feof(fp) != 0) {
		Free(rf);
		return(NULL);
	}
	return(rf);
}

/*
 *	R E A D T E X T  --
 *
 */
static
struct object *readtext(fp,type)
FILE *fp;
{
	struct text *tobj;
	struct textdomain *tdom;
	char *tstring;
	int i;
	
	tdom = (struct textdomain *) Malloc(sizeof(struct textdomain));
	fbread(tdom,sizeof(struct textdomain),1,fp);
	tstring = (char *) Malloc(tdom->stringlen+2);
	tobj = (struct text *) makemain(type,tdom,tstring,NULL,NULL);
	for(i = 0;i <= tdom->stringlen; i++)
		*tstring++ = mgetc(fp);
	*tstring = '\0';		/* just in case ! */
	return((struct object *) tobj);
}

static struct object *readcompounda(fp,type)
FILE *fp;
int type;
{
	extern struct object *readobj();
	struct compounda *c, *makecompounda();
	int i,n,otype;
	otype = getc(fp);
	n = getword(fp);
	c = makecompounda(type,1,n,NULL,otype); 
	for (i=0; i<n; i++)
		c->o[i] = readobj(fp);
	c->p = readpropertylist(fp);
	return ((struct object *) c);
}
	

#endif VTO


