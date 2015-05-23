/*
 * vcvsub3.c	Jim Piper	17/9/84
 */

/* #define SQUARE		/* see "modsepcor" */
/* #define SQRT			/* see "modsepcor" */



#include <stdio.h>
#include <wstruct.h>
#include <chromanal.h>
#include <math.h>

#define abs(x) (x>=0? x: -x)

struct Vcp {
	float Vval;
	int fnum;
};


/*
 * Erik Granum's SEPCOR feature selection procedure
 */
sepcor(self,nself,V,correl,ndim,maxcor,rpt)
int *self, *nself;
double V[MAXDIM][8];
double correl[MAXDIM][MAXDIM];
double maxcor;
FILE *rpt;
{
	int i,j,k;
	struct Vcp Vcopy[MAXDIM];
	int Vsort();
	for (i=0; i<ndim; i++) {
		Vcopy[i].Vval = V[i][0];
		Vcopy[i].fnum = i;
	}
	qsort(Vcopy,ndim,sizeof(struct Vcp),Vsort);
	if (rpt) {
		fprintf(rpt,"\n\nFeatures sorted by overall V value\n");
		for (i=0; i<ndim; i++)
			fprintf(rpt," %2d",Vcopy[i].fnum);
		fprintf(rpt,"\n\n\n");
		fprintf(rpt,"SEPCOR selected features:\nMAXCOR                Features\n");
	}
	self[0] = Vcopy[0].fnum;
	i = 1;
	for (j=1; j<ndim; j++) {
		for (k=0; k<i; k++) {
			if (abs(correl[self[k]][Vcopy[j].fnum]) >= maxcor)
				goto outsideloopend;
		}
		self[i++] = Vcopy[j].fnum;
outsideloopend:		;
	}
	*nself = i;
	if (rpt) {
		fprintf(rpt,"\n%6.2f:     ",maxcor);    
		for (j=0; j<i; j++)
			fprintf(rpt," %2d",self[j]);
		fprintf(rpt,"\n");
	}
}



/*
 * Modified SEPCOR procedure MODSEPCOR.  Here the choice of next feature to
 * select is based on maximising measure 
 *	ADDSEP = V * (1 - max(correlation between feature and previously
 *				selected feature))
 ********************************************************************
 * #ifdef SQUARE then ADDSEP = V*(1-max*max)
 * #ifdef SQRT then ADDSEP = V*(1-sqrt(|max|))
 ********************************************************************
 */
modsepcor(self,nself,V,correl,ndim,rpt)
int *self;
double V[MAXDIM][8];
double correl[MAXDIM][MAXDIM];
FILE *rpt;
{
	double corr, maxcor, sepcorval, maxsepcorval;
	int i,k,l,nextfeat,maxpos;
	struct Vcp Vcopy[MAXDIM];
	int Vsort();

	for (i=0; i<ndim; i++) {
		Vcopy[i].Vval = V[i][0];
		Vcopy[i].fnum = i;
	}
	qsort(Vcopy,ndim,sizeof(struct Vcp),Vsort);
	if (rpt) {
		fprintf(rpt,"\n\n\n\nfeatures sorted by V value:\n");
		for (i=0; i<ndim; i++)
			fprintf(rpt," %2d",Vcopy[i].fnum);
		fprintf(rpt,"\n\nMODSEPCOR selected features:\n");
	}
	self[0] = Vcopy[0].fnum;
	if (rpt) fprintf(rpt," %2d",self[0]);
	Vcopy[0].fnum = -1;
	for (i=1; i<nself; i++) {
		maxsepcorval = 0.0;
		for (k=1; k<ndim; k++) {
			/* feature already selected ?? */
			if (Vcopy[k].fnum < 0)
				continue;
			/*
			 * compute max correlation with
			 * previously selected feature
			 */
			maxcor = 0.0;
			for (l=0; l<i; l++) {
				corr = correl[self[l]][Vcopy[k].fnum];
				corr = abs(corr);
				if (corr > maxcor)
					maxcor = corr;
			}
			/* measure of how good this is as additional feature */
#ifdef SQRT
			sepcorval = (1.0 - sqrt(abs(maxcor))) * Vcopy[k].Vval;
#else SQRT
#ifdef SQUARE
			sepcorval = (1.0 - maxcor*maxcor) * Vcopy[k].Vval;
#else SQUARE
			sepcorval = (1.0 - maxcor) * Vcopy[k].Vval;
#endif SQUARE
#endif SQRT
			if (sepcorval > maxsepcorval) {
				maxsepcorval = sepcorval;
				nextfeat = k;
			}
		}
		self[i] = Vcopy[nextfeat].fnum;
		Vcopy[nextfeat].fnum = -1;
		if (rpt) fprintf(rpt," %2d",self[i]);
	}
	if (rpt) fprintf(rpt,"\n");
}



Vsort (a,b)
struct Vcp *a,*b;
{
	if (a->Vval > b->Vval)
		return(-1);
	else if (a->Vval == b->Vval)
		return(0);
	else
		return(1);
}


_getclass(print,rpt)
FILE *rpt;
{
	if (print && rpt)
		fprintf(rpt, "\n24 classes (22 autosomes, X = 23, Y = 24)\n");
	return(24);
}


/*
 * specify which features are to be used for classification.
 * Note that as usual the features are numbered from zero.
 * Program "vcv" REQUIRES SELECTED FEATURES TO BE IN ASCENDING ORDER !
 */
selectedfeatures(sfv,nsfv)
int *sfv,*nsfv;
{
	*nsfv = 3;
	sfv[0] = 0;
	sfv[1] = 9;
	sfv[2] = 15;
}

_getdim(print,rpt)
FILE *rpt;
{
	if (print&&rpt) {
		fprintf(rpt,"28 features (note numbering from zero) :\n\n");
		fprintf(rpt,"0: area normalised by median object area\n");
		fprintf(rpt,"1: length normalised by median object length\n");
		fprintf(rpt,"2: normalised density (cell mean = 100)\n");
		fprintf(rpt,"3: area centromeric index\n");
		fprintf(rpt,"4: ratio of mass c.i. to area c.i.\n");
		fprintf(rpt,"5: normalised cvdd\n");
		fprintf(rpt,"6: normalised nssd\n");
		fprintf(rpt,"7: normalised ratio mass to area c.i.\n");
		fprintf(rpt,"8-13: normalised wdds\n");
		fprintf(rpt,"14-19: normalised mwdds\n");
		fprintf(rpt,"20-25: normalised gwdds\n");
		fprintf(rpt,"26: length centromeric index\n");
		fprintf(rpt,"27: hull perimeter\n");
	}
	return(28);
}



