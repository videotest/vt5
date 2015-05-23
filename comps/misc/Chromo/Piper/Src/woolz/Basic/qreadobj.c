/*
 *      Q R E A D O B J . C  --  Read objects using unbufferd i/o
 *
 *
 *  Written: Clive A Stubbings
 *           Image Recognition Systems
 *           720 Birchwood Boulevard
 *           Birchwood Science Park
 *           Warrington
 *           Cheshire
 *           WA3 7PX
 *
 *	Copyright (c) and intellectual property rights Image Recognition Systems
 *
 *  Date:    3rd October 1986
 *
 *  Modifications
 *
 *	 7 Feb 1991		CAS		Mizar/OSK defines
 *	22 Nov 1988		dcb		Added param to woolz_exit to say where called from
 *	14 May 1987		BDP		protection against empty objs
 *	2  Mar 1987		GJP		Woolz_exit
 *	15 Oct 1986		CAS		Set up linkcount etc
 *	 5 Oct 1986		CAS		Un-modded use of FILE * - back to path
 *	 4 Oct 1986		GJP		Mask out all byte reads, take out all exit 
 *							statments. Changed get word to read in 2 
 *							not 4 bytes
 *
 * Take from readobj and modified to use unbuffered I/O still pass a
 * file pointer to get into the routine.
 *
 * Routines to input object structures from sequential data streams.
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
 */

#include <stdio.h>
#include <wstruct.h>

#ifdef VTO
typedef	int	SGREY;
typedef	int	SCOORD;
typedef	int	SSMALL;
#define SMSK 0xFFFFFFFF
#else VTO
typedef	short	SGREY;
typedef	short	SCOORD;
typedef	short	SSMALL;
#define SMSK 0xFFFF
#endif VTO

#define BMSK 0xFF

static int infile;

/* "putw()" on Vax was 32 bits, least significant 16 first */
static getword(fd)
int	fd;
{
#ifdef VTO
	int i,j,k,l;
	infile = read(fd,&i,1);
	infile = read(fd,&j,1);
	infile = read(fd,&k,1);
	infile = read(fd,&l,1);
	return((int)(((l&0xFF)<<24)+((k&0xFF)<<16)+((j&0xFF)<<8)+i&0xFF));
#else
	short i;
	infile = read(fd,&i,2);
	return(i);
#endif VTO
}

#ifdef VTO
struct object *vqreadobj (fd)
#else VTO
struct object *qreadobj (fd)
#endif VTO
int		fd;
{
	unsigned char type;
	struct object *obj,*makemain(),*readvector(),*readpoint(),*readtext();
	struct intervaldomain *idmn, *readintervaldomain();
	struct propertylist *ipp, *readpropertylist();
	struct polygondomain *poly, *readpolygon();
	struct boundlist *blist;
	struct irect *rect, *readrect();
	struct histogramdomain *hdom, *readhisto();

	infile = read (fd,&type,1);

	if (!infile)
		return(NULL);

	switch ((int) type) {

	case 1:
		idmn = readintervaldomain(fd);
		obj = makemain( type, idmn, NULL, NULL, NULL);
		readvaluetable(fd,obj);
		obj->plist = readpropertylist(fd);

		if (!infile)
			return(NULL);
		else
			return(obj);

#ifndef VTO
	case 10:
		poly = readpolygon(fd);
		if (poly == NULL)
			return(NULL);
		else
			return( makemain( type, poly, NULL, NULL, NULL));

	case 11:
		if (readboundlist(fd, &blist) != 0)
			return(NULL);
		else
			return( makemain( type, blist, NULL, NULL, NULL));

	case 13:
		hdom = readhisto(fd);
		if (hdom == NULL)
			return(NULL);
		else
			return(makemain(type, hdom, NULL, NULL, NULL));

	case 20:
		rect = readrect(fd);
		if (rect == NULL)
			return(NULL);
		else
			return( makemain( type, rect, NULL, NULL, NULL));

	case 30:
	case 31:
		return(readvector(fd, type));

	case 40:
	case 41:
		return(readpoint(fd, type));
	case 70:
		return(readtext(fd,type));
	case 110:
		return(makemain(110,NULL,NULL,readpropertylist(fd),NULL));
#endif VTO

	default:
		fprintf(stderr,"Readobj ;Can't read object type %d\n",type);

	}
}

/* protection against empty objects added to prevent zero malloc requests
occuring. No protection needed here against NULLs as they wont be written
bdp 14.5.87 */

static
struct intervaldomain *readintervaldomain(fd)
register int fd;
{
	register i,l,ll;
	unsigned char type;
	int l1,k1,kl,nints;
	struct intervaldomain *idmn, *makedomain();
	register struct intervalline *ivln;
	register struct interval *itvl;

	infile = read (fd,&type,1);
	if (type == 0)
		return(NULL);
	l1 = getword(fd);
	ll = getword(fd);
	k1 = getword(fd);
	kl = getword(fd);

	if (!infile)
		return(NULL);

	idmn = makedomain(type,l1,ll,k1,kl);

	if (wzemptyidom(idmn) == 0)
		switch ((int) type) {
		case 1:
			nints = 0;
			ivln = idmn->intvlines;
			for (l=l1; l<=ll; l++) {
				ivln->nintvs = getword(fd);
				nints += ivln->nintvs;
				ivln++;
			}
			if (!infile)
				return(NULL);
			itvl = (struct interval *) Malloc(nints * sizeof(struct interval));
			idmn->freeptr = (char *) itvl;
			idmn->linkcount = 0;
			ivln = idmn->intvlines;
#ifdef VTO
			for (i=0; i<nints; i++,itvl++) {
				itvl->ileft = getword(fd);
				itvl->iright = getword(fd);
			}
			itvl = (struct interval *) idmn->freeptr;
#else VTO
			/* this is much faster on OS9, unfortunately */
			infile = read(fd, itvl, sizeof(struct interval) * nints);
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
			fprintf(stderr,"Readobj illegal domain type %d\n",type);
	
		}

	if (!infile)
		return(NULL);
	else
		return(idmn);
}


static
readvaluetable (fd, obj)
int		fd;
struct object *obj;
{
	struct iwspace iwsp;
	struct intervaldomain *idmn;
	struct valuetable *vtb, *makevaluetb();
	int kstart, l1, ll;
	unsigned char type, packing;
	register int i;
	short sht;
	GREY bckgrnd, *v;
	register GREY *g, *gbot;
	register unsigned char *bg;

	infile = read(fd, &type, 1);

	if (!infile)
		return(NULL);

	switch ((int) type) {
	case 0:
		obj->vdom = NULL;
		return(NULL);
	case 1:
	case 11:
	case 21:
		type = 1;	/* a fiddle for case 11 */
		infile = read (fd, &packing, 1);
		
		idmn = obj->idom;
		l1 = idmn->line1;
		ll = idmn->lastln;
		infile = read(fd, &bckgrnd, sizeof(SGREY));
		bckgrnd  &= SMSK ;
		vtb = makevaluetb(1, l1, ll, bckgrnd, obj);
		obj->vdom = vtb;
		v = (GREY *) Malloc(linearea(obj) * sizeof(GREY));
		vtb->freeptr = (char *) v;
		vtb->linkcount = 1;
		initrasterscan(obj,&iwsp,0);
		while (nextinterval(&iwsp) == 0) {
			if (iwsp.nwlpos)
				kstart = iwsp.lftpos;
			switch (packing) {
#ifndef VTO
			case 1:
				infile = read(fd, v+iwsp.lftpos-kstart, sizeof(GREY) * iwsp.colrmn);
				break;
#ifndef OSK
			case 2:
				g = v+iwsp.lftpos-kstart;
				for (i=0; i<iwsp.colrmn; i++) {
					infile = read(fd, &sht, sizeof(short));
					*g++ = (int) sht;
				}
				break;
#endif OSK
#endif VTO
			case 3:
				/*
				 * byte packing.  Use read to get advantages of
				 * buffering, then unpack in situ
				 */
				gbot = v+iwsp.lftpos-kstart;
				infile = read(fd, gbot, sizeof(char) * iwsp.colrmn);
				bg = (unsigned char *)gbot;
				bg += iwsp.colrmn;
				g = gbot + iwsp.colrmn;
				while (g > gbot)
					*--g = *--bg;
				break;
			default:
				fprintf(stderr,"illegal grey-table packing %d\n",packing);
				woolz_exit(52, "readvaluetable");
			}
			if (iwsp.intrmn == 0) {
				makevalueline(vtb, iwsp.linpos, kstart, iwsp.rgtpos, v);
				v += (iwsp.rgtpos - kstart + 1);
			}
		}
		if (!infile)
			return(1);
		else
			return(0);

	case 2:
	case 12:
	case 22:
		fprintf(stderr,"Readobj can't read float grey-table\n",type);
		break ;
	default:
		fprintf(stderr,"Readobj illegal grey-table type %d\n",type);
	}
}





static
struct propertylist *readpropertylist (fd)
int  fd;
{
	register int si, i;
	unsigned char type;
	struct propertylist *pl;
	char *ch;
#ifdef VTO
	static int warn=1;
#endif VTO

	infile = read (fd, &type, 1);

	if (!infile)
		return(NULL);
	
	switch ((int) type) {
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
		si = getword(fd);
		pl = (struct propertylist *) Malloc(si);
		pl->size = si;
		ch = (char *)pl;
		ch += sizeof(SMALL);
		si -= sizeof(SSMALL);
		infile = read(fd,ch,si);
		if (!infile)
			return(NULL);
		else
			return(pl);
	}
}

#ifndef VTO

static
struct polygondomain *readpolygon (fd)
int	fd;
{
	unsigned char type;
	register int nvertices;
	struct polygondomain *poly, *makepolydmn();

	infile = read (fd, &type, 1);
	
	nvertices = getword(fd);
	if (!infile)
		return(NULL);
	if (type < 1 || type > 2)
		fprintf(stderr,"Illegal polygon domain type %d\n",type);
	poly = makepolydmn(type,NULL,0,nvertices,1);
	poly->nvertices = nvertices;
	switch ((int) type) {

	case 1:
		infile = read(fd, poly->vtx, sizeof(struct ivertex) * nvertices);
		break;

	case 2:
		infile = read(fd, poly->vtx, sizeof(struct fvertex)* nvertices);
		break;
	}
	if (!infile)
		return(NULL);
	else
		return(poly);
}


static
readboundlist (fd, blist)
int fd;
struct boundlist **blist;
{
	unsigned char type;
	infile = read (fd, &type, 1);
	
	if (!infile)
		return(1);

	switch ((int) type) {

	case 0:
		*blist = NULL;
		break;
	case 1:
		*blist = (struct boundlist *) Malloc (sizeof(struct boundlist));
		infile = read(fd, &(*blist)->type, 1);
		
		if (!infile)
			return(1);
		if (readboundlist(fd,&((*blist)->next)) != 0)
			return(1);
		if (readboundlist(fd,&((*blist)->down)) != 0)
			return(1);

		(*blist)->type &= BMSK;
		(*blist)->wrap = getword(fd);
		if (!infile)
			return(1);
		(*blist)->poly = readpolygon(fd);
		if ((*blist)->poly == NULL)
			return(1);
		break;
	}
	return(0);
}


static
struct irect *readrect(fd)
int	fd;
{
	unsigned char type;
	struct irect *ir;
	struct frect *fr;
	infile = read (fd, &type, 1);
	
	if (!infile)
		return(NULL);
	switch ((int) type) {
	case 1:
		ir = (struct irect *) Malloc (sizeof(struct irect));
		ir->type = type;
		infile = read(fd, &(ir->rangle), sizeof(struct irect) - sizeof(SMALL));
		if (!infile)
			return(NULL);
		return(ir);
	case 2:
		fr = (struct frect *) Malloc (sizeof(struct frect));
		fr->type = type;
		infile = read(fd, &(fr->rangle), sizeof(struct frect) - sizeof(SMALL));
		if (!infile)
			return(NULL);
		return((struct irect *)fr);
	}
}


static
struct object *readvector(fd, type)
int	fd;
unsigned char type;
{
	struct ivector *iv;
	struct fvector *fv;
	switch ((int) type) {
	case 30:
		iv = (struct ivector *) Malloc (sizeof(struct ivector));
		iv->type = type;
		infile = read(fd, &(iv->style), sizeof(struct ivector) - sizeof(SMALL));
		if (!infile)
			return(NULL);
		return((struct object *)iv);
	case 31:
		fv = (struct fvector *) Malloc (sizeof(struct fvector));
		fv->type = type;
		infile = read(fd, &(fv->style), sizeof(struct fvector) - sizeof(SMALL));
		if (!infile)
			return(NULL);
		return((struct object *)fv);
	}
}


static
struct object *readpoint(fd, type)
int	fd;
unsigned char type;
{
	struct ipoint *iv;
	struct fpoint *fv;
	switch ((int) type) {
	case 40:
		iv = (struct ipoint *) Malloc (sizeof(struct ipoint));
		iv->type = type;
		infile = read(fd, &(iv->style), sizeof(struct ipoint) - sizeof(SMALL));
		if (!infile)
			return(NULL);
		return((struct object *)iv);
	case 41:
		fv = (struct fpoint *) Malloc (sizeof(struct fpoint));
		fv->type = type;
		infile = read(fd, &(fv->style), sizeof(struct fpoint) - sizeof(SMALL));
		if (!infile)
			return(NULL);
		return((struct object *)fv);
	}
}


static
struct histogramdomain *readhisto(fd)
int fd;
{
	struct histogramdomain *h = (struct histogramdomain *)Malloc(sizeof(struct histogramdomain));
	infile = read (fd, &h->type, 1);
	h->type &= BMSK;
	
	infile = read(fd, &h->r, sizeof(struct histogramdomain) - sizeof(SMALL));
	if (!infile )
		return(NULL);
	h->hv = (int *) Malloc (h->npoints * (h->type == 1? sizeof(int): sizeof(float)));
	infile = read(fd, h->hv, (h->type == 1? sizeof(int): sizeof(float)) * h->npoints);
	if (!infile)
		return(NULL);
	return(h);
}

static
struct object *readtext(fd,type)
int fd;
unsigned char type;
{
	struct text *tobj;
	struct textdomain *tdom;
	char *tstring;
	int i;
	
	tdom = (struct textdomain *) Malloc(sizeof(struct textdomain));
	infile = read(fd, tdom,sizeof(struct textdomain));
	tstring = (char *) Malloc(tdom->stringlen+1);
	tobj = (struct text *) makemain(type,tdom,tstring,NULL,NULL);
	infile = read(fd,tstring,tdom->stringlen);
	*(tstring + tdom->stringlen) = '\0';
	return((struct object *) tobj);
}

#endif VTO
