#include "StdAfx.h"
#include <stdio.h>
#include <math.h>
#include "resource.h"
#include "vcv.h"
#include "ParamsKeys.h"
#include "MessageException.h"

#define abs(x) (x>=0? x: -x)

struct Vcp {
	double Vval;
	int fnum;
};

int Vsort (struct Vcp *a,struct Vcp *b)
{
	if (a->Vval > b->Vval)
		return(-1);
	else if (a->Vval == b->Vval)
		return(0);
	else
		return(1);
}


/*
 * Erik Granum's SEPCOR feature selection procedure
 */
void sepcor(int *self,int *nself,double V[MAXDIM][8],double correl[MAXDIM][MAXDIM],int ndim,
	double maxcor,FILE *rpt)
{
	int i,j,k;
	struct Vcp Vcopy[MAXDIM];
	for (i=0; i<ndim; i++) {
		Vcopy[i].Vval = V[i][0];
		Vcopy[i].fnum = i;
	}
	qsort(Vcopy,ndim,sizeof(struct Vcp),(COMPARE)Vsort);
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
void _modsepcor(int *self,int nself,double V[MAXDIM][8],int nV,double correl[MAXDIM][MAXDIM],int ndim,FILE *rpt)
{
	double corr, maxcor, sepcorval, maxsepcorval;
	int i,k,l,nextfeat;
	struct Vcp Vcopy[MAXDIM];
	for (i=0; i<ndim; i++) {
		Vcopy[i].Vval = V[i][nV];
		Vcopy[i].fnum = i;
	}
	qsort(Vcopy,ndim,sizeof(struct Vcp),(COMPARE)Vsort);
	if (rpt) {
		fprintf(rpt,"\nfeatures sorted by V value");
		if (nV > 0) fprintf(rpt," for Denver class %c", nV+'A'-1);
		fprintf(rpt, ":\n");
		for (i=0; i<ndim; i++)
			fprintf(rpt," %2d",Vcopy[i].fnum);
		fprintf(rpt,"\nMODSEPCOR selected features");
		if (nV > 0) fprintf(rpt," for Denver class %c", nV+'A'-1);
		fprintf(rpt, ":\n");
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

void modsepcor(CSelFeatsFull &Self,double V[MAXDIM][8],double correl[MAXDIM][MAXDIM],int ndim,FILE *rpt)
{
	_modsepcor(Self.m_Sel.m_pnFeats,Self.m_Sel.m_nSelFeats,V,0,correl,ndim,rpt);
	for (int i = 0; i < 7; i++)
		_modsepcor(Self.m_SelDv[i].m_pnFeats,Self.m_SelDv[i].m_nSelFeats,V,i+1,correl,ndim,rpt);
}

/*
 * specify which features are to be used for classification.
 * Note that as usual the features are numbered from zero.
 * Program "vcv" REQUIRES SELECTED FEATURES TO BE IN ASCENDING ORDER !
 */
void selectedfeatures(int nparam, long *keys, int *sfv,int *nsfv)
{
	int area = -1, len = -1, ci = -1;
	for (int i = 0; i < nparam; i++)
	{
		if (keys[i]==KEY_NORMA_AREA)
			area = i;
		else if (keys[i]==KEY_NORMA_LENGHT)
			len = i;
		else if (keys[i]==KEY_LENCI)
			ci = i;
	}
	if (area == -1 || len == -1 || ci == -1)
		throw new CMessageException(IDS_NOT_30_DATABASE);
	*nsfv = 3;
	sfv[0] = area;
	sfv[1] = len;
	sfv[2] = ci;
}




