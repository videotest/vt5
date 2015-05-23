/************************************************************************
 *                                                                      *
 *                              details.c                               *
 *                                                                      *
 ************************************************************************
 *
 *      Function to preint out all the details of a woolz object.
 *      Go down the structure a level at a time passing single pointer
 *      to the next routine.
 *
 *      Structure of code taken from write obj by J Piper
 *
 *      Graham John Page  Shandon southern Products Ltd 5 Dec 1985
 *
 * Modifications
 *
 *	18 Jan 1991		CAS/GJP		Added interval count
 *	22 Nov 1988		dcb			Added param to woolz_exit to say where called from
 *	17 Nov 1987		CAS			Combined two versions..... (TEST + BASIC)
 *	16 Jun 1987		CAS			Print out histogram properly..
 *	 2 Mar 1987		GJP			Woolz_exit
 *	 3 Dec 1986		CAS			Print real property list instead of random
 *								memory
 *	13 Sep 1986		CAS			Hand modified to long names
 *
 */


#include <stdio.h>
#include <wstruct.h>

#define SCREENWIDTH 60

typeprint ( i ) int i ; {fprintf(stderr,"( type=%d )\n",i);}

/*
 *	B Y T E S T R E A M P R I N T
 *
 */
bytestreamprint(pl,si)
char *pl;
int si;
{
	int i;
	int l=0;
	for (i=0; i<si;++i) {
		fprintf(stderr,"%4d",(*pl++)&255);
		l += 4 ;					/* size of %d above*/ 
		if (l>=SCREENWIDTH) {
			putc('\n',stderr);
			l=0;
		}
	}
	putc('\n',stderr);
}

/*
 *	G R E Y S T R E A M P R I N T
 *
 */
greystreamprint(pl,si)
GREY *pl;
int si;
{
	int i;
	int l=0;
	for (i=0; i<si;++i) {
		fprintf(stderr,"%6d",*pl++);
		l += 6;
		if (l>=SCREENWIDTH) {
			putc('\n',stderr);
			l=0;
		}
	}
	putc('\n',stderr);
}

/*
 *	I N T S T R E A M P R I N T
 *
 */
intstreamprint(pl,si)
int *pl;
int si;
{
	int i;
	int l=0;
	for (i=0; i<si;++i)	{
		fprintf(stderr,"%10d",*pl++);
		l += 10;
		if (l>=SCREENWIDTH) {
			putc('\n',stderr);
			l=0;
		}
	}
	putc('\n',stderr);
}

/*
 *	F L O A T S T R E A M P R I N T
 *
 */
floatstreamprint(pl,si)
float *pl;
int si;
{
	int i;
	int l=0;
	for (i=0; i<si;++i) {
		fprintf(stderr,"%4e",*pl++);
		l += 10 ;
		if (l>=SCREENWIDTH) {
			putc('\n',stderr);
			l=0;
		}
	}
	putc('\n',stderr);
}

/*
 *	D E T A I L S
 *
 */
details(obj)
struct object *obj;
{
	fprintf(stderr,"** object **\n");
	typeprint (obj->type);

	switch (obj->type) {
	case 1:
		printintervaldomain(obj->idom);
		printvaluetable(obj);
		printpropertylist(obj->plist);
		break;

	case 10:
		printpolygon(obj->idom);
		break;

	case 11:
		printboundlist(obj->idom);
		break;

	case 13:
		printhisto( obj->idom);
		break;

	case 20:
/*
		printrect(obj->idom);
*/
		fprintf(stderr,"later\n");
		break;

	case 30:
		printintvector(obj);
		break;

	case 31:
		printfloatvector(obj);
		break;

	case 40:
		printintpoint(obj);
		break;

	case 41:
		printfloatpoint(obj);
		break;

	case 70:
		printtext(obj);
		break;

	case 110:
		printpropertylist(obj->plist);
		break;

	default:
		fprintf(stderr,"Invalid object ");
		typeprint(obj->type);
		woolz_exit(51, "details");
	}
}


printinterval(intv)
struct interval *intv;
{
	fprintf ( stderr,"[%5d,%5d] ",intv->ileft,intv->iright);
}

INTSIZE = 14 ;

printintervalline ( ivln , nlines )
struct intervalline *ivln ;
int nlines;
{
	int i, j, l=0, totint=1;
	struct interval *intvsptr;

	for (i=0; i<=nlines; ivln++,i++) {
		fprintf ( stderr, "** intervalline %d **. %d intervals in line (tot int %d)\n",
					i,ivln->nintvs,totint);
		for (j=0,intvsptr=ivln->intvs; j<ivln->nintvs; j++,intvsptr++) {
			totint++;
			printinterval(intvsptr);
			l += INTSIZE ;
			if ( j > SCREENWIDTH ) {
				putc('\n',stderr);
				l=0;
			}
		}
		if (l != 0)
			putc('\n',stderr);
	};
}

printintervaldomain(itvl)
struct intervaldomain *itvl;
{
	register int i, j, nlines;

	fprintf(stderr,"** intervaldomain  ** ");
	if (itvl == NULL)
		fprintf(stderr,"NULL\n");
	else {
		typeprint(itvl->type);
		fprintf(stderr,"First line = %d , Last line = %d ;\
						First column = %d , Last column = %d\n",
						itvl->line1,itvl->lastln,itvl->kol1,itvl->lastkl);
		switch (itvl->type) {
		case 1:
				nlines = itvl->lastln - itvl->line1;
				fprintf ( stderr , "There are %d interval lines\n",nlines);
				printintervalline(itvl->intvlines,nlines);
				break;
		case 2:
				break;
		default:
				fprintf(stderr,"Invalid domain type %d\n",itvl->type);
				woolz_exit(51, "printintervaldomain");
		}
	}
}



static
printpropertylist( plist)
struct propertylist *plist;
{
	fprintf ( stderr , "** propertylist ** ");
	if (plist == NULL)
		fprintf(stderr,"NULL\n");	   
	else {
		fprintf ( stderr ,"Property list is %d bytes long\n",plist->size);
		bytestreamprint(plist,plist->size);
	}
}



static
printvaluetable( obj)
struct object *obj;
{
	struct iwspace iwsp;
	struct gwspace gwsp;
	int packing, type;
	register int i;
	register GREY *g, min, max;
	short sht;
	int reccount=0;
	fprintf(stderr,"** valuetable ** ");

	if (obj->vdom == NULL) 
		fprintf ( stderr ,"NULL\n");
	else {
		typeprint(type=obj->vdom->type);
		switch (type) {
		case 1:
		case 11:
		case 21:
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
				else if (min > -32768 && max < 32768)
					packing = 2;
				else
					packing = 1;
				fprintf(stderr,"packing = %d ",packing);
				fprintf(stderr," background value = %d\n",(obj->vdom->bckgrnd));
				initgreyscan(obj,&iwsp,&gwsp);
				while(nextgreyinterval(&iwsp) == 0) {
					fprintf(stderr,"Interval record %d\n",++reccount);
					greystreamprint(gwsp.grintptr,iwsp.colrmn); 
				}
				break;
				
		case 2:
		case 12:
		case 22:
				fprintf(stderr,"Float grey tables are invalid\n");
				woolz_exit(53, "printvaluetable.1");
		default:
				fprintf(stderr,"Invalid grey table type %d\n",type);
				woolz_exit(53, "printvaluetable.2");
		}
	}
}

printintvtx(vtx,nverticies)
struct ivertex *vtx;
int nverticies;
{
int i;
		for (i=0;i<nverticies;vtx++,i++)
				fprintf(stderr,"X vertex %d Y vertex %d\n",vtx->vtX,vtx->vtY);
}

printfloatvtx(vtx,nverticies)
struct fvertex *vtx;
int nverticies;
{
int i;
		for (i=0;i<nverticies;vtx++,i++)
				fprintf(stderr,"X vertex %e Y vertex %e\n",vtx->vtX,vtx->vtY);
}

static
printpolygon( poly)
struct polygondomain *poly;
{
		register int nvertices;

		fprintf ( stderr ,"** polygon ** ");
		typeprint(poly->type);
		fprintf(stderr,"%d vertices out of a maximum of %d.\n"
				,nvertices = poly->nvertices,poly->maxvertices);
		switch (poly->type) 
		{
		case 1:
				printintvtx(poly->vtx,nvertices);
				break;

		case 2:
				printfloatvtx(poly->vtx,nvertices);
				break;
		
		default:
				fprintf(stderr,"Illegal polygon type %d\n",poly->type);
				woolz_exit(51, "printpolygon");
		}
}


static
printboundlist (blist)
struct boundlist *blist;
{
		fprintf(stderr,"** boundlist ** ");
		if (blist == NULL)
				fprintf(stderr,"NULL\n");
		else 
				{
				typeprint(blist->type);
				fprintf(stderr,"Next boundlist\n");
				printboundlist(blist->next);
				fprintf(stderr,"Down boundlist\n");
				printboundlist(blist->down);
				fprintf(stderr,"Wrap has a value of %d\n",blist->wrap);
				printpolygon(blist->poly);
				}
}

						
static printintpoint( pnt)
struct ipoint *pnt;
{
		fprintf(stderr,"** integer point ** ");
		fprintf(stderr,"k = %d  l = %d  style = %d\n",
						pnt->k, pnt->l,pnt->style);
}

static printfloatpoint( pnt)
struct fpoint *pnt;
{
		fprintf(stderr,"** floating point ** ");
				fprintf(stderr,"k = %e  l = %e style = %e\n",
						pnt->k, pnt->l,pnt->style);
}
						
static printintvector( vec)
struct ivector *vec;
{
		fprintf(stderr,"** integer vector ** ");
		fprintf(stderr,"k1 = %d  l1 = %d  k2 = %d  l2 = %d  style = %d\n",
						vec->k1, vec->l1, vec->k2, vec->l2,vec->style);
}

static printfloatvector( vec)
struct fvector *vec;
{
		fprintf(stderr,"** floating vector ** ");
				fprintf(stderr,"k1 = %e  l1 = %e  k2 = %e  l2 = %e  style = %e\n",
						vec->k1, vec->l1, vec->k2, vec->l2,vec->style);
}
						

static printhisto( h)
struct histogramdomain *h;
{
int type;
		fprintf(stderr,"** histogram ** ");
		typeprint(type=h->type);
		fprintf(stderr,"style = %d k = %d l = %d number of points = %d\n",
				h->r,h->k,h->l,h->npoints);
		switch (h->type) 
		{
		case 1:
				intstreamprint(h->hv,h->npoints);
				break;
		case 2:
				floatstreamprint(h->hv,h->npoints);
				break;
		default:
				fprintf(stderr,"Invalid histogram type\n");
		}
}

static printtext(tobj)
struct text *tobj;
{
	struct textdomain *tdom;
	char *tstring,c;
	int i;

	fprintf(stderr,"** text ** ");  
	tstring = tobj->text;
	tdom = tobj->tdom;
	for(i = 0;i <= tdom->stringlen; i++) {
		c = tstring[i];
		putc(c,stderr);
	}
}
