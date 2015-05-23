/*
 * vcv.c	Jim Piper	23/3/84
 *
 **************************************************************************
 * OS9 VERSION HAS REDUCED FUNCTIONALITY - IN PARTICULAR, NO COVARIANCES
 **************************************************************************
 *
 * Compute variance-covariance matrix from input data vectors produced,
 * e.g., by program "chromnorm".
 * Also means, standard deviations for classes/features, and
 * class-class t statistics.
 *
 * Flags -t, -s, -S, -d turn on four modes:
 *	(1) -t classfile: classifier training, data written to classfile
 *	(2) -s: summary statistics of features and classes
 *	(3) -S:	more statistics.
 *	(4) -d: utilise data from derived objects (class coded +100)
 * Flags -G, -M, -C, -A  set five alternative methods of feature selection:
 *	(1) default - selected by hardwired program
 *	(2) -G n t	Granum's "SEPCOR" with n features, "MAXCOR" t.
 *	(3) -M n	Piper's "MODSEPCOR" with n features.
 *	(4) -C n f1 f2 ... fn Feature read from command line.
 *	(5) -A All features used.
 * Flag -P <filename> - read number of classes, features from <filename>
 */

#include <stdio.h>
//struct _iobuf *fStdIn = stdin;
//struct _iobuf *fStdOut = stdout;
//struct _iobuf *fStdErr = stderr;
#include <wstruct.h>
#include <chromanal.h>
#include <math.h>
#include "vcv.h"


char *split = "   ! !       !  !  ! !  !  ";
int cpos[24] = {0,1,2,3,4,5,6,7,8,9,10,11,22,12,13,14,15,16,17,18,19,20,21,23};



//#ifndef MIZAR
double clvcv[MAXCLASS][MAXDIM*(MAXDIM+1)/2];
double clcp[MAXCLASS][MAXDIM][MAXDIM];
double correl2[MAXDIM][MAXDIM];
double correl3[MAXDIM][MAXDIM];
double vcv[MAXDIM*(MAXDIM+1)/2];
double clsqrtdet[MAXCLASS];
//#endif MIZAR
double correl[MAXDIM][MAXDIM];
double clm[MAXCLASS][MAXDIM],clsd[MAXCLASS][MAXDIM];
double cls[MAXCLASS][MAXDIM];
double clss[MAXCLASS][MAXDIM];


int Vcv(pParams, nChromos, pChromos, pTeachData,bNativeMode,rpt)
	VcvParams *pParams;
	VcvChromoData *pChromos;
	VcvTeachData *pTeachData;
	int bNativeMode;
	FILE *rpt;
{
	int clasi,clasj,ndim,g,h,i,j,k,err,sinverr,chr;
	int nsamp,_class,nclass,clnsamp[MAXCLASS];
	int self[MAXDIM],nself,selfsort();
	int feature_selection_method,TRAIN,STATS,DERIV;
	double maxcor;
	double v[MAXDIM];
	double ss,t,msd,cc,cd;
	double cp[MAXDIM][MAXDIM],s[MAXDIM],m[MAXDIM],psd[MAXDIM];
	double V[MAXDIM][8];

	/*if (pParams->nStat)
		rpt = fopen(pParams->szRptPath, "wt");
	else
		rpt = NULL;*/
	if (rpt) fprintf(rpt, "Training classifier using %d chromosomes\n", nChromos);
	/* mode switches */
	TRAIN = 1;
	STATS = pParams->nStat;
	DERIV = 1;
	feature_selection_method = pParams->nFeatureMethod;
	switch (feature_selection_method)
	{
	case 1:
		nself = pParams->nFeatures;
		maxcor = pParams->dMaxcor;
		break;
	case 2:
		nself = pParams->nFeatures;
		break;
	case 3:
		nself = pParams->nFeatures;
		for (i=0; i<nself; i++)
			self[i] = pParams->pnExplicitFeatArray[i];
	};

	if (bNativeMode)
	{
		ndim = pTeachData->ndim;
		if (ndim > MAXDIM)
			ndim = MAXDIM;
		nclass = pTeachData->nclass;
		if (nclass > MAXCLASS)
			nclass = MAXCLASS;
	}
	else
	{
		/* number of dimensions and classes */
		ndim = _getdim(STATS,rpt);
		if (ndim > MAXDIM)
			return -1;
		nclass = _getclass(STATS,rpt);
		if (nclass > MAXCLASS)
			return -1;
	}

	/* clear arrays */
	nsamp = 0;
	for (i=0; i<ndim; i++)
	{
		s[i] = 0.0;
		for (j=0; j<ndim; j++)
			cp[j][i] = 0.0;
		for (_class=0; _class<nclass; _class++)
		{
			clnsamp[_class] = 0;
			cls[_class][i] = 0.0;
			clss[_class][i] = 0.0;
//#ifndef MIZAR
			for (j=0; j<ndim; j++)
				clcp[_class][j][i] = 0.0;
//#endif MIZAR
		}
	}

	for (chr = 0; chr < nChromos; chr++)
	{
		_class = pChromos[chr].nClass;
		for (i=0; i<ndim; i++)
		{
			v[i] = pChromos[chr].pdValues[i];
		}
		if (DERIV != 0 && _class >= 100)
			_class -= 100;

	/* set up vector sum and matrix of sum of products */
		if (_class >= 0 && _class < nclass)
		{
			nsamp++;
			clnsamp[_class]++;
			for (i=0; i<ndim; i++)
			{
				s[i] += v[i];
				cls[_class][i] += v[i];
				clss[_class][i] += v[i]*v[i];
				for (j=0; j<=i; j++)
				{
					cp[j][i] += v[i]*v[j];
//#ifndef MIZAR
					clcp[_class][j][i] += v[i]*v[j];
//#endif MIZAR
				}
			}
		}
	}

	/* per feature, class-class F values and feature V-value (Granum) */
	/* F is difference of class means/mean of pair of class s.d.s */
	for (i=0; i<ndim; i++)
	{
		/* compute and print per class means, standard deviations */
		classmeansd(STATS,i,nclass,nsamp,clnsamp,clm,clsd,m,s,cls,clss,rpt);
		/* compute and print "f" and "V" values */
		classfandv(STATS,i,nclass,cpos,split,clnsamp,clm,clsd,V,rpt);
	}

	/* compute pooled standard deviation */
	poolsd(STATS,ndim,nsamp,m,cp,psd,rpt);
	if (!(pTeachData->flags&TeachInteger))
	{
		/* compute overall correlations between features */
		correlations(STATS,ndim,cp,nsamp,m,correl,rpt);
		/* and compute mean within-class correlation coefficient */
		withinclasscorrel(STATS,ndim,nclass,nsamp,clcp,clnsamp,clm,correl2,rpt);
		/* and compute weighted between-class-mean correlations coefficients */
		betweenclasscorrel(STATS,ndim,nclass,nsamp,clnsamp,clm,m,correl3,rpt);
	}
	else
	{
		/* and compute weighted between-class-mean correlations coefficients */
		betweenclasscorrel(STATS,ndim,nclass,nsamp,clnsamp,clm,m,correl,rpt);
	}

	/* Print overall and per-Denver class V */
	if (STATS)
		Vtable(ndim,V,rpt);

	switch (feature_selection_method)
	{
	case 1:
		/* Granum's SEPCOR method of feature selection */
		sepcor(self,&i,V,correl,ndim,maxcor,rpt);
		if (i < nself)
		{
//			printf("Number of features selected by SEPCOR %d is less than desired number %d\n",i,nself);
			nself = i;
		}
		break;

	case 2:
		/* Piper's modified sepcor, christened MODSEPCOR */
		modsepcor(self,nself,V,correl,ndim,rpt);
		break;

	case 3:
		/* Specified in command line */
		break;

	case 4:
		/* all features selected */
		nself = ndim;
		for (i=0; i<ndim; i++)
			self[i] = i;
		break;

	case 0:
	default:
		/* Hardwired selection of features */
		selectedfeatures(self,&nself);
		break;
	}

	/*
	 * compute variance-covariance matrices and invert for classifier.
	 * NOTE THAT "self" MUST BE IN ASCENDING ORDER SINCE
	 * "cp" AND "clcp" HAVE ONLY ONE TRIANGLE FILLED !!
	 */
	if (TRAIN)
	{
		qsort(self,nself,sizeof(int),selfsort);
		if (!(pTeachData->flags&TeachInteger))
		{
			makevcv(nself,self,nclass,nsamp,clvcv,clcp,clnsamp,cp,clm,m,vcv);
			trainclass_d(nself,self,nclass,ndim,clvcv,clm,clsd,m,vcv,
				clsqrtdet,psd,pTeachData,rpt);
		}
		else
			trainclass_i(nself,self,nclass,ndim,clm,clsd,m,psd,pTeachData,rpt);
	}

//	if (rpt) fclose(rpt);
	return 0;
}


selfsort(a,b)
int *a,*b;
{
	return(*a - *b);
}
