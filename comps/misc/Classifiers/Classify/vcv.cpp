#include "StdAfx.h"
#include <stdio.h>
#include <math.h>
#include "vcv.h"
#include "Ver30.h"


char *split = "   ! !       !  !  ! !  !  ";
int cpos[24] = {0,1,2,3,4,5,6,7,8,9,10,11,22,12,13,14,15,16,17,18,19,20,21,23};

int _cdecl _selfsort(const void *a, const void *b)
{
	return(*(int *)a - *(int *)b);
}

double clvcv[MAXCLASS][MAXDIM*(MAXDIM+1)/2];
double clcp[MAXCLASS][MAXDIM][MAXDIM];
double correl2[MAXDIM][MAXDIM];
double correl3[MAXDIM][MAXDIM];
double vcv[MAXDIM*(MAXDIM+1)/2];
double clsqrtdet[MAXCLASS];
double correl[MAXDIM][MAXDIM];
double clm[MAXCLASS][MAXDIM],clsd[MAXCLASS][MAXDIM];
double cls[MAXCLASS][MAXDIM];
double clss[MAXCLASS][MAXDIM];


int TeachClassifier(VcvParams *pParams, int nChromos, VcvChromoData *pChromos, long *keys, resultdata *presult, FILE *rpt)
{
	int i,j,chr;
	int nsamp,_class,clnsamp[MAXCLASS];
//	int self[MAXDIM],nself;
	CSelFeatsFull Self;
	double maxcor;
	double v[MAXDIM];
	double t;
	double cp[MAXDIM][MAXDIM],s[MAXDIM],m[MAXDIM],psd[MAXDIM];
	double V[MAXDIM][8];

	Self.Alloc(pParams->nDim);
	if (rpt) fprintf(rpt, "Training classifier using %d chromosomes\n", nChromos);
	/* mode switches */
	if (pParams->nDim > MAXDIM)
		pParams->nDim = MAXDIM;
	if (pParams->nClasses > MAXCLASS)
		pParams->nClasses = MAXCLASS;
	/* clear arrays */
	nsamp = 0;
	for (i=0; i<pParams->nDim; i++)
	{
		s[i] = 0.0;
		for (j=0; j<pParams->nDim; j++)
			cp[j][i] = 0.0;
		for (_class=0; _class<pParams->nClasses; _class++)
		{
			clnsamp[_class] = 0;
			cls[_class][i] = 0.0;
			clss[_class][i] = 0.0;
			for (j=0; j<pParams->nDim; j++)
				clcp[_class][j][i] = 0.0;
		}
	}

	for (chr = 0; chr < nChromos; chr++)
	{
		_class = pChromos[chr].nClass;
		for (i=0; i<pParams->nDim; i++)
		{
			v[i] = pChromos[chr].pdValues[i];
		}
//		if (DERIV != 0 && _class >= 100)
//			_class -= 100;
		/* set up vector sum and matrix of sum of products */
		if (_class >= 0 && _class < pParams->nClasses)
		{
			nsamp++;
			clnsamp[_class]++;
			for (i=0; i<pParams->nDim; i++)
			{
				s[i] += v[i];
				cls[_class][i] += v[i];
				clss[_class][i] += v[i]*v[i];
				for (j=0; j<=i; j++)
				{
					cp[j][i] += v[i]*v[j];
					clcp[_class][j][i] += v[i]*v[j];
				}
			}
		}
	}

	/* per feature, class-class F values and feature V-value (Granum) */
	/* F is difference of class means/mean of pair of class s.d.s */
	for (i=0; i<pParams->nDim; i++)
	{
		/* compute and print per class means, standard deviations */
		classmeansd(pParams->nStat,i,pParams->nClasses,nsamp,clnsamp,clm,clsd,m,s,cls,clss,rpt);
		/* compute and print "f" and "V" values */
		classfandv(pParams->nStat,i,pParams->nClasses,cpos,split,clnsamp,clm,clsd,V,rpt);
	}

	/* compute pooled standard deviation */
	poolsd(pParams->nStat,pParams->nDim,nsamp,m,cp,psd,rpt);
	/* compute overall correlations between features */
	correlations(pParams->nStat,pParams->nDim,cp,nsamp,m,correl,rpt);
	/* and compute mean within-class correlation coefficient */
	withinclasscorrel(pParams->nStat,pParams->nDim,pParams->nClasses,nsamp,clcp,clnsamp,clm,correl2,rpt);
	/* and compute weighted between-class-mean correlations coefficients */
	betweenclasscorrel(pParams->nStat,pParams->nDim,pParams->nClasses,nsamp,clnsamp,clm,m,correl3,rpt);

	/* Print overall and per-Denver class V */
	if (pParams->nStat)
		Vtable(pParams->nDim,V,rpt);

	if (IsVersion3x())
	{
		switch (pParams->nFeatureMethod)
		{
		case 1:
			// Granum's SEPCOR method of feature selection
			Self.m_Sel.m_nSelFeats = pParams->nFeatures;
			maxcor = pParams->dMaxcor;
			sepcor(Self.m_Sel.m_pnFeats,&i,V,correl,pParams->nDim,maxcor,rpt);
			if (i < Self.m_Sel.m_nSelFeats)
				Self.m_Sel.m_nSelFeats = i;
			break;
		case 2:
			// Piper's modified sepcor, christened MODSEPCOR
			Self.Alloc(pParams->nDim,pParams->nFeatures);
			modsepcor(Self,V,correl,pParams->nDim,rpt);
			break;
		case 3:
			// Specified in shell.data
			Self.m_Sel.m_nSelFeats = pParams->nFeatures;
			for (i=0; i<pParams->nFeatures; i++)
				Self.m_Sel.m_pnFeats[i] = pParams->pnExplicitFeatArray[i];
			break;
		case 4:
			// all features selected
			Self.m_Sel.m_nSelFeats = pParams->nDim;
			for (i=0; i<pParams->nDim; i++)
				Self.m_Sel.m_pnFeats[i] = i;
			break;
		default:
			/* Hardwired selection of features */
			selectedfeatures(pParams->nDim,keys,Self.m_Sel.m_pnFeats,&Self.m_Sel.m_nSelFeats);
			break;
		}
	}
	else
		selectedfeatures(pParams->nDim,keys,Self.m_Sel.m_pnFeats,&Self.m_Sel.m_nSelFeats);
	// Now calculate result value
	t = 0.;
	for (i = 0; i < Self.m_Sel.m_nSelFeats; i++)
		t += V[Self.m_Sel.m_pnFeats[i]][0];
	if (t > 100.) t = 100.;
	if (t < 0.)	t = 0.;
	presult->AvrV = t;

	/*
	 * compute variance-covariance matrices and invert for classifier.
	 * NOTE THAT "self" MUST BE IN ASCENDING ORDER SINCE
	 * "cp" AND "clcp" HAVE ONLY ONE TRIANGLE FILLED !!
	 */
//	qsort(Self.m_Sel.m_pnFeats,Self.m_Sel.m_nSelFeats,sizeof(int),(COMPARE)_selfsort);
//	makevcv(nself,self,pParams->nClasses,nsamp,clvcv,clcp,clnsamp,cp,clm,m,vcv);
	trainclass(Self,pParams->nClasses,pParams->nDim,clm,clsd,keys,pParams->pszClassFile,rpt);

	return 0;
}


