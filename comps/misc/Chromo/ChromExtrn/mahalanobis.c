/*
 * mahalanobis.c	Jim Piper	27/3/84
 *
 * Compute Mahalanobis' generalised distance between two vectors
 * using a given inverted variance-covariance matrix.
 *
 * Use it for maximum likelihood classifier.
 *
 *	Modifications
 *
 *	 7 Feb 1991		CAS		remove Mizar defs -> osk
 */

#include <stdio.h>
#include <math.h>
#include <wstruct.h>
#include <chromanal.h>

#define TRUEMAXLIK

//#ifdef OSK
//#define INTARITH
//#endif OSK

//#ifndef OSK
double classmeans[MAXCLASS][MAXDIM];
double classinvvars[MAXCLASS][MAXDIM];
double invvar[MAXDIM];
float sqrtdet,clsqrtdet[MAXCLASS];
float vcv[MAXDIM*(MAXDIM+1)/2];
float clvcv[MAXCLASS][MAXDIM*(MAXDIM+1)/2];
int **tabmahal;
double invvarsqrtdet;
//#endif OSK

double clinvvarsqrtdet[MAXCLASS];

//#ifdef INTARITH
int iclassmeans[MAXCLASS][MAXDIM];
int iclassinvvars[MAXCLASS][MAXDIM];
double conversionf[MAXCLASS];
//#endif INTARITH

static void int2alloc(tab, dim, dim)
int ***tab;
{
	int i;
	*tab = (int**)calloc(sizeof(int*),dim);
	for (i = 0; i < dim; i++)
		(*tab)[i] = (int *)calloc(sizeof(int),dim);
}


/*
 * initialize Mahalanobis vcv lookup table
 */
//#ifndef OSK
_initmahalanobis(dim)
int dim;
{
	register int i, j, *tabmahali;

	int2alloc(&tabmahal, dim, dim);

	for (i = 0; i < dim; i++) {
		tabmahali = tabmahal[i];
		for (j = 0; j < dim; j++)
			tabmahali[j] = j < i ? i * (i + 1) / 2 + j : j * (j + 1) / 2 + i;
	}
}

static float mahalanobis(fv, mv, vcv, dim)
int dim;
double *fv, *mv;
register double *vcv;

/* vcv is upper triangular portion only */

{
	register int j, i;
	register double t, d;
	register int *tabmahali;
	double dv[MAXDIM];

	/* difference vector between sample and class mean */

	for (i = 0; i < dim; i++)
		dv[i] = fv[i] - mv[i];

	/* generalised distance */

	d = 0.0;
	for (i = 0; i < dim; i++) {
		t = 0.0;
		tabmahali = tabmahal[i];
		for (j = 0; j < dim; j++)
			t += dv[j] * vcv[*tabmahali++];
		d += dv[i] * t;
	}
	return(d);
}



/*
 * classify by maximum likelihood.
 * compute mahalanobis distance from class mean vectors,
 * convert to likelihood using square root of determinant
 * of variance - covariance matrix,
 * multiply by prior probability of group to give probability.
 * fill in per-class table of likelihod values for subsequent re-arrangement
 */
static classmahal (fvec,priorp,liklv,nclass,ndim)
double *fvec, *priorp;
struct cllik *liklv;
{
	int i,maxc, liksort();
	double d, lik, maxl;
	struct cllik liks[MAXCLASS], *liklvp;
	liklvp = liks;
	for (i=0; i<nclass; i++) {
		d = mahalanobis(fvec,&classmeans[i][0],&clvcv[i][0],ndim);
		lik = exp(-d/2.0);
#ifdef TRUEMAXLIK
		lik /= clsqrtdet[i];
		lik *= priorp[i];
#endif TRUEMAXLIK
		liklvp->lval = lik;
		liklvp->class = i;
		liklvp++;
		if (i == 0 || lik > maxl) {
			maxl = lik;
			maxc = i;
		}
	}
	/*
	 * sort likelihoods into order and copy first CUTOFF
	 * values into class likelihood structure
	 */
	qsort(liks,nclass,sizeof(struct cllik),liksort);
	liklvp = liks;
	for (i=0; i<CUTOFF; i++) {
		liklv->lval = liklvp->lval;
		liklv->class = liklvp->class;
		liklv++;
		liklvp++;
	}
	return (maxc+1);
}

/*
 * compute distance of feature vector from class mean vector,
 * corrected by per-class feature variances.
 */
static double distvar (fv,mv,cliv,dim)
register double *fv,*mv,*cliv;
{
	register double d;
	double dv;
	register int i,j;

	d = 0.0;
	for (i=0; i<dim; i++) {
		/* difference vector between sample and class mean */
		dv = *fv++ - *mv++;
		/* generalised Euclidean distance */
		d += (dv * dv * (*cliv++));
	}
	return(d);
}




/*
 * classify by distance (variance-corrected), retain in form
 * compatible with log-likelihood.
 */
_classdist (fvec,lpriorp,liklv,nclass,ndim)
double *fvec, *lpriorp;
register struct cllik *liklv;
{
	register int i,maxc;
	int liksort();
	double d, lik, maxl, distvar();
	struct cllik liks[MAXCLASS];
	register struct cllik *liklvp;
	liklvp = liks;
	for (i=0; i<nclass; i++) {
		d = distvar(fvec,&classmeans[i][0],&classinvvars[i][0],ndim);
		lik = -d/2.0;
		lik += clinvvarsqrtdet[i];
		lik += lpriorp[i];	/* log of prior probability */
		liklvp->lval = lik;
		liklvp->class = i;
		liklvp++;
		if (i == 0 || lik > maxl) {
			maxl = lik;
			maxc = i;
		}
	}
	/*
	 * sort likelihoods into order and copy first CUTOFF
	 * values into class likelihood structure
	 */
	qsort(liks,nclass,sizeof(struct cllik),liksort);
	liklvp = liks;
	for (i=0; i<CUTOFF; i++) {
		liklv->lval = liklvp->lval;
		liklv->class = liklvp->class;
		liklv++;
		liklvp++;
	}
	return (maxc+1);
}
//#endif OSK

//#ifdef INTARITH
//#ifdef NOASSEMBLER
/*
 * compute distance of feature vector from class mean vector,
 * corrected by per-class feature variances, IN INTEGER ARITHMETIC.
 *
 * THIS ROUTINE NOW REPLACED BY ASSEMBLER CODED VERSION IN "fastidist.a"
 */
static int idistvar (fv,mv,cliv,dim)
register int *fv,*mv,*cliv;
register int dim;
{
	register int d,dv;
	register int i;

	d = 0;
	for (i=0; i<dim; i++) {
		/* difference between sample and class mean */
		dv = *fv++ - *mv++;
		/* generalised Euclidean distance */
		d += dv * dv * *cliv++;
		if (d < 0) {
//			fprintf(stderr,"overflow\n");
			return(2000000000);	/* near enough 2**31 */
		}
	}
	return(d);
}
//#endif NOASSEMBLER




/*
 * classify by distance (variance-corrected) IN INTEGER ARITHMETIC,
 * computing log-likelihoods
 */
#define LARGENEG -9999999999999.0
_iclassdist (fvec,priorp,liklv,nclass,ndim)
int *fvec;
double *priorp;
struct cllik *liklv;
{
	register int d,i,maxc;
	int liksort();
	double lik, maxl;
	struct cllik liks[MAXCLASS], *liklvp;
	liklvp = liks;
	for (i=0; i<nclass; i++) {
		d = idistvar(fvec,&iclassmeans[i][0],&iclassinvvars[i][0],ndim);
		d = (-d/2);
		lik = (double)d;
		lik /= conversionf[i];
#ifdef TRUEMAXLIK
		lik += clinvvarsqrtdet[i];
		lik += priorp[i];
#endif TRUEMAXLIK
		liklvp->lval = lik;
		liklvp->class = i;
		liklvp++;
	}
	/*
	 * sort likelihoods into order.
	 * copy first CUTOFF values into class likelihood structure
	 */
	for (i=0; i<CUTOFF; i++) {
		liklvp = liks;
		maxl = LARGENEG;
		for (d=0; d<nclass; d++) {
			if (liklvp->lval > maxl) {
				maxl = liklvp->lval;
				maxc = d;
			}
			liklvp++;
		}
		liks[maxc].lval = LARGENEG;
		liklv[i].lval = maxl;
		liklv[i].class = maxc;
	}
	
	return (liklv->class+1);
}
//#endif INTARITH


#if 0
readclassparams(c,nclass,ndim,sf,nsf)
register int *nclass,*ndim,*sf,*nsf;
FILE *c;
{
	register int i,j;
	
/* dimensions and selected feature serial numbers */
	fscanf(c,"%d %d %d",nclass,ndim,nsf);
	for (i=0; i<*nsf; i++)
		fscanf(c,"%d",&sf[i]);
		
#ifndef OSK
/* class means */
	for (i=0; i<*nclass; i++)
		for (j=0; j<*nsf; j++)
			fscanf(c,"%E",&classmeans[i][j]);
/* pooled inverted variance-covariance matrix */
	fscanf(c,"%E",&sqrtdet);
	for (i=0; i<*nsf*(*nsf+1)/2; i++)
		fscanf(c,"%E",&vcv[i]);
/* per-class inverted variance-covariance matrices */
	for (j=0; j<*nclass; j++) {
		fscanf(c,"%E",&clsqrtdet[j]);
		for (i=0; i<*nsf*(*nsf+1)/2; i++)
			fscanf(c,"%E",&clvcv[j][i]);
	}
/* pooled inverted variances */
	for (i=0; i<*nsf; i++) {
		fscanf(c,"%E",&invvar[i]);
	}
	fscanf(c,"%E",&invvarsqrtdet);
#endif OSK

/* per class inverted variances */
	for (i=0; i<*nclass; i++) {
#ifndef OSK
		for (j=0; j<*nsf; j++)
			fscanf(c,"%E",&classinvvars[i][j]);
#endif OSK
		fscanf(c,"%lE",&clinvvarsqrtdet[i]);
	}
	
#ifdef INTARITH
/* class integer means */
	for (i=0; i<*nclass; i++)
		for (j=0; j<*nsf; j++)
			fscanf(c,"%d",&iclassmeans[i][j]);
/* per class integer inverted variances */
	for (i=0; i<*nclass; i++) {
		fscanf(c,"%lE",&conversionf[i]);
		for (j=0; j<*nsf; j++)
			fscanf(c,"%d",&iclassinvvars[i][j]);
	}
#endif INTARITH
#ifndef OSK
	initmahalanobis(*nsf);
#endif OSK
}

#ifndef OSK
liksort(a,b)
register struct cllik *a, *b;
{
	if (a->lval > b->lval)
		return(-1);
	else if (a->lval < b->lval)
		return(1);
	else
	    return(0);
}
#endif OSK

#endif